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


#ifndef _DATACONTAINERARRAY_H_
#define _DATACONTAINERARRAY_H_

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QList>


#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"
#include "SIMPLib/DataContainers/IDataContainerBundle.h"


class DataArrayPath;
/**
 * @class DataContainerArray DataContainerArray.h DREAM3DLib/Common/DataContainerArray.h
 * @brief  This class holds the list of filters that will be run. This should be
 * the class (or a subclass) that you use to build up a pipeline in order to run
 * it.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Sep 28, 2011
 * @version 1.0
 */
class SIMPLib_EXPORT DataContainerArray : public QObject
{
    Q_OBJECT

  public:
    SIMPL_SHARED_POINTERS(DataContainerArray)
    SIMPL_STATIC_NEW_MACRO(DataContainerArray)
    SIMPL_TYPE_MACRO(DataContainerArray)

    virtual ~DataContainerArray();

    /**
     * @brief
     */
    virtual void addDataContainer(DataContainer::Pointer f);

    /**
     * @brief getDataContainer
     * @param path Uses the DataContainerName from the DataArrayPath to return a data container
     * @return
     */
    virtual DataContainer::Pointer getDataContainer(const DataArrayPath& path);

    /**
     * @brief getDataContainer
     * @param name
     * @return
     */
    virtual DataContainer::Pointer getDataContainer(const QString& name);

    /**
     * @brief getDataContainers
     * @return
     */
    QList<DataContainer::Pointer>& getDataContainers();

    /**
     * @brief Returns if a DataContainer with the give name is in the array
     * @param name The name of the DataContiner to find
     * @return
     */
    virtual bool doesDataContainerExist(const QString& name);

    /**
     * @brief DataContainerArray::doesAttributeMatrixExist
     * @param path
     * @return
     */
    virtual bool doesAttributeMatrixExist(const DataArrayPath& path);

    /**
     * @brief doesAttributeArrayExist
     * @param path
     * @return
     */
    virtual bool doesAttributeArrayExist(const DataArrayPath& path);

    /**
     * @brief
     */
    virtual DataContainer::Pointer removeDataContainer(const QString& name);

    /**
     * @brief renameDataContainer
     * @param oldName
     * @param newName
     * @return
     */
    bool renameDataContainer(const QString& oldName, const QString& newName);

    /**
     * @brief Removes all DataContainers from this DataContainerArray
     */
    virtual void clearDataContainers();

    /**
     * @brief getDataContainerNames
     * @return
     */
    QList<QString> getDataContainerNames();

    /**
     * @brief Returns the number of DataContainers
     * @return
     */
    virtual int getNumDataContainers();

    /**
     * @brief duplicateDataContainer
     * @param name
     * @param newName
     */
    virtual void duplicateDataContainer(const QString& name, const QString& newName);


    //////////////////////  AttributeMatrix Functions //////////////////////////
    /**
     * @brief getAttributeMatrix
     * @param path
     * @return
     */
    virtual AttributeMatrix::Pointer getAttributeMatrix(const DataArrayPath& path);

    /**
     * @brief printDataContainerNames
     * @param out
     */
    virtual void printDataContainerNames(QTextStream& out);

    /**
    * @brief Reads desired the DataContainers from HDF5 file
     * @param preflight
     * @param dcaGid
     * @param dcaProxy
     * @param obs
     * @return
     */
    virtual int readDataContainersFromHDF5(bool preflight,
                                           hid_t dcaGid,
                                           DataContainerArrayProxy& dcaProxy,
                                           Observable* obs = NULL);


    /**
     * @brief setDataContainerBundles
     * @param bundles
     */
    void setDataContainerBundles(QMap<QString, IDataContainerBundle::Pointer> bundles);

    /**
     * @brief getDataContainerBundles
     * @return
     */
    QMap<QString, IDataContainerBundle::Pointer>& getDataContainerBundles();

    /**
    * @brief getDataContainerBundle
    * @param name
    * @return
    */
    IDataContainerBundle::Pointer getDataContainerBundle(const QString& name);

