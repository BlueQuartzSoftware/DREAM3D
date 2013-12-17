/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _DataContainer_H_
#define _DataContainer_H_

//-- C++ includes
#include <vector>
#include <map>
#include <sstream>
#include <list>


#include <QtCore/QSet>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QMap>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Observable.h"
#include "DREAM3DLib/DataContainers/AttributeMatrix.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"

/**
 * @brief The DataContainer class
 */
class DREAM3DLib_EXPORT DataContainer : public Observable
{
  public:
    DREAM3D_SHARED_POINTERS (DataContainer)
    DREAM3D_STATIC_NEW_MACRO (DataContainer)
    DREAM3D_TYPE_MACRO_SUPER(DataContainer, Observable)

    virtual ~DataContainer();

    static Pointer New(const QString& name)
    {
      Pointer sharedPtr(new DataContainer(name));
      return sharedPtr;
    }


    DREAM3D_VIRTUAL_INSTANCE_PROPERTY(QString, Name)


    virtual unsigned int getDCType() {return DREAM3D::DataContainerType::UnknownDataContainer;}

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
    template<class Filter>
    AttributeMatrix* getPrereqAttributeMatrix(Filter* filter,
                                              QString attributeMatrixName,
                                              int err)
    {
      QString ss;
      typename AttributeMatrix::Pointer attributeMatrix = AttributeMatrix::NullPointer();
      //Make sure the name is not empty for teh AttributeMatrix and the AttributeArray Name. This would be detected below
      // in the call to get either one BUT the reason for the failure would not be evident so we make these explicit checks
      // here and send back nice error messages to ther user/programmer.
      if (attributeMatrixName.isEmpty() == true)
      {
        if(filter)
        {
          filter->setErrorCondition(err * 1000);
          ss = QObject::tr("DataContainer:'%1' The name of the AttributeMatrix was empty. Please provide a name for this AttributeMatrix").arg(getName());
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return attributeMatrix.get();
      }
      // Now attempt to get the AttributeMatrix which could still come back NULL because the name does not match.
      attributeMatrix = getAttributeMatrix(attributeMatrixName);
      if(NULL == attributeMatrix)
      {
        if(filter)
        {
          filter->setErrorCondition(err * 1020);
          ss = QObject::tr("DataContainer:'%1' An AttributeMatrix with name '%2' does not exist and is required for this filter to execute.").arg(getName()).arg(attributeMatrixName);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return attributeMatrix.get();
      }
      if(false == attributeMatrix->validateAttributeArraySizes())
      {
        if(filter)
        {
          filter->setErrorCondition(err * 1030);
          ss = QObject::tr("DataContainer:'%1' AttributeMatrix: '%2' Attribute Matrix has Attribute Arrays with mismatched number of objects.").arg(getName()).arg(attributeMatrixName);
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return attributeMatrix.get();
      }
      return attributeMatrix.get();
    }

    /**
     * @brief createNonPrereqArray This method will create a new DataArray in the named AttributeMatrix. The conditions for this
     * method to work properly are that an AttributeMatrix with the give name exists in the data container and the name
     * of the attribute array is not empty
     * @param filter The instance of the filter the filter that is requesting the new array
     * @param attributeMatrixName The name of the AttributeMatrix
     * @param attributeArrayName The name of the AttributeArray to create
     * @param initValue The initial value of all the elements of the array
     * @param size The number of tuples in the Array
     * @param dims The dimensions of the components of the AttributeArray
     * @return A Shared Pointer to the newly created array
     */
    template<class Filter>
    AttributeMatrix* createNonPrereqAttributeMatrix(Filter* filter,
                                                    const QString& attributeMatrixName,
                                                    unsigned int amType)
    {
      typename AttributeMatrix::Pointer attributeMatrix = AttributeMatrix::NullPointer();

      QString ss;
      if (attributeMatrixName.isEmpty() == true)
      {
        if(filter)
        {
          filter->setErrorCondition(-10001);
          ss = QObject::tr("The name of the array was empty. Please provide a name for this array.");
          filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
        }
        return attributeMatrix.get();
      }
      attributeMatrix = getAttributeMatrix(attributeMatrixName);
      if(NULL == attributeMatrix)
      {
        AttributeMatrix* attrMat = createAndAddAttributeMatrix(attributeMatrixName);
        return attrMat;
      }
      if(filter)
      {
        filter->setErrorCondition(-10002);
        ss = QObject::tr("An Attribute Matrix already exists with the name %1").arg(attributeMatrixName);
        filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
      }
      return attributeMatrix.get();
    }

    typedef QMap<QString, AttributeMatrix::Pointer> AttributeMatrixMap_t;

    /**
     * @brief getAttributeMatrices
     * @return
     */
    AttributeMatrixMap_t& getAttributeMatrices();


    /**
     * @brief Returns bool of whether a named array exists
     * @param name The name of the data array
     */
    virtual bool doesAttributeMatrixExist(const QString& name);

    /**
     * @brief Creates an AttributeMatrix and automatically adds it to the DataContainer
     * @param attrMatName
     * @return
     */
    virtual AttributeMatrix* createAndAddAttributeMatrix(const QString& attrMatName);

    /**
    * @brief Adds/overwrites the data for a named array
    * @param name The name that the array will be known by
    * @param data The IDataArray::Pointer that will hold the data
    */
    virtual void addAttributeMatrix(const QString& name, AttributeMatrix::Pointer matrix);

    /**
     * @brief Returns the array for a given named array or the equivelant to a
     * null pointer if the name does not exist.
     * @param name The name of the data array
     */
    virtual AttributeMatrix::Pointer getAttributeMatrix(const QString& name);

    /**
    * @brief Removes the named data array from the Data Container and returns it to the calling
    * method.
    * @param name The name of the array
    * @return
    */
    virtual AttributeMatrix::Pointer removeAttributeMatrix(const QString& name);

    /**
    * @brief Renames a cell data array from the Data Container
    * @param name The name of the array
    */
    virtual bool renameAttributeMatrix(const QString& oldname, const QString& newname, bool overwrite = false);

    /**
     * @brief Removes all the Cell Arrays
     */
    virtual void clearAttributeMatrices();

    /**
    * @brief Returns a list that contains the names of all the arrays currently stored in the
    * Cell (Formerly Cell) group
    * @return
    */
    virtual QList<QString> getAttributeMatrixNameList();

    /**
    * @brief Returns the total number of arrays that are stored in the Cell group
    * @return
    */
    virtual int getNumAttributeMatrices();

    /**
    * @brief Writes all the Attribute Matrices to HDF5 file
    * @return
    */
    virtual int writeAttributeMatricesToHDF5(hid_t parentId);

    /**
    * @brief Writes all the Attribute Matrices to HDF5 file
    * @return
    */
    virtual int readAttributeMatricesFromHDF5(bool preflight, hid_t dcGid, QMap<QString, QSet<QString> > arraysToRead);

    virtual int writeMeshToHDF5(hid_t dcGid);
    virtual int writeXdmf(QTextStream& out, QString hdfFileName);
    virtual int readMeshDataFromHDF5(hid_t dcGid, bool preflight);

  protected:
    DataContainer();
    explicit DataContainer(const QString name);

  private:

    AttributeMatrixMap_t   m_AttributeMatrices;

    DataContainer(const DataContainer&); // Copy Constructor Not Implemented
    void operator=(const DataContainer&); // Operator '=' Not Implemented

};

#endif /* _DataContainer_H_ */
