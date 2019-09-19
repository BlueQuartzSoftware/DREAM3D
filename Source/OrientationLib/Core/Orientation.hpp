/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
 * contributors may be used to endorse or promote products derived from this software
 * without specific prior written permission.
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
 * The code contained herein was partially funded by the followig contracts:
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <cassert>
#include <cstring>
#include <iostream>

#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/SIMPLib.h"

#include "OrientationLib/OrientationLib.h"

/**
 * @brief The Orientation class encapsulates one of many types of rotation representations
 * Bunge Euler Angles (3x1), Orientation Matrix (3x3), Rodrigues-Frank Vector (1x4),
 * Axis-Angle (<Axis>, Scalar>) (4x1), Quaternion (4x1) and Homochoric (3x1). The
 * Class is meant to allow easier use of the Rotation Transformation functions
 * included in the @see RotationTransformation class. The base implementation will
 * allocate the "size" number of elements which represent a single orientation
 * in space. Alternate constructors can allow the class to simply wrap an existing
 * array of values which makes looping through an array of orientations easier.
 */
template <typename T>
class Orientation
{

public:
  //========================================= STL INTERFACE COMPATIBILITY =================================
  using size_type = size_t;
  using value_type = T;
  using reference = T&;
  using iterator_category = std::input_iterator_tag;
  using pointer = T*;
  using difference_type = value_type;

  Orientation() = default;

  /**
   * @brief Orientation Constructor
   * @param size The number of elements
   * @param init Initialization value to be assigned to each element
   */
  Orientation(size_t size, T init = (T)(0))
  : m_Size(size)
  {
    allocate();
    for(size_t i = 0; i < m_Size; i++)
    {
      m_Array[i] = init;
    }
  }

  /**
   * @brief Orientation Constructor
   * @param ptr Pointer to an existing array of values
   * @param size How many elements are in the array
   */
  Orientation(T* ptr, size_t size)
  : m_Array(ptr)
  , m_Size(size)
  , m_OwnsData(false)
  {
  }

  /**
   * @brief Orientation Copy Constructor that will do a deep copy of the elements
   * from the incoming array into the newly constructed Orientation class
   * @param rhs Incoming Orientation class to copy
   */
  Orientation(const Orientation<T>& rhs) // Copy Constructor
  : m_Size(rhs.m_Size)
  {
    allocate();
    ::memcpy(m_Array, rhs.m_Array, sizeof(T) * m_Size); // Copy the bytes over to the new array
  }

  //  Orientation(const Orientation<T>&& rhs) noexcept // move Constructor
  //  : m_Size(rhs.m_Size)
  //  {
  //    m_OwnsData = std::move(rhs.m_OwnsData);
  //    m_Size = std::move(rhs.m_Size);
  //    m_Array = std::move(rhs.m_Array);
  //  }

  /**
   * @brief Orientation
   * @param val0
   * @param val1
   * @param val2
   */
  Orientation(T val0, T val1, T val2)
  : m_Size(3)
  {
    allocate();
    m_Array[0] = val0;
    m_Array[1] = val1;
    m_Array[2] = val2;
  }

  /**
   * @brief Orientation
   * @param val0
   * @param val1
   * @param val2
   * @param val3
   */
  Orientation(T val0, T val1, T val2, T val3)
  : m_Size(4)
  {
    allocate();
    m_Array[0] = val0;
    m_Array[1] = val1;
    m_Array[2] = val2;
    m_Array[3] = val3;
  }

  /**
   * @brief Orientation Copy constructor
   * @param quat
   */
  explicit Orientation(T g[3][3])
  : m_Size(9)
  {
    allocate();
    m_Array[0] = g[0][0];
    m_Array[1] = g[0][1];
    m_Array[2] = g[0][2];
    m_Array[3] = g[1][0];
    m_Array[4] = g[1][1];
    m_Array[5] = g[1][2];
    m_Array[6] = g[2][0];
    m_Array[7] = g[2][1];
    m_Array[8] = g[2][2];
  }

  /**
   * @brief ~Orientation
   */
  virtual ~Orientation()
  {
    if(m_Array != nullptr && m_OwnsData == true)
    {
      delete[] m_Array;
    }
    m_Array = nullptr;
  }

