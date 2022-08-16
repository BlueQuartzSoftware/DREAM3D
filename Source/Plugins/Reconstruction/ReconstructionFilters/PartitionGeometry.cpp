/*
 * Your License or Copyright can go here
 */

#include "PartitionGeometry.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/HexahedralGeom.h"
#include "SIMPLib/Geometry/QuadGeom.h"
#include "SIMPLib/Geometry/RectGridGeom.h"
#include "SIMPLib/Geometry/TetrahedralGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"

#include "Reconstruction/ReconstructionConstants.h"
#include "Reconstruction/ReconstructionVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PartitionGeometry::PartitionGeometry()
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PartitionGeometry::~PartitionGeometry() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PartitionGeometry::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PartitionGeometry::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Partition Dimensions", PartitionDimensions, FilterParameter::Category::Parameter, PartitionGeometry));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Partition Origin", PartitionOrigin, FilterParameter::Category::Parameter, PartitionGeometry));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Partition Spacing", PartitionSpacing, FilterParameter::Category::Parameter, PartitionGeometry));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Out-Of-Bounds Default Value", DefaultValue, FilterParameter::Category::Parameter, PartitionGeometry));

  PreflightUpdatedValueFilterParameter::Pointer param = SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("Geometry Size in Length Units", GeometryString, FilterParameter::Category::Parameter, PartitionGeometry);
  param->setReadOnly(true);
  parameters.push_back(param);

  param = SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("Partitioning Space in Length Units", PartitionSpaceString, FilterParameter::Category::Parameter, PartitionGeometry);
  param->setReadOnly(true);
  parameters.push_back(param);

  {
    AttributeMatrixSelectionFilterParameter::RequirementType req;
    req.amTypes = {AttributeMatrix::Type::Cell, AttributeMatrix::Type::Vertex, AttributeMatrix::Type::Face, AttributeMatrix::Type::Edge};
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Attribute Matrix", AttributeMatrixPath, FilterParameter::Category::RequiredArray, PartitionGeometry, req));
  }

  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Partition Ids", PartitionIdsArrayName, AttributeMatrixPath, AttributeMatrixPath, FilterParameter::Category::CreatedArray, PartitionGeometry));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PartitionGeometry::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  if(m_PartitionDimensions.getX() < 0)
  {
    QString ss = QObject::tr("Partition Dimensions: The X value cannot be negative.");
    setErrorCondition(-3000, ss);
    return;
  }
  if(m_PartitionDimensions.getY() < 0)
  {
    QString ss = QObject::tr("Partition Dimensions: The Y value cannot be negative.");
    setErrorCondition(-3001, ss);
    return;
  }
  if(m_PartitionDimensions.getZ() < 0)
  {
    QString ss = QObject::tr("Partition Dimensions: The Z value cannot be negative.");
    setErrorCondition(-3002, ss);
    return;
  }

  if(m_PartitionSpacing.getX() < 0)
  {
    QString ss = QObject::tr("Partition Spacing: The X value cannot be negative.");
    setErrorCondition(-3003, ss);
    return;
  }
  if(m_PartitionSpacing.getY() < 0)
  {
    QString ss = QObject::tr("Partition Spacing: The Y value cannot be negative.");
    setErrorCondition(-3004, ss);
    return;
  }
  if(m_PartitionSpacing.getZ() < 0)
  {
    QString ss = QObject::tr("Partition Spacing: The Z value cannot be negative.");
    setErrorCondition(-3005, ss);
    return;
  }

  if(m_PartitionDimensions.getX() <= 0)
  {
    QString ss = QObject::tr("Partition Dimensions: The 1st dimension must be greater than 0.");
    setErrorCondition(-3006, ss);
    return;
  }

  if(m_PartitionDimensions.getY() <= 0)
  {
    QString ss = QObject::tr("Partition Dimensions: The 2nd dimension must be greater than 0.");
    setErrorCondition(-3007, ss);
    return;
  }

  if(m_PartitionDimensions.getZ() <= 0)
  {
    QString ss = QObject::tr("Partition Dimensions: The 3rd dimension must be greater than 0.");
    setErrorCondition(-3008, ss);
    return;
  }

  getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, m_AttributeMatrixPath, 1);
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(m_AttributeMatrixPath.getDataContainerName());
  IGeometry::Pointer iGeometry = dc->getGeometry();
  if(iGeometry == IGeometry::NullPointer())
  {
    QString ss = QObject::tr("The data container '%1', containing the cell attribute matrix '%2', must have a geometry.")
                     .arg(m_AttributeMatrixPath.getDataContainerName(), m_AttributeMatrixPath.getAttributeMatrixName());
    setErrorCondition(-3009, ss);
    return;
  }

  std::vector<size_t> cDims(1, 1);
  DataArrayPath tempPath(m_AttributeMatrixPath.getDataContainerName(), m_AttributeMatrixPath.getAttributeMatrixName(), getPartitionIdsArrayName());
  m_PartitionIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<Int32ArrayType>(this, tempPath, 0, cDims);
  if(nullptr != m_PartitionIdsPtr.lock())
  {
    m_PartitionIds = m_PartitionIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PartitionGeometry::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  Int32ArrayType::Pointer partitionIdsPtr = m_PartitionIdsPtr.lock();

  ImageGeom::Pointer partitionImageGeom = ImageGeom::CreateGeometry("partitionImageGeom");
  partitionImageGeom->setOrigin(m_PartitionOrigin);
  partitionImageGeom->setSpacing(m_PartitionSpacing);
  partitionImageGeom->setDimensions(m_PartitionDimensions.convertType<size_t>());

  DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(m_AttributeMatrixPath.getDataContainerName());
  IGeometry::Pointer iGeometry = dc->getGeometry();
  switch(iGeometry->getGeometryType())
  {
  case IGeometry::Type::Image:
  {
    ImageGeom::Pointer geometry = dc->getGeometryAs<ImageGeom>();
    partitionCellBasedGeometry(*geometry, *partitionImageGeom, *partitionIdsPtr);
    break;
  }
  case IGeometry::Type::RectGrid:
  {
    RectGridGeom::Pointer geometry = dc->getGeometryAs<RectGridGeom>();
    partitionCellBasedGeometry(*geometry, *partitionImageGeom, *partitionIdsPtr);
    break;
  }
  case IGeometry::Type::Vertex:
  {
    VertexGeom::Pointer geometry = dc->getGeometryAs<VertexGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    partitionNodeBasedGeometry(*vertexList, *partitionImageGeom, *partitionIdsPtr);
    break;
  }
  case IGeometry::Type::Edge:
  {
    EdgeGeom::Pointer geometry = dc->getGeometryAs<EdgeGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    partitionNodeBasedGeometry(*vertexList, *partitionImageGeom, *partitionIdsPtr);
    break;
  }
  case IGeometry::Type::Triangle:
  {
    TriangleGeom::Pointer geometry = dc->getGeometryAs<TriangleGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    partitionNodeBasedGeometry(*vertexList, *partitionImageGeom, *partitionIdsPtr);
    break;
  }
  case IGeometry::Type::Quad:
  {
    QuadGeom::Pointer geometry = dc->getGeometryAs<QuadGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    partitionNodeBasedGeometry(*vertexList, *partitionImageGeom, *partitionIdsPtr);
    break;
  }
  case IGeometry::Type::Tetrahedral:
  {
    TetrahedralGeom::Pointer geometry = dc->getGeometryAs<TetrahedralGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    partitionNodeBasedGeometry(*vertexList, *partitionImageGeom, *partitionIdsPtr);
    break;
  }
  case IGeometry::Type::Hexahedral:
  {
    HexahedralGeom::Pointer geometry = dc->getGeometryAs<HexahedralGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    partitionNodeBasedGeometry(*vertexList, *partitionImageGeom, *partitionIdsPtr);
    break;
  }
  default:
  {
    QString ss = QObject::tr("Unable to partition geometry - Unknown geometry type detected.");
    setErrorCondition(-3010, ss);
    return;
  }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PartitionGeometry::partitionCellBasedGeometry(const IGeometryGrid& geometry, const ImageGeom& partitionImageGeom, Int32ArrayType& partitionIds)
{
  SizeVec3Type dims = geometry.getDimensions();

  for(size_t z = 0; z < dims[2]; z++)
  {
    for(size_t y = 0; y < dims[1]; y++)
    {
      for(size_t x = 0; x < dims[0]; x++)
      {
        size_t index = (z * dims[1] * dims[0]) + (y * dims[0]) + x;

        std::array<float, 3> coord;
        geometry.getCoords(x, y, z, coord.data());
        auto partitionIndexResult = partitionImageGeom.getIndex(coord[0], coord[1], coord[2]);
        if(partitionIndexResult.has_value())
        {
          size_t value = partitionIndexResult.value();
          partitionIds.setValue(index, value);
        }
        else
        {
          partitionIds.setValue(index, m_DefaultValue);
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PartitionGeometry::partitionNodeBasedGeometry(const SharedVertexList& vertexList, const ImageGeom& partitionImageGeom, Int32ArrayType& partitionIds)
{
  size_t numOfVertices = vertexList.getNumberOfTuples();
  float* vertexArray = vertexList.getPointer(0);
  for(size_t idx = 0; idx < numOfVertices; idx++)
  {
    auto partitionIndexResult = partitionImageGeom.getIndex(vertexArray[idx * 3], vertexArray[idx * 3 + 1], vertexArray[idx * 3 + 2]);
    if(partitionIndexResult.has_value())
    {
      partitionIds.setValue(idx, partitionIndexResult.value());
    }
    else
    {
      partitionIds.setValue(idx, m_DefaultValue);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer PartitionGeometry::newFilterInstance(bool copyFilterParameters) const
{
  PartitionGeometry::Pointer filter = PartitionGeometry::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PartitionGeometry::getCompiledLibraryName() const
{
  return ReconstructionConstants::ReconstructionBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PartitionGeometry::getBrandingString() const
{
  return "Reconstruction";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PartitionGeometry::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Reconstruction::Version::Major() << "." << Reconstruction::Version::Minor() << "." << Reconstruction::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PartitionGeometry::getGroupName() const
{
  return SIMPL::FilterGroups::ReconstructionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PartitionGeometry::getSubGroupName() const
{
  return "Reconstruction";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PartitionGeometry::getHumanLabel() const
{
  return "Partition Geometry";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid PartitionGeometry::getUuid() const
{
  return QUuid("{aad29ebc-bf1c-5dd3-ad25-0f7f8907a02d}");
}

// -----------------------------------------------------------------------------
PartitionGeometry::Pointer PartitionGeometry::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<PartitionGeometry> PartitionGeometry::New()
{
  struct make_shared_enabler : public PartitionGeometry
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getNameOfClass() const
{
  return QString("PartitionGeometry");
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::ClassName()
{
  return QString("PartitionGeometry");
}

// -----------------------------------------------------------------------------
void PartitionGeometry::setAttributeMatrixPath(const DataArrayPath& value)
{
  m_AttributeMatrixPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath PartitionGeometry::getAttributeMatrixPath() const
{
  return m_AttributeMatrixPath;
}

// -----------------------------------------------------------------------------
void PartitionGeometry::setPartitionOrigin(const FloatVec3Type& value)
{
  m_PartitionOrigin = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type PartitionGeometry::getPartitionOrigin() const
{
  return m_PartitionOrigin;
}

// -----------------------------------------------------------------------------
void PartitionGeometry::setPartitionSpacing(const FloatVec3Type& value)
{
  m_PartitionSpacing = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type PartitionGeometry::getPartitionSpacing() const
{
  return m_PartitionSpacing;
}

// -----------------------------------------------------------------------------
void PartitionGeometry::setPartitionDimensions(const IntVec3Type& value)
{
  m_PartitionDimensions = value;
}

// -----------------------------------------------------------------------------
IntVec3Type PartitionGeometry::getPartitionDimensions() const
{
  return m_PartitionDimensions;
}

// -----------------------------------------------------------------------------
void PartitionGeometry::setDefaultValue(const int& value)
{
  m_DefaultValue = value;
}

// -----------------------------------------------------------------------------
int PartitionGeometry::getDefaultValue() const
{
  return m_DefaultValue;
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getGeometryString() const
{
  DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(m_AttributeMatrixPath.getDataContainerName());
  if(dc == DataContainer::NullPointer())
  {
    return {};
  }

  IGeometry::Pointer iGeometry = dc->getGeometry();
  if(iGeometry == IGeometry::NullPointer())
  {
    return {};
  }

  switch(iGeometry->getGeometryType())
  {
  case IGeometry::Type::Image:
  {
    ImageGeom::Pointer geometry = dc->getGeometryAs<ImageGeom>();
    return getImageGeometrySpaceDescription(*geometry);
  }
  case IGeometry::Type::RectGrid:
    return getRectGridGeometrySpaceDescription();
  case IGeometry::Type::Vertex:
    return getVertexGeometrySpaceDescription();
  case IGeometry::Type::Edge:
    return getEdgeGeometrySpaceDescription();
  case IGeometry::Type::Triangle:
    return getTriangleGeometrySpaceDescription();
  case IGeometry::Type::Quad:
    return getQuadGeometrySpaceDescription();
  case IGeometry::Type::Tetrahedral:
    return getTetrahedralGeometrySpaceDescription();
  case IGeometry::Type::Hexahedral:
    return getHexahedralGeometrySpaceDescription();
  default:
    return getUnknownGeometrySpaceDescription();
  }
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getPartitionSpaceString() const
{
  float xRangeMax = (m_PartitionOrigin[0] + (m_PartitionDimensions[0] * m_PartitionSpacing[0]));
  float xDelta = m_PartitionDimensions[0] * m_PartitionSpacing[0];
  float yRangeMax = (m_PartitionOrigin[1] + (m_PartitionDimensions[1] * m_PartitionSpacing[1]));
  float yDelta = m_PartitionDimensions[1] * m_PartitionSpacing[1];
  float zRangeMax = (m_PartitionOrigin[2] + (m_PartitionDimensions[2] * m_PartitionSpacing[2]));
  float zDelta = m_PartitionDimensions[2] * m_PartitionSpacing[2];

  QString desc =
      QObject::tr("X Range: %1 to %2 (Delta: %3) %4 Partitions (Length Per Partition: %5)\n")
          .arg(QString::number(m_PartitionOrigin[0]), QString::number(xRangeMax), QString::number(xDelta), QString::number(m_PartitionDimensions[0]), QString::number(m_PartitionSpacing[0]));
  desc.append(QObject::tr("Y Range: %1 to %2 (Delta: %3) %4 Partitions (Length Per Partition: %5)\n")
                  .arg(QString::number(m_PartitionOrigin[1]), QString::number(yRangeMax), QString::number(yDelta), QString::number(m_PartitionDimensions[1]), QString::number(m_PartitionSpacing[1])));
  desc.append(QObject::tr("Z Range: %1 to %2 (Delta: %3) %4 Partitions (Length Per Partition: %5)\n")
                  .arg(QString::number(m_PartitionOrigin[2]), QString::number(zRangeMax), QString::number(zDelta), QString::number(m_PartitionDimensions[2]), QString::number(m_PartitionSpacing[2])));

  DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(m_AttributeMatrixPath.getDataContainerName());
  if(dc != DataContainer::NullPointer())
  {
    ImageGeom::Pointer geometry = dc->getGeometryAs<ImageGeom>();
    if(geometry != ImageGeom::NullPointer())
    {
      FloatVec3Type origin = geometry->getOrigin();
      FloatVec3Type spacing = geometry->getSpacing();
      SizeVec3Type dims = geometry->getDimensions();
      float gxRangeMax = origin[0] + (dims[0] * spacing[0]);
      float gyRangeMax = origin[1] + (dims[1] * spacing[1]);
      float gzRangeMax = origin[2] + (dims[2] * spacing[2]);

      if(origin[0] < m_PartitionOrigin[0] || origin[1] < m_PartitionOrigin[1] || origin[2] < m_PartitionOrigin[2] || gxRangeMax > xRangeMax || gyRangeMax > yRangeMax || gzRangeMax > zRangeMax)
      {
        desc.append("Geometry size DOES NOT fit within the partitioning space!");
      }
      else
      {
        desc.append("Geometry size fits within partitioning space.");
      }
    }
  }

  return desc;
}

// -----------------------------------------------------------------------------
void PartitionGeometry::setPartitionIdsArrayName(const QString& value)
{
  m_PartitionIdsArrayName = value;
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getPartitionIdsArrayName() const
{
  return m_PartitionIdsArrayName;
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getImageGeometrySpaceDescription(const ImageGeom& geometry) const
{
  FloatVec3Type origin = geometry.getOrigin();
  FloatVec3Type spacing = geometry.getSpacing();
  SizeVec3Type dims = geometry.getDimensions();
  float xRangeMax = origin[0] + (dims[0] * spacing[0]);
  float xDelta = dims[0] * spacing[0];
  float yRangeMax = origin[1] + (dims[1] * spacing[1]);
  float yDelta = dims[1] * spacing[1];
  float zRangeMax = origin[2] + (dims[2] * spacing[2]);
  float zDelta = dims[2] * spacing[2];

  QString desc = QObject::tr("X Range: %1 to %2 (Delta: %3) 0-%4 Voxels\n").arg(QString::number(origin[0]), QString::number(xRangeMax), QString::number(xDelta), QString::number(dims[0] - 1));
  desc.append(QObject::tr("Y Range: %1 to %2 (Delta: %3) 0-%4 Voxels\n").arg(QString::number(origin[1]), QString::number(yRangeMax), QString::number(yDelta), QString::number(dims[1] - 1)));
  desc.append(QObject::tr("Z Range: %1 to %2 (Delta: %3) 0-%4 Voxels\n").arg(QString::number(origin[2]), QString::number(zRangeMax), QString::number(zDelta), QString::number(dims[2] - 1)));
  return desc;
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getRectGridGeometrySpaceDescription() const
{
  return "Rectilinear grid geometry space unknown during preflight.";
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getVertexGeometrySpaceDescription() const
{
  return "Vertex geometry space unknown during preflight.";
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getEdgeGeometrySpaceDescription() const
{
  return "Edge geometry space unknown during preflight.";
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getTriangleGeometrySpaceDescription() const
{
  return "Triangle geometry space unknown during preflight.";
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getQuadGeometrySpaceDescription() const
{
  return "Quad geometry space unknown during preflight.";
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getTetrahedralGeometrySpaceDescription() const
{
  return "Tetrahedral geometry space unknown during preflight.";
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getHexahedralGeometrySpaceDescription() const
{
  return "Hexahedral geometry space unknown during preflight.";
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getUnknownGeometrySpaceDescription() const
{
  return "Unknown geometry type detected.";
}
