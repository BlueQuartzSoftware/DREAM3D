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
#ifndef _DynamicListArray_H_
#define _DynamicListArray_H_

#include <string>
#include <vector>

#include <boost/shared_array.hpp>

//-- DREAM3D Includes
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

/**
 * @brief The MeshFaceNeighbors class contains arrays of Faces for each Node in the mesh. This allows quick query to the node
 * to determine what Cells the node is a part of.
 */
class DynamicListArray
{
  public:

    DREAM3D_SHARED_POINTERS(DynamicListArray)
    DREAM3D_STATIC_NEW_MACRO(DynamicListArray)
    DREAM3D_TYPE_MACRO(DynamicListArray)

    class ElementList {
      public:
      unsigned short ncells;
      int* cells;
    };



    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    virtual ~DynamicListArray(){ }

    //----------------------------------------------------------------------------
    inline void insertCellReference(size_t ptId, unsigned short pos, size_t cellId)
    {
      this->Array[ptId].cells[pos] = cellId;
    }

    // Description:
    // Get a link structure given a point id.
    ElementList& getElementList(size_t ptId) {return this->Array[ptId];}

    bool setElementList(size_t ptId, uint16_t nCells, int32_t* data)
    {
      if(ptId >= Size) return false;
      if(NULL != Array[ptId].cells && Array[ptId].ncells > 0)
      {
        Array[ptId].cells = NULL;
        Array[ptId].ncells = 0;
      }
      Array[ptId].ncells = nCells;
      //If nCells is huge then there could be problems with this
      this->Array[ptId].cells = new int[nCells];    
      ::memcpy(Array[ptId].cells, data, sizeof(int) * nCells);
      return true;
    }

    // Description:
    // Get the number of cells using the point specified by ptId.
    unsigned short getNumberOfElements(size_t ptId) {return this->Array[ptId].ncells;}

    // Description:
    // Return a list of cell ids using the point.
    int* getElementListPointer(size_t ptId) {return this->Array[ptId].cells;}

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void deserializeLinks(QVector<uint8_t> &buffer, size_t nElements)
    {
      size_t offset = 0;
      allocate(nElements); // Allocate all the links with 0 and NULL;
      uint8_t* bufPtr = &(buffer.front());

      // Walk the array and allocate all the array links to Zero and NULL
      uint16_t* ncells = NULL;
      //int32_t* cells = NULL;
      for(size_t i = 0; i < nElements; ++i)
      {
        ncells = reinterpret_cast<uint16_t*>(bufPtr + offset);
        this->Array[i].ncells = *ncells; // Set the number of cells in this link
        offset += 2;
        this->Array[i].cells = new int32_t[(*ncells)]; // Allocate a new chunk of memory to store the list
        ::memcpy(this->Array[i].cells, bufPtr + offset, (*ncells)*sizeof(int32_t) ); // Copy from teh buffer into the new list memory
        offset += (*ncells) * sizeof(int32_t); // Increment the offset
      }
    }


    void allocateLists(QVector<uint16_t> &linkCounts)
    {
      allocate(linkCounts.size());
      for (size_t i=0; i < linkCounts.size(); i++)
      {
        this->Array[i].cells = new int[this->Array[i].ncells];
        this->Array[i].ncells = linkCounts[i];
      }
    }

  protected:
    DynamicListArray();

    //----------------------------------------------------------------------------
    // This will allocate memory to hold all the NeighborList structures where each
    // structure is initialized to Zero Entries and a NULL Pointer
    void allocate(size_t sz, size_t ext=1000)
    {
      static DynamicListArray::ElementList linkInit = {0,NULL};

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
      this->Array = new DynamicListArray::ElementList[sz];

      // Initialize each structure to have 0 entries and NULL pointer.
      for (size_t i=0; i < sz; i++)
      {
        this->Array[i] = linkInit;
      }
    }


  private:
    ElementList* Array;   // pointer to data
    size_t Size;


};

#endif /* _DynamicListArray_H_ */