  /**
   * @brief operator = This function will reallocate a new array that matches
   * the incoming Orientation instance *ONLY* if it owns its pointer. The data from `rhs` will be copied into this instance.
   * @param rhs
   */
  Orientation& operator=(const Orientation& rhs) // Copy Assignment
  {
    if(m_Array != nullptr && m_OwnsData == true)
    {
      if(rhs.m_Size != m_Size)
      {
        deallocate();
        m_Size = rhs.size();
        allocate();
      }
      ::memcpy(m_Array, rhs.m_Array, sizeof(T) * m_Size); // Copy the bytes over to the new array
    }
    if(m_OwnsData == false)
    {
      assert(m_Size == rhs.size());
      assert(m_Array != nullptr);
      ::memcpy(m_Array, rhs.m_Array, sizeof(T) * m_Size); // Copy the bytes over to the new array
    }
    return *this;
  }
  /**
   * @brief operator = This function will reallocate a new array that matches
   * the incoming Orientation instance *ONLY* if it owns its pointer. The data from `rhs` will be copied into this instance.
   * @param rhs
   */
  Orientation& operator=(Orientation&& rhs) noexcept // Move Assignment
  {
    // If we own our data deallocate it.
    if(m_OwnsData)
    {
      deallocate();
      m_OwnsData = std::move(rhs.m_OwnsData);
      m_Size = std::move(rhs.m_Size);
      m_Array = std::move(rhs.m_Array);
      rhs.m_Array = nullptr;
      rhs.m_Size = 0;
      rhs.m_OwnsData = false;
    }
    else
    {
      assert(m_Size == rhs.size());
      assert(m_Array != nullptr);
      ::memcpy(m_Array, rhs.m_Array, sizeof(T) * m_Size); // Copy the bytes over to the new array
    }
    return *this;
  }

  //========================================= STL INTERFACE COMPATIBILITY =================================

  class iterator
  {
  public:
    using self_type = iterator;
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using difference_type = value_type;
    using iterator_category = std::forward_iterator_tag;

    iterator(pointer ptr)
    : ptr_(ptr)
    {
    }
    iterator(pointer ptr, size_type ununsed)
    : ptr_(ptr)
    {
    }

    self_type operator++()
    {
      ptr_++;
      return *this;
    } // PREFIX
    self_type operator++(int junk)
    {
      self_type i = *this;
      ptr_++;
      return i;
    } // POSTFIX
    self_type operator+(int amt)
    {
      ptr_ += amt;
      return *this;
    }
    reference operator*()
    {
      return *ptr_;
    }
    pointer operator->()
    {
      return ptr_;
    }
    bool operator==(const self_type& rhs)
    {
      return ptr_ == rhs.ptr_;
    }
    bool operator!=(const self_type& rhs)
    {
      return ptr_ != rhs.ptr_;
    }

  private:
    pointer ptr_;
  };

  class const_iterator
  {
  public:
    using self_type = const_iterator;
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using difference_type = value_type;
    using iterator_category = std::forward_iterator_tag;
    const_iterator(pointer ptr)
    : ptr_(ptr)
    {
    }
    const_iterator(pointer ptr, size_type unused)
    : ptr_(ptr)
    {
    }

    self_type operator++()
    {
      ptr_++;
      return *this;
    } // PREFIX
    self_type operator++(int amt)
    {
      self_type i = *this;
      ptr_ += amt;
      return i;
    } // POSTFIX
    self_type operator+(int amt)
    {
      ptr_ += amt;
      return *this;
    }
    const value_type& operator*()
    {
      return *ptr_;
    }
    const pointer operator->()
    {
      return ptr_;
    }
    bool operator==(const self_type& rhs)
    {
      return ptr_ == rhs.ptr_;
    }
    bool operator!=(const self_type& rhs)
    {
      return ptr_ != rhs.ptr_;
    }

  private:
    pointer ptr_;
  };

  // ######### Iterators #########

  iterator begin()
  {
    return iterator(m_Array);
  }

  iterator end()
  {
    return iterator(m_Array + m_Size);
  }

  const_iterator begin() const
  {
    return const_iterator(m_Array);
  }

