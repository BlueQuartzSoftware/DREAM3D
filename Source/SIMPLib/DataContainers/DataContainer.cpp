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

#include "DataContainer.h"


#include "SIMPLib/DataContainers/DataContainerProxy.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/DataContainers/DataArrayPath.h"


#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/RectGridGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/QuadGeom.h"

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
DataContainer::Pointer DataContainer::createNewDataContainer(const QString& name)
{
  DataContainer::Pointer p = DataContainer::New(name);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainer::ReadDataContainerStructure(hid_t dcArrayGroupId, DataContainerArrayProxy& proxy, QString h5InternalPath)
{
  QList<QString> dataContainers;
  QH5Utilities::getGroupObjects(dcArrayGroupId, H5Utilities::H5Support_GROUP, dataContainers);
  foreach(QString dataContainerName, dataContainers)
  {
    if(__SHOW_DEBUG_MSG__)
    {
      std::cout << "Data Container:" << dataContainerName.toStdString() << std::endl;
    }
    hid_t containerGid = H5Gopen(dcArrayGroupId, dataContainerName.toLatin1().constData(), H5P_DEFAULT);
    if (containerGid < 0)
    {
      continue;
    }
    HDF5ScopedGroupSentinel sentinel(&containerGid, false);
    DataContainerProxy dcProxy(dataContainerName);
    dcProxy.name = dataContainerName;
    dcProxy.flag = Qt::Checked;

    QString h5Path = h5InternalPath + "/" + dataContainerName;
    // Read the Attribute Matricies for this Data Container
    AttributeMatrix::ReadAttributeMatrixStructure(containerGid, dcProxy, h5Path);

    // Insert the DataContainerProxy proxy into the DataContainerArrayProxy
    proxy.dataContainers.insert(dcProxy.name, dcProxy);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainer::setName(const QString& name)
{
  m_Name = name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataContainer::getName()
{
  return m_Name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainer::setGeometry(IGeometry::Pointer geometry)
{
  m_Geometry = geometry;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
IGeometry::Pointer DataContainer::getGeometry()
{
  return m_Geometry;
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
    qDebug() << "Adding Attribute Matrix with different array name than key name";
    qDebug() << "Key name: " << name;
    qDebug() << "Array Name: " << data->getName();
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
  it = m_AttributeMatrices.find(name);
  if ( it == m_AttributeMatrices.end() )
  {
    return AttributeMatrix::NullPointer();
  }
  return it.value();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::Pointer DataContainer::getAttributeMatrix(const DataArrayPath& path)
{
  QMap<QString, AttributeMatrix::Pointer>::iterator it;
  it = m_AttributeMatrices.find(path.getAttributeMatrixName());
  if (it == m_AttributeMatrices.end())
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
    // DO NOT return a NullPointer for any reason other than "Attribute Matrix was not found"
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
QList<QString> DataContainer::getAttributeMatrixNames()
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
    if(err < 0)
    {
      return err;
    }
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
    if(iter.value().flag == Qt::Unchecked)
    {
      continue;
    }
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
  DataContainer::Pointer dcCopy = DataContainer::New(getName());
  dcCopy->setName(getName());

  if (m_Geometry.get() != NULL)
  {
    IGeometry::Pointer geomCopy = m_Geometry->deepCopy();
    dcCopy->setGeometry(geomCopy);
  }

  for (AttributeMatrixMap_t::iterator iter = getAttributeMatrices().begin(); iter != getAttributeMatrices().end(); ++iter)
  {
    AttributeMatrix::Pointer attrMat = (*iter)->deepCopy();
    dcCopy->addAttributeMatrix(attrMat->getName(), attrMat);
  }

  return dcCopy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainer::writeMeshToHDF5(hid_t dcGid, bool writeXdmf)
{
  int err;
  hid_t geometryId;
  err = QH5Utilities::createGroupsFromPath(DREAM3D::Geometry::Geometry, dcGid);
  if (err < 0)
  {
    return err;
  }
  geometryId = H5Gopen(dcGid, DREAM3D::Geometry::Geometry.toLatin1().data(), H5P_DEFAULT);
  if (geometryId < 0)
  {
    return -1;
  }
  HDF5ScopedGroupSentinel gSentinel(&geometryId, false);

  if (NULL == m_Geometry.get())
  {
    err = QH5Lite::writeScalarAttribute(dcGid, DREAM3D::Geometry::Geometry, DREAM3D::Geometry::GeometryType, DREAM3D::GeometryType::UnknownGeometry);
    if (err < 0)
    {
      return err;
    }
    err = QH5Lite::writeStringAttribute(dcGid, DREAM3D::Geometry::Geometry, DREAM3D::Geometry::GeometryTypeName, DREAM3D::Geometry::UnknownGeometry);
    if (err < 0)
    {
      return err;
    }
  }
  else
  {
    err = QH5Lite::writeScalarAttribute(dcGid, DREAM3D::Geometry::Geometry, DREAM3D::Geometry::GeometryType, m_Geometry->getGeometryType());
    if (err < 0)
    {
      return err;
    }
    err = QH5Lite::writeStringAttribute(dcGid, DREAM3D::Geometry::Geometry, DREAM3D::Geometry::GeometryTypeName, m_Geometry->getGeometryTypeAsString());
    if (err < 0)
    {
      return err;
    }
    err = QH5Lite::writeStringAttribute(dcGid, DREAM3D::Geometry::Geometry, DREAM3D::Geometry::GeometryName, m_Geometry->getName());
    if (err < 0)
    {
      return err;
    }
    err = QH5Lite::writeScalarAttribute(dcGid, DREAM3D::Geometry::Geometry, DREAM3D::Geometry::UnitDimensionality, m_Geometry->getUnitDimensionality());
    if (err < 0)
    {
      return err;
    }
    err = QH5Lite::writeScalarAttribute(dcGid, DREAM3D::Geometry::Geometry, DREAM3D::Geometry::SpatialDimensionality, m_Geometry->getSpatialDimensionality());
    if (err < 0)
    {
      return err;
    }
    err = m_Geometry->writeGeometryToHDF5(geometryId, writeXdmf);
    if (err < 0)
    {
      return err;
    }
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainer::writeXdmf(QTextStream& out, QString hdfFileName)
{
  if (NULL == m_Geometry.get())
  {
    return -1;
  }

  m_Geometry->writeXdmf(out, getName(), hdfFileName);
  unsigned int geomType = m_Geometry->getGeometryType();

  // Get all of our AttributeMatrices
  AttributeMatrixMap_t amMap = getAttributeMatrices();
  // Loop over each AttributeMatrix and write the meta data to the Xdmf file
  QString xdmfCenter = "";
  for(QMap<QString, AttributeMatrix::Pointer>::iterator iter = amMap.begin(); iter != amMap.end(); ++iter)
  {
    xdmfCenter = "";
    AttributeMatrix::Pointer attrMat = iter.value();
    uint32_t amType = attrMat->getType();
    switch(geomType)
    {
      case DREAM3D::GeometryType::VertexGeometry:
        switch(amType)
        {
          //FIXME: There are more AttributeMatrix Types that should be implemented
          case DREAM3D::AttributeMatrixType::Vertex:
            xdmfCenter = DREAM3D::XdmfCenterType::Node;
            break;
          default:
            break;
        }
      case DREAM3D::GeometryType::EdgeGeometry:
        switch(amType)
        {
          //FIXME: There are more AttributeMatrix Types that should be implemented
          case DREAM3D::AttributeMatrixType::Vertex:
            xdmfCenter = DREAM3D::XdmfCenterType::Node;
            break;
          case DREAM3D::AttributeMatrixType::Edge:
            xdmfCenter = DREAM3D::XdmfCenterType::Cell;
            break;
          default:
            break;
        }
      case DREAM3D::GeometryType::TriangleGeometry:
        switch(amType)
        {
          //FIXME: There are more AttributeMatrix Types that should be implemented
          case DREAM3D::AttributeMatrixType::Vertex:
            xdmfCenter = DREAM3D::XdmfCenterType::Node;
            break;
          case DREAM3D::AttributeMatrixType::Edge:
            xdmfCenter = DREAM3D::XdmfCenterType::Cell;
            break;
          case DREAM3D::AttributeMatrixType::Face:
            xdmfCenter = DREAM3D::XdmfCenterType::Cell;
            break;
          case DREAM3D::AttributeMatrixType::Cell:
            xdmfCenter = DREAM3D::XdmfCenterType::Cell;
            break;
          default:
            break;
        }
      case DREAM3D::GeometryType::QuadGeometry:
        switch(amType)
        {
          //FIXME: There are more AttributeMatrix Types that should be implemented
          case DREAM3D::AttributeMatrixType::Vertex:
            xdmfCenter = DREAM3D::XdmfCenterType::Node;
            break;
          case DREAM3D::AttributeMatrixType::Edge:
            xdmfCenter = DREAM3D::XdmfCenterType::Cell;
            break;
          case DREAM3D::AttributeMatrixType::Face:
            xdmfCenter = DREAM3D::XdmfCenterType::Cell;
            break;
          case DREAM3D::AttributeMatrixType::Cell:
            xdmfCenter = DREAM3D::XdmfCenterType::Cell;
            break;
          default:
            break;
        }
      case DREAM3D::GeometryType::ImageGeometry:
        switch(amType)
        {
          //FIXME: There are more AttributeMatrix Types that should be implemented
          case DREAM3D::AttributeMatrixType::Vertex:
            xdmfCenter = DREAM3D::XdmfCenterType::Node;
            break;
          case DREAM3D::AttributeMatrixType::Edge:
            xdmfCenter = DREAM3D::XdmfCenterType::Cell;
            break;
          case DREAM3D::AttributeMatrixType::Face:
            xdmfCenter = DREAM3D::XdmfCenterType::Cell;
            break;
          case DREAM3D::AttributeMatrixType::Cell:
            xdmfCenter = DREAM3D::XdmfCenterType::Cell;
            break;
          default:
            break;
        }
    }

    if(xdmfCenter.isEmpty() == false)
    {
      QString xdmfText = attrMat->generateXdmfText(xdmfCenter, getName(), hdfFileName, m_Geometry->getXdmfGridType());
      out << xdmfText;
    }
  }

  writeXdmfFooter(out);

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainer::writeXdmfFooter(QTextStream& xdmf)
{
  xdmf << "  </Grid>" << "\n";
  xdmf << "  <!-- *************** END OF " << getName() << " *************** -->" << "\n";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainer::readMeshDataFromHDF5(hid_t dcGid, bool preflight)
{
  herr_t err = 0;
  QString geometryTypeName = DREAM3D::Geometry::UnknownGeometry;

  err = QH5Lite::readStringAttribute(dcGid, DREAM3D::Geometry::Geometry, DREAM3D::Geometry::GeometryTypeName, geometryTypeName);
  if (err < 0)
  {
    return err;
  }

  hid_t geometryId = H5Gopen(dcGid, DREAM3D::Geometry::Geometry.toLatin1().data(), H5P_DEFAULT);
  if (geometryId < 0)
  {
    return -1;
  }
  HDF5ScopedGroupSentinel gSentinel(&geometryId, false);

  IGeometry::Pointer geomPtr = IGeometry::NullPointer();

  if (NULL == m_Geometry.get())
  {
    if (geometryTypeName.compare(DREAM3D::Geometry::ImageGeometry) == 0)
    {
      ImageGeom::Pointer image = ImageGeom::New();
      err = image->readGeometryFromHDF5(geometryId, preflight);
      err = GeometryHelpers::GeomIO::ReadMetaDataFromHDF5(dcGid, image);
      setGeometry(image);
    }
    else if (geometryTypeName.compare(DREAM3D::Geometry::RectGridGeometry) == 0)
    {
      RectGridGeom::Pointer rectGrid = RectGridGeom::New();
      err = rectGrid->readGeometryFromHDF5(geometryId, preflight);
      err = GeometryHelpers::GeomIO::ReadMetaDataFromHDF5(dcGid, rectGrid);
      setGeometry(rectGrid);
    }
    else if (geometryTypeName.compare(DREAM3D::Geometry::VertexGeometry) == 0)
    {
      VertexGeom::Pointer vertices = VertexGeom::New();
      err = vertices->readGeometryFromHDF5(geometryId, preflight);
      err = GeometryHelpers::GeomIO::ReadMetaDataFromHDF5(dcGid, vertices);
      setGeometry(vertices);
    }
    else if (geometryTypeName.compare(DREAM3D::Geometry::EdgeGeometry) == 0)
    {
      EdgeGeom::Pointer edges = EdgeGeom::New();
      err = edges->readGeometryFromHDF5(geometryId, preflight);
      err = GeometryHelpers::GeomIO::ReadMetaDataFromHDF5(dcGid, edges);
      setGeometry(edges);
    }
    else if (geometryTypeName.compare(DREAM3D::Geometry::TriangleGeometry) == 0)
    {
      TriangleGeom::Pointer triangles = TriangleGeom::New();
      err = triangles->readGeometryFromHDF5(geometryId, preflight);
      err = GeometryHelpers::GeomIO::ReadMetaDataFromHDF5(dcGid, triangles);
      setGeometry(triangles);
    }
    else if (geometryTypeName.compare(DREAM3D::Geometry::QuadGeometry) == 0)
    {
      QuadGeom::Pointer quads = QuadGeom::New();
      err = quads->readGeometryFromHDF5(geometryId, preflight);
      err = GeometryHelpers::GeomIO::ReadMetaDataFromHDF5(dcGid, quads);
      setGeometry(quads);
    }
    else if (geometryTypeName.compare(DREAM3D::Geometry::UnknownGeometry) == 0)
    {
      setGeometry(geomPtr);
    }
    else
    {
      setGeometry(geomPtr);
    }
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<DataArrayPath> DataContainer::getAllDataArrayPaths()
{

  QVector<DataArrayPath> paths;
  for(QMap<QString, AttributeMatrix::Pointer>::iterator iter = m_AttributeMatrices.begin(); iter != m_AttributeMatrices.end(); ++iter)
  {
    AttributeMatrix::Pointer am = iter.value();
    QString amName = am->getName();
    QList<QString> aaNames = am->getAttributeArrayNames();

    foreach(QString aaName, aaNames)
    {
      DataArrayPath dap(getName(), amName, aaName);
      paths.push_back(dap);
    }
  }

  return paths;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataContainer::getInfoString(DREAM3D::InfoStringFormat format)
{
  QString info;
  QTextStream ss (&info);
  if(format == DREAM3D::HtmlFormat)
  {
    ss << "<html><head></head>\n";
    ss << "<body>\n";
    ss << "<table cellpadding=\"4\" cellspacing=\"0\" border=\"0\">\n";
    ss << "<tbody>\n";
    ss << "<tr bgcolor=\"#D3D8E0\"><th colspan=2>Data Container Info</th></tr>";

    ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Name:</th><td>" << getName() << "</td></tr>";
    ss << "<tr bgcolor=\"#C3C8D0\"><th align=\"right\">Attribute Matrix Count:</th><td>" << getNumAttributeMatrices() << "</td></tr>";
    ss << "<tr><td></td><td></td></tr>";
    if(getGeometry().get() != NULL)
    {
      ss << getGeometry()->getInfoString(DREAM3D::HtmlFormat);
    }

    ss << "</tbody></table>\n";
    ss << "<br/>";
    ss << "</body></html>";
  }
  else
  {

  }
  return info;
}


