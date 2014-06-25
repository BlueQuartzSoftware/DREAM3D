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
#ifndef _CellArray_H_
#define _CellArray_H_


#include <string.h>


#include <boost/shared_array.hpp>

#include <QtCore/QVector>
#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataContainers/VertexArray.h"
#include "DREAM3DLib/DataContainers/DynamicListArray.hpp"

/**
 * @brief The MeshLinks class contains arrays of Cells for each Node in the mesh. This allows quick query to the node
 * to determine what Cells the node is a part of.
 */
class CellArray
{
  public:

    typedef struct
    {
      int32_t verts[4];
    } Cell_t;

    typedef QSet<int32_t> UniqueCellIds_t;
    typedef StructArray<Cell_t> CellContainerType;

    DREAM3D_SHARED_POINTERS(CellArray)
    DREAM3D_STATIC_NEW_MACRO(CellArray)
    DREAM3D_TYPE_MACRO(CellArray)

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    virtual ~CellArray() { }


    DREAM3D_INSTANCE_PROPERTY(Int32DynamicListArray::Pointer, CellsContainingVert)
    DREAM3D_INSTANCE_PROPERTY(Int32DynamicListArray::Pointer, CellNeighbors)
    DREAM3D_INSTANCE_PROPERTY(QString, CellType)

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void resizeArray(size_t newSize) { m_Array->resize(newSize); }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    int64_t getNumberOfTuples() { return m_Array->getNumberOfTuples(); }
    int64_t count() { return m_Array->getNumberOfTuples(); }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    static Pointer CreateArray(size_t numElements, const QString& name, VertexArray* verts, QString cellType)
    {
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }
      BOOST_ASSERT(cellType == DREAM3D::CellType::Quadrilateral || cellType == DREAM3D::CellType::Tetrahedron || cellType == DREAM3D::CellType::Triangle);
      CellArray* d = new CellArray();
      d->resizeArray(numElements);
      d->m_Verts = verts;
      d->m_CellType = cellType;
      Pointer ptr(d);
      return ptr;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void getVerts(size_t edgeId, size_t* verts)
    {
      Cell_t& Cell = *(m_Array->getPointer(edgeId));
      verts[0] = Cell.verts[0];
      verts[1] = Cell.verts[1];
      verts[2] = Cell.verts[2];
      verts[3] = Cell.verts[3];
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void setVerts(size_t edgeId, float* verts)
    {
      Cell_t& Cell = *(m_Array->getPointer(edgeId));
      Cell.verts[0] = verts[0];
      Cell.verts[1] = verts[1];
      Cell.verts[2] = verts[2];
      Cell.verts[3] = verts[3];
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void deleteCellsContainingVert()
    {
      m_CellsContainingVert = Int32DynamicListArray::NullPointer();
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void findCellsContainingVert()
    {

      size_t numPts = m_Verts->getNumberOfTuples();
      size_t numCells = m_Array->getNumberOfTuples();

      m_CellsContainingVert = Int32DynamicListArray::New();

      // Allocate the basic structures
      QVector<int32_t> linkCount(numCells, 0);

      size_t cellId;
      int32_t* linkLoc;

      // fill out lists with number of references to cells
      typedef boost::shared_array<int32_t> SharedInt32Array_t;
      SharedInt32Array_t linkLocPtr(new int32_t[numPts]);
      linkLoc = linkLocPtr.get();

      ::memset(linkLoc, 0, numPts * sizeof(int32_t));


      size_t pts[4];

      //vtkPolyData *pdata = static_cast<vtkPolyData *>(data);
      // traverse data to determine number of uses of each point
      for (cellId = 0; cellId < numCells; cellId++)
      {
        getVerts(cellId, pts);
        for (size_t j = 0; j < 4; j++)
        {
          linkCount[pts[j]]++;
        }
      }

      // now allocate storage for the links
      m_CellsContainingVert->allocateLists(linkCount);

      for (cellId = 0; cellId < numCells; cellId++)
      {
        getVerts(cellId, pts);
        for (size_t j = 0; j < 4; j++)
        {
          m_CellsContainingVert->insertCellReference(pts[j], (linkLoc[pts[j]])++, cellId);
        }
      }

    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void deleteCellNeighbors()
    {
      m_CellNeighbors = Int32DynamicListArray::NullPointer();
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void findCellNeighbors()
    {

      size_t nCells = m_Array->getNumberOfTuples();

      m_CellNeighbors = Int32DynamicListArray::New();

      std::vector<uint16_t> linkCount(nCells, 0);

      // Allocate an array of bools that we use each iteration of triangle so that we don't put duplicates into the array
      boost::shared_array<bool> visitedPtr(new bool[nCells]);
      bool* visited = visitedPtr.get();
      ::memset(visitedPtr.get(), 0, nCells);

      // Reuse this vector for each loop. Avoids re-allocating the memory each time through the loop
      QVector<int> loop_neighbors(32, 0);

      // Build up the Cell Adjacency list now that we have the cell links
      for(size_t t = 0; t < nCells; ++t)
      {
        //   qDebug() << "Analyzing Cell " << t << "\n";
        Cell_t& seedCell = *(m_Array->getPointer(t));
        for(size_t v = 0; v < 4; ++v)
        {
          //   qDebug() << " vert " << v << "\n";
          int nCs = m_CellsContainingVert->getNumberOfElements(seedCell.verts[v]);
          int* vertIdxs = m_CellsContainingVert->getElementListPointer(seedCell.verts[v]);

          for(int vt = 0; vt < nCs; ++vt)
          {
            if (vertIdxs[vt] == static_cast<int>(t) ) { continue; } // This is the same triangle as our "source" triangle
            if (visited[vertIdxs[vt]] == true) { continue; } // We already added this triangle so loop again
            //      qDebug() << "   Comparing Cell " << vertIdxs[vt] << "\n";
            Cell_t& vertCell = *(m_Array->getPointer(vertIdxs[vt]));
            int vCount = 0;
            // Loop over all the vertex indices of this triangle and try to match 2 of them to the current loop triangle
            // If there are 2 matches then that triangle is a neighbor of this triangle. if there are more than 2 matches
            // then there is a real problem with the mesh and the program is going to assert.
            // Unrolled the loop to shave about 25% of time off the outer loop.
            int seedCellVert0 = seedCell.verts[0];
            int seedCellVert1 = seedCell.verts[1];
            int seedCellVert2 = seedCell.verts[2];
            int seedCellVert3 = seedCell.verts[3];
            int trgtCellVert0 = vertCell.verts[0];
            int trgtCellVert1 = vertCell.verts[1];
            int trgtCellVert2 = vertCell.verts[2];
            int trgtCellVert3 = vertCell.verts[3];

            if (seedCellVert0 == trgtCellVert0 || seedCellVert0 == trgtCellVert1 || seedCellVert0 == trgtCellVert2 || seedCellVert0 == trgtCellVert3)
            {
              vCount++;
            }
            if (seedCellVert1 == trgtCellVert0 || seedCellVert1 == trgtCellVert1 || seedCellVert1 == trgtCellVert2 || seedCellVert1 == trgtCellVert3)
            {
              vCount++;
            }
            if (seedCellVert2 == trgtCellVert0 || seedCellVert2 == trgtCellVert1 || seedCellVert2 == trgtCellVert2 || seedCellVert2 == trgtCellVert3)
            {
              vCount++;
            }
            if (seedCellVert3 == trgtCellVert0 || seedCellVert3 == trgtCellVert1 || seedCellVert3 == trgtCellVert2 || seedCellVert3 == trgtCellVert3)
            {
              vCount++;
            }

            BOOST_ASSERT(vCount < 4); // No way 2 edges can share both vertices. Something is VERY wrong at this point

            // So if our vertex match count is 2 and we have not visited the triangle in question then add this triangle index
            // into the list of Cell Indices as neighbors for the source triangle.
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
        for(size_t k = 0; k < linkCount[t]; ++k)
        {
          visited[loop_neighbors[k]] = false;
        }
        // Allocate the array storage for the current triangle to hold its Cell list
        m_CellNeighbors->setElementList(t, linkCount[t], &(loop_neighbors[0]));
      }
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    Cell_t* getPointer(size_t i)
    {
      return m_Array->getPointer(i);
    }

    /**
    * @brief Returns reference to the Face_t at the index i
    * @param i
    * @return
    */
    inline Cell_t& operator[](size_t i)
    {
      return (*m_Array)[i];
    }
    /**
    * @brief Returns reference to the Face_t at the index i
    * @param i
    * @return
    */
    inline Cell_t& getcell(size_t i)
    {
      return (*m_Array)[i];
    }

  protected:
    CellArray() :
      m_CellType(""),
      m_Verts(NULL)
    {
      m_Array = CellContainerType::CreateArray(0, "CellArray_Internal_Use_Only");
    }

  private:
    StructArray<Cell_t>::Pointer  m_Array;
    VertexArray* m_Verts;
    QString m_Name;

    CellArray(const CellArray&); // Copy Constructor Not Implemented
    void operator=(const CellArray&); // Operator '=' Not Implemented

};



#endif /* _CellArray_H_ */

