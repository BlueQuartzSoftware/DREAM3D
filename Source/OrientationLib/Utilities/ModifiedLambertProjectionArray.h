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
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#ifndef _modifiedlambertprojectionarray_h_
#define _modifiedlambertprojectionarray_h_

#include <vector>

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "OrientationLib/OrientationLib.h"
#include "OrientationLib/Utilities/ModifiedLambertProjection.h"


/**
 * @brief The ModifiedLambertProjectionArray class
 */
class OrientationLib_EXPORT ModifiedLambertProjectionArray : public IDataArray
{
  public:
    SIMPL_SHARED_POINTERS(ModifiedLambertProjectionArray)
    SIMPL_STATIC_NEW_MACRO(ModifiedLambertProjectionArray)
    SIMPL_TYPE_MACRO_SUPER(ModifiedLambertProjectionArray, IDataArray)
    SIMPL_CLASS_VERSION(2)

    virtual ~ModifiedLambertProjectionArray();

    /**
     * @brief getXdmfTypeAndSize
     * @param xdmfTypeName
     * @param precision
     */
    void getXdmfTypeAndSize(QString& xdmfTypeName, int& precision);

    /**
     * @brief GetTypeName Returns a string representation of the type of data that is stored by this class. This
     * can be a primitive like char, float, int or the name of a class.
     * @return
     */
    virtual QString getTypeAsString();

    SIMPL_INSTANCE_PROPERTY(int, Phase)

    SIMPL_INSTANCE_PROPERTY(QVector<ModifiedLambertProjection::Pointer>, ModifiedLambertProjectionArray)

    /**
     * @brief createNewArray
     * @param numElements
     * @param rank
     * @param dims
     * @param name
     * @param allocate
     * @return
     */
    virtual IDataArray::Pointer createNewArray(size_t numElements, int rank, size_t* dims, const QString& name, bool allocate = true);

    /**
     * @brief createNewArray
     * @param numElements
     * @param dims
     * @param name
     * @param allocate
     * @return
     */
    virtual IDataArray::Pointer createNewArray(size_t numElements, std::vector<size_t> dims, const QString& name, bool allocate = true);

    /**
     * @brief createNewArray
     * @param numElements
     * @param dims
     * @param name
     * @param allocate
     * @return
     */
    virtual IDataArray::Pointer createNewArray(size_t numElements, QVector<size_t> dims, const QString& name, bool allocate = true);

    /**
    * @brief
    */
    virtual bool isAllocated();

    /**
     * @brief clearAll
     */
    void clearAll();

    /**
     * @brief setModifiedLambertProjection
     * @param index
     * @param ModifiedLambertProjection
     */
    void setModifiedLambertProjection(int index, ModifiedLambertProjection::Pointer ModifiedLambertProjection);

    /**
     * @brief fillArrayWithNewModifiedLambertProjection
     * @param n
     */
    void fillArrayWithNewModifiedLambertProjection(size_t n);

    /**
     * @brief getModifiedLambertProjection
     * @param idx
     * @return
     */
    ModifiedLambertProjection::Pointer getModifiedLambertProjection(int idx);

    /**
     * @brief operator []
     * @param idx
     * @return
     */
    ModifiedLambertProjection::Pointer operator[](int idx);

    /* **************** This is the interface for the IDataArray Class which MUST
     *  Be implemented. Most of it is useless and will simply ASSERT if called. */

    /**
     * @brief setName
     * @param name
     */
    void setName(const QString& name);

    /**
     * @brief getName
     * @return
     */
    QString getName();

    /**
     * @brief Makes this class responsible for freeing the memory.
     */
    virtual void takeOwnership();

    /**
     * @brief This class will NOT free the memory associated with the internal pointer.
     * This can be useful if the user wishes to keep the data around after this
     * class goes out of scope.
     */
    virtual void releaseOwnership();

    /**
     * @brief Returns a void pointer pointing to the index of the array. nullptr
     * pointers are entirely possible. No checks are performed to make sure
     * the index is with in the range of the internal data array.
     * @param i The index to have the returned pointer pointing to.
     * @return Void Pointer. Possibly nullptr.
     */
    virtual void* getVoidPointer(size_t i);

