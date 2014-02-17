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
#ifndef _DataArrayProxy_H_
#define _DataArrayProxy_H_

#include <QtCore/QMetaType>
#include <QtCore/QString>

#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"
#include "H5Support/HDF5ScopedFileSentinel.h"


#include "DREAM3DLib/Common/Constants.h"



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
     * @brief ReadDataArrayStructure
     * @param attrMatGid
     * @param dataArrays
     * @param h5InternalPath
     */
    static void ReadDataArrayStructure(hid_t attrMatGid, QMap<QString, DataArrayProxy>& dataArrays, QString h5InternalPath)
    {

      QList<QString> dataArrayNames;
      QH5Utilities::getGroupObjects(attrMatGid, H5Utilities::H5Support_DATASET, dataArrayNames);
      foreach(QString dataArrayName, dataArrayNames)
      {
        if(__SHOW_DEBUG_MSG__) { std::cout << "        DataArray: " << dataArrayName.toStdString()  << std::endl; }

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

};

Q_DECLARE_METATYPE(DataArrayProxy)

#endif /* _DataArrayProxy_H_ */

