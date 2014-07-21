/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
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
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/Observable.h"
#include "DREAM3DLib/DataContainers/DataContainerProxy.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"


/**
 * @class AttributeMatrix AttributeMatrix DREAM3DLib/DataContainers/AttributeMatrix.h
 * @brief This data container holds data the represents a structured rectangular
 * grid of data typically referred to as a Voxel Volume
 * @author Michael A. Jackson for BlueQuartz Software
 * @author Michael A. Groeber for AFRL
 * @date
 * @version 1.0
 */
class DREAM3DLib_EXPORT AttributeMatrix : public Observable
{
  public:
    DREAM3D_SHARED_POINTERS(AttributeMatrix)
    //   DREAM3D_STATIC_NEW_MACRO(AttributeMatrix)
    DREAM3D_TYPE_MACRO_SUPER(AttributeMatrix, Observable)

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
     * @brief getPrereqArray
     * @param filter
     * @param attributeMatrixName
     * @param attributeArrayName
     * @param err
     * @param size
     * @param dims
     * @return
     */
    template<class ArrayType, class Filter>
    typename ArrayType::Pointer getPrereqArray(Filter* filter,
                                               QString attributeArrayName,
                                               int err,
                                               QVector<size_t> dims)
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
          filter->setErrorCondition(err * 1010);
          ss = QObject::tr("AttributeMatrix:'%1' The name of a requested Attribute Array was empty. Please provide a name for this array").arg(getName());
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
      }
      // Now ask for the actual AttributeArray from the AttributeMatrix
      if (doesAttributeArrayExist(attributeArrayName) == false)
      {
        if(filter)
        {
          filter->setErrorCondition(err * 1020);
          ss = QObject::tr("The AttributeMatrix named '%1' does NOT have a DataArray with name '%2'. This filter requires this DataArray in order to execute.").arg(getName()).arg(attributeArrayName);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return attributeArray;
      }
      int NumComp = dims[0];
      for(int i = 1; i < dims.size(); i++)
      {
        NumComp *= dims[i];
      }
      // Check to make sure the AttributeArray we have is of the proper type, size and number of components
      if(false == dataArrayCompatibility<ArrayType, Filter>(attributeArrayName, NumComp, filter) )
      {
        if(filter)
        {
          filter->setErrorCondition(err * 1030);
          ss = QObject::tr("The AttributeMatrix named '%1' contains an array with name '%2' but this DataArray is not compatible with desired dimensions.").arg(getName()).arg(attributeArrayName);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return attributeArray;
      }
      IDataArray::Pointer iDataArray = getAttributeArray(attributeArrayName);
      attributeArray = boost::dynamic_pointer_cast< ArrayType >(iDataArray);
      if(NULL == attributeArray.get() && filter)
      {
        filter->setErrorCondition(err * 1040);
        ss = QObject::tr("The AttributeMatrix named '%1' contains an array with name '%2' but the DataArray could not be downcast using boost::dynamic_pointer_cast<T>.").arg(getName()).arg(attributeArrayName);
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
    typename ArrayType::Pointer getExistingPrereqArray(Filter* filter,
                                                       QString attributeArrayName,
                                                       int err)
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
          filter->setErrorCondition(err * 1010);
          ss = QObject::tr("AttributeMatrix:'%1' The name of a requested Attribute Array was empty. Please provide a name for this array").arg(getName());
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
      }
      // Now ask for the actual AttributeArray from the AttributeMatrix
      if (doesAttributeArrayExist(attributeArrayName) == false)
      {
        if(filter)
        {
          filter->setErrorCondition(err * 1020);
          ss = QObject::tr("The AttributeMatrix named '%1' does NOT have a DataArray with name '%2'. This filter requires this DataArray in order to execute.").arg(getName()).arg(attributeArrayName);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return attributeArray;
      }
      else
      {
        return getAttributeArray(attributeArrayName);
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
                                                     QVector<size_t> dims)
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
      createAndAddAttributeArray<ArrayType, Filter, T>(filter, attributeArrayName, initValue, dims);
      IDataArray::Pointer iDataArray = getAttributeArray(attributeArrayName);
      if(NULL == iDataArray && filter)
      {
        filter->setErrorCondition(-10003);
        ss = QObject::tr("AttributeMatrix:'%1' An array with name '%2' could not be created.").arg(getName()).arg(attributeArrayName);
        filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
      }
      attributeArray = boost::dynamic_pointer_cast< ArrayType >(iDataArray);
      if(NULL == attributeArray.get() && filter)
      {
        filter->setErrorCondition(-10003);
        ss = QObject::tr("AttributeMatrix:'%1' An array with name '%2' could not be downcast using boost::dynamic_pointer_cast<T>.").arg(getName()).arg(attributeArrayName);
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
      typename ArrayType::Pointer attributeArray = boost::dynamic_pointer_cast< ArrayType >(getAttributeArray(arrayName));

      if (attributeArray.get() == 0)
      {
        if (NULL != filter)
        {
          QString ss = QObject::tr("Filter '%1' requires an available array with name '%2'").arg(filter->getHumanLabel()).arg(arrayName);
          filter->setErrorCondition(-501);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return false;
      }
      // Make sure the sizes are equal to what is being asked for
      if (getNumTuples() != attributeArray->getNumberOfTuples())
      {
        if (NULL != filter)
        {
          QString ss = QObject::tr("Filter '%1' requires array with name '%2' to have Number of Tuples = %3. The currently available array "
                                   " has %4").arg(filter->getHumanLabel()).arg(arrayName).arg((getNumTuples())).arg(attributeArray->getNumberOfTuples());
          filter->setErrorCondition(-502);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return false;
      }
      // Make sure the number of components match
      if (numComp != attributeArray->getNumberOfComponents())
      {
        if (NULL != filter)
        {
          QString ss = QObject::tr("Filter '%1' is trying to use array '%2' where the number of components is %3 but the filter requires that array "
                                   " to have %4.").arg(filter->getHumanLabel()).arg(attributeArray->getName()).arg(attributeArray->getNumberOfComponents()).arg(numComp);
          filter->setErrorCondition(-503);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return false;
      }
      // Make sure we can downcast to the proper type
      ArrayType* array = ArrayType::SafePointerDownCast(attributeArray.get());
      if (NULL == array)
      {
        typename ArrayType::Pointer dat = ArrayType::CreateArray(1, "JUNK-INTERNAL-USE-ONLY");
        QString ss = QObject::tr(" - The filter requested an array named '%1' with type '%2' from the %3.\n"
                                 "An Array with name '%4' is stored in the %5 but is of type %6\n")
                     .arg(arrayName).arg(dat->getTypeAsString()).arg(getNameOfClass()).arg(arrayName).arg(getNameOfClass()).arg(attributeArray->getTypeAsString());
        if (NULL != filter)
        {
          filter->setErrorCondition(-504);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return false;
      }
      return true;
    }

    //DREAM3D_INSTANCE_PROPERTY(uint32_t, Type)
    void setType(uint32_t value);
    uint32_t getType();

    DREAM3D_INSTANCE_PROPERTY(QString, Name)

    /**
     * @brief Returns bool of whether a named array exists
     * @param name The name of the data array
     */
    virtual bool doesAttributeArrayExist(const QString& name);

    /**
     * @brief Returns bool of whether a named array exists
     * @param name The name of the data array
     */
    virtual bool validateAttributeArraySizes();

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
    typename ArrayType::Pointer getArray(const QString& name)
    {
      IDataArray::Pointer iDataArray = getAttributeArray(name);
      return boost::dynamic_pointer_cast< ArrayType >(iDataArray);
    }
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
    virtual bool renameAttributeArray(const QString& oldname, const QString& newname);

    /**
    * @brief Resizes an array from the Attribute Matrix
    * @param size The new size of the array
    */
    void resizeAttributeArrays(QVector<size_t> tDims);

    /**
    * @brief Removes inactive objects from the Attribute Matrix and renumbers the active objects to preserve a compact matrix
      (only valid for feature or ensemble type matrices)
    * @param size The new size of the array
    */
    bool removeInactiveObjects(QVector<bool> activeObjects, Int32ArrayType::Pointer Ids);

    void setTupleDimensions(QVector<size_t> tupleDims);

    QVector<size_t> getTupleDimensions();

    /**
     * @brief Removes all the Cell Arrays
     */
    virtual void clearAttributeArrays();

    /**
    * @brief Returns a list that contains the names of all the arrays currently stored in the
    * Cell (Formerly Cell) group
    * @return
    */
    virtual QList<QString> getAttributeArrayNameList();

    /**
    * @brief Returns the total number of arrays that are stored in the Cell group
    * @return
    */
    virtual int getNumAttributeArrays();

    /**
    * @brief Returns the number of Tuples that the feature data has. For example if there are 32 features
    * in during a set of filtering operations then the a value of '32' would be returned.
    * @return
    */
    size_t getNumTuples();

    /**
    * @brief creates and returns a copy of the attribute matrix
    * @return
    */
    virtual AttributeMatrix::Pointer deepCopy();

    virtual int writeAttributeArraysToHDF5(hid_t parentId);
    virtual int addAttributeArrayFromHDF5Path(hid_t gid, QString name, bool preflight);
    virtual int readAttributeArraysFromHDF5(hid_t amGid, bool preflight, AttributeMatrixProxy& attrMatProxy);
    virtual QString generateXdmfText(const QString& centering, const QString& dataContainerName, const QString& hdfFileName, const uint8_t gridType = 0);

  protected:
    AttributeMatrix(QVector<size_t> tDims, const QString& name, unsigned int attrType);

    virtual QString writeXdmfAttributeData(IDataArray::Pointer array, const QString& centering, const QString& dataContainerName, const QString& hdfFileName, const uint8_t gridType = 0);
    virtual QString writeXdmfAttributeDataHelper(int numComp, const QString& attrType, const QString& dataContainerName, IDataArray::Pointer array, const QString& centering, int precision, const QString& xdmfTypeName, const QString& hdfFileName, const uint8_t gridType = 0);

  private:
    QVector<size_t> m_TupleDims;
    QMap<QString, IDataArray::Pointer> m_AttributeArrays;
    uint32_t m_Type;

    AttributeMatrix(const AttributeMatrix&);
    void operator =(const AttributeMatrix&);
};

#endif
