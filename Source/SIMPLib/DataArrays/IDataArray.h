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
#include <QtCore/QString>
#include <QtCore/QtDebug>
#include <vector>

#include <hdf5.h>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/Constants.h"


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
class SIMPLib_EXPORT IDataArray
{
  public:
    SIMPL_SHARED_POINTERS(IDataArray)
    SIMPL_TYPE_MACRO(IDataArray)

    /**
     * This templated method is used to get at the low level pointer that points
     * to the actual data by testing the conversion with dynamic_cast<> first to
     * see if it can be done, the finally returns the low level pointer.
     * @code
     *    typedef DataArray<int32_t>  Int32ArrayType;
     *    int32_t* iPtr = IDataArray::SafeReinterpretCast<IDataArray*, Int32ArrayType*, int32_t*>(ptr.get());
    *     Q_ASSERT(NULL != iPtr);
     * @endcode
     * @param x The Pointer to IDataArray
     * @return
     */
    template <class Source, class Target, typename Raw>
    static Raw SafeReinterpretCast(Source x)
    {
      if( dynamic_cast<Target>(x) != x )
      {
        return 0;
      }
      return reinterpret_cast<Raw>(x->getVoidPointer(0));
    }


    IDataArray();
    virtual ~IDataArray();

    virtual void setName(const QString& name) = 0;
    virtual QString getName() = 0;

    virtual Pointer createNewArray(size_t numElements, int rank, size_t* dims, const QString& name, bool allocate = true) = 0;
    virtual Pointer createNewArray(size_t numElements, std::vector<size_t> dims, const QString& name, bool allocate = true) = 0;
    virtual Pointer createNewArray(size_t numElements, QVector<size_t> dims, const QString& name, bool allocate = true) = 0;

    virtual int getClassVersion() = 0;

    /**
     * @brief Has all the memory needed for this class been allocated?
     */
    virtual bool isAllocated() = 0;

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
    virtual void* getVoidPointer ( size_t i) = 0;

    /**
    * @brief Returns the number of Tuples in the array.
    */
    virtual size_t getNumberOfTuples () = 0;


    /**
     * @brief Return the number of elements in the array
     * @return
     */
    virtual size_t getSize() = 0;

    virtual int getNumberOfComponents() = 0;
    virtual QVector<size_t> getComponentDimensions() = 0;


    /**
     * @brief Returns the number of bytes that make up the data type.
     * 1 = char
     * 2 = 16 bit integer
     * 4 = 32 bit integer/Float
     * 8 = 64 bit integer/Double
     */
    virtual size_t getTypeSize() = 0;

    /**
     * @brief GetTypeName Returns a string representation of the type of data that is stored by this class. This
     * can be a primitive like char, float, int or the name of a class.
     * @return
     */
    virtual void getXdmfTypeAndSize(QString& xdmfTypeName, int& precision) = 0;

    /**
     * @brief Erases tuples based on a list of specific Tuple indices
     * @param idxs The indices to erase
     * @return
     */
    virtual int eraseTuples(QVector<size_t>& idxs) = 0;

    /**
     * @brief Copies a Tuple from one position to another.
     * @param currentPos The index of the source data
     * @param newPos The destination index to place the copied data
     * @return
     */
    virtual int copyTuple(size_t currentPos, size_t newPos) = 0;

    /**
     * @brief copyData This method copies all data from the <b>sourceArray</b> into
     * the current array starting at the target destination tuple offset value.
     *
     * The method will check to ensure that various conditions are properly met
     * before attempting to copy the data into the array at the give offset to avoid
     * the possibility of walking off the end of the array into application memory
     * or causing a General protection fault by the OS.
     *
     * For example if the DataArray has 10 tuples and the destTupleOffset = 5 then
     * then source data will be copied into the destination array starting at
     * destination tuple 5. In psuedo code it would be the following:
     * @code
     *  destArray[5] = sourceArray[0];
     *  destArray[6] = sourceArray[1];
     *  .....
     * @endcode
     * @param destTupleOffset
     * @param sourceArray
     * @return
     */
    virtual bool copyData(size_t destTupleOffset, IDataArray::Pointer sourceArray) = 0;


    /**
     * @brief Splats the same value c across all values in the Tuple
     * @param pos The index of the Tuple
     * @param value pointer to value
     */
    virtual void initializeTuple(size_t pos, double value) = 0;

    /**
     * @brief Sets all the values to zero.
     */
    virtual void initializeWithZeros() = 0;

    /**
     * @brief Resizes the internal array
     * @param size The new size of the internal array
     * @return 1 on success, 0 on failure
     */
    virtual int32_t resizeTotalElements(size_t size) = 0;

    /**
     * @brief Reseizes the internal array
     * @param size The new size of the internal array
     * @return 1 on success, 0 on failure
     */
    virtual int32_t resize(size_t numTuples) = 0;

    /**
     * @brief printTuple
     * @param out
     * @param i
     * @param delimiter
     */
    virtual void printTuple(QTextStream& out, size_t i, char delimiter = ',') = 0;

    /**
     * @brief printComponent
     * @param out
     * @param i
     * @param j
     */
    virtual void printComponent(QTextStream& out, size_t i, int j) = 0;

    /**
     * @brief deepCopy
     * @param forceNoAllocate
     * @return
     */
    virtual IDataArray::Pointer deepCopy(bool forceNoAllocate = false) = 0;

    /**
     * @brief writeH5Data
     * @param parentId
     * @param tDims
     * @return
     */
    virtual int writeH5Data(hid_t parentId, QVector<size_t> tDims) = 0;

    /**
     * @brief readH5Data
     * @param parentId
     * @return
     */
    virtual int readH5Data(hid_t parentId) = 0;

    /**
     * @brief writeXdmfAttribute
     * @param out
     * @param volDims
     * @param hdfFileName
     * @param groupPath
     * @param label
     * @return
     */
    virtual int writeXdmfAttribute(QTextStream& out, int64_t* volDims, const QString& hdfFileName, const QString& groupPath, const QString& label) = 0;

    /**
     * @brief getTypeAsString
     * @return
     */
    virtual QString getTypeAsString() = 0;

    /**
     * @brief getInfoString
     * @return Returns a formatted string that contains general infomation about
     * the instance of the object.
     */
    virtual QString getInfoString(DREAM3D::InfoStringFormat format) = 0;

  protected:

  private:
    IDataArray (const IDataArray&);    //Not Implemented
    void operator=(const IDataArray&); //Not Implemented

};



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
class CheckDataArrayType
{
  public:
    CheckDataArrayType() {}
    virtual ~CheckDataArrayType() {}
    bool operator()(IDataArray::Pointer p)
    {
      return (std::dynamic_pointer_cast<T>(p).get() != NULL);
    }
};



#endif //_IDATAARRAY_HPP_




