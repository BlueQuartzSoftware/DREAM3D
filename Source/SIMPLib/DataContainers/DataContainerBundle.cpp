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


#include "DataContainerBundle.h"


#include "SIMPLib/DataArrays/StringDataArray.hpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
namespace Detail
{
  class H5GroupAutoCloser
  {
    public:
      H5GroupAutoCloser(hid_t* groupId) :
        gid(groupId)
      {}

      virtual ~H5GroupAutoCloser()
      {
        if (*gid > 0)
        {
          H5Gclose(*gid);
        }
      }
    private:
      hid_t* gid;
  };
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerBundle::DataContainerBundle() :
  m_MetaDataAMName(DREAM3D::StringConstants::MetaData)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerBundle::~DataContainerBundle()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataContainerBundle::GetMetaDataName()
{
  return DREAM3D::StringConstants::MetaData;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerBundle::setDataContainers(QVector<DataContainer::Pointer>& containers)
{
  m_DataContainers = containers;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> DataContainerBundle::getDataContainerNames()
{
  QVector<QString> dcNames;
  for (qint32 i = 0; i < m_DataContainers.size(); i++)
  {
    dcNames.push_back(m_DataContainers[i]->getName());
  }
  return dcNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerBundle::addDataContainer(DataContainer::Pointer dc)
{
  m_DataContainers.append(dc);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerBundle::removeDataContainer(DataContainer::Pointer dc)
{
  for(qint32 i = 0; i < m_DataContainers.size(); i++)
  {
    if(m_DataContainers[i].get() == dc.get())
    {
      m_DataContainers.remove(i);
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerBundle::removeDataContainer(const QString& name)
{
  for(qint32 i = 0; i < m_DataContainers.size(); i++)
  {
    if(m_DataContainers[i]->getName().compare(name) == 0)
    {
      m_DataContainers.remove(i);
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerBundle::removeDataContainer(qint32 i)
{
  m_DataContainers.remove(i);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::Pointer DataContainerBundle::getDataContainer(qint32 index)
{
  return m_DataContainers.value(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
qint32 DataContainerBundle::count()
{
  return m_DataContainers.count();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerBundle::clear()
{
  return m_DataContainers.clear();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<DataArrayPath> DataContainerBundle::findCommonDataArrayPaths(bool filterMetaData)
{
  QVector<DataArrayPath> commonPaths;
  if(m_DataContainers.count() == 0)
  {
    return commonPaths;
  }

  // Get the first DataContainer
  DataContainer::Pointer dc0 = m_DataContainers[0];
  if(NULL == dc0.get())
  {
    return commonPaths;
  }
  QVector<DataArrayPath> dc0Paths = dc0->getAllDataArrayPaths();

  if(m_DataContainers.count() == 1)
  {
    return commonPaths;
  }

  int count = m_DataContainers.count();
  // We already have the first DataContainer, so start at the 2nd
  for (int dcIdx = 1; dcIdx < count; ++dcIdx)
  {
    DataContainer::Pointer dcX = m_DataContainers[dcIdx];
    QVector<DataArrayPath> paths = dcX->getAllDataArrayPaths();

    int numPaths = dc0Paths.count();
    // Loop over the paths from the first data container from back to front,
    // removing the path if it does not exist.
    for(int i = numPaths - 1; i >= 0; i--)
    {
      DataArrayPath dc0Path = dc0Paths[i];
      bool match = false;
      foreach(DataArrayPath path, paths)
      {
        //qDebug() << "Comparing " << dc0Path.serialize() << " TO " << path.serialize();
        if(path.sameAttributeMatrix(dc0Path) &&  path.sameDataArray(dc0Path) )
        {
          match = true;
        }
        if(filterMetaData == true && (path.getAttributeMatrixName() == DREAM3D::StringConstants::MetaData) )
        {
          match = false;
        }
      }

      if(!match)
      {
        dc0Paths.remove(i);
      }
    }
  }

  commonPaths = dc0Paths;
  return commonPaths;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerBundle::writeH5Data(hid_t groupId)
{

  hid_t bundleId = QH5Utilities::createGroup(groupId, getName());
  // This object will make sure the HDF5 Group id is closed when it goes out of scope.
  Detail::H5GroupAutoCloser bundleIdClose(&bundleId);

  size_t count = static_cast<size_t>(m_DataContainers.size());
  QStringList dcNameList;

  for (size_t i = 0; i < count; i++)
  {
    dcNameList << m_DataContainers.at(i)->getName();
  }

  char sep = 0x1E; // Use the ASCII 'record separator' value (Decimal value 30) to separate the names
  // Write the Names of the Data Containers that this bundle holds
  QString nameList = dcNameList.join(QString(sep));
  int err = QH5Lite::writeStringDataset(bundleId, DREAM3D::StringConstants::DataContainerNames, nameList);
  if(err < 0)
  {
    return err;
  }

  // Write the names of the Meta Data Attribute Arrays that this bundle uses for grouping
  QString metaNameList = m_MetaDataArrays.join(QString(sep));
  err = QH5Lite::writeStringDataset(bundleId, DREAM3D::StringConstants::MetaDataArrays, metaNameList);
  if(err < 0)
  {
    return err;
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerBundle::readH5Data(hid_t groupId)
{
  int err = -1;


  return err;
}

