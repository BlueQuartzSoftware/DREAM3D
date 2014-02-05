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

  QList<DataContainer::Pointer> containers = dca->getDataContainerArray();
  for(int i = 0; i < containers.size(); i++) // Loop on each Data Container
  {
    DataContainer::Pointer container = containers.at(i);
    DataContainerProxy dcProxy(container->getName(), false, container->getDCType()); // Create a new DataContainerProxy

    // Now loop over each AttributeMatrix in the data container that was selected
    DataContainer::AttributeMatrixMap_t attrMats = container->getAttributeMatrices();
    QMapIterator<QString, AttributeMatrix::Pointer> iter(attrMats);
    while(iter.hasNext())
    {
      iter.next();
      QString amName = iter.key();
      AttributeMatrix::Pointer attrMat = iter.value();
      AttributeMatrixProxy amProxy(amName, false, attrMat->getType());

      QList<QString> attrArrayNames = attrMat->getAttributeArrayNameList();
      QListIterator<QString> attrArrayNamesIter(attrArrayNames);
      while(attrArrayNamesIter.hasNext())
      {
        QString aaName = attrArrayNamesIter.next();
        QString daPath = container->getName() + "/" + amName + "/";
        IDataArray::Pointer attrArray = attrMat->getAttributeArray(aaName);
        DataArrayProxy daProxy(daPath, aaName, false, attrArray->getTypeAsString(), attrArray->getClassVersion() );
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
    if(dcProxy.read == false) { continue; } // Skip to the next DataContainer if we are not reading this one.
    QMapIterator<QString, AttributeMatrixProxy> amIter(dcProxy.attributeMatricies);
    while(amIter.hasNext()) // AttributeMatrixLevel
    {
      amIter.next();

      const AttributeMatrixProxy& amProxy = amIter.value();
      if(amProxy.read == false) { continue; } // Skip to the next AttributeMatrix if not reading this one

      QMapIterator<QString, DataArrayProxy> dIter(amProxy.dataArrays);
      while(dIter.hasNext()) // DataArray Level
      {
        dIter.next();

        const DataArrayProxy& daProxy = dIter.value();
        if(daProxy.read == false) { continue; } // Skip to the next DataArray if not reading this one

        QString path = QString("%1|%2|%3").arg(dcProxy.name).arg(amProxy.name).arg(daProxy.name);
        strList << path;
      }
    }
  }
  return strList;
}
