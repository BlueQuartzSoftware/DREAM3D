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
#ifndef _IDATAARRAY_HPP_
#define _IDATAARRAY_HPP_


//-- C++
#include <string>
#include <iostream>
#include <vector>

#include <hdf5.h>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"


/**
* @class IDataArray IDataArray.h PathToHeader/IDataArray.h
* @brief This class holds a raw pointer to some allocated data that can be stored
* into or read from an HDF5 data file.
* The class design was borrowed heavily from the vtkDataArray class from www.vtk.org.
* The work was all performed by those individuals. I have merely changed a few
* methods to meet my specific needs.
* @author mjackson
* @date Jan 3, 2008
* @version $Revision: 1.2 $
*/
class DREAM3DLib_EXPORT IDataArray
{
  public:
    DREAM3D_SHARED_POINTERS(IDataArray)
    DREAM3D_TYPE_MACRO(IDataArray)

    /**
     * This templated method is used to get at the low level pointer that points
     * to the actual data by testing the conversion with dynamic_cast<> first to
     * see if it can be done, the finally returns the low level pointer.
     * @code
     *    typedef DataArray<int32_t>  Int32ArrayType;
     *    int32_t* iPtr = IDataArray::SafeReinterpretCast<IDataArray*, Int32ArrayType*, int32_t*>(ptr.get());
    *     BOOST_ASSERT(NULL != iPtr);
     * @endcode
     * @param x The Pointer to IDataArray
     * @return
     */
    template <class Source, class Target, typename Raw>
    static Raw SafeReinterpretCast(Source x) {
        if( dynamic_cast<Target>(x) != x ) {
          return 0;
        }
        return reinterpret_cast<Raw>(x->GetVoidPointer(0));
    }


    IDataArray();
    virtual ~IDataArray();

    virtual void SetName(const std::string &name) = 0;
    virtual std::string GetName() = 0;

    virtual Pointer createNewArray(size_t numElements, int numComponents, const std::string &name) = 0;

    /**
     * @brief Makes this class responsible for freeing the memory.
     */
    virtual void takeOwnership () = 0;

    /**
     * @brief This class will NOT free the memory associated with the internal pointer.
     * This can be useful if the user wishes to keep the data around after this
     * class goes out of scope.
     */
    virtual void releaseOwnership () = 0;

    /**
     * @brief Returns a void pointer pointing to the index of the array. NULL
     * pointers are entirely possible. No checks are performed to make sure
     * the index is with in the range of the internal data array.
     * @param i The index to have the returned pointer pointing to.
     * @return Void Pointer. Possibly NULL.
     */
    virtual void* GetVoidPointer ( size_t i) = 0;

     /**
     * @brief Returns the number of Tuples in the array.
     */
    virtual size_t GetNumberOfTuples () = 0;


    /**
     * @brief Return the number of elements in the array
     * @return
     */
    virtual size_t GetSize() = 0;

    virtual void SetNumberOfComponents(int nc)  = 0;
    virtual int GetNumberOfComponents()  = 0;

    /**
     * @brief Returns the number of bytes that make up the data type.
     * 1 = char
     * 2 = 16 bit integer
     * 4 = 32 bit integer/Float
     * 8 = 64 bit integer/Double
     */
    virtual size_t GetTypeSize() = 0;

    /**
     * @brief GetTypeName Returns a string representation of the type of data that is stored by this class. This
     * can be a primitive like char, float, int or the name of a class.
     * @return
     */
    virtual void GetXdmfTypeAndSize(std::string &xdmfTypeName, int &precision) = 0;

    /**
     * @brief Erases tuples based on a list of specific Tuple indices
     * @param idxs The indices to erase
     * @return
     */
    virtual int EraseTuples(std::vector<size_t> &idxs) = 0;

    /**
     * @brief Copies a Tuple from one position to another.
     * @param currentPos The index of the source data
     * @param newPos The destination index to place the copied data
     * @return
     */
    virtual int CopyTuple(size_t currentPos, size_t newPos) = 0;


    /**
     * @brief Splats the same value c across all values in the Tuple
     * @param pos The index of the Tuple
     * @param value pointer to value
     */
    virtual void InitializeTuple(size_t pos, double value) = 0;

    /**
     * @brief Sets all the values to zero.
     */
    virtual void initializeWithZeros() = 0;

    /**
     * @brief Reseizes the internal array
     * @param size The new size of the internal array
     * @return 1 on success, 0 on failure
     */
    virtual int32_t RawResize(size_t size) = 0;

    /**
     * @brief Reseizes the internal array
     * @param size The new size of the internal array
     * @return 1 on success, 0 on failure
     */
    virtual int32_t Resize(size_t numTuples) = 0;


    virtual void printTuple(std::ostream &out, size_t i, char delimiter = ',') = 0;
    virtual void printComponent(std::ostream &out, size_t i, int j) = 0;


    /**
     *
     * @param parentId
     * @return
     */
    virtual int writeH5Data(hid_t parentId) = 0;
    virtual int readH5Data(hid_t parentId) = 0;

    virtual std::string getTypeAsString() = 0;

  protected:

  private:
      IDataArray (const IDataArray&);    //Not Implemented
      void operator=(const IDataArray&); //Not Implemented

};

#endif //_IDATAARRAY_HPP_




