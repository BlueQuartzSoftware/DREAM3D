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
#ifndef _MeshEdgeNeighbors_hpp_H_
#define _MeshEdgeNeighbors_hpp_H_

#include <string>
#include <vector>

#include <boost/shared_array.hpp>

//-- DREAM3D Includes
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/MeshStructs.h"
#include "DREAM3DLib/SurfaceMeshingFilters/MeshLinks.hpp"

/**
 * @brief The MeshEdgeNeighbors class contains arrays of Faces for each Node in the mesh. This allows quick query to the node
 * to determine what Cells the node is a part of.
 */
class MeshEdgeNeighbors
{
  public:

    DREAM3D_SHARED_POINTERS(MeshEdgeNeighbors)
    DREAM3D_STATIC_NEW_MACRO(MeshEdgeNeighbors)
    DREAM3D_TYPE_MACRO(MeshEdgeNeighbors)

    class NeighborList {
      public:
      unsigned short ncells;
      int* cells;
    };



    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    virtual ~MeshEdgeNeighbors()
    {
      if ( this->Array == NULL )
      {
        return;
      }

      for (size_t i=0; i < this->Size; i++)
      {
        if ( this->Array[i].cells != NULL )
        {
          delete [] this->Array[i].cells;
        }
      }

      delete [] this->Array;
    }

    //----------------------------------------------------------------------------
    inline void insertCellReference(size_t ptId,
                                    unsigned short pos,
                                    size_t cellId)
    {
      this->Array[ptId].cells[pos] = cellId;
    }

    // Description:
    // Get a link structure given a point id.
    NeighborList& getNeighborList(size_t ptId) {
      return this->Array[ptId];
    }

    // Description:
    // Get the number of cells using the point specified by ptId.
    unsigned short getNumberOfFaces(size_t ptId) {
      return this->Array[ptId].ncells;
    }

