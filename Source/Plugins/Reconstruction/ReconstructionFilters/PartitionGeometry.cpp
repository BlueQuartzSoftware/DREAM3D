/* ============================================================================
 * Copyright (c) 2022-2022 BlueQuartz Software, LLC
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "PartitionGeometry.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
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

namespace Detail
{
const QString k_RectGridSpaceUnknownStr = "Rectilinear grid geometry space unknown during preflight.";

enum PartitioningMode
{
  Simple = 0,
  Custom = 1,
  BoundingBox = 2
};

std::optional<QString> InitPartitioningGeometryUsingBoundingBox(ImageGeom& partitionImageGeometry, IntVec3Type& numberOfPartitionsPerAxis, const FloatArrayType& vertices, bool inPreflight)
{
  if(inPreflight)
  {
    // Do not do this in preflight because the bounding box data is not available
    return "Node-based geometry space unknown during preflight.";
  }

  FloatVec3Type ll = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
  FloatVec3Type ur = {std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min()};

  // For each tuple, update the current lower-left and upper-right coordinates
  for(size_t tuple = 0; tuple < vertices.getNumberOfTuples(); tuple++)
  {
    float x = vertices.getComponent(tuple, 0);
    ll[0] = (x < ll[0]) ? x : ll[0];
    ur[0] = (x > ur[0]) ? x : ur[0];

    float y = vertices.getComponent(tuple, 1);
    ll[1] = (y < ll[1]) ? y : ll[1];
    ur[1] = (y > ur[1]) ? y : ur[1];

    float z = vertices.getComponent(tuple, 2);
    ll[2] = (z < ll[2]) ? z : ll[2];
    ur[2] = (z > ur[2]) ? z : ur[2];
  }

  // Set the origin to the bottom left vertex
  partitionImageGeometry.setOrigin(ll);

  // Calculate the total distance for each dimension
  float totalXDist = ur[0] - ll[0];
  float totalYDist = ur[1] - ll[1];
  float totalZDist = ur[2] - ll[2];

  // Calculate the length per partition for each dimension, and set it into the partitioning scheme image geometry
  float lengthX = totalXDist / numberOfPartitionsPerAxis.getX();
  float lengthY = totalYDist / numberOfPartitionsPerAxis.getY();
  float lengthZ = totalZDist / numberOfPartitionsPerAxis.getZ();
  FloatVec3Type lengthPerPartition = {lengthX, lengthY, lengthZ};
  partitionImageGeometry.setSpacing(lengthPerPartition);

  return {};
}

template <typename T>
std::optional<QString> InitSimplePartitioningGeometry(const T& geometry, ImageGeom& partitionImageGeometry, IntVec3Type& numberOfPartitionsPerAxis, bool inPreflight)
{
  SharedVertexList::Pointer vertexList = geometry.getVertices();
  return Detail::InitPartitioningGeometryUsingBoundingBox(partitionImageGeometry, numberOfPartitionsPerAxis, *vertexList, inPreflight);
}

template <>
std::optional<QString> InitSimplePartitioningGeometry(const ImageGeom& geometry, ImageGeom& partitionImageGeometry, IntVec3Type& numberOfPartitionsPerAxis, bool inPreflight)
{
  Q_UNUSED(inPreflight)

  partitionImageGeometry.setOrigin(geometry.getOrigin());

  SizeVec3Type dims = geometry.getDimensions();
  FloatVec3Type spacing = geometry.getSpacing();
  float lengthX = static_cast<float>(dims.getX()) / numberOfPartitionsPerAxis.getX() * spacing[0];
  float lengthY = static_cast<float>(dims.getY()) / numberOfPartitionsPerAxis.getY() * spacing[1];
  float lengthZ = static_cast<float>(dims.getZ()) / numberOfPartitionsPerAxis.getZ() * spacing[2];
  FloatVec3Type lengthPerPartition = {lengthX, lengthY, lengthZ};
  partitionImageGeometry.setSpacing(lengthPerPartition);
  return {};
}

template <>
std::optional<QString> InitSimplePartitioningGeometry(const RectGridGeom& geometry, ImageGeom& partitionImageGeometry, IntVec3Type& numberOfPartitionsPerAxis, bool inPreflight)
{
  // Do not do this in preflight because the bounds data is not available
  if(inPreflight)
  {
    return Detail::k_RectGridSpaceUnknownStr;
  }

  FloatArrayType::Pointer xBounds = geometry.getXBounds();
  FloatArrayType::Pointer yBounds = geometry.getYBounds();
  FloatArrayType::Pointer zBounds = geometry.getZBounds();
  FloatVec3Type origin = {0.0f, 0.0f, 0.0f};
  FloatVec3Type lengthPerPartition = {0.0f, 0.0f, 0.0f};
  if(xBounds != FloatArrayType::NullPointer() && xBounds->getSize() > 0)
  {
    origin.setX(xBounds->at(0));

    float maxX = xBounds->getValue(xBounds->getNumberOfTuples() - 1);
    lengthPerPartition.setX(maxX / numberOfPartitionsPerAxis.getX());
  }
  if(yBounds != FloatArrayType::NullPointer() && yBounds->getSize() > 0)
  {
    origin.setY(yBounds->at(0));

    float maxY = yBounds->getValue(yBounds->getNumberOfTuples() - 1);
    lengthPerPartition.setY(maxY / numberOfPartitionsPerAxis.getY());
  }
  if(zBounds != FloatArrayType::NullPointer() && zBounds->getSize() > 0)
  {
    origin.setZ(zBounds->at(0));

    float maxZ = zBounds->getValue(zBounds->getNumberOfTuples() - 1);
    lengthPerPartition.setZ(maxZ / numberOfPartitionsPerAxis.getZ());
  }
  partitionImageGeometry.setOrigin(origin);
  partitionImageGeometry.setSpacing(lengthPerPartition);

  return {};
}
} // namespace Detail

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

  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Select the partitioning mode");
    parameter->setPropertyName("PartitioningMode");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(PartitionGeometry, this, PartitioningMode));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(PartitionGeometry, this, PartitioningMode));
    parameter->setDefaultValue(0); // Always start with the standard sampling mode

    std::vector<QString> choices;
    choices.push_back("Simple");
    choices.push_back("Custom");
    choices.push_back("Bounding Box");
    parameter->setChoices(choices);
    std::vector<QString> linkedProps;
    linkedProps.push_back("OutOfBoundsValue");
    linkedProps.push_back("PartitioningSchemeOrigin");
    linkedProps.push_back("LengthPerPartition");
    linkedProps.push_back("BoundingBoxPath");
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Category::Parameter);
    parameters.push_back(parameter);
  }

  parameters.push_back(SIMPL_NEW_INTEGER_FP("Starting Partition ID", StartingPartitionID, FilterParameter::Category::Parameter, PartitionGeometry));
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Out-Of-Bounds Default Value", OutOfBoundsValue, FilterParameter::Category::Parameter, PartitionGeometry, 1));
  parameters.push_back(SeparatorFilterParameter::Create("Partitioning Scheme", FilterParameter::Category::Parameter));
  parameters.push_back(SIMPL_NEW_INT_VEC3_FP("Number Of Partitions Per Axis (X, Y, Z)", NumberOfPartitionsPerAxis, FilterParameter::Category::Parameter, PartitionGeometry));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Partitioning Scheme Origin (X, Y, Z)", PartitioningSchemeOrigin, FilterParameter::Category::Parameter, PartitionGeometry, 1));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Length Per Partition (X, Y, Z)", LengthPerPartition, FilterParameter::Category::Parameter, PartitionGeometry, 1));

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

  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Type::Any, IGeometry::Type::Any);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Partitioning Scheme Bounding Box", BoundingBoxPath, FilterParameter::Category::RequiredArray, PartitionGeometry, req, 2));
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

  if(m_NumberOfPartitionsPerAxis.getX() < 0)
  {
    QString ss = QObject::tr("Number of Partitions Per Axis: The X value cannot be negative.");
    setErrorCondition(-3000, ss);
    return;
  }
  if(m_NumberOfPartitionsPerAxis.getY() < 0)
  {
    QString ss = QObject::tr("Number of Partitions Per Axis: The Y value cannot be negative.");
    setErrorCondition(-3001, ss);
    return;
  }
  if(m_NumberOfPartitionsPerAxis.getZ() < 0)
  {
    QString ss = QObject::tr("Number of Partitions Per Axis: The Z value cannot be negative.");
    setErrorCondition(-3002, ss);
    return;
  }

  if(m_LengthPerPartition.getX() < 0)
  {
    QString ss = QObject::tr("Length Per Partition: The X value cannot be negative.");
    setErrorCondition(-3003, ss);
    return;
  }
  if(m_LengthPerPartition.getY() < 0)
  {
    QString ss = QObject::tr("Length Per Partition: The Y value cannot be negative.");
    setErrorCondition(-3004, ss);
    return;
  }
  if(m_LengthPerPartition.getZ() < 0)
  {
    QString ss = QObject::tr("Length Per Partition: The Z value cannot be negative.");
    setErrorCondition(-3005, ss);
    return;
  }

  if(m_NumberOfPartitionsPerAxis.getX() <= 0)
  {
    QString ss = QObject::tr("Number of Partitions Per Axis: The 1st dimension must be greater than 0.");
    setErrorCondition(-3006, ss);
    return;
  }

  if(m_NumberOfPartitionsPerAxis.getY() <= 0)
  {
    QString ss = QObject::tr("Number of Partitions Per Axis: The 2nd dimension must be greater than 0.");
    setErrorCondition(-3007, ss);
    return;
  }

  if(m_NumberOfPartitionsPerAxis.getZ() <= 0)
  {
    QString ss = QObject::tr("Number of Partitions Per Axis: The 3rd dimension must be greater than 0.");
    setErrorCondition(-3008, ss);
    return;
  }

  AttributeMatrix::Pointer am = getDataContainerArray()->getPrereqAttributeMatrixFromPath(this, m_AttributeMatrixPath, 1);
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(m_AttributeMatrixPath.getDataContainerName());
  IGeometry::Pointer iGeometry = dc->getGeometry();
  if(iGeometry == IGeometry::NullPointer())
  {
    QString ss = QObject::tr("The data container '%1', containing the attribute matrix '%2', must have a geometry.")
                     .arg(m_AttributeMatrixPath.getDataContainerName(), m_AttributeMatrixPath.getAttributeMatrixName());
    setErrorCondition(-3009, ss);
    return;
  }

  if(am->getNumberOfTuples() != iGeometry->getNumberOfElements())
  {
    QString ss = QObject::tr("The attribute matrix '%1' does not have the same number of tuples (%2) as the geometry in data container '%3' (%4).")
                     .arg(m_AttributeMatrixPath.getAttributeMatrixName(), QString::number(am->getNumberOfTuples()), dc->getName(), QString::number(iGeometry->getNumberOfElements()));
    setErrorCondition(-3010, ss);
    return;
  }

  if(m_PartitioningMode == Detail::PartitioningMode::BoundingBox)
  {
    getDataContainerArray()->getPrereqArrayFromPath<FloatArrayType>(this, m_BoundingBoxPath, {3});
    if(getErrorCode() < 0)
    {
      return;
    }
  }

  DataArrayPath tempPath;

  switch(iGeometry->getGeometryType())
  {
  case IGeometry::Type::Image:
  {
    tempPath = DataArrayPath(m_AttributeMatrixPath.getDataContainerName(), m_AttributeMatrixPath.getAttributeMatrixName(), getPartitionIdsArrayName());
    ImageGeom::Pointer geometry = dc->getGeometryAs<ImageGeom>();
    m_PartitionImageGeometryResult = createPartitioningSchemeGeometry(*geometry);
    break;
  }
  case IGeometry::Type::RectGrid:
  {
    tempPath = DataArrayPath(m_AttributeMatrixPath.getDataContainerName(), m_AttributeMatrixPath.getAttributeMatrixName(), getPartitionIdsArrayName());
    RectGridGeom::Pointer geometry = dc->getGeometryAs<RectGridGeom>();
    m_PartitionImageGeometryResult = createPartitioningSchemeGeometry(*geometry);
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
    m_PartitionImageGeometryResult = createPartitioningSchemeGeometry(*geometry);
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
    m_PartitionImageGeometryResult = createPartitioningSchemeGeometry(*geometry);
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
    m_PartitionImageGeometryResult = createPartitioningSchemeGeometry(*geometry);
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
    m_PartitionImageGeometryResult = createPartitioningSchemeGeometry(*geometry);
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
    m_PartitionImageGeometryResult = createPartitioningSchemeGeometry(*geometry);
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
    m_PartitionImageGeometryResult = createPartitioningSchemeGeometry(*geometry);
    break;
  }
  default:
  {
    QString ss = QObject::tr("Unable to partition geometry - Unknown geometry type detected.");
    setErrorCondition(-3011, ss);
    return;
  }
  }

  if(getErrorCode() < 0)
  {
    return;
  }

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

    ps_dc->setGeometry(m_PartitionImageGeometryResult.first);

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
    partitionCellBasedGeometry(*geometry, *partitionIdsPtr);
    break;
  }
  case IGeometry::Type::RectGrid:
  {
    RectGridGeom::Pointer geometry = dc->getGeometryAs<RectGridGeom>();
    partitionCellBasedGeometry(*geometry, *partitionIdsPtr);
    break;
  }
  case IGeometry::Type::Vertex:
  {
    VertexGeom::Pointer geometry = dc->getGeometryAs<VertexGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    partitionNodeBasedGeometry(*vertexList, *partitionIdsPtr);
    break;
  }
  case IGeometry::Type::Edge:
  {
    EdgeGeom::Pointer geometry = dc->getGeometryAs<EdgeGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    partitionNodeBasedGeometry(*vertexList, *partitionIdsPtr);
    break;
  }
  case IGeometry::Type::Triangle:
  {
    TriangleGeom::Pointer geometry = dc->getGeometryAs<TriangleGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    partitionNodeBasedGeometry(*vertexList, *partitionIdsPtr);
    break;
  }
  case IGeometry::Type::Quad:
  {
    QuadGeom::Pointer geometry = dc->getGeometryAs<QuadGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    partitionNodeBasedGeometry(*vertexList, *partitionIdsPtr);
    break;
  }
  case IGeometry::Type::Tetrahedral:
  {
    TetrahedralGeom::Pointer geometry = dc->getGeometryAs<TetrahedralGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    partitionNodeBasedGeometry(*vertexList, *partitionIdsPtr);
    break;
  }
  case IGeometry::Type::Hexahedral:
  {
    HexahedralGeom::Pointer geometry = dc->getGeometryAs<HexahedralGeom>();
    SharedVertexList::Pointer vertexList = geometry->getVertices();
    partitionNodeBasedGeometry(*vertexList, *partitionIdsPtr);
    break;
  }
  default:
  {
    QString ss = QObject::tr("Unable to partition geometry - Unknown geometry type detected.");
    setErrorCondition(-3012, ss);
    return;
  }
  }

  if(m_SavePartitioningScheme)
  {
    for(size_t i = 0; i < m_PartitionImageGeometryResult.first->getNumberOfElements(); i++)
    {
      m_PartitioningSchemeIds[i] = i + m_StartingPartitionID;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
PartitionGeometry::PartitioningImageGeomResult PartitionGeometry::createPartitioningSchemeGeometry(const T& geometry)
{
  ImageGeom::Pointer partitionImageGeometry = ImageGeom::CreateGeometry(m_PSImageGeomName);
  partitionImageGeometry->setDimensions(m_NumberOfPartitionsPerAxis.convertType<size_t>());
  partitionImageGeometry->setOrigin({0, 0, 0});
  partitionImageGeometry->setSpacing({0, 0, 0});

  switch(m_PartitioningMode)
  {
  case Detail::PartitioningMode::Simple:
  {
    std::optional<QString> maybeErrMsg = Detail::InitSimplePartitioningGeometry(geometry, *partitionImageGeometry, m_NumberOfPartitionsPerAxis, getInPreflight());
    return {partitionImageGeometry, maybeErrMsg};
  }
  case Detail::PartitioningMode::Custom:
  {
    partitionImageGeometry->setOrigin(m_PartitioningSchemeOrigin);
    partitionImageGeometry->setSpacing(m_LengthPerPartition);
    return {partitionImageGeometry, {}};
  }
  case Detail::PartitioningMode::BoundingBox:
  {
    AttributeMatrix::Pointer am = getDataContainerArray()->getAttributeMatrix(m_BoundingBoxPath);
    FloatArrayType::Pointer boundingBoxArray = am->getAttributeArrayAs<FloatArrayType>(m_BoundingBoxPath.getDataArrayName());
    std::optional<QString> maybeErrMsg = Detail::InitPartitioningGeometryUsingBoundingBox(*partitionImageGeometry, m_NumberOfPartitionsPerAxis, *boundingBoxArray, getInPreflight());
    return {partitionImageGeometry, maybeErrMsg};
  }
  default:
  {
    QString ss = QObject::tr("Unable to create partitioning scheme geometry - Unknown partitioning mode.");
    setErrorCondition(-3011, ss);
    return {partitionImageGeometry, ss};
  }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PartitionGeometry::partitionCellBasedGeometry(const IGeometryGrid& geometry, Int32ArrayType& partitionIds)
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
        auto partitionIndexResult = m_PartitionImageGeometryResult.first->getIndex(coord[0], coord[1], coord[2]);
        if(partitionIndexResult.has_value())
        {
          partitionIds.setValue(index, *partitionIndexResult + m_StartingPartitionID);
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
void PartitionGeometry::partitionNodeBasedGeometry(const SharedVertexList& vertexList, Int32ArrayType& partitionIds)
{
  size_t numOfVertices = vertexList.getNumberOfTuples();
  float* vertexArray = vertexList.getPointer(0);
  for(size_t idx = 0; idx < numOfVertices; idx++)
  {
    auto partitionIndexResult = m_PartitionImageGeometryResult.first->getIndex(vertexArray[idx * 3], vertexArray[idx * 3 + 1], vertexArray[idx * 3 + 2]);
    if(partitionIndexResult.has_value())
    {
      partitionIds.setValue(idx, *partitionIndexResult + m_StartingPartitionID);
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
void PartitionGeometry::setPartitioningMode(const int& value)
{
  m_PartitioningMode = value;
}

// -----------------------------------------------------------------------------
int PartitionGeometry::getPartitioningMode() const
{
  return m_PartitioningMode;
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
void PartitionGeometry::setBoundingBoxPath(const DataArrayPath& value)
{
  m_BoundingBoxPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath PartitionGeometry::getBoundingBoxPath() const
{
  return m_BoundingBoxPath;
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
  if(m_PartitionImageGeometryResult.first == ImageGeom::NullPointer())
  {
    return {};
  }

  if(m_PartitionImageGeometryResult.second.has_value())
  {
    // There was an error while creating the partition image geometry, return it
    return *m_PartitionImageGeometryResult.second;
  }

  FloatVec3Type partitioningSchemeOrigin = m_PartitionImageGeometryResult.first->getOrigin();
  FloatVec3Type lengthPerPartition = m_PartitionImageGeometryResult.first->getSpacing();
  float xRangeMax = (partitioningSchemeOrigin[0] + (m_NumberOfPartitionsPerAxis[0] * lengthPerPartition[0]));
  float xDelta = m_NumberOfPartitionsPerAxis[0] * lengthPerPartition[0];
  float yRangeMax = (partitioningSchemeOrigin[1] + (m_NumberOfPartitionsPerAxis[1] * lengthPerPartition[1]));
  float yDelta = m_NumberOfPartitionsPerAxis[1] * lengthPerPartition[1];
  float zRangeMax = (partitioningSchemeOrigin[2] + (m_NumberOfPartitionsPerAxis[2] * lengthPerPartition[2]));
  float zDelta = m_NumberOfPartitionsPerAxis[2] * lengthPerPartition[2];

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
      QObject::tr("X Partition Bounds: %1 to %2 [%4].   Delta: %3 [%4].\n").arg(QString::number(partitioningSchemeOrigin[0]), QString::number(xRangeMax), QString::number(xDelta), lengthUnitStr);
  desc.append(
      QObject::tr("Y Partition Bounds: %1 to %2 [%4].   Delta: %3 [%4].\n").arg(QString::number(partitioningSchemeOrigin[1]), QString::number(yRangeMax), QString::number(yDelta), lengthUnitStr));
  desc.append(
      QObject::tr("Z Partition Bounds: %1 to %2 [%4].   Delta: %3 [%4].\n").arg(QString::number(partitioningSchemeOrigin[2]), QString::number(zRangeMax), QString::number(zDelta), lengthUnitStr));

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

      if(origin[0] < partitioningSchemeOrigin[0] || origin[1] < partitioningSchemeOrigin[1] || origin[2] < partitioningSchemeOrigin[2] || gxRangeMax > xRangeMax || gyRangeMax > yRangeMax ||
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
  return Detail::k_RectGridSpaceUnknownStr;
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