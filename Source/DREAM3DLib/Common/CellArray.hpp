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

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/VertexArray.hpp"
#include "DREAM3DLib/Common/DynamicListArray.hpp"

/**
 * @brief The MeshLinks class contains arrays of Cells for each Node in the mesh. This allows quick query to the node
 * to determine what Cells the node is a part of.
 */
class CellArray
{
  public:
    
    typedef struct
    {
      size_t verts[3];
    } Cell_t;

    DREAM3D_SHARED_POINTERS(CellArray)
    DREAM3D_STATIC_NEW_MACRO(CellArray)
    DREAM3D_TYPE_MACRO(CellArray)

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    virtual ~CellArray(){ }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void resizeArray(size_t newSize) { m_Array->Resize(newSize); }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    int64_t GetNumberOfTuples() { return m_Array->GetNumberOfTuples(); }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void getVerts(StructArray<Cell_t>::Pointer Cells, size_t edgeId, size_t* verts)
    {
      Cell_t& Cell = *(Cells->GetPointer(edgeId));
      verts[0] = Cell.verts[0];
      verts[1] = Cell.verts[1];
      verts[2] = Cell.verts[2];
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void setVerts(StructArray<Cell_t>::Pointer Cells, size_t edgeId, float* verts)
    {
      Cell_t& Cell = *(Cells->GetPointer(edgeId));
      Cell.verts[0] = verts[0];
      Cell.verts[1] = verts[1];
      Cell.verts[2] = verts[2];
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void findCellsContainingVert(StructArray<VertexArray::Vert_t>::Pointer Verts,
                               StructArray<Cell_t>::Pointer Cells )
    {

      size_t numPts = Verts->GetNumberOfTuples();
      size_t numCells = Cells->GetNumberOfTuples();

      m_CellsContainingVert = DynamicListArray::New();

      // Allocate the basic structures
      m_CellsContainingVert->allocate(numCells);

      size_t cellId;
      unsigned short* linkLoc;

      // fill out lists with number of references to cells
      typedef boost::shared_array<unsigned short> SharedShortArray_t;
      SharedShortArray_t linkLocPtr(new unsigned short[numPts]);
      linkLoc = linkLocPtr.get();

      ::memset(linkLoc, 0, numPts*sizeof(unsigned short));


      size_t pts[3];

      //vtkPolyData *pdata = static_cast<vtkPolyData *>(data);
      // traverse data to determine number of uses of each point
      for (cellId=0; cellId < numCells; cellId++)
      {
        getVerts(Cells, cellId, pts);
        for (size_t j=0; j < 3; j++)
        {
          m_CellsContainingVert->incrementLinkCount(pts[j]);
        }
      }

      // now allocate storage for the links
      m_CellsContainingVert->allocateLinks(numCells);

      for (cellId=0; cellId < numCells; cellId++)
      {
        getVerts(Cells, cellId, pts);
        for (size_t j=0; j < 3; j++)
        {
          m_CellsContainingVert->insertCellReference(pts[j], (linkLoc[pts[j]])++, cellId);
        }
      }

    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void FindCellNeighbors(StructArray<VertexArray::Vert_t>::Pointer Verts, StructArray<Cell_t>::Pointer Cells)
    {

      size_t nCells = Cells->GetNumberOfTuples();

      DynamicListArray::Pointer m_CellNeighbors = DynamicListArray::New();

      m_CellNeighbors->allocate(nCells);

      // Allocate an array of bools that we use each iteration of triangle so that we don't put duplicates into the array
      boost::shared_array<bool> visitedPtr(new bool[nCells]);
      bool* visited = visitedPtr.get();
      ::memset(visitedPtr.get(), 0, nCells);

      // Reuse this vector for each loop. Avoids re-allocating the memory each time through the loop
      QVector<int> loop_neighbors(32, 0);

      // Build up the Cell Adjacency list now that we have the cell links
      for(size_t t = 0; t < nCells; ++t)
      {
        //   std::cout << "Analyzing Cell " << t << std::endl;
        Cell_t& seedCell = *(Cells->GetPointer(t));
        for(size_t v = 0; v < 3; ++v)
        {
          //   std::cout << " vert " << v << std::endl;
          int nCs = m_CellsContainingVert->getNumberOfElements(seedCell.verts[v]);
          int* vertIdxs = m_CellsContainingVert->getElementListPointer(seedCell.verts[v]);

          for(int vt = 0; vt < nCs; ++vt)
          {
            if (vertIdxs[vt] == static_cast<int>(t) ) { continue; } // This is the same triangle as our "source" triangle
            if (visited[vertIdxs[vt]] == true) { continue; } // We already added this triangle so loop again
            //      std::cout << "   Comparing Cell " << vertIdxs[vt] << std::endl;
            Cell_t& vertCell = *(Cells->GetPointer(vertIdxs[vt]));
            int vCount = 0;
            // Loop over all the vertex indices of this triangle and try to match 2 of them to the current loop triangle
            // If there are 2 matches then that triangle is a neighbor of this triangle. if there are more than 2 matches
            // then there is a real problem with the mesh and the program is going to assert.
            // Unrolled the loop to shave about 25% of time off the outer loop.
            int seedCellVert0 = seedCell.verts[0];
            int seedCellVert1 = seedCell.verts[1];
            int seedCellVert2 = seedCell.verts[2];
            int trgtCellVert0 = vertCell.verts[0];
            int trgtCellVert1 = vertCell.verts[1];
            int trgtCellVert2 = vertCell.verts[2];

            if (seedCellVert0 == trgtCellVert0 || seedCellVert0 == trgtCellVert1 || seedCellVert0 == trgtCellVert2  )
            {
              vCount++;
            }
            if (seedCellVert1 == trgtCellVert0 || seedCellVert1 == trgtCellVert1 || seedCellVert1 == trgtCellVert2  )
            {
              vCount++;
            }
            if (seedCellVert2 == trgtCellVert0 || seedCellVert2 == trgtCellVert1 || seedCellVert2 == trgtCellVert2  )
            {
              vCount++;
            }

            BOOST_ASSERT(vCount < 3); // No way 2 edges can share both vertices. Something is VERY wrong at this point

            // So if our vertex match count is 2 and we have not visited the triangle in question then add this triangle index
            // into the list of Cell Indices as neighbors for the source triangle.
            if (vCount == 2)
            {
              //std::cout << "       Neighbor: " << vertIdxs[vt] << std::endl;
              // Use the current count of neighbors as the index
              // into the loop_neighbors vector and place the value of the vertex triangle at that index
              loop_neighbors[m_CellNeighbors->Array[t].ncells] = vertIdxs[vt];
              m_CellNeighbors->Array[t].ncells++;// Increment the count for the next time through
              if (m_CellNeighbors->Array[t].ncells >= loop_neighbors.size())
              {
                loop_neighbors.resize(loop_neighbors.size() + 10);
              }
              visited[vertIdxs[vt]] = true; // Set this triangle as visited so we do NOT add it again
            }
          }
        }
        BOOST_ASSERT(m_CellNeighbors->Array[t].ncells > 2);
        // Reset all the visited triangle indexs back to false (zero)
        for(size_t k = 0;k < m_CellNeighbors->Array[t].ncells; ++k)
        {
          visited[loop_neighbors[k]] = false;
        }
        // Allocate the array storage for the current triangle to hold its Cell list
        m_CellNeighbors->Array[t].cells = new int[m_CellNeighbors->Array[t].ncells];
        // Only copy the first "N" values from the loop_neighbors vector into the storage array
        ::memcpy(m_CellNeighbors->Array[t].cells, &(loop_neighbors[0]), sizeof(int) * m_CellNeighbors->Array[t].ncells);
      }
    }

  protected:
    CellArray();

  private:
    StructArray<Cell_t>::Pointer  m_Array;
    DynamicListArray::Pointer m_CellsContainingVert;
    DynamicListArray::Pointer m_CellNeighbors;


    CellArray(const CellArray&); // Copy Constructor Not Implemented
    void operator=(const CellArray&); // Operator '=' Not Implemented

};



#endif /* _CellArray_H_ */
