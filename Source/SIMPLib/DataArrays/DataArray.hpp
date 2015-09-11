/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _DataArray_h_
#define _DataArray_h_

// STL Includes
#include <vector>
#include <sstream>
#include <fstream>


#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/HDF5/H5DataArrayWriter.hpp"
#include "SIMPLib/HDF5/H5DataArrayReader.h"


#define mxa_bswap(s,d,t)\
  t[0] = ptr[s];\
  ptr[s] = ptr[d];\
  ptr[d] = t[0];

/** @brief Resizes the DataArray Shared m_Array and assigns its internal data pointer
 *
 */
#define RESIZE_ARRAY(sharedArray, pointer, size)\
  pointer = sharedArray->WritePointer(0, size);

#define DECLARE_WRAPPED_ARRAY(pubVar, priVar, Type)\
  DataArray<Type>::Pointer priVar;\
  Type* pubVar;

#define INIT_DataArray(var, Type)\
  var = DataArray<Type>::CreateArray(0, #var);



/**
 * @class DataArray DataArray.hpp DREAM3DLib/Common/DataArray.hpp
 * @brief Template class for wrapping raw arrays of data.
 * @author mjackson
 * @date July 3, 2008
 * @version $Revision: 1.2 $
 */
template<typename T>
class DataArray : public IDataArray
{
  public:

    SIMPL_SHARED_POINTERS(DataArray<T> )
    SIMPL_TYPE_MACRO_SUPER(DataArray<T>, IDataArray)
    SIMPL_CLASS_VERSION(2)

    typedef QVector<Pointer>   ContainterType;

    enum NumType
    {
      Int8 = 0,
      UInt8,
      Int16,
      UInt16,
      Int32,
      UInt32,
      Int64,
      UInt64,
      Float,
      Double,
      Bool,
      UnknownNumType
    };

    /**
     * @brief GetTypeName Returns a string representation of the type of data that is stored by this class. This
     * can be a primitive like char, float, int or the name of a class.
     * @return
     */
    void getXdmfTypeAndSize(QString& xdmfTypeName, int& precision)
    {
      T value = 0x00;
      xdmfTypeName = "UNKNOWN";
      precision = 0;
      if (typeid(value) == typeid(int8_t)) { xdmfTypeName = "Char"; precision = 1;}
      if (typeid(value) == typeid(uint8_t)) { xdmfTypeName = "UChar"; precision = 1;}

      if (typeid(value) == typeid(int16_t)) { xdmfTypeName = "Int"; precision = 2;}
      if (typeid(value) == typeid(uint16_t)) { xdmfTypeName = "UInt"; precision = 2;}

      if (typeid(value) == typeid(int32_t)) { xdmfTypeName = "Int"; precision = 4;}
      if (typeid(value) == typeid(uint32_t)) { xdmfTypeName = "UInt"; precision = 4;}

      if (typeid(value) == typeid(int64_t)) { xdmfTypeName = "Int"; precision = 8;}
      if (typeid(value) == typeid(uint64_t)) { xdmfTypeName = "UInt"; precision = 8;}

      if (typeid(value) == typeid(float)) { xdmfTypeName = "Float"; precision = 4;}
      if (typeid(value) == typeid(double)) { xdmfTypeName = "Float"; precision = 8;}

      if (typeid(value) == typeid(bool)) { xdmfTypeName = "uchar"; precision = 1;}
    }


    /**
     * @brief GetTypeName Returns a string representation of the type of data that is stored by this class. This
     * can be a primitive like char, float, int or the name of a class.
     * @return
     */

    NumType getType()
    {
      T value = 0x00;
      if (typeid(value) == typeid(int8_t)) { return Int8;}
      if (typeid(value) == typeid(uint8_t)) { return UInt8;}

      if (typeid(value) == typeid(int16_t)) { return Int16;}
      if (typeid(value) == typeid(uint16_t)) { return UInt16;}

      if (typeid(value) == typeid(int32_t)) { return Int32;}
      if (typeid(value) == typeid(uint32_t)) { return UInt32;}

      if (typeid(value) == typeid(int64_t)) { return Int64;}
      if (typeid(value) == typeid(uint64_t)) { return UInt64;}

      if (typeid(value) == typeid(float)) { return Float;}
      if (typeid(value) == typeid(double)) { return Double;}

      if (typeid(value) == typeid(bool)) { return Bool;}

      return UnknownNumType;
    }

    /**
     * @brief Static constructor
     * @param numElements The number of elements in the internal array.
     * @param name The name of the array
     * @return Boost::Shared_Ptr wrapping an instance of DataArrayTemplate<T>
     */
    static Pointer CreateArray(size_t numElements, const QString& name, bool allocate = true)
    {
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }
      QVector<size_t> cDims(1, 1);
      DataArray<T>* d = new DataArray<T>(numElements, cDims, name, allocate);
      if(allocate)
      {
        if (d->allocate() < 0)
        {
          // Could not allocate enough memory, reset the pointer to null and return
          delete d;
          return DataArray<T>::NullPointer();
        }
      }
      Pointer ptr(d);
      return ptr;
    }

    /**
     * @brief Static constructor
     * @param numTuples The number of tuples in the array.
     * @param rank The number of dimensions of the attribute on each Tuple
     * @param dims The actual dimensions of the attribute on each Tuple
     * @param name The name of the array
     * @return Boost::Shared_Ptr wrapping an instance of DataArrayTemplate<T>
     */
    static Pointer CreateArray(size_t numTuples, int rank, size_t* dims, const QString& name, bool allocate = true)
    {
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }
      QVector<size_t> cDims(rank);
      for (int i = 0; i < rank; i++)
      {
        cDims[i] = dims[i];
      }
      DataArray<T>* d = new DataArray<T>(numTuples, cDims, name, allocate);
      if(allocate)
      {
        if (d->allocate() < 0)
        {
          // Could not allocate enough memory, reset the pointer to null and return
          delete d;
          return DataArray<T>::NullPointer();
        }
      }
      Pointer ptr(d);
      return ptr;
    }

    /**
     * @brief Static constructor
     * @param numTuples The number of tuples in the array.
     * @param dims The actual dimensions of the attribute on each Tuple
     * @param name The name of the array
     * @return Boost::Shared_Ptr wrapping an instance of DataArrayTemplate<T>
     */
    static Pointer CreateArray(size_t numTuples, std::vector<size_t> cDims, const QString& name, bool allocate = true)
    {
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }
      DataArray<T>* d = new DataArray<T>(numTuples, QVector<size_t>::fromStdVector(cDims), name, allocate);
      if(allocate)
      {
        if (d->allocate() < 0)
        {
          // Could not allocate enough memory, reset the pointer to null and return
          delete d;
          return DataArray<T>::NullPointer();
        }
      }
      Pointer ptr(d);
      return ptr;
    }

    /**
     * @brief Static constructor
     * @param numTuples The number of tuples in the array.
     * @param dims The actual dimensions of the attribute on each Tuple
     * @param name The name of the array
     * @return Boost::Shared_Ptr wrapping an instance of DataArrayTemplate<T>
     */
    static Pointer CreateArray(size_t numTuples, QVector<size_t> cDims, const QString& name, bool allocate = true)
    {
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }
      DataArray<T>* d = new DataArray<T>(numTuples, cDims, name, allocate);
      if(allocate)
      {
        if (d->allocate() < 0)
        {
          // Could not allocate enough memory, reset the pointer to null and return
          delete d;
          return DataArray<T>::NullPointer();
        }
      }
      Pointer ptr(d);
      return ptr;
    }

    /**
    * @brief Static constructor
    * @param numTuples The number of tuples in the array.
    * @param tDims The actual dimensions of the Tuples
    * @param cDims The actual dimensions of the attribute on each Tuple
    * @param name The name of the array
    * @return Boost::Shared_Ptr wrapping an instance of DataArrayTemplate<T>
    */
    static Pointer CreateArray(QVector<size_t> tDims, QVector<size_t> cDims, const QString& name, bool allocate = true)
    {
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }
      size_t numTuples = tDims[0];
      for(int i = 1; i < tDims.size(); i++)
      {
        numTuples *= tDims[i];
      }
      DataArray<T>* d = new DataArray<T>(numTuples, cDims, name, allocate);
      if(allocate)
      {
        if (d->allocate() < 0)
        {
          // Could not allocate enough memory, reset the pointer to null and return
          delete d;
          return DataArray<T>::NullPointer();
        }
      }
      Pointer ptr(d);
      return ptr;
    }

    /**
     * @brief Static Method to create a DataArray from a QVector through a deep copy of the data
     * contained in the vector. The number of components will be set to 1.
     * @param vec The vector to copy the data from
     * @param name The name of the array
     * @return Boost::Shared_Ptr wrapping an instance of DataArrayTemplate<T>
     */
    static Pointer FromQVector(QVector<T>& vec, const QString& name, bool allocate = true)
    {

      Pointer p = CreateArray(vec.size(), name, allocate);
      if (NULL != p.get())
      {
        ::memcpy(p->getPointer(0), vec.data(), vec.size() * sizeof(T));
      }
      return p;
    }

    /**
     * @brief Static Method to create a DataArray from a QVector through a deep copy of the data
     * contained in the vector. The number of components will be set to 1.
     * @param vec The vector to copy the data from
     * @param name The name of the array
     * @return Boost::Shared_Ptr wrapping an instance of DataArrayTemplate<T>
     */
    static Pointer FromStdVector(std::vector<T>& vec, const QString& name, bool allocate = true)
    {
      QVector<size_t> cDims(1, 1);
      Pointer p = CreateArray(vec.size(), cDims, name, allocate);
      if (NULL != p.get())
      {
        ::memcpy(p->getPointer(0), &(vec.front()), vec.size() * sizeof(T));
      }
      return p;
    }

    /**
     * @brief FromPointer
     * @param data
     * @param size
     * @param name
     * @return
     */
    static Pointer FromPointer(T* data, size_t size, const QString& name, bool allocate = true)
    {
      Pointer p = CreateArray(size, name, allocate);
      if (NULL != p.get())
      {
        ::memcpy(p->getPointer(0), data, size * sizeof(T));
      }
      return p;
    }


    /**
     * @brief WrapPointer
     * @param data
     * @param numTuples
     * @param cDims
     * @param name
     * @param ownsData
     * @return
     */
    static Pointer WrapPointer(T* data, size_t numTuples, QVector<size_t> cDims, const QString& name, bool ownsData)
    {
      // Allocate on the heap
      DataArray* d = new DataArray(numTuples, cDims, name, false);
      // Wrap that heap pointer with a shared_pointer from boost to make it reference counted
      Pointer p(d);

      p->m_Array = data; // Now set the internal array to the raw pointer
      p->m_OwnsData = ownsData; // Set who owns the data, i.e., who is going to "free" the memory
      if (NULL != data) { p->m_IsAllocated = true; }

      return p;
    }


    /**
     * @brief copyIntoArray
     * @param dest
     */
    bool copyIntoArray(Pointer dest)
    {
      if(m_IsAllocated == true && dest->isAllocated() && m_Array && dest->getPointer(0))
      {
        size_t totalBytes = m_Size * sizeof(T);
        ::memcpy(dest->getPointer(0), m_Array, totalBytes);
        return true;
      }
      return false;
    }

    /**
     * @brief createNewArray
     * @param numTuples
     * @param rank
     * @param dims
     * @param name
     * @return
     */
    virtual IDataArray::Pointer createNewArray(size_t numTuples, int rank, size_t* dims, const QString& name, bool allocate = true)
    {
      IDataArray::Pointer p = DataArray<T>::CreateArray(numTuples, rank, dims, name, allocate);
      return p;
    }

    /**
     * @brief createNewArray
     * @param numTuples
     * @param dims
     * @param name
     * @return
     */
    virtual IDataArray::Pointer createNewArray(size_t numTuples, std::vector<size_t> dims, const QString& name, bool allocate = true)
    {
      IDataArray::Pointer p = DataArray<T>::CreateArray(numTuples, dims, name, allocate);
      return p;
    }

    /**
     * @brief createNewArray
     * @param numTuples
     * @param dims
     * @param name
     * @return
     */
    virtual IDataArray::Pointer createNewArray(size_t numTuples, QVector<size_t> dims, const QString& name, bool allocate = true)
    {
      IDataArray::Pointer p = DataArray<T>::CreateArray(numTuples, dims, name, allocate);
      return p;
    }

    /**
     * @brief Destructor
     */
    virtual ~DataArray()
    {
      //qDebug() << "~DataArrayTemplate '" << m_Name << "'" ;
      if ((NULL != m_Array) && (true == m_OwnsData))
      {
        _deallocate();
      }
    }

    /**
     * @brief isAllocated
     * @return
     */
    virtual bool isAllocated() { return m_IsAllocated; }

    /**
     * @brief Gives this array a human readable name
     * @param name The name of this array
     */
    virtual void setInitValue(T initValue)
    {
      m_InitValue = initValue;
    }

    /**
     * @brief Gives this array a human readable name
     * @param name The name of this array
     */
    virtual void setName(const QString& name)
    {
      m_Name = name;
    }

    /**
     * @brief Returns the human readable name of this array
     * @return
     */
    virtual QString getName()
    {
      return m_Name;
    }

    /**
     * @brief Makes this class responsible for freeing the memory
     */
    virtual void takeOwnership()
    {
      m_OwnsData = true;
    }

    /**
     * @brief This class will NOT free the memory associated with the internal pointer.
     * This can be useful if the user wishes to keep the data around after this
     * class goes out of scope.
     */
    virtual void releaseOwnership()
    {
      m_OwnsData = false;
    }

    /**
     * @brief Allocates the memory needed for this class
     * @return 1 on success, -1 on failure
     */
    virtual int32_t allocate()
    {
      if ((NULL != m_Array) && (true == m_OwnsData))
      {
        _deallocate();
      }
      m_Array = NULL;
      m_OwnsData = true;
      m_IsAllocated = false;
      if (m_Size == 0)
      {
        clear();
        return 1;
      }


      size_t newSize = m_Size;
#if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
      m_Array = static_cast<T*>( _mm_malloc (newSize * sizeof(T), 16) );
#else
      m_Array = (T*)malloc(newSize * sizeof(T));
#endif
      if (!m_Array)
      {
        qDebug() << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. " ;
        return -1;
      }
      m_Size = newSize;
      m_IsAllocated = true;

      return 1;
    }


    /**
     * @brief Removes all elements from the array (which are destroyed), leaving the container with a size of 0.
     */
    virtual void clear()
    {
      if (NULL != m_Array && true == m_OwnsData)
      {
        _deallocate();
      }
      m_Array = NULL;
      m_Size = 0;
      m_OwnsData = true;
      m_MaxId = 0;
      m_IsAllocated = false;
      m_NumTuples = 0;
      // We need to actually keep the numComps and the dimensions in case the user resizes the array
      //      m_CompDims.clear();
      //      m_NumComponents = 0;
    }

    /**
     * @brief Sets all the values to zero.
     */
    virtual void initializeWithZeros()
    {
      if(!m_IsAllocated) { return; }
      size_t typeSize = sizeof(T);
      ::memset(m_Array, 0, m_Size * typeSize);
    }

    /**
     * @brief Sets all the values to value.
     */
    virtual void initializeWithValue(T initValue, size_t offset = 0)
    {
      if(!m_IsAllocated) { return; }
      for (size_t i = offset; i < m_Size; i++)
      {
        m_Array[i] = initValue;
      }
    }

    /**
     * @brief Removes Tuples from the m_Array. If the size of the vector is Zero nothing is done. If the size of the
     * vector is greater than or Equal to the number of Tuples then the m_Array is Resized to Zero. If there are
     * indices that are larger than the size of the original (before erasing operations) then an error code (-100) is
     * returned from the program.
     * @param idxs The indices to remove
     * @return error code.
     */
    virtual int eraseTuples(QVector<size_t>& idxs)
    {

      int err = 0;

      // If nothing is to be erased just return
      if(idxs.size() == 0)
      {
        return 0;
      }
      size_t idxs_size = static_cast<size_t>(idxs.size());
      if (idxs_size >= getNumberOfTuples() )
      {
        resize(0);
        return 0;
      }

      // Sanity Check the Indices in the vector to make sure we are not trying to remove any indices that are
      // off the end of the array and return an error code.
      for(QVector<size_t>::size_type i = 0; i < idxs.size(); ++i)
      {
        if (idxs[i] * m_NumComponents > m_MaxId) { return -100; }
      }

      // Calculate the new size of the array to copy into
      size_t newSize = (getNumberOfTuples() - idxs.size()) * m_NumComponents ;

      // Create a new m_Array to copy into
      T* newArray = (T*)malloc(newSize * sizeof(T));
      // Splat AB across the array so we know if we are copying the values or not
      ::memset(newArray, 0xAB, newSize * sizeof(T));

      // Keep the current Destination Pointer
      T* currentDest = newArray;
      size_t j = 0;
      int k = 0;
      // Find the first chunk to copy by walking the idxs array until we get an
      // index that is NOT a continuous increment from the start
      for (k = 0; k < idxs.size(); ++k)
      {
        if(j == idxs[k])
        {
          ++j;
        }
        else
        {
          break;
        }
      }

      if(k == idxs.size()) // Only front elements are being dropped
      {
        T* currentSrc = m_Array + (j * m_NumComponents);
        ::memcpy(currentDest, currentSrc, (getNumberOfTuples() - idxs.size()) * m_NumComponents * sizeof(T));
        _deallocate(); // We are done copying - delete the current m_Array
        m_Size = newSize;
        m_Array = newArray;
        m_OwnsData = true;
        m_MaxId = newSize - 1;
        m_IsAllocated = true;
        return 0;
      }

      QVector<size_t> srcIdx(idxs.size() + 1);
      QVector<size_t> destIdx(idxs.size() + 1);
      QVector<size_t> copyElements(idxs.size() + 1);
      srcIdx[0] = 0;
      destIdx[0] = 0;
      copyElements[0] = (idxs[0] - 0) * m_NumComponents;

      for (int i = 1; i < srcIdx.size(); ++i)
      {
        srcIdx[i] = (idxs[i - 1] + 1) * m_NumComponents;

        if(i < srcIdx.size() - 1)
        {
          copyElements[i] = (idxs[i] - idxs[i - 1] - 1) * m_NumComponents;
        }
        else
        {
          copyElements[i] = (getNumberOfTuples() - idxs[i - 1] - 1) * m_NumComponents;
        }
        destIdx[i] = copyElements[i - 1] + destIdx[i - 1];
      }

      // Copy the data
      for (int i = 0; i < srcIdx.size(); ++i)
      {
        currentDest = newArray + destIdx[i];
        T* currentSrc = m_Array + srcIdx[i];
        size_t bytes = copyElements[i] * sizeof(T);
        ::memcpy(currentDest, currentSrc, bytes);
      }

      // We are done copying - delete the current m_Array
      _deallocate();

      // Allocation was successful.  Save it.
      m_Size = newSize;
      m_Array = newArray;
      // This object has now allocated its memory and owns it.
      m_OwnsData = true;
      m_IsAllocated = true;
      m_MaxId = newSize - 1;

      return err;
    }

    /**
     * @brief
     * @param currentPos
     * @param newPos
     * @return
     */
    virtual int copyTuple(size_t currentPos, size_t newPos)
    {
      size_t max =  ((m_MaxId + 1) / m_NumComponents);
      if (currentPos >= max
          || newPos >= max )
      {return -1;}
      T* src = m_Array + (currentPos * m_NumComponents);
      T* dest = m_Array + (newPos * m_NumComponents);
      size_t bytes = sizeof(T) * m_NumComponents;
      ::memcpy(dest, src, bytes);
      return 0;
    }

    virtual IDataArray::Pointer reorderCopy(QVector<size_t> newOrderMap)
    {
      if(newOrderMap.size() != static_cast<QVector<size_t>::size_type>(getNumberOfTuples()))
      {
        return IDataArray::NullPointer();
      }
      IDataArray::Pointer daCopy = createNewArray(getNumberOfTuples(), getComponentDimensions(), getName(), m_IsAllocated);
      if(m_IsAllocated == true)
      {
        daCopy->initializeWithZeros();
        size_t chunkSize = getNumberOfComponents() * sizeof(T);
        for(size_t i = 0; i < getNumberOfTuples(); i++)
        {
          T* src = getPointer(i * getNumberOfComponents());
          void* dest = daCopy->getVoidPointer(newOrderMap[i] * getNumberOfComponents());
          ::memcpy(dest, src, chunkSize);
        }
      }
      return daCopy;
    }

    /**
     * @brief Returns the number of bytes that make up the data type.
     * 1 = char
     * 2 = 16 bit integer
     * 4 = 32 bit integer/Float
     * 8 = 64 bit integer/Double
     */
    virtual size_t getTypeSize()
    {
      return sizeof(T);
    }


    /**
     * @brief Returns the number of elements in the internal array.
     */
    virtual size_t getNumberOfTuples()
    {
      return m_NumTuples;
    }

    /**
    * @brief Returns the total number of elements that make up this array. Equal to NumTuples * NumComponents
    */
    virtual size_t getSize()
    {
      return m_Size;
    }

    /**
    * @brief Returns the dimensions for the data residing at each Tuple. For example if you have a simple Scalar value
    * at each tuple then this will return a single element QVector. If you have a 1x3 array (like EUler Angles) then
    * this will return a 3 Element QVector.
    */
    virtual QVector<size_t> getComponentDimensions()
    {
      return m_CompDims;
    }

    /**
    * @brief Returns the number component values at each Tuple location. For example if you have a
    * 3 element component (vector) then this will be 3. If you are storing a small image of size 80x60
    * at each Tuple (like EBSD Kikuchi patterns) then the result would be 4800.
    */
    virtual int getNumberOfComponents()
    {
      return m_NumComponents;
    }


    /**
     * @brief Returns a void pointer pointing to the index of the array. NULL
     * pointers are entirely possible. No checks are performed to make sure
     * the index is with in the range of the internal data array.
     * @param i The index to have the returned pointer pointing to.
     * @return Void Pointer. Possibly NULL.
     */
    virtual void* getVoidPointer(size_t i)
    {
      if (i >= m_Size) { return NULL;}

      return (void*)(&(m_Array[i]));
    }


    /**
     * @brief Returns the pointer to a specific index into the array. No checks are made
     * as to the correctness of the index being passed in. If you ask for an index off
     * then end of the array they you will likely cause your program to abort.
     * @param i The index to return the pointer to.
     * @return The pointer to the index
     */
    virtual T* getPointer(size_t i)
    {
#ifndef NDEBUG
      if (m_Size > 0) { BOOST_ASSERT(i < m_Size);}
#endif
      return (T*)(&(m_Array[i]));
    }

    /**
     * @brief Returns the value for a given index
     * @param i The index to return the value at
     * @return The value at index i
     */
    virtual T getValue(size_t i)
    {
#ifndef NDEBUG
      if (m_Size > 0) { BOOST_ASSERT(i < m_Size);}
#endif
      return m_Array[i];
    }

    /**
     * @brief Sets a specific value in the array
     * @param i The index of the value to set
     * @param value The new value to be set at the specified index
     */
    void setValue(size_t i, T value)
    {
#ifndef NDEBUG
      if (m_Size > 0)
      { BOOST_ASSERT(i < m_Size);}
#endif
      m_Array[i] = value;
    }

    //----------------------------------------------------------------------------
    // These can be overridden for more efficiency
    T getComponent(size_t i, int j)
    {
#ifndef NDEBUG
      if (m_Size > 0) { BOOST_ASSERT(i * m_NumComponents + j < m_Size);}
#endif
      return m_Array[i * m_NumComponents + j];
    }

    /**
     * @brief Sets a specific component of the Tuple located at i
     * @param i The index of the Tuple
     * @param j The Component index into the Tuple
     * @param c The value to set
     */
    void setComponent(size_t i, int j, T c)
    {
#ifndef NDEBUG
      if (m_Size > 0) { BOOST_ASSERT(i * m_NumComponents + j < m_Size);}
#endif
      m_Array[i * m_NumComponents + j] = c;
    }

    /**
     * @brief Splats the same value c across all values in the Tuple
     * @param i The index of the Tuple
     * @param c The value to splat across all components in the tuple
     */
    void initializeTuple(size_t i, double p)
    {
      if(!m_IsAllocated) { return; }
#ifndef NDEBUG
      if (m_Size > 0) { BOOST_ASSERT(i * m_NumComponents < m_Size);}
#endif
      T c = static_cast<T>(p);
      for (size_t j = 0; j < m_NumComponents; ++j)
      {
        m_Array[i * m_NumComponents + j] = c;
      }
    }

    /**
     * @brief getTuplePointer Returns the pointer to a specific tuple
     * @param tupleIndex The index of tuple
     */
    T* getTuplePointer(size_t tupleIndex)
    {
#ifndef NDEBUG
      if (m_Size > 0) { BOOST_ASSERT(tupleIndex * m_NumComponents < m_Size);}
#endif
      return m_Array + (tupleIndex * m_NumComponents);
    }

    /**
     * @brief resize
     * @param numTuples
     * @return
     */
    virtual int32_t resize(size_t numTuples)
    {
      int32_t check = resizeTotalElements(numTuples * m_NumComponents);
      if(check > 0) { m_NumTuples = numTuples; }
      return check;
    }

    /**
     * @brief printTuple
     * @param out
     * @param i
     * @param delimiter
     */
    virtual void printTuple(QTextStream& out, size_t i, char delimiter = ',')
    {
      for(size_t j = 0; j < m_NumComponents; ++j)
      {
        if (j != 0) { out << delimiter; }
        out << m_Array[i * m_NumComponents + j];
      }
    }

    /**
     * @brief printComponent
     * @param out
     * @param i
     * @param j
     */
    virtual void printComponent(QTextStream& out, size_t i, int j)
    {
      out << m_Array[i * m_NumComponents + j];
    }

    /**
    * @brief Returns the HDF Type for a given primitive value.
     * @param value A value to use. Can be anything. Just used to get the type info
     * from
     * @return The HDF5 native type for the value
     */
    QString getFullNameOfClass()
    {
      QString theType = getTypeAsString();
      theType = "DataArray<" + theType + ">";
      return theType;
    }

    /**
     * @brief getTypeAsString
     * @return
     */
    QString getTypeAsString()
    {
      T value = static_cast<T>(0);
      if (typeid(value) == typeid(float)) { return "float"; }
      if (typeid(value) == typeid(double)) { return "double"; }

      if (typeid(value) == typeid(int8_t)) { return "int8_t"; }
      if (typeid(value) == typeid(uint8_t)) { return "uint8_t"; }
# if CMP_TYPE_CHAR_IS_SIGNED
      if (typeid(value) == typeid(char)) { return "char"; }
#else
      if (typeid(value) == typeid(char)) { return "char"; }
#endif
      if (typeid(value) == typeid(signed char)) { return "signed char"; }
      if (typeid(value) == typeid(unsigned char)) { return "unsigned char"; }


      if (typeid(value) == typeid(int16_t)) { return "int16_t"; }
      if (typeid(value) == typeid(short)) { return "short"; }
      if (typeid(value) == typeid(signed short)) { return "signed short"; }
      if (typeid(value) == typeid(uint16_t)) { return "uint16_t"; }
      if (typeid(value) == typeid(unsigned short)) { return "unsigned short"; }


      if (typeid(value) == typeid(int32_t)) { return "int32_t"; }
      if (typeid(value) == typeid(uint32_t)) { return "uint32_t"; }
#if (CMP_SIZEOF_INT == 4)
      if (typeid(value) == typeid(int)) { return "int"; }
      if (typeid(value) == typeid(signed int)) { return "signed int"; }
      if (typeid(value) == typeid(unsigned int)) { return "unsigned int"; }
#endif

      if (typeid(value) == typeid(int64_t)) { return "int64_t"; }
      if (typeid(value) == typeid(uint64_t)) { return "uint64_t";}

#if (CMP_SIZEOF_LONG == 4)
      if (typeid(value) == typeid(long int)) { return "long int"; }
      if (typeid(value) == typeid(signed long int)) { return "signed long int"; }
      if (typeid(value) == typeid(unsigned long int)) { return "unsigned long int"; }
#elif (CMP_SIZEOF_LONG == 8)
      if (typeid(value) == typeid(long int)) { return "long int"; }
      if (typeid(value) == typeid(signed long int)) { return "signed long int"; }
      if (typeid(value) == typeid(unsigned long int)) { return "unsigned long int"; }
#endif


#if (CMP_SIZEOF_LONG_LONG == 8)
      if (typeid(value) == typeid(long long int)) { return "long long int"; }
      if (typeid(value) == typeid(signed long long int)) { return "signed long long int"; }
      if (typeid(value) == typeid(unsigned long long int)) { return "unsigned long long int"; }
#endif


      if (typeid(value) == typeid(bool)) { return "bool"; }

      // qDebug()  << "Error: HDFTypeForPrimitive - Unknown Type: " << (typeid(value).name()) ;
      const char* name = typeid(value).name();
      if (NULL != name && name[0] == 'l' )
      {
        qDebug() << "You are using 'long int' as a type which is not 32/64 bit safe. Suggest you use one of the H5SupportTypes defined in <Common/H5SupportTypes.h> such as int32_t or uint32_t." ;
      }
      return "UnknownType";
    }

    /**
     * @brief deepCopy
     * @param forceNoAllocate
     * @return
     */
    virtual IDataArray::Pointer deepCopy(bool forceNoAllocate = false)
    {
      IDataArray::Pointer daCopy = createNewArray(getNumberOfTuples(), getComponentDimensions(), getName(), m_IsAllocated);
      if(m_IsAllocated == true && forceNoAllocate == false)
      {
        T* src = getPointer(0);
        void* dest = daCopy->getVoidPointer(0);
        size_t totalBytes = (getNumberOfTuples() * getNumberOfComponents() * sizeof(T));
        ::memcpy(dest, src, totalBytes);
      }
      return daCopy;
    }



    /**
     *
     * @param parentId
     * @return
     */
    virtual int writeH5Data(hid_t parentId, QVector<size_t> tDims)
    {
      if (m_Array == NULL)
      { return -85648; }
#if 0
      return H5DataArrayWriter<T>::writeArray(parentId, getName(), getNumberOfTuples(), getNumberOfComponents(), getRank(), getDims(), getClassVersion(), m_Array, getFullNameOfClass());
#else
      return H5DataArrayWriter::writeDataArray<Self>(parentId, this, tDims);
#endif
    }

    /**
     * @brief writeXdmfAttribute
     * @param out
     * @param volDims
     * @return
     */
    virtual int writeXdmfAttribute(QTextStream& out, int64_t* volDims, const QString& hdfFileName,
                                   const QString& groupPath, const QString& label)
    {
      if (m_Array == NULL) { return -85648; }
      QString dimStr;
      int precision = 0;
      QString xdmfTypeName;
      getXdmfTypeAndSize(xdmfTypeName, precision);
      if (0 == precision)
      {
        out << "<!-- " << getName() << " has unknown type or unsupported type or precision for XDMF to understand" << " -->" << "\n";
        return -100;
      }

      int numComp = getNumberOfComponents();
      out << "    <Attribute Name=\"" << getName() << label  << "\" ";
      if (numComp == 1)
      {
        out << "AttributeType=\"Scalar\" ";
        dimStr = QString("%1 %2 %3 ").arg(volDims[2]).arg(volDims[1]).arg(volDims[0]);
      }
      else if( numComp == 6)
      {
        out << "AttributeType=\"Tensor6\" ";
        dimStr = QString("%1 %2 %3 %4 ").arg(volDims[2]).arg(volDims[1]).arg(volDims[0]).arg(numComp);
      }
      else if( numComp == 9)
      {
        out << "AttributeType=\"Tensor\" ";
        dimStr = QString("%1 %2 %3 %4 ").arg(volDims[2]).arg(volDims[1]).arg(volDims[0]).arg(numComp);
      }
      else
      {
        out << "AttributeType=\"Vector\" ";
        dimStr = QString("%1 %2 %3 %4 ").arg(volDims[2]).arg(volDims[1]).arg(volDims[0]).arg(numComp);
      }
      out << "Center=\"Cell\">\n" ;
      // Open the <DataItem> Tag
      out << "      <DataItem Format=\"HDF\" Dimensions=\"" << dimStr <<  "\" ";
      out << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >\n" ;


      out << "        " << hdfFileName << groupPath << "/" << getName() << "\n";
      out << "      </DataItem>" << "\n";
      out << "    </Attribute>" << "\n";
      return 1;
    }


    /**
     * @brief getInfoString
     * @return Returns a formatted string that contains general infomation about
     * the instance of the object.
     */
    virtual QString getInfoString(DREAM3D::InfoStringFormat format)
    {

      QLocale usa(QLocale::English, QLocale::UnitedStates);

      QString info;
      QTextStream ss (&info);
      if(format == DREAM3D::HtmlFormat)
      {
        ss << "<html><head></head>\n";
        ss << "<body>\n";
        ss << "<table cellpadding=\"4\" cellspacing=\"0\" border=\"0\">\n";
        ss << "<tbody>\n";
        ss << "<tr bgcolor=\"#D3D8E0\"><th colspan=2>Attribute Array Info</th></tr>";

        ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Name:</th><td>" << getName() << "</td></tr>";


        ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Type:</th><td> DataArray&lt;" << getTypeAsString() << "&gt;</td></tr>";
        QString numStr = usa.toString(static_cast<qlonglong>(getNumberOfTuples() ));
        ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Number of Tuples:</th><td>" << numStr << "</td></tr>";

        QString compDimStr = "(";
        for(int i = 0; i < m_CompDims.size(); i++)
        {
          compDimStr = compDimStr + QString::number(m_CompDims[i]);
          if(i < m_CompDims.size() - 1)
          {
            compDimStr = compDimStr + QString(", ");
          }
        }
        compDimStr = compDimStr + ")";
        ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Component Dimensions:</th><td>" << compDimStr << "</td></tr>";
        numStr = usa.toString(static_cast<qlonglong>(m_Size));
        ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Total Elements:</th><td>" << numStr << "</td></tr>";
        numStr = usa.toString(static_cast<qlonglong>(m_Size * sizeof(T)));
        ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Total Memory Required:</th><td>" << numStr << "</td></tr>";
        ss << "</tbody></table>\n";
        ss << "<br/>";
        ss << "</body></html>";
      }
      else
      {

      }
      return info;
    }

    /**
     * @brief
     * @param parentId
     * @return
     */
    virtual int readH5Data(hid_t parentId)
    {
      int err = 0;

      resize(0);
      IDataArray::Pointer p = H5DataArrayReader::ReadIDataArray(parentId, getName());
      if (p.get() == NULL)
      {
        return -1;
      }
      m_Array = reinterpret_cast<T*>(p->getVoidPointer(0));
      m_Size = p->getSize();
      m_OwnsData = true;
      m_MaxId = (m_Size == 0) ? 0 : m_Size - 1;
      m_IsAllocated = true;
      m_Name = p->getName();
      m_NumTuples = p->getNumberOfTuples();
      m_CompDims = p->getComponentDimensions();
      m_NumComponents = p->getNumberOfComponents();

      // Tell the intermediate DataArray to release ownership of the data as we are going to be responsible
      // for deleting the memory
      p->releaseOwnership();
      return err;
    }

    /**
     * @brief
     */
    virtual void byteSwapElements()
    {
      char* ptr = (char*)(m_Array);
      char t[8];
      size_t size = getTypeSize();
      for (uint64_t var = 0; var < m_Size; ++var)
      {
        if (sizeof(T) == 2)
        {
          mxa_bswap(0, 1, t);
        }
        else if (sizeof(T) == 4)
        {
          mxa_bswap(0, 3, t);
          mxa_bswap(1, 2, t);
        }
        else if (sizeof(T) == 8)
        {
          mxa_bswap(0, 7, t);
          mxa_bswap(1, 6, t);
          mxa_bswap(2, 5, t);
          mxa_bswap(3, 4, t);
        }
        ptr += size; // increment the pointer
      }
    }

    /**
      * @brief operator []
      * @param i
      * @return
      */
    inline T& operator[](size_t i)
    {
      BOOST_ASSERT(i < m_Size);
      return m_Array[i];
    }

  protected:
    /**
    * @brief Protected Constructor
    * @param numTuples The number of elements in the internal array.
    * @param rank The number of dimensions the attribute on each Tuple has.
    * @param dims The actual dimensions the attribute on each Tuple has.
    * @param takeOwnership Will the class clean up the memory. Default=true
    */
    DataArray(size_t numTuples, QVector<size_t> compDims, const QString& name, bool ownsData = true) :
      m_Array(NULL),
      m_OwnsData(ownsData),
      m_IsAllocated(false),
      m_Name(name),
      m_NumTuples(numTuples)
    {
      // Set the Component Dimensions and compute the number of components at each tuple for caching
      m_CompDims = compDims;
      m_NumComponents = m_CompDims[0];
      for (int i = 1; i < m_CompDims.size(); i++)
      {
        m_NumComponents = m_NumComponents * m_CompDims[i];
      }

      m_Size = m_NumTuples * m_NumComponents;
      m_MaxId = (m_Size > 0) ? m_Size - 1 : m_Size;

      m_InitValue = static_cast<T>(0);
      //  MUD_FLAP_0 = MUD_FLAP_1 = MUD_FLAP_2 = MUD_FLAP_3 = MUD_FLAP_4 = MUD_FLAP_5 = 0xABABABABABABABABul;
    }

    /**
     * @brief deallocates the memory block
     */
    void _deallocate()
    {
      // We are going to splat 0xABABAB across the first value of the array as a debugging aid
      unsigned char* cptr = reinterpret_cast<unsigned char*>(m_Array);
      if(NULL != cptr)
      {
        if(m_Size > 0)
        {
          if (sizeof(T) >= 1) { cptr[0] = 0xAB; }
          if (sizeof(T) >= 2) { cptr[1] = 0xAB; }
          if (sizeof(T) >= 4) { cptr[2] = 0xAB; cptr[3] = 0xAB;}
          if (sizeof(T) >= 8) { cptr[4] = 0xAB; cptr[5] = 0xAB; cptr[6] = 0xAB; cptr[7] = 0xAB;}
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
        BOOST_ASSERT(false);
      }
#endif

#if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
      _mm_free( m_buffer );
#else
      free(m_Array);
#endif
      m_Array = NULL;
      m_IsAllocated = false;
    }

    /**
     * @brief Resizes the internal array
     * @param size The new size of the internal array
     * @return 1 on success, 0 on failure
     */
    virtual int32_t resizeTotalElements(size_t size)
    {
      // std::cout << "DataArray::resizeTotalElements(" << size << ")" << std::endl;
      if (size == 0)
      {
        clear();
        return 1;
      }
      T* ptr = resizeAndExtend(size);
      if ( NULL != ptr)
      {
        return 1;
      }
      else
      {
        return 0;
      }
    }


    /**
     * @brief resizes the internal array to be 'size' elements in length
     * @param size
     * @return Pointer to the internal array
     */
    virtual T* resizeAndExtend(size_t size)
    {
      T* newArray;
      size_t newSize;
      size_t oldSize;

      if (size == m_Size) // Requested size is equal to current size.  Do nothing.
      {
        return m_Array;
      }
      newSize = size;
      oldSize = m_Size;

      // Wipe out the array completely if new size is zero.
      if (newSize == 0)
      {
        clear();
        return m_Array;
      }
      // OS X's realloc does not free memory if the new block is smaller.  This
      // is a very serious problem and causes huge amount of memory to be
      // wasted. Do not use realloc on the Mac.
      bool dontUseRealloc = false;
#if defined __APPLE__
      dontUseRealloc = true;
#endif

      // Allocate a new array if we DO NOT own the current array
      if ((NULL != m_Array) && (false == m_OwnsData))
      {
        // The old array is owned by the user so we cannot try to
        // reallocate it.  Just allocate new memory that we will own.
        newArray = (T*)malloc(newSize * sizeof(T));
        if (!newArray)
        {
          qDebug() << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. " ;
          return NULL;
        }

        // Copy the data from the old array.
        memcpy(newArray, m_Array, (newSize < m_Size ? newSize : m_Size) * sizeof(T));
      }
      else if (!dontUseRealloc)
      {
        // Try to reallocate with minimal memory usage and possibly avoid copying.
        newArray = (T*)realloc(m_Array, newSize * sizeof(T));
        if (!newArray)
        {
          qDebug() << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. " ;
          return NULL;
        }
      }
      else
      {
        newArray = (T*)malloc(newSize * sizeof(T));
        if (!newArray)
        {
          qDebug() << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. " ;
          return NULL;
        }

        // Copy the data from the old array.
        if (m_Array != NULL)
        {
          memcpy(newArray, m_Array, (newSize < m_Size ? newSize : m_Size) * sizeof(T));
        }
        // Free the old array
        _deallocate();
      }

      // Allocation was successful.  Save it.
      m_Size = newSize;
      m_Array = newArray;

      // This object has now allocated its memory and owns it.
      m_OwnsData = true;

      m_MaxId = newSize - 1;
      m_IsAllocated = true;

      // Initialize the new tuples if newSize is larger than old size
      if(newSize > oldSize)
      {
        initializeWithValue(m_InitValue, oldSize);
      }

      return m_Array;
    }


  private:

    //  unsigned long long int MUD_FLAP_0;
    T* m_Array;
    //  unsigned long long int MUD_FLAP_1;
    size_t m_Size;
    //  unsigned long long int MUD_FLAP_4;
    bool m_OwnsData;
    //  unsigned long long int MUD_FLAP_2;
    size_t m_MaxId;

    bool m_IsAllocated;
    //   unsigned long long int MUD_FLAP_3;
    QString m_Name;
    //  unsigned long long int MUD_FLAP_5;

    size_t m_NumTuples;

    QVector<size_t> m_CompDims;
    size_t m_NumComponents;

    T m_InitValue;

    DataArray(const DataArray&); //Not Implemented
    void operator=(const DataArray&); //Not Implemented

};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

typedef DataArray<bool> BoolArrayType;

typedef DataArray<unsigned char> UCharArrayType;

typedef DataArray<int8_t>  Int8ArrayType;
typedef DataArray<uint8_t>  UInt8ArrayType;

typedef DataArray<int16_t>  Int16ArrayType;
typedef DataArray<uint16_t>  UInt16ArrayType;

typedef DataArray<int32_t>  Int32ArrayType;
typedef DataArray<uint32_t>  UInt32ArrayType;

typedef DataArray<int64_t>  Int64ArrayType;
typedef DataArray<uint64_t>  UInt64ArrayType;

typedef DataArray<float>  FloatArrayType;
typedef DataArray<double>  DoubleArrayType;

typedef DataArray<size_t>  SizeTArrayType;

#endif //_DataArray_h_
