
//
//  Copyright (c) 2008, mjackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//

#ifndef _PreFlightArray_h_
#define _PreFlightArray_h_

// STL Includes
#include <vector>
#include <sstream>
#include <fstream>


#include "MXA/Utilities/StringUtils.h"


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"

#define mxa_bswap(s,d,t)\
  t[0] = ptr[s];\
  ptr[s] = ptr[d];\
  ptr[d] = t[0];

/** @brief Resizes the PreFlightArray Shared Array and assigns its internal data pointer
 *
 */


#define INIT_PreFlightArray(var, type)\
  var = PreFlightArray<type>::CreateArray(0);\
  var->SetName(#var);

/**
 * @class PreFlightArray PreFlightArray.hpp DREAM3DLib/Common/PreFlightArray.hpp
 * @brief .
 * @author mjackson
 * @date July 3, 2008
 * @version 1.0
 */
template<typename T>
class PreFlightArray : public IDataArray
{
  public:

    DREAM3D_SHARED_POINTERS(PreFlightArray<T> )
    DREAM3D_TYPE_MACRO_SUPER(PreFlightArray<T>, IDataArray)


    typedef std::vector<Pointer>   ContainterType;

    /**
     * @brief Static constructor
     * @param numElements The number of elements in the internal array.
     * @return Boost::Shared_Ptr wrapping an instance of PreFlightArrayTemplate<T>
     */
    static Pointer CreateArray(size_t numElements = 0)
    {
      PreFlightArray<T>* d = new PreFlightArray<T> (numElements, true);
      if (d->Allocate() < 0)
      { // Could not allocate enough memory, reset the pointer to null and return
        delete d;
        return PreFlightArray<T>::NullPointer();
      }
      Pointer ptr((d));
      return ptr;
    }

    /**
     * @brief Destructor
     */
    virtual ~PreFlightArray()
    {
      //std::cout << "~PreFlightArrayTemplate '" << m_Name << "'" << std::endl;
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
      this->Array = (T*)malloc(1 * sizeof(T));
#endif
      if (!this->Array)
      {
        std::cout << "Unable to allocate " << newSize << " elements of size " << sizeof(T) << " bytes. " << std::endl;
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
    * @param id The offset of the data index
    * @param number The number of elements to ensure memory allocation
    * @return
    */
    T* WritePointer(size_t id, size_t number)
    {

      return this->Array;
    }

    /**
     * @brief Initializes this class to zero bytes freeing any data that it currently owns
     */
    virtual void initialize()
    {
    }

    /**
     * @brief Sets all the values to zero.
     */
    virtual void initializeWithZeros()
    {
    }

    /**
     * @brief Reseizes the internal array
     * @param size The new size of the internal array
     * @return 1 on success, 0 on failure
     */
    virtual int32_t Resize(size_t size)
    {
      Size = size;
      return 0;
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
      return (void*)(&(Array[0]));
    }


    /**
     * @brief Returns the value for a given index
     * @param i The index to return the value at
     * @return The value at index i
     */
    virtual T GetValue(size_t i)
    {
      return this->Array[0];
    }

    /**
     * @brief Returns the number of elements in the internal array.
     */
    virtual size_t GetNumberOfTuples()
    {
      return Size;
    }

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
      this->Array[0] = value;
    }

    //----------------------------------------------------------------------------
    // These can be overridden for more efficiency
    T GetComponent(size_t i, int j)
    {
      return Array[0*this->NumberOfComponents + j];
    }

    //----------------------------------------------------------------------------
    void SetComponent(size_t i, int j, T c)
    {
      Array[0*this->NumberOfComponents + j] = c;
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
      for (uint64_t var = 0; var < 1; ++var)
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
      return (T*)(&(Array[0]));
    }

  protected:

    int NumberOfComponents; // the number of components per tuple

    /**
     * @brief Protected Constructor
     * @param numElements The number of elements in the internal array.
     * @param takeOwnership Will the class clean up the memory. Default=true
     */
    PreFlightArray(size_t numElements, bool ownsData = true) :
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
      Size = size;
      return this->Array;
    }

  private:

    T* Array;
    size_t Size;
    bool _ownsData;
    size_t MaxId;

    std::string m_Name;

    PreFlightArray(const PreFlightArray&); //Not Implemented
    void operator=(const PreFlightArray&); //Not Implemented

};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

typedef PreFlightArray<bool> PFBoolArrayType;
typedef PreFlightArray<int32_t> PFInt32ArrayType;
typedef PreFlightArray<int8_t> PFInt8ArrayType;
typedef PreFlightArray<float> PFFloatArrayType;
typedef PreFlightArray<double> PFDoubleArrayType;

#endif //_PreFlightArray_h_

