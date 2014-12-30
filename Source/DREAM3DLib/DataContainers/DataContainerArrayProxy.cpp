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
#include "DataContainerArrayProxy.h"

#include <iostream>

#include "DREAM3DLib/DataContainers/DataContainerProxy.h"

#include "DREAM3DLib/DataContainers/DataContainerArray.h"
#include "DREAM3DLib/DataContainers/DataContainer.h"
#include "DREAM3DLib/DataContainers/AttributeMatrix.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy::DataContainerArrayProxy() : isValid(false) {}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy::DataContainerArrayProxy(bool is_valid) : isValid(is_valid) {}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy::DataContainerArrayProxy(const DataContainerArrayProxy& rhs)
{
  isValid = rhs.isValid;
  list = rhs.list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy::DataContainerArrayProxy(DataContainerArray* dca) :
  isValid(false)
{

  if(NULL == dca) { return; }

  QList<DataContainer::Pointer> containers = dca->getDataContainers();
  for(int i = 0; i < containers.size(); i++) // Loop on each Data Container
  {
    DataContainer::Pointer container = containers.at(i);
    DataContainerProxy dcProxy(container->getName(), Qt::Checked, container->getDCType()); // Create a new DataContainerProxy

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
    list.push_back(dcProxy);
  }
  isValid = true;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxy::operator=(const DataContainerArrayProxy& rhs)
{
  isValid = rhs.isValid;
  list = rhs.list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList DataContainerArrayProxy::flattenHeirarchy()
{
  QStringList strList;
  QListIterator<DataContainerProxy> dcIter(list);

  while (dcIter.hasNext()) // DataContainerLevel
  {
    const DataContainerProxy& dcProxy =  dcIter.next();
    if(dcProxy.flag == Qt::Unchecked) { continue; } // Skip to the next DataContainer if we are not reading this one.
    QMapIterator<QString, AttributeMatrixProxy> amIter(dcProxy.attributeMatricies);
    while(amIter.hasNext()) // AttributeMatrixLevel
    {
      amIter.next();

      const AttributeMatrixProxy& amProxy = amIter.value();
      if(amProxy.flag == Qt::Unchecked) { continue; } // Skip to the next AttributeMatrix if not reading this one

      QMapIterator<QString, DataArrayProxy> dIter(amProxy.dataArrays);
      while(dIter.hasNext()) // DataArray Level
      {
        dIter.next();

        const DataArrayProxy& daProxy = dIter.value();
        if(daProxy.flag == DREAM3D::Unchecked) { continue; } // Skip to the next DataArray if not reading this one

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
QVector<DataArrayPath> DataContainerArrayProxy::getCheckedDataArrayPaths()
{
  Q_ASSERT_X(false, "", "");
  //FIXME: THIS IS COMPLETELY BROKEN. If a data container is not checked, we are going to skip it.  AND THIS IS BAD.
  QVector<DataArrayPath> pathList;
  QListIterator<DataContainerProxy> dcIter(list);

  while (dcIter.hasNext()) // DataContainerLevel
  {
    const DataContainerProxy& dcProxy =  dcIter.next();
    if(dcProxy.flag == Qt::Unchecked) { continue; } // Skip to the next DataContainer if we are not reading this one.
    QMapIterator<QString, AttributeMatrixProxy> amIter(dcProxy.attributeMatricies);
    while(amIter.hasNext()) // AttributeMatrixLevel
    {
      amIter.next();

      const AttributeMatrixProxy& amProxy = amIter.value();
      if(amProxy.flag == Qt::Unchecked) { continue; } // Skip to the next AttributeMatrix if not reading this one

      QMapIterator<QString, DataArrayProxy> dIter(amProxy.dataArrays);
      while(dIter.hasNext()) // DataArray Level
      {
        dIter.next();

        const DataArrayProxy& daProxy = dIter.value();
        if(daProxy.flag == DREAM3D::Unchecked) { continue; } // Skip to the next DataArray if not reading this one

        QString path = QString("%1|%2|%3").arg(dcProxy.name).arg(amProxy.name).arg(daProxy.name);
        DataArrayPath dPath(path);
        pathList.push_back(dPath);
      }
    }
  }
  return pathList;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxy::print(const QString header)
{
  QString str;
  QTextStream out(&str);
  QListIterator<DataContainerProxy> dcIter(list);

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

//// -----------------------------------------------------------------------------
////
//// -----------------------------------------------------------------------------
//void DataContainerArrayProxy::removeSelectionsFromDataContainerArray(DataContainerArray* dca, Qt::CheckState state)
//{
//  // Loop over the data containers until we find the proper data container
//  QList<DataContainerProxy> containers = list;
//  QListIterator<DataContainerProxy> containerIter(containers);
//  QStringList dcList;
//  while(containerIter.hasNext())
//  {
//    DataContainerProxy dcProxy = containerIter.next();
//    dcList.push_back(dcProxy.name);
//    DataContainer::Pointer dcItem = dca->getDataContainer(dcProxy.name);
//    if(dcItem.get() == NULL) { continue; }
//    // Check to see if the DataContainer is checked, if it is NOT checked then we remove the entire DataContainer from
//    // the DataContainerArray
//    if (dcProxy.flag == state)
//    {
//        DataContainer::Pointer ptr = dca->removeDataContainer(dcProxy.name); // Remove it out
//
//      continue; // Continue to the next DataContainer
//    }
//    QMap<QString, AttributeMatrixProxy>& attrMats = dcProxy.attributeMatricies;
//    QMapIterator<QString, AttributeMatrixProxy> attrMatsIter(attrMats);
//    while(attrMatsIter.hasNext() )
//    {
//      attrMatsIter.next();
//      QString amName = attrMatsIter.key();
//      AttributeMatrix::Pointer amItem = dcItem->getAttributeMatrix(amName);
//      //assert(amItem.get() != NULL);
//      if(amItem.get() == NULL) { continue; }
//      AttributeMatrixProxy attrProxy = attrMatsIter.value();
//      // Check to see if this AttributeMatrix is checked, if not then remove it from the DataContainer and go to the next loop
//      if(attrProxy.flag == state)
//      {
//        dcItem->removeAttributeMatrix(amName);
//        continue;
//      }
//      // We found the selected AttributeMatrix, so loop over this attribute matrix arrays and populate the list widget
//      QMap<QString, DataArrayProxy>& dataArrays = attrProxy.dataArrays;
//      QMapIterator<QString, DataArrayProxy> dataArraysIter(dataArrays);
//      while(dataArraysIter.hasNext() )
//      {
//        dataArraysIter.next();
//        QString daName = dataArraysIter.key();
//        IDataArray::Pointer daItem = amItem->getAttributeArray(daName);
//        if(daItem.get() == NULL) { continue; }
//        DataArrayProxy daProxy = dataArraysIter.value();
//        // Check to see if the user selected this item
//        if(daProxy.flag == state)
//        {
//          amItem->removeAttributeArray(daName);
//          continue;
//        }
//      }
//    }
//  }
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArrayProxy::setAllFlags(Qt::CheckState state)
{
  QMutableListIterator<DataContainerProxy> dcIter(list);

  while (dcIter.hasNext()) // DataContainerLevel
  {
    DataContainerProxy& dcProxy =  dcIter.next();
    dcProxy.flag = state;
    QMutableMapIterator<QString, AttributeMatrixProxy> amIter(dcProxy.attributeMatricies);
    while(amIter.hasNext()) // AttributeMatrixLevel
    {
      amIter.next();
      AttributeMatrixProxy& amProxy = amIter.value();
      amProxy.flag = state;
      QMutableMapIterator<QString, DataArrayProxy> dIter(amProxy.dataArrays);
      while(dIter.hasNext()) // DataArray Level
      {
        dIter.next();
        DataArrayProxy& daProxy = dIter.value();
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
  QMutableListIterator<DataContainerProxy> dcIter(list);

  while (dcIter.hasNext()) // DataContainerLevel
  {
    DataContainerProxy& dcProxy =  dcIter.next();
    if(dcProxy.flag == Qt::Checked) { dcProxy.flag = Qt::Unchecked; }
    else if(dcProxy.flag == Qt::Unchecked) { dcProxy.flag = Qt::Checked; }

    QMutableMapIterator<QString, AttributeMatrixProxy> amIter(dcProxy.attributeMatricies);
    while(amIter.hasNext()) // AttributeMatrixLevel
    {
      amIter.next();
      AttributeMatrixProxy& amProxy = amIter.value();
      if(amProxy.flag == Qt::Checked) { amProxy.flag = Qt::Unchecked; }
      else if(amProxy.flag == Qt::Unchecked) { amProxy.flag = Qt::Checked; }
      QMutableMapIterator<QString, DataArrayProxy> dIter(amProxy.dataArrays);
      while(dIter.hasNext()) // DataArray Level
      {
        dIter.next();
        DataArrayProxy& daProxy = dIter.value();
        if(daProxy.flag == Qt::Checked) { daProxy.flag = Qt::Unchecked; }
        else if(daProxy.flag == Qt::Unchecked) { daProxy.flag = Qt::Checked; }
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

  QMutableListIterator<DataContainerProxy> dcIter(list);

  while (dcIter.hasNext()) // DataContainerLevel
  {
    DataContainerProxy& dcProxy =  dcIter.next();
    QMutableMapIterator<QString, AttributeMatrixProxy> amIter(dcProxy.attributeMatricies);
    while(amIter.hasNext()) // AttributeMatrixLevel
    {
      amIter.next();
      AttributeMatrixProxy& amProxy = amIter.value();
      QMutableMapIterator<QString, DataArrayProxy> dIter(amProxy.dataArrays);
      while(dIter.hasNext()) // DataArray Level
      {
        dIter.next();
        DataArrayProxy& daProxy = dIter.value();

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
bool DataContainerArrayProxy::contains(const QString& name)
{
  QListIterator<DataContainerProxy> iter(list);
  while(iter.hasNext())
  {
    const DataContainerProxy dc = iter.next();
    if(dc.name.compare(name) == 0) { return true;}
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerProxy& DataContainerArrayProxy::getDataContainerProxy(const QString& name)
{
  QMutableListIterator<DataContainerProxy> iter(list);
  while(iter.hasNext())
  {
    DataContainerProxy& dc = iter.next();
    if(dc.name.compare(name) == 0) { return dc;}
  }
  DataContainerProxy proxy;
  list.push_back(proxy);
  return list.back();
}
