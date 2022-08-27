/*
 * Your License or Copyright can go here
 */

#include "PartitionGeometry.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/PreflightUpdatedValueFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/HexahedralGeom.h"
#include "SIMPLib/Geometry/QuadGeom.h"
#include "SIMPLib/Geometry/RectGridGeom.h"
#include "SIMPLib/Geometry/TetrahedralGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"

#include "Reconstruction/ReconstructionConstants.h"
#include "Reconstruction/ReconstructionVersion.h"

enum createdPathID : RenameDataPath::DataID_t
{
  PartitionSchemeDataContainerID = 1,
  PartitionSchemeAttributeMatrixID = 2,
  PartitionSchemeDataArrayID = 3,
  PartitionAttributeMatrixID = 4
};

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

  parameters.push_back(SIMPL_NEW_INTEGER_FP("Starting Partition ID", StartingPartitionID, FilterParameter::Category::Parameter, PartitionGeometry));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Out-Of-Bounds Default Value", OutOfBoundsValue, FilterParameter::Category::Parameter, PartitionGeometry));
  parameters.push_back(SeparatorFilterParameter::Create("Partitioning Scheme", FilterParameter::Category::Parameter));
  parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Number Of Partitions Per Axis (X, Y, Z)", NumberOfPartitionsPerAxis, FilterParameter::Category::Parameter, PartitionGeometry));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Partitioning Scheme Origin (X, Y, Z)", PartitioningSchemeOrigin, FilterParameter::Category::Parameter, PartitionGeometry));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Length Per Partition (X, Y, Z)", LengthPerPartition, FilterParameter::Category::Parameter, PartitionGeometry));

  PreflightUpdatedValueFilterParameter::Pointer param =
      SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("Partitioning Scheme Information", PartitioningSchemeInformation, FilterParameter::Category::Parameter, PartitionGeometry);
  param->setReadOnly(true);
  parameters.push_back(param);

  std::vector<QString> linkedProps = {"PSDataContainerPath", "PSAttributeMatrixName"};
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Save Partitioning Scheme", SavePartitioningScheme, FilterParameter::Category::Parameter, PartitionGeometry, linkedProps));
  parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Partitioning Scheme Data Container", PSDataContainerPath, FilterParameter::Category::Parameter, PartitionGeometry));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Partition Scheme Attribute Matrix", PSAttributeMatrixName, PSDataContainerPath, FilterParameter::Category::Parameter, PartitionGeometry));

  {
    AttributeMatrixSelectionFilterParameter::RequirementType req;
    req.amTypes = {AttributeMatrix::Type::Cell, AttributeMatrix::Type::Vertex, AttributeMatrix::Type::Face, AttributeMatrix::Type::Edge};
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Attribute Matrix", AttributeMatrixPath, FilterParameter::Category::RequiredArray, PartitionGeometry, req));
  }

  param = SIMPL_NEW_PREFLIGHTUPDATEDVALUE_FP("Input Geometry Information", InputGeometryInformation, FilterParameter::Category::RequiredArray, PartitionGeometry);
  param->setReadOnly(true);
  parameters.push_back(param);

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

  if(m_NumberOfPartitionsPerAxis.getX() < 0)
  {
    QString ss = QObject::tr("Partition Dimensions: The X value cannot be negative.");
    setErrorCondition(-3000, ss);
    return;
  }
  if(m_NumberOfPartitionsPerAxis.getY() < 0)
  {
    QString ss = QObject::tr("Partition Dimensions: The Y value cannot be negative.");
    setErrorCondition(-3001, ss);
    return;
  }
  if(m_NumberOfPartitionsPerAxis.getZ() < 0)
  {
    QString ss = QObject::tr("Partition Dimensions: The Z value cannot be negative.");
    setErrorCondition(-3002, ss);
    return;
  }

  if(m_LengthPerPartition.getX() < 0)
  {
    QString ss = QObject::tr("Partition Spacing: The X value cannot be negative.");
    setErrorCondition(-3003, ss);
    return;
  }
  if(m_LengthPerPartition.getY() < 0)
  {
    QString ss = QObject::tr("Partition Spacing: The Y value cannot be negative.");
    setErrorCondition(-3004, ss);
    return;
  }
  if(m_LengthPerPartition.getZ() < 0)
  {
    QString ss = QObject::tr("Partition Spacing: The Z value cannot be negative.");
    setErrorCondition(-3005, ss);
    return;
  }

  if(m_NumberOfPartitionsPerAxis.getX() <= 0)
  {
    QString ss = QObject::tr("Partition Dimensions: The 1st dimension must be greater than 0.");
    setErrorCondition(-3006, ss);
    return;
  }

  if(m_NumberOfPartitionsPerAxis.getY() <= 0)
  {
    QString ss = QObject::tr("Partition Dimensions: The 2nd dimension must be greater than 0.");
    setErrorCondition(-3007, ss);
    return;
  }

  if(m_NumberOfPartitionsPerAxis.getZ() <= 0)
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

  DataArrayPath tempPath;

  switch(iGeometry->getGeometryType())
  {
  case IGeometry::Type::Image:
  case IGeometry::Type::RectGrid:
  {
    tempPath = DataArrayPath(m_AttributeMatrixPath.getDataContainerName(), m_AttributeMatrixPath.getAttributeMatrixName(), getPartitionIdsArrayName());
    break;
  }
  case IGeometry::Type::Vertex:
  {
    VertexGeom::Pointer geometry = dc->getGeometryAs<VertexGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    tempPath = DataArrayPath(m_AttributeMatrixPath.getDataContainerName(), m_VertexAttrMatrixName, getPartitionIdsArrayName());
    if(dc->getAttributeMatrix(m_VertexAttrMatrixName) == AttributeMatrix::NullPointer())
    {
      dc->createNonPrereqAttributeMatrix(this, tempPath, std::vector<size_t>{vertexList->getNumberOfTuples()}, AttributeMatrix::Type::Vertex, PartitionAttributeMatrixID);
    }
    break;
  }
  case IGeometry::Type::Edge:
  {
    EdgeGeom::Pointer geometry = dc->getGeometryAs<EdgeGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    tempPath = DataArrayPath(m_AttributeMatrixPath.getDataContainerName(), m_VertexAttrMatrixName, getPartitionIdsArrayName());
    if(dc->getAttributeMatrix(m_VertexAttrMatrixName) == AttributeMatrix::NullPointer())
    {
      dc->createNonPrereqAttributeMatrix(this, tempPath, std::vector<size_t>{vertexList->getNumberOfTuples()}, AttributeMatrix::Type::Vertex, PartitionAttributeMatrixID);
    }
    break;
  }
  case IGeometry::Type::Triangle:
  {
    TriangleGeom::Pointer geometry = dc->getGeometryAs<TriangleGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    tempPath = DataArrayPath(m_AttributeMatrixPath.getDataContainerName(), m_VertexAttrMatrixName, getPartitionIdsArrayName());
    if(dc->getAttributeMatrix(m_VertexAttrMatrixName) == AttributeMatrix::NullPointer())
    {
      dc->createNonPrereqAttributeMatrix(this, tempPath, std::vector<size_t>{vertexList->getNumberOfTuples()}, AttributeMatrix::Type::Vertex, PartitionAttributeMatrixID);
    }
    break;
  }
  case IGeometry::Type::Quad:
  {
    QuadGeom::Pointer geometry = dc->getGeometryAs<QuadGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    tempPath = DataArrayPath(m_AttributeMatrixPath.getDataContainerName(), m_VertexAttrMatrixName, getPartitionIdsArrayName());
    if(dc->getAttributeMatrix(m_VertexAttrMatrixName) == AttributeMatrix::NullPointer())
    {
      dc->createNonPrereqAttributeMatrix(this, tempPath, std::vector<size_t>{vertexList->getNumberOfTuples()}, AttributeMatrix::Type::Vertex, PartitionAttributeMatrixID);
    }
    break;
  }
  case IGeometry::Type::Tetrahedral:
  {
    TetrahedralGeom::Pointer geometry = dc->getGeometryAs<TetrahedralGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    tempPath = DataArrayPath(m_AttributeMatrixPath.getDataContainerName(), m_VertexAttrMatrixName, getPartitionIdsArrayName());
    if(dc->getAttributeMatrix(m_VertexAttrMatrixName) == AttributeMatrix::NullPointer())
    {
      dc->createNonPrereqAttributeMatrix(this, tempPath, std::vector<size_t>{vertexList->getNumberOfTuples()}, AttributeMatrix::Type::Vertex, PartitionAttributeMatrixID);
    }
    break;
  }
  case IGeometry::Type::Hexahedral:
  {
    HexahedralGeom::Pointer geometry = dc->getGeometryAs<HexahedralGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    tempPath = DataArrayPath(m_AttributeMatrixPath.getDataContainerName(), m_VertexAttrMatrixName, getPartitionIdsArrayName());
    if(dc->getAttributeMatrix(m_VertexAttrMatrixName) == AttributeMatrix::NullPointer())
    {
      dc->createNonPrereqAttributeMatrix(this, tempPath, std::vector<size_t>{vertexList->getNumberOfTuples()}, AttributeMatrix::Type::Vertex, PartitionAttributeMatrixID);
    }
    break;
  }
  default:
  {
    QString ss = QObject::tr("Unable to partition geometry - Unknown geometry type detected.");
    setErrorCondition(-3010, ss);
    return;
  }
  }

  m_PartitionImageGeometry = ImageGeom::CreateGeometry(m_PSImageGeomName);
  m_PartitionImageGeometry->setOrigin(m_PartitioningSchemeOrigin);
  m_PartitionImageGeometry->setSpacing(m_LengthPerPartition);
  m_PartitionImageGeometry->setDimensions(m_NumberOfPartitionsPerAxis.convertType<size_t>());

  std::vector<size_t> cDims(1, 1);
  m_PartitionIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<Int32ArrayType>(this, tempPath, 0, cDims);
  if(nullptr != m_PartitionIdsPtr.lock())
  {
    m_PartitionIds = m_PartitionIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_SavePartitioningScheme)
  {
    DataContainer::Pointer ps_dc = getDataContainerArray()->createNonPrereqDataContainer(this, m_PSDataContainerPath, PartitionSchemeDataContainerID);
    if(getErrorCode() < 0)
    {
      return;
    }

    ps_dc->setGeometry(m_PartitionImageGeometry);

    DataArrayPath dap = m_PSDataContainerPath;
    dap.setAttributeMatrixName(m_PSAttributeMatrixName);
    AttributeMatrix::Pointer ps_am = ps_dc->createNonPrereqAttributeMatrix(this, dap, m_NumberOfPartitionsPerAxis.convertType<size_t>(), AttributeMatrix::Type::Cell, PartitionSchemeAttributeMatrixID);
    if(getErrorCode() < 0)
    {
      return;
    }

    m_PartitioningSchemeIdsPtr = ps_am->createNonPrereqArray<Int32ArrayType>(this, m_PSDataArrayName, 0, cDims, PartitionSchemeDataArrayID);
    if(getErrorCode() < 0)
    {
      return;
    }

    if(nullptr != m_PartitioningSchemeIdsPtr.lock())
    {
      m_PartitioningSchemeIds = m_PartitioningSchemeIdsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
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

  DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(m_AttributeMatrixPath.getDataContainerName());
  IGeometry::Pointer iGeometry = dc->getGeometry();
  switch(iGeometry->getGeometryType())
  {
  case IGeometry::Type::Image:
  {
    ImageGeom::Pointer geometry = dc->getGeometryAs<ImageGeom>();
    partitionCellBasedGeometry(*geometry, *m_PartitionImageGeometry, *partitionIdsPtr);
    break;
  }
  case IGeometry::Type::RectGrid:
  {
    RectGridGeom::Pointer geometry = dc->getGeometryAs<RectGridGeom>();
    partitionCellBasedGeometry(*geometry, *m_PartitionImageGeometry, *partitionIdsPtr);
    break;
  }
  case IGeometry::Type::Vertex:
  {
    VertexGeom::Pointer geometry = dc->getGeometryAs<VertexGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    partitionNodeBasedGeometry(*vertexList, *m_PartitionImageGeometry, *partitionIdsPtr);
    break;
  }
  case IGeometry::Type::Edge:
  {
    EdgeGeom::Pointer geometry = dc->getGeometryAs<EdgeGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    partitionNodeBasedGeometry(*vertexList, *m_PartitionImageGeometry, *partitionIdsPtr);
    break;
  }
  case IGeometry::Type::Triangle:
  {
    TriangleGeom::Pointer geometry = dc->getGeometryAs<TriangleGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    partitionNodeBasedGeometry(*vertexList, *m_PartitionImageGeometry, *partitionIdsPtr);
    break;
  }
  case IGeometry::Type::Quad:
  {
    QuadGeom::Pointer geometry = dc->getGeometryAs<QuadGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    partitionNodeBasedGeometry(*vertexList, *m_PartitionImageGeometry, *partitionIdsPtr);
    break;
  }
  case IGeometry::Type::Tetrahedral:
  {
    TetrahedralGeom::Pointer geometry = dc->getGeometryAs<TetrahedralGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    partitionNodeBasedGeometry(*vertexList, *m_PartitionImageGeometry, *partitionIdsPtr);
    break;
  }
  case IGeometry::Type::Hexahedral:
  {
    HexahedralGeom::Pointer geometry = dc->getGeometryAs<HexahedralGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    partitionNodeBasedGeometry(*vertexList, *m_PartitionImageGeometry, *partitionIdsPtr);
    break;
  }
  default:
  {
    QString ss = QObject::tr("Unable to partition geometry - Unknown geometry type detected.");
    setErrorCondition(-3010, ss);
    return;
  }
  }

  if(m_SavePartitioningScheme)
  {
    for(size_t i = 0; i < m_PartitionImageGeometry->getNumberOfElements(); i++)
    {
      m_PartitioningSchemeIds[i] = i + m_StartingPartitionID;
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
          partitionIds.setValue(index, value + m_StartingPartitionID);
        }
        else
        {
          partitionIds.setValue(index, m_OutOfBoundsValue);
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
      size_t value = partitionIndexResult.value();
      partitionIds.setValue(idx, value + m_StartingPartitionID);
    }
    else
    {
      partitionIds.setValue(idx, m_OutOfBoundsValue);
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
void PartitionGeometry::setPartitioningSchemeOrigin(const FloatVec3Type& value)
{
  m_PartitioningSchemeOrigin = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type PartitionGeometry::getPartitioningSchemeOrigin() const
{
  return m_PartitioningSchemeOrigin;
}

// -----------------------------------------------------------------------------
void PartitionGeometry::setLengthPerPartition(const FloatVec3Type& value)
{
  m_LengthPerPartition = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type PartitionGeometry::getLengthPerPartition() const
{
  return m_LengthPerPartition;
}

// -----------------------------------------------------------------------------
void PartitionGeometry::setNumberOfPartitionsPerAxis(const IntVec3Type& value)
{
  m_NumberOfPartitionsPerAxis = value;
}

// -----------------------------------------------------------------------------
IntVec3Type PartitionGeometry::getNumberOfPartitionsPerAxis() const
{
  return m_NumberOfPartitionsPerAxis;
}

// -----------------------------------------------------------------------------
void PartitionGeometry::setOutOfBoundsValue(const int& value)
{
  m_OutOfBoundsValue = value;
}

// -----------------------------------------------------------------------------
int PartitionGeometry::getOutOfBoundsValue() const
{
  return m_OutOfBoundsValue;
}

// -----------------------------------------------------------------------------
void PartitionGeometry::setStartingPartitionID(const int& value)
{
  m_StartingPartitionID = value;
}

// -----------------------------------------------------------------------------
int PartitionGeometry::getStartingPartitionID() const
{
  return m_StartingPartitionID;
}

// -----------------------------------------------------------------------------
void PartitionGeometry::setSavePartitioningScheme(const bool& value)
{
  m_SavePartitioningScheme = value;
}

// -----------------------------------------------------------------------------
bool PartitionGeometry::getSavePartitioningScheme() const
{
  return m_SavePartitioningScheme;
}

// -----------------------------------------------------------------------------
void PartitionGeometry::setPSDataContainerPath(const DataArrayPath& value)
{
  m_PSDataContainerPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath PartitionGeometry::getPSDataContainerPath() const
{
  return m_PSDataContainerPath;
}

// -----------------------------------------------------------------------------
void PartitionGeometry::setPSAttributeMatrixName(const QString& value)
{
  m_PSAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getPSAttributeMatrixName() const
{
  return m_PSAttributeMatrixName;
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getInputGeometryInformation() const
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
    return getInputImageGeometryInformation(*geometry);
  }
  case IGeometry::Type::RectGrid:
    return getInputRectGridGeometryInformation();
  case IGeometry::Type::Vertex:
    return getInputVertexGeometryInformation();
  case IGeometry::Type::Edge:
    return getInputEdgeGeometryInformation();
  case IGeometry::Type::Triangle:
    return getInputTriangleGeometryInformation();
  case IGeometry::Type::Quad:
    return getInputQuadGeometryInformation();
  case IGeometry::Type::Tetrahedral:
    return getInputTetrahedralGeometryInformation();
  case IGeometry::Type::Hexahedral:
    return getInputHexahedralGeometryInformation();
  default:
    return getInputUnknownGeometryInformation();
  }
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getPartitioningSchemeInformation() const
{
  float xRangeMax = (m_PartitioningSchemeOrigin[0] + (m_NumberOfPartitionsPerAxis[0] * m_LengthPerPartition[0]));
  float xDelta = m_NumberOfPartitionsPerAxis[0] * m_LengthPerPartition[0];
  float yRangeMax = (m_PartitioningSchemeOrigin[1] + (m_NumberOfPartitionsPerAxis[1] * m_LengthPerPartition[1]));
  float yDelta = m_NumberOfPartitionsPerAxis[1] * m_LengthPerPartition[1];
  float zRangeMax = (m_PartitioningSchemeOrigin[2] + (m_NumberOfPartitionsPerAxis[2] * m_LengthPerPartition[2]));
  float zDelta = m_NumberOfPartitionsPerAxis[2] * m_LengthPerPartition[2];

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

  IGeometry::LengthUnit lengthUnits = iGeometry->getUnits();
  QString lengthUnitStr = IGeometry::LengthUnitToString(lengthUnits);
  if(lengthUnits == IGeometry::LengthUnit::Unspecified)
  {
    lengthUnitStr.append(" Units");
  }

  QString desc =
      QObject::tr("X Partition Bounds: %1 to %2 [%4].   Delta: %3 [%4].\n").arg(QString::number(m_PartitioningSchemeOrigin[0]), QString::number(xRangeMax), QString::number(xDelta), lengthUnitStr);
  desc.append(
      QObject::tr("Y Partition Bounds: %1 to %2 [%4].   Delta: %3 [%4].\n").arg(QString::number(m_PartitioningSchemeOrigin[1]), QString::number(yRangeMax), QString::number(yDelta), lengthUnitStr));
  desc.append(
      QObject::tr("Z Partition Bounds: %1 to %2 [%4].   Delta: %3 [%4].\n").arg(QString::number(m_PartitioningSchemeOrigin[2]), QString::number(zRangeMax), QString::number(zDelta), lengthUnitStr));

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

      if(origin[0] < m_PartitioningSchemeOrigin[0] || origin[1] < m_PartitioningSchemeOrigin[1] || origin[2] < m_PartitioningSchemeOrigin[2] || gxRangeMax > xRangeMax || gyRangeMax > yRangeMax ||
         gzRangeMax > zRangeMax)
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
QString PartitionGeometry::getInputImageGeometryInformation(const ImageGeom& geometry) const
{
  FloatVec3Type origin = geometry.getOrigin();
  FloatVec3Type spacing = geometry.getSpacing();
  SizeVec3Type dims = geometry.getDimensions();
  IGeometry::LengthUnit lengthUnits = geometry.getUnits();
  QString lengthUnitStr = IGeometry::LengthUnitToString(lengthUnits);
  if(lengthUnits == IGeometry::LengthUnit::Unspecified)
  {
    lengthUnitStr.append(" Units");
  }
  float xRangeMax = origin[0] + (dims[0] * spacing[0]);
  float xDelta = dims[0] * spacing[0];
  float yRangeMax = origin[1] + (dims[1] * spacing[1]);
  float yDelta = dims[1] * spacing[1];
  float zRangeMax = origin[2] + (dims[2] * spacing[2]);
  float zDelta = dims[2] * spacing[2];

  QString desc = QObject::tr("X Range: %1 to %2 [%5] (Delta: %3 [%5]) 0-%4 Voxels\n")
                     .arg(QString::number(origin[0]), QString::number(xRangeMax), QString::number(xDelta), QString::number(dims[0] - 1), lengthUnitStr);
  desc.append(QObject::tr("Y Range: %1 to %2 [%5] (Delta: %3 [%5]) 0-%4 Voxels\n")
                  .arg(QString::number(origin[1]), QString::number(yRangeMax), QString::number(yDelta), QString::number(dims[1] - 1), lengthUnitStr));
  desc.append(QObject::tr("Z Range: %1 to %2 [%5] (Delta: %3 [%5]) 0-%4 Voxels\n")
                  .arg(QString::number(origin[2]), QString::number(zRangeMax), QString::number(zDelta), QString::number(dims[2] - 1), lengthUnitStr));
  return desc;
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getInputRectGridGeometryInformation() const
{
  return "Rectilinear grid geometry space unknown during preflight.";
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getInputVertexGeometryInformation() const
{
  return "Vertex geometry space unknown during preflight.";
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getInputEdgeGeometryInformation() const
{
  return "Edge geometry space unknown during preflight.";
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getInputTriangleGeometryInformation() const
{
  return "Triangle geometry space unknown during preflight.";
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getInputQuadGeometryInformation() const
{
  return "Quad geometry space unknown during preflight.";
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getInputTetrahedralGeometryInformation() const
{
  return "Tetrahedral geometry space unknown during preflight.";
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getInputHexahedralGeometryInformation() const
{
  return "Hexahedral geometry space unknown during preflight.";
}

// -----------------------------------------------------------------------------
QString PartitionGeometry::getInputUnknownGeometryInformation() const
{
  return "Unknown geometry type detected.";
}
