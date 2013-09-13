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
#ifndef _DataArray_h_
#define _DataArray_h_

// STL Includes
#include <vector>
#include <sstream>
#include <fstream>





#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArrayFilter.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"
#include "DREAM3DLib/HDF5/H5DataArrayWriter.hpp"
#include "DREAM3DLib/HDF5/H5DataArrayReader.h"


#define mxa_bswap(s,d,t)\
  t[0] = ptr[s];\
  ptr[s] = ptr[d];\
  ptr[d] = t[0];

/** @brief Resizes the DataArray Shared Array and assigns its internal data pointer
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

    DREAM3D_SHARED_POINTERS(DataArray<T> )
    DREAM3D_TYPE_MACRO_SUPER(DataArray<T>, IDataArray)

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
     * @brief GetTypeName Returns a string representation of the type of data that is stored by this class. This
     * can be a primitive like char, float, int or the name of a class.
     * @return
     */
    void GetXdmfTypeAndSize(QString &xdmfTypeName, int &precision)
    {
      T value = 0x00;
      xdmfTypeName = "UNKNOWN";
      precision = 0;
      if (typeid(value) == typeid(int8_t)) { xdmfTypeName = "Char"; precision = 1;}
      if (typeid(value) == typeid(uint8_t)) { xdmfTypeName = "UChar"; precision = 1;}

      if (typeid(value) == typeid(int16_t)) { xdmfTypeName = "16 BIT NOT SUPPORTED BY XDMF"; precision = 0;}
      if (typeid(value) == typeid(uint16_t)) { xdmfTypeName = "16 BIT NOT SUPPORTED BY XDMF"; precision = 0;}

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
     * @brief Static constructor
     * @param numElements The number of elements in the internal array.
     * @param name The name of the array
     * @return Boost::Shared_Ptr wrapping an instance of DataArrayTemplate<T>
     */
    static Pointer CreateArray(size_t numElements, const QString &name)
    {
      if (name.isEmpty() == true)
      {
        return NullPointer();
      }
      DataArray<T>* d = new DataArray<T> (numElements, true);
      if (d->Allocate() < 0)
      { // Could not allocate enough memory, reset the pointer to null and return
        delete d;
        return DataArray<T>::NullPointer();
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
     * @return Boost::Shared_Ptr wrapping an instance of DataArrayTemplate<T>
     */
    static Pointer CreateArray(size_t numTuples, int numComponents, const QString &name)
    {

      DataArray<T>* d = new DataArray<T> (numTuples, numComponents, true);
      if (d->Allocate() < 0)
      { // Could not allocate enough memory, reset the pointer to null and return
        delete d;
        return DataArray<T>::NullPointer();
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
    static Pointer FromStdVector(QVector<T> &vec, const QString &name)
    {
      Pointer p = CreateArray(vec.size(), name);
      ::memcpy(p->getPointer(0), &(vec.front()), vec.size() * sizeof(T));
      return p;
    }

    /**
     * @brief FromPointer
     * @param data
     * @param size
     * @param name
     * @return
     */
    static Pointer FromPointer(T* data, size_t size, const QString &name)
    {
      Pointer p = CreateArray(size, name);
      ::memcpy(p->getPointer(0), data, size * sizeof(T));
      return p;
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
      IDataArray::Pointer p = DataArray<T>::CreateArray(numElements, numComponents, name);
      return p;
    }

    /**
     * @brief Destructor
     */
    virtual ~DataArray()
    {
      //qDebug() << "~DataArrayTemplate '" << m_Name << "'" ;
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
      m_IsAllocated = false;
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
      m_IsAllocated = true;

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
      m_IsAllocated = false;

      //   this->_dims[0] = _nElements;
    }

    /**
     * @brief Sets all the values to value.
     */
    virtual void initializeWithValues(T value)
    {
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

      int err = 0;

      // If nothing is to be erased just return
      if(idxs.size() == 0)
      {
        return 0;
      }

      if (idxs.size() >= getNumberOfTuples() )
      {
        Resize(0);
        return 0;
      }

      // Sanity Check the Indices in the vector to make sure we are not trying to remove any indices that are
      // off the end of the array and return an error code.
      for(QVector<size_t>::size_type i = 0; i < idxs.size(); ++i)
      {
        if (idxs[i] * this->NumberOfComponents > this->MaxId) { return -100; }
      }

      // Calculate the new size of the array to copy into
      size_t newSize = (getNumberOfTuples() - idxs.size()) * NumberOfComponents ;

      // Create a new Array to copy into
      T* newArray = (T*)malloc(newSize * sizeof(T));
      // Splat AB across the array so we know if we are copying the values or not
      ::memset(newArray, 0xAB, newSize * sizeof(T));

      // Keep the current Destination Pointer
      T* currentDest = newArray;
      size_t j = 0;
      size_t k = 0;
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
        T* currentSrc = Array + (j * NumberOfComponents);
        ::memcpy(currentDest, currentSrc, (getNumberOfTuples() - idxs.size()) * NumberOfComponents * sizeof(T));
        _deallocate(); // We are done copying - delete the current Array
        this->Size = newSize;
        this->Array = newArray;
        this->_ownsData = true;
        this->MaxId = newSize - 1;
        return 0;
      }

      QVector<size_t> srcIdx(idxs.size() + 1);
      QVector<size_t> destIdx(idxs.size() + 1);
      QVector<size_t> copyElements(idxs.size() + 1);
      srcIdx[0] = 0;
      destIdx[0] = 0;
      copyElements[0] = (idxs[0] - 0) * NumberOfComponents;

      for (size_t i = 1; i < srcIdx.size(); ++i)
      {
        srcIdx[i] = (idxs[i - 1] + 1) * NumberOfComponents;

        if(i < srcIdx.size() - 1)
        {
          copyElements[i] = (idxs[i] - idxs[i - 1] - 1) * NumberOfComponents;
        }
        else
        {
          copyElements[i] = (getNumberOfTuples() - idxs[i - 1] - 1) * NumberOfComponents;
        }
        destIdx[i] = copyElements[i - 1] + destIdx[i - 1];
      }

      // Copy the data
      for (size_t i = 0; i < srcIdx.size(); ++i)
      {
        currentDest = newArray + destIdx[i];
        T* currentSrc = Array + srcIdx[i];
        size_t bytes = copyElements[i] * sizeof(T);
        ::memcpy(currentDest, currentSrc, bytes);
      }

      // We are done copying - delete the current Array
      _deallocate();

      // Allocation was successful.  Save it.
      this->Size = newSize;
      this->Array = newArray;
      // This object has now allocated its memory and owns it.
      this->_ownsData = true;

      this->MaxId = newSize - 1;

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
      size_t max =  ((this->MaxId + 1)/this->NumberOfComponents);
      if (currentPos >= max
          || newPos >= max )
      {return -1;}
      T* src = this->Array + (currentPos * NumberOfComponents);
      T* dest = this->Array + (newPos * NumberOfComponents);
      size_t bytes = sizeof(T) * NumberOfComponents;
      ::memcpy(dest, src, bytes);
      return 0;
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
      return (this->MaxId + 1)/this->NumberOfComponents;
    }

    virtual size_t GetSize()
    {
      return Size;
    }

    // Description:
    // Set/Get the dimension (n) of the components. Must be >= 1. Make sure that
    // this is set before allocation.
    virtual void SetNumberOfComponents(int nc)
    {
      if(nc > 0) this->NumberOfComponents = nc;
    }

    virtual int GetNumberOfComponents()
    {
      return this->NumberOfComponents;
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
     * @brief Returns the value for a given index
     * @param i The index to return the value at
     * @return The value at index i
     */
    virtual T GetValue(size_t i)
    {
#ifndef NDEBUG
      if (Size > 0) { BOOST_ASSERT(i < Size);}
#endif
      return this->Array[i];
    }

    /**
     * @brief Sets a specific value in the array
     * @param i The index of the value to set
     * @param value The new value to be set at the specified index
     */
    void SetValue(size_t i, T value)
    {
#ifndef NDEBUG
      if (Size > 0) { BOOST_ASSERT(i < Size);}
#endif
      this->Array[i] = value;
    }

    //----------------------------------------------------------------------------
    // These can be overridden for more efficiency
    T GetComponent(size_t i, int j)
    {
#ifndef NDEBUG
      if (Size > 0) { BOOST_ASSERT(i*NumberOfComponents+j < Size);}
#endif
      return Array[i*this->NumberOfComponents + j];
    }

    /**
     * @brief Sets a specific component of the Tuple located at i
     * @param i The index of the Tuple
     * @param j The Component index into the Tuple
     * @param c The value to set
     */
    void SetComponent(size_t i, int j, T c)
    {
#ifndef NDEBUG
      if (Size > 0) { BOOST_ASSERT(i*NumberOfComponents+j < Size);}
#endif
      Array[i*this->NumberOfComponents + j] = c;
    }

    /**
     * @brief Splats the same value c across all values in the Tuple
     * @param i The index of the Tuple
     * @param c The value to splat across all components in the tuple
     */
    void InitializeTuple(size_t i, double p)
    {
#ifndef NDEBUG
      if (Size > 0) { BOOST_ASSERT(i*NumberOfComponents < Size);}
#endif
      T c = static_cast<T>(p);
      for (int j = 0; j < this->NumberOfComponents; ++j) {
        Array[i*this->NumberOfComponents + j] = c;
      }
    }

    /**
     * @brief Reseizes the internal array
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

    virtual int32_t Resize(size_t numTuples)
    {
      return RawResize(numTuples * this->NumberOfComponents);
    }

    virtual void printTuple(QTextStream &out, size_t i, char delimiter = ',')
    {
      for(int j = 0; j < NumberOfComponents; ++j)
      {
        if (j != 0) { out << delimiter; }
        out << Array[i*NumberOfComponents + j];
      }
    }

    virtual void printComponent(QTextStream &out, size_t i, int j)
    {
      out << Array[i*NumberOfComponents + j];
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
      return H5DataArrayWriter<T>::writeArray(parentId, GetName(), getNumberOfTuples(), GetNumberOfComponents(), Array, getFullNameOfClass());
    }

    /**
     * @brief writeXdmfAttribute
     * @param out
     * @param volDims
     * @return
     */
    virtual int writeXdmfAttribute(QTextStream &out, int64_t* volDims, const QString &hdfFileName,
                                                    const QString &groupPath, const QString &label)
    {
      if (Array == NULL) { return -85648; }
      QString dimStr;
      int precision = 0;
      QString xdmfTypeName;
      GetXdmfTypeAndSize(xdmfTypeName, precision);
      if (0 == precision)
      {
        out << "<!-- " << GetName() << " has unknown type or unsupported type or precision for XDMF to understand" << " -->" << "\n";
        return -100;
      }

      int numComp = GetNumberOfComponents();
      out << "    <Attribute Name=\"" << GetName() << label  << "\" ";
      if (numComp == 1)
      {
        out << "AttributeType=\"Scalar\" ";
        dimStr = QString("%1 %2 %3 ").arg(volDims[2]).arg(volDims[1]).arg(volDims[0]);
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


      out << "        " << hdfFileName << groupPath << "/" << GetName() << "\n";
      out << "      </DataItem>" << "\n";
      out << "    </Attribute>" << "\n";
      return 1;
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
      IDataArray::Pointer p = H5DataArrayReader::readIDataArray(parentId, GetName());
      if (p.get() == NULL)
      {
        return -1;
      }
      this->NumberOfComponents = p->GetNumberOfComponents();
      this->Size = p->GetSize();
      this->MaxId = (Size == 0) ? 0 : Size -1;
      this->Array = reinterpret_cast<T*>(p->GetVoidPointer(0));
      p->releaseOwnership();

      return err;
    }

    /**
     * @brief
     */
    virtual void byteSwapElements()
    {
      char* ptr = (char*)(Array);
      char t[8];
      size_t size = GetTypeSize();
      for (uint64_t var = 0; var < Size; ++var)
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
          mxa_bswap(0,7,t);
          mxa_bswap(1,6,t);
          mxa_bswap(2,5,t);
          mxa_bswap(3,4,t);
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
      BOOST_ASSERT(i < Size);
      return Array[i];
    }

  protected:

    int NumberOfComponents; // the number of components per tuple

    /**
     * @brief Protected Constructor
     * @param numElements The number of elements in the internal array.
     * @param takeOwnership Will the class clean up the memory. Default=true
     */
    DataArray(size_t numElements, bool ownsData = true) :
      Array(NULL),
      Size(numElements),
      _ownsData(ownsData),
      m_IsAllocated(false)
    {
      NumberOfComponents = 1;
      MaxId = (Size > 0) ? Size - 1: Size;

      //  MUD_FLAP_0 = MUD_FLAP_1 = MUD_FLAP_2 = MUD_FLAP_3 = MUD_FLAP_4 = MUD_FLAP_5 = 0xABABABABABABABABul;
    }

    /**
     * @brief Protected Constructor
     * @param numTuples The number of elements in the internal array.
     * @param numComponents The number of values each Tuple has. Size = NumTuples * NumComponents
     * @param takeOwnership Will the class clean up the memory. Default=true
     */
    DataArray(size_t numTuples, int numComponents, bool ownsData = true) :
      Array(NULL),
      _ownsData(ownsData),
      m_IsAllocated(false)
    {
      NumberOfComponents = numComponents;
      Size = numTuples * numComponents;
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
      if(NULL != cptr)
      {
        if(Size > 0)
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
      _mm_free( this->m_buffer );
#else
      free(this->Array);
#endif
      this->Array = NULL;
      m_IsAllocated = false;
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
      m_IsAllocated = true;
      return this->Array;
    }


  private:

    //  unsigned long long int MUD_FLAP_0;
    T* Array;
    //  unsigned long long int MUD_FLAP_1;
    size_t Size;
    //  unsigned long long int MUD_FLAP_4;
    bool _ownsData;
    //  unsigned long long int MUD_FLAP_2;
    size_t MaxId;

    bool m_IsAllocated;
    //   unsigned long long int MUD_FLAP_3;
    QString m_Name;
    //  unsigned long long int MUD_FLAP_5;

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
