/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
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
#include "CreateLambertSphere.h"

#include <cassert>
#include <cmath>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/QuadGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"

#include "EbsdLib/Utilities/LambertUtilities.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID21 = 21,
  AttributeMatrixID22 = 22,
  AttributeMatrixID23 = 23,
  AttributeMatrixID24 = 24,
  AttributeMatrixID25 = 25,
  AttributeMatrixID26 = 26,
  AttributeMatrixID27 = 27,

  DataArrayID31 = 31,
  DataArrayID32 = 32,
  DataArrayID33 = 33,
  DataArrayID34 = 34,

  DataContainerID = 1
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateLambertSphere::CreateLambertSphere() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateLambertSphere::~CreateLambertSphere() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateLambertSphere::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
  parameter->setHumanLabel("Select Hemisphere to Generate");
  parameter->setPropertyName("Hemisphere");
  parameter->setSetterCallback(SIMPL_BIND_SETTER(CreateLambertSphere, this, Hemisphere));
  parameter->setGetterCallback(SIMPL_BIND_GETTER(CreateLambertSphere, this, Hemisphere));
  std::vector<QString> choices;
  choices.push_back("Northern");
  choices.push_back("Southern");
  // choices.push_back("Both");
  parameter->setChoices(choices);
  parameter->setCategory(FilterParameter::Category::Parameter);
  parameters.push_back(parameter);

  {
    std::vector<QString> linkedProperties = {"VertexDataContainerName"};
    parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Create Vertex Geometry", CreateVertexGeometry, FilterParameter::Category::Parameter, CreateLambertSphere, linkedProperties));
    parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Output Vertex DataContainer Name", VertexDataContainerName, FilterParameter::Category::Parameter, CreateLambertSphere));
    //    parameters.push_back(SIMPL_NEW_STRING_FP("Vertex Attribute Matrix", VertexAttributeMatrixName, FilterParameter::Category::Parameter, CreateLambertSphere));
  }

  {
    std::vector<QString> linkedProperties = {"EdgeDataContainerName"};
    parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Create Edge Geometry", CreateEdgeGeometry, FilterParameter::Category::Parameter, CreateLambertSphere, linkedProperties));
    parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Output Edge DataContainer Name", EdgeDataContainerName, FilterParameter::Category::Parameter, CreateLambertSphere));
    //    parameters.push_back(SIMPL_NEW_STRING_FP("Edge Attribute Matrix", EdgeAttributeMatrixName, FilterParameter::Category::Parameter, CreateLambertSphere));
  }

  {
    std::vector<QString> linkedProperties = {"TriangleDataContainerName"};
    parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Create Triangle Geometry", CreateTriangleGeometry, FilterParameter::Category::Parameter, CreateLambertSphere, linkedProperties));
    parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Output Triangle DataContainer Name", TriangleDataContainerName, FilterParameter::Category::Parameter, CreateLambertSphere));
  }

  {
    std::vector<QString> linkedProperties = {"QuadDataContainerName"};
    parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Create Quad Geometry", CreateQuadGeometry, FilterParameter::Category::Parameter, CreateLambertSphere, linkedProperties));
    parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Output Quad DataContainer Name", QuadDataContainerName, FilterParameter::Category::Parameter, CreateLambertSphere));
    //    parameters.push_back(SIMPL_NEW_STRING_FP("Quad Attribute Matrix", FaceAttributeMatrixName, FilterParameter::Category::Parameter, CreateLambertSphere));
    //    parameters.push_back(SIMPL_NEW_STRING_FP("Quad Face ArrayName", ImageFaceDataArrayName, FilterParameter::Category::Parameter, CreateLambertSphere));
  }

  {
    // Option to map an existing image
    //  QStringList linkedProperties = {"ImageDataArrayPath"};
    //  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Use Existing Image", UseExistingImage, FilterParameter::Category::Parameter, CreateLambertSphere, linkedProperties));

    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt8, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Image Data", ImageDataArrayPath, FilterParameter::Category::RequiredArray, CreateLambertSphere, req));
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateLambertSphere::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateLambertSphere::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  initialize();

  if(getHemisphere() < 0 || getHemisphere() > 2)
  {
    QString msg("Invalid selection of the Hemisphere value. Valid values are 0 (Northern), 1 (Southern)");
    setErrorCondition(-99006, msg);
    return;
  }
  std::vector<size_t> cDims = {1};
  DataContainerArray::Pointer dca = getDataContainerArray();

  m_VertexDataName = "Vertex_" + getImageDataArrayPath().getDataArrayName();
  m_EdgeDataName = "Edge_" + getImageDataArrayPath().getDataArrayName();
  m_TriangleDataName = "Triangle_" + getImageDataArrayPath().getDataArrayName();
  m_QuadDataName = "Quad_" + getImageDataArrayPath().getDataArrayName();

  if(getUseExistingImage())
  {
    m_ImageDataPtr = getDataContainerArray()->getPrereqArrayFromPath<UInt8ArrayType>(this, getImageDataArrayPath(), cDims);
    if(nullptr != m_ImageDataPtr.lock())
    {
      m_ImageData = m_ImageDataPtr.lock()->getPointer(0);
    }
  }

  DataContainer::Pointer m = dca->getPrereqDataContainer(this, getImageDataArrayPath().getDataContainerName(), false);
  if(nullptr == m.get())
  {
    return;
  }
  ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();
  if(nullptr == imageGeom)
  {
    QString msg("The geometry object was invalid for the image data. Please select a DataContainer that has an Image Geometry or use a filter to create an ImageGeom.");
    setErrorCondition(-99003, msg);
    return;
  }
  SizeVec3Type imageDims = imageGeom->getDimensions();
  if(imageDims[0] != imageDims[1])
  {
    QString msg;
    QTextStream ss(&msg);
    ss << "The input image must be square, i.e., the number of pixels in the X & Y direction must be equal. The current dimensions are";
    ss << " X=" << imageDims[0] << " Y=" << imageDims[1] << " Z=" << imageDims[2];
    setErrorCondition(-99004, msg);
    return;
  }
  if(imageDims[2] != 1)
  {
    QString msg;
    QTextStream ss(&msg);
    ss << "The input image must be a single XY Plane image. The current dimensions are";
    ss << " X=" << imageDims[0] << " Y=" << imageDims[1] << " Z=" << imageDims[2];
    setErrorCondition(-99005, msg);
    return;
  }

  int64_t points[3] = {static_cast<int64_t>(imageDims[0] + 1), static_cast<int64_t>(imageDims[1] + 1), 1};
  int64_t totalVerts = points[0] * points[1];
  int64_t totalQuads = static_cast<int64_t>(imageDims[0] * imageDims[1]);
  if(getHemisphere() == 2)
  {
    totalVerts = totalVerts * 2;
    totalQuads = totalQuads * 2;
  }

  size_t numVertices = static_cast<size_t>(points[0] * points[1]);
  std::vector<size_t> vertDims(1, 3);
  m_Vertices = SharedVertexList::CreateArray(numVertices, vertDims, SIMPL::Geometry::SharedVertexList, !getInPreflight());
  m_Vertices->initializeWithZeros();

  // Create a Vertex Geometry
  if(getCreateVertexGeometry())
  {
    DataContainer::Pointer vertDC = dca->createNonPrereqDataContainer(this, getVertexDataContainerName(), DataContainerID);
    if(getErrorCode() < 0)
    {
      return;
    }

    // Create the Vertex Geometry
    VertexGeom::Pointer vertGeom = VertexGeom::CreateGeometry(m_Vertices, "VertexGeometry");
    vertDC->setGeometry(vertGeom);

    std::vector<size_t> tDims = {static_cast<size_t>(totalVerts)};
    vertDC->createNonPrereqAttributeMatrix(this, getVertexAttributeMatrixName(), tDims, AttributeMatrix::Type::Vertex, AttributeMatrixID21);

    //    std::vector<size_t> cDims = {1};
    //    DataArrayPath path(getVertexDataContainerName(), getVertexAttributeMatrixName(), m_VertexDataName);
    //    m_VertexDataPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>>(this, path, 0, cDims, "", DataArrayID31);    //
    //    if(nullptr != m_VertexDataPtr.lock())
    //    {
    //      m_VertexData = m_VertexDataPtr.lock()->getPointer(0);
    //    }
  }

  // Create a Edge Geometry
  if(getCreateEdgeGeometry())
  {
    DataContainer::Pointer edgeDC = dca->createNonPrereqDataContainer(this, getEdgeDataContainerName(), DataContainerID);
    if(getErrorCode() < 0)
    {
      return;
    }

    size_t numEdges = ((imageDims[0] + 1) * imageDims[0]) + ((imageDims[1] + 1) * imageDims[1]);

    std::vector<size_t> tDims = {static_cast<size_t>(totalVerts)};
    edgeDC->createNonPrereqAttributeMatrix(this, getVertexAttributeMatrixName(), tDims, AttributeMatrix::Type::Vertex, AttributeMatrixID22);
    tDims[0] = numEdges;
    edgeDC->createNonPrereqAttributeMatrix(this, getEdgeAttributeMatrixName(), tDims, AttributeMatrix::Type::Edge, AttributeMatrixID23);

    // Create Edge Geometry
    std::vector<size_t> cDims = {2};
    EdgeGeom::Pointer edgetGeom = EdgeGeom::CreateGeometry(numEdges, m_Vertices, "EdgeGeometry");
    edgeDC->setGeometry(edgetGeom);

    cDims[0] = 1;
    DataArrayPath path(getEdgeDataContainerName().getDataContainerName(), getEdgeAttributeMatrixName(), m_EdgeDataName);
    m_EdgeDataPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>>(this, path, 0, cDims, "", DataArrayID32);
    if(nullptr != m_EdgeDataPtr.lock())
    {
      m_EdgeData = m_EdgeDataPtr.lock()->getPointer(0);
    }
  }

  // Create a Triangle Geometry
  if(getCreateTriangleGeometry())
  {
    DataContainer::Pointer triangleDC = dca->createNonPrereqDataContainer(this, getTriangleDataContainerName(), DataContainerID);
    if(getErrorCode() < 0)
    {
      return;
    }

    std::vector<size_t> tDims = {static_cast<size_t>(totalVerts)};
    triangleDC->createNonPrereqAttributeMatrix(this, getVertexAttributeMatrixName(), tDims, AttributeMatrix::Type::Vertex, AttributeMatrixID24);
    tDims[0] = static_cast<size_t>(totalQuads * 2);
    triangleDC->createNonPrereqAttributeMatrix(this, getFaceAttributeMatrixName(), tDims, AttributeMatrix::Type::Face, AttributeMatrixID25);

    // Create a Triangle Geometry
    size_t numTris = imageDims[0] * imageDims[1] * 2; // Twice the number of Quads
    std::vector<size_t> cDims = {3};
    TriangleGeom::Pointer triangleGeom = TriangleGeom::CreateGeometry(numTris, m_Vertices, "Triangle Geometry");
    triangleDC->setGeometry(triangleGeom);

    cDims[0] = 1;
    DataArrayPath path(getTriangleDataContainerName().getDataContainerName(), getFaceAttributeMatrixName(), m_TriangleDataName);
    m_TriangleFaceDataPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>>(this, path, 0, cDims, "", DataArrayID33);
    if(nullptr != m_TriangleFaceDataPtr.lock())
    {
      m_TriangleFaceData = m_TriangleFaceDataPtr.lock()->getPointer(0);
    }
  }

  // Create a QuadGeometry
  if(getCreateQuadGeometry())
  {
    DataContainer::Pointer quadDC = dca->createNonPrereqDataContainer(this, getQuadDataContainerName(), DataContainerID);
    if(getErrorCode() < 0)
    {
      return;
    }

    std::vector<size_t> tDims(1, static_cast<size_t>(totalVerts));
    quadDC->createNonPrereqAttributeMatrix(this, getVertexAttributeMatrixName(), tDims, AttributeMatrix::Type::Vertex, AttributeMatrixID26);
    tDims[0] = static_cast<size_t>(totalQuads);
    quadDC->createNonPrereqAttributeMatrix(this, getFaceAttributeMatrixName(), tDims, AttributeMatrix::Type::Face, AttributeMatrixID27);

    // Create a Quad Geometry
    QuadGeom::Pointer quadGeom = QuadGeom::CreateGeometry(totalQuads, m_Vertices, SIMPL::Geometry::QuadGeometry, !getInPreflight());
    quadDC->setGeometry(quadGeom);

    cDims[0] = 1;
    DataArrayPath path(getQuadDataContainerName().getDataContainerName(), getFaceAttributeMatrixName(), m_QuadDataName);
    m_QuadFaceDataPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>>(this, path, 0, cDims, "", DataArrayID34);
    if(nullptr != m_QuadFaceDataPtr.lock())
    {
      m_QuadFaceData = m_QuadFaceDataPtr.lock()->getPointer(0);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateLambertSphere::execute()
{
  // 1.253314137315501
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  createVertices();

  createVertexGeometry();
  createEdgeGeometry();
  createTriangleGeometry();
  createQuadGeometry();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateLambertSphere::createVertices()
{

  // Get the dimensions of the lambert image we are going to map to a sphere.
  ImageGeom::Pointer imageGeom = getDataContainerArray()->getDataContainer(getImageDataArrayPath())->getGeometryAs<ImageGeom>();
  SizeVec3Type imageDims = imageGeom->getDimensions();

  UInt8ArrayType::Pointer masterPattern = getDataContainerArray()->getAttributeMatrix(getImageDataArrayPath())->getAttributeArrayAs<UInt8ArrayType>(getImageDataArrayPath().getDataArrayName());

  float L = SIMPLib::Constants::k_SqrtHalfPiF;
  float res = (2.0f * L) / imageDims[0];

  // The number of vertices in X & Y is one more than the dims
  int64_t points[3] = {static_cast<int64_t>(imageDims[0] + 1), static_cast<int64_t>(imageDims[1] + 1), 1};

  //  DataContainer::Pointer vertexDC = getDataContainerArray()->getDataContainer(getVertexDataContainerName());
  //  m_VertexData = m_VertexDataPtr.lock()->getPointer(0);

  // int64_t iIndex = 0;
  size_t vIndex = 0;
  // Generate all the vertex values
  for(int64_t y = 0; y < points[1]; y++)
  {
    float* vert = nullptr;
    for(int64_t x = 0; x < points[0]; x++)
    {
      vert = m_Vertices->getTuplePointer(vIndex);
      vert[0] = x * res - L;
      vert[1] = y * res - L;
      vert[2] = 0.0;

      //      if (x < points[0]-1 && y < points[1] - 1)
      //      {
      //        iIndex = (imageDims[0] * y) + x;
      //        m_VertexData[iIndex] = masterPattern->getValue(iIndex);
      //      }
      vIndex++;
    }
  }

  transformFromLambertSquareToSphere(m_Vertices.get());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateLambertSphere::createVertexGeometry()
{
  if(!getCreateVertexGeometry())
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateLambertSphere::createEdgeGeometry()
{
  if(!getCreateEdgeGeometry())
  {
    return;
  }

  // Get the dimensions of the lambert image we are going to map to a sphere.
  ImageGeom::Pointer imageGeom = getDataContainerArray()->getDataContainer(getImageDataArrayPath())->getGeometryAs<ImageGeom>();
  SizeVec3Type imageDims = imageGeom->getDimensions();

  // now create node and quad arrays knowing the number that will be needed
  DataContainer::Pointer edgeDC = getDataContainerArray()->getDataContainer(getEdgeDataContainerName());

  EdgeGeom::Pointer edgeGeom = edgeDC->getGeometryAs<EdgeGeom>();
  SharedEdgeList::Pointer edges = edgeGeom->getEdges();

  size_t vIndex;
  size_t eIndex = 0;
  for(size_t y = 0; y < imageDims[1]; y++)
  {
    for(size_t x = 0; x < imageDims[0]; x++)
    {
      vIndex = ((imageDims[0] + 1) * y) + x;

      MeshIndexType* edge = edges->getTuplePointer(eIndex++);
      edge[0] = static_cast<int64_t>(vIndex + imageDims[0] + 1);
      edge[1] = static_cast<int64_t>(vIndex);
      edge = edges->getTuplePointer(eIndex++);
      edge[0] = static_cast<int64_t>(vIndex);
      edge[1] = static_cast<int64_t>(vIndex + 1);

      if(x == imageDims[0] - 1)
      {
        edge = edges->getTuplePointer(eIndex++);
        edge[0] = static_cast<int64_t>(vIndex + 1);
        edge[1] = static_cast<int64_t>(vIndex + imageDims[0] + 1 + 1);
      }

      if(y == imageDims[1] - 1)
      {
        edge = edges->getTuplePointer(eIndex++);
        edge[0] = static_cast<int64_t>(vIndex + imageDims[0] + 1 + 1);
        edge[1] = static_cast<int64_t>(vIndex + imageDims[0] + 1);
      }

      //      int64_t* quad = quads->getTuplePointer(qIndex);
      //      quad[0] = static_cast<int64_t>(vIndex);
      //      quad[1] = static_cast<int64_t>(vIndex + 1);
      //      quad[2] = static_cast<int64_t>(vIndex + imageDims[0] + 1 + 1);
      //      quad[3] = static_cast<int64_t>(vIndex + imageDims[0] + 1);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateLambertSphere::createTriangleGeometry()
{
  if(!getCreateTriangleGeometry())
  {
    return;
  }
  // Get the dimensions of the lambert image we are going to map to a sphere.
  ImageGeom::Pointer imageGeom = getDataContainerArray()->getDataContainer(getImageDataArrayPath())->getGeometryAs<ImageGeom>();
  SizeVec3Type imageDims = imageGeom->getDimensions();

  UInt8ArrayType::Pointer masterPattern = getDataContainerArray()->getAttributeMatrix(getImageDataArrayPath())->getAttributeArrayAs<UInt8ArrayType>(getImageDataArrayPath().getDataArrayName());

  // now create node and triangle arrays knowing the number that will be needed
  DataContainer::Pointer triangleDC = getDataContainerArray()->getDataContainer(getTriangleDataContainerName());

  TriangleGeom::Pointer triangleGeom = triangleDC->getGeometryAs<TriangleGeom>();
  SharedTriList::Pointer triangles = triangleGeom->getTriangles();

  m_TriangleFaceData = m_TriangleFaceDataPtr.lock()->getPointer(0);
  size_t iIndex = 0;
  size_t vIndex = 0;
  size_t tIndex = 0;
  for(size_t y = 0; y < imageDims[1]; y++)
  {
    for(size_t x = 0; x < imageDims[0]; x++)
    {
      vIndex = ((imageDims[0] + 1) * y) + x;

      MeshIndexType* tri = triangles->getTuplePointer(tIndex);
      tri[0] = static_cast<int64_t>(vIndex);
      tri[1] = static_cast<int64_t>(vIndex + 1);
      tri[2] = static_cast<int64_t>(vIndex + imageDims[0] + 1 + 1);
      m_TriangleFaceData[tIndex] = masterPattern->getValue(iIndex);
      tIndex++;

      tri = triangles->getTuplePointer(tIndex);
      tri[0] = static_cast<int64_t>(vIndex);
      tri[1] = static_cast<int64_t>(vIndex + imageDims[0] + 1 + 1);
      tri[2] = static_cast<int64_t>(vIndex + imageDims[0] + 1);
      m_TriangleFaceData[tIndex] = masterPattern->getValue(iIndex);
      tIndex++;
      iIndex++;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateLambertSphere::createQuadGeometry()
{
  if(!getCreateQuadGeometry())
  {
    return;
  }

  float L = SIMPLib::Constants::k_SqrtHalfPiF;

  // Get the dimensions of the lambert image we are going to map to a sphere.
  ImageGeom::Pointer imageGeom = getDataContainerArray()->getDataContainer(getImageDataArrayPath())->getGeometryAs<ImageGeom>();
  SizeVec3Type imageDims = imageGeom->getDimensions();

  UInt8ArrayType::Pointer masterPattern = getDataContainerArray()->getAttributeMatrix(getImageDataArrayPath())->getAttributeArrayAs<UInt8ArrayType>(getImageDataArrayPath().getDataArrayName());

  // The number of vertices in X & Y is one more than the dims
  // int64_t points[3] = {static_cast<int64_t>(imageDims[0] + 1), static_cast<int64_t>(imageDims[1] + 1), 1};

  // now create node and quad arrays knowing the number that will be needed
  DataContainer::Pointer quadDC = getDataContainerArray()->getDataContainer(getQuadDataContainerName());
  QuadGeom::Pointer quadGeom = quadDC->getGeometryAs<QuadGeom>();
  SharedVertexList::Pointer vertices = quadGeom->getVertices();

  float res = (2.0f * L) / imageDims[0];
  imageGeom->setSpacing(FloatVec3Type(res, res, res));

  FloatVec3Type origin = {-(imageDims[0] * res) / 2.0f, -(imageDims[1] * res) / 2.0f, 0.0f};
  imageGeom->setOrigin(origin);

  size_t vIndex = 0;

  size_t totalQuads = (imageDims[0] * imageDims[1]);
  std::vector<size_t> tDims(1, totalQuads);
  quadDC->getAttributeMatrix(getFaceAttributeMatrixName())->resizeAttributeArrays(tDims);
  AttributeMatrix::Pointer am = quadDC->getAttributeMatrix(getFaceAttributeMatrixName());

  m_QuadFaceDataPtr = am->getAttributeArrayAs<UInt8ArrayType>(m_QuadDataName);
  m_QuadFaceData = m_QuadFaceDataPtr.lock()->getPointer(0);

  SharedQuadList::Pointer quads = quadGeom->getQuads();
  size_t qIndex = 0;
  for(size_t y = 0; y < imageDims[1]; y++)
  {
    for(size_t x = 0; x < imageDims[0]; x++)
    {
      vIndex = ((imageDims[0] + 1) * y) + x;

      MeshIndexType* quad = quads->getTuplePointer(qIndex);
      quad[0] = static_cast<int64_t>(vIndex);
      quad[1] = static_cast<int64_t>(vIndex + 1);
      quad[2] = static_cast<int64_t>(vIndex + imageDims[0] + 1 + 1);
      quad[3] = static_cast<int64_t>(vIndex + imageDims[0] + 1);

      // Fill in the value for the Quad Cell value.
      m_QuadFaceData[qIndex] = masterPattern->getValue(qIndex);

      qIndex++;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float CreateLambertSphere::cp(float p)
{
  float cc = (2.0f * p / SIMPLib::Constants::k_PiF) * std::sqrt(SIMPLib::Constants::k_PiF - p * p);
  return cc;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateLambertSphere::transformFromLambertSquareToSphere(SharedVertexList* verts)
{
  size_t numVerts = verts->getNumberOfTuples();

  LambertUtilities::Hemisphere hemisphere = LambertUtilities::Hemisphere::North;
  if(getHemisphere() == 0)
  {
    hemisphere = LambertUtilities::Hemisphere::North;
  }
  else if(getHemisphere() == 1)
  {
    hemisphere = LambertUtilities::Hemisphere::South;
  }
  for(size_t v = 0; v < numVerts; v++)
  {
    float* vert = verts->getTuplePointer(v);
    int32_t error = LambertUtilities::LambertSquareVertToSphereVert(vert, hemisphere);
    if(error < 0)
    {
      QString msg;
      QTextStream ss(&msg);
      ss << "Error calculating sphere vertex from Lambert Square. Vertex ID=" << v;
      ss << " with value (" << vert[0] << ", " << vert[1] << ", " << vert[2] << ")";
      setErrorCondition(-99000, msg);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CreateLambertSphere::newFilterInstance(bool copyFilterParameters) const
{
  CreateLambertSphere::Pointer filter = CreateLambertSphere::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateLambertSphere::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateLambertSphere::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateLambertSphere::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateLambertSphere::getGroupName() const
{
  return SIMPL::FilterGroups::SurfaceMeshingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid CreateLambertSphere::getUuid() const
{
  return QUuid("{c4398303-db7d-506e-81ea-08f253895ccb}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateLambertSphere::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::GenerationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString CreateLambertSphere::getHumanLabel() const
{
  return "Create Sphere (Lambert Projection Technique)";
}

// -----------------------------------------------------------------------------
CreateLambertSphere::Pointer CreateLambertSphere::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<CreateLambertSphere> CreateLambertSphere::New()
{
  struct make_shared_enabler : public CreateLambertSphere
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString CreateLambertSphere::getNameOfClass() const
{
  return QString("CreateLambertSphere");
}

// -----------------------------------------------------------------------------
QString CreateLambertSphere::ClassName()
{
  return QString("CreateLambertSphere");
}

// -----------------------------------------------------------------------------
void CreateLambertSphere::setHemisphere(int value)
{
  m_Hemisphere = value;
}

// -----------------------------------------------------------------------------
int CreateLambertSphere::getHemisphere() const
{
  return m_Hemisphere;
}

// -----------------------------------------------------------------------------
void CreateLambertSphere::setImageDataArrayPath(const DataArrayPath& value)
{
  m_ImageDataArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CreateLambertSphere::getImageDataArrayPath() const
{
  return m_ImageDataArrayPath;
}

// -----------------------------------------------------------------------------
void CreateLambertSphere::setQuadDataContainerName(const DataArrayPath& value)
{
  m_QuadDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CreateLambertSphere::getQuadDataContainerName() const
{
  return m_QuadDataContainerName;
}

// -----------------------------------------------------------------------------
void CreateLambertSphere::setTriangleDataContainerName(const DataArrayPath& value)
{
  m_TriangleDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CreateLambertSphere::getTriangleDataContainerName() const
{
  return m_TriangleDataContainerName;
}

// -----------------------------------------------------------------------------
void CreateLambertSphere::setEdgeDataContainerName(const DataArrayPath& value)
{
  m_EdgeDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CreateLambertSphere::getEdgeDataContainerName() const
{
  return m_EdgeDataContainerName;
}

// -----------------------------------------------------------------------------
void CreateLambertSphere::setVertexDataContainerName(const DataArrayPath& value)
{
  m_VertexDataContainerName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath CreateLambertSphere::getVertexDataContainerName() const
{
  return m_VertexDataContainerName;
}

// -----------------------------------------------------------------------------
void CreateLambertSphere::setVertexAttributeMatrixName(const QString& value)
{
  m_VertexAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString CreateLambertSphere::getVertexAttributeMatrixName() const
{
  return m_VertexAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void CreateLambertSphere::setEdgeAttributeMatrixName(const QString& value)
{
  m_EdgeAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString CreateLambertSphere::getEdgeAttributeMatrixName() const
{
  return m_EdgeAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void CreateLambertSphere::setFaceAttributeMatrixName(const QString& value)
{
  m_FaceAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString CreateLambertSphere::getFaceAttributeMatrixName() const
{
  return m_FaceAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void CreateLambertSphere::setCreateVertexGeometry(bool value)
{
  m_CreateVertexGeometry = value;
}

// -----------------------------------------------------------------------------
bool CreateLambertSphere::getCreateVertexGeometry() const
{
  return m_CreateVertexGeometry;
}

// -----------------------------------------------------------------------------
void CreateLambertSphere::setCreateEdgeGeometry(bool value)
{
  m_CreateEdgeGeometry = value;
}

// -----------------------------------------------------------------------------
bool CreateLambertSphere::getCreateEdgeGeometry() const
{
  return m_CreateEdgeGeometry;
}

// -----------------------------------------------------------------------------
void CreateLambertSphere::setCreateTriangleGeometry(bool value)
{
  m_CreateTriangleGeometry = value;
}

// -----------------------------------------------------------------------------
bool CreateLambertSphere::getCreateTriangleGeometry() const
{
  return m_CreateTriangleGeometry;
}

// -----------------------------------------------------------------------------
void CreateLambertSphere::setCreateQuadGeometry(bool value)
{
  m_CreateQuadGeometry = value;
}

// -----------------------------------------------------------------------------
bool CreateLambertSphere::getCreateQuadGeometry() const
{
  return m_CreateQuadGeometry;
}

// -----------------------------------------------------------------------------
void CreateLambertSphere::setUseExistingImage(bool value)
{
  m_UseExistingImage = value;
}

// -----------------------------------------------------------------------------
bool CreateLambertSphere::getUseExistingImage() const
{
  return m_UseExistingImage;
}
