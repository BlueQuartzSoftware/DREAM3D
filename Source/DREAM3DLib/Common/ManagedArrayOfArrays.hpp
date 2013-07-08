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

#ifndef _ManagedArrayOfArrays_H_
#define _ManagedArrayOfArrays_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"


template<typename T>
class ManagedArrayOfArrays : public IDataArray
{
  public:
    DREAM3D_SHARED_POINTERS(ManagedArrayOfArrays<T> )
    DREAM3D_TYPE_MACRO_SUPER(ManagedArrayOfArrays<T>, IDataArray)

    typedef struct {
      size_t count;
      T* data;
    } Data_t;

    /**
     * @brief Static constructor
     * @param numElements The number of elements in the internal array.
     * @param name The name of the array
     * @return Boost::Shared_Ptr wrapping an instance of ManagedArrayOfArraysTemplate<T>
     */
    static Pointer CreateArray(size_t numElements, const std::string &name)
    {
      if (name.empty() == true)
      {
        return NullPointer();
      }
      ManagedArrayOfArrays<T>* d = new ManagedArrayOfArrays<T> (numElements, true);
      if (d->Allocate() < 0)
      { // Could not allocate enough memory, reset the pointer to null and return
        delete d;
        return ManagedArrayOfArrays<T>::NullPointer();
      }
      d->SetName(name);
      Pointer ptr(d);
      return ptr;
    }

    /**
     * @brief Static Method to create a DataArray from a std::vector through a deep copy of the data
     * contained in the vector. The number of components will be set to 1.
     * @param vec The vector to copy the data from
     * @param name The name of the array
     * @return Boost::Shared_Ptr wrapping an instance of DataArrayTemplate<T>
     */
    virtual IDataArray::Pointer createNewArray(size_t numElements, int numComponents, const std::string &name)
    {
      IDataArray::Pointer p = ManagedArrayOfArrays<T>::CreateArray(numElements, name);
      return p;
    }


    /**
     * @brief Destructor
     */
    virtual ~ManagedArrayOfArrays()
    {
      //std::cout << "~ManagedArrayOfArraysTemplate '" << m_Name << "'" << std::endl;
      if ((NULL != this->Array) && (true == this->_ownsData))
      {
        _deallocate();
      }
    }

    /**
     * @brief isAllocated
     * @return
     */
    virtual bool isAllocated() {
      return m_IsAllocated;
    }

    /**
     * @brief GetTypeName Returns a string representation of the type of data that is stored by this class. This
     * can be a primitive like char, float, int or the name of a class.
     * @return
     */
    void GetXdmfTypeAndSize(std::string &xdmfTypeName, int &precision)
    {
      xdmfTypeName = getNameOfClass();
      precision = 0;
    }

    virtual std::string getTypeAsString() { return "ManagedArrayOfArrays";}
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

      if (this->Size == 0)
      {
        initialize();
        return 1;
      }


      size_t newSize = this->Size;

      this->Array = (Data_t*)malloc(newSize * sizeof(Data_t));
      if (!this->Array)
      {
        std::cout << "Unable to allocate " << newSize << " elements of size " << sizeof(Data_t) << " bytes. " << std::endl;
        return -1;
      }
      this->Size = newSize;
      this->m_IsAllocated = true;
      initializeWithZeros(); // MAKE SURE TO SPLAT Zero and NULL across the entire array otherwise we are going to have issues.
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
     * @brief Sets all the values to zero.
     */
    virtual void initializeWithZeros()
    {
      size_t typeSize = sizeof(Data_t);
      ::memset(this->Array, 0, this->Size * typeSize);
    }

    /**
     * @brief Sets all the values to value.
     */
    void initializeWithValues(T value)
    {
      for (size_t i = 0; i < this->Size; i++)
      {
            // Free each Pointer in each of the structures first
          for(size_t i = 0; i < MaxId; ++i)
          {
            Data_t& d = Array[i];
            d.count = 1;
            if (d.data != NULL)
            {
              free(d.data);
            }
            d.data[0] = value;
          }
      }
    }

