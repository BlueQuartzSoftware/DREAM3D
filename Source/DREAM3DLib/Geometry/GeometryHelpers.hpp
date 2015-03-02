#ifndef _GeometryHelpers_H_
#define _GeometryHelpers_H_

#include <boost/shared_ptr.hpp>

#include <math.h>

#include "H5Support/QH5Utilities.h"
#include "H5Support/HDF5ScopedFileSentinel.h"
#include "H5Support/QH5Lite.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Geometry/IGeometry.h"

/**
* @brief This file contains a namespace with classes for manipulating IGeometry objects
*/

namespace GeometryHelpers
{
  /**
   * @brief The GeomIO class
   */
  class GeomIO
  {
    public:
      GeomIO() {}
      virtual ~GeomIO() {}

      /**
       * @brief ReadMeshFromHDF5
       * @param listName
       * @param parentId
       * @param preflight
       * @return
       */
      template<typename ListType>
      static typename ListType::Pointer ReadMeshFromHDF5(const QString& listName, hid_t parentId, bool preflight)
      {
        herr_t err = 0;
        QVector<hsize_t> dims;
        H5T_class_t type_class;
        size_t type_size;
        typename ListType::Pointer mesh = ListType::CreateArray(0, listName);
        if (true == preflight)
        {
          err = QH5Lite::getDatasetInfo(parentId, listName, dims, type_class, type_size);
          if (err < 0)
          {
            mesh = ListType::NullPointer();
          }
        }
        else
        {
          err = mesh->readH5Data(parentId);
          if (err < 0)
          {
            mesh = ListType::NullPointer();
          }
        }

        return mesh;
      }

      /**
       * @brief ReadMetaDataFromHDF5
       * @param parentId
       * @param geometry
       * @return
       */
      static int ReadMetaDataFromHDF5(hid_t parentId, IGeometry::Pointer geometry)
      {
        herr_t err = 0;
        unsigned int spatialDims = 0;
        QString geomName = "";
        err = QH5Lite::readScalarAttribute(parentId, DREAM3D::Geometry::Geometry, DREAM3D::Geometry::SpatialDimensionality, spatialDims);
        if(err < 0)
        {
          return err;
        }
        err = QH5Lite::readStringAttribute(parentId, DREAM3D::Geometry::Geometry, DREAM3D::Geometry::GeometryName, geomName);
        if(err < 0)
        {
          return err;
        }
        geometry->setSpatialDimensionality(spatialDims);
        geometry->setName(geomName);

        return 1;
      }

      /**
       * @brief WriteListToHDF5
       * @param parentId
       * @param list
       * @return
       */
      static int WriteListToHDF5(hid_t parentId, IDataArray::Pointer list)
      {
        herr_t err = 0;
        QVector<size_t> tDims(1, list->getNumberOfTuples());
        err = list->writeH5Data(parentId, tDims);
        return err;
      }

      /**
       * @brief WriteDynamicListToHDF5
       * @param parentId
       * @param dynamicList
       * @param numCells
       * @return
       */
      template<typename T, typename K>
      static int WriteDynamicListToHDF5(hid_t parentId, typename DynamicListArray<T, K>::Pointer dynamicList, size_t numCells)
      {
        herr_t err = 0;
        int32_t rank = 0;
        hsize_t dims[2] = {0, 2ULL};
        size_t total = 0;
        for(size_t v = 0; v < numCells; ++v)
        {
          total += dynamicList->getNumberOfElements(v);
        }

        size_t totalBytes = numCells * sizeof(K) + total * sizeof(T);

        // Allocate a flat array to copy the data into
        QVector<uint8_t> buffer(totalBytes, 0);
        uint8_t* bufPtr = &(buffer.front());
        size_t offset = 0;

        for(size_t v = 0; v < numCells; ++v)
        {
          T ncells = dynamicList->getNumberOfElements(v);
          K* cells = dynamicList->getElementListPointer(v);
          ::memcpy(bufPtr + offset, &ncells, sizeof(T));
          offset += sizeof(T);
          ::memcpy(bufPtr + offset, cells, ncells * sizeof(K) );
          offset += ncells * sizeof(K);
        }

        rank = 1;
        dims[0] = totalBytes;

        err = QH5Lite::writePointerDataset(parentId, DREAM3D::StringConstants::EdgeNeighbors, rank, dims, bufPtr);
        return err;
      }
  };

