///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, mjackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//
///////////////////////////////////////////////////////////////////////////////
#ifndef _MXAArrayTemplate_h_
#define _MXAArrayTemplate_h_

// STL Includes
#include <sstream>
#include <fstream>

#include <MXA/Common/MXATypeDefs.h>
#include <MXA/Common/LogTime.h>
#include <MXA/Common/MXAEndian.h>
#include <MXA/Common/MXASetGetMacros.h>


//#ifdef MXA_USE_HDF5_PRIMITIVE_TYPES
#include <MXA/HDF5/H5Lite.h>
//#endif
#include <MXA/Utilities/StringUtils.h>



// -- Boost algorithms
// #include <boost/iostreams/device/file.hpp>


#define mxa_bswap(s,d,t)\
  t[0] = ptr[s];\
  ptr[s] = ptr[d];\
  ptr[d] = t[0];

/**
* @class MXAArrayTemplate MXAArrayTemplate.hpp PathToHeader/MXAArrayTemplate.hpp
* @brief Template class for wrapping raw arrays of data.
* @author mjackson
* @date July 3, 2008
* @version $Revision: 1.2 $
*/
template<typename T>
class MXAArrayTemplate
{
  public:

/**
 * @brief Static constructor
 * @param numElements The number of elements in the internal array.
 * @return Boost::Shared_Ptr wrapping an instance of MXAArrayTemplateTemplate<T>
 */
    static MXAArrayTemplate::Pointer CreateArray( uint64_t numElements)
    {
      MXAArrayTemplate<T>* d = new MXAArrayTemplate<T>( numElements, true);
      if ( d->_allocate() < 0)
      {  // Could not allocate enough memory, reset the pointer to null and return
        d = NULL;
      }
      MXAArrayTemplate::Pointer ptr ( static_cast<MXAArrayTemplate*>(d) );
      return ptr;
    }

    /**
    * @brief Creates an MXAArrayTemplate::Pointer object for the supplied arguments.
    * @param nDims Number of dimensions
    * @param dims Size of each dimension
    * @return Boost::Shared_Ptr wrapping an instance of MXAArrayTemplateTemplate<T>
    */
    static MXAArrayTemplate::Pointer CreateMultiDimensionalArray(size_t nDims, const uint64_t* dims)
    {
      MXAArrayTemplate<T>* d = new MXAArrayTemplate<T>( nDims, dims, true);
      if ( d->_allocate() < 0)
      {  // Could not allocate enough memory, reset the pointer to null and return
        d = NULL;
      }
      MXAArrayTemplate::Pointer ptr ( static_cast<MXAArrayTemplate*>(d) );
      return ptr;
    }

/**
 * @brief Creates an Attribute from a single value
 * @param value The value to store in the attribute
 * @return Boost::Shared_Ptr wrapping an instance of MXAArrayTemplateTemplate<T>
 */
    static MXAArrayTemplate::Pointer CreateSingleValueArray( T value)
    {
      MXAArrayTemplate<T>* d = new MXAArrayTemplate<T>(1, true);
      if ( d->_allocate() < 0)
      {  // Could not allocate enough memory, reset the pointer to null and return
        d = NULL;
      }
      d->setValue(0, value);
      MXAArrayTemplate::Pointer ptr ( static_cast<MXAArrayTemplate*>(d) );
      return ptr;
    }


/**
 * @brief Static construction of MXAArrayTemplate objects. YOU are
 * responsible for cleaning up the memory that this method creates.
 * @param numElements The number of elements in the internal array.
 * @return
 */
    static MXAArrayTemplate<T>* New( uint64_t numElements)
    {
      MXAArrayTemplate<T>* ptr = new MXAArrayTemplate<T>( numElements, true);
      if (ptr->_allocate() < 0)
      { // Could not allocate enough memory, reset the pointer to null and return
        ptr = 0x0;
      }
      return ptr;
    }