  const_iterator end() const
  {
    return const_iterator(m_Array + m_Size);
  }

  // rbegin
  // rend
  // cbegin
  // cend
  // crbegin
  // crend

  // ######### Capacity #########
  /**
   * @brief Returns the number of elements
   * @return
   */
  size_t size() const
  {
    return m_Size;
  }
  size_type max_size() const
  {
    return m_Size;
  }
  size_type capacity() const noexcept
  {
    return m_Size;
  }
  bool empty() const noexcept
  {
    return (m_Size == 0);
  }

  // ######### Element Access #########

  inline reference operator[](size_type index)
  {
    // assert(index < m_Size);
    return m_Array[index];
  }

  inline const T& operator[](size_type index) const
  {
    // assert(index < m_Size);
    return m_Array[index];
  }

  inline reference at(size_type index)
  {
    assert(index < m_Size);
    return m_Array[index];
  }

  inline const T& at(size_type index) const
  {
    assert(index < m_Size);
    return m_Array[index];
  }

  inline reference front()
  {
    return m_Array[0];
  }
  inline const T& front() const
  {
    return m_Array[0];
  }

  inline reference back()
  {
    return m_Array[m_Size - 1];
  }
  inline const T& back() const
  {
    return m_Array[m_Size - 1];
  }

  inline T* data() noexcept
  {
    return m_Array;
  }
  inline const T* data() const noexcept
  {
    return m_Array;
  }

  // ######### Modifiers #########

  /**
   * @brief In the range version (1), the new contents are elements constructed from each of the elements in the range
   * between first and last, in the same order.
   */
  template <class InputIterator>
  void assign(InputIterator first, InputIterator last) // range (1)
  {
    size_type size = last - first;
    resize(size);
    size_type idx = 0;
    while(first != last)
    {
      m_Array[idx] = *first;
      first++;
    }
  }

  /**
   * @brief In the fill version (2), the new contents are n elements, each initialized to a copy of val.
   * @param n
   * @param val
   */
  void assign(size_type n, const value_type& val) // fill (2)
  {
    resize(n);
    for(size_t i = 0; i < n; i++)
    {
      m_Array[i] = val;
    }
  }

  /**
   * @brief In the initializer list version (3), the new contents are copies of the values passed as initializer list, in the same order.
   * @param il
   */
  void assign(std::initializer_list<value_type> il) //  initializer list (3)
  {
    assign(il.begin(), il.end());
  }

  /**
   * @brief clear
   */
  void clear()
  {
    if(nullptr != m_Array && m_OwnsData)
    {
      deallocate();
    }
    m_Array = nullptr;
    m_Size = 0;
    m_OwnsData = true;
  }

  /**
   * @brief equal
   * @param range1
   * @param range2
   * @return
   */
  template <typename Range1, typename Range2>
  bool equal(Range1 const& range1, Range2 const& range2)
  {
    if(range1.size() != range2.size())
    {
      return false;
    }

    return std::equal(begin(range1), end(range1), begin(range2));
  }

  /**
   * @brief toGMatrix Copies the internal values into the 3x3 "G" Matrix
   * @param g
   */
  void toGMatrix(T g[3][3]) const
  {
    assert(m_Size == 9);
    g[0][0] = m_Array[0];
    g[0][1] = m_Array[1];
    g[0][2] = m_Array[2];
    g[1][0] = m_Array[3];
    g[1][1] = m_Array[4];
    g[1][2] = m_Array[5];
    g[2][0] = m_Array[6];
    g[2][1] = m_Array[7];
    g[2][2] = m_Array[8];
  }

  /**
   * @brief toAxisAngle Copies the values out to an Axis-Angle representation. Note that
   * arguments will have values copied into them as they are pass-by-referemce.
   * @param x
   * @param y
   * @param z
   * @param w
   */
  void toAxisAngle(T& x, T& y, T& z, T& w) const
  {
    x = m_Array[0];
    y = m_Array[1];
    z = m_Array[2];
    w = m_Array[3];
  }

  void copyInto(T* ptr, size_type size) const
  {
    if(m_Size < size)
    {
      size = m_Size;
    }
    for(size_type i = 0; i < size; i++)
    {
      ptr[i] = m_Array[i];
    }
  }