  /**
   * @brief The Connectivity class
   */
  class Connectivity
  {
    public:
      Connectivity() {}
      virtual ~Connectivity() {}

      /**
       * @brief FindCellsContainingVert
       * @param cellList
       * @param dynamicList
       * @param numVerts
       */
      template<typename T, typename K>
      static void FindCellsContainingVert(typename DataArray<K>::Pointer cellList, typename DynamicListArray<T, K>::Pointer dynamicList, size_t numVerts)
      {
        size_t numCells = cellList->getNumberOfTuples();
        size_t numVertsPerCell = cellList->getNumberOfComponents();

        // Allocate the basic structures
        QVector<size_t> linkCount(numCells, 0);
        size_t cellId = 0;
        int64_t* linkLoc;

        // Fill out lists with number of references to cells
        typedef boost::shared_array<K> SharedArray_t;
        SharedArray_t linkLocPtr(new K[numVerts]);
        linkLoc = linkLocPtr.get();

        ::memset(linkLoc, 0, numVerts * sizeof(K));

        K* verts;

        //vtkPolyData *pdata = static_cast<vtkPolyData *>(data);
        // Traverse data to determine number of uses of each point
        for (cellId = 0; cellId < numCells; cellId++)
        {
          verts = cellList->getTuplePointer(cellId);
          for (size_t j = 0; j < numVertsPerCell; j++)
          {
            linkCount[verts[j]]++;
          }
        }

        // Now allocate storage for the links
        dynamicList->allocateLists(linkCount);

        for (cellId = 0; cellId < numCells; cellId++)
        {
          verts = cellList->getTuplePointer(cellId);
          for (size_t j = 0; j < numVertsPerCell; j++)
          {
            dynamicList->insertCellReference(verts[j], (linkLoc[verts[j]])++, cellId);
          }
        }
      }

