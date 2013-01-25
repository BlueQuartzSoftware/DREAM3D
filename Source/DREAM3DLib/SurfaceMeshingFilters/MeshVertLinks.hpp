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
#include "DREAM3DLib/Common/SurfaceMeshDataContainer.h"

/**
 * @brief The MeshVertLinks class contains arrays of Triangles for each Node in the mesh. This allows quick query to the node
 * to determine what Cells the node is a part of.
 */
class MeshVertLinks
{
  public:

    DREAM3D_SHARED_POINTERS(MeshVertLinks)
    DREAM3D_STATIC_NEW_MACRO(MeshVertLinks)
    DREAM3D_TYPE_MACRO(MeshVertLinks)

    class TriangleList {
      public:
      unsigned short ncells;
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

      for (size_t i=0; i<=this->MaxId; i++)
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
    TriangleList& getTriangleList(size_t ptId) {
      return this->Array[ptId];
    }

    // Description:
    // Get the number of cells using the point specified by ptId.
    uint16_t getNumberOfTriangles(size_t ptId) {
      return this->Array[ptId].ncells;
    }
    // Description:
    // Return a list of cell ids using the point.
    int* getTriangleListPointer(size_t ptId) {
      return this->Array[ptId].cells;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void getCellPoints(SurfaceMesh::DataStructures::FaceList_t::Pointer triangles, size_t cellId, size_t npts, size_t* pts)
    {
      SurfaceMesh::DataStructures::Face_t& triangle = *(triangles->GetPointer(cellId));
      pts[0] = triangle.verts[0];
      pts[1] = triangle.verts[1];
      pts[2] = triangle.verts[2];
    }



    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void generateMeshVertLinks(SurfaceMesh::DataStructures::VertListPointer_t nodes,
                               SurfaceMesh::DataStructures::FaceListPointer_t triangles )
    {

      size_t numPts = nodes->GetNumberOfTuples();
      size_t numCells = triangles->GetNumberOfTuples();

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
      size_t npts = 3; // ALWAYS 3 points for a triangle

      //vtkPolyData *pdata = static_cast<vtkPolyData *>(data);
      // traverse data to determine number of uses of each point
      for (cellId=0; cellId < numCells; cellId++)
      {
        getCellPoints(triangles, cellId, npts, pts);
        for (size_t j=0; j < npts; j++)
        {
          this->incrementLinkCount(pts[j]);
        }
      }

      // now allocate storage for the links
      this->allocateLinks(numPts);
      this->MaxId = numPts - 1;
      //size_t memory_used = 0;

      for (cellId=0; cellId < numCells; cellId++)
      {
        getCellPoints(triangles, cellId, npts, pts);
        for (size_t j=0; j < npts; j++)
        {
          this->insertCellReference(pts[j], (linkLoc[pts[j]])++, cellId);
        }
      }

    }

  protected:
    MeshVertLinks():Array(NULL),Size(0),MaxId(-1),Extend(1000) {}

    //----------------------------------------------------------------------------
    void allocate(size_t sz, size_t ext=1000)
    {
      static MeshVertLinks::TriangleList linkInit = {0,NULL};

      this->Size = sz;
      if ( this->Array != NULL )
      {
        delete [] this->Array;
      }
      this->Array = new MeshVertLinks::TriangleList[sz];
      this->Extend = ext;
      this->MaxId = -1;

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
     // size_t memory_used = 0;
      for (size_t i=0; i < n; i++)
      {
        this->Array[i].cells = new int[this->Array[i].ncells];
     //   memory_used= memory_used + (10 + sizeof(int)*this->Array[i].ncells);
      }
     // std::cout << "MeshVertLinks Memory Used: " << memory_used << std::endl;
    }

  private:
    TriangleList* Array;   // pointer to data
    size_t Size;
    size_t MaxId;
    size_t Extend;

    MeshVertLinks(const MeshVertLinks&); // Copy Constructor Not Implemented
    void operator=(const MeshVertLinks&); // Operator '=' Not Implemented

};



#endif /* _MeshVertLinks_hpp_H_ */
