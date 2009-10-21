/* <distribution-statement>
 * *****************************************************************************
 * Distribution authorized to DOD components and DOD contractors only;         *
 * Administrative or Operational Use;                                          *
 * February 2009.                                                              *
 * Other requests for this document shall be referred to                       *
 * U.S. Air Force Research Laboratory                                          *
 * 2230 Tenth St., Ste. 1                                                      *
 * Wright-Patterson AFB, OH 45433-7817                                         *
 *                                                                             *
 * Destroy by contents or reconstruction of the document.                      *
 *                                                                             *
 * This code was written under the U.S. Air Force contrct F33615-03-C-5220     *
 * *****************************************************************************
 * </distribution-statement> */

#ifndef AIMARRAY_HPP_
#define AIMARRAY_HPP_

#include <MXA/Common/MXATypes.h>
#include <MXA/Common/MXASetGetMacros.h>

#include <iostream>

#if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
#include <mm_malloc.h>
#endif

template<typename T>
class AIMArray
{
  public:

    MXA_SHARED_POINTERS(AIMArray)
    MXA_STATIC_NEW_MACRO(AIMArray)
    MXA_TYPE_MACRO(AIMArray)

    virtual ~AIMArray()
    {
      if (this->m_buffer != NULL && this->_managememory == true)
      {
        this->deallocateArrayData();
      }
    }

    MXA_INSTANCE_PROPERTY_m(size_t, NumberOfElements);

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    MXA_INSTANCE_PROPERTY(bool, ManageMemory, _managememory)


    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    void setArrayData(T* value, bool manageMemory = false)
    {
      if (this->m_buffer != NULL && this->_managememory == true && value != this->m_buffer)
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
      this->_managememory = manageMemory;
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
      if (this->m_buffer != NULL && this->_managememory == true)
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
    int32 initializeImageWithSourceData(size_t numberOfElements, T* source)
    {
      this->allocateArray(numberOfElements, true);

      T* b = static_cast<T*> (::memcpy(m_buffer, source, sizeof(T) * numberOfElements));
      return (b == m_buffer) ? 1 : -1;
    }

    // -----------------------------------------------------------------------------
    // Tested
    // -----------------------------------------------------------------------------
    int32 zeroArrayData()
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
      out << "  ManageMemory:           " << _managememory << std::endl;
      out << "  ImageBuffer:            " << *m_buffer << std::endl;
      // _intersectedTile->printSelf(out);
    }

  protected:
    AIMArray()
    {
      m_NumberOfElements = 0;
      _managememory = false;
      this->m_buffer = NULL;
    }

  private:
    T* m_buffer;
    AIMArray(const AIMArray&); // Copy Constructor Not Implemented
    void operator=(const AIMArray&); // Operator '=' Not Implemented
};

#endif /* AIMARRAY_HPP_ */