    template<typename BundleType>
    typename BundleType::Pointer getDataContainerBundleAs(const QString& name)
    {
      typename BundleType::Pointer dcb = std::dynamic_pointer_cast<BundleType>(getDataContainerBundle(name));
      return dcb;
    }

    /**
     * @brief addDataContainerBundle
     * @param dataContainer
     */
    void addDataContainerBundle(IDataContainerBundle::Pointer dataContainerBundle);

    /**
     * @brief removeDataContainerBundle
     * @param name
     * @return
     */
    int removeDataContainerBundle(const QString& name);

    /**
     * @brief renameDataContainerBundle
     * @param oldName
     * @param newName
     */
    bool renameDataContainerBundle(const QString& oldName, const QString newName);

    /**
     * @brief removeDataContainerFromBundle
     * @param name
     */
    void removeDataContainerFromBundles(const QString& name);

    /**
     * @brief getPrereqDataContainer
     * @param name
     * @param createIfNotExists
     * @return
     */
    template<typename Filter>
    DataContainer::Pointer getPrereqDataContainer(Filter* filter, const QString& name, bool createIfNotExists = false)
    {
      DataContainer::Pointer dc = getDataContainer(name);
      if(NULL == dc.get() && createIfNotExists == false)
      {
        if (filter)
        {
          filter->setErrorCondition(-999);
          QString ss = "The DataContainer Object with the specific name '" + name + "' was not available.";
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return dc;
      }
      else if(NULL != dc && createIfNotExists == true)
      {
        DataContainer::Pointer dataContainer = DataContainer::New(name); // Create a new Data Container
        addDataContainer(dataContainer); // Put the new DataContainer into the array
        return dataContainer; // Return the wrapped pointer
      }
      // The DataContainer we asked for was present and NON Null so return that.
      return dc;
    }

    /**
     * @brief This function will create a new DataContainer of type <T>
     * @param dataContainerName The name of the DataContainer. Must not be empty or this method will ASSERT()
     * @return Valid DataContainer Object UNLESS the DataContainer with the given name already exists or the
     * dataContainerName is empty in which case a Null DataContainer will be returned.
     */
    template<typename Filter>
    DataContainer::Pointer createNonPrereqDataContainer(Filter* filter, const QString& dataContainerName)
    {
      if(dataContainerName.isEmpty())
      {
        if (filter)
        {
          filter->setErrorCondition(-887);
          QString ss = QObject::tr("The DataContainer Object must have a name to be created.");
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
          return DataContainer::NullPointer();
        }
      }

      if(doesDataContainerExist(dataContainerName) == true)
      {
        if (filter)
        {
          filter->setErrorCondition(-888);
          QString ss = QObject::tr("The DataContainer Object with the specific name '%1' already exists.").arg(dataContainerName);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
          return DataContainer::NullPointer();
        }
      }
      DataContainer::Pointer dataContainer = DataContainer::New(dataContainerName);
      addDataContainer(dataContainer);
      return dataContainer;
    }


    /**
     * @brief getPrereqGeometryFromDataContainer Returns an IGeometry object of the templated type
     * if it is available for the given DataContainer
     * @param filter
     * @param dcName
     * @return
     */
    template<typename GeometryType, typename Filter>
    typename GeometryType::Pointer getPrereqGeometryFromDataContainer(Filter* filter, const QString& dcName)
    {
      typename GeometryType::Pointer geom = GeometryType::NullPointer();
      DataContainer::Pointer dc = getPrereqDataContainer<Filter>(filter, dcName, false);
      if(NULL == dc) { return geom; }

      return dc->getPrereqGeometry<GeometryType>(filter);
    }


    /**
     * @brief getPrereqAttributeMatrixFromPath This function will return an AttributeMatrix if it is availabe
     * at the path
     * @param filter An AbstractFilter or subclass where error messages and error codes can be sent
     * @param path The DataArrayPath object that has the path to the AttributeMatrix
     * @param err The error code to display to the user
     * @return
     */
    template<typename Filter>
    AttributeMatrix::Pointer getPrereqAttributeMatrixFromPath(Filter* filter, const DataArrayPath& path, int err)
    {
      // First try to get the Parent DataContainer. If an error occurs the error message will have been set
      // so just return a NULL shared pointer
      DataContainer::Pointer dc = getPrereqDataContainer<Filter>(filter, path.getDataContainerName(), false);
      if(NULL == dc) { return AttributeMatrix::NullPointer(); }

      // Now just return what ever the DataContainer gives us. if the AttributeMatrix was not available then an
      // error message and code will have been set into the "filter" object if that object was non-null itself.
      return dc->getPrereqAttributeMatrix<Filter>(filter, path.getAttributeMatrixName(), err);
    }

    /**
     * @brief getPrereqArrayFromPath
     * @param filter Instance of an AbstractFilter. Can be NULL
     * @param path The path to the IDataArray
     * @param cDims The component dimensions of the IDataArray subclass
     * @return Valid or NULL shared pointer based on availability of the array
     */
    template<class ArrayType, class Filter>
    typename ArrayType::Pointer getPrereqArrayFromPath(Filter* filter, const DataArrayPath& path, QVector<size_t> cDims)
    {

      QString ss;
      typename ArrayType::Pointer dataArray = ArrayType::NullPointer();

      if(path.isEmpty() == true)
      {
        if(filter)
        {
          filter->setErrorCondition(-80000);
          ss = QObject::tr("DataContainerArray::getPrereqArrayFromPath Error at line %1. The DataArrayPath object was empty").arg(__LINE__);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return dataArray;
      }

      if(path.isValid() == false)
      {
        if(filter)
        {
          filter->setErrorCondition(-80001);
          ss = QObject::tr("DataContainerArray::getPrereqArrayFromPath Error at line %1. The DataArrayPath object was not valid meaning one of the strings in the object is empty. The path is %2").arg(__LINE__).arg(path.serialize());
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return dataArray;
      }

      QString dcName = path.getDataContainerName();
      QString amName = path.getAttributeMatrixName();
      QString daName = path.getDataArrayName();


      DataContainer::Pointer dc = getDataContainer(dcName);
      if(NULL == dc.get())
      {
        if(filter)
        {
          filter->setErrorCondition(-80002);
          ss = QObject::tr("The DataContainer '%1' was not found in the DataContainerArray").arg(dcName);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return dataArray;
      }

      AttributeMatrix::Pointer attrMat = dc->getAttributeMatrix(amName);
      if(NULL == attrMat.get())
      {
        if(filter)
        {
          filter->setErrorCondition(-80003);
          ss = QObject::tr("The AttributeMatrix '%1' was not found in the DataContainer '%2'").arg(amName).arg(dcName);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return dataArray;
      }

      dataArray = attrMat->getPrereqArray<ArrayType, Filter>(filter, daName, -80002, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
      return dataArray;
    }

    /**
    * @brief getPrereqIDataArrayFromPath
    * @param filter
    * @param path
    * @return
    */
    template<class ArrayType, class Filter>
    typename ArrayType::Pointer getPrereqIDataArrayFromPath(Filter* filter, const DataArrayPath& path)
    {

      QString ss;
      typename ArrayType::Pointer dataArray = ArrayType::NullPointer();

      if(path.isEmpty() == true)
      {
        if(filter)
        {
          filter->setErrorCondition(-90000);
          ss = QObject::tr("DataContainerArray::getPrereqArrayFromPath Error at line %1. The DataArrayPath object was empty").arg(__LINE__);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return dataArray;
      }

      if(path.isValid() == false)
      {
        if(filter)
        {
          filter->setErrorCondition(-90001);
          ss = QObject::tr("DataContainerArray::getPrereqArrayFromPath Error at line %1. The DataArrayPath object was not valid meaning one of the strings in the object is empty. The path is %2").arg(__LINE__).arg(path.serialize());
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return dataArray;
      }

      QString dcName = path.getDataContainerName();
      QString amName = path.getAttributeMatrixName();
      QString daName = path.getDataArrayName();


      DataContainer::Pointer dc = getDataContainer(dcName);
      if(NULL == dc.get())
      {
        if(filter)
        {
          filter->setErrorCondition(-999);
          ss = QObject::tr("The DataContainer '%1' was not found in the DataContainerArray").arg(dcName);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return dataArray;
      }

      AttributeMatrix::Pointer attrMat = dc->getAttributeMatrix(amName);
      if(NULL == attrMat.get())
      {
        if(filter)
        {
          filter->setErrorCondition(-307020);
          ss = QObject::tr("The AttributeMatrix '%1' was not found in the DataContainer '%2'").arg(amName).arg(dcName);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return dataArray;
      }

      dataArray = attrMat->getPrereqIDataArray<ArrayType, Filter>(filter, daName, -90002); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
      return dataArray;
    }

    /**
     * @brief createNonPrereqArray This method will create a new DataArray in the AttributeMatrix. The conditions for this
     * method to work properly include: a valid DataArrayPath is supplied, the name of the attribute array is not empty,
     * and an array with the same name cannot already exist
     * @param filter The instance of the filter the filter that is requesting the new array
     * @param attributeArrayName The name of the AttributeArray to create
     * @param initValue The initial value of all the elements of the array
     * @param size The number of tuples in the Array
     * @param dims The dimensions of the components of the AttributeArray
     * @return A Shared Pointer to the newly created array
     */
    template<class ArrayType, class Filter, typename T>
    typename ArrayType::Pointer createNonPrereqArrayFromPath(Filter* filter,
                                                             const DataArrayPath& path,
                                                             T initValue,
                                                             QVector<size_t> compDims,
                                                             const QString property = "")
    {
      typename ArrayType::Pointer dataArray = ArrayType::NullPointer();
      QString ss;
      if(path.isValid() == false)
      {
        if(filter)
        {
          filter->setErrorCondition(-80010);
          ss = QObject::tr("Property '%1': The DataArrayPath is invalid because one of the elements was empty.\n  DataContainer: %2\n  AttributeMatrix: %3\n  DataArray: %4").arg(property).arg(path.getDataContainerName()).arg(path.getAttributeMatrixName()).arg(path.getDataArrayName());
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return dataArray;
      }

      DataContainer::Pointer dc = getDataContainer(path.getDataContainerName());
      if(NULL == dc.get())
      {
        if(filter)
        {
          filter->setErrorCondition(-80002);
          ss = QObject::tr("The DataContainer '%1' was not found in the DataContainerArray").arg(path.getDataContainerName());
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return dataArray;
      }

      AttributeMatrix::Pointer attrMat = dc->getAttributeMatrix(path.getAttributeMatrixName());
      if(NULL == attrMat.get())
      {
        if(filter)
        {
          filter->setErrorCondition(-80003);
          ss = QObject::tr("The AttributeMatrix '%1' was not found in the DataContainer '%2'").arg(path.getAttributeMatrixName()).arg(path.getDataContainerName());
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return dataArray;
      }

      // If something goes wrong at this point the error message will be directly set in the 'filter' object so we just
      // simply return what ever is given to us.
      dataArray = attrMat->createNonPrereqArray<ArrayType, Filter, T>(filter, path.getDataArrayName(), initValue, compDims);
      return dataArray;
    }

    /**
     * @brief validateNumberOfTuples This method will validate that all of the DataArray
     * paths supplied are valid, return non-NULL DataArray pointers, and that all have the
     * same number of tuples.  It will return false if and any of the checks fail, or
     * if the QVector of input paths has 0 or 1 element.
     * @param filter The filter calling the validation
     * @param paths The paths that should be checked
     * @return bool Validation check
     */
    template<typename Filter>
    bool validateNumberOfTuples(Filter* filter, QVector<DataArrayPath> paths)
    {
      if (paths.size() <= 1) { return false; }
      QVector<IDataArray::Pointer> dataArrays;
      bool valid = true;
      QString ss;
      if (paths.at(0).isValid() == false && NULL != filter)
      {
        filter->setErrorCondition(-10000);
        ss = QObject::tr("DataContainerArray::validateNumberOfTuples Error at line %1. The DataArrayPath object was not valid meaning one of the strings in the object is empty. The path is %2").arg(__LINE__).arg(paths.at(0).serialize());
        filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        valid = false;
      }
      IDataArray::Pointer array0 = getPrereqIDataArrayFromPath<IDataArray, Filter>(filter, paths.at(0));
      if (NULL == array0.get() && NULL != filter)
      {
        filter->setErrorCondition(-10100);
        ss = QObject::tr("DataContainerArray::validateNumberOfTuples Error at line %1. The DataArray object was not available. The path is %2").arg(__LINE__).arg(paths.at(0).serialize());
        filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        valid = false;
      }
      else
      {
        dataArrays.push_back(array0);
      }
      for (int32_t i = 1; i < paths.size(); i++)
      {
        if (paths.at(i).isValid() == false && NULL != filter)
        {
          filter->setErrorCondition(-10000);
          ss = QObject::tr("DataContainerArray::validateNumberOfTuples Error at line %1. The DataArrayPath object was not valid meaning one of the strings in the object is empty. The path is %2").arg(__LINE__).arg(paths.at(i).serialize());
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
          valid = false;
        }
        IDataArray::Pointer nextArray = getPrereqIDataArrayFromPath<IDataArray, Filter>(filter, paths.at(i));
        if (NULL == nextArray.get() && NULL != filter)
        {
          filter->setErrorCondition(-10100);
          ss = QObject::tr("DataContainerArray::validateNumberOfTuples Error at line %1. The DataArray object was not available. The path is %2").arg(__LINE__).arg(paths.at(i).serialize());
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
          valid = false;
        }
        else
        {
          dataArrays.push_back(nextArray);
        }
      }
      size_t numTuples = dataArrays[0]->getNumberOfTuples();
      for (int32_t i = 1; i < dataArrays.size(); i++)
      {
        if (numTuples != dataArrays[i]->getNumberOfTuples() && NULL != filter)
        {
          filter->setErrorCondition(-10200);
          ss = QObject::tr("The number of tuples for the DataArray %1 is %2 and for DataArray %3 is %4. The number of tuples must match.")
               .arg(dataArrays[i - 1]->getName()).arg(dataArrays[i - 1]->getNumberOfTuples()).arg(dataArrays[i]->getName()).arg(dataArrays[i]->getNumberOfTuples());
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
          valid = false;
        }
      }
      return valid;
    }

    /**
     * @brief validateNumberOfTuples This method will validate that all of the DataArray
     * objects supplied are non-NULL and that all are have the same number of tuples.
     * It will return false if and any of the checks fail, or
     * if the QVector of input DataArray objects has 0 or 1 element.
     * @param filter The filter calling the validation
     * @param paths The paths that should be checked
     * @return bool Validation check
     */
    template<typename Filter>
    bool validateNumberOfTuples(Filter* filter, QVector<IDataArray::Pointer> dataArrays)
    {
      if (dataArrays.size() <= 1) { return false; }
      bool valid = true;
      QString ss;
      for (int32_t i = 0; i < dataArrays.size(); i++)
      {
        if (NULL == dataArrays.at(i).get() && NULL != filter)
        {
          filter->setErrorCondition(-10100);
          ss = QObject::tr("DataContainerArray::validateNumberOfTuples Error at line %1. The DataArray object was not available").arg(__LINE__);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
          valid = false;
        }
      }
      size_t numTuples = dataArrays[0]->getNumberOfTuples();
      for (int32_t i = 1; i < dataArrays.size(); i++)
      {
        if (numTuples != dataArrays[i]->getNumberOfTuples() && NULL != filter)
        {
          filter->setErrorCondition(-10200);
          ss = QObject::tr("The number of tuples for the DataArray %1 is %2 and for DataArray %3 is %4. The number of tuples must match.")
               .arg(dataArrays[i - 1]->getName()).arg(dataArrays[i - 1]->getNumberOfTuples()).arg(dataArrays[i]->getName()).arg(dataArrays[i]->getNumberOfTuples());
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
          valid = false;
        }
      }
      return valid;
    }

  protected:
    DataContainerArray();

  private:
    QList<DataContainer::Pointer>  m_Array;
    QMap<QString, IDataContainerBundle::Pointer> m_DataContainerBundles;


    DataContainerArray(const DataContainerArray&); // Copy Constructor Not Implemented
    void operator=(const DataContainerArray&); // Operator '=' Not Implemented
};

#endif /* DATACONTAINERARRAY_H_ */

