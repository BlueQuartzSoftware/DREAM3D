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

#ifndef _AttributeMatrix_H_
#define _AttributeMatrix_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif


//-- C++ includes
#include <vector>
#include <map>
#include <sstream>
#include <list>

//-- EBSD Lib Includes
#include "EbsdLib/EbsdConstants.h"


#include "H5Support/QH5Utilities.h"
#include "H5Support/HDF5ScopedFileSentinel.h"


//-- DREAM3D Includes
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/Observable.h"
#include "SIMPLib/DataContainers/DataContainerProxy.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

enum RenameErrorCodes
{
  OLD_DOES_NOT_EXIST,
  SUCCESS,
  NEW_EXISTS
};

/**
 * @class AttributeMatrix AttributeMatrix DREAM3DLib/DataContainers/AttributeMatrix.h
 * @brief This data container holds data the represents a structured rectangular
 * grid of data typically referred to as a Voxel Volume
 *
 * @author Michael A. Groeber for AFRL
 * @date
 * @version 1.0
 */
class SIMPLib_EXPORT AttributeMatrix : public Observable
{
  public:
    SIMPL_SHARED_POINTERS(AttributeMatrix)
    //   SIMPL_STATIC_NEW_MACRO(AttributeMatrix)
    SIMPL_TYPE_MACRO_SUPER(AttributeMatrix, Observable)

    virtual ~AttributeMatrix();

    /**
     * @brief New Creates an AttributeMatrix with the give name
     * @param tupleDims The dimensions of the Attribute matrix given in the order fastest moving to slowest moving (XYZ)
     * @param name The name of the AttributeMatrix. Each AttributeMatrix should have a unique name.
     * @param attrType The type of AttributeMatrix, one of
     * @li const unsigned int Vertex = 0; //!<
     * @li const unsigned int Edge = 1; //!<
     * @li const unsigned int Face = 2; //!<
     * @li const unsigned int Cell = 3; //!<
     * @li const unsigned int VertexFeature = 4; //!<
     * @li const unsigned int EdgeFeature = 5; //!<
     * @li const unsigned int FaceFeature = 6; //!<
     * @li const unsigned int CellFeature = 7; //!<
     * @li const unsigned int VertexEnsemble = 8; //!<
     * @li const unsigned int EdgeEnsemble = 9; //!<
     * @li const unsigned int FaceEnsemble = 10; //!<
     * @li const unsigned int CellEnsemble = 11; //!<
     * @li const unsigned int Unknown = 999; //!<
     *
     * @see DREAM3DLib/Common/Constants.h file.
     * @return
     */
    static Pointer New(QVector<size_t> tupleDims, const QString& name, unsigned int attrType)
    {
      Pointer sharedPtr(new AttributeMatrix(tupleDims, name, attrType));
      return sharedPtr;
    }

    /**
     * @brief ReadAttributeMatrixStructure
     * @param containerId
     * @param dataContainer
     * @param h5InternalPath
     */
    static void ReadAttributeMatrixStructure(hid_t containerId, DataContainerProxy& dcProxy, QString h5InternalPath);

    /**
     * @brief setType
     * @param value
     */
    void setType(uint32_t value);

    /**
     * @brief getType
     * @return
     */
    uint32_t getType();


    /**
    * @param name
    */
    SIMPL_INSTANCE_PROPERTY(QString, Name)


    /**
    * @brief Adds/overwrites the data for a named array
    * @param name The name that the array will be known by
    * @param data The IDataArray::Pointer that will hold the data
    * @return error code if the addition did not work
    */
    virtual int addAttributeArray(const QString& name, IDataArray::Pointer data);

    /**
     * @brief Returns the array for a given named array or the equivelant to a
     * null pointer if the name does not exist.
     * @param name The name of the data array
     */
    virtual IDataArray::Pointer getAttributeArray(const QString& name);


