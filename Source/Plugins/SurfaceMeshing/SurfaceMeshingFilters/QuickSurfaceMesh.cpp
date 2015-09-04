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


#include "QuickSurfaceMesh.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"

#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"

#define QSM_GETCOORD(index, res, coord, origin)\
  coord = float((float(index)*float(res)) + float(origin));\

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  QuickSurfaceMesh::QuickSurfaceMesh() :
    AbstractFilter(),
    m_SurfaceDataContainerName(DREAM3D::Defaults::TriangleDataContainerName),
    m_VertexAttributeMatrixName(DREAM3D::Defaults::VertexAttributeMatrixName),
    m_FaceAttributeMatrixName(DREAM3D::Defaults::FaceAttributeMatrixName),
    m_TransferPhaseId(false),
    m_FeatureIdsArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::FeatureIds),
    m_CellPhasesArrayPath(DREAM3D::Defaults::ImageDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::Phases),
    m_FaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
    m_NodeTypesArrayName(DREAM3D::VertexData::SurfaceMeshNodeType),
    m_FacePhasesArrayName(DREAM3D::FaceData::SurfaceMeshFacePhases),
    m_FeatureIds(NULL),
    m_CellPhases(NULL),
    m_FaceLabels(NULL),
    m_NodeTypes(NULL),
    m_FacePhases(NULL)
  {

    setupFilterParameters();
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  QuickSurfaceMesh::~QuickSurfaceMesh()
  {
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  void QuickSurfaceMesh::setupFilterParameters()
  {
    QVector<FilterParameter::Pointer> parameters;
    QStringList linkedProps;
    linkedProps << "CellPhasesArrayPath" << "FacePhasesArrayName";
    parameters.push_back(LinkedBooleanFilterParameter::New("Transfer Phase Id", "TransferPhaseId", getTransferPhaseId(), linkedProps, FilterParameter::Parameter));
    parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
    {
      DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixType::Cell, DREAM3D::GeometryType::ImageGeometry);
      parameters.push_back(DataArraySelectionFilterParameter::New("Feature Ids", "FeatureIdsArrayPath", getFeatureIdsArrayPath(), FilterParameter::RequiredArray, req));
    }
    {
      DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixType::Cell, DREAM3D::GeometryType::ImageGeometry);
      parameters.push_back(DataArraySelectionFilterParameter::New("Phases", "CellPhasesArrayPath", getCellPhasesArrayPath(), FilterParameter::RequiredArray, req));
    }

    parameters.push_back(StringFilterParameter::New("Data Container", "SurfaceDataContainerName", getSurfaceDataContainerName(), FilterParameter::CreatedArray));
    parameters.push_back(SeparatorFilterParameter::New("Vertex Data", FilterParameter::CreatedArray));
    parameters.push_back(StringFilterParameter::New("Vertex Attribute Matrix", "VertexAttributeMatrixName", getVertexAttributeMatrixName(), FilterParameter::CreatedArray));
    parameters.push_back(StringFilterParameter::New("Node Types", "NodeTypesArrayName", getNodeTypesArrayName(), FilterParameter::CreatedArray));
    parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::CreatedArray));
    parameters.push_back(StringFilterParameter::New("Face Attribute Matrix", "FaceAttributeMatrixName", getFaceAttributeMatrixName(), FilterParameter::CreatedArray));
    parameters.push_back(StringFilterParameter::New("Face Labels", "FaceLabelsArrayName", getFaceLabelsArrayName(), FilterParameter::CreatedArray));
    parameters.push_back(StringFilterParameter::New("Face Phases", "FacePhasesArrayName", getFacePhasesArrayName(), FilterParameter::CreatedArray));
    setFilterParameters(parameters);
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  void QuickSurfaceMesh::readFilterParameters(AbstractFilterParametersReader* reader, int index)
  {
    reader->openFilterGroup(this, index);
    setSurfaceDataContainerName(reader->readString("SurfaceDataContainerName", getSurfaceDataContainerName() ) );
    setVertexAttributeMatrixName(reader->readString("VertexAttributeMatrixName", getVertexAttributeMatrixName() ) );
    setFaceAttributeMatrixName(reader->readString("FaceAttributeMatrixName", getFaceAttributeMatrixName() ) );
    setFacePhasesArrayName(reader->readString("FacePhasesArrayName", getFacePhasesArrayName() ) );
    setNodeTypesArrayName(reader->readString("NodeTypesArrayName", getNodeTypesArrayName() ) );
    setFaceLabelsArrayName(reader->readString("FaceLabelsArrayName", getFaceLabelsArrayName() ) );
    setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath() ) );
    setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
    setTransferPhaseId(reader->readValue("TransferPhaseId", getTransferPhaseId()));
    reader->closeFilterGroup();
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  int QuickSurfaceMesh::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
  {
    writer->openFilterGroup(this, index);
    SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
    SIMPL_FILTER_WRITE_PARAMETER(SurfaceDataContainerName)
    SIMPL_FILTER_WRITE_PARAMETER(VertexAttributeMatrixName)
    SIMPL_FILTER_WRITE_PARAMETER(FaceAttributeMatrixName)
    SIMPL_FILTER_WRITE_PARAMETER(FacePhasesArrayName)
    SIMPL_FILTER_WRITE_PARAMETER(NodeTypesArrayName)
    SIMPL_FILTER_WRITE_PARAMETER(FaceLabelsArrayName)
    SIMPL_FILTER_WRITE_PARAMETER(CellPhasesArrayPath)
    SIMPL_FILTER_WRITE_PARAMETER(FeatureIdsArrayPath)
    SIMPL_FILTER_WRITE_PARAMETER(TransferPhaseId)
    writer->closeFilterGroup();
    return ++index; // we want to return the next index that was just written to
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  void QuickSurfaceMesh::updateVertexInstancePointers()
  {
    setErrorCondition(0);
    if( NULL != m_NodeTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_NodeTypes = m_NodeTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  void QuickSurfaceMesh::updateFaceInstancePointers()
  {
    setErrorCondition(0);
    if( NULL != m_FaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_FaceLabels = m_FaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(m_TransferPhaseId == true)
    {
      if( NULL != m_FacePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
      { m_FacePhases = m_FacePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    }
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  void QuickSurfaceMesh::dataCheck()
  {
    setErrorCondition(0);
    DataArrayPath tempPath;

    getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

    QVector<DataArrayPath> dataArrayPaths;

    QVector<size_t> cDims(1, 1);
    m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getFeatureIdsArrayPath()); }

    if(m_TransferPhaseId == true)
    {
      m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
      if( NULL != m_CellPhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
      { m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
      if(getErrorCondition() >= 0) { dataArrayPaths.push_back(getCellPhasesArrayPath()); }
    }

    getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);

    // Create a SufaceMesh Data Container with Faces, Vertices, Feature Labels and optionally Phase labels
    DataContainer::Pointer sm = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getSurfaceDataContainerName());
    if(getErrorCondition() < 0) { return; }

    QVector<size_t> tDims(1, 0);
    sm->createNonPrereqAttributeMatrix<AbstractFilter>(this, getVertexAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Vertex);
    sm->createNonPrereqAttributeMatrix<AbstractFilter>(this, getFaceAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Face);

    // Create a Triangle Geometry
    SharedVertexList::Pointer vertices = TriangleGeom::CreateSharedVertexList(0);
    TriangleGeom::Pointer triangleGeom = TriangleGeom::CreateGeometry(0, vertices, DREAM3D::Geometry::TriangleGeometry);
    sm->setGeometry(triangleGeom);

    cDims[0] = 2;
    tempPath.update(getSurfaceDataContainerName(), getFaceAttributeMatrixName(), getFaceLabelsArrayName() );
    m_FaceLabelsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_FaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_FaceLabels = m_FaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

    cDims[0] = 1;
    tempPath.update(getSurfaceDataContainerName(), getVertexAttributeMatrixName(), getNodeTypesArrayName() );
    m_NodeTypesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_NodeTypesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_NodeTypes = m_NodeTypesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

    if (m_TransferPhaseId == true)
    {
      cDims[0] = 2;
      tempPath.update(getSurfaceDataContainerName(), getFaceAttributeMatrixName(), getFacePhasesArrayName() );
      m_FacePhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
      if( NULL != m_FacePhasesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
      { m_FacePhases = m_FacePhasesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
    }
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  void QuickSurfaceMesh::preflight()
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
  void QuickSurfaceMesh::execute()
  {
    setErrorCondition(0);
    dataCheck();
    if(getErrorCondition() < 0) { return; }

    DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
    DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceDataContainerName());

    float m_OriginX = 0.0f, m_OriginY = 0.0f, m_OriginZ = 0.0f;
    m->getGeometryAs<ImageGeom>()->getOrigin(m_OriginX, m_OriginY, m_OriginZ);

    size_t udims[3] = { 0, 0, 0 };
    m->getGeometryAs<ImageGeom>()->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
    typedef int32_t DimType;
#else
    typedef int64_t DimType;
#endif
    DimType dims[3] =
    {
      static_cast<DimType>(udims[0]),
      static_cast<DimType>(udims[1]),
      static_cast<DimType>(udims[2]),
    };

    DimType xP = dims[0];
    DimType yP = dims[1];
    DimType zP = dims[2];
    float xRes = m->getGeometryAs<ImageGeom>()->getXRes();
    float yRes = m->getGeometryAs<ImageGeom>()->getYRes();
    float zRes = m->getGeometryAs<ImageGeom>()->getZRes();

    std::vector<std::set<int32_t> > ownerLists;

    DimType possibleNumNodes = (xP + 1) * (yP + 1) * (zP + 1);
    std::vector<int64_t> m_NodeIds(possibleNumNodes, -1);

    int64_t nodeCount = 0;
    int64_t triangleCount = 0;

    DimType point = 0, neigh1 = 0, neigh2 = 0, neigh3 = 0;

    DimType nodeId1 = 0, nodeId2 = 0, nodeId3 = 0, nodeId4 = 0;

    // first determining which nodes are actually boundary nodes and
    // count number of nodes and triangles that will be created
    for (DimType k = 0; k < zP; k++)
    {
      for (DimType j = 0; j < yP; j++)
      {
        for (DimType i = 0; i < xP; i++)
        {
          point = (k * xP * yP) + (j * xP) + i;
          neigh1 = point + 1;
          neigh2 = point + xP;
          neigh3 = point + (xP * yP);

          if (i == 0)
          {
            nodeId1 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
            if (m_NodeIds[nodeId1] == -1)
            {
              m_NodeIds[nodeId1] = nodeCount;
              nodeCount++;
            }
            nodeId2 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
            if (m_NodeIds[nodeId2] == -1)
            {
              m_NodeIds[nodeId2] = nodeCount;
              nodeCount++;
            }
            nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
            if (m_NodeIds[nodeId3] == -1)
            {
              m_NodeIds[nodeId3] = nodeCount;
              nodeCount++;
            }
            nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
            if (m_NodeIds[nodeId4] == -1)
            {
              m_NodeIds[nodeId4] = nodeCount;
              nodeCount++;
            }
            triangleCount++;
            triangleCount++;
          }
          if (j == 0)
          {
            nodeId1 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
            if (m_NodeIds[nodeId1] == -1)
            {
              m_NodeIds[nodeId1] = nodeCount;
              nodeCount++;
            }
            nodeId2 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
            if (m_NodeIds[nodeId2] == -1)
            {
              m_NodeIds[nodeId2] = nodeCount;
              nodeCount++;
            }
            nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
            if (m_NodeIds[nodeId3] == -1)
            {
              m_NodeIds[nodeId3] = nodeCount;
              nodeCount++;
            }
            nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
            if (m_NodeIds[nodeId4] == -1)
            {
              m_NodeIds[nodeId4] = nodeCount;
              nodeCount++;
            }
            triangleCount++;
            triangleCount++;
          }
          if (k == 0)
          {
            nodeId1 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
            if (m_NodeIds[nodeId1] == -1)
            {
              m_NodeIds[nodeId1] = nodeCount;
              nodeCount++;
            }
            nodeId2 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
            if (m_NodeIds[nodeId2] == -1)
            {
              m_NodeIds[nodeId2] = nodeCount;
              nodeCount++;
            }
            nodeId3 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
            if (m_NodeIds[nodeId3] == -1)
            {
              m_NodeIds[nodeId3] = nodeCount;
              nodeCount++;
            }
            nodeId4 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
            if (m_NodeIds[nodeId4] == -1)
            {
              m_NodeIds[nodeId4] = nodeCount;
              nodeCount++;
            }
            triangleCount++;
            triangleCount++;
          }
          if (i == (xP - 1))
          {
            nodeId1 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
            if (m_NodeIds[nodeId1] == -1)
            {
              m_NodeIds[nodeId1] = nodeCount;
              nodeCount++;
            }
            nodeId2 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
            if (m_NodeIds[nodeId2] == -1)
            {
              m_NodeIds[nodeId2] = nodeCount;
              nodeCount++;
            }
            nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
            if (m_NodeIds[nodeId3] == -1)
            {
              m_NodeIds[nodeId3] = nodeCount;
              nodeCount++;
            }
            nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
            if (m_NodeIds[nodeId4] == -1)
            {
              m_NodeIds[nodeId4] = nodeCount;
              nodeCount++;
            }
            triangleCount++;
            triangleCount++;
          }
          else if (m_FeatureIds[point] != m_FeatureIds[neigh1])
          {
            nodeId1 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
            if (m_NodeIds[nodeId1] == -1)
            {
              m_NodeIds[nodeId1] = nodeCount;
              nodeCount++;
            }
            nodeId2 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
            if (m_NodeIds[nodeId2] == -1)
            {
              m_NodeIds[nodeId2] = nodeCount;
              nodeCount++;
            }
            nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
            if (m_NodeIds[nodeId3] == -1)
            {
              m_NodeIds[nodeId3] = nodeCount;
              nodeCount++;
            }
            nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
            if (m_NodeIds[nodeId4] == -1)
            {
              m_NodeIds[nodeId4] = nodeCount;
              nodeCount++;
            }
            triangleCount++;
            triangleCount++;
          }
          if (j == (yP - 1))
          {
            nodeId1 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
            if (m_NodeIds[nodeId1] == -1)
            {
              m_NodeIds[nodeId1] = nodeCount;
              nodeCount++;
            }
            nodeId2 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
            if (m_NodeIds[nodeId2] == -1)
            {
              m_NodeIds[nodeId2] = nodeCount;
              nodeCount++;
            }
            nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
            if (m_NodeIds[nodeId3] == -1)
            {
              m_NodeIds[nodeId3] = nodeCount;
              nodeCount++;
            }
            nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
            if (m_NodeIds[nodeId4] == -1)
            {
              m_NodeIds[nodeId4] = nodeCount;
              nodeCount++;
            }
            triangleCount++;
            triangleCount++;
          }
          else if (m_FeatureIds[point] != m_FeatureIds[neigh2])
          {
            nodeId1 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
            if (m_NodeIds[nodeId1] == -1)
            {
              m_NodeIds[nodeId1] = nodeCount;
              nodeCount++;
            }
            nodeId2 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
            if (m_NodeIds[nodeId2] == -1)
            {
              m_NodeIds[nodeId2] = nodeCount;
              nodeCount++;
            }
            nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
            if (m_NodeIds[nodeId3] == -1)
            {
              m_NodeIds[nodeId3] = nodeCount;
              nodeCount++;
            }
            nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
            if (m_NodeIds[nodeId4] == -1)
            {
              m_NodeIds[nodeId4] = nodeCount;
              nodeCount++;
            }
            triangleCount++;
            triangleCount++;
          }
          if (k == (zP - 1))
          {
            nodeId1 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
            if (m_NodeIds[nodeId1] == -1)
            {
              m_NodeIds[nodeId1] = nodeCount;
              nodeCount++;
            }
            nodeId2 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
            if (m_NodeIds[nodeId2] == -1)
            {
              m_NodeIds[nodeId2] = nodeCount;
              nodeCount++;
            }
            nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
            if (m_NodeIds[nodeId3] == -1)
            {
              m_NodeIds[nodeId3] = nodeCount;
              nodeCount++;
            }
            nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
            if (m_NodeIds[nodeId4] == -1)
            {
              m_NodeIds[nodeId4] = nodeCount;
              nodeCount++;
            }
            triangleCount++;
            triangleCount++;
          }
          else if(k < zP - 1 && m_FeatureIds[point] != m_FeatureIds[neigh3])
          {
            nodeId1 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
            if (m_NodeIds[nodeId1] == -1)
            {
              m_NodeIds[nodeId1] = nodeCount;
              nodeCount++;
            }
            nodeId2 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
            if (m_NodeIds[nodeId2] == -1)
            {
              m_NodeIds[nodeId2] = nodeCount;
              nodeCount++;
            }
            nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
            if (m_NodeIds[nodeId3] == -1)
            {
              m_NodeIds[nodeId3] = nodeCount;
              nodeCount++;
            }
            nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
            if (m_NodeIds[nodeId4] == -1)
            {
              m_NodeIds[nodeId4] = nodeCount;
              nodeCount++;
            }
            triangleCount++;
            triangleCount++;
          }
        }
      }
    }

    // now create node and triangle arrays knowing the number that will be needed
    TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();
    triangleGeom->resizeTriList(triangleCount);
    triangleGeom->resizeVertexList(nodeCount);

    float* vertex = triangleGeom->getVertexPointer(0);
    int64_t* triangle = triangleGeom->getTriPointer(0);


    QVector<size_t> tDims(1, nodeCount);
    sm->getAttributeMatrix(getVertexAttributeMatrixName())->resizeAttributeArrays(tDims);
    tDims[0] = triangleCount;
    sm->getAttributeMatrix(getFaceAttributeMatrixName())->resizeAttributeArrays(tDims);

    updateVertexInstancePointers();
    updateFaceInstancePointers();

    ownerLists.resize(nodeCount);

    // Cycle through again assigning coordinates to each node and assigning node numbers and feature labels to each triangle
    DimType triangleIndex = 0;
    for (DimType k = 0; k < zP; k++)
    {
      for (DimType j = 0; j < yP; j++)
      {
        for (DimType i = 0; i < xP; i++)
        {
          point = (k * xP * yP) + (j * xP) + i;
          neigh1 = point + 1;
          neigh2 = point + xP;
          neigh3 = point + (xP * yP);

          if (i == 0)
          {
            nodeId1 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
            QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId1] * 3 + 0], m_OriginX);
            QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId1] * 3 + 1], m_OriginY);
            QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId1] * 3 + 2], m_OriginZ);

            nodeId2 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
            QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId2] * 3 + 0], m_OriginX);
            QSM_GETCOORD((j + 1), yRes, vertex[m_NodeIds[nodeId2] * 3 + 1], m_OriginY);
            QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId2] * 3 + 2], m_OriginZ);

            nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
            QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId3] * 3 + 0], m_OriginX);
            QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId3] * 3 + 1], m_OriginY);
            QSM_GETCOORD((k + 1), zRes, vertex[m_NodeIds[nodeId3] * 3 + 2], m_OriginZ);

            nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
            QSM_GETCOORD((i + 1), xRes, vertex[m_NodeIds[nodeId4] * 3 + 0], m_OriginX);
            QSM_GETCOORD((j + 1), yRes, vertex[m_NodeIds[nodeId4] * 3 + 1], m_OriginY);
            QSM_GETCOORD((k + 1), zRes, vertex[m_NodeIds[nodeId4] * 3 + 2], m_OriginZ);

            triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId1];
            triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId2];
            triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId3];
            m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
            m_FaceLabels[triangleIndex * 2 + 1] = -1;
            if (m_TransferPhaseId == true) { m_FacePhases[triangleIndex * 2] = m_CellPhases[point]; m_FacePhases[triangleIndex * 2 + 1] = 0;}
            triangleIndex++;

            triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId2];
            triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId4];
            triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId3];
            m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
            m_FaceLabels[triangleIndex * 2 + 1] = -1;
            if (m_TransferPhaseId == true) { m_FacePhases[triangleIndex * 2] = m_CellPhases[point]; m_FacePhases[triangleIndex * 2 + 1] = 0;}
            triangleIndex++;

            ownerLists[m_NodeIds[nodeId1]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId1]].insert(-1);
            ownerLists[m_NodeIds[nodeId2]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId2]].insert(-1);
            ownerLists[m_NodeIds[nodeId3]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId3]].insert(-1);
            ownerLists[m_NodeIds[nodeId4]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId4]].insert(-1);
          }
          if (j == 0)
          {
            nodeId1 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
            QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId1] * 3 + 0], m_OriginX);
            QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId1] * 3 + 1], m_OriginY);
            QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId1] * 3 + 2], m_OriginZ);

            nodeId2 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
            QSM_GETCOORD((i + 1), xRes, vertex[m_NodeIds[nodeId2] * 3 + 0], m_OriginX);
            QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId2] * 3 + 1], m_OriginY);
            QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId2] * 3 + 2], m_OriginZ);

            nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
            QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId3] * 3 + 0], m_OriginX);
            QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId3] * 3 + 1], m_OriginY);
            QSM_GETCOORD((k + 1), zRes, vertex[m_NodeIds[nodeId3] * 3 + 2], m_OriginZ);

            nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
            QSM_GETCOORD((i + 1), xRes, vertex[m_NodeIds[nodeId4] * 3 + 0], m_OriginX);
            QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId4] * 3 + 1], m_OriginY);
            QSM_GETCOORD((k + 1), zRes, vertex[m_NodeIds[nodeId4] * 3 + 2], m_OriginZ);

            triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId1];
            triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId3];
            triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId2];
            m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
            m_FaceLabels[triangleIndex * 2 + 1] = -1;
            if (m_TransferPhaseId == true) { m_FacePhases[triangleIndex * 2] = m_CellPhases[point]; m_FacePhases[triangleIndex * 2 + 1] = 0;}
            triangleIndex++;

            triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId2];
            triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId3];
            triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId4];
            m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
            m_FaceLabels[triangleIndex * 2 + 1] = -1;
            if (m_TransferPhaseId == true) { m_FacePhases[triangleIndex * 2] = m_CellPhases[point]; m_FacePhases[triangleIndex * 2 + 1] = 0;}
            triangleIndex++;

            ownerLists[m_NodeIds[nodeId1]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId1]].insert(-1);
            ownerLists[m_NodeIds[nodeId2]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId2]].insert(-1);
            ownerLists[m_NodeIds[nodeId3]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId3]].insert(-1);
            ownerLists[m_NodeIds[nodeId4]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId4]].insert(-1);
          }
          if (k == 0)
          {
            nodeId1 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
            QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId1] * 3 + 0], m_OriginX);
            QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId1] * 3 + 1], m_OriginY);
            QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId1] * 3 + 2], m_OriginZ);

            nodeId2 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
            QSM_GETCOORD((i + 1), xRes, vertex[m_NodeIds[nodeId2] * 3 + 0], m_OriginX);
            QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId2] * 3 + 1], m_OriginY);
            QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId2] * 3 + 2], m_OriginZ);

            nodeId3 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
            QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId3] * 3 + 0], m_OriginX);
            QSM_GETCOORD((j + 1), yRes, vertex[m_NodeIds[nodeId3] * 3 + 1], m_OriginY);
            QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId3] * 3 + 2], m_OriginZ);

            nodeId4 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
            QSM_GETCOORD((i + 1), xRes, vertex[m_NodeIds[nodeId4] * 3 + 0], m_OriginX);
            QSM_GETCOORD((j + 1), yRes, vertex[m_NodeIds[nodeId4] * 3 + 1], m_OriginY);
            QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId4] * 3 + 2], m_OriginZ);

            triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId1];
            triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId2];
            triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId3];
            m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
            m_FaceLabels[triangleIndex * 2 + 1] = -1;
            if (m_TransferPhaseId == true) { m_FacePhases[triangleIndex * 2] = m_CellPhases[point]; m_FacePhases[triangleIndex * 2 + 1] = 0;}
            triangleIndex++;

            triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId2];
            triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId4];
            triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId3];
            m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
            m_FaceLabels[triangleIndex * 2 + 1] = -1;
            if (m_TransferPhaseId == true) { m_FacePhases[triangleIndex * 2] = m_CellPhases[point]; m_FacePhases[triangleIndex * 2 + 1] = 0;}
            triangleIndex++;

            ownerLists[m_NodeIds[nodeId1]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId1]].insert(-1);
            ownerLists[m_NodeIds[nodeId2]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId2]].insert(-1);
            ownerLists[m_NodeIds[nodeId3]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId3]].insert(-1);
            ownerLists[m_NodeIds[nodeId4]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId4]].insert(-1);
          }
          if (i == (xP - 1))
          {
            nodeId1 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
            QSM_GETCOORD((i + 1), xRes, vertex[m_NodeIds[nodeId1] * 3 + 0], m_OriginX);
            QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId1] * 3 + 1], m_OriginY);
            QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId1] * 3 + 2], m_OriginZ);

            nodeId2 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
            QSM_GETCOORD((i + 1), xRes, vertex[m_NodeIds[nodeId2] * 3 + 0], m_OriginX);
            QSM_GETCOORD((j + 1), yRes, vertex[m_NodeIds[nodeId2] * 3 + 1], m_OriginY);
            QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId2] * 3 + 2], m_OriginZ);

            nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
            QSM_GETCOORD((i + 1), xRes, vertex[m_NodeIds[nodeId3] * 3 + 0], m_OriginX);
            QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId3] * 3 + 1], m_OriginY);
            QSM_GETCOORD((k + 1), zRes, vertex[m_NodeIds[nodeId3] * 3 + 2], m_OriginZ);

            nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
            QSM_GETCOORD((i + 1), xRes, vertex[m_NodeIds[nodeId4] * 3 + 0], m_OriginX);
            QSM_GETCOORD((j + 1), yRes, vertex[m_NodeIds[nodeId4] * 3 + 1], m_OriginY);
            QSM_GETCOORD((k + 1), zRes, vertex[m_NodeIds[nodeId4] * 3 + 2], m_OriginZ);

            triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId3];
            triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId2];
            triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId1];
            m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
            m_FaceLabels[triangleIndex * 2 + 1] = -1;
            if (m_TransferPhaseId == true) { m_FacePhases[triangleIndex * 2] = m_CellPhases[point]; m_FacePhases[triangleIndex * 2 + 1] = 0;}
            triangleIndex++;

            triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId3];
            triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId4];
            triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId2];
            m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
            m_FaceLabels[triangleIndex * 2 + 1] = -1;
            if (m_TransferPhaseId == true) { m_FacePhases[triangleIndex * 2] = m_CellPhases[point]; m_FacePhases[triangleIndex * 2 + 1] = 0;}
            triangleIndex++;

            ownerLists[m_NodeIds[nodeId1]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId1]].insert(-1);
            ownerLists[m_NodeIds[nodeId2]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId2]].insert(-1);
            ownerLists[m_NodeIds[nodeId3]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId3]].insert(-1);
            ownerLists[m_NodeIds[nodeId4]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId4]].insert(-1);
          }
          else if (m_FeatureIds[point] != m_FeatureIds[neigh1])
          {
            nodeId1 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
            QSM_GETCOORD((i + 1), xRes, vertex[m_NodeIds[nodeId1] * 3 + 0], m_OriginX);
            QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId1] * 3 + 1], m_OriginY);
            QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId1] * 3 + 2], m_OriginZ);

            nodeId2 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
            QSM_GETCOORD((i + 1), xRes, vertex[m_NodeIds[nodeId2] * 3 + 0], m_OriginX);
            QSM_GETCOORD((j + 1), yRes, vertex[m_NodeIds[nodeId2] * 3 + 1], m_OriginY);
            QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId2] * 3 + 2], m_OriginZ);

            nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
            QSM_GETCOORD((i + 1), xRes, vertex[m_NodeIds[nodeId3] * 3 + 0], m_OriginX);
            QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId3] * 3 + 1], m_OriginY);
            QSM_GETCOORD((k + 1), zRes, vertex[m_NodeIds[nodeId3] * 3 + 2], m_OriginZ);

            nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
            QSM_GETCOORD((i + 1), xRes, vertex[m_NodeIds[nodeId4] * 3 + 0], m_OriginX);
            QSM_GETCOORD((j + 1), yRes, vertex[m_NodeIds[nodeId4] * 3 + 1], m_OriginY);
            QSM_GETCOORD((k + 1), zRes, vertex[m_NodeIds[nodeId4] * 3 + 2], m_OriginZ);

            triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId1];
            triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId2];
            triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId3];
            m_FaceLabels[triangleIndex * 2] = m_FeatureIds[neigh1];
            m_FaceLabels[triangleIndex * 2 + 1] = m_FeatureIds[point];
            if (m_TransferPhaseId == true) { m_FacePhases[triangleIndex * 2] = m_CellPhases[neigh1]; m_FacePhases[triangleIndex * 2 + 1] = m_CellPhases[point];}
            triangleIndex++;

            triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId2];
            triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId4];
            triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId3];
            m_FaceLabels[triangleIndex * 2] = m_FeatureIds[neigh1];
            m_FaceLabels[triangleIndex * 2 + 1] = m_FeatureIds[point];
            if (m_TransferPhaseId == true) { m_FacePhases[triangleIndex * 2] = m_CellPhases[neigh1]; m_FacePhases[triangleIndex * 2 + 1] = m_CellPhases[point];}
            triangleIndex++;

            ownerLists[m_NodeIds[nodeId1]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId1]].insert(m_FeatureIds[neigh1]);
            ownerLists[m_NodeIds[nodeId2]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId2]].insert(m_FeatureIds[neigh1]);
            ownerLists[m_NodeIds[nodeId3]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId3]].insert(m_FeatureIds[neigh1]);
            ownerLists[m_NodeIds[nodeId4]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId4]].insert(m_FeatureIds[neigh1]);
          }
          if (j == (yP - 1))
          {
            nodeId1 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
            QSM_GETCOORD((i + 1), xRes, vertex[m_NodeIds[nodeId1] * 3 + 0], m_OriginX);
            QSM_GETCOORD((j + 1), yRes, vertex[m_NodeIds[nodeId1] * 3 + 1], m_OriginY);
            QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId1] * 3 + 2], m_OriginZ);

            nodeId2 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
            QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId2] * 3 + 0], m_OriginX);
            QSM_GETCOORD((j + 1), yRes, vertex[m_NodeIds[nodeId2] * 3 + 1], m_OriginY);
            QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId2] * 3 + 2], m_OriginZ);

            nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
            QSM_GETCOORD((i + 1), xRes, vertex[m_NodeIds[nodeId3] * 3 + 0], m_OriginX);
            QSM_GETCOORD((j + 1), yRes, vertex[m_NodeIds[nodeId3] * 3 + 1], m_OriginY);
            QSM_GETCOORD((k + 1), zRes, vertex[m_NodeIds[nodeId3] * 3 + 2], m_OriginZ);

            nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
            QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId4] * 3 + 0], m_OriginX);
            QSM_GETCOORD((j + 1), yRes, vertex[m_NodeIds[nodeId4] * 3 + 1], m_OriginY);
            QSM_GETCOORD((k + 1), zRes, vertex[m_NodeIds[nodeId4] * 3 + 2], m_OriginZ);

            triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId3];
            triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId2];
            triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId1];
            m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
            m_FaceLabels[triangleIndex * 2 + 1] = -1;
            if (m_TransferPhaseId == true) { m_FacePhases[triangleIndex * 2] = m_CellPhases[point]; m_FacePhases[triangleIndex * 2 + 1] = 0;}
            triangleIndex++;

            triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId3];
            triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId4];
            triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId2];
            m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
            m_FaceLabels[triangleIndex * 2 + 1] = -1;
            if (m_TransferPhaseId == true) { m_FacePhases[triangleIndex * 2] = m_CellPhases[point]; m_FacePhases[triangleIndex * 2 + 1] = 0;}
            triangleIndex++;

            ownerLists[m_NodeIds[nodeId1]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId1]].insert(-1);
            ownerLists[m_NodeIds[nodeId2]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId2]].insert(-1);
            ownerLists[m_NodeIds[nodeId3]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId3]].insert(-1);
            ownerLists[m_NodeIds[nodeId4]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId4]].insert(-1);
          }
          else if (m_FeatureIds[point] != m_FeatureIds[neigh2])
          {
            nodeId1 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
            QSM_GETCOORD((i + 1), xRes, vertex[m_NodeIds[nodeId1] * 3 + 0], m_OriginX);
            QSM_GETCOORD((j + 1), yRes, vertex[m_NodeIds[nodeId1] * 3 + 1], m_OriginY);
            QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId1] * 3 + 2], m_OriginZ);

            nodeId2 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
            QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId2] * 3 + 0], m_OriginX);
            QSM_GETCOORD((j + 1), yRes, vertex[m_NodeIds[nodeId2] * 3 + 1], m_OriginY);
            QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId2] * 3 + 2], m_OriginZ);

            nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
            QSM_GETCOORD((i + 1), xRes, vertex[m_NodeIds[nodeId3] * 3 + 0], m_OriginX);
            QSM_GETCOORD((j + 1), yRes, vertex[m_NodeIds[nodeId3] * 3 + 1], m_OriginY);
            QSM_GETCOORD((k + 1), zRes, vertex[m_NodeIds[nodeId3] * 3 + 2], m_OriginZ);

            nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
            QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId4] * 3 + 0], m_OriginX);
            QSM_GETCOORD((j + 1), yRes, vertex[m_NodeIds[nodeId4] * 3 + 1], m_OriginY);
            QSM_GETCOORD((k + 1), zRes, vertex[m_NodeIds[nodeId4] * 3 + 2], m_OriginZ);

            triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId1];
            triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId2];
            triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId3];
            m_FaceLabels[triangleIndex * 2] = m_FeatureIds[neigh2];
            m_FaceLabels[triangleIndex * 2 + 1] = m_FeatureIds[point];
            if (m_TransferPhaseId == true) { m_FacePhases[triangleIndex * 2] = m_CellPhases[neigh2]; m_FacePhases[triangleIndex * 2 + 1] = m_CellPhases[point];}
            triangleIndex++;

            triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId2];
            triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId4];
            triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId3];
            m_FaceLabels[triangleIndex * 2] = m_FeatureIds[neigh2];
            m_FaceLabels[triangleIndex * 2 + 1] = m_FeatureIds[point];
            if (m_TransferPhaseId == true) { m_FacePhases[triangleIndex * 2] = m_CellPhases[neigh2]; m_FacePhases[triangleIndex * 2 + 1] = m_CellPhases[point];}
            triangleIndex++;

            ownerLists[m_NodeIds[nodeId1]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId1]].insert(m_FeatureIds[neigh2]);
            ownerLists[m_NodeIds[nodeId2]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId2]].insert(m_FeatureIds[neigh2]);
            ownerLists[m_NodeIds[nodeId3]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId3]].insert(m_FeatureIds[neigh2]);
            ownerLists[m_NodeIds[nodeId4]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId4]].insert(m_FeatureIds[neigh2]);
          }
          if (k == (zP - 1))
          {
            nodeId1 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
            QSM_GETCOORD((i + 1), xRes, vertex[m_NodeIds[nodeId1] * 3 + 0], m_OriginX);
            QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId1] * 3 + 1], m_OriginY);
            QSM_GETCOORD((k + 1), zRes, vertex[m_NodeIds[nodeId1] * 3 + 2], m_OriginZ);

            nodeId2 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
            QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId2] * 3 + 0], m_OriginX);
            QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId2] * 3 + 1], m_OriginY);
            QSM_GETCOORD((k + 1), zRes, vertex[m_NodeIds[nodeId2] * 3 + 2], m_OriginZ);

            nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
            QSM_GETCOORD((i + 1), xRes, vertex[m_NodeIds[nodeId3] * 3 + 0], m_OriginX);
            QSM_GETCOORD((j + 1), yRes, vertex[m_NodeIds[nodeId3] * 3 + 1], m_OriginY);
            QSM_GETCOORD((k + 1), zRes, vertex[m_NodeIds[nodeId3] * 3 + 2], m_OriginZ);

            nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
            QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId4] * 3 + 0], m_OriginX);
            QSM_GETCOORD((j + 1), yRes, vertex[m_NodeIds[nodeId4] * 3 + 1], m_OriginY);
            QSM_GETCOORD((k + 1), zRes, vertex[m_NodeIds[nodeId4] * 3 + 2], m_OriginZ);

            triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId2];
            triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId3];
            triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId1];
            m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
            m_FaceLabels[triangleIndex * 2 + 1] = -1;
            if (m_TransferPhaseId == true) { m_FacePhases[triangleIndex * 2] = m_CellPhases[point]; m_FacePhases[triangleIndex * 2 + 1] = 0;}
            triangleIndex++;

            triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId4];
            triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId3];
            triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId2];
            m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
            m_FaceLabels[triangleIndex * 2 + 1] = -1;
            if (m_TransferPhaseId == true) { m_FacePhases[triangleIndex * 2] = m_CellPhases[point]; m_FacePhases[triangleIndex * 2 + 1] = 0;}
            triangleIndex++;

            ownerLists[m_NodeIds[nodeId1]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId1]].insert(-1);
            ownerLists[m_NodeIds[nodeId2]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId2]].insert(-1);
            ownerLists[m_NodeIds[nodeId3]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId3]].insert(-1);
            ownerLists[m_NodeIds[nodeId4]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId4]].insert(-1);
          }
          else if (m_FeatureIds[point] != m_FeatureIds[neigh3])
          {
            nodeId1 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
            QSM_GETCOORD((i + 1), xRes, vertex[m_NodeIds[nodeId1] * 3 + 0], m_OriginX);
            QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId1] * 3 + 1], m_OriginY);
            QSM_GETCOORD((k + 1), zRes, vertex[m_NodeIds[nodeId1] * 3 + 2], m_OriginZ);

            nodeId2 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
            QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId2] * 3 + 0], m_OriginX);
            QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId2] * 3 + 1], m_OriginY);
            QSM_GETCOORD((k + 1), zRes, vertex[m_NodeIds[nodeId2] * 3 + 2], m_OriginZ);

            nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
            QSM_GETCOORD((i + 1), xRes, vertex[m_NodeIds[nodeId3] * 3 + 0], m_OriginX);
            QSM_GETCOORD((j + 1), yRes, vertex[m_NodeIds[nodeId3] * 3 + 1], m_OriginY);
            QSM_GETCOORD((k + 1), zRes, vertex[m_NodeIds[nodeId3] * 3 + 2], m_OriginZ);

            nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
            QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId4] * 3 + 0], m_OriginX);
            QSM_GETCOORD((j + 1), yRes, vertex[m_NodeIds[nodeId4] * 3 + 1], m_OriginY);
            QSM_GETCOORD((k + 1), zRes, vertex[m_NodeIds[nodeId4] * 3 + 2], m_OriginZ);

            triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId1];
            triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId3];
            triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId2];
            m_FaceLabels[triangleIndex * 2] = m_FeatureIds[neigh3];
            m_FaceLabels[triangleIndex * 2 + 1] = m_FeatureIds[point];
            if (m_TransferPhaseId == true) { m_FacePhases[triangleIndex * 2] = m_CellPhases[neigh3]; m_FacePhases[triangleIndex * 2 + 1] = m_CellPhases[point];}
            triangleIndex++;

            triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId2];
            triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId3];
            triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId4];
            m_FaceLabels[triangleIndex * 2] = m_FeatureIds[neigh3];
            m_FaceLabels[triangleIndex * 2 + 1] = m_FeatureIds[point];
            if (m_TransferPhaseId == true) { m_FacePhases[triangleIndex * 2] = m_CellPhases[neigh3]; m_FacePhases[triangleIndex * 2 + 1] = m_CellPhases[point];}
            triangleIndex++;

            ownerLists[m_NodeIds[nodeId1]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId1]].insert(m_FeatureIds[neigh3]);
            ownerLists[m_NodeIds[nodeId2]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId2]].insert(m_FeatureIds[neigh3]);
            ownerLists[m_NodeIds[nodeId3]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId3]].insert(m_FeatureIds[neigh3]);
            ownerLists[m_NodeIds[nodeId4]].insert(m_FeatureIds[point]);
            ownerLists[m_NodeIds[nodeId4]].insert(m_FeatureIds[neigh3]);
          }
        }
      }
    }

    for (int64_t i = 0; i < nodeCount; i++)
    {
      m_NodeTypes[i] = ownerLists[i].size();
      if (m_NodeTypes[i] > 4) { m_NodeTypes[i] = 4; }
      if (ownerLists[i].find(-1) != ownerLists[i].end()) { m_NodeTypes[i] += 10; }
    }

    notifyStatusMessage(getHumanLabel(), "Complete");
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  AbstractFilter::Pointer QuickSurfaceMesh::newFilterInstance(bool copyFilterParameters)
  {
    QuickSurfaceMesh::Pointer filter = QuickSurfaceMesh::New();
    if(true == copyFilterParameters)
    {
      copyFilterParameterInstanceVariables(filter.get());
    }
    return filter;
  }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  const QString QuickSurfaceMesh::getCompiledLibraryName()
  { return SurfaceMeshingConstants::SurfaceMeshingBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  const QString QuickSurfaceMesh::getGroupName()
  { return DREAM3D::FilterGroups::SurfaceMeshingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  const QString QuickSurfaceMesh::getSubGroupName()
  { return DREAM3D::FilterSubGroups::GenerationFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
  const QString QuickSurfaceMesh::getHumanLabel()
  { return "Quick Surface Mesh"; }