      /**
       * @brief FindCellNeighbors
       * @param cellList
       * @param cellsContainingVerts
       * @param dynamicList
       */
      template<typename T, typename K>
      static int FindCellNeighbors(typename DataArray<K>::Pointer cellList, typename DynamicListArray<T, K>::Pointer cellsContainingVerts,
                                   typename DynamicListArray<T, K>::Pointer dynamicList)
      {
        size_t numCells = cellList->getNumberOfTuples();
        size_t numVertsPerCell = cellList->getNumberOfComponents();
        size_t numSharedVerts = 0;
        QVector<T> linkCount(numCells, 0);
        int err = 0;

        switch(numVertsPerCell)
        {
        case 2: // edges
        {
          numSharedVerts = 1;
          break;
        }
        case 3: // triangles
        {
          numSharedVerts = 2;
          break;
        }
        case 4: // quadrilaterals
        {
          numSharedVerts = 2;
          break;
        }
        default:
          numSharedVerts = 0;
          break;
        }

        // Allocate an array of bools that we use each iteration so that we don't put duplicates into the array
        boost::shared_array<bool> visitedPtr(new bool[numCells]);
        bool* visited = visitedPtr.get();
        ::memset(visitedPtr.get(), 0, numCells);

        // Reuse this vector for each loop. Avoids re-allocating the memory each time through the loop
        QVector<K> loop_neighbors(32, 0);

        // Build up the cell adjacency list now that we have the cell links
        for (size_t t = 0; t < numCells; ++t)
        {
          //   qDebug() << "Analyzing Cell " << t << "\n";
          K* seedCell = cellList->getTuplePointer(t);
          for (size_t v = 0; v < numVertsPerCell; ++v)
          {
            //   qDebug() << " vert " << v << "\n";
            T nEs = cellsContainingVerts->getNumberOfElements(seedCell[v]);
            K* vertIdxs = cellsContainingVerts->getElementListPointer(seedCell[v]);

            for (T vt = 0; vt < nEs; ++vt)
            {
              if (vertIdxs[vt] == static_cast<K>(t) ) { continue; } // This is the same cell as our "source"
              if (visited[vertIdxs[vt]] == true) { continue; } // We already added this cell so loop again
              //      qDebug() << "   Comparing Cell " << vertIdxs[vt] << "\n";
              K* vertCell = cellList->getTuplePointer(vertIdxs[vt]);
              size_t vCount = 0;
              // Loop over all the vertex indices of this cell and try to match 1 of them to the current loop edge
              // If there is 1 match then that cell is a neighbor of the source. If there are more than numVertsPerCell
              // matches then there is a real problem with the mesh and the program is going to assert.
              for (size_t i = 0; i < numVertsPerCell; i++)
              {
                for (size_t j = 0; j < numVertsPerCell; j++)
                {
                  if (seedCell[i] == vertCell[i])
                  {
                    vCount++;
                  }
                }
              }

              if (vCount < numVertsPerCell) // No way 2 cells can share all vertices. Something is VERY wrong at this point
              {
                return -1;
              }

              // So if our vertex match count is numSharedVerts and we have not visited the cell in question then add this cell index
              // into the list of vertex indices as neighbors for the source cell.
              if (vCount == numSharedVerts)
              {
                //qDebug() << "       Neighbor: " << vertIdxs[vt] << "\n";
                // Use the current count of neighbors as the index
                // into the loop_neighbors vector and place the value of the vertex cell at that index
                loop_neighbors[linkCount[t]] = vertIdxs[vt];
                linkCount[t]++; // Increment the count for the next time through
                if (linkCount[t] >= loop_neighbors.size())
                {
                  loop_neighbors.resize(loop_neighbors.size() + 10);
                }
                visited[vertIdxs[vt]] = true; // Set this cell as visited so we do NOT add it again
              }
            }
          }
          // Reset all the visited cell indexs back to false (zero)
          for(int64_t k = 0; k < linkCount[t]; ++k)
          {
            visited[loop_neighbors[k]] = false;
          }
          // Allocate the array storage for the current edge to hold its vertex list
          dynamicList->setElementList(t, linkCount[t], &(loop_neighbors[0]));
        }

        return err;
      }
  };

  /**
   * @brief The Topology class
   */
  class Topology
  {
    public:
      Topology() {}
      virtual ~Topology() {}

      /**
       * @brief FindCellCentroids
       * @param cellList
       * @param vertices
       * @param elementCentroids
       */
      template<typename T>
      static void FindCellCentroids(typename DataArray<T>::Pointer cellList, FloatArrayType::Pointer vertices, FloatArrayType::Pointer elementCentroids)
      {
        size_t numCells = cellList->getNumberOfTuples();
        size_t numVertsPerCell = cellList->getNumberOfComponents();
        size_t numDims = 3;
        float* cellCntrds = elementCentroids->getPointer(0);
        float* vertex = vertices->getPointer(0);

        for (size_t i = 0; i < numDims; i++)
        {
          for (size_t j = 0; j < numCells; j++)
          {
            T* Cell = cellList->getTuplePointer(j);
            float vertPos = 0.0;
            for (size_t k = 0; k < numVertsPerCell; k++)
            {
              vertPos += vertex[3*Cell[k]+i];
            }
            vertPos /= static_cast<float>(numVertsPerCell);
            cellCntrds[numDims*j+i] = vertPos;
          }
        }
      }
  };

  /**
   * @brief The Generic class
   */
  class Generic
  {
    public:
      Generic() {}
      virtual ~Generic() {}

