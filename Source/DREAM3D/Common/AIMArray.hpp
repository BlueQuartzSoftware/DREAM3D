
//
//  Copyright (c) 2008, mjackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//

#ifndef _AIMArray_h_
#define _AIMArray_h_

// STL Includes
#include <vector>
#include <sstream>
#include <fstream>

#include <MXA/Common/LogTime.h>
#include <MXA/Common/MXAEndian.h>
#include <MXA/Common/MXASetGetMacros.h>

#include <MXA/Utilities/StringUtils.h>

// -- Boost algorithms
// #include <boost/iostreams/device/file.hpp>


#define mxa_bswap(s,d,t)\
  t[0] = ptr[s];\
  ptr[s] = ptr[d];\
  ptr[d] = t[0];

/** @brief Resizes the AIMArray Shared Array and assigns its internal data pointer
 *
 */
#define RESIZE_ARRAY(sharedArray, pointer, size)\
  pointer = sharedArray->WritePointer(0, size);

#define     DECLARE_WRAPPED_ARRAY(pubVar, priVar, type)\
  AIMArray<type>::Pointer priVar;\
  type* pubVar;

/**
 * @class AIMArray AIMArray.hpp PathToHeader/AIMArray.hpp
 * @brief Template class for wrapping raw arrays of data.
 * @author mjackson
 * @date July 3, 2008
 * @version $Revision: 1.2 $
 */
template<typename T>
class AIMArray
{
  public:

    MXA_SHARED_POINTERS(AIMArray<T> )
    typedef std::vector<Pointer>   ContainterType;

    /**
     * @brief Static constructor
     * @param numElements The number of elements in the internal array.
     * @return Boost::Shared_Ptr wrapping an instance of AIMArrayTemplate<T>
     */
    static Pointer CreateArray(size_t numElements = 0)
    {
      AIMArray<T>* d = new AIMArray<T> (numElements, true);
      if (d->Allocate() < 0)
      { // Could not allocate enough memory, reset the pointer to null and return
        delete d;
        return AIMArray<T>::NullPointer();
      }
      Pointer ptr((d));
      return ptr;
    }

    /**
     * @brief Destructor
     */
    virtual ~AIMArray()
    {
      //std::cout << "~AIMArrayTemplate '" << m_Name << "'" << std::endl;
      if ((NULL != this->Array) && (true == this->_ownsData))
      {
        _deallocate();
      }
    }

    /**
     * @brief Gives this array a human readable name
     * @param name The name of this array
     */
    void SetName(const std::string &name)
    {
      m_Name = name;
    }

    /**
     * @brief Returns the human readable name of this array
     * @return
     */
    std::string GetName()
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
    int32_t Allocate()
    {
      if ((NULL != this->Array) && (true == this->_ownsData))
      {
        _deallocate();
      }
      this->Array = NULL;
      this->_ownsData = true;
      size_t newSize = (this->Size > 0 ? this->Size : 1);
#if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
      Array = static_cast<T*>( _mm_malloc (newSize * sizeof(T), 16) );
#else
      this->Array = (T*)malloc(newSize * sizeof(T));
#endif
      if (!this->Array)
      {
        std::cout << DEBUG_OUT(logTime) << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. " << std::endl;
        return -1;
      }
      this->Size = newSize;
      return 1;
    }

