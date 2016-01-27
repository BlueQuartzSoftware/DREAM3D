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


#include "SIMPLib/DataContainers/AttributeMatrix.h"

// C Includes

// C++ Includes
#include <iostream>
#include <fstream>

//HDF5 Includes
#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"
#include "H5Support/HDF5ScopedFileSentinel.h"

// EbsdLib Includes
#include "EbsdLib/EbsdConstants.h"

// DREAM3D Includes
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Utilities/SIMPLibRandom.h"
#include "SIMPLib/HDF5/VTKH5Constants.h"
#include "SIMPLib/HDF5/H5DataArrayReader.h"
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/DataContainers/AttributeMatrixProxy.h"
#include "SIMPLib/DataContainers/DataContainerProxy.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::AttributeMatrix(QVector<size_t> tDims, const QString& name, unsigned int attrType) :
  m_Name(name),
  m_TupleDims(tDims),
  m_Type(attrType)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::~AttributeMatrix()
{
  // std::cout << "~AttributeMatrix" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrix::ReadAttributeMatrixStructure(hid_t containerId, DataContainerProxy& dcProxy, QString h5InternalPath)
{
  QList<QString> attributeMatrixNames;
  QH5Utilities::getGroupObjects(containerId, H5Utilities::H5Support_GROUP, attributeMatrixNames);
  foreach(QString attributeMatrixName, attributeMatrixNames)
  {
    if (attributeMatrixName.compare(DREAM3D::Geometry::Geometry) != 0)
    {
      if(__SHOW_DEBUG_MSG__)
      {
        std::cout << "    AttributeMatrix: " << attributeMatrixName.toStdString()  << std::endl;
      }
      hid_t attrMatGid = H5Gopen(containerId, attributeMatrixName.toLatin1().constData(), H5P_DEFAULT);
      if (attrMatGid < 0)
      {
        continue;
      }
      HDF5ScopedGroupSentinel sentinel(&attrMatGid, true);

      AttributeMatrixProxy amProxy(attributeMatrixName);
      amProxy.name = attributeMatrixName;
      amProxy.flag = Qt::Checked;
      herr_t err = QH5Lite::readScalarAttribute(containerId, attributeMatrixName, DREAM3D::StringConstants::AttributeMatrixType, amProxy.amType);
      if(err < 0)
      {
        std::cout << "Error Reading the AttributeMatrix Type for AttributeMatrix " << attributeMatrixName.toStdString() << std::endl;
      }

      QString h5Path = h5InternalPath + "/" + attributeMatrixName;

      // Read in the names of the Data Arrays that make up the AttributeMatrix
      QMap<QString, DataArrayProxy>& daProxies = amProxy.dataArrays;
      DataArrayProxy::ReadDataArrayStructure(attrMatGid, daProxies, h5Path);

      // Insert the AttributeMatrixProxy proxy into the dataContainer proxy
      dcProxy.attributeMatricies.insert(attributeMatrixName, amProxy);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrix::setType(uint32_t value)
{
  m_Type = value;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint32_t AttributeMatrix::getType()
{
  return m_Type;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AttributeMatrix::doesAttributeArrayExist(const QString& name)
{
  return  m_AttributeArrays.contains(name);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AttributeMatrix::validateAttributeArraySizes()
{
  int64_t arraySize = 0;
  int64_t matrixSize = getNumTuples();
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_AttributeArrays.begin(); iter != m_AttributeArrays.end(); ++iter)
  {
    IDataArray::Pointer d = iter.value();
    arraySize = d->getNumberOfTuples();
    if(arraySize != matrixSize)
    {
      return false;
    }
  }
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AttributeMatrix::addAttributeArray(const QString& name, IDataArray::Pointer data)
{
  if (data->getName().compare(name) != 0)
  {
    qDebug() << "Adding Attribute Array with different array name than key name" << "\n";
    qDebug() << "Key name: " << name << "\n";
    qDebug() << "Array Name:" << data->getName() << "\n";
    data->setName(name);
  }
  if(getNumTuples() != data->getNumberOfTuples())
  {
    qDebug() << "AttributeMatrix::Name: " << getName() << "  dataArray::name:  " << data->getName() << " Type: " << data->getTypeAsString();
    qDebug() << "getNumTuples(): " << getNumTuples() << "  data->getNumberOfTuples(): " << data->getNumberOfTuples();
  }
  Q_ASSERT(getNumTuples() == data->getNumberOfTuples());

  m_AttributeArrays[name] = data;
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IDataArray::Pointer AttributeMatrix::getAttributeArray(const QString& name)
{
  QMap<QString, IDataArray::Pointer>::iterator it;
  it =  m_AttributeArrays.find(name);
  if ( it == m_AttributeArrays.end() )
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
  it =  m_AttributeArrays.find(name);
  if ( it == m_AttributeArrays.end() )
  {
    // DO NOT return a NullPointer for any reason other than "Data Array was not found"
    return IDataArray::NullPointer();
  }
  IDataArray::Pointer p = it.value();
  m_AttributeArrays.erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenameErrorCodes AttributeMatrix::renameAttributeArray(const QString& oldname, const QString& newname, bool overwrite)
{
  QMap<QString, IDataArray::Pointer>::iterator itOld;
  QMap<QString, IDataArray::Pointer>::iterator itNew;

  itNew = m_AttributeArrays.find(newname);
  // If new name doesn't exist or we want to overwrite one that does exist...
  if (itNew == m_AttributeArrays.end() || overwrite == true)
  {
    itOld =  m_AttributeArrays.find(oldname);
    // If old name doesn't exist...
    if (itOld == m_AttributeArrays.end())
    {
      return OLD_DOES_NOT_EXIST;
    }
    IDataArray::Pointer p = itOld.value();
    p->setName(newname);
    removeAttributeArray(oldname);
    addAttributeArray(newname, p);
    return SUCCESS;
  }
  return NEW_EXISTS;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<size_t> AttributeMatrix::getTupleDimensions()
{
  return m_TupleDims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrix::setTupleDimensions(QVector<size_t> tupleDims)
{
  resizeAttributeArrays(tupleDims);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t AttributeMatrix::getNumTuples()
{
  size_t numTuples = m_TupleDims[0];
  for(int i = 1; i < m_TupleDims.size(); i++)
  {
    numTuples *= m_TupleDims[i];
  }
  return numTuples;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool AttributeMatrix::removeInactiveObjects(QVector<bool> activeObjects, Int32ArrayType::Pointer Ids)
{
  bool acceptableMatrix = false;
  //Only valid for feature or ensemble type matrices
  if(m_Type == DREAM3D::AttributeMatrixType::VertexFeature || m_Type == DREAM3D::AttributeMatrixType::VertexEnsemble ||
      m_Type == DREAM3D::AttributeMatrixType::EdgeFeature || m_Type == DREAM3D::AttributeMatrixType::EdgeEnsemble ||
      m_Type == DREAM3D::AttributeMatrixType::FaceFeature || m_Type == DREAM3D::AttributeMatrixType::FaceEnsemble ||
      m_Type == DREAM3D::AttributeMatrixType::CellFeature || m_Type == DREAM3D::AttributeMatrixType::CellEnsemble)
  {
    acceptableMatrix = true;
  }
  size_t totalTuples = getNumTuples();
  if( static_cast<size_t>(activeObjects.size()) == totalTuples && acceptableMatrix == true)
  {
    size_t goodcount = 1;
    QVector<size_t> NewNames(totalTuples, 0);
    QVector<size_t> RemoveList;

    for(qint32 i = 1; i < activeObjects.size(); i++)
    {
      if(activeObjects[i] == false)
      {
        RemoveList.push_back(i);
        NewNames[i] = 0;
      }
      else
      {
        NewNames[i] = goodcount;
        goodcount++;
      }
    }

    if(RemoveList.size() > 0)
    {
      QList<QString> headers = getAttributeArrayNames();
      for (QList<QString>::iterator iter = headers.begin(); iter != headers.end(); ++iter)
      {
        IDataArray::Pointer p = getAttributeArray(*iter);
        QString type = p->getTypeAsString();
        if(type.compare("NeighborList<T>") == 0)
        {
          removeAttributeArray(*iter);
        }
        else
        {
          p->eraseTuples(RemoveList);
        }
      }
      QVector<size_t> tDims(1, (totalTuples - RemoveList.size()));
      setTupleDimensions(tDims);

      // Loop over all the points and correct all the feature names
      size_t totalPoints = Ids->getNumberOfTuples();
      int32_t* id = Ids->getPointer(0);
      for (size_t i = 0; i < totalPoints; i++)
      {
        if(id[i] >= 0 && id[i] < NewNames.size())
        {
          id[i] = static_cast<int32_t>( NewNames[id[i]] );
        }
      }
    }
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
void AttributeMatrix::resizeAttributeArrays(QVector<size_t> tDims)
{
  // int success = 0;
  m_TupleDims = tDims;
  size_t numTuples = m_TupleDims[0];
  for(int i = 1; i < m_TupleDims.size(); i++)
  {
    numTuples *= m_TupleDims[i];
  }

  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_AttributeArrays.begin(); iter != m_AttributeArrays.end(); ++iter)
  {
    //std::cout << "Resizing Array '" << (*iter).first << "' : " << success << std::endl;
    IDataArray::Pointer d = iter.value();
    d->resize(numTuples);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AttributeMatrix::clearAttributeArrays()
{
  m_AttributeArrays.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> AttributeMatrix::getAttributeArrayNames()
{
  QList<QString> keys;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_AttributeArrays.begin(); iter != m_AttributeArrays.end(); ++iter)
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
  return static_cast<int>(m_AttributeArrays.size());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::Pointer AttributeMatrix::deepCopy()
{
  AttributeMatrix::Pointer newAttrMat = AttributeMatrix::New(getTupleDimensions(), getName(), getType());

  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_AttributeArrays.begin(); iter != m_AttributeArrays.end(); ++iter)
  {
    IDataArray::Pointer d = iter.value();
    IDataArray::Pointer new_d = d->deepCopy();
    if (new_d.get() == NULL)
    {
      return AttributeMatrix::NullPointer();
    }
    newAttrMat->addAttributeArray(new_d->getName(), new_d);
  }

  return newAttrMat;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AttributeMatrix::writeAttributeArraysToHDF5(hid_t parentId)
{
  int err;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_AttributeArrays.begin(); iter != m_AttributeArrays.end(); ++iter)
  {
    IDataArray::Pointer d = iter.value();
    err = d->writeH5Data(parentId, m_TupleDims);
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
int AttributeMatrix::addAttributeArrayFromHDF5Path(hid_t gid, QString name, bool preflight)
{
  int err = 0;
  QString classType;
  QH5Lite::readStringAttribute(gid, name, DREAM3D::HDF5::ObjectType, classType);
  //   qDebug() << groupName << " Array: " << *iter << " with C++ ClassType of " << classType << "\n";
  IDataArray::Pointer dPtr = IDataArray::NullPointer();

  if(classType.startsWith("DataArray") == true)
  {
    dPtr = H5DataArrayReader::ReadIDataArray(gid, name, preflight);
    if(preflight == true)
    {
      dPtr->resize(getNumTuples());
    }
  }
  else if(classType.compare("StringDataArray") == 0)
  {
    dPtr = H5DataArrayReader::ReadStringDataArray(gid, name, preflight);
    if (preflight == true)
    {
      dPtr->resize(getNumTuples());
    }
  }
  else if(classType.compare("vector") == 0)
  {

  }
  else if(classType.compare("NeighborList<T>") == 0)
  {
    dPtr = H5DataArrayReader::ReadNeighborListData(gid, name, preflight);
    if (preflight == true)
    {
      dPtr->resize(getNumTuples());
    }
  }
  else if ( name.compare(DREAM3D::EnsembleData::Statistics) == 0)
  {
    StatsDataArray::Pointer statsData = StatsDataArray::New();
    statsData->setName(DREAM3D::EnsembleData::Statistics);
    statsData->readH5Data(gid);
    dPtr = statsData;
    if (preflight == true)
    {
      dPtr->resize(getNumTuples());
    }
  }

  if (NULL != dPtr.get())
  {
    addAttributeArray(dPtr->getName(), dPtr);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AttributeMatrix::readAttributeArraysFromHDF5(hid_t amGid, bool preflight, AttributeMatrixProxy& attrMatProxy)
{
  int err = 0;
  QMap<QString, DataArrayProxy> dasToRead = attrMatProxy.dataArrays;
  QString classType;
  for (QMap<QString, DataArrayProxy>::iterator iter = dasToRead.begin(); iter != dasToRead.end(); ++iter)
  {
    //qDebug() << "Reading the " << iter->name << " Array from the " << m_Name << " Attribute Matrix \n";
    if(iter->flag == DREAM3D::Unchecked)
    {
      continue;
    }
    QH5Lite::readStringAttribute(amGid, iter->name, DREAM3D::HDF5::ObjectType, classType);
    //   qDebug() << groupName << " Array: " << *iter << " with C++ ClassType of " << classType << "\n";
    IDataArray::Pointer dPtr = IDataArray::NullPointer();

    if(classType.startsWith("DataArray") == true)
    {
      dPtr = H5DataArrayReader::ReadIDataArray(amGid, iter->name, preflight);
    }
    else if(classType.compare("StringDataArray") == 0)
    {
      dPtr = H5DataArrayReader::ReadStringDataArray(amGid, iter->name, preflight);
    }
    else if(classType.compare("vector") == 0)
    {

    }
    else if(classType.compare("NeighborList<T>") == 0)
    {
      dPtr = H5DataArrayReader::ReadNeighborListData(amGid, iter->name, preflight);
    }
    else if(classType.compare("Statistics") == 0)
    {
      StatsDataArray::Pointer statsData = StatsDataArray::New();
      statsData->setName(iter->name);
      statsData->readH5Data(amGid);
      dPtr = statsData;
    }
    //    else if ( (iter->name).compare(DREAM3D::EnsembleData::Statistics) == 0)
    //    {
    //      StatsDataArray::Pointer statsData = StatsDataArray::New();
    //      statsData->setName(DREAM3D::EnsembleData::Statistics);
    //      statsData->readH5Data(amGid);
    //      dPtr = statsData;
    //    }

    if (NULL != dPtr.get())
    {
      addAttributeArray(dPtr->getName(), dPtr);
    }

  }
  H5Gclose(amGid); // Close the Cell Group
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AttributeMatrix::generateXdmfText(const QString& centering, const QString& dataContainerName, const QString& hdfFileName, const uint8_t gridType)
{
  QString xdmfText;
  QString block;
  QTextStream out(&xdmfText);

  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_AttributeArrays.begin(); iter != m_AttributeArrays.end(); ++iter)
  {
    IDataArray::Pointer d = iter.value();
    block = writeXdmfAttributeData(d, centering, dataContainerName, hdfFileName, gridType);
    out << block;
  }
  return xdmfText;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AttributeMatrix::getInfoString(DREAM3D::InfoStringFormat format)
{
  QString info;
  QTextStream ss (&info);
  if(format == DREAM3D::HtmlFormat)
  {
    ss << "<html><head></head>\n";
    ss << "<body>\n";
    ss << "<table cellpadding=\"4\" cellspacing=\"0\" border=\"0\">\n";
    ss << "<tbody>\n";
    ss << "<tr bgcolor=\"#D3D8E0\"><th colspan=2>Attribute Matrix Info</th></tr>";

    ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Name:</th><td>" << getName() << "</td></tr>";

    QString typeString;
    switch(m_Type)
    {
      case DREAM3D::AttributeMatrixType::Vertex:
        typeString = "Vertex";
        break;
      case DREAM3D::AttributeMatrixType::Edge:
        typeString = "Edge";
        break;
      case DREAM3D::AttributeMatrixType::Face:
        typeString = "Face";
        break;
      case DREAM3D::AttributeMatrixType::Cell:
        typeString = "Cell";
        break;
      case DREAM3D::AttributeMatrixType::VertexFeature:
        typeString = "Vertex Feature";
        break;
      case DREAM3D::AttributeMatrixType::EdgeFeature:
        typeString = "Edge Feature";
        break;
      case DREAM3D::AttributeMatrixType::FaceFeature:
        typeString = "Face Feature";
        break;
      case DREAM3D::AttributeMatrixType::CellFeature:
        typeString = "Cell Feature";
        break;
      case DREAM3D::AttributeMatrixType::VertexEnsemble:
        typeString = "Vertex Ensemble";
        break;
      case DREAM3D::AttributeMatrixType::EdgeEnsemble:
        typeString = "Edge Ensemble";
        break;
      case DREAM3D::AttributeMatrixType::FaceEnsemble:
        typeString = "Face Ensemble";
        break;
      case DREAM3D::AttributeMatrixType::CellEnsemble:
        typeString = "Cell Ensemble";
        break;
      case DREAM3D::AttributeMatrixType::MetaData:
        typeString = "MetaData";
        break;
      case DREAM3D::AttributeMatrixType::Generic:
        typeString = "Generic";
      default:
        typeString = "Unknown";
        break;
    }

    ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Type:</th><td>" << typeString << "</td></tr>";
    QString tupleStr = "(";
    for(int i = 0; i < m_TupleDims.size(); i++)
    {
      tupleStr = tupleStr + QString::number(m_TupleDims[i]);
      if(i < m_TupleDims.size() - 1)
      {
        tupleStr = tupleStr + QString(", ");
      }
    }
    tupleStr = tupleStr + ")";
    ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Tuple Dimensions:</th><td>" << tupleStr << "</td></tr>";

    ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Attribute Array Count:</th><td>" << getNumAttributeArrays() << "</td></tr>";
    ss << "</tbody></table>\n";
    ss << "<br/>";
    ss << "</body></html>";
  }
  else
  {

  }
  return info;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AttributeMatrix::writeXdmfAttributeDataHelper(int numComp, const QString& attrType,
                                                      const QString& dataContainerName,
                                                      IDataArray::Pointer array,
                                                      const QString& centering,
                                                      int precision, const QString& xdmfTypeName, const QString& hdfFileName, const uint8_t gridType)
{
  QString buf;
  QTextStream out(&buf);

  QString tupleStr;
  for(int i = m_TupleDims.size() - 1; i >= 0 ; i--)
  {
    tupleStr = tupleStr + QString::number(m_TupleDims[i]) + QString(" ");
  }
  QString dimStr = tupleStr + QString::number(array->getNumberOfComponents());
  QString dimStrHalf = tupleStr + QString::number(array->getNumberOfComponents() / 2);

  if(numComp == 1 || numComp == 3 || numComp == 9)
    //  if(numComp == 1 || numComp == 3 || numComp == 6 || numComp == 9)
  {
    out << "    <Attribute Name=\"" << array->getName() << "\" ";
    out << "AttributeType=\"" << attrType << "\" ";
    out << "Center=\"" << centering << "\">" << "\n";
    // Open the <DataItem> Tag
    out << "      <DataItem Format=\"HDF\" Dimensions=\"" << dimStr <<  "\" ";
    out << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << "\n";
    out << "        " << hdfFileName << ":/DataContainers/" << dataContainerName << "/" << getName() << "/" << array->getName() << "\n";
    out << "      </DataItem>" << "\n";
    out << "    </Attribute>" << "\n";
  }
  else if(numComp == 2 || numComp == 6)
    //  else if(numComp == 2)
  {
    //First Slab
    out << "    <Attribute Name=\"" << array->getName() << " (Feature 0)\" ";
    out << "AttributeType=\"" << attrType << "\" ";

    out << "Center=\"" << centering << "\">" << "\n";
    // Open the <DataItem> Tag
    out << "      <DataItem ItemType=\"HyperSlab\" Dimensions=\"" << dimStrHalf <<  "\" ";
    out << "Type=\"HyperSlab\" " << "Name=\"" << array->getName() << " (Feature 0)\" >" << "\n";
    out << "        <DataItem Dimensions=\"3 2\" " << "Format=\"XML\" >" << "\n";
    out << "          0        0" << "\n";
    out << "          1        1" << "\n";
    out << "          " << dimStrHalf << " </DataItem>" << "\n";
    out << "\n";
    out << "        <DataItem Format=\"HDF\" Dimensions=\"" << dimStr << "\" " << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << "\n";

    out << "        " << hdfFileName << ":/DataContainers/" << dataContainerName << "/" << getName() << "/" << array->getName() << "\n";
    out << "        </DataItem>" << "\n";
    out << "      </DataItem>" << "\n";
    out << "    </Attribute>" << "\n" << "\n";

    //Second Slab
    out << "    <Attribute Name=\"" << array->getName() << " (Feature 1)\" ";
    out << "AttributeType=\"" << attrType << "\" ";

    out << "Center=\"" << centering << "\">" << "\n";
    // Open the <DataItem> Tag
    out << "      <DataItem ItemType=\"HyperSlab\" Dimensions=\"" << dimStrHalf <<  "\" ";
    out << "Type=\"HyperSlab\" " << "Name=\"" << array->getName() << " (Feature 1)\" >" << "\n";
    out << "        <DataItem Dimensions=\"3 2\" " << "Format=\"XML\" >" << "\n";
    out << "          0        " << (array->getNumberOfComponents() / 2) << "\n";
    out << "          1        1" << "\n";
    out << "          " << dimStrHalf << " </DataItem>" << "\n";
    out << "\n";
    out << "        <DataItem Format=\"HDF\" Dimensions=\"" << dimStr << "\" " << "NumberType=\"" << xdmfTypeName << "\" " << "Precision=\"" << precision << "\" >" << "\n";
    out << "        " << hdfFileName << ":/DataContainers/" << dataContainerName << "/" << getName() << "/" << array->getName() << "\n";
    out << "        </DataItem>" << "\n";
    out << "      </DataItem>" << "\n";
    out << "    </Attribute>" << "\n";
  }
  return buf;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString AttributeMatrix::writeXdmfAttributeData(IDataArray::Pointer array, const QString& centering, const QString& dataContainerName, const QString& hdfFileName, const uint8_t gridType)
{
  QString xdmfText;
  QTextStream out(&xdmfText);

  int precision = 0;
  QString xdmfTypeName;
  array->getXdmfTypeAndSize(xdmfTypeName, precision);
  if (0 == precision)
  {
    out << "<!-- " << array->getName() << " has unkown type or unsupported type or precision for XDMF to understand" << " -->" << "\n";
    return xdmfText;
  }
  int numComp = array->getNumberOfComponents();
  QString attrType = "";
  if(numComp == 1)
  {
    attrType = "Scalar";
  }
  //we are assuming a component of 2 is for scalars on either side of a single object (ie faceIds)
  if(numComp == 2)
  {
    attrType = "Scalar";
  }
  if(numComp == 3)
  {
    attrType = "Vector";
  }
  if(numComp == 6)
  {
    attrType = "Vector";
  }
  //  if(numComp == 6) { attrType = "Tensor6"; }
  if(numComp == 9)
  {
    attrType = "Tensor";
  }
  QString block = writeXdmfAttributeDataHelper(numComp, attrType, dataContainerName, array, centering, precision, xdmfTypeName, hdfFileName, gridType);

  out << block;

  return xdmfText;
}
