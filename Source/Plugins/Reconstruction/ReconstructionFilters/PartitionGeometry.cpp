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

  parameters.push_back(SIMPL_NEW_FLOAT_VEC2_FP("Partition X Bounds", XBounds, FilterParameter::Category::Parameter, PartitionGeometry));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC2_FP("Partition Y Bounds", YBounds, FilterParameter::Category::Parameter, PartitionGeometry));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC2_FP("Partition Z Bounds", ZBounds, FilterParameter::Category::Parameter, PartitionGeometry));
  parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Partition Grid Dimensions", GridDimensions, FilterParameter::Category::Parameter, PartitionGeometry));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Out-Of-Bounds Default Value", DefaultValue, FilterParameter::Category::Parameter, PartitionGeometry));

  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Origin", Origin, FilterParameter::Category::Parameter, PartitionGeometry));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Spacing", Spacing, FilterParameter::Category::Parameter, PartitionGeometry));

  {
    AttributeMatrixSelectionFilterParameter::RequirementType req;
    req.amTypes = {AttributeMatrix::Type::Cell};
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Cell Attribute Matrix", CellAttributeMatrixPath, FilterParameter::Category::RequiredArray, PartitionGeometry, req));
  }

  parameters.push_back(
      SIMPL_NEW_DA_WITH_LINKED_AM_FP("Partition Ids", PartitionIdsArrayName, CellAttributeMatrixPath, CellAttributeMatrixPath, FilterParameter::Category::CreatedArray, PartitionGeometry));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PartitionGeometry::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  if(m_GridDimensions.getX() <= 0)
  {
    QString ss = QObject::tr("Partition Grid Dimensions: The 1st grid dimension must be greater than 0.");
    setErrorCondition(-3000, ss);
    return;
  }

  if(m_GridDimensions.getY() <= 0)
  {
    QString ss = QObject::tr("Partition Grid Dimensions: The 2nd grid dimension must be greater than 0.");
    setErrorCondition(-3001, ss);
    return;
  }

  if(m_GridDimensions.getZ() <= 0)
  {
    QString ss = QObject::tr("Partition Grid Dimensions: The 3rd grid dimension must be greater than 0.");
    setErrorCondition(-3002, ss);
    return;
  }

  if(m_XBounds.getX() < 0 || m_XBounds.getY() < 0)
  {
    QString ss = QObject::tr("Partition X Bounds: The values cannot be negative.");
    setErrorCondition(-3003, ss);
    return;
  }
  if(m_XBounds.getX() > m_XBounds.getY())
  {
    QString ss = QObject::tr("Partition X Bounds: The minimum value must be smaller than the maximum value.");
    setErrorCondition(-3004, ss);
    return;
  }
  if(m_YBounds.getX() < 0 || m_YBounds.getY() < 0)
  {
    QString ss = QObject::tr("Partition Y Bounds: The values cannot be negative.");
    setErrorCondition(-3005, ss);
    return;
  }
  if(m_YBounds.getX() > m_YBounds.getY())
  {
    QString ss = QObject::tr("Partition Y Bounds: The minimum value must be smaller than the maximum value.");
    setErrorCondition(-3006, ss);
    return;
  }
  if(m_ZBounds.getX() < 0 || m_ZBounds.getY() < 0)
  {
    QString ss = QObject::tr("Partition Z Bounds: The values cannot be negative.");
    setErrorCondition(-3007, ss);
    return;
  }
  if(m_ZBounds.getX() > m_ZBounds.getY())
  {
    QString ss = QObject::tr("Partition Z Bounds: The minimum value must be smaller than the maximum value.");
    setErrorCondition(-3008, ss);
    return;
  }

  getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, m_CellAttributeMatrixPath, 1);

  DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(m_CellAttributeMatrixPath.getDataContainerName());
  IGeometry::Pointer iGeometry = dc->getGeometry();
  if(iGeometry == IGeometry::NullPointer())
  {
    QString ss = QObject::tr("The data container '%1', containing the cell attribute matrix '%2', must have a geometry.")
                     .arg(m_CellAttributeMatrixPath.getDataContainerName(), m_CellAttributeMatrixPath.getAttributeMatrixName());
    setErrorCondition(-3009, ss);
    return;
  }

  std::vector<size_t> cDims(1, 1);
  DataArrayPath tempPath(m_CellAttributeMatrixPath.getDataContainerName(), m_CellAttributeMatrixPath.getAttributeMatrixName(), getPartitionIdsArrayName());
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
  partitionImageGeom->setOrigin(m_Origin);
  partitionImageGeom->setSpacing(m_Spacing);
  partitionImageGeom->setDimensions(m_GridDimensions.convertType<size_t>());

  DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(m_CellAttributeMatrixPath.getDataContainerName());
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
  size_t numOfVertices = vertexList.getSize();
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
void PartitionGeometry::setCellAttributeMatrixPath(const DataArrayPath& value)
{
  m_CellAttributeMatrixPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath PartitionGeometry::getCellAttributeMatrixPath() const
{
  return m_CellAttributeMatrixPath;
}

// -----------------------------------------------------------------------------
void PartitionGeometry::setOrigin(const FloatVec3Type& value)
{
  m_Origin = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type PartitionGeometry::getOrigin() const
{
  return m_Origin;
}

// -----------------------------------------------------------------------------
void PartitionGeometry::setSpacing(const FloatVec3Type& value)
{
  m_Spacing = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type PartitionGeometry::getSpacing() const
{
  return m_Spacing;
}

// -----------------------------------------------------------------------------
void PartitionGeometry::setXBounds(const FloatVec2Type& value)
{
  m_XBounds = value;
}

// -----------------------------------------------------------------------------
FloatVec2Type PartitionGeometry::getXBounds() const
{
  return m_XBounds;
}

// -----------------------------------------------------------------------------
void PartitionGeometry::setYBounds(const FloatVec2Type& value)
{
  m_YBounds = value;
}

// -----------------------------------------------------------------------------
FloatVec2Type PartitionGeometry::getYBounds() const
{
  return m_YBounds;
}

// -----------------------------------------------------------------------------
void PartitionGeometry::setZBounds(const FloatVec2Type& value)
{
  m_ZBounds = value;
}

// -----------------------------------------------------------------------------
FloatVec2Type PartitionGeometry::getZBounds() const
{
  return m_ZBounds;
}

// -----------------------------------------------------------------------------
void PartitionGeometry::setGridDimensions(const IntVec3Type& value)
{
  m_GridDimensions = value;
}

// -----------------------------------------------------------------------------
IntVec3Type PartitionGeometry::getGridDimensions() const
{
  return m_GridDimensions;
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
void PartitionGeometry::setPartitionIdsArrayName(const QString& value)
{
  m_PartitionIdsArrayName = value;
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getPartitionIdsArrayName() const
{
  return m_PartitionIdsArrayName;
}