   /**
    * @brief Get the address of a particular data index. Make sure data is allocated
    * for the number of items requested. Set MaxId according to the number of
    * data values requested. For example if you want to ensure that you have enough
    * memory allocated to write to the 1000 element then you would have code such
    * as:
    * @code
    *  int* ptr = array->WritePointer(0, 1000);
    * @endcode
    * @param id The Address of the data index
    * @param number The number of elements to ensure memory allocation
    * @return
    */
    T* WritePointer(size_t id, size_t number)
    {
        size_t newSize=id+number;
      if ( newSize > this->Size )
        {
        if (this->ResizeAndExtend(newSize)==0)
          {
          return 0;
          }
        }
      if ( (--newSize) > this->MaxId )
        {
        this->MaxId = newSize;
        }

      return this->Array + id;
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
      this->Array = 0;
      this->Size = 0;
      this->_ownsData = true;
   //   this->_dims[0] = _nElements;
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
     * @brief Reseizes the internal array
     * @param size The new size of the internal array
     * @return 1 on success, 0 on failure
     */
    virtual int32_t Resize(size_t size)
    {
      if (this->ResizeAndExtend(size) || size <= 0)
      {
        return 1;
      }
      else
      {
        return 0;
      }
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
      if (i >= this->GetNumberOfTuples())
      {
        return 0x0;
      }
      return (void*)(&(Array[i]));
    }

    /**
     * @brief Returns the value for a given index
     * @param i The index to return the value at
     * @return The value at index i
     */
    virtual T GetValue(size_t i)
    {
      return this->Array[i];
    }

//    /**
//     * @brief Overload the operator[] to return a specific element
//     * @param i
//     * @return
//     */
//    T & operator[] (std::ptrdiff_t i) const // never throws
//    {
//      return this->_data[i];
//    }

    /**
     * @brief Returns the number of elements in the internal array.
     */
    virtual size_t GetNumberOfTuples()
    {
      return Size;
    }

    /**
     * @brief This is for convenience and compatibility with other container types
     * which return the number of elements in the array.
     * @return The number of elements in the array.
     */
//    virtual size_t size()
//    {
//      return _nElements;
//    }

    /**
     * @brief This is for convenience and compatibility with other container types
     * which return the number of elements in the array.
     * @return The number of elements in the array.
     */
//    virtual size_t count()
//    {
//      return _nElements;
//    }

    // Description:
    // Set/Get the dimension (n) of the components. Must be >= 1. Make sure that
    // this is set before allocation.
    void SetNumberOfComponents(int nc) { if (nc > 0) this->NumberOfComponents = nc; }
    int GetNumberOfComponents() { return this->NumberOfComponents; }

    /**
     * @brief Sets a specific value in the array
     * @param i The index of the value to set
     * @param value The new value to be set at the specified index
     */
    void SetValue(size_t i, T value)
    {
      this->Array[i] = value;
    }

    //----------------------------------------------------------------------------
    // These can be overridden for more efficiency
    T GetComponent(size_t i, int j)
    {
      return Array[i*this->NumberOfComponents + j];
    }

    //----------------------------------------------------------------------------
    void SetComponent(size_t i, int j, T c)
    {
      Array[i*this->NumberOfComponents + j] = c;
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
     * @brief
     */
    virtual void byteSwapElements()
    {
      char* ptr = (char*)(Array);
      char t[8];
      size_t size = getTypeSize();
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
     * @brief Returns the pointer to a specific index into the array. No checks are made
     * as to the correctness of the index being passed in. If you ask for an index off
     * then end of the array they you will likely cause your program to abort.
     * @param i The index to return the pointer to.
     * @return The pointer to the index
     */
    virtual T* GetPointer(size_t i)
    {
      return (T*)(&(Array[i]));
    }

    /**
     * @brief Prints information about the class to ostream
     * @param os
     * @param indent
     */
    virtual void printSelf(std::ostream &os, int32_t indent)
    {
      std::string ind = StringUtils::indent(indent);
      os << ind << "AIMArray<T>" << std::endl;
      ind = StringUtils::indent(indent + 1);
      os << ind << "Number of Elements: " << this->GetNumberOfTuples() << std::endl;
 //     os << ind << "Number of Dimensions: " << this->getNumberOfDimensions() << std::endl;
      //   os << ind << "DataType: " << this->getDataType() << std::endl;
      os << ind << "Begin Data" << std::endl;
      os << ind << "{";
      T* data = this->GetPointer(0);
      for (uint64_t i = 0; i < this->GetNumberOfTuples(); ++i)
      {
        os << ind << *data << " ";
        if (i % 10 == 0)
        {
          os << std::endl;
        }
      }
      os << ind << "}" << std::endl;
    }

    /**
     * @brief Converts the data array into a string delimited by the supplied
     * delimiter.
     * @param delimiter The delimiter to use between each value. Default is a single space
     * @return The generated string
     */
    virtual std::string valueToString(char delimiter = ' ')
    {
      std::stringstream sstream;
      uint64_t limit = Size - 1;
      for (uint64_t i = 0; i < Size; ++i)
      {
        if (sizeof(T) != 1)
        {
          sstream << Array[i];
        }
        else
        {
          sstream << static_cast<int32_t> (Array[i]);
        }
        if (i < limit)
        {
          sstream << delimiter;
        }
      }
      return sstream.str();
    }

    /**
     * @brief Converts the data array into a string delimited by the supplied
     * delimiter.
     * @param delimiter The delimiter to use between each value. Default is a single space
     * @return The generated string
     */
    virtual std::string valueToString(const std::string &delimiter = " ")
    {
      std::stringstream sstream;
      uint64_t limit = Size - 1;
      for (uint64_t i = 0; i < Size; ++i)
      {
        if (sizeof(T) != 1)
        {
          sstream << Array[i];
        }
        else
        {
          sstream << static_cast<int32_t> (Array[i]);
        }
        if (i < limit)
        {
          sstream << delimiter;
        }
      }
      return sstream.str();
    }

  protected:

    int NumberOfComponents; // the number of components per tuple

    /**
     * @brief Protected Constructor
     * @param numElements The number of elements in the internal array.
     * @param takeOwnership Will the class clean up the memory. Default=true
     */
    AIMArray(size_t numElements, bool ownsData = true) :
      Array(NULL), Size(numElements), _ownsData(ownsData)
    {
      NumberOfComponents = 1;
    }

    /**
     * @brief deallocates the memory block
     */
    void _deallocate()
    {
#if defined ( AIM_USE_SSE ) && defined ( __SSE2__ )
      _mm_free( this->m_buffer );
#else
      free(this->Array);
#endif
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
      if (newSize <= 0)
      {
        this->initialize();
        return 0;
      }

      // Allocate a new array if we DO NOT own the current array
      if ((NULL != this->Array) && (false == this->_ownsData))
      {
        // The old array is owned by the user so we cannot try to
        // reallocate it.  Just allocate new memory that we will own.
        newArray = (T*)malloc(newSize * sizeof(T));
        if (!newArray)
        {
          std::cout << DEBUG_OUT(logTime) << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. " << std::endl;
          return 0;
        }

        // Copy the data from the old array.
        memcpy(newArray, this->Array, (newSize < this->Size ? newSize : this->Size) * sizeof(T));
      }
      else
      {
        // Try to reallocate with minimal memory usage and possibly avoid copying.
        newArray = (T*)realloc(this->Array, newSize * sizeof(T));
        if (!newArray)
        {
          std::cout << DEBUG_OUT(logTime) << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. " << std::endl;
          return 0;
        }
      }

      // Allocation was successful.  Save it.
      this->Size = newSize;
//      this->_dims.resize(1);
//      this->_dims[0] = this->_nElements;
      this->Array = newArray;
      // This object has now allocated its memory and owns it.
      this->_ownsData = true;

      return this->Array;
    }

  private:

    T* Array;
    size_t Size;
    bool _ownsData;
    size_t MaxId;

    std::string m_Name;

    AIMArray(const AIMArray&); //Not Implemented
    void operator=(const AIMArray&); //Not Implemented

};

#endif //_AIMArray_h_