    /**
     * @brief Removes Tuples from the Array. If the size of the vector is Zero nothing is done. If the size of the
     * vector is greater than or Equal to the number of Tuples then the Array is Resized to Zero. If there are
     * indices that are larger than the size of the original (before erasing operations) then an error code (-100) is
     * returned from the program.
     * @param idxs The indices to remove
     * @return error code.
     */
    virtual int EraseTuples(std::vector<size_t> &idxs)
    {

      int err = 0;

      // If nothing is to be erased just return
      if(idxs.size() == 0)
      {
        return 0;
      }

      if (idxs.size() >= GetNumberOfTuples() )
      {
        Resize(0);
        return 0;
      }

      // Sanity Check the Indices in the vector to make sure we are not trying to remove any indices that are
      // off the end of the array and return an error code.
      for(std::vector<size_t>::size_type i = 0; i < idxs.size(); ++i)
      {
        if (idxs[i] > this->MaxId) { return -100; }
      }

      // Calculate the new size of the array to copy into
      size_t newSize = (GetNumberOfTuples() - idxs.size());
      Data_t* currentSrc = this->Array;

      // Create a new Array to copy into
      Data_t* newArray = (Data_t*)malloc(newSize * sizeof(Data_t));
      // Splat AB across the array so we know if we are copying the values or not
      ::memset(newArray, 0xAB, newSize * sizeof(Data_t));

      // Keep the current Destination Pointer
      Data_t* currentDest = newArray;
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
        currentSrc = Array + (j);
        ::memcpy(currentDest, currentSrc, (GetNumberOfTuples() - idxs.size()) * sizeof(Data_t));
        _deallocate(); // We are done copying - delete the current Array
        this->Size = newSize;
        this->Array = newArray;
        this->_ownsData = true;
        this->MaxId = newSize - 1;
        return 0;
      }

      std::vector<size_t> srcIdx(idxs.size() + 1);
      std::vector<size_t> destIdx(idxs.size() + 1);
      std::vector<size_t> copyElements(idxs.size() + 1);
      srcIdx[0] = 0;
      destIdx[0] = 0;
      copyElements[0] = (idxs[0] - 0);

      for (size_t i = 1; i < srcIdx.size(); ++i)
      {
        srcIdx[i] = (idxs[i - 1] + 1);

        if(i < srcIdx.size() - 1)
        {
          copyElements[i] = (idxs[i] - idxs[i - 1] - 1);
        }
        else
        {
          copyElements[i] = (GetNumberOfTuples() - idxs[i - 1] - 1);
        }
        destIdx[i] = copyElements[i - 1] + destIdx[i - 1];
      }

      // Copy the data
      for (size_t i = 0; i < srcIdx.size(); ++i)
      {
        currentDest = newArray + destIdx[i];
        currentSrc = Array + srcIdx[i];
        size_t bytes = copyElements[i] * sizeof(Data_t);
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
      size_t max =  ((this->MaxId + 1));
      if (currentPos >= max
          || newPos >= max )
      {return -1;}
      Data_t* src = this->Array + (currentPos);
      Data_t* dest = this->Array + (newPos);
      size_t bytes = sizeof(Data_t);
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
      return sizeof(Data_t);
    }

    /**
     * @brief Returns the number of elements in the internal array.
     */
    virtual size_t GetNumberOfTuples()
    {
      if (Size == 0) { return 0; }
      return (this->MaxId + 1);
    }

    virtual size_t GetSize()
    {
      return Size;
    }

    // Description:
    // Set/Get the dimension (n) of the components. Must be >= 1. Make sure that
    // this is set before allocation.
    void SetNumberOfComponents(int nc)
    {

    }

