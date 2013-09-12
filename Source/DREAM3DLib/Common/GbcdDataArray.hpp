/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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

 //
 //  This code was written under United States Air Force Contract number
 //                           FA8650-07-D-5800
 //
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _GbcdDataArray_h_
#define _GbcdDataArray_h_

// STL Includes
#include <vector>
#include <sstream>
#include <fstream>





#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArrayFilter.h"
#include "DREAM3DLib/Common/IDataArray.h"
#include "DREAM3DLib/HDF5/H5DataArrayWriter.hpp"
#include "DREAM3DLib/HDF5/H5DataArrayReader.h"
#include "DREAM3DLib/HDF5/H5GbcdArrayReader.h"


/**
 * @class GbcdDataArray GbcdDataArray.hpp DREAM3DLib/Common/GbcdDataArray.hpp
 * @brief Template class for wrapping raw arrays of data.
 * @author mjackson
 * @date July 3, 2008
 * @version $Revision: 1.2 $
 */
template<typename T>
class GbcdDataArray : public IDataArray
{
  public:

    DREAM3D_SHARED_POINTERS(GbcdDataArray<T> )
    DREAM3D_TYPE_MACRO_SUPER(GbcdDataArray<T>, IDataArray)

    typedef QVector<Pointer>   ContainterType;

  enum NumType {
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
     * @brief Static constructor
     * @param numElements The number of elements in the internal array.
     * @param name The name of the array
     * @return Boost::Shared_Ptr wrapping an instance of GbcdDataArrayTemplate<T>
     */
    static Pointer CreateArray(size_t* dims, const QString &name, bool allocateArray = true)
    {
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }
      GbcdDataArray<T>* d = new GbcdDataArray<T> (dims, true);
      if (allocateArray && d->Allocate() < 0)
      { // Could not allocate enough memory, reset the pointer to null and return
        delete d;
        return GbcdDataArray<T>::NullPointer();
      }
      d->SetName(name);
      Pointer ptr(d);
      return ptr;
    }

