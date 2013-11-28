/* ============================================================================
 * Copyright (c) 2009, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2009, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "DREAM3DLib/DataContainers/AttributeMatrix.h"

// C Includes

// C++ Includes
#include <iostream>
#include <fstream>

//HDF5 Includes
#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"

// EbsdLib Includes
#include "EbsdLib/EbsdConstants.h"

// DREAM3D Includes
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#include "DREAM3DLib/HDF5/VTKH5Constants.h"
#include "DREAM3DLib/HDF5/H5DataArrayReader.h"
#include "DREAM3DLib/DataArrays/StatsDataArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::AttributeMatrix() :
  m_NumTuples(0)
{
  setName(DREAM3D::HDF5::AttributeMatrixName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::~AttributeMatrix()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AttributeMatrix::doesAttributeArrayExist(const QString& name)
{
  return  m_AttributeMatrix.contains(name);
  \
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrix::addAttributeArray(const QString& name, IDataArray::Pointer data)
{
  if (data->GetName().compare(name) != 0)
  {
    qDebug() << "Adding Attribute Array with different array name than key name" << "\n";
    qDebug() << "Key name: " << name << "\n";
    qDebug() << "Array Name:" << data->GetName() << "\n";
    data->SetName(name);
  }
  m_AttributeMatrix[name] = data;
  m_NumTuples = data->getNumberOfTuples();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer AttributeMatrix::getAttributeArray(const QString& name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_AttributeMatrix.find(name);
  if ( it == m_AttributeMatrix.end() )
  {
    return IDataArray::NullPointer();
  }
  return it.value();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer AttributeMatrix::removeAttributeArray(const QString& name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_AttributeMatrix.find(name);
  if ( it == m_AttributeMatrix.end() )
  {
    return IDataArray::NullPointer();
  }
  IDataArray::Pointer p = it.value();
  m_AttributeMatrix.erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AttributeMatrix::renameAttributeArray(const QString& oldname, const QString& newname)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_AttributeMatrix.find(oldname);
  if ( it == m_AttributeMatrix.end() )
  {
    return false;
  }
  IDataArray::Pointer p = it.value();
  p->SetName(newname);
  removeAttributeArray(oldname);
  addAttributeArray(newname, p);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrix::resizeAttributeArrays(size_t size)
{
// int success = 0;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_AttributeMatrix.begin(); iter != m_AttributeMatrix.end(); ++iter)
  {
    //std::cout << "Resizing Array '" << (*iter).first << "' : " << success << std::endl;
    IDataArray::Pointer d = iter.value();
    d->Resize(size);
  }
  m_NumTuples = size;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrix::clearAttributeArrays()
{
  m_AttributeMatrix.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> AttributeMatrix::getAttributeArrayNameList()
{
  QList<QString> keys;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_AttributeMatrix.begin(); iter != m_AttributeMatrix.end(); ++iter)
  {
    keys.push_back( iter.key() );
  }
  return keys;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AttributeMatrix::getNumAttributeArrays()
{
  return static_cast<int>(m_AttributeMatrix.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AttributeMatrix::writeAttributeArraysToHDF5(hid_t parentId)
{
  int err;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_AttributeMatrix.begin(); iter != m_AttributeMatrix.end(); ++iter)
  {
//    QString ss = QObject::tr("Writing Data '%1' to HDF5 File").arg(*iter);
//    notifyStatusMessage(ss);
    IDataArray::Pointer d = iter.value();
    err = d->writeH5Data(parentId);
    if(err < 0)
    {
//      ss = QObject::tr("Error writing array '%1' to the HDF5 File").arg(*iter);
//      addErrorMessage(getHumanLabel(), ss, err);
//      setErrorCondition(err);
      H5Gclose(parentId); // Close the Cell Group
      return err;
    }
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AttributeMatrix::readAttributeArraysFromHDF5(hid_t amGid, bool preflight, QSet<QString>& namesToRead)
{
  int err = 0;

  QList<QString> names;
  QH5Utilities::getGroupObjects(amGid, H5Utilities::H5Support_DATASET | H5Utilities::H5Support_ANY, names);
  //  qDebug() << "Number of Items in " << groupName << " Group: " << names.size() << "\n";
  QString classType;
  for (QList<QString>::iterator iter = names.begin(); iter != names.end(); ++iter)
  {
    QSet<QString>::iterator contains = namesToRead.find(*iter);
    if (contains == namesToRead.end()) { continue; } // Do not read this item if it is NOT in the set of arrays to read
    classType.clear();
    QH5Lite::readStringAttribute(amGid, *iter, DREAM3D::HDF5::ObjectType, classType);
    //   qDebug() << groupName << " Array: " << *iter << " with C++ ClassType of " << classType << "\n";
    IDataArray::Pointer dPtr = IDataArray::NullPointer();

    if(classType.startsWith("DataArray") == true)
    {
      dPtr = H5DataArrayReader::readIDataArray(amGid, *iter, preflight);
    }
    else if(classType.compare("StringDataArray") == 0)
    {
      dPtr = H5DataArrayReader::readStringDataArray(amGid, *iter, preflight);
    }
    else if(classType.compare("vector") == 0)
    {

    }
    else if(classType.compare("NeighborList<T>") == 0)
    {
      dPtr = H5DataArrayReader::readNeighborListData(amGid, *iter, preflight);
    }
    else if ( (*iter).compare(DREAM3D::EnsembleData::Statistics) == 0)
    {
      StatsDataArray::Pointer statsData = StatsDataArray::New();
      statsData->SetName(DREAM3D::EnsembleData::Statistics);
      statsData->readH5Data(amGid);
      dPtr = statsData;
    }

    if (NULL != dPtr.get())
    {
      addAttributeArray(dPtr->GetName(), dPtr);
    }

  }
  H5Gclose(amGid); // Close the Cell Group
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AttributeMatrix::generateXdmfText(const QString& centering, const QString& dataContainerName, const QString& hdfFileName)
{
  QString xdmfText;
  QString block;
  QTextStream out(&xdmfText);

  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_AttributeMatrix.begin(); iter != m_AttributeMatrix.end(); ++iter)
  {
    IDataArray::Pointer d = iter.value();
    block = writeXdmfAttributeData(d, centering, dataContainerName, hdfFileName);
    out << block << "\n";
  }
  return xdmfText;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AttributeMatrix::writeXdmfAttributeDataHelper(int numComp, const QString& attrType,
                                                      const QString& dataContainerName,
                                                      IDataArray::Pointer array,
                                                      const QString& centering,
                                                      int precision, const QString& xdmfTypeName, const QString& hdfFileName)
{
  QString buf;
  QTextStream out(&buf);

  QString dimStr = QString::number(array->getNumberOfTuples()) + QString(" ") + QString::number(array->GetNumberOfComponents());
  QString dimStrHalf = QString::number(array->getNumberOfTuples()) + QString(" ") + QString::number(array->GetNumberOfComponents() / 2);

  if((numComp % 2) == 1)
  {
    out << "    <Attribute Name=\"" << array->GetName() << "\" ";
    out << "AttributeType=\"" << attrType << "\" ";
    out << "Center=\"" << centering << "\">" << "\n";
    // Open the <DataItem> Tag
    out << "      <DataItem Format=\"HDF\" Dimensions=\"" << dimStr <<  "\" ";
    out << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << "\n";
    out << "        " << hdfFileName << ":/DataContainers/" << dataContainerName << "/" << getName() << "/" << array->GetName() << "\n";
    out << "      </DataItem>" << "\n";
    out << "    </Attribute>" << "\n" << "\n";
  }
  else
  {
    //First Slab
    out << "    <Attribute Name=\"" << array->GetName() << " (Feature 0)\" ";
    out << "AttributeType=\"" << attrType << "\" ";

    out << "Center=\"" << centering << "\">" << "\n";
    // Open the <DataItem> Tag
    out << "      <DataItem ItemType=\"HyperSlab\" Dimensions=\"" << dimStrHalf <<  "\" ";
    out << "Type=\"HyperSlab\" " << "Name=\"" << array->GetName() << " (Feature 0)\" >" << "\n";
    out << "        <DataItem Dimensions=\"3 2\" " << "Format=\"XML\" >" << "\n";
    out << "          0        0" << "\n";
    out << "          1        1" << "\n";
    out << "          " << dimStrHalf << " </DataItem>" << "\n";
    out << "\n";
    out << "        <DataItem Format=\"HDF\" Dimensions=\"" << dimStr << "\" " << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << "\n";

    out << "        " << hdfFileName << ":/DataContainers/" << dataContainerName << "/" << getName() << "/" << array->GetName() << "\n";
    out << "        </DataItem>" << "\n";
    out << "      </DataItem>" << "\n";
    out << "    </Attribute>" << "\n" << "\n";

    //Second Slab
    out << "    <Attribute Name=\"" << array->GetName() << " (Feature 1)\" ";
    out << "AttributeType=\"" << attrType << "\" ";

    out << "Center=\"" << centering << "\">" << "\n";
    // Open the <DataItem> Tag
    out << "      <DataItem ItemType=\"HyperSlab\" Dimensions=\"" << dimStrHalf <<  "\" ";
    out << "Type=\"HyperSlab\" " << "Name=\"" << array->GetName() << " (Feature 1)\" >" << "\n";
    out << "        <DataItem Dimensions=\"3 2\" " << "Format=\"XML\" >" << "\n";
    out << "          0        " << (array->GetNumberOfComponents() / 2) << "\n";
    out << "          1        1" << "\n";
    out << "          " << dimStrHalf << " </DataItem>" << "\n";
    out << "\n";
    out << "        <DataItem Format=\"HDF\" Dimensions=\"" << dimStr << "\" " << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << "\n";
    out << "        " << hdfFileName << ":/DataContainers/" << dataContainerName << "/" << getName() << "/" << array->GetName() << "\n";
    out << "        </DataItem>" << "\n";
    out << "      </DataItem>" << "\n";
    out << "    </Attribute>" << "\n" << "\n";
  }
  return buf;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AttributeMatrix::writeXdmfAttributeData(IDataArray::Pointer array, const QString& centering, const QString& dataContainerName, const QString& hdfFileName)
{
  QString xdmfText;
  QTextStream out(&xdmfText);

  int precision = 0;
  QString xdmfTypeName;
  array->GetXdmfTypeAndSize(xdmfTypeName, precision);
  if (0 == precision)
  {
    out << "<!-- " << array->GetName() << " has unkown type or unsupported type or precision for XDMF to understand" << " -->" << "\n";
    return xdmfText;
  }
  int numComp = array->GetNumberOfComponents();
  QString attrType = "Scalar";
  if(numComp > 2) { attrType = "Vector"; }

  QString block = writeXdmfAttributeDataHelper(numComp, attrType, dataContainerName, array, centering, precision, xdmfTypeName, hdfFileName);

  out << block << "\n";

  return xdmfText;
}