    /**
     * @brief Creates a MultiDimensional Array allocating the memory immediately
     * @param nDims The number of dimensions
     * @param dims The size of each dimension
     * @return Pointer to Object or NULL if there was an error creating the object.
     */
    static MXAArrayTemplate<T>* New(size_t nDims, const uint64_t* dims)
    {
      MXAArrayTemplate<T>* d = new MXAArrayTemplate<T>( static_cast<int32_t>(nDims), dims, true);
      if ( d->_allocate() < 0)
      {  // Could not allocate enough memory, reset the pointer to null and return
        d = NULL;
      }
      return d;
    }



/**
 * @brief Destructor
 */
    virtual ~MXAArrayTemplate()
    {
      //std::cout << "~MXAArrayTemplateTemplate" << std::endl;
      if ((NULL != this->_data) && (true == this->_ownsData))
      {
        free(this->_data);
      }
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
 * @brief Initializes this class to zero bytes freeing any data that it currently owns
 */
    virtual void initialize()
    {
      if(NULL != this->_data && true == this->_ownsData)
        {
        free(this->_data);
        }
      this->_data = 0;
      this->_nElements = 0;
      this->_ownsData = true;
      this->_dims[0] = _nElements;
    }

    /**
     * @brief Sets all the values to zero.
     */
    virtual void initializeWithZeros()
    {
      size_t typeSize = sizeof(T);
      ::memset(this->_data, 0, this->_nElements * typeSize);
    }


    /**
     * @brief Reseizes the internal array
     * @param size The new size of the internal array
     * @return 1 on success, 0 on failure
     */
    virtual int32_t resize(uint64_t size)
    {
      if(this->_resizeAndExtend(size) || size <= 0)
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
    virtual void* getVoidPointer(uint64_t i)
    {
      if (i >= this->getNumberOfElements() )
      {
        return 0x0;
      }
      return (void*)(&(_data[i]) );
    }

  /**
   * @brief Returns the value for a given index
   * @param i The index to return the value at
   * @return The value at index i
   */
    virtual T getValue(uint64_t i)
    {
      return this->_data[i];
    }

    /**
     * @brief Returns the number of elements in the internal array.
     */
    virtual uint64_t getNumberOfElements()
    {
      return _nElements;
    }

    /**
     * @brief Copies the values of the dimensions into the supplied pointer
     * @param dims Pointer to store the dimension values into
     */
    virtual void getDimensions(uint64_t* dims)
    {
      uint64_t nBytes = _dims.size() * sizeof(uint64_t);
      ::memcpy(dims, &(_dims.front()), nBytes );
    }

    /**
     * Returns the number of dimensions the data has.
     */
    virtual int32_t getNumberOfDimensions()
    {
      return static_cast<int32_t>(this->_dims.size());
    }

/**
 * @brief Sets a specific value in the array
 * @param i The index of the value to set
 * @param value The new value to be set at the specified index
 */
    void setValue(uint64_t i, T value)
    {
      this->_data[i] = value;
    }

    /**
     * @brief Returns an enumerated type that can be used to find out the type
     * of primitive stored in the internal array. Currently the HDF5 type is returned
     * in order to use this class effectively with HDF5
     */
    virtual int32_t getDataType()
    {
      T value = 0x0;
      return H5Lite::HDFTypeForPrimitive<T>(value);
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
      char* ptr = (char*)(_data);
      char t[8];
      size_t size = getTypeSize();
      for (uint64_t var = 0; var < _nElements; ++var)
      {
        if (sizeof(T) == 2) {
          mxa_bswap(0, 1, t);
        }
        else if (sizeof(T) == 4 )
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
    virtual T* getPointer(uint64_t i)
    {
      return (T*)(&(_data[i]) );
    }



    /**
     * @brief Prints information about the class to ostream
     * @param os
     * @param indent
     */
    virtual void printSelf(std::ostream &os, int32_t indent)
    {
      std::string ind = StringUtils::indent(indent);
      os << ind << "MXAArrayTemplate<T>" << std::endl;
      ind = StringUtils::indent(indent + 1);
      os << ind << "Number of Elements: " << this->getNumberOfElements() << std::endl;
      os << ind << "Number of Dimensions: " << this->getNumberOfDimensions() << std::endl;
      os << ind << "DataType: " << this->getDataType() << std::endl;
      os << ind << "Begin Data" << std::endl;
      os << ind << "{";
      T* data = this->getPointer(0);
      for (uint64_t i = 0; i < this->getNumberOfElements(); ++i)
      {
        os << ind << *data << " ";
        if (i%10 ==0)
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
      uint64_t limit = _nElements - 1;
      for(uint64_t i = 0; i < _nElements; ++i)
      {
        if (sizeof(T) != 1 )
         {
          sstream  << _data[i];
         }
         else
         {
           sstream  << static_cast<int32_t>(_data[i]);
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
    virtual std::string valueToString(const std::string &delimiter=" ")
    {
      std::stringstream sstream;
      uint64_t limit = _nElements - 1;
      for(uint64_t i = 0; i < _nElements; ++i)
      {
        if (sizeof(T) != 1 )
        {
          sstream  << _data[i];
        }
        else
        {
          sstream  << static_cast<int32_t>(_data[i]);
        }
        if (i < limit)
        {
          sstream << delimiter;
        }
      }
      return sstream.str();
    }

  protected:

  /**
   * @brief Protected Constructor
   * @param numElements The number of elements in the internal array.
   * @param takeOwnership Will the class clean up the memory. Default=true
     */
      MXAArrayTemplate(int32_t numElements,
                       bool ownsData = true) :
        _data(NULL),
        _nElements(numElements),
        _ownsData(ownsData)
      {
        _dims.resize(1);
        _dims[0] = numElements;
      }

  /**
   * @brief Constructor used to create an MXAArrayTemplate class that has multiple dimensions.
   * @param numDims The number of dimensions to the data set.
   * @param dims The actual values of the dimensions.
   * @param takeOwnership Will the class clean up the memory. Default=true
   */
      MXAArrayTemplate(size_t numDims,
                       const uint64_t* dims,
                       bool ownsData = true) :
        _data(NULL),
        _ownsData(ownsData)
      {
        _dims.resize(numDims);
        ::memcpy( &(_dims.front()), dims, numDims * sizeof(uint64_t));
        _nElements = 1;
        for(size_t i = 0; i < numDims; ++i)
        {
          _nElements = _nElements * dims[i];
        }
      }


    /**
     * @brief Allocates the memory needed for this class
     * @return 1 on success, -1 on failure
     */
        int32_t _allocate()
        {
          if ( (NULL != this->_data) && (true == this->_ownsData) )
          {
            ::free(this->_data);
          }
          this->_data = NULL;
          this->_ownsData = true;
          int newSize = (this->_nElements > 0 ? this->_nElements : 1);
          this->_data = (T*)malloc(newSize * sizeof(T));
          if (!this->_data)
          {
            std::cout << DEBUG_OUT(logTime) << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. " << std::endl;
            return -1;
          }
          this->_nElements = newSize;
          return 1;
        }

      /**
       * @brief resizes the internal array to be 'size' elements in length
       * @param size
       * @return Pointer to the internal array
       */
      virtual T* _resizeAndExtend(uint64_t size)
        {
          T* newArray;
          uint64_t newSize;

          if (size > this->_nElements)
          {
            newSize = size;
          }
          else if (size == this->_nElements) // Requested size is equal to current size.  Do nothing.
          {
            return this->_data;
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
          if ( (NULL != this->_data) && (false == this->_ownsData))
          {
            // The old array is owned by the user so we cannot try to
            // reallocate it.  Just allocate new memory that we will own.
            newArray = (T*)malloc(newSize*sizeof(T));
            if (!newArray)
            {
              std::cout << DEBUG_OUT(logTime) << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. " << std::endl;
              return 0;
            }

            // Copy the data from the old array.
            memcpy(newArray, this->_data, (newSize < this->_nElements ? newSize : this->_nElements) * sizeof(T));
          }
          else
          {
            // Try to reallocate with minimal memory usage and possibly avoid copying.
            newArray = (T*)realloc(this->_data, newSize*sizeof(T));
            if (!newArray)
            {
              std::cout << DEBUG_OUT(logTime) << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. " << std::endl;
              return 0;
            }
          }

          // Allocation was successful.  Save it.
          this->_nElements = newSize;
          this->_dims[0] = this->_nElements;
          this->_data = newArray;
          // This object has now allocated its memory and owns it.
          this->_ownsData = true;

          return this->_data;
        }


  private:

    T* _data;
    uint64_t _nElements;
    bool _ownsData;

    std::vector<uint64_t> _dims;

    MXAArrayTemplate(const MXAArrayTemplate&);    //Not Implemented
    void operator=(const MXAArrayTemplate&); //Not Implemented

};

#endif //_MXAArrayTemplate_h_



