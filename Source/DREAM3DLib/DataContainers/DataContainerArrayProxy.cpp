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
#if 0
	QMap<QString, DataContainerProxy> fDcList = fileProxy.dataContainers;
	QMap<QString, DataContainerProxy>& cDcList = cacheProxy.dataContainers;

	// Add extra items in the file to the cache
	for (int i = 0; i < fDcList.count(); i++)
	{
		DataContainerProxy fileDcProxy = fDcList[i];
		int cIndex = cDcList.indexOf(fileDcProxy);
		// If the cache does not have the file dc proxy, add it to the cache
		if (cIndex < 0)
		{
			cDcList.push_back(fileDcProxy);
		}
		int dcIndex = cDcList.indexOf(fileDcProxy);

		QList<AttributeMatrixProxy> fAmList = fileDcProxy.attributeMatricies.values();
		QMap<QString, AttributeMatrixProxy>& cAmMap = cDcList[dcIndex].attributeMatricies;
		for (int j = 0; j < fAmList.count(); j++)
		{
			AttributeMatrixProxy fileAmProxy = fAmList[j];
			// If the cache does not have the file am proxy, add it to the cache
			if (cAmMap.contains(fileAmProxy.name) == false)
			{
				cAmMap.insert(fileAmProxy.name, fileAmProxy);
			}

			QMap<QString, AttributeMatrixProxy>::iterator iter = cAmMap.find(fileAmProxy.name);

			QList<DataArrayProxy> fDaList = fileAmProxy.dataArrays.values();
			QMap<QString, DataArrayProxy>& cDaMap = iter.value().dataArrays;
			for (int k = 0; k < fDaList.count(); k++)
			{
				DataArrayProxy fileDaProxy = fDaList[k];
				// If the cache does not have the file da proxy, add it to the cache
				if (cDaMap.contains(fileDaProxy.name) == false)
				{
					cDaMap.insert(fileDaProxy.name, fileDaProxy);
				}
			}
		}
	}

	// Remove items from the cache that are no longer in the file
	for (int i = 0; i < cDcList.count(); i++)
	{
		DataContainerProxy& cacheDcProxy = cDcList[i];
		int fIndex = fDcList.indexOf(cacheDcProxy);
		// If the file does not have the cached dc proxy, remove it from the cache
		if (fIndex < 0)
		{
			cDcList.removeAt(i);
		}
		else
		{
			QMap<QString, AttributeMatrixProxy>& cAmMap = cacheDcProxy.attributeMatricies;
			QMap<QString, AttributeMatrixProxy> fAmMap = fDcList[fIndex].attributeMatricies;

			QList<QString> amItemsToDelete;
			for (QMap<QString, AttributeMatrixProxy>::iterator amIter = cAmMap.begin(); amIter != cAmMap.end(); ++amIter)
			{
				AttributeMatrixProxy& cacheAmProxy = amIter.value();
				// If the file does not have the cached am proxy, remove it from the cache
				if (fAmMap.contains(cacheAmProxy.name) == false)
				{
					amItemsToDelete.push_back(cacheAmProxy.name);
				}
				else
				{
					QMap<QString, AttributeMatrixProxy>::iterator iter = fAmMap.find(cacheAmProxy.name);
					QMap<QString, DataArrayProxy>& cDaMap = cacheAmProxy.dataArrays;
					QMap<QString, DataArrayProxy> fDaMap = iter.value().dataArrays;

					QList<QString> daItemsToDelete;
					for (QMap<QString, DataArrayProxy>::iterator daIter = cDaMap.begin(); daIter != cDaMap.end(); ++daIter)
					{
						DataArrayProxy cacheDaProxy = daIter.value();
						// If the file does not have the cached da proxy, remove it from the cache
						if (fDaMap.contains(cacheDaProxy.name) == false)
						{
							daItemsToDelete.push_back(cacheDaProxy.name);
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
#endif // 0


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
	DataContainerProxy proxy;
	dataContainers.insert(proxy.name, proxy);
	return proxy;
}