    /**
    * @brief returns a IDataArray based object that is stored in the attribute matrix by a
    * given name.
    * @param name The name of the array
    */
    template<class ArrayType>
    typename ArrayType::Pointer getAttributeArrayAs(const QString& name)
    {
      IDataArray::Pointer iDataArray = getAttributeArray(name);
      return std::dynamic_pointer_cast< ArrayType >(iDataArray);
    }


    /**
     * @brief Returns bool of whether a named array exists
     * @param name The name of the data array
     */
    virtual bool doesAttributeArrayExist(const QString& name);


    /**
    * @brief Removes the named data array from the Data Container and returns it to the calling
    * method.
    * @param name The name of the array
    * @return
    */
    virtual IDataArray::Pointer removeAttributeArray(const QString& name);

    /**
    * @brief Renames a cell data array from the Data Container
    * @param name The name of the array
    */
    virtual RenameErrorCodes renameAttributeArray(const QString& oldname, const QString& newname, bool overwrite = false);


    /**
     * @brief Removes all the Cell Arrays
     */
    virtual void clearAttributeArrays();

    /**
    * @brief Returns a list that contains the names of all the arrays currently stored in the
    * Cell (Formerly Cell) group
    * @return
    */
    virtual QList<QString> getAttributeArrayNames();

    /**
    * @brief Returns the total number of arrays that are stored in the Cell group
    * @return
    */
    virtual int getNumAttributeArrays();


    /**
    * @brief Resizes an array from the Attribute Matrix
    * @param size The new size of the array
    */
    void resizeAttributeArrays(QVector<size_t> tDims);

    /**
     * @brief Returns bool of whether a named array exists
     * @param name The name of the data array
     */
    virtual bool validateAttributeArraySizes();

