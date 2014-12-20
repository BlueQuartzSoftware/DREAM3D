/* ============================================================================
 * Copyright (c) 2014 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2014 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "DataContainerBundle.h"


#include "DREAM3DLib/DataArrays/StringDataArray.hpp"

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
void DataContainerBundle::setDataContainers(QVector<DataContainer::Pointer> &containers)
{
  m_DataContainers = containers;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> DataContainerBundle::getDataContainerNames()
{
  QVector<QString> dcNames;
  for (qint32 i=0;i<m_DataContainers.size();i++)
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
void DataContainerBundle::removeDataContainer(const QString &name)
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
int DataContainerBundle::writeH5Data(hid_t groupId)
{

  hid_t bundleId = QH5Utilities::createGroup(groupId, getName());
  // This object will make sure the HDF5 Group id is closed when it goes out of scope.
  Detail::H5GroupAutoCloser bundleIdClose(&bundleId);

  size_t count = static_cast<size_t>(m_DataContainers.size());
  QStringList dcNameList;

  for (size_t i = 0;i < count; i++)
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
 void DataContainerBundle::findCommonDataArrayPaths()
 {
    // Get the first DataContainer
    DataContainer::Pointer dc0 = m_DataContainers[0];


 }