    /**
     * @brief Static constructor
     * @param numTuples The number of tuples in the array.
     * @param numComponents The number of Components in each Tuple
     * @param name The name of the array
     * @return Boost::Shared_Ptr wrapping an instance of GbcdDataArrayTemplate<T>
     */
    static Pointer CreateArray(size_t dim0, size_t dim1, size_t dim2, size_t dim3, size_t dim4,
                              const QString &name, bool allocateArray = true)
    {
      GbcdDataArray<T>* d = new GbcdDataArray<T> (dim0, dim1, dim2, dim3, dim4, true);
      if (allocateArray && d->Allocate() < 0)
      { // Could not allocate enough memory, reset the pointer to null and return
        delete d;
        return GbcdDataArray<T>::NullPointer();
      }
      d->SetName(name);
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
    virtual IDataArray::Pointer createNewArray(size_t numElements, int numComponents, const QString &name)
    {
      IDataArray::Pointer p = GbcdDataArray<T>::CreateArray(numElements, numComponents, 1, 1, 1, name);
      return p;
    }


    /**
     * @brief Destructor
     */
    virtual ~GbcdDataArray()
    {
      //qDebug() << "~GbcdDataArrayTemplate '" << m_Name << "'" ;
      if ((NULL != this->Array) && (true == this->_ownsData))
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
     * @brief GetGbcdDimension Returns through the passed in argument the sizes of the GBCD Array
     * @param dims The 1x5 array that will hold the values of the GBCD dimensions
     */
    virtual void GetGbcdDimension(size_t* dims)
    {
      dims[0] = m_Dims[0];
      dims[1] = m_Dims[1];
      dims[2] = m_Dims[2];
      dims[3] = m_Dims[3];
      dims[4] = m_Dims[4];
    }

    virtual void GetGbcdDimension(size_t &dim0, size_t& dim1, size_t& dim2, size_t& dim3, size_t& dim4)
    {
      dim0 = m_Dims[0];
      dim1 = m_Dims[1];
      dim2 = m_Dims[2];
      dim3 = m_Dims[3];
      dim4 = m_Dims[4];
    }


    /**
     * @brief GetTypeName Returns a string representation of the type of data that is stored by this class. This
     * can be a primitive like char, float, int or the name of a class.
     * @return
     */
    void GetXdmfTypeAndSize(QString &xdmfTypeName, int &precision)
    {
   //   T value = 0x00;
      xdmfTypeName = "UNKNOWN";
      precision = 0;
//      if (typeid(value) == typeid(int8_t)) { xdmfTypeName = "Char"; precision = 1;}
//      if (typeid(value) == typeid(uint8_t)) { xdmfTypeName = "UChar"; precision = 1;}

//      if (typeid(value) == typeid(int16_t)) { xdmfTypeName = "16 BIT NOT SUPPORTED BY XDMF"; precision = 0;}
//      if (typeid(value) == typeid(uint16_t)) { xdmfTypeName = "16 BIT NOT SUPPORTED BY XDMF"; precision = 0;}

//      if (typeid(value) == typeid(int32_t)) { xdmfTypeName = "Int"; precision = 4;}
//      if (typeid(value) == typeid(uint32_t)) { xdmfTypeName = "UInt"; precision = 4;}

//      if (typeid(value) == typeid(int64_t)) { xdmfTypeName = "Int"; precision = 8;}
//      if (typeid(value) == typeid(uint64_t)) { xdmfTypeName = "UInt"; precision = 8;}

//      if (typeid(value) == typeid(float)) { xdmfTypeName = "Float"; precision = 4;}
//      if (typeid(value) == typeid(double)) { xdmfTypeName = "Float"; precision = 8;}

//      if (typeid(value) == typeid(bool)) { xdmfTypeName = "uchar"; precision = 1;}
    }


    /**
     * @brief GetTypeName Returns a string representation of the type of data that is stored by this class. This
     * can be a primitive like char, float, int or the name of a class.
     * @return
     */

    NumType GetType()
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
     * @brief Gives this array a human readable name
     * @param name The name of this array
     */
    virtual void SetName(const QString &name)
    {
      m_Name = name;
    }

    /**
     * @brief Returns the human readable name of this array
     * @return
     */
    virtual QString GetName()
    {
      return m_Name;
    }

    /**
     * @brief Makes this class responsible for freeing the memory
     */
    virtual void takeOwnership()
    {
      this->_ownsData = true;
    }

    /**
     * @brief This class will NOT free the memory associated with the internal pointer.
     * This can be useful if the user wishes to keep the data around after this
     * class goes out of scope.
     */
    virtual void releaseOwnership()
    {
      this->_ownsData = false;
    }

    /**
     * @brief GetSize
     * @return
     */
    virtual size_t GetSize()
    {
      return Size;
    }

    /**
     * @brief Allocates the memory needed for this class
     * @return 1 on success, -1 on failure
     */
    virtual int32_t Allocate()
    {
      if ((NULL != this->Array) && (true == this->_ownsData))
      {
        _deallocate();
      }
      this->Array = NULL;
      this->_ownsData = true;
      this->m_IsAllocated = false;
      if (this->Size == 0)
      {
        initialize();
        return 1;
      }

      size_t newSize = this->Size;
#if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
      Array = static_cast<T*>( _mm_malloc (newSize * sizeof(T), 16) );
#else
      this->Array = (T*)malloc(newSize * sizeof(T));
#endif
      if (!this->Array)
      {
        qDebug() << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. " ;
        return -1;
      }
      this->Size = newSize;
      this->m_IsAllocated = true;
      return 1;
    }

    /**
     * @brief Initializes this class to zero bytes freeing any data that it currently owns
     */
    virtual void initialize()
    {
      if (NULL != this->Array && true == this->_ownsData)
      {
        _deallocate();
      }
      this->Array = NULL;
      this->Size = 0;
      this->_ownsData = true;
      this->MaxId = 0;
      this->m_IsAllocated = false;
    }

    /**
     * @brief Sets all the values to value.
     */
    virtual void initializeWithValues(T value)
    {
      BOOST_ASSERT(m_IsAllocated == true);
      for (size_t i = 0; i < this->Size; i++)
      {
        this->Array[i] = value;
      }
    }

    /**
     * @brief Sets all the values to zero.
     */
    virtual void initializeWithZeros()
    {
      BOOST_ASSERT(m_IsAllocated == true);
      size_t typeSize = sizeof(T);
      ::memset(this->Array, 0, this->Size * typeSize);
    }

    /**
     * @brief Removes Tuples from the Array. If the size of the vector is Zero nothing is done. If the size of the
     * vector is greater than or Equal to the number of Tuples then the Array is Resized to Zero. If there are
     * indices that are larger than the size of the original (before erasing operations) then an error code (-100) is
     * returned from the program.
     * @param idxs The indices to remove
     * @return error code.
     */
    virtual int EraseTuples(QVector<size_t> &idxs)
    {
      int err = -1;
      return err;
    }

    /**
     * @brief
     * @param currentPos
     * @param newPos
     * @return
     */
    virtual int CopyTuple(size_t currentPos, size_t newPos)
    {
      int err = -1;
      return err;
    }

    /**
     * @brief Returns the number of bytes that make up the data type.
     * 1 = char
     * 2 = 16 bit integer
     * 4 = 32 bit integer/Float
     * 8 = 64 bit integer/Double
     */
    virtual size_t GetTypeSize()
    {
      return sizeof(T);
    }


    /**
     * @brief Returns the number of elements in the internal array.
     */
    virtual size_t getNumberOfTuples()
    {
      if (Size == 0) { return 0; }
      return 1;
    }


    // Description:
    // Set/Get the dimension (n) of the components. Must be >= 1. Make sure that
    // this is set before allocation.
    virtual void SetNumberOfComponents(int nc)
    {
      BOOST_ASSERT(false);
    }

    virtual int GetNumberOfComponents()
    {
      return this->Size;
    }

    /**
     * @brief Returns a void pointer pointing to the index of the array. NULL
     * pointers are entirely possible. No checks are performed to make sure
     * the index is with in the range of the internal data array.
     * @param i The index to have the returned pointer pointing to.
     * @return Void Pointer. Possibly NULL.
     */
    virtual void* GetVoidPointer(size_t i)
    {
      if (i >= Size) { return NULL;}

      return (void*)(&(Array[i]));
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
      if (Size > 0) { BOOST_ASSERT(i < Size);}
#endif
      return (T*)(&(Array[i]));
    }



    /**
     * @brief This function makes no sense for this class and will ASSERT if used
     */
    void InitializeTuple(size_t i, double p)
    {
      BOOST_ASSERT(false);
//#ifndef NDEBUG
//      if (Size > 0) { BOOST_ASSERT(i*NumberOfComponents < Size);}
//#endif
//      T c = static_cast<T>(p);
//      for (int j = 0; j < this->NumberOfComponents; ++j) {
//        Array[i*this->NumberOfComponents + j] = c;
//      }
    }

    /**
     * @brief Resizes the internal array
     * @param size The new size of the internal array
     * @return 1 on success, 0 on failure
     */
    virtual int32_t RawResize(size_t size)
    {
      if (this->ResizeAndExtend(size) || size == 0)
      {
        return 1;
      }
      else
      {
        return 0;
      }
    }

    virtual int32_t Resize(size_t numElements)
    {
      return RawResize(numElements);
    }

    virtual void printTuple(QTextStream &out, size_t i, char delimiter = ',')
    {
      BOOST_ASSERT(false);
    }

    virtual void printComponent(QTextStream &out, size_t i, int j)
    {
      BOOST_ASSERT(false);
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
      theType = "GbcdDataArray<" + theType + ">";
      return theType;
    }

    /**
     * @brief getTypeAsString
     * @return
     */
    QString getTypeAsString()
    {
      T value = static_cast<T>(0);
      if (typeid(value) == typeid(float)) return "float";
      if (typeid(value) == typeid(double)) return "double";

      if (typeid(value) == typeid(int8_t)) return "int8_t";
      if (typeid(value) == typeid(uint8_t)) return "uint8_t";
# if CMP_TYPE_CHAR_IS_SIGNED
      if (typeid(value) == typeid(char)) return "char";
#else
      if (typeid(value) == typeid(char)) return "char";
#endif
      if (typeid(value) == typeid(signed char)) return "signed char";
      if (typeid(value) == typeid(unsigned char)) return "unsigned char";


      if (typeid(value) == typeid(int16_t)) return "int16_t";
      if (typeid(value) == typeid(short)) return "short";
      if (typeid(value) == typeid(signed short)) return "signed short";
      if (typeid(value) == typeid(uint16_t)) return "uint16_t";
      if (typeid(value) == typeid(unsigned short)) return "unsigned short";


      if (typeid(value) == typeid(int32_t)) return "int32_t";
      if (typeid(value) == typeid(uint32_t)) return "uint32_t";
#if (CMP_SIZEOF_INT == 4)
      if (typeid(value) == typeid(int)) return "int";
      if (typeid(value) == typeid(signed int)) return "signed int";
      if (typeid(value) == typeid(unsigned int)) return "unsigned int";
#endif


#if (CMP_SIZEOF_LONG == 4)
      if (typeid(value) == typeid(long int)) return "long int";
      if (typeid(value) == typeid(signed long int)) return "signed long int";
      if (typeid(value) == typeid(unsigned long int)) return "unsigned long int";
#elif (CMP_SIZEOF_LONG == 8)
      if (typeid(value) == typeid(long int)) return "long int";
      if (typeid(value) == typeid(signed long int)) return "signed long int";
      if (typeid(value) == typeid(unsigned long int)) return "unsigned long int";
#endif


#if (CMP_SIZEOF_LONG_LONG == 8)
      if (typeid(value) == typeid(long long int)) return "long long int";
      if (typeid(value) == typeid(signed long long int)) return "signed long long int";
      if (typeid(value) == typeid(unsigned long long int)) return "unsigned long long int";
#endif
      if (typeid(value) == typeid(int64_t)) return "int64_t";
      if (typeid(value) == typeid(uint64_t)) return "uint64_t";

      if (typeid(value) == typeid(bool)) return "bool";

      // qDebug()  << "Error: HDFTypeForPrimitive - Unknown Type: " << (typeid(value).name()) ;
      const char* name = typeid(value).name();
      if (NULL != name && name[0] == 'l' ) {
        qDebug() << "You are using 'long int' as a type which is not 32/64 bit safe. Suggest you use one of the H5SupportTypes defined in <Common/H5SupportTypes.h> such as int32_t or uint32_t." ;
      }
      return "UnknownType";
    }


    /**
     *
     * @param parentId
     * @return
     */
    virtual int writeH5Data(hid_t parentId)
    {
      if (Array == NULL) { return -85648; }
      return H5GBCDArrayWriter<T>::writeArray(parentId, GetName(), m_Dims, Array, getFullNameOfClass());
    }

    /**
     * @brief writeXdmfAttribute
     * @param out
     * @param volDims
     * @return
     */
    virtual int writeXdmfAttribute(QTextStream &out, int64_t* volDims, const QString &hdfFileName, const QString &groupPath,
    const QString &label)
    {
      int err = -1;
      return err;
    }

    /**
     * @brief
     * @param parentId
     * @return
     */
    virtual int readH5Data(hid_t parentId)
    {
      int err = 0;

      this->Resize(0);
      IDataArray::Pointer p = H5GbcdArrayReader::readIDataArray(parentId, GetName());
      if (p.get() == NULL)
      {
        return -1;
      }
      GbcdDataArray<T>* ptr = GbcdDataArray<T>::SafePointerDownCast(p.get());
      if (ptr == NULL)
      {
        return -1;
      }
      ptr->GetGbcdDimension(this->m_Dims);
      this->Size = p->GetSize();
      this->MaxId = (Size == 0) ? 0 : Size -1;
      this->Array = reinterpret_cast<T*>(p->GetVoidPointer(0));
      p->releaseOwnership();

      return err;
    }

   /**
     * @brief operator []
     * @param i
     * @return
     */
    inline T& operator[](size_t i)
    {
      BOOST_ASSERT(i < Size);
      return Array[i];
    }

  protected:


    /**
     * @brief Protected Constructor
     * @param numElements The number of elements in the internal array.
     * @param takeOwnership Will the class clean up the memory. Default=true
     */
    GbcdDataArray(size_t dim0, size_t dim1, size_t dim2, size_t dim3, size_t dim4, bool ownsData = true) :
      Array(NULL),
      _ownsData(ownsData),
      m_IsAllocated(false)
    {
      m_Dims[0] = dim0;
      m_Dims[1] = dim1;
      m_Dims[2] = dim2;
      m_Dims[3] = dim3;
      m_Dims[4] = dim4;
      Size = m_Dims[0] * m_Dims[1] * m_Dims[2] * m_Dims[3] * m_Dims[4];

      MaxId = (Size > 0) ? Size - 1: Size;
      //  MUD_FLAP_0 = MUD_FLAP_1 = MUD_FLAP_2 = MUD_FLAP_3 = MUD_FLAP_4 = MUD_FLAP_5 = 0xABABABABABABABABul;
    }

    /**
     * @brief Protected Constructor
     * @param numTuples The number of elements in the internal array.
     * @param numComponents The number of values each Tuple has. Size = NumTuples * NumComponents
     * @param takeOwnership Will the class clean up the memory. Default=true
     */
    GbcdDataArray(size_t* dims, bool ownsData = true) :
      Array(NULL),
      _ownsData(ownsData),
      m_IsAllocated(false)
    {

      m_Dims[0] = dims[0];
      m_Dims[1] = dims[1];
      m_Dims[2] = dims[2];
      m_Dims[3] = dims[3];
      m_Dims[4] = dims[4];
      Size = m_Dims[0] * m_Dims[1] * m_Dims[2] * m_Dims[3] * m_Dims[4];
      MaxId = (Size > 0) ? Size - 1: Size;
      //  MUD_FLAP_0 = MUD_FLAP_1 = MUD_FLAP_2 = MUD_FLAP_3 = MUD_FLAP_4 = MUD_FLAP_5 = 0xABABABABABABABABul;
    }
    /**
     * @brief deallocates the memory block
     */
    void _deallocate()
    {
      // We are going to splat 0xABABAB across the first value of the array as a debugging aid
      unsigned char* cptr = reinterpret_cast<unsigned char*>(this->Array);
      if (sizeof(T) >= 1 && Size > 0) { cptr[0] = 0xAB; }
      if (sizeof(T) >= 2 && Size > 0) { cptr[1] = 0xAB; }
      if (sizeof(T) >= 4 && Size > 0) { cptr[2] = 0xAB; cptr[3] = 0xAB;}
      if (sizeof(T) >= 8 && Size > 0) { cptr[4] = 0xAB; cptr[5] = 0xAB; cptr[6] = 0xAB; cptr[7] = 0xAB;}
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
      _mm_free( this->m_buffer );
#else
      free(this->Array);
#endif
      this->Array = NULL;
      this->m_IsAllocated = false;
    }

    /**
     * @brief resizes the internal array to be 'size' elements in length
     * @param size
     * @return Pointer to the internal array
     */
    virtual T* ResizeAndExtend(size_t size)
    {
      T* newArray;
      size_t newSize;

      if (size > this->Size)
      {
        newSize = size;
      }
      else if (size == this->Size) // Requested size is equal to current size.  Do nothing.
      {
        return this->Array;
      }
      else // Requested size is smaller than current size.  Squeeze the memory.
      {
        newSize = size;
      }

      // Wipe out the array completely if new size is zero.
      if (newSize == 0)
      {
        this->initialize();
        return this->Array;
      }
      // OS X's realloc does not free memory if the new block is smaller.  This
      // is a very serious problem and causes huge amount of memory to be
      // wasted. Do not use realloc on the Mac.
      bool dontUseRealloc=false;
#if defined __APPLE__
      dontUseRealloc=true;
#endif

      // Allocate a new array if we DO NOT own the current array
      if ((NULL != this->Array) && (false == this->_ownsData))
      {
        // The old array is owned by the user so we cannot try to
        // reallocate it.  Just allocate new memory that we will own.
        newArray = (T*)malloc(newSize * sizeof(T));
        if (!newArray)
        {
          qDebug() << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. " ;
          return 0;
        }

        // Copy the data from the old array.
        memcpy(newArray, this->Array, (newSize < this->Size ? newSize : this->Size) * sizeof(T));
      }
      else if (!dontUseRealloc)
      {
        // Try to reallocate with minimal memory usage and possibly avoid copying.
        newArray = (T*)realloc(this->Array, newSize * sizeof(T));
        if (!newArray)
        {
          qDebug() << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. " ;
          return 0;
        }
      }
      else
      {
        newArray = (T*)malloc(newSize * sizeof(T));
        if (!newArray)
        {
          qDebug() << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. " ;
          return 0;
        }

        // Copy the data from the old array.
        if (this->Array != NULL) {
          memcpy(newArray, this->Array, (newSize < this->Size ? newSize : this->Size) * sizeof(T));
        }
        // Free the old array
        _deallocate();
      }

      // Allocation was successful.  Save it.
      this->Size = newSize;
      this->Array = newArray;
      // This object has now allocated its memory and owns it.
      this->_ownsData = true;

      this->MaxId = newSize-1;
      this->m_IsAllocated = true;
      return this->Array;
    }


  private:

    //  unsigned long long int MUD_FLAP_0;
    T* Array;
    //  unsigned long long int MUD_FLAP_1;
    size_t Size;
    //  unsigned long long int MUD_FLAP_2;
    bool _ownsData;
    //  unsigned long long int MUD_FLAP_3;
    size_t MaxId;

    bool m_IsAllocated;
    // unsigned long long int MUD_FLAP_4;
    size_t m_Dims[5];
    //  unsigned long long int MUD_FLAP_5;
    QString m_Name;


    GbcdDataArray(const GbcdDataArray&); //Not Implemented
    void operator=(const GbcdDataArray&); //Not Implemented

};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

typedef GbcdDataArray<bool> GBCDBoolArrayType;

typedef GbcdDataArray<unsigned char> GBCDUCharArrayType;

typedef GbcdDataArray<int8_t>  GBCDInt8ArrayType;
typedef GbcdDataArray<uint8_t>  GBCDUInt8ArrayType;

typedef GbcdDataArray<int16_t>  GBCDInt16ArrayType;
typedef GbcdDataArray<uint16_t>  GBCDUInt16ArrayType;

typedef GbcdDataArray<int32_t>  GBCDInt32ArrayType;
typedef GbcdDataArray<uint32_t>  GBCDUInt32ArrayType;

typedef GbcdDataArray<int64_t>  GBCDInt64ArrayType;
typedef GbcdDataArray<uint64_t>  GBCDUInt64ArrayType;

typedef GbcdDataArray<float>  GBCDFloatArrayType;
typedef GbcdDataArray<double>  GBCDDoubleArrayType;

#endif //_GbcdDataArray_h_
