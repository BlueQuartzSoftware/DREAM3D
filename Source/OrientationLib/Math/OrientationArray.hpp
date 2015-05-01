/* ============================================================================
 * Copyright (c) 2015 BlueQuartz Software, LLC
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
 * Neither the name of Michael A. Jackson, BlueQuartz Software nor the names of
 * its contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
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
 *                 FA8650-07-D-5800 & FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _OrientationArray_H_
#define _OrientationArray_H_

#include <assert.h>
#include <string.h>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"


#include "OrientationLib/OrientationLib.h"


template<typename T>
/**
 * @brief The OrientationArray class encapsulates one of many types of rotation representations
 * Bunge Euler Angles (3x1), Orientation Matrix (3x3), Rodrigues-Frank Vector (1x3),
 * Axis-Angle (<Axis>, Scalar>) (4x1), Quaternion (4x1) and Homochoric (3x1). The
 * Class is meant to allow easier use of the Rotation Transformation functions
 * included in the @see RotationTransformation class. The base implementation will
 * allocate the "size" number of elements which represent a single orientation
 * in space. Alternate constructors can allow the class to simply wrap an existing
 * array of values which makes looping through an array of orientations easier.
 */
class OrientationArray
{

  public:
    /**
     * @brief OrientationArray Constructor
     * @param size The number of elements
     * @param init Initialization value to be assigned to each element
     */
    OrientationArray(size_t size, T init = (T)(0) ) :
      m_Ptr(NULL),
      m_Size(size),
      m_Owns(true)
    {
      allocate();
      for(size_t i = 0; i < m_Size; i++)
      {
        m_Ptr[i] = init;
      }
    }

    /**
     * @brief OrientationArray Constructor
     * @param ptr Pointer to an existing array of values
     * @param size How many elements are in the array
     */
    OrientationArray(T* ptr, size_t size) :
      m_Ptr(ptr),
      m_Size(size),
      m_Owns(false)
    {

    }

    /**
     * @brief OrientationArray Copy Constructor that will do a deep copy of the elements
     * from the incoming array into the newly constructed OrientationArray class
     * @param rhs Incoming OrientationArray class to copy
     */
    OrientationArray(const OrientationArray<T>& rhs) :
    m_Ptr(NULL),
    m_Size(rhs.m_Size),
    m_Owns(true)
    {
      allocate();
      ::memcpy(m_Ptr, rhs.m_Ptr, sizeof(T) * m_Size); // Copy the bytes over to the new array
    }

    /**
     * @brief ~OrientationArray
     */
    virtual ~OrientationArray() {
      if(m_Ptr != NULL && m_Owns == true)
      {
        free(m_Ptr);
      }
      m_Ptr = NULL;
    }

    /**
     * @brief operator = This function will reallocate a new array that matches
     * the incoming OrientationArray instance and copy all the data from the incoming
     * representation into the current instance.
     */
    void operator=(const OrientationArray& rhs)
    {
      if(m_Ptr != NULL && m_Owns == true)
      {
        free(m_Ptr);
      }
      m_Ptr = NULL;
      m_Size = rhs.size();
      allocate();
      ::memcpy(m_Ptr, rhs.m_Ptr, sizeof(T) * m_Size); // Copy the bytes over to the new array
    }

    /**
     * @brief Returns the number of elements
     * @return
     */
    size_t size() const { return m_Size; }

    /**
     * @brief operator [] Returns a reference to the value at the indicated offset.
     * This will assert if "i" is not within the bounds of the array size
     * @param i
     * @return
     */
    T& operator[](size_t i) const {
      assert(i < m_Size);
      return m_Ptr[i];
    }

    /**
     * @brief data Returns a pointer to the internal data array
     * @return
     */
    T* data() const { return m_Ptr; }

    /**
     * @brief resize Resizes the array to the new length
     * @param elements The number of elements in the new array
     */
    void resize(size_t size)
    {
      T* newArray;
      size_t newSize;
      size_t oldSize;

      if (size == m_Size) // Requested size is equal to current size.  Do nothing.
      {
        return;
      }
      //If we do NOT own the array then there is no way to resize the array without
      // detaching and making a copy. Not sure what we would want to do so I am
      // going to assert here and die.
      assert(m_Owns);

      newSize = size;
      oldSize = m_Size;

      // Wipe out the array completely if new size is zero.
      if (newSize == 0)
      {
        if(m_Ptr != NULL && m_Owns == true)
        {
          free(m_Ptr);
        }
        m_Ptr = NULL;
        m_Owns = false;
        m_Size = 0;
        return;
      }
      // OS X's realloc does not free memory if the new block is smaller.  This
      // is a very serious problem and causes huge amount of memory to be
      // wasted. Do not use realloc on the Mac.
      bool dontUseRealloc = false;
#if defined __APPLE__
      dontUseRealloc = true;
#endif

      if (!dontUseRealloc)
      {
        // Try to reallocate with minimal memory usage and possibly avoid copying.
        newArray = (T*)realloc(m_Ptr, newSize * sizeof(T));
        if (!newArray)
        {
          free(m_Ptr);
          m_Ptr = NULL;
          m_Owns = false;
          m_Size = 0;
          return;
        }
      }
      else
      {
        newArray = (T*)malloc(newSize * sizeof(T));
        if (!newArray)
        {
          free(m_Ptr);
          m_Ptr = NULL;
          m_Owns = false;
          m_Size = 0;
          return;
        }

        // Copy the data from the old array.
        if (m_Ptr != NULL)
        {
          memcpy(newArray, m_Ptr, (newSize < m_Size ? newSize : m_Size) * sizeof(T));
        }
        // Free the old array
        free(m_Ptr);
        m_Ptr = NULL;
      }

      // Allocation was successful.  Save it.
      m_Size = newSize;
      m_Ptr = newArray;

      // This object has now allocated its memory and owns it.
      m_Owns = true;

    }


  protected:
    /**
     * @brief allocate Allocates the needed amount of memory freeing any memory
     * that is currently being used.
     */
    void allocate()
    {

      if(m_Ptr != NULL && m_Owns == true)
      {
        free(m_Ptr);
        m_Ptr = NULL;
      }
      else if(m_Ptr != NULL && m_Owns == false)
      {
        assert(false); // If the pointer is owned by another class then we can not allocate.
      }
      // If we made it this far the pointer should be NULL and we can go ahead and allocate our memory
      if(m_Ptr == NULL)
      {
        m_Ptr = reinterpret_cast<T*>(malloc(sizeof(T) * m_Size));
        ::memset(m_Ptr, 0, sizeof(T) * m_Size);
        m_Owns = true;
      }

    }

  private:
    T* m_Ptr;
    size_t m_Size;
    bool m_Owns;

};

/**
 * @brief OrientationArrayF A convenience Typedef for a OrientationArray<float>
 */
typedef OrientationArray<float> FloatOrientationArray_t;

/**
 * @brief OrientationArrayD A convenience Typedef for a OrientationArray<double>
 */
typedef OrientationArray<double> DoubleOrientationArray_t;


#endif /* _OrientationArray_H_ */
