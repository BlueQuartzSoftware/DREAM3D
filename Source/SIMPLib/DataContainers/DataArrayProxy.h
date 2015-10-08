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

#ifndef _DataArrayProxy_H_
#define _DataArrayProxy_H_

#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"
#include "H5Support/HDF5ScopedFileSentinel.h"


#include "SIMPLib/Common/Constants.h"



class DataArrayProxy
{

  public:
    /**
     * @brief DataArrayProxy
     */
    DataArrayProxy() :
      flag(DREAM3D::Unchecked),
      version(0),
      path(""),
      name(""),
      objectType("")
    {}

    /**
     * @brief DataArrayProxy
     * @param da_path
     * @param da_name
     * @param da_flag
     * @param da_objectType
     * @param da_version
     */
    DataArrayProxy(QString da_path, QString da_name, uint8_t da_flag = DREAM3D::Checked, QString da_objectType = "NOT_DEFINED", int da_version = 0) :
      flag(da_flag),
      version(da_version),
      path(da_path),
      name(da_name),
      objectType(da_objectType)
    {}

    /**
    * @brief Copy Constructor
    */
    DataArrayProxy(const DataArrayProxy& rhs)
    {
      flag = rhs.flag;
      version = rhs.version;
      path = rhs.path;
      name = rhs.name;
      objectType = rhs.objectType;
      tupleDims = rhs.tupleDims;
      compDims = rhs.compDims;
    }

    /**
    * @brief Writes the contents of the proxy to the json object 'json'
    * @param json
    * @return
    */
    void writeJson(QJsonObject& json)
    {
      json["Flag"] = static_cast<double>(flag);
      json["Version"] = static_cast<double>(version);
      json["Path"] = path;
      json["Name"] = name;
      json["Object Type"] = objectType;
      json["Tuple Dimensions"] = writeVector(tupleDims);
      json["Component Dimensions"] = writeVector(compDims);
    }

    /**
    * @brief Reads the contents of the the json object 'json' into the proxy
    * @param json
    * @return
    */
    bool readJson(const QJsonObject& json)
    {
      if (json["Flag"].isDouble() && json["Version"].isDouble() && json["Path"].isString() && json["Name"].isString()
          && json["Object Type"].isString() && json["Tuple Dimensions"].isArray() && json["Component Dimensions"].isArray())
      {
        if (json["Flag"].toDouble() >= std::numeric_limits<uint8_t>().min() && json["Flag"].toDouble() <= std::numeric_limits<uint8_t>().max())
        {
          flag = static_cast<uint8_t>(json["Flag"].toDouble());
        }
        version = json["Version"].toInt();
        path = json["Path"].toString();
        name = json["Name"].toString();
        objectType = json["Object Type"].toString();
        tupleDims = readVector(json["Tuple Dimensions"].toArray());
        compDims = readVector(json["Component Dimensions"].toArray());
        return true;
      }
      return false;
    }

    /**
     * @brief ReadDataArrayStructure
     * @param attrMatGid
     * @param dataArrays
     * @param h5InternalPath
     */
    static void ReadDataArrayStructure(hid_t attrMatGid, QMap<QString, DataArrayProxy>& dataArrays, QString h5InternalPath)
    {

      QList<QString> dataArrayNames;
      QH5Utilities::getGroupObjects(attrMatGid, H5Utilities::H5Support_DATASET | H5Utilities::H5Support_GROUP, dataArrayNames);
      foreach(QString dataArrayName, dataArrayNames)
      {
        if(__SHOW_DEBUG_MSG__)
        { std::cout << "        DataArray: " << dataArrayName.toStdString()  << std::endl; }

        DataArrayProxy proxy(h5InternalPath, dataArrayName, DREAM3D::Checked);

        herr_t err = QH5Lite::readVectorAttribute(attrMatGid, dataArrayName, DREAM3D::HDF5::TupleDimensions, proxy.tupleDims);
        if(err < 0) { std::cout << "Error Reading the Tuple Dimensions for DataArray " << dataArrayName.toStdString() << std::endl; }

        err = QH5Lite::readVectorAttribute(attrMatGid, dataArrayName, DREAM3D::HDF5::ComponentDimensions, proxy.compDims);
        if(err < 0) { std::cout << "Error Reading the Component Dimensions for DataArray " << dataArrayName.toStdString() << std::endl; }

        err = QH5Lite::readScalarAttribute(attrMatGid, dataArrayName, DREAM3D::HDF5::DataArrayVersion, proxy.version);
        if(err < 0) { std::cout << "Error Reading the Version for DataArray " << dataArrayName.toStdString() << std::endl; }

        err = QH5Lite::readStringAttribute(attrMatGid, dataArrayName, DREAM3D::HDF5::ObjectType, proxy.objectType);
        if(err < 0) { std::cout << "Error Reading the Object Type for DataArray " << dataArrayName.toStdString() << std::endl; }

        dataArrays.insert(dataArrayName, proxy);
      }
    }




    /**
    * @brief operator = method
    */
    void operator=(const DataArrayProxy& rhs)
    {
      flag = rhs.flag;
      version = rhs.version;
      path = rhs.path;
      name = rhs.name;
      objectType = rhs.objectType;
      tupleDims = rhs.tupleDims;
      compDims = rhs.compDims;
    }

    //----- Our variables, publicly available
    uint8_t flag;
    int version;
    QString path;
    QString name;
    QString objectType;
    QVector<size_t> tupleDims;
    QVector<size_t> compDims;

  private:


    QJsonArray writeVector(QVector<size_t> vector)
    {
      QJsonArray jsonArray;
      foreach(size_t num, compDims)
      {
        jsonArray.push_back(static_cast<double>(num));
      }

      return jsonArray;
    }

    QVector<size_t> readVector(QJsonArray jsonArray)
    {
      QVector<size_t> vector;
      foreach(QJsonValue val, jsonArray)
      {
        if (val.isDouble())
        {
          if (val.toDouble() >= std::numeric_limits<size_t>().min() && val.toDouble() <= std::numeric_limits<size_t>().max())
          {
            vector.push_back(static_cast<size_t>(val.toDouble()));
          }
        }
      }
      return vector;
    }

};

Q_DECLARE_METATYPE(DataArrayProxy)

#endif /* _DataArrayProxy_H_ */