  /**
   * @brief resize Resizes the array to the new length
   * @param elements The number of elements in the new array
   */
  void resize(size_t size)
  {
    T* newArray;
    size_t newSize;
    size_t oldSize;

    if(size == m_Size) // Requested size is equal to current size.  Do nothing.
    {
      return;
    }
    newSize = size;
    oldSize = m_Size;

    // Wipe out the array completely if new size is zero.
    if(newSize == 0)
    {
      clear();
      return;
    }
    // OS X's realloc does not free memory if the new block is smaller.  This
    // is a very serious problem and causes huge amount of memory to be
    // wasted. Do not use realloc on the Mac.
    bool dontUseRealloc = false;
#if defined __APPLE__
    dontUseRealloc = true;
#endif

    // Allocate a new array if we DO NOT own the current array
    if((nullptr != m_Array) && (false == m_OwnsData))
    {
      // The old array is owned by the user so we cannot try to
      // reallocate it.  Just allocate new memory that we will own.
      newArray = new T[newSize]();

      if(!newArray)
      {
        std::cout << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. ";
        return;
      }

      // Copy the data from the old array.
      std::memcpy(newArray, m_Array, (newSize < m_Size ? newSize : m_Size) * sizeof(T));
    }
    else if(!dontUseRealloc)
    {
      newArray = new T[newSize]();

      if(!newArray)
      {
        std::cout << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. ";
        return;
      }
    }
    else
    {
      newArray = new T[newSize]();
      if(!newArray)
      {
        std::cout << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. ";
        return;
      }

      // Copy the data from the old array.
      if(m_Array != nullptr)
      {
        std::memcpy(newArray, m_Array, (newSize < m_Size ? newSize : m_Size) * sizeof(T));
      }
      // Free the old array
      deallocate();
    }

    // Allocation was successful.  Save it.
    m_Size = newSize;
    m_Array = newArray;

    // This object has now allocated its memory and owns it.
    m_OwnsData = true;
  }

protected:
  /**
   * @brief allocate Allocates the needed amount of memory freeing any memory
   * that is currently being used.
   */
  void allocate()
  {
    if((nullptr != m_Array) && (true == m_OwnsData))
    {
      deallocate();
    }
    m_Array = nullptr;
    m_OwnsData = true;
    if(m_Size == 0)
    {
      clear();
      return;
    }

    size_t newSize = m_Size;
    m_Array = new T[newSize]();
    if(!m_Array)
    {
      std::cout << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. " << std::endl;
      return;
    }
    m_Size = newSize;
  }

  /**
   * @brief deallocates the memory block
   */
  void deallocate()
  {
    // We are going to splat 0xABABAB across the first value of the array as a debugging aid
    auto cptr = reinterpret_cast<unsigned char*>(m_Array);
    if(nullptr != cptr)
    {
      if(m_Size > 0)
      {
        if(sizeof(T) >= 1)
        {
          cptr[0] = 0xAB;
        }
        if(sizeof(T) >= 2)
        {
          cptr[1] = 0xAB;
        }
        if(sizeof(T) >= 4)
        {
          cptr[2] = 0xAB;
          cptr[3] = 0xAB;
        }
        if(sizeof(T) >= 8)
        {
          cptr[4] = 0xAB;
          cptr[5] = 0xAB;
          cptr[6] = 0xAB;
          cptr[7] = 0xAB;
        }
      }
    }
#if 0
      if (MUD_FLAP_0 != 0xABABABABABABABABul
          || MUD_FLAP_1 != 0xABABABABABABABABul
          || MUD_FLAP_2 != 0xABABABABABABABABul
          || MUD_FLAP_3 != 0xABABABABABABABABul
          || MUD_FLAP_4 != 0xABABABABABABABABul
          || MUD_FLAP_5 != 0xABABABABABABABABul)
      {
        Q_ASSERT(false);
      }
#endif
    delete[](m_Array);

    m_Array = nullptr;
  }

private:
  T* m_Array = nullptr;
  size_t m_Size = 0;
  bool m_OwnsData = true;
};

using OrientationType = Orientation<double>;
using OrientationD = Orientation<double>;
using OrientationF = Orientation<float>;