    int GetNumberOfComponents()
    {
      return 1;
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
    virtual Data_t* GetPointer(size_t i)
    {
#ifndef NDEBUG
      if (Size > 0) { BOOST_ASSERT(i < Size);}
#endif
      return (Data_t*)(&(Array[i]));
    }

    /**
     * @brief Splats the same value c across all values in the Tuple
     * @param i The index of the Tuple
     * @param c The value to splat across all components in the tuple
     */
    void InitializeTuple(size_t i, double p)
    {
#ifndef NDEBUG
      if (Size > 0) { BOOST_ASSERT(i < Size);}
#endif
      BOOST_ASSERT(false);
      //T c = static_cast<T>(p);
      //for (int j = 0; j < this->NumberOfComponents; ++j)
      {
        // Array[i] = c;
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
      return RawResize(numTuples );
    }

    virtual void printTuple(std::ostream &out, size_t i, char delimiter = ',')
    {
      BOOST_ASSERT(false);
      //        for(int j = 0; j < NumberOfComponents; ++j)
      //        {
      //          if (j != 0) { out << delimiter; }
      //          out << Array[i + j];
      //        }
    }
    virtual void printComponent(std::ostream &out, size_t i, int j)
    {
      BOOST_ASSERT(false);
      //        out << Array[i + j];
    }


    /**
     *
     * @param parentId
     * @return
     */
    virtual int writeH5Data(hid_t parentId)
    {
      BOOST_ASSERT(false);
      return -1;
      //   return H5ManagedArrayOfArraysWriter<T>::writeArray(parentId, GetName(), GetNumberOfTuples(), GetNumberOfComponents(), Array, getFullNameOfClass());
    }

    /**
     * @brief writeXdmfAttribute
     * @param out
     * @param volDims
     * @param hdfFileName
     * @param groupPath
     * @return
     */
    virtual int writeXdmfAttribute(std::ostream &out, int64_t* volDims, const std::string &hdfFileName,
                                    const std::string &groupPath, const std::string &labelb)
    {
      out << "<!-- Xdmf is not supported for " << getNameOfClass() << " with type " << getTypeAsString() << " --> ";
      return -1;
    }

    /**
     * @brief
     * @param parentId
     * @return
     */
    virtual int readH5Data(hid_t parentId)
    {
      BOOST_ASSERT(false);
      int err = -1;

      //      this->Resize(0);
      //      IManagedArrayOfArrays::Pointer p = H5ManagedArrayOfArraysReader::readIManagedArrayOfArrays(parentId, GetName());
      //      if (p.get() == NULL)
      //      {
      //        return -1;
      //      }
      //      this->NumberOfComponents = p->GetNumberOfComponents();
      //      this->Size = p->GetSize();
      //      this->MaxId = (Size == 0) ? 0 : Size -1;
      //      this->Array = reinterpret_cast<T*>(p->GetVoidPointer(0));
      //      p->releaseOwnership();

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
    ManagedArrayOfArrays(size_t numElements, bool ownsData = true) :
      Array(NULL),
      Size(numElements),
      _ownsData(ownsData),
      m_IsAllocated(false)
    {

      MaxId = (Size > 0) ? Size - 1: Size;
      //  MUD_FLAP_0 = MUD_FLAP_1 = MUD_FLAP_2 = MUD_FLAP_3 = MUD_FLAP_4 = MUD_FLAP_5 = 0xABABABABABABABABul;
    }
    /**
     * @brief deallocates the memory block
     */
    void _deallocate()
    {
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
    // Free each Pointer in each of the structures first
    for(size_t i = 0; i < MaxId; ++i)
    {
      Data_t& d = Array[i];
      free(d.data);
      d.count = 0;
      d.data = NULL;
    }
    // Then free this set of memory
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
    virtual Data_t* ResizeAndExtend(size_t size)
    {
      Data_t* newArray;
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
        newArray = (Data_t*)malloc(newSize * sizeof(Data_t));
        if (!newArray)
        {
          std::cout << "Unable to allocate " << newSize << " elements of size " << sizeof(Data_t) << " bytes. " << std::endl;
          return 0;
        }

        // Copy the data from the old array.
        memcpy(newArray, this->Array, (newSize < this->Size ? newSize : this->Size) * sizeof(Data_t));
      }
      else if (!dontUseRealloc)
      {
        // Try to reallocate with minimal memory usage and possibly avoid copying.
        newArray = (Data_t*)realloc(this->Array, newSize * sizeof(Data_t));
        if (!newArray)
        {
          std::cout << "Unable to allocate " << newSize << " elements of size " << sizeof(Data_t) << " bytes. " << std::endl;
          return 0;
        }
      }
      else
      {
        newArray = (Data_t*)malloc(newSize * sizeof(Data_t));
        if (!newArray)
        {
          std::cout << "Unable to allocate " << newSize << " elements of size " << sizeof(Data_t) << " bytes. " << std::endl;
          return 0;
        }

        // Copy the data from the old array.
        if (this->Array != NULL) {
          memcpy(newArray, this->Array, (newSize < this->Size ? newSize : this->Size) * sizeof(Data_t));
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
    Data_t* Array;
    //  unsigned long long int MUD_FLAP_1;
    size_t Size;
    //  unsigned long long int MUD_FLAP_4;
    bool _ownsData;
    //  unsigned long long int MUD_FLAP_2;
    size_t MaxId;

    bool m_IsAllocated;
    //   unsigned long long int MUD_FLAP_3;
    std::string m_Name;
    //  unsigned long long int MUD_FLAP_5;

    ManagedArrayOfArrays(const ManagedArrayOfArrays&); //Not Implemented
    void operator=(const ManagedArrayOfArrays&); //Not Implemented


};


#endif /* _ManagedArrayOfArrays_H_ */
