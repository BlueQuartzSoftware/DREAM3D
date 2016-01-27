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

#ifndef _AttributeMatrixProxy_H_
#define _AttributeMatrixProxy_H_

#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtCore/QMap>


#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataArrayProxy.h"


class AttributeMatrixProxy
{
  public:
    /**
     * @brief AttributeMatrixProxy
     */
    AttributeMatrixProxy() :
      flag(0),
      name(""),
      amType(DREAM3D::AttributeMatrixType::Unknown)
    {}

    /**
     * @brief AttributeMatrixProxy
     * @param am_name
     * @param read_am
     * @param am_type
     */
    AttributeMatrixProxy(QString am_name, uint8_t read_am = Qt::Checked, unsigned int am_type = DREAM3D::AttributeMatrixType::Unknown) :
      flag(read_am),
      name(am_name),
      amType(am_type)
    {}

    /**
    * @brief Copy Constructor
    */
    AttributeMatrixProxy(const AttributeMatrixProxy& amp)
    {
      flag = amp.flag;
      name = amp.name;
      amType = amp.amType;
      dataArrays = amp.dataArrays;
    }

    /**
    * @brief operator = method
    */
    void operator=(const AttributeMatrixProxy& amp)
    {
      flag = amp.flag;
      name = amp.name;
      amType = amp.amType;
      dataArrays = amp.dataArrays;
    }

    /**
    * @brief Writes the contents of the proxy to the json object 'json'
    * @param json
    * @return
    */
    void writeJson(QJsonObject& json)
    {
      json["Flag"] = static_cast<double>(flag);
      json["Name"] = name;
      json["Type"] = static_cast<double>(amType);
      json["Data Arrays"] = writeMap(dataArrays);
    }

    /**
    * @brief Reads the contents of the the json object 'json' into the proxy
    * @param json
    * @return
    */
    bool readJson(QJsonObject& json)
    {
      if (json["Flag"].isDouble() && json["Name"].isString() && json["Type"].isDouble() && json["Data Arrays"].isArray())
      {
        if (json["Flag"].toDouble() >= std::numeric_limits<uint8_t>().min() && json["Flag"].toDouble() <= std::numeric_limits<uint8_t>().max())
        {
          flag = static_cast<uint8_t>(json["Flag"].toDouble());
        }
        name = json["Name"].toString();
        if (json["Type"].toDouble() >= std::numeric_limits<unsigned int>().min() && json["Type"].toDouble() <= std::numeric_limits<unsigned int>().max())
        {
          amType = static_cast<unsigned int>(json["Type"].toDouble());
        }
        dataArrays = readMap(json["Data Arrays"].toArray());
        return true;
      }
      return false;
    }

    //----- Our variables, publicly available
    uint8_t flag;
    QString name;
    unsigned int amType;
    QMap<QString, DataArrayProxy> dataArrays;

  private:


    QJsonArray writeMap(QMap<QString, DataArrayProxy> map)
    {
      QJsonArray daArray;
      for (QMap<QString, DataArrayProxy>::iterator iter = map.begin(); iter != map.end(); ++iter)
      {
        QJsonObject obj;
        (*iter).writeJson(obj);
        daArray.push_back(obj);
      }
      return daArray;
    }

    QMap<QString, DataArrayProxy> readMap(QJsonArray jsonArray)
    {
      QMap<QString, DataArrayProxy> map;
      foreach(QJsonValue val, jsonArray)
      {
        if (val.isObject())
        {
          DataArrayProxy da;
          da.readJson(val.toObject());
          map.insert(da.name, da);
        }
      }
      return map;
    }

};
Q_DECLARE_METATYPE(AttributeMatrixProxy)

#endif /* _AttributeMatrixProxy_H_ */