    /**
     * @brief Returns the number of Tuples in the array.
     */
    virtual size_t getNumberOfTuples();

    /**
     * @brief Return the number of elements in the array
     * @return
     */
    virtual size_t getSize();

    /**
     * @brief SetNumberOfComponents
     * @param nc
     */
    virtual void SetNumberOfComponents(int nc);

    /**
     * @brief getNumberOfComponents
     * @return
     */
    virtual int getNumberOfComponents();

    /**
     * @brief getComponentDimensions
     * @return
     */
    virtual QVector<size_t> getComponentDimensions();

    /**
     * @brief SetRank
     * @param rank
     */
    virtual void SetRank(int rank);

    /**
     * @brief getRank
     * @return
     */
    virtual int getRank();

    /**
     * @brief Returns the number of bytes that make up the data type.
     * 1 = char
     * 2 = 16 bit integer
     * 4 = 32 bit integer/Float
     * 8 = 64 bit integer/Double
     */
    virtual size_t getTypeSize();


    /**
     * @brief Removes Tuples from the Array. If the size of the vector is Zero nothing is done. If the size of the
     * vector is greater than or Equal to the number of Tuples then the Array is Resized to Zero. If there are
     * indices that are larger than the size of the original (before erasing operations) then an error code (-100) is
     * returned from the program.
     * @param idxs The indices to remove
     * @return error code.
     */
    virtual int eraseTuples(QVector<size_t>& idxs);

    /**
     * @brief Copies a Tuple from one position to another.
     * @param currentPos The index of the source data
     * @param newPos The destination index to place the copied data
     * @return
     */
    virtual int copyTuple(size_t currentPos, size_t newPos);

    /**
     * @brief copyData This method copies all data from the <b>sourceArray</b> into
     * the current array starting at the target destination tuple offset value.
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
    virtual bool copyData(size_t destTupleOffset, IDataArray::Pointer sourceArray);


    /**
     * @brief Splats the same value c across all values in the Tuple
     * @param i The index of the Tuple
     * @param c The value to splat across all components in the tuple
     */
    virtual void initializeTuple(size_t i, void* p);

    /**
     * @brief Sets all the values to zero.
     */
    virtual void initializeWithZeros();

    /**
     * @brief deepCopy
     * @return
     */
    virtual IDataArray::Pointer deepCopy(bool forceNoAllocate = false);

    /**
       * @brief Reseizes the internal array
       * @param size The new size of the internal array
       * @return 1 on success, 0 on failure
       */
    virtual int32_t resizeTotalElements(size_t size);

    /**
     * @brief Reseizes the internal array
     * @param size The new size of the internal array
     * @return 1 on success, 0 on failure
     */
    virtual int32_t resize(size_t numTuples);

    /**
     * @brief printTuple
     * @param out
     * @param i
     * @param delimiter
     */
    virtual void printTuple(QTextStream& out, size_t i, char delimiter = ',');

    /**
     * @brief printComponent
     * @param out
     * @param i
     * @param j
     */
    virtual void printComponent(QTextStream& out, size_t i, int j);

    /**
     *
     * @param parentId
     * @return
     */
    virtual int writeH5Data(hid_t parentId, QVector<size_t> tDims);

    /**
     * @brief readH5Data
     * @param parentId
     * @return
     */
    virtual int readH5Data(hid_t parentId);

    /**
     * @brief writeXdmfAttribute
     * @param out
     * @param volDims
     * @param hdfFileName
     * @param groupPath
     * @return
     */
    virtual int writeXdmfAttribute(QTextStream& out, int64_t* volDims, const QString& hdfFileName,
                                   const QString& groupPath, const QString& labelb);

    /**
     * @brief getInfoString
     * @return Returns a formatted string that contains general infomation about
     * the instance of the object.
     */
    virtual QString getInfoString(SIMPL::InfoStringFormat format);

  protected:
    ModifiedLambertProjectionArray();

  private:
    QString m_Name;
    bool m_IsAllocated;

    ModifiedLambertProjectionArray(const ModifiedLambertProjectionArray&); // Copy Constructor Not Implemented
    void operator=(const ModifiedLambertProjectionArray&); // Operator '=' Not Implemented
};

#endif /* _ModifiedLambertProjectionArray_H_ */

