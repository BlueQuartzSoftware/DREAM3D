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
#ifndef _MeshVertLinks_hpp_H_
#define _MeshVertLinks_hpp_H_


#include <string.h>

#include <boost/shared_array.hpp>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
//#include "DREAM3DLib/DataContainers/SurfaceMeshDataContainer.h"

/**
 * @brief The MeshVertLinks class contains arrays of Faces for each Node in the mesh. This allows quick query to the node
 * to determine what Cells the node is a part of.
 */
class MeshVertLinks
{
  public:

    DREAM3D_SHARED_POINTERS(MeshVertLinks)
    DREAM3D_STATIC_NEW_MACRO(MeshVertLinks)
    DREAM3D_TYPE_MACRO(MeshVertLinks)

    class FaceList {
      public:
      uint16_t ncells;
      int* cells;
    };

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    virtual ~MeshVertLinks()
    {
      if ( this->Array == NULL )
      {
        return;
      }

      for (size_t i=0; i<this->Size; i++)
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

    //----------------------------------------------------------------------------
    // Description:
    // Get a link structure given a point id.
    FaceList& getFaceList(size_t ptId) {
      return this->Array[ptId];
    }

    //----------------------------------------------------------------------------
    // Description:
    // Get the number of cells using the point specified by ptId.
    uint16_t getNumberOfFaces(size_t ptId) {
      return this->Array[ptId].ncells;
    }

    //----------------------------------------------------------------------------
    // Description:
    // Return a list of cell ids using the point.
    int* getFaceListPointer(size_t ptId) {
      return this->Array[ptId].cells;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void getCellPoints(DREAM3D::SurfaceMesh::FaceList_t::Pointer Faces, size_t cellId, size_t npts, size_t* pts)
    {
      DREAM3D::SurfaceMesh::Face_t& Face = *(Faces->GetPointer(cellId));
      pts[0] = Face.verts[0];
      pts[1] = Face.verts[1];
      pts[2] = Face.verts[2];
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void generateMeshVertLinks(DREAM3D::SurfaceMesh::VertListPointer_t nodes,
                               DREAM3D::SurfaceMesh::FaceListPointer_t Faces )
    {

      size_t numPts = nodes->GetNumberOfTuples();
      size_t numCells = Faces->GetNumberOfTuples();

      // Allocate the basic structures
      allocate(numPts);

      size_t cellId;
      unsigned short* linkLoc;

      // fill out lists with number of references to cells
      typedef boost::shared_array<unsigned short> SharedShortArray_t;
      SharedShortArray_t linkLocPtr(new unsigned short[numPts]);
      linkLoc = linkLocPtr.get();

      ::memset(linkLoc, 0, numPts*sizeof(unsigned short));


      size_t pts[3];
      size_t npts = 3; // ALWAYS 3 points for a Face

      //vtkPolyData *pdata = static_cast<vtkPolyData *>(data);
      // traverse data to determine number of uses of each point
      for (cellId=0; cellId < numCells; cellId++)
      {
        getCellPoints(Faces, cellId, npts, pts);
        for (size_t j=0; j < npts; j++)
        {
          this->incrementLinkCount(pts[j]);
        }
      }

      // now allocate storage for the links
      this->allocateLinks(numPts);

      for (cellId=0; cellId < numCells; cellId++)
      {
        getCellPoints(Faces, cellId, npts, pts);
        for (size_t j=0; j < npts; j++)
        {
          this->insertCellReference(pts[j], (linkLoc[pts[j]])++, cellId);
        }
      }

    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void deserializeLinks(std::vector<uint8_t> &buffer, size_t nVerts)
    {
      size_t offset = 0;
      allocate(nVerts); // Allocate all the links with 0 and NULL;
      uint8_t* bufPtr = &(buffer.front());

      // Walk the array and allocate all the array links to Zero and NULL
      uint16_t* ncells = NULL;
     // int32_t* cells = NULL;
      for(size_t i = 0; i < nVerts; ++i)
      {
        ncells = reinterpret_cast<uint16_t*>(bufPtr + offset);
        this->Array[i].ncells = *ncells; // Set the number of cells in this link
        offset += 2;
        this->Array[i].cells = new int32_t[this->Array[i].ncells]; // Allocate a new chunk of memory to store the list
        ::memcpy(this->Array[i].cells, bufPtr + offset, this->Array[i].ncells*sizeof(int32_t) ); // Copy from teh buffer into the new list memory
        offset += this->Array[i].ncells * sizeof(int32_t); // Increment the offset
      }
    }

  protected:
    MeshVertLinks():Array(NULL),Size(0) {}

    //----------------------------------------------------------------------------
    void allocate(size_t sz, size_t ext=1000)
    {
      static MeshVertLinks::FaceList linkInit = {0,NULL};


      // This makes sure we deallocate any lists that have been created
      for (size_t i = 0; i < Size; i++)
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
      this->Array = new MeshVertLinks::FaceList[sz];

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
    // Allocate memory for the list of lists of cell ids.
    void allocateLinks(size_t n)
    {
      for (size_t i=0; i < n; i++)
      {
        this->Array[i].cells = new int[this->Array[i].ncells];
      }
    }

  private:
    FaceList* Array;   // pointer to data
    size_t Size;

    MeshVertLinks(const MeshVertLinks&); // Copy Constructor Not Implemented
    void operator=(const MeshVertLinks&); // Operator '=' Not Implemented

};



#endif /* _MeshVertLinks_hpp_H_ */
