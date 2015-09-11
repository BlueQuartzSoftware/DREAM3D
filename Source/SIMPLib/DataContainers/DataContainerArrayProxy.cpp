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

#include "DataContainerArrayProxy.h"

#include <iostream>

#include "SIMPLib/DataContainers/DataContainerProxy.h"

#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/DataArrays/IDataArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy::DataContainerArrayProxy() {}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy::DataContainerArrayProxy(const DataContainerArrayProxy& rhs)
{
  dataContainers = rhs.dataContainers;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy::DataContainerArrayProxy(DataContainerArray* dca)
{

  if(NULL == dca)
  {
    return;
  }

  QList<DataContainer::Pointer> containers = dca->getDataContainers();
  for(int i = 0; i < containers.size(); i++) // Loop on each Data Container
  {
    DataContainer::Pointer container = containers.at(i);
    DataContainerProxy dcProxy(container->getName(), Qt::Checked); // Create a new DataContainerProxy

    // Now loop over each AttributeMatrix in the data container that was selected
    DataContainer::AttributeMatrixMap_t attrMats = container->getAttributeMatrices();
    QMapIterator<QString, AttributeMatrix::Pointer> iter(attrMats);
    while(iter.hasNext())
    {
      iter.next();
      QString amName = iter.key();
      AttributeMatrix::Pointer attrMat = iter.value();
      AttributeMatrixProxy amProxy(amName, Qt::Checked, attrMat->getType());

      QList<QString> attrArrayNames = attrMat->getAttributeArrayNames();
      QListIterator<QString> attrArrayNamesIter(attrArrayNames);
      while(attrArrayNamesIter.hasNext())
      {
        QString aaName = attrArrayNamesIter.next();
        QString daPath = container->getName() + "/" + amName + "/";
        IDataArray::Pointer attrArray = attrMat->getAttributeArray(aaName);
        DataArrayProxy daProxy(daPath, aaName, Qt::Checked, attrArray->getTypeAsString(), attrArray->getClassVersion() );
        amProxy.dataArrays.insert(aaName, daProxy);
      }
      dcProxy.attributeMatricies.insert(amName, amProxy); // Add the new AttributeMatrix to the DataContainerProxy
    }
    dataContainers.insert(dcProxy.name, dcProxy);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy::~DataContainerArrayProxy()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxy::operator=(const DataContainerArrayProxy& rhs)
{
  dataContainers = rhs.dataContainers;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DataContainerArrayProxy::flattenHeirarchy(Qt::CheckState dcFlag, Qt::CheckState amFlag, Qt::CheckState daFlag)
{
  QStringList strList;
  QList<DataContainerProxy> dcProxies = dataContainers.values();
  QListIterator<DataContainerProxy> dcIter(dcProxies);

  while (dcIter.hasNext()) // DataContainerLevel
  {
    const DataContainerProxy& dcProxy =  dcIter.next();
    if(dcProxy.flag != dcFlag)
    {
      continue;    // Skip to the next DataContainer if we are not reading this one.
    }
    QMapIterator<QString, AttributeMatrixProxy> amIter(dcProxy.attributeMatricies);
    while(amIter.hasNext()) // AttributeMatrixLevel
    {
      amIter.next();

      const AttributeMatrixProxy& amProxy = amIter.value();
      if(amProxy.flag != amFlag)
      {
        continue;    // Skip to the next AttributeMatrix if not reading this one
      }

      QMapIterator<QString, DataArrayProxy> dIter(amProxy.dataArrays);
      while(dIter.hasNext()) // DataArray Level
      {
        dIter.next();

        const DataArrayProxy& daProxy = dIter.value();
        if(daProxy.flag != daFlag)
        {
          continue;    // Skip to the next DataArray if not reading this one
        }

        QString path = QString("%1|%2|%3").arg(dcProxy.name).arg(amProxy.name).arg(daProxy.name);
        strList << path;
      }
    }
  }
  return strList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxy::print(const QString header)
{
  QString str;
  QTextStream out(&str);
  QList<DataContainerProxy> dcProxies = dataContainers.values();
  QListIterator<DataContainerProxy> dcIter(dcProxies);

  while (dcIter.hasNext()) // DataContainerLevel
  {
    const DataContainerProxy& dcProxy =  dcIter.next();

    out << dcProxy.name << "|" << "  [flag:" << (int)(dcProxy.flag) << "]\n";
    QMapIterator<QString, AttributeMatrixProxy> amIter(dcProxy.attributeMatricies);
    while(amIter.hasNext()) // AttributeMatrixLevel
    {
      amIter.next();

      const AttributeMatrixProxy& amProxy = amIter.value();
      out << dcProxy.name << "|" << amProxy.name << "|" << "  [flag:" << (int)(amProxy.flag) << "]\n";
      QMapIterator<QString, DataArrayProxy> dIter(amProxy.dataArrays);
      while(dIter.hasNext()) // DataArray Level
      {
        dIter.next();

        const DataArrayProxy& daProxy = dIter.value();

        out << dcProxy.name << "|" << amProxy.name << "|" << daProxy.name << "  [flag:" << (int)(daProxy.flag) << "]"
            << " [Object Type: " << daProxy.objectType << "] [CompDims: ";
        for(int i = 0; i < daProxy.compDims.size(); i++)
        {
          out << daProxy.compDims[i] << " ";
        }
        out << "]\n";
      }
    }
  }
  std::cout << "---------------- " << header.toStdString() << " ----------------------" << std::endl;
  std::cout << str.toStdString() << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxy::setAllFlags(Qt::CheckState state)
{
  for (QMap<QString, DataContainerProxy>::iterator dcIter = dataContainers.begin(); dcIter != dataContainers.end(); ++dcIter) // DataContainer Level
  {
    DataContainerProxy& dcProxy = dcIter.value();
    dcProxy.flag = state;

    QMap<QString, AttributeMatrixProxy>& amProxies = dcProxy.attributeMatricies;
    for (QMap<QString, AttributeMatrixProxy>::iterator amIter = amProxies.begin(); amIter != amProxies.end(); ++amIter) // AttributeMatrix Level
    {
      AttributeMatrixProxy& amProxy = amIter.value();
      amProxy.flag = state;

      QMap<QString, DataArrayProxy>& daProxies = amProxy.dataArrays;
      for (QMap<QString, DataArrayProxy>::iterator daIter = daProxies.begin(); daIter != daProxies.end(); ++daIter) // DataArray Level
      {
        DataArrayProxy& daProxy = daIter.value();
        daProxy.flag = state;
      }
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxy::reverseFlags()
{
  for (QMap<QString, DataContainerProxy>::iterator dcIter = dataContainers.begin(); dcIter != dataContainers.end(); ++dcIter) // DataContainer Level
  {
    DataContainerProxy& dcProxy =  dcIter.value();
    if(dcProxy.flag == Qt::Checked)
    {
      dcProxy.flag = Qt::Unchecked;
    }
    else if(dcProxy.flag == Qt::Unchecked)
    {
      dcProxy.flag = Qt::Checked;
    }

    QMap<QString, AttributeMatrixProxy>& amProxies = dcProxy.attributeMatricies;
    for (QMap<QString, AttributeMatrixProxy>::iterator amIter = amProxies.begin(); amIter != amProxies.end(); ++amIter) // AttributeMatrix Level
    {
      AttributeMatrixProxy& amProxy = amIter.value();
      if(amProxy.flag == Qt::Checked)
      {
        amProxy.flag = Qt::Unchecked;
      }
      else if(amProxy.flag == Qt::Unchecked)
      {
        amProxy.flag = Qt::Checked;
      }

      QMap<QString, DataArrayProxy>& daProxies = amProxy.dataArrays;
      for (QMap<QString, DataArrayProxy>::iterator daIter = daProxies.begin(); daIter != daProxies.end(); ++daIter) // DataArray Level
      {
        DataArrayProxy& daProxy = daIter.value();
        if(daProxy.flag == Qt::Checked)
        {
          daProxy.flag = Qt::Unchecked;
        }
        else if(daProxy.flag == Qt::Unchecked)
        {
          daProxy.flag = Qt::Checked;
        }
      }
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DataContainerArrayProxy::serialize()
{

  QStringList entries;

  for (QMap<QString, DataContainerProxy>::iterator dcIter = dataContainers.begin(); dcIter != dataContainers.end(); ++dcIter) // DataContainer Level
  {
    DataContainerProxy& dcProxy =  dcIter.value();

    QMap<QString, AttributeMatrixProxy>& amProxies = dcProxy.attributeMatricies;
    for (QMap<QString, AttributeMatrixProxy>::iterator amIter = amProxies.begin(); amIter != amProxies.end(); ++amIter) // AttributeMatrix Level
    {
      AttributeMatrixProxy& amProxy = amIter.value();

      QMap<QString, DataArrayProxy>& daProxies = amProxy.dataArrays;
      for (QMap<QString, DataArrayProxy>::iterator daIter = daProxies.begin(); daIter != daProxies.end(); ++daIter) // DataArray Level
      {
        DataArrayProxy& daProxy = daIter.value();

        QString str = QString("[PATH=%1|%2|%3][FLAG=%4]").arg(dcProxy.name).arg(amProxy.name).arg(daProxy.name).arg(daProxy.flag);
        entries << str;
      }
    }
  }
  return entries;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy DataContainerArrayProxy::MergeProxies(DataContainerArrayProxy fileProxy, DataContainerArrayProxy cacheProxy)
{
  QMap<QString, DataContainerProxy> fDcMap = fileProxy.dataContainers;
  QMap<QString, DataContainerProxy>& cDcMap = cacheProxy.dataContainers;

  // Add extra items in the file to the cache
  for (QMap<QString, DataContainerProxy>::iterator fDcIter = fDcMap.begin(); fDcIter != fDcMap.end(); ++fDcIter)
  {
    QString fileDcName = fDcIter.key();
    DataContainerProxy fileDcProxy = fDcIter.value();

    // If the cache does not have the file dc proxy, add it to the cache
    if (cDcMap.contains(fileDcName) == false)
    {
      cDcMap.insert(fileDcName, fileDcProxy);
    }

    QMap<QString, AttributeMatrixProxy> fAmMap = fileDcProxy.attributeMatricies;
    QMap<QString, AttributeMatrixProxy>& cAmMap = cDcMap.find(fileDcName).value().attributeMatricies;
    for (QMap<QString, AttributeMatrixProxy>::iterator fAmIter = fAmMap.begin(); fAmIter != fAmMap.end(); ++fAmIter)
    {
      QString fileAmName = fAmIter.key();
      AttributeMatrixProxy fileAmProxy = fAmIter.value();
      // If the cache does not have the file am proxy, add it to the cache
      if (cAmMap.contains(fileAmName) == false)
      {
        cAmMap.insert(fileAmName, fileAmProxy);
      }

      QMap<QString, DataArrayProxy> fDaMap = fileAmProxy.dataArrays;
      QMap<QString, DataArrayProxy>& cDaMap = cAmMap.find(fileAmName).value().dataArrays;
      for (QMap<QString, DataArrayProxy>::iterator fDaIter = fDaMap.begin(); fDaIter != fDaMap.end(); ++fDaIter)
      {
        QString fileDaName = fDaIter.key();
        DataArrayProxy fileDaProxy = fDaIter.value();
        // If the cache does not have the file da proxy, add it to the cache
        if (cDaMap.contains(fileDaName) == false)
        {
          cDaMap.insert(fileDaName, fileDaProxy);
        }
      }
    }
  }

  // Remove items from the cache that are no longer in the file
  QList<QString> dcItemsToDelete;
  for (QMap<QString, DataContainerProxy>::iterator cDcIter = cDcMap.begin(); cDcIter != cDcMap.end(); ++cDcIter)
  {
    QString cacheDcName = cDcIter.key();
    DataContainerProxy& cacheDcProxy = cDcIter.value();

    // If the file does not have the cached dc proxy, remove it from the cache
    if (fDcMap.contains(cacheDcName) == false)
    {
      dcItemsToDelete.push_back(cacheDcName);
    }
    else
    {
      QMap<QString, AttributeMatrixProxy>& cAmMap = cacheDcProxy.attributeMatricies;
      QMap<QString, AttributeMatrixProxy> fAmMap = fDcMap.find(cacheDcName).value().attributeMatricies;

      QList<QString> amItemsToDelete;
      for (QMap<QString, AttributeMatrixProxy>::iterator amIter = cAmMap.begin(); amIter != cAmMap.end(); ++amIter)
      {
        QString cacheAmName = amIter.key();
        AttributeMatrixProxy& cacheAmProxy = amIter.value();
        // If the file does not have the cached am proxy, remove it from the cache
        if (fAmMap.contains(cacheAmName) == false)
        {
          amItemsToDelete.push_back(cacheAmName);
        }
        else
        {
          QMap<QString, DataArrayProxy>& cDaMap = cacheAmProxy.dataArrays;
          QMap<QString, DataArrayProxy> fDaMap = fAmMap.find(cacheAmName).value().dataArrays;

          QList<QString> daItemsToDelete;
          for (QMap<QString, DataArrayProxy>::iterator daIter = cDaMap.begin(); daIter != cDaMap.end(); ++daIter)
          {
            QString cacheDaName = daIter.key();
            DataArrayProxy cacheDaProxy = daIter.value();
            // If the file does not have the cached da proxy, remove it from the cache
            if (fDaMap.contains(cacheDaName) == false)
            {
              daItemsToDelete.push_back(cacheDaName);
            }
          }
          // Remove extra da entries from cache
          for (QList<QString>::iterator iter = daItemsToDelete.begin(); iter != daItemsToDelete.end(); ++iter)
          {
            cDaMap.remove(*iter);
          }
        }
      }
      // Remove extra am entries from cache
      for (QList<QString>::iterator iter = amItemsToDelete.begin(); iter != amItemsToDelete.end(); ++iter)
      {
        cAmMap.remove(*iter);
      }
    }
  }
  // Remove extra dc entries from cache
  for (QList<QString>::iterator iter = dcItemsToDelete.begin(); iter != dcItemsToDelete.end(); ++iter)
  {
    cDcMap.remove(*iter);
  }

  return cacheProxy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerArrayProxy::contains(const QString& name)
{
  QList<DataContainerProxy> dcProxies = dataContainers.values();
  QListIterator<DataContainerProxy> iter(dcProxies);
  while(iter.hasNext())
  {
    const DataContainerProxy dc = iter.next();
    if(dc.name.compare(name) == 0)
    {
      return true;
    }
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerProxy& DataContainerArrayProxy::getDataContainerProxy(const QString& name)
{
  for (QMap<QString, DataContainerProxy>::iterator dcIter = dataContainers.begin(); dcIter != dataContainers.end(); ++dcIter) // DataContainer Level
  {
    DataContainerProxy& dc = dcIter.value();
    if (dc.name.compare(name) == 0)
    {
      return dc;
    }
  }

  DataContainerProxy proxy(name);
  dataContainers.insert(proxy.name, proxy);
  return dataContainers[name];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxy::writeJson(QJsonObject& json) const
{
  json["Data Containers"] = writeMap(dataContainers);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerArrayProxy::readJson(QJsonObject& json)
{
  if (json["Data Containers"].isArray())
  {
    dataContainers = readMap(json["Data Containers"].toArray());
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QJsonArray DataContainerArrayProxy::writeMap(QMap<QString, DataContainerProxy> map) const
{
  QJsonArray dcArray;
  for (QMap<QString, DataContainerProxy>::iterator iter = map.begin(); iter != map.end(); ++iter)
  {
    QJsonObject obj;
    (*iter).writeJson(obj);
    dcArray.push_back(obj);
  }
  return dcArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<QString, DataContainerProxy> DataContainerArrayProxy::readMap(QJsonArray jsonArray)
{
  QMap<QString, DataContainerProxy> map;
  foreach(QJsonValue val, jsonArray)
  {
    if (val.isObject())
    {
      DataContainerProxy dc;
      QJsonObject obj = val.toObject();
      dc.readJson(obj);
      map.insert(dc.name, dc);
    }
  }
  return map;
}