    // Description:
    // Return a list of cell ids using the point.
    int* getNeighborListPointer(size_t ptId) {
      return this->Array[ptId].cells;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void generateNeighborLists(DREAM3D::Mesh::VertListPointer_t nodes,
                               DREAM3D::Mesh::EdgeListPointer_t edges,
                               MeshLinks::Pointer cellLinks)
    {

      size_t nEdges = edges->GetNumberOfTuples();

      allocate(edges->GetNumberOfTuples());

      // Allocate an array of bools that we use each iteration of triangle so that we don't put duplicates into the array
      boost::shared_array<bool> visitedPtr(new bool[nEdges]);
      bool* visited = visitedPtr.get();
      ::memset(visitedPtr.get(), 0, nEdges);

      // Reuse this vector for each loop. Avoids re-allocating the memory each time through the loop
      std::vector<int> loop_neighbors(32, 0);

      // Build up the Face Adjacency list now that we have the cell links
      for(size_t t = 0; t < nEdges; ++t)
      {
        //   std::cout << "Analyzing Face " << t << std::endl;
        DREAM3D::Mesh::Edge_t& seedEdge = *(edges->GetPointer(t));
        for(size_t v = 0; v < 3; ++v)
        {
          //   std::cout << " vert " << v << std::endl;
          int nTris = cellLinks->getNumberOfFaces(seedEdge.verts[v]);
          int* vertIdxs = cellLinks->getFaceListPointer(seedEdge.verts[v]);

          for(int vt = 0; vt < nTris; ++vt)
          {
            if (vertIdxs[vt] == static_cast<int>(t) ) { continue; } // This is the same triangle as our "source" triangle
            if (visited[vertIdxs[vt]] == true) { continue; } // We already added this triangle so loop again
            //      std::cout << "   Comparing Face " << vertIdxs[vt] << std::endl;
            DREAM3D::Mesh::Face_t& vertTri = *(faces->GetPointer(vertIdxs[vt]));
            int vCount = 0;
            // Loop over all the vertex indices of this triangle and try to match 2 of them to the current loop triangle
            // If there are 2 matches then that triangle is a neighbor of this triangle. if there are more than 2 matches
            // then there is a real problem with the mesh and the program is going to assert.
            // Unrolled the loop to shave about 25% of time off the outer loop.
            int seedTriVert0 = seedFace.verts[0];
            int seedTriVert1 = seedFace.verts[1];
            int seedTriVert2 = seedFace.verts[2];
            int trgtTriVert0 = vertTri.verts[0];
            int trgtTriVert1 = vertTri.verts[1];
            int trgtTriVert2 = vertTri.verts[2];

            if (seedTriVert0 == trgtTriVert0 || seedTriVert0 == trgtTriVert1 || seedTriVert0 == trgtTriVert2  )
            {
              vCount++;
            }
            if (seedTriVert1 == trgtTriVert0 || seedTriVert1 == trgtTriVert1 || seedTriVert1 == trgtTriVert2  )
            {
              vCount++;
            }
            if (seedTriVert2 == trgtTriVert0 || seedTriVert2 == trgtTriVert1 || seedTriVert2 == trgtTriVert2  )
            {
              vCount++;
            }

            BOOST_ASSERT(vCount < 3); // No way 2 faces can share all 3 vertices. Something is VERY wrong at this point

            // So if our vertex match count is 2 and we have not visited the triangle in question then add this triangle index
            // into the list of Face Indices as neighbors for the source triangle.
            if (vCount == 2)
            {
              //std::cout << "       Neighbor: " << vertIdxs[vt] << std::endl;
              // Use the current count of neighbors as the index
              // into the loop_neighbors vector and place the value of the vertex triangle at that index
              loop_neighbors[this->Array[t].ncells] = vertIdxs[vt];
              this->Array[t].ncells++;// Increment the count for the next time through
              if (this->Array[t].ncells >= loop_neighbors.size())
              {
                loop_neighbors.resize(loop_neighbors.size() + 10);
              }
              visited[vertIdxs[vt]] = true; // Set this triangle as visited so we do NOT add it again
            }
          }
        }
        BOOST_ASSERT(this->Array[t].ncells > 2);
        // Reset all the visited triangle indexs back to false (zero)
        for(size_t k = 0;k < this->Array[t].ncells; ++k)
        {
          visited[loop_neighbors[k]] = false;
        }
        // Allocate the array storage for the current triangle to hold its Face list
        this->Array[t].cells = new int[this->Array[t].ncells];
        // Only copy the first "N" values from the loop_neighbors vector into the storage array
        ::memcpy(this->Array[t].cells, &(loop_neighbors[0]), sizeof(int) * this->Array[t].ncells);
      }
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void deserializeLinks(std::vector<uint8_t> &buffer, size_t nFaces)
    {
      size_t offset = 0;
      allocate(nFaces); // Allocate all the links with 0 and NULL;
      uint8_t* bufPtr = &(buffer.front());

      // Walk the array and allocate all the array links to Zero and NULL
      uint16_t* ncells = NULL;
      //int32_t* cells = NULL;
      for(size_t i = 0; i < nFaces; ++i)
      {
        ncells = reinterpret_cast<uint16_t*>(bufPtr + offset);
        this->Array[i].ncells = *ncells; // Set the number of cells in this link
        offset += 2;
        this->Array[i].cells = new int32_t[(*ncells)]; // Allocate a new chunk of memory to store the list
        ::memcpy(this->Array[i].cells, bufPtr + offset, (*ncells)*sizeof(int32_t) ); // Copy from teh buffer into the new list memory
        offset += (*ncells) * sizeof(int32_t); // Increment the offset
      }
    }


  protected:
    MeshEdgeNeighbors():Array(NULL),Size(0) {}

    //----------------------------------------------------------------------------
    // This will allocate memory to hold all the NeighborList structures where each
    // structure is initialized to Zero Entries and a NULL Pointer
    void allocate(size_t sz, size_t ext=1000)
    {
      static MeshEdgeNeighbors::NeighborList linkInit = {0,NULL};

      // This makes sure we deallocate any lists that have been created
      for (size_t i=0; i<this->Size; i++)
      {
        if ( this->Array[i].cells != NULL )
        {
          delete [] this->Array[i].cells;
        }
      }
      // Now delete all the "NeighborLists" structures
      if ( this->Array != NULL )
      {
        delete [] this->Array;
      }

      this->Size = sz;
      // Allocate a whole new set of structures
      this->Array = new MeshEdgeNeighbors::NeighborList[sz];

      // Initialize each structure to have 0 entries and NULL pointer.
      for (size_t i=0; i < sz; i++)
      {
        this->Array[i] = linkInit;
      }
    }


    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void incrementLinkCount(size_t ptId)
    {
      this->Array[ptId].ncells++;
    }


    //----------------------------------------------------------------------------
    // This will allocate the actual memory to hold each NeighborList
    void allocateLinks(size_t n)
    {
      for (size_t i=0; i < n; i++)
      {
        this->Array[i].cells = new int[this->Array[i].ncells];
      }
    }

  private:
    NeighborList* Array;   // pointer to data
    size_t Size;


};

#endif /* _MeshEdgeNeighbors_hpp_H_ */
