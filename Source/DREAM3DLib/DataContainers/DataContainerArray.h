/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _DATACONTAINERARRAY_H_
#define _DATACONTAINERARRAY_H_

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QTextStream>


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Observer.h"
#include "DREAM3DLib/Common/Observable.h"
#include "DREAM3DLib/DataContainers/DataContainer.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/DataContainers/SurfaceDataContainer.h"
#include "DREAM3DLib/DataContainers/VertexDataContainer.h"
#include "DREAM3DLib/DataContainers/EdgeDataContainer.h"
#include "DREAM3DLib/DataContainers/DataArrayPath.h"
#include "DREAM3DLib/DataContainers/IDataContainerBundle.h"
#include "DREAM3DLib/DataContainers/DataContainerBundle.h"

/**
 * @class DataContainerArray DataContainerArray.h DREAM3DLib/Common/DataContainerArray.h
 * @brief  This class holds the list of filters that will be run. This should be
 * the class (or a subclass) that you use to build up a pipeline in order to run
 * it.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Sep 28, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT DataContainerArray : public QObject
{
    Q_OBJECT

  public:
    DREAM3D_SHARED_POINTERS(DataContainerArray)
    DREAM3D_STATIC_NEW_MACRO(DataContainerArray)
    DREAM3D_TYPE_MACRO(DataContainerArray)

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

    template<typename T>
    T* getDataContainerBundleAs(const QString& name)
    {
      IDataContainerBundle::Pointer dc = getDataContainerBundle(name);
      if(NULL == dc.get()) { return NULL; }
      T* m = T::SafePointerDownCast(dc.get());
      return m;
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
     * @brief getDataContainerAs
     * @param name
     * @return
     */
    template<typename T>
    T* getDataContainerAs(const QString& name)
    {
      DataContainer::Pointer dc = getDataContainer(name);
      if(NULL == dc.get()) { return NULL; }
      T* m = T::SafePointerDownCast(dc.get());
      return m;
    }

    /**
     * @brief getPrereqDataContainer
     * @param name
     * @param createIfNotExists
     * @return
     */
    template<typename DataContainerType, typename Filter>
    DataContainerType* getPrereqDataContainer(Filter* filter, const QString& name, bool createIfNotExists = false)
    {
      DataContainerType* dc = getDataContainerAs<DataContainerType>(name);
      if(NULL == dc && createIfNotExists == false)
      {
        if (filter)
        {
          filter->setErrorCondition(-999);
          QString ss = "The DataContainer Object with the specific name " + name + " was not available.";
          PipelineMessage em(filter->getHumanLabel(), ss, filter->getErrorCondition(), PipelineMessage::Error);
          filter->broadcastPipelineMessage(em);
        }
        return NULL;
      }
      else if(NULL != dc && createIfNotExists == true)
      {
        typename DataContainerType::Pointer dataContainer = DataContainerType::New(name); // Create a new Data Container
        addDataContainer(dataContainer); // Put the new DataContainer into the array
        return dataContainer.get(); // Return the wrapped pointer
      }
      // The DataContainer we asked for was present and NON Null so return that.
      return dc;
    }

    /**
     * @brief This function will create a new DataContainer of type <T>
     * @param dataContainerName The name of the DataContainer. Must not be empty or this method will ASSERT()
     * @return
     */
    template<class DataContainerType, typename Filter>
    DataContainerType* createNonPrereqDataContainer(Filter* filter, const QString& dataContainerName)
    {
      Q_ASSERT(dataContainerName.isEmpty() == false);
      if(doesDataContainerExist(dataContainerName) == true)
      {
        if (filter)
        {
          filter->setErrorCondition(-888);
          QString ss = "The DataContainer Object with the specific name " + dataContainerName + " already exists.";
          PipelineMessage em(filter->getHumanLabel(), ss, filter->getErrorCondition(), PipelineMessage::Error);
          filter->broadcastPipelineMessage(em);
          return (getPrereqDataContainer<DataContainerType, Filter>(filter, dataContainerName));
        }
      }
      typename DataContainerType::Pointer dataContainer = DataContainerType::New(dataContainerName);
      addDataContainer(dataContainer);
      return dataContainer.get();
    }


    /**
     * @brief getPrereqAttributeMatrixFromPath This function will return an AttributeMatrix if it is availabe
     * at the path
     * @param filter An AbstractFilter or subclass where error messages and error codes can be sent
     * @param path The DataArrayPath object that has the path to the AttributeMatrix
     * @param err The error code to display to the user
     * @return
     */
    template<typename DataContainerType, typename Filter>
    AttributeMatrix::Pointer getPrereqAttributeMatrixFromPath(Filter* filter, DataArrayPath path, int err)
    {
      // First try to get the Parent DataContainer. If an error occurs the error message will have been set
      // so just return a NULL shared pointer
      DataContainer* dc = getPrereqDataContainer<DataContainerType, Filter>(filter, path.getDataContainerName(), false);
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
    * @brief getExistingPrereqArrayFromPath
    * @param filter
    * @param path
    * @return
    */
    template<class ArrayType, class Filter>
    typename ArrayType::Pointer getExistingPrereqArrayFromPath(Filter* filter, const DataArrayPath& path)
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
          filter->setErrorCondition(-90002);
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
          filter->setErrorCondition(-90003);
          ss = QObject::tr("The AttributeMatrix '%1' was not found in the DataContainer '%2'").arg(amName).arg(dcName);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return dataArray;
      }

      dataArray = attrMat->getExistingPrereqArray<ArrayType, Filter>(filter, daName, -90002); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
      return dataArray;
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



  protected:
    DataContainerArray();


  private:
    QList<DataContainer::Pointer>  m_Array;
    QMap<QString, IDataContainerBundle::Pointer> m_DataContainerBundles;


    DataContainerArray(const DataContainerArray&); // Copy Constructor Not Implemented
    void operator=(const DataContainerArray&); // Operator '=' Not Implemented
};

#endif /* DATACONTAINERARRAY_H_ */

