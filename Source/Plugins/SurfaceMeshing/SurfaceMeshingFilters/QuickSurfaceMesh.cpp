/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/MultiDataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"

#include "moc_QuickSurfaceMesh.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuickSurfaceMesh::QuickSurfaceMesh()
: AbstractFilter()
, m_SelectedDataArrayPaths(QVector<DataArrayPath>())
, m_SurfaceDataContainerName(SIMPL::Defaults::TriangleDataContainerName)
, m_VertexAttributeMatrixName(SIMPL::Defaults::VertexAttributeMatrixName)
, m_FaceAttributeMatrixName(SIMPL::Defaults::FaceAttributeMatrixName)
, m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
, m_FaceLabelsArrayName(SIMPL::FaceData::SurfaceMeshFaceLabels)
, m_NodeTypesArrayName(SIMPL::VertexData::SurfaceMeshNodeType)
, m_FeatureAttributeMatrixName(SIMPL::Defaults::FaceFeatureAttributeMatrixName)
, m_FeatureIds(nullptr)
, m_FaceLabels(nullptr)
, m_NodeTypes(nullptr)
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
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Any);
    IGeometry::Types geomTypes = { IGeometry::Type::Image, IGeometry::Type::RectGrid };
    req.dcGeometryTypes = geomTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, QuickSurfaceMesh, req));
  }
  {
    MultiDataArraySelectionFilterParameter::RequirementType req = MultiDataArraySelectionFilterParameter::CreateRequirement(SIMPL::Defaults::AnyPrimitive, SIMPL::Defaults::AnyComponentSize,
                                                                                                                            AttributeMatrix::Type::Cell, IGeometry::Type::Any);
    IGeometry::Types geomTypes = { IGeometry::Type::Image, IGeometry::Type::RectGrid };
    req.dcGeometryTypes = geomTypes;
    parameters.push_back(SIMPL_NEW_MDA_SELECTION_FP("Attribute Arrays to Transfer", SelectedDataArrayPaths, FilterParameter::RequiredArray, QuickSurfaceMesh, req));
  }
  parameters.push_back(SIMPL_NEW_STRING_FP("Data Container", SurfaceDataContainerName, FilterParameter::CreatedArray, QuickSurfaceMesh));
  parameters.push_back(SeparatorFilterParameter::New("Vertex Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Vertex Attribute Matrix", VertexAttributeMatrixName, FilterParameter::CreatedArray, QuickSurfaceMesh));
  parameters.push_back(SIMPL_NEW_STRING_FP("Node Types", NodeTypesArrayName, FilterParameter::CreatedArray, QuickSurfaceMesh));
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Face Attribute Matrix", FaceAttributeMatrixName, FilterParameter::CreatedArray, QuickSurfaceMesh));
  parameters.push_back(SIMPL_NEW_STRING_FP("Face Labels", FaceLabelsArrayName, FilterParameter::CreatedArray, QuickSurfaceMesh));
  parameters.push_back(SeparatorFilterParameter::New("Face Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Face Feature Attribute Matrix", FeatureAttributeMatrixName, FilterParameter::CreatedArray, QuickSurfaceMesh));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuickSurfaceMesh::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedDataArrayPaths(reader->readDataArrayPathVector("SelectedDataArrayPaths", getSelectedDataArrayPaths()));
  setSurfaceDataContainerName(reader->readString("SurfaceDataContainerName", getSurfaceDataContainerName()));
  setVertexAttributeMatrixName(reader->readString("VertexAttributeMatrixName", getVertexAttributeMatrixName()));
  setFaceAttributeMatrixName(reader->readString("FaceAttributeMatrixName", getFaceAttributeMatrixName()));
  setNodeTypesArrayName(reader->readString("NodeTypesArrayName", getNodeTypesArrayName()));
  setFaceLabelsArrayName(reader->readString("FaceLabelsArrayName", getFaceLabelsArrayName()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setFeatureAttributeMatrixName(reader->readString("FeatureAttributeMatrixName", getFeatureAttributeMatrixName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuickSurfaceMesh::updateVertexInstancePointers()
{
  setErrorCondition(0);
  setWarningCondition(0);
  if(nullptr != m_NodeTypesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_NodeTypes = m_NodeTypesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuickSurfaceMesh::updateFaceInstancePointers()
{
  setErrorCondition(0);
  setWarningCondition(0);
  if(nullptr != m_FaceLabelsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FaceLabels = m_FaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
void copyCellArraysToFaceArrays(size_t faceIndex, size_t firstCellIndex, size_t secondCellIndex, IDataArray::Pointer cellArray, IDataArray::Pointer faceArray, bool forceSecondToZero = false)
{
  typename DataArray<T>::Pointer cellPtr = std::dynamic_pointer_cast<DataArray<T>>(cellArray);
  typename DataArray<T>::Pointer facePtr = std::dynamic_pointer_cast<DataArray<T>>(faceArray);

  int32_t numComps = cellPtr->getNumberOfComponents();
  QVector<size_t> cDims = facePtr->getComponentDimensions();

  T* faceTuplePtr = facePtr->getTuplePointer(faceIndex);
  T* firstCellTuplePtr = cellPtr->getTuplePointer(firstCellIndex);
  T* secondCellTuplePtr = cellPtr->getTuplePointer(secondCellIndex);

  ::memcpy(faceTuplePtr, firstCellTuplePtr, sizeof(T) * numComps);
  if(!forceSecondToZero)
  {
    ::memcpy(faceTuplePtr + numComps, secondCellTuplePtr, sizeof(T) * numComps);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuickSurfaceMesh::initialize()
{
  m_SelectedWeakPtrVector.clear();
  m_CreatedWeakPtrVector.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuickSurfaceMesh::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
  initialize();

  DataArrayPath tempPath;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<IGeometryGrid, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  QVector<DataArrayPath> dataArrayPaths;

  QVector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getFeatureIdsArrayPath());
  }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);

  QVector<DataArrayPath> paths = getSelectedDataArrayPaths();

  if(DataArrayPath::ValidateVector(paths) == false)
  {
    setErrorCondition(-11004);
    QString ss = QObject::tr("There are Attribute Arrays selected that are not contained in the same Attribute Matrix. All selected Attribute Arrays must belong to the same Attribute Matrix");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  for(int32_t i = 0; i < paths.count(); i++)
  {
    DataArrayPath path = paths.at(i);
    IDataArray::WeakPointer ptr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, path);
    if(getErrorCondition() >= 0)
    {
      dataArrayPaths.push_back(path);
      m_SelectedWeakPtrVector.push_back(ptr);
    }
  }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);

  // Create a SufaceMesh Data Container with Faces, Vertices, Feature Labels and optionally Phase labels
  DataContainer::Pointer sm = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getSurfaceDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }

  QVector<size_t> tDims(1, 0);
  sm->createNonPrereqAttributeMatrix<AbstractFilter>(this, getVertexAttributeMatrixName(), tDims, AttributeMatrix::Type::Vertex);
  sm->createNonPrereqAttributeMatrix<AbstractFilter>(this, getFaceAttributeMatrixName(), tDims, AttributeMatrix::Type::Face);

  // Create a Triangle Geometry
  SharedVertexList::Pointer vertices = TriangleGeom::CreateSharedVertexList(0);
  TriangleGeom::Pointer triangleGeom = TriangleGeom::CreateGeometry(0, vertices, SIMPL::Geometry::TriangleGeometry, !getInPreflight());
  sm->setGeometry(triangleGeom);

  cDims[0] = 2;
  tempPath.update(getSurfaceDataContainerName(), getFaceAttributeMatrixName(), getFaceLabelsArrayName());
  m_FaceLabelsPtr =
      getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FaceLabelsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FaceLabels = m_FaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 1;
  tempPath.update(getSurfaceDataContainerName(), getVertexAttributeMatrixName(), getNodeTypesArrayName());
  m_NodeTypesPtr =
      getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int8_t>, AbstractFilter>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_NodeTypesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_NodeTypes = m_NodeTypesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  for(int32_t i = 0; i < m_SelectedWeakPtrVector.size(); i++)
  {
    tempPath.update(getSurfaceDataContainerName(), getFaceAttributeMatrixName(), m_SelectedDataArrayPaths[i].getDataArrayName());
    cDims = m_SelectedWeakPtrVector[i].lock()->getComponentDimensions();
    QVector<size_t> faceDims;
    // If the cell array is 1-dimensional, scale the face array accordingly
    if(cDims.size() == 1)
    {
      faceDims.push_back(cDims[0] * 2);
    }
    else
    {
      // If the cell array is multi-dimensional, error out for now because the Xdmf hyperslab output
      // will crash ParaView every time. This requires re-engineering the Xdmdf writing to support
      // the correct "standard" for writing "owners" of an array
      setErrorCondition(-11005);
      QString ss = QObject::tr("Selected Cell Attribute Arrays must have a single component dimension");
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
    m_CreatedWeakPtrVector.push_back(TemplateHelpers::CreateNonPrereqArrayFromArrayType()(this, tempPath, faceDims, m_SelectedWeakPtrVector[i].lock()));
  }

  if(m_SelectedWeakPtrVector.size() != m_CreatedWeakPtrVector.size())
  {
    setErrorCondition(-11006);
    QString ss = QObject::tr("The number of selected Cell Attribute Arrays available does not match the number of Face Attribute Arrays created");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  sm->createNonPrereqAttributeMatrix<AbstractFilter>(this, getFeatureAttributeMatrixName(), tDims, AttributeMatrix::Type::FaceFeature);
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
void QuickSurfaceMesh::getGridCoordinates(IGeometryGrid::Pointer grid, size_t x, size_t y, size_t z, float *coords)
{
  float tmpCoords[3] = {0.0f, 0.0f, 0.0f};
  grid->getPlaneCoords(x, y, z, tmpCoords);
  coords[0] = tmpCoords[0];
  coords[1] = tmpCoords[1];
  coords[2] = tmpCoords[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuickSurfaceMesh::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());
  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceDataContainerName());

  AttributeMatrix::Pointer featAttrMat = sm->getAttributeMatrix(m_FeatureAttributeMatrixName);
  size_t numFeatures = 0;
  size_t numTuples = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  for(size_t i = 0; i < numTuples; i++)
  {
    if(m_FeatureIds[i] > numFeatures) { numFeatures = m_FeatureIds[i]; }
  }

  QVector<size_t> featDims(1, numFeatures + 1);
  featAttrMat->setTupleDimensions(featDims);

  IGeometryGrid::Pointer grid = m->getGeometryAs<IGeometryGrid>();

  size_t udims[3] = {0, 0, 0};
  grid->getDimensions(udims);

  int64_t dims[3] = {
      static_cast<int64_t>(udims[0]), static_cast<int64_t>(udims[1]), static_cast<int64_t>(udims[2]),
  };

  int64_t xP = dims[0];
  int64_t yP = dims[1];
  int64_t zP = dims[2];

  std::vector<std::set<int32_t>> ownerLists;

  int64_t possibleNumNodes = (xP + 1) * (yP + 1) * (zP + 1);
  std::vector<int64_t> m_NodeIds(possibleNumNodes, -1);

  int64_t nodeCount = 0;
  int64_t triangleCount = 0;

  int64_t point = 0, neigh1 = 0, neigh2 = 0, neigh3 = 0;

  int64_t nodeId1 = 0, nodeId2 = 0, nodeId3 = 0, nodeId4 = 0;

  // first determining which nodes are actually boundary nodes and
  // count number of nodes and triangles that will be created
  for(int64_t k = 0; k < zP; k++)
  {
    for(int64_t j = 0; j < yP; j++)
    {
      for(int64_t i = 0; i < xP; i++)
      {
        point = (k * xP * yP) + (j * xP) + i;
        neigh1 = point + 1;
        neigh2 = point + xP;
        neigh3 = point + (xP * yP);

        if(i == 0)
        {
          nodeId1 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
          if(m_NodeIds[nodeId1] == -1)
          {
            m_NodeIds[nodeId1] = nodeCount;
            nodeCount++;
          }
          nodeId2 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
          if(m_NodeIds[nodeId2] == -1)
          {
            m_NodeIds[nodeId2] = nodeCount;
            nodeCount++;
          }
          nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
          if(m_NodeIds[nodeId3] == -1)
          {
            m_NodeIds[nodeId3] = nodeCount;
            nodeCount++;
          }
          nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
          if(m_NodeIds[nodeId4] == -1)
          {
            m_NodeIds[nodeId4] = nodeCount;
            nodeCount++;
          }
          triangleCount++;
          triangleCount++;
        }
        if(j == 0)
        {
          nodeId1 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
          if(m_NodeIds[nodeId1] == -1)
          {
            m_NodeIds[nodeId1] = nodeCount;
            nodeCount++;
          }
          nodeId2 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
          if(m_NodeIds[nodeId2] == -1)
          {
            m_NodeIds[nodeId2] = nodeCount;
            nodeCount++;
          }
          nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
          if(m_NodeIds[nodeId3] == -1)
          {
            m_NodeIds[nodeId3] = nodeCount;
            nodeCount++;
          }
          nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
          if(m_NodeIds[nodeId4] == -1)
          {
            m_NodeIds[nodeId4] = nodeCount;
            nodeCount++;
          }
          triangleCount++;
          triangleCount++;
        }
        if(k == 0)
        {
          nodeId1 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
          if(m_NodeIds[nodeId1] == -1)
          {
            m_NodeIds[nodeId1] = nodeCount;
            nodeCount++;
          }
          nodeId2 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
          if(m_NodeIds[nodeId2] == -1)
          {
            m_NodeIds[nodeId2] = nodeCount;
            nodeCount++;
          }
          nodeId3 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
          if(m_NodeIds[nodeId3] == -1)
          {
            m_NodeIds[nodeId3] = nodeCount;
            nodeCount++;
          }
          nodeId4 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
          if(m_NodeIds[nodeId4] == -1)
          {
            m_NodeIds[nodeId4] = nodeCount;
            nodeCount++;
          }
          triangleCount++;
          triangleCount++;
        }
        if(i == (xP - 1))
        {
          nodeId1 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
          if(m_NodeIds[nodeId1] == -1)
          {
            m_NodeIds[nodeId1] = nodeCount;
            nodeCount++;
          }
          nodeId2 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
          if(m_NodeIds[nodeId2] == -1)
          {
            m_NodeIds[nodeId2] = nodeCount;
            nodeCount++;
          }
          nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
          if(m_NodeIds[nodeId3] == -1)
          {
            m_NodeIds[nodeId3] = nodeCount;
            nodeCount++;
          }
          nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
          if(m_NodeIds[nodeId4] == -1)
          {
            m_NodeIds[nodeId4] = nodeCount;
            nodeCount++;
          }
          triangleCount++;
          triangleCount++;
        }
        else if(m_FeatureIds[point] != m_FeatureIds[neigh1])
        {
          nodeId1 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
          if(m_NodeIds[nodeId1] == -1)
          {
            m_NodeIds[nodeId1] = nodeCount;
            nodeCount++;
          }
          nodeId2 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
          if(m_NodeIds[nodeId2] == -1)
          {
            m_NodeIds[nodeId2] = nodeCount;
            nodeCount++;
          }
          nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
          if(m_NodeIds[nodeId3] == -1)
          {
            m_NodeIds[nodeId3] = nodeCount;
            nodeCount++;
          }
          nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
          if(m_NodeIds[nodeId4] == -1)
          {
            m_NodeIds[nodeId4] = nodeCount;
            nodeCount++;
          }
          triangleCount++;
          triangleCount++;
        }
        if(j == (yP - 1))
        {
          nodeId1 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
          if(m_NodeIds[nodeId1] == -1)
          {
            m_NodeIds[nodeId1] = nodeCount;
            nodeCount++;
          }
          nodeId2 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
          if(m_NodeIds[nodeId2] == -1)
          {
            m_NodeIds[nodeId2] = nodeCount;
            nodeCount++;
          }
          nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
          if(m_NodeIds[nodeId3] == -1)
          {
            m_NodeIds[nodeId3] = nodeCount;
            nodeCount++;
          }
          nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
          if(m_NodeIds[nodeId4] == -1)
          {
            m_NodeIds[nodeId4] = nodeCount;
            nodeCount++;
          }
          triangleCount++;
          triangleCount++;
        }
        else if(m_FeatureIds[point] != m_FeatureIds[neigh2])
        {
          nodeId1 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
          if(m_NodeIds[nodeId1] == -1)
          {
            m_NodeIds[nodeId1] = nodeCount;
            nodeCount++;
          }
          nodeId2 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
          if(m_NodeIds[nodeId2] == -1)
          {
            m_NodeIds[nodeId2] = nodeCount;
            nodeCount++;
          }
          nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
          if(m_NodeIds[nodeId3] == -1)
          {
            m_NodeIds[nodeId3] = nodeCount;
            nodeCount++;
          }
          nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
          if(m_NodeIds[nodeId4] == -1)
          {
            m_NodeIds[nodeId4] = nodeCount;
            nodeCount++;
          }
          triangleCount++;
          triangleCount++;
        }
        if(k == (zP - 1))
        {
          nodeId1 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
          if(m_NodeIds[nodeId1] == -1)
          {
            m_NodeIds[nodeId1] = nodeCount;
            nodeCount++;
          }
          nodeId2 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
          if(m_NodeIds[nodeId2] == -1)
          {
            m_NodeIds[nodeId2] = nodeCount;
            nodeCount++;
          }
          nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
          if(m_NodeIds[nodeId3] == -1)
          {
            m_NodeIds[nodeId3] = nodeCount;
            nodeCount++;
          }
          nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
          if(m_NodeIds[nodeId4] == -1)
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
          if(m_NodeIds[nodeId1] == -1)
          {
            m_NodeIds[nodeId1] = nodeCount;
            nodeCount++;
          }
          nodeId2 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
          if(m_NodeIds[nodeId2] == -1)
          {
            m_NodeIds[nodeId2] = nodeCount;
            nodeCount++;
          }
          nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
          if(m_NodeIds[nodeId3] == -1)
          {
            m_NodeIds[nodeId3] = nodeCount;
            nodeCount++;
          }
          nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
          if(m_NodeIds[nodeId4] == -1)
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
  int64_t triangleIndex = 0;
  for(int64_t k = 0; k < zP; k++)
  {
    for(int64_t j = 0; j < yP; j++)
    {
      for(int64_t i = 0; i < xP; i++)
      {
        point = (k * xP * yP) + (j * xP) + i;
        neigh1 = point + 1;
        neigh2 = point + xP;
        neigh3 = point + (xP * yP);

        if(i == 0)
        {
          nodeId1 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
          getGridCoordinates(grid, i, j, k, vertex + (m_NodeIds[nodeId1] * 3));

          nodeId2 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
          getGridCoordinates(grid, i, j + 1, k, vertex + (m_NodeIds[nodeId2] * 3));

          nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
          getGridCoordinates(grid, i, j, k + 1, vertex + (m_NodeIds[nodeId3] * 3));

          nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
          getGridCoordinates(grid, i + 1, j + 1, k + 1, vertex + (m_NodeIds[nodeId4] * 3));

          triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId1];
          triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId2];
          triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId3];
          m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
          m_FaceLabels[triangleIndex * 2 + 1] = -1;

          for(int32_t i = 0; i < m_SelectedWeakPtrVector.size(); i++)
          {
            EXECUTE_FUNCTION_TEMPLATE(this, copyCellArraysToFaceArrays, m_SelectedWeakPtrVector[i].lock(), triangleIndex, point, point, m_SelectedWeakPtrVector[i].lock(),
                                      m_CreatedWeakPtrVector[i].lock(), true)
          }

          triangleIndex++;

          triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId2];
          triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId4];
          triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId3];
          m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
          m_FaceLabels[triangleIndex * 2 + 1] = -1;

          for(int32_t i = 0; i < m_SelectedWeakPtrVector.size(); i++)
          {
            EXECUTE_FUNCTION_TEMPLATE(this, copyCellArraysToFaceArrays, m_SelectedWeakPtrVector[i].lock(), triangleIndex, point, point, m_SelectedWeakPtrVector[i].lock(),
                                      m_CreatedWeakPtrVector[i].lock(), true)
          }

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
        if(j == 0)
        {
          nodeId1 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
          getGridCoordinates(grid, i, j, k, vertex + (m_NodeIds[nodeId1] * 3));

          nodeId2 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
          getGridCoordinates(grid, i + 1, j, k, vertex + (m_NodeIds[nodeId2] * 3));

          nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
          getGridCoordinates(grid, i, j, k + 1, vertex + (m_NodeIds[nodeId3] * 3));

          nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
          getGridCoordinates(grid, i + 1, j, k + 1, vertex + (m_NodeIds[nodeId4] * 3));

          triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId1];
          triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId3];
          triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId2];
          m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
          m_FaceLabels[triangleIndex * 2 + 1] = -1;

          for(int32_t i = 0; i < m_SelectedWeakPtrVector.size(); i++)
          {
            EXECUTE_FUNCTION_TEMPLATE(this, copyCellArraysToFaceArrays, m_SelectedWeakPtrVector[i].lock(), triangleIndex, point, point, m_SelectedWeakPtrVector[i].lock(),
                                      m_CreatedWeakPtrVector[i].lock(), true)
          }

          triangleIndex++;

          triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId2];
          triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId3];
          triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId4];
          m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
          m_FaceLabels[triangleIndex * 2 + 1] = -1;

          for(int32_t i = 0; i < m_SelectedWeakPtrVector.size(); i++)
          {
            EXECUTE_FUNCTION_TEMPLATE(this, copyCellArraysToFaceArrays, m_SelectedWeakPtrVector[i].lock(), triangleIndex, point, point, m_SelectedWeakPtrVector[i].lock(),
                                      m_CreatedWeakPtrVector[i].lock(), true)
          }

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
        if(k == 0)
        {
          nodeId1 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
          getGridCoordinates(grid, i, j, k, vertex + (m_NodeIds[nodeId1] * 3));

          nodeId2 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
          getGridCoordinates(grid, i + 1, j, k, vertex + (m_NodeIds[nodeId2] * 3));

          nodeId3 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
          getGridCoordinates(grid, i, j + 1, k, vertex + (m_NodeIds[nodeId3] * 3));

          nodeId4 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
          getGridCoordinates(grid, i + 1, j + 1, k, vertex + (m_NodeIds[nodeId4] * 3));

          triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId1];
          triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId2];
          triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId3];
          m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
          m_FaceLabels[triangleIndex * 2 + 1] = -1;

          for(int32_t i = 0; i < m_SelectedWeakPtrVector.size(); i++)
          {
            EXECUTE_FUNCTION_TEMPLATE(this, copyCellArraysToFaceArrays, m_SelectedWeakPtrVector[i].lock(), triangleIndex, point, point, m_SelectedWeakPtrVector[i].lock(),
                                      m_CreatedWeakPtrVector[i].lock(), true)
          }

          triangleIndex++;

          triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId2];
          triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId4];
          triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId3];
          m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
          m_FaceLabels[triangleIndex * 2 + 1] = -1;

          for(int32_t i = 0; i < m_SelectedWeakPtrVector.size(); i++)
          {
            EXECUTE_FUNCTION_TEMPLATE(this, copyCellArraysToFaceArrays, m_SelectedWeakPtrVector[i].lock(), triangleIndex, point, point, m_SelectedWeakPtrVector[i].lock(),
                                      m_CreatedWeakPtrVector[i].lock(), true)
          }

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
        if(i == (xP - 1))
        {
          nodeId1 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
          getGridCoordinates(grid, i + 1, j, k, vertex + (m_NodeIds[nodeId1] * 3));

          nodeId2 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
          getGridCoordinates(grid, i + 1, j + 1, k, vertex + (m_NodeIds[nodeId2] * 3));

          nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
          getGridCoordinates(grid, i + 1, j, k + 1, vertex + (m_NodeIds[nodeId3] * 3));

          nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
          getGridCoordinates(grid, i + 1, j + 1, k + 1, vertex + (m_NodeIds[nodeId4] * 3));

          triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId3];
          triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId2];
          triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId1];
          m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
          m_FaceLabels[triangleIndex * 2 + 1] = -1;

          for(int32_t i = 0; i < m_SelectedWeakPtrVector.size(); i++)
          {
            EXECUTE_FUNCTION_TEMPLATE(this, copyCellArraysToFaceArrays, m_SelectedWeakPtrVector[i].lock(), triangleIndex, point, point, m_SelectedWeakPtrVector[i].lock(),
                                      m_CreatedWeakPtrVector[i].lock(), true)
          }

          triangleIndex++;

          triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId3];
          triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId4];
          triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId2];
          m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
          m_FaceLabels[triangleIndex * 2 + 1] = -1;

          for(int32_t i = 0; i < m_SelectedWeakPtrVector.size(); i++)
          {
            EXECUTE_FUNCTION_TEMPLATE(this, copyCellArraysToFaceArrays, m_SelectedWeakPtrVector[i].lock(), triangleIndex, point, point, m_SelectedWeakPtrVector[i].lock(),
                                      m_CreatedWeakPtrVector[i].lock(), true)
          }

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
        else if(m_FeatureIds[point] != m_FeatureIds[neigh1])
        {
          nodeId1 = (k * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
          getGridCoordinates(grid, i + 1, j, k, vertex + (m_NodeIds[nodeId1] * 3));

          nodeId2 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
          getGridCoordinates(grid, i + 1, j + 1, k, vertex + (m_NodeIds[nodeId2] * 3));

          nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
          getGridCoordinates(grid, i + 1, j, k + 1, vertex + (m_NodeIds[nodeId3] * 3));

          nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
          getGridCoordinates(grid, i + 1, j + 1, k + 1, vertex + (m_NodeIds[nodeId4] * 3));

          triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId1];
          triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId2];
          triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId3];
          m_FaceLabels[triangleIndex * 2] = m_FeatureIds[neigh1];
          m_FaceLabels[triangleIndex * 2 + 1] = m_FeatureIds[point];

          for(int32_t i = 0; i < m_SelectedWeakPtrVector.size(); i++)
          {
            EXECUTE_FUNCTION_TEMPLATE(this, copyCellArraysToFaceArrays, m_SelectedWeakPtrVector[i].lock(), triangleIndex, neigh1, point, m_SelectedWeakPtrVector[i].lock(),
                                      m_CreatedWeakPtrVector[i].lock())
          }

          triangleIndex++;

          triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId2];
          triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId4];
          triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId3];
          m_FaceLabels[triangleIndex * 2] = m_FeatureIds[neigh1];
          m_FaceLabels[triangleIndex * 2 + 1] = m_FeatureIds[point];

          for(int32_t i = 0; i < m_SelectedWeakPtrVector.size(); i++)
          {
            EXECUTE_FUNCTION_TEMPLATE(this, copyCellArraysToFaceArrays, m_SelectedWeakPtrVector[i].lock(), triangleIndex, neigh1, point, m_SelectedWeakPtrVector[i].lock(),
                                      m_CreatedWeakPtrVector[i].lock())
          }

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
        if(j == (yP - 1))
        {
          nodeId1 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
          getGridCoordinates(grid, i + 1, j + 1, k, vertex + (m_NodeIds[nodeId1] * 3));

          nodeId2 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
          getGridCoordinates(grid, i, j + 1, k, vertex + (m_NodeIds[nodeId2] * 3));

          nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
          getGridCoordinates(grid, i + 1, j + 1, k + 1, vertex + (m_NodeIds[nodeId3] * 3));

          nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
          getGridCoordinates(grid, i, j + 1, k + 1, vertex + (m_NodeIds[nodeId4] * 3));

          triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId3];
          triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId2];
          triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId1];
          m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
          m_FaceLabels[triangleIndex * 2 + 1] = -1;

          for(int32_t i = 0; i < m_SelectedWeakPtrVector.size(); i++)
          {
            EXECUTE_FUNCTION_TEMPLATE(this, copyCellArraysToFaceArrays, m_SelectedWeakPtrVector[i].lock(), triangleIndex, point, point, m_SelectedWeakPtrVector[i].lock(),
                                      m_CreatedWeakPtrVector[i].lock(), true)
          }

          triangleIndex++;

          triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId3];
          triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId4];
          triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId2];
          m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
          m_FaceLabels[triangleIndex * 2 + 1] = -1;

          for(int32_t i = 0; i < m_SelectedWeakPtrVector.size(); i++)
          {
            EXECUTE_FUNCTION_TEMPLATE(this, copyCellArraysToFaceArrays, m_SelectedWeakPtrVector[i].lock(), triangleIndex, point, point, m_SelectedWeakPtrVector[i].lock(),
                                      m_CreatedWeakPtrVector[i].lock(), true)
          }

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
        else if(m_FeatureIds[point] != m_FeatureIds[neigh2])
        {
          nodeId1 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
          getGridCoordinates(grid, i + 1, j + 1, k, vertex + (m_NodeIds[nodeId1] * 3));

          nodeId2 = (k * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
          getGridCoordinates(grid, i, j + 1, k, vertex + (m_NodeIds[nodeId2] * 3));

          nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
          getGridCoordinates(grid, i + 1, j + 1, k + 1, vertex + (m_NodeIds[nodeId3] * 3));

          nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
          getGridCoordinates(grid, i, j + 1, k + 1, vertex + (m_NodeIds[nodeId4] * 3));

          triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId1];
          triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId2];
          triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId3];
          m_FaceLabels[triangleIndex * 2] = m_FeatureIds[neigh2];
          m_FaceLabels[triangleIndex * 2 + 1] = m_FeatureIds[point];

          for(int32_t i = 0; i < m_SelectedWeakPtrVector.size(); i++)
          {
            EXECUTE_FUNCTION_TEMPLATE(this, copyCellArraysToFaceArrays, m_SelectedWeakPtrVector[i].lock(), triangleIndex, neigh2, point, m_SelectedWeakPtrVector[i].lock(),
                                      m_CreatedWeakPtrVector[i].lock())
          }

          triangleIndex++;

          triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId2];
          triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId4];
          triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId3];
          m_FaceLabels[triangleIndex * 2] = m_FeatureIds[neigh2];
          m_FaceLabels[triangleIndex * 2 + 1] = m_FeatureIds[point];

          for(int32_t i = 0; i < m_SelectedWeakPtrVector.size(); i++)
          {
            EXECUTE_FUNCTION_TEMPLATE(this, copyCellArraysToFaceArrays, m_SelectedWeakPtrVector[i].lock(), triangleIndex, neigh2, point, m_SelectedWeakPtrVector[i].lock(),
                                      m_CreatedWeakPtrVector[i].lock())
          }

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
        if(k == (zP - 1))
        {
          nodeId1 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
          getGridCoordinates(grid, i + 1, j, k + 1, vertex + (m_NodeIds[nodeId1] * 3));

          nodeId2 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
          getGridCoordinates(grid, i, j, k + 1, vertex + (m_NodeIds[nodeId2] * 3));

          nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
          getGridCoordinates(grid, i + 1, j + 1, k + 1, vertex + (m_NodeIds[nodeId3] * 3));

          nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
          getGridCoordinates(grid, i, j + 1, k + 1, vertex + (m_NodeIds[nodeId4] * 3));

          triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId2];
          triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId3];
          triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId1];
          m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
          m_FaceLabels[triangleIndex * 2 + 1] = -1;

          for(int32_t i = 0; i < m_SelectedWeakPtrVector.size(); i++)
          {
            EXECUTE_FUNCTION_TEMPLATE(this, copyCellArraysToFaceArrays, m_SelectedWeakPtrVector[i].lock(), triangleIndex, point, point, m_SelectedWeakPtrVector[i].lock(),
                                      m_CreatedWeakPtrVector[i].lock(), true)
          }

          triangleIndex++;

          triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId4];
          triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId3];
          triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId2];
          m_FaceLabels[triangleIndex * 2] = m_FeatureIds[point];
          m_FaceLabels[triangleIndex * 2 + 1] = -1;

          for(int32_t i = 0; i < m_SelectedWeakPtrVector.size(); i++)
          {
            EXECUTE_FUNCTION_TEMPLATE(this, copyCellArraysToFaceArrays, m_SelectedWeakPtrVector[i].lock(), triangleIndex, point, point, m_SelectedWeakPtrVector[i].lock(),
                                      m_CreatedWeakPtrVector[i].lock(), true)
          }

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
        else if(m_FeatureIds[point] != m_FeatureIds[neigh3])
        {
          nodeId1 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + (i + 1);
          getGridCoordinates(grid, i + 1, j, k + 1, vertex + (m_NodeIds[nodeId1] * 3));

          nodeId2 = ((k + 1) * (xP + 1) * (yP + 1)) + (j * (xP + 1)) + i;
          getGridCoordinates(grid, i, j, k + 1, vertex + (m_NodeIds[nodeId2] * 3));

          nodeId3 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + (i + 1);
          getGridCoordinates(grid, i + 1, j + 1, k + 1, vertex + (m_NodeIds[nodeId3] * 3));

          nodeId4 = ((k + 1) * (xP + 1) * (yP + 1)) + ((j + 1) * (xP + 1)) + i;
          getGridCoordinates(grid, i, j + 1, k + 1, vertex + (m_NodeIds[nodeId4] * 3));

          triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId1];
          triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId3];
          triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId2];
          m_FaceLabels[triangleIndex * 2] = m_FeatureIds[neigh3];
          m_FaceLabels[triangleIndex * 2 + 1] = m_FeatureIds[point];

          for(int32_t i = 0; i < m_SelectedWeakPtrVector.size(); i++)
          {
            EXECUTE_FUNCTION_TEMPLATE(this, copyCellArraysToFaceArrays, m_SelectedWeakPtrVector[i].lock(), triangleIndex, neigh3, point, m_SelectedWeakPtrVector[i].lock(),
                                      m_CreatedWeakPtrVector[i].lock())
          }

          triangleIndex++;

          triangle[triangleIndex * 3 + 0] = m_NodeIds[nodeId2];
          triangle[triangleIndex * 3 + 1] = m_NodeIds[nodeId3];
          triangle[triangleIndex * 3 + 2] = m_NodeIds[nodeId4];
          m_FaceLabels[triangleIndex * 2] = m_FeatureIds[neigh3];
          m_FaceLabels[triangleIndex * 2 + 1] = m_FeatureIds[point];

          for(int32_t i = 0; i < m_SelectedWeakPtrVector.size(); i++)
          {
            EXECUTE_FUNCTION_TEMPLATE(this, copyCellArraysToFaceArrays, m_SelectedWeakPtrVector[i].lock(), triangleIndex, neigh3, point, m_SelectedWeakPtrVector[i].lock(),
                                      m_CreatedWeakPtrVector[i].lock())
          }

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

  for(int64_t i = 0; i < nodeCount; i++)
  {
    m_NodeTypes[i] = ownerLists[i].size();
    if(m_NodeTypes[i] > 4)
    {
      m_NodeTypes[i] = 4;
    }
    if(ownerLists[i].find(-1) != ownerLists[i].end())
    {
      m_NodeTypes[i] += 10;
    }
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
{
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString QuickSurfaceMesh::getBrandingString()
{
  return "SurfaceMeshing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString QuickSurfaceMesh::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString QuickSurfaceMesh::getGroupName()
{
  return SIMPL::FilterGroups::SurfaceMeshingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString QuickSurfaceMesh::getSubGroupName()
{
  return SIMPL::FilterSubGroups::GenerationFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString QuickSurfaceMesh::getHumanLabel()
{
  return "Quick Surface Mesh";
}
