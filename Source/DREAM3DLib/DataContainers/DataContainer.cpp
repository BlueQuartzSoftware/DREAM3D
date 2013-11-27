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
DataContainer::~DataContainer()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainer::doesAttributeMatrixExist(const QString &name)
{
  return  m_AttributeMatrices.contains(name);
}
  
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainer::addAttributeMatrix(const QString &name, AttributeMatrix::Pointer data)
{
  if (data->getName().compare(name) != 0)
  {
    qDebug() << "Adding Attribute Array with different array name than key name" << "\n";
    qDebug() << "Key name: " << name << "\n";
    qDebug() << "Array Name:" << data->getName() << "\n";
    data->setName(name);
  }
  m_AttributeMatrices[name] = data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::Pointer DataContainer::getAttributeMatrix(const QString &name)
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
AttributeMatrix::Pointer DataContainer::removeAttributeMatrix(const QString &name)
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
bool DataContainer::renameAttributeMatrix(const QString &oldname, const QString &newname)
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
  addAttributeMatrix(newname, p);
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
//      QString ss = QObject::tr("Error creating HDF Group ").arg(iter.key());
//      setErrorCondition(-63);
//      addErrorMessage(getHumanLabel(), ss, err);
      H5Gclose(parentId); // Close the Data Container Group
      return err;
    }
    attributeMatrixId = H5Gopen(parentId, iter.key().toLatin1().data(), H5P_DEFAULT);
    err = QH5Lite::writeScalarAttribute(parentId, iter.key(), DREAM3D::HDF5::AttributeMatrixType, (*iter)->getAMType());
    if(err < 0)
    {
//      QString ss = QObject::tr("Error writing string attribute to HDF Group %1").arg(iter.key());
//      setErrorCondition(-64);
//      addErrorMessage(getHumanLabel(), ss, err);
      H5Gclose(parentId); // Close the Data Container Group
      return err;
    }
    err = (*iter)->writeAttributeArraysToHDF5(attributeMatrixId);
    if(err < 0)
    {
      H5Gclose(parentId); // Close the Data Container Group
      return err;
    }
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainer::writeMeshToHDF5(hid_t dcGid)
{
  BOOST_ASSERT(false);
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainer::writeXdmf(QTextStream& out)
{
  BOOST_ASSERT(false);
  return -1;
}