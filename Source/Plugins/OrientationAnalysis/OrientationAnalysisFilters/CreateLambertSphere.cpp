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

#include <assert.h>
#include <cmath>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataContainerCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/QuadGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"

#include "OrientationLib/Utilities/LambertUtilities.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateLambertSphere::CreateLambertSphere()
: m_Hemisphere(0)
, m_ImageDataArrayPath(DataArrayPath())
, m_QuadDataContainerName(SIMPL::Defaults::QuadDataContainerName)
, m_TriangleDataContainerName(SIMPL::Defaults::TriangleDataContainerName)
, m_EdgeDataContainerName(SIMPL::Defaults::EdgeDataContainerName)
, m_VertexDataContainerName(SIMPL::Defaults::VertexDataContainerName)
, m_VertexAttributeMatrixName(SIMPL::Defaults::VertexAttributeMatrixName)
, m_EdgeAttributeMatrixName(SIMPL::Defaults::EdgeAttributeMatrixName)
, m_FaceAttributeMatrixName(SIMPL::Defaults::FaceAttributeMatrixName)
, m_CreateVertexGeometry(true)
, m_CreateEdgeGeometry(true)
, m_CreateTriangleGeometry(true)
, m_CreateQuadGeometry(true)
, m_UseExistingImage(true)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateLambertSphere::~CreateLambertSphere() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateLambertSphere::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
  ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
  parameter->setHumanLabel("Select Hemisphere to Generate");
  parameter->setPropertyName("Hemisphere");
  parameter->setSetterCallback(SIMPL_BIND_SETTER(CreateLambertSphere, this, Hemisphere));
  parameter->setGetterCallback(SIMPL_BIND_GETTER(CreateLambertSphere, this, Hemisphere));
  QVector<QString> choices;
  choices.push_back("Northern");
  choices.push_back("Southern");
  // choices.push_back("Both");
  parameter->setChoices(choices);
  parameter->setCategory(FilterParameter::Parameter);
  parameters.push_back(parameter);

  
  {
    QStringList linkedProperties = {"VertexDataContainerName", "VertexAttributeMatrixName"};
    parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Create Vertex Geometry", CreateVertexGeometry, FilterParameter::Parameter, CreateLambertSphere, linkedProperties));
    parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Output Vertex DataContainer Name", VertexDataContainerName, FilterParameter::Parameter, CreateLambertSphere));
//    parameters.push_back(SIMPL_NEW_STRING_FP("Vertex Attribute Matrix", VertexAttributeMatrixName, FilterParameter::Parameter, CreateLambertSphere));
  }
  
  {
    QStringList linkedProperties = {"EdgeDataContainerName", "EdgeAttributeMatrixName"};
    parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Create Edge Geometry", CreateEdgeGeometry, FilterParameter::Parameter, CreateLambertSphere, linkedProperties));
    parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Output Edge DataContainer Name", EdgeDataContainerName, FilterParameter::Parameter, CreateLambertSphere));
//    parameters.push_back(SIMPL_NEW_STRING_FP("Edge Attribute Matrix", EdgeAttributeMatrixName, FilterParameter::Parameter, CreateLambertSphere));
  }
  
  {
    QStringList linkedProperties = {"TriangleDataContainerName"};
    parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Create Triangle Geometry", CreateTriangleGeometry, FilterParameter::Parameter, CreateLambertSphere, linkedProperties));
    parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Output Triangle DataContainer Name", TriangleDataContainerName, FilterParameter::Parameter, CreateLambertSphere));
  }
  
  {
    QStringList linkedProperties = {"QuadDataContainerName", "FaceAttributeMatrixName", "ImageFaceDataArrayName"};
    parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Create Quad Geometry", CreateQuadGeometry, FilterParameter::Parameter, CreateLambertSphere, linkedProperties));
    parameters.push_back(SIMPL_NEW_DC_CREATION_FP("Output Quad DataContainer Name", QuadDataContainerName, FilterParameter::Parameter, CreateLambertSphere));
//    parameters.push_back(SIMPL_NEW_STRING_FP("Quad Attribute Matrix", FaceAttributeMatrixName, FilterParameter::Parameter, CreateLambertSphere));
//    parameters.push_back(SIMPL_NEW_STRING_FP("Quad Face ArrayName", ImageFaceDataArrayName, FilterParameter::Parameter, CreateLambertSphere));
  }
  
   {
    // Option to map an existing image
  //  QStringList linkedProperties = {"ImageDataArrayPath"};
  //  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Use Existing Image", UseExistingImage, FilterParameter::Parameter, CreateLambertSphere, linkedProperties));
 
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt8, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Image Data", ImageDataArrayPath, FilterParameter::RequiredArray, CreateLambertSphere, req));
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
  setErrorCondition(0);
  setWarningCondition(0);
  initialize();

  if(getHemisphere() < 0 || getHemisphere() > 2)
  {
    setErrorCondition(-99006);
    QString msg("Invalid selection of the Hemisphere value. Valid values are 0 (Northern), 1 (Southern)");
    notifyErrorMessage(getHumanLabel(), msg, getErrorCondition());
    return;
  }
  QVector<size_t> cDims = {1};
  DataContainerArray::Pointer dca = getDataContainerArray();
  
  
  m_VertexDataName = "Vertex_" + getImageDataArrayPath().getDataArrayName();
  m_EdgeDataName = "Edge_" + getImageDataArrayPath().getDataArrayName();
  m_TriangleDataName = "Triangle_" + getImageDataArrayPath().getDataArrayName();
  m_QuadDataName = "Quad_" + getImageDataArrayPath().getDataArrayName();
  
  if(getUseExistingImage())
  {
    m_ImageDataPtr = getDataContainerArray()->getPrereqArrayFromPath<UInt8ArrayType, AbstractFilter>(this, getImageDataArrayPath(), cDims);
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
    setErrorCondition(-99003);
    QString msg("The geometry object was invalid for the image data. Please select a DataContainer that has an Image Geometry or use a filter to create an ImageGeom.");
    notifyErrorMessage(getHumanLabel(), msg, getErrorCondition());
    return;
  }
  size_t imageDims[3] = {0, 0, 0};
  std::tie(imageDims[0], imageDims[1], imageDims[2]) = imageGeom->getDimensions();
  if(imageDims[0] != imageDims[1])
  {
    setErrorCondition(-99004);
    QString msg;
    QTextStream ss(&msg);
    ss << "The input image must be square, i.e., the number of pixels in the X & Y direction must be equal. The current dimensions are";
    ss << " X=" << imageDims[0] << " Y=" << imageDims[1] << " Z=" << imageDims[2];
    notifyErrorMessage(getHumanLabel(), msg, getErrorCondition());
    return;
  }
  if(imageDims[2] != 1)
  {
    setErrorCondition(-99005);
    QString msg;
    QTextStream ss(&msg);
    ss << "The input image must be a single XY Plane image. The current dimensions are";
    ss << " X=" << imageDims[0] << " Y=" << imageDims[1] << " Z=" << imageDims[2];
    notifyErrorMessage(getHumanLabel(), msg, getErrorCondition());
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
  QVector<size_t> vertDims(1, 3);
  m_Vertices = SharedVertexList::CreateArray(numVertices, vertDims, SIMPL::Geometry::SharedVertexList, !getInPreflight());
  m_Vertices->initializeWithZeros();

  // Create a Vertex Geometry
  if(getCreateVertexGeometry())
  {
    DataContainer::Pointer vertDC = dca->createNonPrereqDataContainer<AbstractFilter>(this, getVertexDataContainerName());
    if(getErrorCondition() < 0)
    {
      return;
    }
    
    // Create the Vertex Geometry
    VertexGeom::Pointer vertGeom = VertexGeom::CreateGeometry(m_Vertices, "VertexGeometry");
    vertDC->setGeometry(vertGeom);
    
    QVector<size_t> tDims = {static_cast<size_t>(totalVerts)};
    vertDC->createNonPrereqAttributeMatrix(this, getVertexAttributeMatrixName(), tDims, AttributeMatrix::Type::Vertex);
    
//    QVector<size_t> cDims = {1};
//    DataArrayPath path(getVertexDataContainerName(), getVertexAttributeMatrixName(), m_VertexDataName);
//    m_VertexDataPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, path, 0, cDims);
    //    if(nullptr != m_VertexDataPtr.lock())
    //    {
    //      m_VertexData = m_VertexDataPtr.lock()->getPointer(0);
    //    }
  }

  // Create a Edge Geometry
  if(getCreateEdgeGeometry())
  {
    DataContainer::Pointer edgeDC = dca->createNonPrereqDataContainer<AbstractFilter>(this, getEdgeDataContainerName());
    if(getErrorCondition() < 0)
    {
      return;
    }

    size_t numEdges = ((imageDims[0] + 1) * imageDims[0]) + ((imageDims[1] + 1) * imageDims[1]);

    QVector<size_t> tDims = {static_cast<size_t>(totalVerts)};
    edgeDC->createNonPrereqAttributeMatrix(this, getVertexAttributeMatrixName(), tDims, AttributeMatrix::Type::Vertex);
    tDims[0] = numEdges;
    edgeDC->createNonPrereqAttributeMatrix(this, getEdgeAttributeMatrixName(), tDims, AttributeMatrix::Type::Edge);
    
    // Create Edge Geometry
    QVector<size_t> cDims = {2};
    EdgeGeom::Pointer edgetGeom = EdgeGeom::CreateGeometry(numEdges, m_Vertices, "EdgeGeometry");
    edgeDC->setGeometry(edgetGeom);
    
    cDims[0] = 1;
    DataArrayPath path(getEdgeDataContainerName(), getEdgeAttributeMatrixName(), m_EdgeDataName);
    m_EdgeDataPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, path, 0, cDims);
    if(nullptr != m_EdgeDataPtr.lock())
    {
      m_EdgeData = m_EdgeDataPtr.lock()->getPointer(0);
    }
  }

  // Create a Triangle Geometry
  if(getCreateTriangleGeometry())
  {
    DataContainer::Pointer triangleDC = dca->createNonPrereqDataContainer<AbstractFilter>(this, getTriangleDataContainerName());
    if(getErrorCondition() < 0)
    {
      return;
    }

    QVector<size_t> tDims = {static_cast<size_t>(totalVerts)};
    triangleDC->createNonPrereqAttributeMatrix(this, getVertexAttributeMatrixName(), tDims, AttributeMatrix::Type::Vertex);
    tDims[0] = static_cast<size_t>(totalQuads * 2);
    triangleDC->createNonPrereqAttributeMatrix(this, getFaceAttributeMatrixName(), tDims, AttributeMatrix::Type::Face);
    
    // Create a Triangle Geometry
    size_t numTris = imageDims[0] * imageDims[1] * 2; // Twice the number of Quads
    QVector<size_t> cDims = {3};
    TriangleGeom::Pointer triangleGeom = TriangleGeom::CreateGeometry(numTris, m_Vertices, "Triangle Geometry");
    triangleDC->setGeometry(triangleGeom);

    cDims[0] = 1;
    DataArrayPath path(getTriangleDataContainerName(), getFaceAttributeMatrixName(), m_TriangleDataName);
    m_TriangleFaceDataPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, path, 0, cDims);
    if(nullptr != m_TriangleFaceDataPtr.lock())
    {
      m_TriangleFaceData = m_TriangleFaceDataPtr.lock()->getPointer(0);
    }
  }

  // Create a QuadGeometry
  if(getCreateQuadGeometry())
  {
    DataContainer::Pointer quadDC = dca->createNonPrereqDataContainer<AbstractFilter>(this, getQuadDataContainerName());
    if(getErrorCondition() < 0)
    {
      return;
    }

    QVector<size_t> tDims(1, static_cast<size_t>(totalVerts));
    quadDC->createNonPrereqAttributeMatrix(this, getVertexAttributeMatrixName(), tDims, AttributeMatrix::Type::Vertex);
    tDims[0] = static_cast<size_t>(totalQuads);
    quadDC->createNonPrereqAttributeMatrix(this, getFaceAttributeMatrixName(), tDims, AttributeMatrix::Type::Face);

    // Create a Quad Geometry
    QuadGeom::Pointer quadGeom = QuadGeom::CreateGeometry(totalQuads, m_Vertices, SIMPL::Geometry::QuadGeometry, !getInPreflight());
    quadDC->setGeometry(quadGeom);

    cDims[0] = 1;
    DataArrayPath path(getQuadDataContainerName(), getFaceAttributeMatrixName(), m_QuadDataName);
    m_QuadFaceDataPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, path, 0, cDims);
    if(nullptr != m_QuadFaceDataPtr.lock())
    {
      m_QuadFaceData = m_QuadFaceDataPtr.lock()->getPointer(0);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateLambertSphere::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateLambertSphere::execute()
{
  // 1.253314137315501
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  createVertices();

  createVertexGeometry();
  createEdgeGeometry();
  createTriangleGeometry();
  createQuadGeometry();

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateLambertSphere::createVertices()
{

  // Get the dimensions of the lambert image we are going to map to a sphere.
  ImageGeom::Pointer imageGeom = getDataContainerArray()->getDataContainer(getImageDataArrayPath())->getGeometryAs<ImageGeom>();
  size_t imageDims[3] = {0, 0, 0};
  std::tie(imageDims[0], imageDims[1], imageDims[2]) = imageGeom->getDimensions();

  UInt8ArrayType::Pointer masterPattern =
      getDataContainerArray()->getAttributeMatrix(getImageDataArrayPath())->getAttributeArrayAs<UInt8ArrayType>(getImageDataArrayPath().getDataArrayName());


  float L = static_cast<float>(SIMPLib::Constants::k_SqrtHalfPi);
  float res = (2.0f * L) / imageDims[0];

  // The number of vertices in X & Y is one more than the dims
  int64_t points[3] = {static_cast<int64_t>(imageDims[0] + 1), static_cast<int64_t>(imageDims[1] + 1), 1};
  
  
//  DataContainer::Pointer vertexDC = getDataContainerArray()->getDataContainer(getVertexDataContainerName());
//  m_VertexData = m_VertexDataPtr.lock()->getPointer(0);

  //int64_t iIndex = 0;
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
  size_t imageDims[3] = {0, 0, 0};
  std::tie(imageDims[0], imageDims[1], imageDims[2]) = imageGeom->getDimensions();

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

      int64_t* edge = edges->getTuplePointer(eIndex++);
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
  size_t imageDims[3] = {0, 0, 0};
  std::tie(imageDims[0], imageDims[1], imageDims[2]) = imageGeom->getDimensions();

  UInt8ArrayType::Pointer masterPattern =
      getDataContainerArray()->getAttributeMatrix(getImageDataArrayPath())->getAttributeArrayAs<UInt8ArrayType>(getImageDataArrayPath().getDataArrayName());

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

      int64_t* tri = triangles->getTuplePointer(tIndex);
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

  float L = static_cast<float>(SIMPLib::Constants::k_SqrtHalfPi);

  // Get the dimensions of the lambert image we are going to map to a sphere.
  ImageGeom::Pointer imageGeom = getDataContainerArray()->getDataContainer(getImageDataArrayPath())->getGeometryAs<ImageGeom>();
  size_t imageDims[3] = {0, 0, 0};
  std::tie(imageDims[0], imageDims[1], imageDims[2]) = imageGeom->getDimensions();

  UInt8ArrayType::Pointer masterPattern =
      getDataContainerArray()->getAttributeMatrix(getImageDataArrayPath())->getAttributeArrayAs<UInt8ArrayType>(getImageDataArrayPath().getDataArrayName());

  // The number of vertices in X & Y is one more than the dims
  // int64_t points[3] = {static_cast<int64_t>(imageDims[0] + 1), static_cast<int64_t>(imageDims[1] + 1), 1};

  // now create node and quad arrays knowing the number that will be needed
  DataContainer::Pointer quadDC = getDataContainerArray()->getDataContainer(getQuadDataContainerName());
  QuadGeom::Pointer quadGeom = quadDC->getGeometryAs<QuadGeom>();
  SharedVertexList::Pointer vertices = quadGeom->getVertices();

  float res = (2.0f * L) / imageDims[0];
  imageGeom->setResolution(res, res, res);

  float origin[3] = {-(imageDims[0] * res) / 2.0f, -(imageDims[1] * res) / 2.0f, 0.0f};
  imageGeom->setOrigin(origin);

  size_t vIndex = 0;

  size_t totalQuads = (imageDims[0] * imageDims[1]);
  QVector<size_t> tDims(1, totalQuads);
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

      int64_t* quad = quads->getTuplePointer(qIndex);
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
  float cc = (2.0f * p / SIMPLib::Constants::k_Pif) * std::sqrt(SIMPLib::Constants::k_Pif - p * p);
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
      setErrorCondition(-99000);
      QString msg;
      QTextStream ss(&msg);
      ss << "Error calculating sphere vertex from Lambert Square. Vertex ID=" << v;
      ss << " with value (" << vert[0] << ", " << vert[1] << ", " << vert[2] << ")";
      notifyErrorMessage(getHumanLabel(), msg, getErrorCondition());
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
const QString CreateLambertSphere::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateLambertSphere::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateLambertSphere::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateLambertSphere::getGroupName() const
{
  return SIMPL::FilterGroups::SurfaceMeshingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid CreateLambertSphere::getUuid()
{
  return QUuid("{c4398303-db7d-506e-81ea-08f253895ccb}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateLambertSphere::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::GenerationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateLambertSphere::getHumanLabel() const
{
  return "Create Sphere (Lambert Projection Technique)";
}
