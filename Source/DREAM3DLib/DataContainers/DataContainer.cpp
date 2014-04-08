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
#include "DataContainer.h"

#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"
#include "H5Support/HDF5ScopedFileSentinel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::DataContainer() :
  Observable()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::DataContainer(const QString name) :
  Observable(),
  m_Name(name)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::~DataContainer()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainer::ReadDataContainerStructure(hid_t dcArrayGroupId, DataContainerArrayProxy &proxy, QString h5InternalPath)
{
  QList<QString> dataContainers;
  QH5Utilities::getGroupObjects(dcArrayGroupId, H5Utilities::H5Support_GROUP, dataContainers);
  foreach(QString dataContainerName, dataContainers)
  {
    if(__SHOW_DEBUG_MSG__) { std::cout << "Data Container:" << dataContainerName.toStdString() << std::endl; }
    hid_t containerGid = H5Gopen(dcArrayGroupId, dataContainerName.toAscii().constData(), H5P_DEFAULT);
    if (containerGid < 0) { continue; }
    HDF5ScopedGroupSentinel sentinel(&containerGid, false);
    DataContainerProxy dcProxy(dataContainerName);
    dcProxy.name = dataContainerName;
    dcProxy.flag = Qt::Checked;
    herr_t err = QH5Lite::readScalarAttribute(dcArrayGroupId, dataContainerName, DREAM3D::StringConstants::DataContainerType, dcProxy.dcType);
    if(err < 0) { std::cout << "Error Reading the DataContainer Type for DataContainer " << dataContainerName.toStdString() << std::endl; }

    QString h5Path = h5InternalPath + "/" + dataContainerName;
    // Read the Attribute Matricies for this Data Container
    AttributeMatrix::ReadAttributeMatrixStructure(containerGid, dcProxy, h5Path);

    // Insert the DataContainerProxy proxy into the DataContainerArrayProxy
    proxy.list.push_back(dcProxy);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::AttributeMatrixMap_t& DataContainer::getAttributeMatrices()
{
  return m_AttributeMatrices;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainer::doesAttributeMatrixExist(const QString& name)
{
  return  m_AttributeMatrices.contains(name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::Pointer DataContainer::createAndAddAttributeMatrix(QVector<size_t> tDims, const QString& attrMatName, unsigned int attrType)
{
  AttributeMatrix::Pointer attrMat = AttributeMatrix::New(tDims, attrMatName, attrType);
  addAttributeMatrix(attrMatName, attrMat);
  return attrMat; // Return the shared pointer
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainer::addAttributeMatrix(const QString& name, AttributeMatrix::Pointer data)
{
  if (data->getName().compare(name) != 0)
  {
    qDebug() << "Adding Attribute Array with different array name than key name";
    qDebug() << "Key name: " << name;
    qDebug() << "Array Name:" << data->getName();
    qDebug() << "This action is NOT typical of DREAM3D Usage. Are you sure you want to be doing this? We are forcing the name of the AttributeMatrix to be the same as the key";
    data->setName(name);
  }
  m_AttributeMatrices[name] = data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::Pointer DataContainer::getAttributeMatrix(const QString& name)
{
  QMap<QString, AttributeMatrix::Pointer>::iterator it;
  it =  m_AttributeMatrices.find(name);
  if ( it == m_AttributeMatrices.end() )
  {
    return AttributeMatrix::NullPointer();
  }
  return it.value();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::Pointer DataContainer::removeAttributeMatrix(const QString& name)
{
  QMap<QString, AttributeMatrix::Pointer>::iterator it;
  it =  m_AttributeMatrices.find(name);
  if ( it == m_AttributeMatrices.end() )
  {
    return AttributeMatrix::NullPointer();
  }
  AttributeMatrix::Pointer p = it.value();
  m_AttributeMatrices.erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainer::renameAttributeMatrix(const QString& oldname, const QString& newname, bool overwrite)
{
  QMap<QString, AttributeMatrix::Pointer>::iterator it;
  it =  m_AttributeMatrices.find(oldname);
  if ( it == m_AttributeMatrices.end() )
  {
    return false;
  }
  AttributeMatrix::Pointer p = it.value();
  p->setName(newname);
  removeAttributeMatrix(oldname);

  // Now check to make sure there isn't one with the same name
  it =  m_AttributeMatrices.find(newname);
  if ( it == m_AttributeMatrices.end() ) // Didn't find another AttributeMatrix with the new name
  {
    addAttributeMatrix(newname, p);
  }
  else if(overwrite == true) // We are here because we found another attribute matrix with the new name
  {
    AttributeMatrix::Pointer removedAttributeMatrix = removeAttributeMatrix(newname); // Remove the existing one
    addAttributeMatrix(newname, p);
  }
  else
  {
    return false;
  }
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainer::clearAttributeMatrices()
{
  m_AttributeMatrices.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> DataContainer::getAttributeMatrixNameList()
{
  QList<QString> keys;
  for(QMap<QString, AttributeMatrix::Pointer>::iterator iter = m_AttributeMatrices.begin(); iter != m_AttributeMatrices.end(); ++iter)
  {
    keys.push_back( iter.key() );
  }
  return keys;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainer::getNumAttributeMatrices()
{
  return static_cast<int>(m_AttributeMatrices.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainer::writeAttributeMatricesToHDF5(hid_t parentId)
{
  int err;
  hid_t attributeMatrixId;
  for(QMap<QString, AttributeMatrix::Pointer>::iterator iter = m_AttributeMatrices.begin(); iter != m_AttributeMatrices.end(); ++iter)
  {
    err = QH5Utilities::createGroupsFromPath(iter.key(), parentId);
    if(err < 0)
    {
      return err;
    }
    attributeMatrixId = H5Gopen(parentId, iter.key().toLatin1().data(), H5P_DEFAULT);
    HDF5ScopedGroupSentinel gSentinel(&attributeMatrixId, false);

    err = QH5Lite::writeScalarAttribute(parentId, iter.key(), DREAM3D::StringConstants::AttributeMatrixType, (*iter)->getType());
    hsize_t size = (*iter)->getTupleDimensions().size();
    err = QH5Lite::writePointerAttribute(parentId, iter.key(), DREAM3D::HDF5::TupleDimensions, 1, &size, (*iter)->getTupleDimensions().data());
    if(err < 0)
    {
      return err;
    }
    err = (*iter)->writeAttributeArraysToHDF5(attributeMatrixId);
    if(err < 0)
    {
      return err;
    }
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainer::readAttributeMatricesFromHDF5(bool preflight, hid_t dcGid, const DataContainerProxy& dcProxy)
{
  int err = 0;
  QVector<size_t> tDims;

  QMap<QString, AttributeMatrixProxy> attrMatsToRead = dcProxy.attributeMatricies;
  unsigned int amType = DREAM3D::AttributeMatrixType::Unknown;
  QString amName;
  for(QMap<QString, AttributeMatrixProxy>::iterator iter = attrMatsToRead.begin(); iter != attrMatsToRead.end(); ++iter)
  {
    if(iter.value().flag == Qt::Unchecked) continue;
    amName = iter.key();
    amType = DREAM3D::AttributeMatrixType::Unknown;
    err = QH5Lite::readScalarAttribute(dcGid, amName, DREAM3D::StringConstants::AttributeMatrixType, amType);
    err = QH5Lite::readVectorAttribute(dcGid, amName, DREAM3D::HDF5::TupleDimensions, tDims);
    if (err < 0)
    {
      return -1;
    }

    hid_t amGid = H5Gopen(dcGid, amName.toLatin1().data(), H5P_DEFAULT );
    if (amGid < 0)
    {
      return -1;
    }

    if(getAttributeMatrix(amName) == NULL)
    {
      AttributeMatrix::Pointer am = AttributeMatrix::New(tDims, amName, amType);
      addAttributeMatrix(amName, am);
    }

    err = getAttributeMatrix(amName)->readAttributeArraysFromHDF5(amGid, preflight, iter.value());
    if(err < 0)
    {
      err |= H5Gclose(dcGid);
      //      setErrorCondition(err);
      return -1;
    }
  }

  err |= H5Gclose(dcGid);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::Pointer DataContainer::deepCopy()
{
  BOOST_ASSERT(false);
  return DataContainer::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainer::writeMeshToHDF5(hid_t dcGid, bool writeXdmf)
{
  BOOST_ASSERT(false);
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainer::writeXdmf(QTextStream &out, QString hdfFileName)
{
  BOOST_ASSERT(false);
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainer::readMeshDataFromHDF5(hid_t dcGid, bool preflight)
{
  BOOST_ASSERT(false);
  return -1;
}
