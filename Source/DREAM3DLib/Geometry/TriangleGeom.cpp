/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "DREAM3DLib/Geometry/TriangleGeom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleGeom::TriangleGeom()
{
  m_GeometryTypeName = DREAM3D::Geometry::TriangleGeometry;
  m_GeometryType = DREAM3D::GeometryType::TriangleGeometry;
  m_XdmfGridType = DREAM3D::XdmfGridType::PolyData;
  m_UnitDimensionality = 2;
  m_SpatialDimensionality = 3;
  m_VertexList = TriangleGeom::CreateSharedVertexList(0);
  m_TriList = TriangleGeom::CreateSharedTriList(0);
  // We don't require edges, so set them to NULL if not available
  m_EdgeList = SharedEdgeList::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleGeom::~TriangleGeom()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleGeom::Pointer TriangleGeom::CreateGeometry(int64_t numTriangles, SharedVertexList::Pointer vertices, const QString& name)
{
  if (name.isEmpty() == true)
  {
    return NullPointer();
  }
  SharedTriList::Pointer triangles = TriangleGeom::CreateSharedTriList(0);
  TriangleGeom* d = new TriangleGeom();
  d->setVertices(vertices);
  d->setTriangles(triangles);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleGeom::Pointer TriangleGeom::CreateGeometry(SharedTriList::Pointer triangles, SharedVertexList::Pointer vertices, const QString& name)
{
  if (name.isEmpty() == true)
  {
    return NullPointer();
  }
  if (vertices.get() == NULL)
  {
    return TriangleGeom::NullPointer();
  }
  if (triangles.get() == NULL)
  {
    return TriangleGeom::NullPointer();
  }
  TriangleGeom* d = new TriangleGeom();
  d->setVertices(vertices);
  d->setTriangles(triangles);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleGeom::Pointer TriangleGeom::CreateGeometry(SharedTriList::Pointer triangles, SharedEdgeList::Pointer edges, SharedVertexList::Pointer vertices, const QString& name)
{
  if (name.isEmpty() == true)
  {
    return NullPointer();
  }
  if (vertices.get() == NULL)
  {
    return TriangleGeom::NullPointer();
  }
  if (edges.get() == NULL)
  {
    return TriangleGeom::NullPointer();
  }
  if (triangles.get() == NULL)
  {
    return TriangleGeom::NullPointer();
  }
  TriangleGeom* d = new TriangleGeom();
  d->setVertices(vertices);
  d->setEdges(edges);
  d->setTriangles(triangles);
  d->setName(name);
  Pointer ptr(d);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::initializeWithZeros()
{
  m_VertexList->initializeWithZeros();
  m_EdgeList->initializeWithZeros();
  m_TriList->initializeWithZeros();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::findTrianglesContainingVert()
{
  size_t numVerts = getNumberOfVertices();
  size_t numTris = getNumberOfTris();

  m_TrianglesContainingVert = UInt16Int64DynamicListArray::New();

  // Allocate the basic structures
  QVector<size_t> linkCount(numTris, 0);
  size_t triId;
  int64_t* linkLoc;

  // Fill out lists with number of references to cells
  typedef boost::shared_array<int64_t> SharedInt64Array_t;
  SharedInt64Array_t linkLocPtr(new int64_t[numVerts]);
  linkLoc = linkLocPtr.get();

  ::memset(linkLoc, 0, numVerts * sizeof(int64_t));

  int64_t verts[3];

  //vtkPolyData *pdata = static_cast<vtkPolyData *>(data);
  // Traverse data to determine number of uses of each point
  for (triId = 0; triId < numTris; triId++)
  {
    getVertsAtTri(triId, verts);
    for (size_t j = 0; j < 3; j++)
    {
      linkCount[verts[j]]++;
    }
  }

  // Now allocate storage for the links
  m_TrianglesContainingVert->allocateLists(linkCount);

  for (triId = 0; triId < numTris; triId++)
  {
    getVertsAtTri(triId, verts);
    for (size_t j = 0; j < 3; j++)
    {
      m_TrianglesContainingVert->insertCellReference(verts[j], (linkLoc[verts[j]])++, triId);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::deleteTrianglesContainingVert()
{
  m_TrianglesContainingVert = UInt16Int64DynamicListArray::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::findTriangleNeighbors()
{
  int64_t numTris = getNumberOfTris();

  m_TriangleNeighbors = UInt16Int64DynamicListArray::New();

  QVector<uint16_t> linkCount(numTris, 0);

  // Allocate an array of bools that we use each iteration of triangle so that we don't put duplicates into the array
  boost::shared_array<bool> visitedPtr(new bool[numTris]);
  bool* visited = visitedPtr.get();
  ::memset(visitedPtr.get(), 0, numTris);

  // Reuse this vector for each loop. Avoids re-allocating the memory each time through the loop
  QVector<int64_t> loop_neighbors(32, 0);

  // Build up the Face Adjacency list now that we have the cell links
  for(int64_t t = 0; t < numTris; ++t)
  {
    //   qDebug() << "Analyzing Face " << t << "\n";
    int64_t* seedFace = getTriPointer(t);
    for(int32_t v = 0; v < 3; ++v)
    {
      //   qDebug() << " vert " << v << "\n";
      int nFs = m_TrianglesContainingVert->getNumberOfElements(seedFace[v]);
      int64_t* vertIdxs = m_TrianglesContainingVert->getElementListPointer(seedFace[v]);

      for(int vt = 0; vt < nFs; ++vt)
      {
        if (vertIdxs[vt] == static_cast<int64_t>(t) ) { continue; } // This is the same triangle as our "source" triangle
        if (visited[vertIdxs[vt]] == true) { continue; } // We already added this triangle so loop again
        //      qDebug() << "   Comparing Face " << vertIdxs[vt] << "\n";
        int64_t* vertFace = getTriPointer(vertIdxs[vt]);
        int vCount = 0;
        // Loop over all the vertex indices of this triangle and try to match 2 of them to the current loop triangle
        // If there are 2 matches then that triangle is a neighbor of this triangle. if there are more than 2 matches
        // then there is a real problem with the mesh and the program is going to assert.
        // Unrolled the loop to shave about 25% of time off the outer loop.
        int64_t seedFaceVert0 = seedFace[0];
        int64_t seedFaceVert1 = seedFace[1];
        int64_t seedFaceVert2 = seedFace[2];
        int64_t trgtFaceVert0 = vertFace[0];
        int64_t trgtFaceVert1 = vertFace[1];
        int64_t trgtFaceVert2 = vertFace[2];

        if (seedFaceVert0 == trgtFaceVert0 || seedFaceVert0 == trgtFaceVert1 || seedFaceVert0 == trgtFaceVert2  )
        {
          vCount++;
        }
        if (seedFaceVert1 == trgtFaceVert0 || seedFaceVert1 == trgtFaceVert1 || seedFaceVert1 == trgtFaceVert2  )
        {
          vCount++;
        }
        if (seedFaceVert2 == trgtFaceVert0 || seedFaceVert2 == trgtFaceVert1 || seedFaceVert2 == trgtFaceVert2  )
        {
          vCount++;
        }

        BOOST_ASSERT(vCount < 3); // No way 2 edges can share both vertices. Something is VERY wrong at this point

        // So if our vertex match count is 2 and we have not visited the triangle in question then add this triangle index
        // into the list of Face Indices as neighbors for the source triangle.
        if (vCount == 2)
        {
          //qDebug() << "       Neighbor: " << vertIdxs[vt] << "\n";
          // Use the current count of neighbors as the index
          // into the loop_neighbors vector and place the value of the vertex triangle at that index
          loop_neighbors[linkCount[t]] = vertIdxs[vt];
          linkCount[t]++;// Increment the count for the next time through
          if (linkCount[t] >= loop_neighbors.size())
          {
            loop_neighbors.resize(loop_neighbors.size() + 10);
          }
          visited[vertIdxs[vt]] = true; // Set this triangle as visited so we do NOT add it again
        }
      }
    }
    BOOST_ASSERT(linkCount[t] > 2);
    // Reset all the visited triangle indexs back to false (zero)
    for(int64_t k = 0; k < linkCount[t]; ++k)
    {
      visited[loop_neighbors[k]] = false;
    }
    // Allocate the array storage for the current triangle to hold its Face list
    m_TriangleNeighbors->setElementList(t, linkCount[t], &(loop_neighbors[0]));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleGeom::deleteTriangleNeighbors()
{
  m_TriangleNeighbors = UInt16Int64DynamicListArray::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleGeom::writeGeometryToHDF5(hid_t parentId, bool writeXdmf)
{
  herr_t err = 0;
  int32_t rank = 0;
  hsize_t dims[2] = {0, 2ULL};
  QVector<size_t> tDims(1, 0);

  if (m_VertexList.get() != NULL)
  {
    tDims[0] = getNumberOfVertices();
    err = m_VertexList->writeH5Data(parentId, tDims);
    if (err < 0)
    {
      return err;
    }
    if(writeXdmf == true)
    {
      QVector<size_t> cDims(1, 1);
      DataArray<int64_t>::Pointer vertsPtr = DataArray<int64_t>::CreateArray(getNumberOfVertices(), cDims, DREAM3D::StringConstants::VertsName);
      int64_t* verts = vertsPtr->getPointer(0);
      for(size_t i = 0; i < vertsPtr->getNumberOfTuples(); i++)
      {
        verts[i] = i;
      }
      tDims[0] = vertsPtr->getNumberOfTuples();
      err = vertsPtr->writeH5Data(parentId, tDims);
    }
  }

//  if (m_EdgeList.get() != NULL)
//  {
//    tDims[0] = getNumberOfEdges();
//    err = m_EdgeList->writeH5Data(parentId, tDims);
//    if (err < 0)
//    {
//      return err;
//    }
//  }

  if (m_TriList.get() != NULL)
  {
    tDims[0] = getNumberOfTris();
    err = m_TriList->writeH5Data(parentId, tDims);
    if (err < 0)
    {
      return err;
    }
  }

  // Next write face neighbors if they exist
  UInt16Int64DynamicListArray::Pointer triNeighbors = getTriangleNeighbors();
  if (NULL != triNeighbors.get())
  {
    size_t total = 0;
    size_t numTris = getNumberOfTris();
    for(size_t v = 0; v < numTris; ++v)
    {
      total += triNeighbors->getNumberOfElements(v);
    }

    size_t totalBytes = numTris * sizeof(uint16_t) + total * sizeof(int32_t);

    // Allocate a flat array to copy the data into
    QVector<uint8_t> buffer(totalBytes, 0);
    uint8_t* bufPtr = &(buffer.front());
    size_t offset = 0;

    for(size_t v = 0; v < numTris; ++v)
    {
      uint16_t ncells = triNeighbors->getNumberOfElements(v);
      int64_t* cells = triNeighbors->getElementListPointer(v);
      ::memcpy(bufPtr + offset, &ncells, sizeof(uint16_t));
      offset += sizeof(uint16_t);
      ::memcpy(bufPtr + offset, cells, ncells * sizeof(int64_t) );
      offset += ncells * sizeof(int64_t);
    }

    rank = 1;
    dims[0] = totalBytes;

    err = QH5Lite::writePointerDataset(parentId, DREAM3D::StringConstants::TriangleNeighbors, rank, dims, bufPtr);
    if (err < 0)
    {
      return err;
    }
  }

  // Last write faces containing verts if they exist
  UInt16Int64DynamicListArray::Pointer trisContainingVert = getTrianglesContainingVert();
  if (NULL != trisContainingVert.get())
  {
    size_t total = 0;
    size_t numVerts = getNumberOfVertices();
    for(size_t v = 0; v < numVerts; ++v)
    {
      total += trisContainingVert->getNumberOfElements(v);
    }

    size_t totalBytes = numVerts * sizeof(uint16_t) + total * sizeof(int32_t);

    // Allocate a flat array to copy the data into
    QVector<uint8_t> buffer(totalBytes, 0);
    uint8_t* bufPtr = &(buffer.front());
    size_t offset = 0;

    for(size_t v = 0; v < numVerts; ++v)
    {
      uint16_t ncells = trisContainingVert->getNumberOfElements(v);
      int64_t* cells = trisContainingVert->getElementListPointer(v);
      ::memcpy(bufPtr + offset, &ncells, sizeof(uint16_t));
      offset += sizeof(uint16_t);
      ::memcpy(bufPtr + offset, cells, ncells * sizeof(int64_t) );
      offset += ncells * sizeof(int64_t);
    }

    rank = 1;
    dims[0] = totalBytes;

    err = QH5Lite::writePointerDataset(parentId, DREAM3D::StringConstants::TrianglesContainingVert, rank, dims, bufPtr);
    if (err < 0)
    {
      return err;
    }
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleGeom::writeXdmf(QTextStream& out, QString dcName, QString hdfFileName)
{
  herr_t err = 0;

  // Always start the grid
  out << "  <!-- *************** START OF " << dcName << " *************** -->" << "\n";
  out << "  <Grid Name=\"" << dcName << "\" GridType=\"Uniform\">" << "\n";

  out << "    <Topology TopologyType=\"Triangle\" NumberOfElements=\"" << getNumberOfTris() << "\">" << "\n";
  out << "      <DataItem Format=\"HDF\" NumberType=\"Int\" Dimensions=\"" << getNumberOfTris() << " 3\">" << "\n";
  out << "        " << hdfFileName << ":/DataContainers/" << dcName << "/" << DREAM3D::Geometry::Geometry << "/" << DREAM3D::Geometry::SharedTriList << "\n";
  out << "      </DataItem>" << "\n";
  out << "    </Topology>" << "\n";

  if (m_VertexList.get() == NULL)
  {
    out << "<!-- ********************* GEOMETRY ERROR ****************************************\n";
    out << "The Geometry with name '" << getName() << "' in DataContainer '" << dcName <<  "' \n";
    out << "did not have any vertices assigned.\n";
    out << "The Geometry types will be missing from the Xdmf which will cause issues when\n";
    out << "trying to load the file\n";
    out << " ********************************************************************************** -->\n";
  }
  else
  {
    out << "    <Geometry Type=\"XYZ\">" << "\n";
    out << "      <DataItem Format=\"HDF\"  Dimensions=\"" << getNumberOfVertices() << " 3\" NumberType=\"Float\" Precision=\"4\">" << "\n";
    out << "        " << hdfFileName << ":/DataContainers/" << dcName << "/" << DREAM3D::Geometry::Geometry << "/" << DREAM3D::Geometry::SharedVertexList << "\n";
    out << "      </DataItem>" << "\n";
    out << "    </Geometry>" << "\n";
    out << "" << "\n";
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleGeom::readGeometryFromHDF5(hid_t parentId, bool preflight)
{
  herr_t err = 0;
  QVector<hsize_t> dims;
  H5T_class_t type_class;
  size_t type_size;
  SharedVertexList::Pointer vertices = SharedVertexList::NullPointer();
  SharedEdgeList::Pointer edges = SharedEdgeList::NullPointer();
  SharedTriList::Pointer triangles = SharedTriList::NullPointer();
  vertices = IGeometry::ReadMeshFromHDF5<SharedVertexList>(DREAM3D::Geometry::SharedVertexList, parentId, preflight);
  edges = IGeometry::ReadMeshFromHDF5<SharedEdgeList>(DREAM3D::Geometry::SharedEdgeList, parentId, preflight);
  triangles = IGeometry::ReadMeshFromHDF5<SharedTriList>(DREAM3D::Geometry::SharedTriList, parentId, preflight);
  if (true == preflight)
  {
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::TriangleNeighbors, dims, type_class, type_size);
    if(err >= 0)
    {
      UInt16Int64DynamicListArray::Pointer triNeighbors = UInt16Int64DynamicListArray::New();
      setTriangleNeighbors(triNeighbors);
    }
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::TrianglesContainingVert, dims, type_class, type_size);
    if(err >= 0)
    {
      UInt16Int64DynamicListArray::Pointer trisContainingVert = UInt16Int64DynamicListArray::New();
      setTrianglesContainingVert(trisContainingVert);
    }
    setVertices(vertices);
    setEdges(edges);
    setTriangles(triangles);
  }
  else
  {
    size_t numTris = triangles->getNumberOfTuples();
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::TriangleNeighbors, dims, type_class, type_size);
    if (err >= 0)
    {
      // Read the triNeighbors array into the buffer
      std::vector<uint8_t> buffer;
      err = QH5Lite::readVectorDataset(parentId, DREAM3D::StringConstants::TriangleNeighbors, buffer);
      if(err < 0)
      {
        return err;
      }
      UInt16Int64DynamicListArray::Pointer triNeighbors = UInt16Int64DynamicListArray::New();
      triNeighbors->deserializeLinks(buffer, numTris);
      setTriangleNeighbors(triNeighbors);
    }
    err = QH5Lite::getDatasetInfo(parentId, DREAM3D::StringConstants::TrianglesContainingVert, dims, type_class, type_size);
    if (err >= 0)
    {
      // Read the trisContainingVert array into the buffer
      std::vector<uint8_t> buffer;
      err = QH5Lite::readVectorDataset(parentId, DREAM3D::StringConstants::TrianglesContainingVert, buffer);
      if(err < 0)
      {
        return err;
      }
      UInt16Int64DynamicListArray::Pointer trisContainingVert = UInt16Int64DynamicListArray::New();
      trisContainingVert->deserializeLinks(buffer, numTris);
      setTrianglesContainingVert(trisContainingVert);
    }
  }

  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

// Shared ops includes
#define GEOM_CLASS_NAME TriangleGeom
#include "DREAM3DLib/Geometry/SharedGeometryOps.cpp"
#include "DREAM3DLib/Geometry/SharedVertexOps.cpp"
#include "DREAM3DLib/Geometry/SharedEdgeOps.cpp"
#include "DREAM3DLib/Geometry/SharedTriOps.cpp"
