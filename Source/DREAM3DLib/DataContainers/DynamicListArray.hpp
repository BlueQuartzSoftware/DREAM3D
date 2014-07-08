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

#include <QtCore/QString>
#include <vector>

#include <boost/shared_array.hpp>

//-- DREAM3D Includes
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

/**
 * @brief The MeshFaceNeighbors class contains arrays of Faces for each Node in the mesh. This allows quick query to the node
 * to determine what Cells the node is a part of.
 */
template<typename T>
class DynamicListArray
{
  public:

    DREAM3D_SHARED_POINTERS(DynamicListArray<T>)
    DREAM3D_STATIC_NEW_MACRO(DynamicListArray<T>)
    DREAM3D_TYPE_MACRO(DynamicListArray<T>)

    class ElementList
    {
      public:
        int32_t ncells;
        T* cells;
    };



    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    virtual ~DynamicListArray()
    {
      // This makes sure we deallocate any lists that have been created
      for (size_t i = 0; i < this->m_Size; i++)
      {
        if ( this->m_Array[i].cells != NULL )
        {
          delete [] this->m_Array[i].cells;
        }
      }
      // Now delete all the "NeighborLists" structures
      if ( this->m_Array != NULL )
      {
        delete [] this->m_Array;
      }

    }

    //----------------------------------------------------------------------------
    inline void insertCellReference(size_t ptId, int32_t pos, size_t cellId)
    {
      this->m_Array[ptId].cells[pos] = cellId;
    }

    // Description:
    // Get a link structure given a point id.
    ElementList& getElementList(size_t ptId) {return this->m_Array[ptId];}

    /**
     * @brief setElementList
     * @param ptId
     * @param nCells
     * @param data
     * @return
     */
    bool setElementList(size_t ptId, int32_t nCells, T* data)
    {
      if(ptId >= m_Size) { return false; }
      if(NULL != m_Array[ptId].cells && m_Array[ptId].ncells > 0)
      {
        m_Array[ptId].cells = NULL;
        m_Array[ptId].ncells = 0;
      }
      m_Array[ptId].ncells = nCells;
      //If nCells is huge then there could be problems with this
      this->m_Array[ptId].cells = new T[nCells];
      ::memcpy(m_Array[ptId].cells, data, sizeof(T) * nCells);
      return true;
    }

    // Description:
    // Get the number of cells using the point specified by ptId.
    int32_t getNumberOfElements(size_t ptId) {return this->m_Array[ptId].ncells;}

    // Description:
    // Return a list of cell ids using the point.
    T* getElementListPointer(size_t ptId) {return this->m_Array[ptId].cells;}

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void deserializeLinks(QVector<uint8_t>& buffer, size_t nElements)
    {
      size_t offset = 0;
      allocate(nElements); // Allocate all the links with 0 and NULL;
      uint8_t* bufPtr = buffer.data();

      // Walk the array and allocate all the array links to Zero and NULL
      int32_t* ncells = NULL;
      //int32_t* cells = NULL;
      for(size_t i = 0; i < nElements; ++i)
      {
        ncells = reinterpret_cast<int32_t*>(bufPtr + offset);
        this->m_Array[i].ncells = *ncells; // Set the number of cells in this link
        offset += 2;
        this->m_Array[i].cells = new T[(*ncells)]; // Allocate a new chunk of memory to store the list
        ::memcpy(this->m_Array[i].cells, bufPtr + offset, (*ncells)*sizeof(T) ); // Copy from teh buffer into the new list memory
        offset += (*ncells) * sizeof(T); // Increment the offset
      }
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void deserializeLinks(std::vector<uint8_t>& buffer, size_t nElements)
    {
      size_t offset = 0;
      allocate(nElements); // Allocate all the links with 0 and NULL;
      uint8_t* bufPtr = &(buffer.front());

      // Walk the array and allocate all the array links to Zero and NULL
      int32_t* ncells = NULL;
      //int32_t* cells = NULL;
      for(size_t i = 0; i < nElements; ++i)
      {
        ncells = reinterpret_cast<int32_t*>(bufPtr + offset);
        this->m_Array[i].ncells = *ncells; // Set the number of cells in this link
        offset += 2;
        this->m_Array[i].cells = new T[(*ncells)]; // Allocate a new chunk of memory to store the list
        ::memcpy(this->m_Array[i].cells, bufPtr + offset, (*ncells)*sizeof(T) ); // Copy from teh buffer into the new list memory
        offset += (*ncells) * sizeof(T); // Increment the offset
      }
    }

    void allocateLists(QVector<int32_t>& linkCounts)
    {
      allocate(linkCounts.size());
      for (int i = 0; i < linkCounts.size(); i++)
      {
        this->m_Array[i].ncells = linkCounts[i];
        this->m_Array[i].cells = new int[this->m_Array[i].ncells];
      }
    }

  protected:
    DynamicListArray() :
      m_Array(NULL),
      m_Size(0)
    {}

    //----------------------------------------------------------------------------
    // This will allocate memory to hold all the NeighborList structures where each
    // structure is initialized to Zero Entries and a NULL Pointer
    void allocate(size_t sz, size_t ext = 1000)
    {
      static typename DynamicListArray<T>::ElementList linkInit = {0, NULL};

      // This makes sure we deallocate any lists that have been created
      for (size_t i = 0; i < this->m_Size; i++)
      {
        if ( this->m_Array[i].cells != NULL )
        {
          delete [] this->m_Array[i].cells;
        }
      }
      // Now delete all the "NeighborLists" structures
      if ( this->m_Array != NULL )
      {
        delete [] this->m_Array;
      }

      this->m_Size = sz;
      // Allocate a whole new set of structures
      this->m_Array = new typename DynamicListArray<T>::ElementList[sz];

      // Initialize each structure to have 0 entries and NULL pointer.
      for (size_t i = 0; i < sz; i++)
      {
        this->m_Array[i] = linkInit;
      }
    }


  private:
    ElementList* m_Array;   // pointer to data
    size_t m_Size;


};


typedef DynamicListArray<int32_t> Int32DynamicListArray;

#endif /* _DynamicListArray_H_ */

