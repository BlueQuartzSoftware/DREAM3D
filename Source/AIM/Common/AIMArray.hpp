///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////

#ifndef AIMARRAY_HPP_
#define AIMARRAY_HPP_


#include <string.h>
#include <iostream>

#include "AIM/Common/AIMCommonConfiguration.h"
#include <MXA/MXATypes.h>
#include <MXA/Common/MXASetGetMacros.h>

#if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
#include <mm_malloc.h>
#endif

template<typename T>
class AIMCOMMON_EXPORT AIMArray
{
  public:

    MXA_SHARED_POINTERS(AIMArray)
    MXA_STATIC_NEW_MACRO(AIMArray)
    MXA_TYPE_MACRO(AIMArray)

    virtual ~AIMArray()
    {
      if (this->m_buffer != NULL && this->m_ManageMemory == true)
      {
        this->deallocateArrayData();
      }
    }

    MXA_INSTANCE_PROPERTY(size_t, NumberOfElements);

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    MXA_INSTANCE_PROPERTY(bool, ManageMemory)


    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    void setArrayData(T* value, bool manageMemory = false)
    {
      if (this->m_buffer != NULL && this->m_ManageMemory == true && value != this->m_buffer)
      {
        this->deallocateArrayData();
      }
      this->m_buffer = value;
    }

    void setValue(T value, size_t index)
    {
      m_buffer[index] = value;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    T* getPointer(size_t index = 0)
    {
      return &(m_buffer[index]);
    }

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    T* allocateArray(size_t numberOfElements, bool manageMemory = false)
    {
      this->deallocateArrayData();

#if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
      m_buffer = static_cast<T*>( _mm_malloc (numberOfElements * sizeof(T), 16) );
#else
        m_buffer = new T[numberOfElements];
#endif
        m_NumberOfElements = numberOfElements;
      this->m_ManageMemory = manageMemory;
      return m_buffer;
    }

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    template<typename K>
    T* allocateSameSizeArray(typename AIMArray<K>::Pointer array)
    {
      return allocateArray(array->getNumberOfElements(), true);
    }

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    void deallocateArrayData()
    {
      if (this->m_buffer != NULL && this->m_ManageMemory == true)
      {
#if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
        _mm_free( this->m_buffer );
#else
        delete[] this->m_buffer;
#endif
      }
      m_buffer = NULL;
    }


    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    int32_t initializeImageWithSourceData(size_t numberOfElements, T* source)
    {
      this->allocateArray(numberOfElements, true);

      T* b = static_cast<T*> (::memcpy(m_buffer, source, sizeof(T) * numberOfElements));
      return (b == m_buffer) ? 1 : -1;
    }

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    int32_t zeroArrayData()
    {
      ::memset(m_buffer, 0, m_NumberOfElements);
      return (NULL != m_buffer) ? 1 : -1;
    }

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    void printSelf(std::ostream& out)
    {
      out << "AIMImage Properties" << std::endl;
      // out << "  Origin:                 " << _origin[0] << ", " << _origin[1] << std::endl;
      //out << "  ImageMicronSize:        " << _micronSize[0] << " x " << _micronSize[1] << std::endl;
      out << "  Number Of Elements:         " << m_NumberOfElements << std::endl;
      //out << "  Scaling:                " << _scaling[0] << ", " << _scaling[1] << std::endl;
      out << "  ManageMemory:           " << m_ManageMemory << std::endl;
      out << "  ImageBuffer:            " << *m_buffer << std::endl;
      // _intersectedTile->printSelf(out);
    }

  protected:
    AIMArray()
    {
      m_NumberOfElements = 0;
      m_ManageMemory = false;
      this->m_buffer = NULL;
    }

  private:
    T* m_buffer;
    AIMArray(const AIMArray&); // Copy Constructor Not Implemented
    void operator=(const AIMArray&); // Operator '=' Not Implemented
};

#endif /* AIMARRAY_HPP_ */
