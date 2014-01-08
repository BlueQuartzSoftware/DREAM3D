/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "DataContainerArray.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::DataContainerArray() :
  QObject()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::~DataContainerArray()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::pushFront(DataContainer::Pointer f)
{
  m_Array.push_front(f);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::popFront()
{
  m_Array.pop_front();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::pushBack(DataContainer::Pointer f)
{
  m_Array.push_back(f);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::popBack()
{
  m_Array.pop_back();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::insert(size_t index, DataContainer::Pointer f)
{
  QList<DataContainer::Pointer>::iterator it = m_Array.begin();
  for(size_t i = 0; i < index; ++i)
  {
    ++it;
  }
  m_Array.insert(it, f);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::erase(size_t index)
{
  QList<DataContainer::Pointer>::iterator it = m_Array.begin();
  for(size_t i = 0; i < index; ++i)
  {
    ++it;
  }
  m_Array.erase(it);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::clear()
{
  m_Array.clear();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t DataContainerArray::size()
{
  return m_Array.size();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerArray::empty()
{
  return m_Array.isEmpty();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::Pointer DataContainerArray::removeDataContainer(const QString& name)
{
  DataContainer::Pointer f = DataContainer::NullPointer();
  for(QList<DataContainer::Pointer>::iterator it = m_Array.begin(); it != m_Array.end(); ++it)
  {
    if((*it)->getName().compare(name) == 0)
    {
      f = *it;
      m_Array.erase(it);
      break;
    }
  }

  return f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::Pointer DataContainerArray::getDataContainer(const QString& name)
{
  DataContainer::Pointer f = DataContainer::NullPointer();
  for(QList<DataContainer::Pointer>::iterator it = m_Array.begin(); it != m_Array.end(); ++it)
  {
    if((*it)->getName().compare(name) == 0)
    {
      f = *it;
      break;
    }
  }

  return f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> DataContainerArray::getDataContainerNames()
{
  QList<QString> names;
  for(QList<DataContainer::Pointer>::iterator it = m_Array.begin(); it != m_Array.end(); ++it)
  {
    names.push_back((*it)->getName());
  }
  return names;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<DataContainer::Pointer>& DataContainerArray::getDataContainerArray()
{
  return m_Array;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::printDataContainerNames(QTextStream& out)
{
  out << "---------------------------------------------------------------------" ;
  for (QList<DataContainer::Pointer>::iterator iter = m_Array.begin(); iter != m_Array.end(); ++iter )
  {
    out << (*iter)->getNameOfClass() ;
  }
  out << "---------------------------------------------------------------------" ;
}

int DataContainerArray::readDataContainersFromHDF5(bool preflight, hid_t dcaGid, DataContainerArrayProxy& dcaProxy)
{
  int err;
  QList<DataContainerProxy> dcsToRead = dcaProxy.list;
  uint32_t dcType = DREAM3D::DataContainerType::UnknownDataContainer;
  QListIterator<DataContainerProxy> dcIter(dcsToRead);
  while (dcIter.hasNext()) // DataContainerLevel
  {
    const DataContainerProxy& dcProxy =  dcIter.next();
    if(dcProxy.read == false) continue;
    if (this->contains(dcProxy.name) == true )
    {
      //setErrorCondition(-10987);
      //QString ss = QObject::tr("A Data Container with name %1 already exists in Memory. Reading a Data Container with the same name would over write the one in memory. Currently this is not allowed.").arg(dcNames[iter]);
      //notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return -1;
    }
    err = QH5Lite::readScalarAttribute(dcaGid, dcProxy.name, DREAM3D::StringConstants::DataContainerType, dcType);
    if (err < 0)
    {
      //setErrorCondition(-109283);
      //QString ss = QObject::tr("The DataContainer is missing the 'DataContainerType' attribute on the '%1' Data Container").arg(dcNames[iter]);
      //notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return -1;
    }
    if(dcType == DREAM3D::DataContainerType::VolumeDataContainer)
    {
      VolumeDataContainer::Pointer dc = VolumeDataContainer::New();
      dc->setName(dcProxy.name);
      this->pushBack(dc);
    }
    if(dcType == DREAM3D::DataContainerType::SurfaceDataContainer)
    {
      SurfaceDataContainer::Pointer dc = SurfaceDataContainer::New();
      dc->setName(dcProxy.name);
      this->pushBack(dc);
    }
    if(dcType == DREAM3D::DataContainerType::EdgeDataContainer)
    {
      EdgeDataContainer::Pointer dc = EdgeDataContainer::New();
      dc->setName(dcProxy.name);
      this->pushBack(dc);
    }
    if(dcType == DREAM3D::DataContainerType::VertexDataContainer)
    {
      VertexDataContainer::Pointer dc = VertexDataContainer::New();
      dc->setName(dcProxy.name);
      this->pushBack(dc);
    }
    hid_t dcGid = H5Gopen(dcaGid, dcProxy.name.toLatin1().data(), H5P_DEFAULT );
    if (dcGid < 0)
    {
      //QString ss = QObject::tr("Error opening Group %1").arg(dcNames[iter]);
      //setErrorCondition(-61);
      //notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return -1;
    }

    this->getDataContainer(dcProxy.name)->readMeshDataFromHDF5(dcGid, preflight);
    err = this->getDataContainer(dcProxy.name)->readAttributeMatricesFromHDF5(preflight, dcGid, dcProxy);
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerArray::contains(const QString& name)
{
  for(QList<DataContainer::Pointer>::iterator it = m_Array.begin(); it != m_Array.end(); ++it)
  {
    if( (*it)->getName().compare(name) == 0 )
    { return true; }
  }
  return false;
}