    /**
     * @brief getPrereqArray
     * @param filter An instance of an AbstractFilter that is calling this function. Can be NULL in which case
     * no error message will be returned if there is an error.
     * @param attributeArrayName The name of the Attribute Array
     * @param err The error code to set into the filter if there is an error
     * @param cDims The component Dimensions of the Data Array
     * @return A valid IDataArray Subclass if the array exists otherwise a null shared pointer.
     */
    template<class ArrayType, class Filter>
    typename ArrayType::Pointer getPrereqArray(Filter* filter,
                                               QString attributeArrayName,
                                               int err,
                                               QVector<size_t> cDims)
    {
      QString ss;
      typename ArrayType::Pointer attributeArray = ArrayType::NullPointer();
      //Make sure the name is not empty for the AttributeArrayName. This would be detected below
      // in the call to get either one BUT the reason for the failure would not be evident so we make these explicit checks
      // here and send back nice error messages to ther user/programmer.
      if (attributeArrayName.isEmpty() == true)
      {
        if(filter)
        {
          filter->setErrorCondition(err);
          ss = QObject::tr("AttributeMatrix:'%1' The name of a requested Attribute Array was empty. Please provide a name for this array").arg(getName());
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
      }
      // Now ask for the actual AttributeArray from the AttributeMatrix
      if (doesAttributeArrayExist(attributeArrayName) == false)
      {
        if(filter)
        {
          filter->setErrorCondition(err);
          ss = QObject::tr("The AttributeMatrix named '%1' does NOT have a DataArray with name '%2'. This filter requires this DataArray in order to execute.").arg(getName()).arg(attributeArrayName);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return attributeArray;
      }
      int NumComp = cDims[0];
      for(int i = 1; i < cDims.size(); i++)
      {
        NumComp *= cDims[i];
      }
      // Check to make sure the AttributeArray we have is of the proper type, size and number of components
      if(false == dataArrayCompatibility<ArrayType, Filter>(attributeArrayName, NumComp, filter) )
      {
        return attributeArray;
      }
      IDataArray::Pointer iDataArray = getAttributeArray(attributeArrayName);
      attributeArray = std::dynamic_pointer_cast< ArrayType >(iDataArray);
      if(NULL == attributeArray.get() && filter)
      {
        filter->setErrorCondition(err);
        ss = QObject::tr("The AttributeMatrix named '%1' contains an array with name '%2' but the DataArray could not be downcast using std::dynamic_pointer_cast<T>.").arg(getName()).arg(attributeArrayName);
        filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
      }
      return attributeArray;
    }

    /**
     * @brief getExistingPrereqArray
     * @param filter
     * @param attributeArrayName
     * @param err
     * @return
     */
    template<class ArrayType, class Filter>
    typename ArrayType::Pointer getPrereqIDataArray(Filter* filter,
                                                    QString attributeArrayName,
                                                    int err)
    {
      QString ss;
      typename ArrayType::Pointer attributeArray = ArrayType::NullPointer();
      // Make sure the name is not empty for the AttributeArrayName. This would be detected below
      // in the call to get either one BUT the reason for the failure would not be evident so we make these explicit checks
      // here and send back nice error messages to ther user/programmer.
      if (attributeArrayName.isEmpty() == true)
      {
        if(filter)
        {
          filter->setErrorCondition(err);
          ss = QObject::tr("AttributeMatrix:'%1' The name of a requested Attribute Array was empty. Please provide a name for this array").arg(getName());
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
      }
      // Now ask for the actual AttributeArray from the AttributeMatrix
      if (doesAttributeArrayExist(attributeArrayName) == false)
      {
        if(filter)
        {
          filter->setErrorCondition(err);
          ss = QObject::tr("The AttributeMatrix named '%1' does NOT have a DataArray with name '%2'. This filter requires this DataArray in order to execute.").arg(getName()).arg(attributeArrayName);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return attributeArray;
      }
      else
      {
        IDataArray::Pointer ptr = getAttributeArray(attributeArrayName);
        if (std::dynamic_pointer_cast<ArrayType>(ptr) != NULL)
        {
          return std::dynamic_pointer_cast<ArrayType>(ptr);
        }
        else
        {
          filter->setErrorCondition(err);
          ss = QObject::tr("Unable to cast input array %1 to the necessary type.").arg(attributeArrayName);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
      }
      return attributeArray;
    }

    /**
     * @brief createNonPrereqArray This method will create a new DataArray in the AttributeMatrix. The condition for this
     * method to work properly is the name of the attribute array is not empty
     * @param filter The instance of the filter the filter that is requesting the new array
     * @param attributeArrayName The name of the AttributeArray to create
     * @param initValue The initial value of all the elements of the array
     * @param size The number of tuples in the Array
     * @param dims The dimensions of the components of the AttributeArray
     * @return A Shared Pointer to the newly created array
     */
    template<class ArrayType, class Filter, typename T>
    typename ArrayType::Pointer createNonPrereqArray(Filter* filter,
                                                     const QString& attributeArrayName,
                                                     T initValue,
                                                     QVector<size_t> compDims)
    {
      typename ArrayType::Pointer attributeArray = ArrayType::NullPointer();

      QString ss;
      if (attributeArrayName.isEmpty() == true)
      {
        if(filter)
        {
          filter->setErrorCondition(-10002);
          ss = QObject::tr("The name of the array was empty. Please provide a name for this array.");
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return attributeArray;
      }
      IDataArray::Pointer iDataArray = getAttributeArray(attributeArrayName);
      if (NULL == iDataArray.get())
      {
        createAndAddAttributeArray<ArrayType, Filter, T>(filter, attributeArrayName, initValue, compDims);
      }
      else if (filter)
      {
        filter->setErrorCondition(-10002);
        ss = QObject::tr("An Attribute Array already exists with the name %1.").arg(attributeArrayName);
        filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        return attributeArray;
      }
      iDataArray = getAttributeArray(attributeArrayName);
      if(NULL == iDataArray && filter)
      {
        filter->setErrorCondition(-10003);
        ss = QObject::tr("AttributeMatrix:'%1' An array with name '%2' could not be created.").arg(getName()).arg(attributeArrayName);
        filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
      }
      attributeArray = std::dynamic_pointer_cast< ArrayType >(iDataArray);
      if(NULL == attributeArray.get() && filter)
      {
        filter->setErrorCondition(-10003);
        ss = QObject::tr("AttributeMatrix:'%1' An array with name '%2' could not be downcast using std::dynamic_pointer_cast<T>.").arg(getName()).arg(attributeArrayName);
        filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
      }
      return attributeArray;
    }

    /**
    * @brief Creates and Adds the data for a named array
    * @param name The name that the array will be known by
    * @param dims The size the data on each tuple
    */
    template<class ArrayType, class Filter, typename T>
    void createAndAddAttributeArray(Filter* filter, const QString& name, T initValue, QVector<size_t> compDims)
    {
      bool allocateData = false;
      if(NULL == filter) { allocateData = true; }
      else { allocateData = !filter->getInPreflight(); }
      typename ArrayType::Pointer attributeArray = ArrayType::CreateArray(getNumTuples(), compDims, name, allocateData);
      if(attributeArray.get() != NULL)
      {
        if(allocateData)
        {
          attributeArray->initializeWithValue(initValue);
        }
        attributeArray->setInitValue(initValue);
        addAttributeArray(name, attributeArray);
      }
    }

    /**
     * @brief dataArrayCompatibility
     * @param arrayName
     * @param numComp
     * @param filter
     * @return
     */
    template<class ArrayType, class AbstractFilter>
    bool dataArrayCompatibility(const QString& arrayName, int numComp, AbstractFilter* filter)
    {
      // First try checking by name
      // IDataArray::Pointer iDataArray = ;
      typename ArrayType::Pointer targetDestArray = std::dynamic_pointer_cast< ArrayType >(getAttributeArray(arrayName));
      typename ArrayType::Pointer validTargetArray = ArrayType::CreateArray(1, "JUNK_INTERNAL_ARRAY", false);

      if (targetDestArray.get() == 0)
      {
        if (NULL != filter)
        {
          IDataArray::Pointer srcArray = getAttributeArray(arrayName);
          QString srcDesc = srcArray->getTypeAsString();
          QString desc = validTargetArray->getTypeAsString();
          QString ss = QObject::tr("The Filter '%1' requires an array of type '%2' but the data array '%3' has a type of '%4'").arg(filter->getHumanLabel()).arg(desc).arg(srcArray->getName()).arg(srcDesc);
          filter->setErrorCondition(-501);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return false;
      }
      // Make sure the sizes are equal to what is being asked for
      if (getNumTuples() != targetDestArray->getNumberOfTuples())
      {
        if (NULL != filter)
        {
          QString ss = QObject::tr("Filter '%1' requires array with name '%2' to have Number of Tuples = %3. The currently selected array "
                                   " has %4").arg(filter->getHumanLabel()).arg(arrayName).arg((getNumTuples())).arg(targetDestArray->getNumberOfTuples());
          filter->setErrorCondition(-502);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return false;
      }
      // Make sure the number of components match
      if (numComp != targetDestArray->getNumberOfComponents())
      {
        if (NULL != filter)
        {
          QString ss = QObject::tr("Filter '%1' is trying to use array '%2' where the number of components is %3 but the filter requires that array "
                                   " to have %4.").arg(filter->getHumanLabel()).arg(targetDestArray->getName()).arg(targetDestArray->getNumberOfComponents()).arg(numComp);
          filter->setErrorCondition(-503);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return false;
      }
      // Make sure we can downcast to the proper type
      ArrayType* array = ArrayType::SafePointerDownCast(targetDestArray.get());
      if (NULL == array)
      {
        typename ArrayType::Pointer dat = ArrayType::CreateArray(1, "JUNK-INTERNAL-USE-ONLY");
        QString ss = QObject::tr(" - The filter requested an array named '%1' with type '%2' from the filter '%3'.\n"
                                 "An Array with name '%4' is stored in the %5 but is of type %6\n")
                     .arg(arrayName).arg(dat->getTypeAsString()).arg(getNameOfClass()).arg(arrayName).arg(getNameOfClass()).arg(targetDestArray->getTypeAsString());
        if (NULL != filter)
        {
          filter->setErrorCondition(-504);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return false;
      }
      return true;
    }

    /**
    * @brief Removes inactive objects from the Attribute Matrix and renumbers the active objects to preserve a compact matrix
      (only valid for feature or ensemble type matrices)
    * @param size The new size of the array
    */
    bool removeInactiveObjects(QVector<bool> activeObjects, Int32ArrayType::Pointer Ids);

    /**
     * @brief Sets the Tuple Dimensions for the Attribute Matrix
     * @param tupleDims
     */
    void setTupleDimensions(QVector<size_t> tupleDims);

    /**
     * @brief Returns the Tuple Dimensions of the AttributeMatrix
     * @return
     */
    QVector<size_t> getTupleDimensions();


    /**
    * @brief Returns the number of Tuples that the feature data has. For example if there are 32 features
    * in during a set of filtering operations then the a value of '32' would be returned.
    * @return
    */
    size_t getNumTuples();

    /**
    * @brief creates and returns a copy of the attribute matrix
    * @return On error, will return a null pointer.  It is the responsibility of the calling function to check for errors and return an error message using the PipelineMessage
    */
    virtual AttributeMatrix::Pointer deepCopy();

    /**
     * @brief writeAttributeArraysToHDF5
     * @param parentId
     * @return
     */
    virtual int writeAttributeArraysToHDF5(hid_t parentId);

    /**
     * @brief addAttributeArrayFromHDF5Path
     * @param gid
     * @param name
     * @param preflight
     * @return
     */
    virtual int addAttributeArrayFromHDF5Path(hid_t gid, QString name, bool preflight);

    /**
     * @brief readAttributeArraysFromHDF5
     * @param amGid
     * @param preflight
     * @param attrMatProxy
     * @return
     */
    virtual int readAttributeArraysFromHDF5(hid_t amGid, bool preflight, AttributeMatrixProxy& attrMatProxy);

    /**
     * @brief generateXdmfText
     * @param centering
     * @param dataContainerName
     * @param hdfFileName
     * @param gridType
     * @return
     */
    virtual QString generateXdmfText(const QString& centering, const QString& dataContainerName, const QString& hdfFileName, const uint8_t gridType = 0);

    /**
     * @brief getInfoString Returns a text string in the given format that has information
     * about the attribute Matrix
     * @param format A value from the DREAM3D::InfoStringFormat enumeration
     * @return
     */
    virtual QString getInfoString(DREAM3D::InfoStringFormat format);

  protected:
    AttributeMatrix(QVector<size_t> tDims, const QString& name, unsigned int attrType);

    /**
     * @brief writeXdmfAttributeData
     * @param array
     * @param centering
     * @param dataContainerName
     * @param hdfFileName
     * @param gridType
     * @return
     */
    virtual QString writeXdmfAttributeData(IDataArray::Pointer array, const QString& centering, const QString& dataContainerName, const QString& hdfFileName, const uint8_t gridType = 0);

    /**
     * @brief writeXdmfAttributeDataHelper
     * @param numComp
     * @param attrType
     * @param dataContainerName
     * @param array
     * @param centering
     * @param precision
     * @param xdmfTypeName
     * @param hdfFileName
     * @param gridType
     * @return
     */
    virtual QString writeXdmfAttributeDataHelper(int numComp, const QString& attrType, const QString& dataContainerName, IDataArray::Pointer array, const QString& centering, int precision, const QString& xdmfTypeName, const QString& hdfFileName, const uint8_t gridType = 0);

  private:
    QVector<size_t> m_TupleDims;
    QMap<QString, IDataArray::Pointer> m_AttributeArrays;
    uint32_t m_Type;

    AttributeMatrix(const AttributeMatrix&);
    void operator =(const AttributeMatrix&);
};

#endif
