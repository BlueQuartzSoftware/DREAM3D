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
  QString dcName = m_Array.front()->getName();
  removeDataContainerFromBundles(dcName);
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
void DataContainerArray::addDataContainer(DataContainer::Pointer f)
{
  m_Array.push_back(f);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::popBack()
{
  QString dcName = m_Array.back()->getName();
  removeDataContainerFromBundles(dcName);
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
  QString dcName = (*it)->getName();
  removeDataContainerFromBundles(dcName);
  m_Array.erase(it);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::clear()
{
  m_Array.clear();
  m_DataContainerBundles.clear();
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
  removeDataContainerFromBundles(name);
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
bool DataContainerArray::renameDataContainer(const QString& oldName, const QString& newName)
{
  DataContainer::Pointer dc = DataContainer::NullPointer();

  // Make sure we do not already have a DataContainer with the newname
  for(QList<DataContainer::Pointer>::iterator it = m_Array.begin(); it != m_Array.end(); ++it)
  {
    if((*it)->getName().compare(newName) == 0)
    {
      dc = *it;
      break;
    }
  }

  if(NULL == dc)
  {
    // We did not find any data container that matches the new name so we can rename if we find one that matches
    // the 'oldname' argument
    // Now find the data container we want to rename
    for(QList<DataContainer::Pointer>::iterator it = m_Array.begin(); it != m_Array.end(); ++it)
    {
      if((*it)->getName().compare(oldName) == 0)
      {
        // we have an existing DataContainer that matches our "oldname" that we want to rename so all is good.
        dc = *it;
        dc->setName(newName);
        return true;
      }
    }
  }
  else if (NULL != dc)
  {
    // We found an existing Data Container with the 'newname' but we do NOT want to over write it so just bail out now
    return false;
  }

  return false; // default (but we should just NEVER make it here)
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
DataContainer::Pointer DataContainerArray::getDataContainer(const DataArrayPath& path)
{
  QString dcName = path.getDataContainerName();
  return getDataContainer(dcName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::Pointer DataContainerArray::getAttributeMatrix(const DataArrayPath& path)
{
  DataContainer::Pointer dc = getDataContainer(path);
  if(NULL == dc.get() ) { return AttributeMatrix::NullPointer(); }

  return dc->getAttributeMatrix(path.getAttributeMatrixName() );
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::duplicateDataContainer(const QString& name, const QString& newName)
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

  if(f == NULL) { return; }

  DataContainer::Pointer new_f = f->deepCopy();
  new_f->setName(newName);
  pushBack(new_f);
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerArray::readDataContainersFromHDF5(bool preflight,
                                                   hid_t dcaGid,
                                                   DataContainerArrayProxy& dcaProxy,
                                                   Observable* obs)
{
  int err = 0;
  QList<DataContainerProxy> dcsToRead = dcaProxy.list;
  uint32_t dcType = DREAM3D::DataContainerType::UnknownDataContainer;
  QListIterator<DataContainerProxy> dcIter(dcsToRead);
  while (dcIter.hasNext()) // DataContainerLevel
  {
    const DataContainerProxy& dcProxy =  dcIter.next();
    if(dcProxy.flag == Qt::Unchecked) { continue; }
    if (this->contains(dcProxy.name) == true )
    {
      if(NULL != obs)
      {
        QString ss = QObject::tr("A Data Container with name %1 already exists in Memory. Reading a Data Container with the same name would over write the one in memory. Currently this is not allowed.").arg(dcProxy.name);
        obs->notifyErrorMessage(getNameOfClass(), ss, -198745600);
      }
      return -198745600;
    }
    err = QH5Lite::readScalarAttribute(dcaGid, dcProxy.name, DREAM3D::StringConstants::DataContainerType, dcType);
    if (err < 0)
    {
      if(NULL != obs)
      {
        QString ss = QObject::tr("The DataContainer is missing the 'DataContainerType' attribute on the '%1' Data Container").arg(dcProxy.name);
        obs->notifyErrorMessage(getNameOfClass(), ss, -198745601);
      }
      return -198745601;
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

    // Now open the DataContainer Group in the HDF5 file
    hid_t dcGid = H5Gopen(dcaGid, dcProxy.name.toLatin1().data(), H5P_DEFAULT );
    if (dcGid < 0)
    {
      if(NULL != obs)
      {
        QString ss = QObject::tr("Error opening Group '%1'").arg(dcProxy.name);
        obs->notifyErrorMessage(getNameOfClass(), ss, -198745602);
      }
      return -198745602;
    }

    err = this->getDataContainer(dcProxy.name)->readMeshDataFromHDF5(dcGid, preflight);
    if(err < 0)
    {
      if(NULL != obs)
      {
        QString ss = QObject::tr("Error reading Mesh Data from '%1'").arg(dcProxy.name);
        obs->notifyErrorMessage(getNameOfClass(), ss, -198745603);
      }
      return -198745603;
    }
    err = this->getDataContainer(dcProxy.name)->readAttributeMatricesFromHDF5(preflight, dcGid, dcProxy);
    if (err < 0)
    {
      if(NULL != obs)
      {
        QString ss = QObject::tr("Error reading AttributeMatrix Data from '%1'").arg(dcProxy.name);
        obs->notifyErrorMessage(getNameOfClass(), ss, -198745604);
      }
      return -198745604;
    }
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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::setDataContainerBundles(QMap<QString, IDataContainerBundle::Pointer> bundles)
{
  m_DataContainerBundles = bundles;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<QString, IDataContainerBundle::Pointer>& DataContainerArray::getDataContainerBundles()
{
  return m_DataContainerBundles;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::addDataContainerBundle(IDataContainerBundle::Pointer dataContainer)
{
  m_DataContainerBundles[dataContainer->getName()] = dataContainer;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainerArray::removeDataContainerBundle(const QString &name)
{
  return m_DataContainerBundles.remove(name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainerArray::removeDataContainerFromBundles(const QString &name)
{
  for(QMap<QString, IDataContainerBundle::Pointer>::iterator iter = m_DataContainerBundles.begin(); iter != m_DataContainerBundles.end(); ++iter)
  {
    IDataContainerBundle::Pointer dcbPtr = iter.value();
    if (dcbPtr.get() != NULL)
    {
      QVector<QString> dcbNames = dcbPtr->getDataContainerNames();
      for (size_t i=0;i<dcbNames.size();i++)
      {
        if (dcbNames[i].compare(name) == 0)
        {
          dcbPtr->remove(name);
        }
      }
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainerArray::renameDataContainerBundle(const QString &oldName, const QString newName)
{
  // Make sure we do not already have a DataContainerBundle with the newname
  QMap<QString, IDataContainerBundle::Pointer>::iterator iter = m_DataContainerBundles.find(newName);

  // Now rename the DataContainerBundle
  if (iter == m_DataContainerBundles.end() )
  {
    iter = m_DataContainerBundles.find(oldName);
    if (iter == m_DataContainerBundles.end() )
    {
      return false;
    }
    else
    {
      m_DataContainerBundles.insert(newName, iter.value() );
      iter.value()->setName(newName);
      m_DataContainerBundles.remove(oldName);
    }
  }
  return false;
}