      /**
       * @brief AverageVertexArrayValues
       * @param cellList
       * @param inVertexArray
       * @param outCellArray
       */
      template<typename T, typename K>
      static void AverageVertexArrayValues(typename DataArray<T>::Pointer cellList, typename DataArray<K>::Pointer inVertexArray, DataArray<float>::Pointer outCellArray)
      {
        BOOST_ASSERT(outCellArray->getComponentDimensions() == inVertexArray->getComponentDimensions());
        BOOST_ASSERT(cellList->getNumberOfTuples() == outCellArray->getNumberOfTuples());

        K* vertArray = inVertexArray->getPointer(0);
        float* cellArray = outCellArray->getPointer(0);

        size_t numCells = outCellArray->getNumberOfTuples();
        size_t numDims = inVertexArray->getNumberOfComponents();
        size_t numVertsPerCell = cellList->getNumberOfComponents();

        for (size_t i=0;i<numDims;i++)
        {
          for (size_t j=0;j<numCells;j++)
          {
            T* Cell = cellList->getTuplePointer(j);
            float vertValue = 0.0;
            for (size_t k=0;k<numVertsPerCell;k++)
            {
              vertValue += vertArray[numDims*Cell[k]+i];
            }
            vertValue /= static_cast<float>(numVertsPerCell);
            cellArray[numDims*j+i] = vertValue;
          }
        }
      }

      /**
       * @brief WeightedAverageVertexArrayValues
       * @param cellList
       * @param vertices
       * @param centroids
       * @param inVertexArray
       * @param outCellArray
       */
      template<typename T, typename K>
      static void WeightedAverageVertexArrayValues(typename DataArray<T>::Pointer cellList, DataArray<float>::Pointer vertices,
                                                   DataArray<float>::Pointer centroids, typename DataArray<K>::Pointer inVertexArray, DataArray<float>::Pointer outCellArray)
      {
        BOOST_ASSERT(outCellArray->getNumberOfTuples() == cellList->getNumberOfTuples());
        BOOST_ASSERT(outCellArray->getComponentDimensions() == inVertexArray->getComponentDimensions());

        K* vertArray = inVertexArray->getPointer(0);
        float* cellArray = outCellArray->getPointer(0);
        float* elementCentroids = centroids->getPointer(0);
        float* vertex = vertices->getPointer(0);

        size_t numCells = outCellArray->getNumberOfTuples();
        size_t cDims = inVertexArray->getNumberOfComponents();
        size_t numVertsPerCell = cellList->getNumberOfComponents();
        size_t numDims = 3;

        // Vector to hold vertex-centroid distances, 4 per cell
        std::vector<float> vertCentDist(numCells*numVertsPerCell);

        for (size_t i=0;i<numCells;i++)
        {
          T* Cell = cellList->getTuplePointer(i);
          for (size_t j=0;j<numVertsPerCell;j++)
          {
            for (size_t k=0;k<numDims;k++)
            {
              vertCentDist[numVertsPerCell*i+j] += (vertex[numDims*Cell[j]+k] - elementCentroids[numDims*i+k]) * (vertex[numDims*Cell[j]+k] - elementCentroids[numDims*i+k]);
            }
            vertCentDist[numVertsPerCell*i+j] = sqrt(vertCentDist[numVertsPerCell*i+j]);
          }
        }

        for (size_t i=0;i<cDims;i++)
        {
          for (size_t j=0;j<numCells;j++)
          {
            T* Cell = cellList->getTuplePointer(j);
            float vertValue = 0.0;
            float sumDist = 0.0;
            for (size_t k=0;k<numVertsPerCell;k++)
            {
              vertValue += vertArray[cDims*Cell[k]+i] * vertCentDist[numVertsPerCell*j+k];
              sumDist += vertCentDist[numVertsPerCell*j+k];
            }
            vertValue /= static_cast<float>(sumDist);
            cellArray[cDims*j+i] = vertValue;
          }
        }
      }
  };

}

#endif /* _GeometryHelpers_H_ */
