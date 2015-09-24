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
#include "FeatureFaceCurvatureFilter.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/task_scheduler_init.h>
#include <tbb/task_group.h>
#include <tbb/task.h>
#endif

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"

#include "CalculateTriangleGroupCurvatures.h"

// Include the MOC generated file for this class
#include "moc_FeatureFaceCurvatureFilter.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FeatureFaceCurvatureFilter::FeatureFaceCurvatureFilter() :
  SurfaceMeshFilter(),
  m_FaceAttributeMatrixPath(DREAM3D::Defaults::TriangleDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, ""),
  m_SurfaceMeshPrincipalCurvature1sArrayName(DREAM3D::FaceData::SurfaceMeshPrincipalCurvature1),
  m_SurfaceMeshPrincipalCurvature2sArrayName(DREAM3D::FaceData::SurfaceMeshPrincipalCurvature2),
  m_SurfaceMeshPrincipalDirection1sArrayName(DREAM3D::FaceData::SurfaceMeshPrincipalDirection1),
  m_SurfaceMeshPrincipalDirection2sArrayName(DREAM3D::FaceData::SurfaceMeshPrincipalDirection2),
  m_SurfaceMeshGaussianCurvaturesArrayName(DREAM3D::FaceData::SurfaceMeshGaussianCurvatures),
  m_SurfaceMeshMeanCurvaturesArrayName(DREAM3D::FaceData::SurfaceMeshMeanCurvatures),
  m_NRing(3),
  m_ComputePrincipalDirectionVectors(true),
  m_ComputeMeanCurvature(false),
  m_ComputeGaussianCurvature(false),
  m_UseNormalsForCurveFitting(true),
  m_SurfaceMeshFaceLabelsArrayPath(DREAM3D::Defaults::TriangleDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFeatureFaceIdsArrayPath(DREAM3D::Defaults::TriangleDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFeatureFaceId),
  m_SurfaceMeshFaceNormalsArrayPath(DREAM3D::Defaults::TriangleDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceNormals),
  m_SurfaceMeshTriangleCentroidsArrayPath(DREAM3D::Defaults::TriangleDataContainerName, DREAM3D::Defaults::FaceAttributeMatrixName, DREAM3D::FaceData::SurfaceMeshFaceCentroids),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshTriangleCentroids(NULL),
  m_SurfaceMeshFaceNormals(NULL),
  m_SurfaceMeshPrincipalCurvature1s(NULL),
  m_SurfaceMeshPrincipalCurvature2s(NULL),
  m_SurfaceMeshPrincipalDirection1s(NULL),
  m_SurfaceMeshPrincipalDirection2s(NULL),
  m_SurfaceMeshGaussianCurvatures(NULL),
  m_SurfaceMeshMeanCurvatures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FeatureFaceCurvatureFilter::~FeatureFaceCurvatureFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureFaceCurvatureFilter::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;

  parameters.push_back(IntFilterParameter::New("Neighborhood Ring Count", "NRing", getNRing(), FilterParameter::Parameter));
  QStringList linkedProps;
  linkedProps << "SurfaceMeshPrincipalCurvature1sArrayName" << "SurfaceMeshPrincipalCurvature2sArrayName";
  linkedProps << "SurfaceMeshPrincipalDirection1sArrayName" << "SurfaceMeshPrincipalDirection2sArrayName";
  parameters.push_back(LinkedBooleanFilterParameter::New("Compute Principal Direction Vectors", "ComputePrincipalDirectionVectors", getComputePrincipalDirectionVectors(), linkedProps, FilterParameter::Parameter));
  linkedProps.clear();
  linkedProps << "SurfaceMeshGaussianCurvaturesArrayName";
  parameters.push_back(LinkedBooleanFilterParameter::New("Compute Gaussian Curvature", "ComputeGaussianCurvature", getComputeGaussianCurvature(), linkedProps, FilterParameter::Parameter));
  linkedProps.clear();
  linkedProps << "SurfaceMeshMeanCurvaturesArrayName";
  parameters.push_back(LinkedBooleanFilterParameter::New("Compute Mean Curvature", "ComputeMeanCurvature", getComputeMeanCurvature(), linkedProps, FilterParameter::Parameter));
  linkedProps.clear();
  linkedProps << "SurfaceMeshFaceNormalsArrayPath";
  parameters.push_back(LinkedBooleanFilterParameter::New("Use Face Normals for Curve Fitting", "UseNormalsForCurveFitting", getUseNormalsForCurveFitting(), linkedProps, FilterParameter::Parameter));

  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(DREAM3D::AttributeMatrixType::Face, DREAM3D::GeometryType::TriangleGeometry);
    parameters.push_back(AttributeMatrixSelectionFilterParameter::New("Face Attribute Matrix", "FaceAttributeMatrixPath", getFaceAttributeMatrixPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Int32, 2, DREAM3D::AttributeMatrixType::Face, DREAM3D::GeometryType::TriangleGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Face Labels", "SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Int32, 1, DREAM3D::AttributeMatrixType::Face, DREAM3D::GeometryType::TriangleGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Feature Face Ids", "SurfaceMeshFeatureFaceIdsArrayPath", getSurfaceMeshFeatureFaceIdsArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Double, 3, DREAM3D::AttributeMatrixType::Face, DREAM3D::GeometryType::TriangleGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Face Normals", "SurfaceMeshFaceNormalsArrayPath", getSurfaceMeshFaceNormalsArrayPath(), FilterParameter::RequiredArray, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(DREAM3D::TypeNames::Double, 3, DREAM3D::AttributeMatrixType::Face, DREAM3D::GeometryType::TriangleGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Face Centroids", "SurfaceMeshTriangleCentroidsArrayPath", getSurfaceMeshTriangleCentroidsArrayPath(), FilterParameter::RequiredArray, req));
  }

  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Principal Curvature 1", "SurfaceMeshPrincipalCurvature1sArrayName", getSurfaceMeshPrincipalCurvature1sArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Principal Curvature 2", "SurfaceMeshPrincipalCurvature2sArrayName", getSurfaceMeshPrincipalCurvature2sArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Principal Direction 1", "SurfaceMeshPrincipalDirection1sArrayName", getSurfaceMeshPrincipalDirection1sArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Principal Direction 2", "SurfaceMeshPrincipalDirection2sArrayName", getSurfaceMeshPrincipalDirection2sArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Gaussian Curvature", "SurfaceMeshGaussianCurvaturesArrayName", getSurfaceMeshGaussianCurvaturesArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Mean Curvature", "SurfaceMeshMeanCurvaturesArrayName", getSurfaceMeshMeanCurvaturesArrayName(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureFaceCurvatureFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFaceAttributeMatrixPath(reader->readDataArrayPath("FaceAttributeMatrixPath", getFaceAttributeMatrixPath() ) );
  setSurfaceMeshPrincipalCurvature1sArrayName(reader->readString("SurfaceMeshPrincipalCurvature1sArrayName", getSurfaceMeshPrincipalCurvature1sArrayName() ) );
  setSurfaceMeshPrincipalCurvature2sArrayName(reader->readString("SurfaceMeshPrincipalCurvature2sArrayName", getSurfaceMeshPrincipalCurvature2sArrayName() ) );
  setSurfaceMeshPrincipalDirection1sArrayName(reader->readString("SurfaceMeshPrincipalDirection1sArrayName", getSurfaceMeshPrincipalDirection1sArrayName() ) );
  setSurfaceMeshPrincipalDirection2sArrayName(reader->readString("SurfaceMeshPrincipalDirection2sArrayName", getSurfaceMeshPrincipalDirection2sArrayName() ) );
  setSurfaceMeshGaussianCurvaturesArrayName(reader->readString("SurfaceMeshGaussianCurvaturesArrayName", getSurfaceMeshGaussianCurvaturesArrayName() ) );
  setSurfaceMeshMeanCurvaturesArrayName(reader->readString("SurfaceMeshMeanCurvaturesArrayName", getSurfaceMeshMeanCurvaturesArrayName() ) );
  setSurfaceMeshTriangleCentroidsArrayPath(reader->readDataArrayPath("SurfaceMeshTriangleCentroidsArrayPath", getSurfaceMeshTriangleCentroidsArrayPath() ) );
  setSurfaceMeshFaceNormalsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceNormalsArrayPath", getSurfaceMeshFaceNormalsArrayPath() ) );
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() ) );
  setSurfaceMeshFeatureFaceIdsArrayPath(reader->readDataArrayPath("SurfaceMeshFeatureFaceIdsArrayPath", getSurfaceMeshFeatureFaceIdsArrayPath() ) );
  setNRing( reader->readValue("NRing", getNRing()) );
  setComputePrincipalDirectionVectors( reader->readValue("ComputePrincipalDirectionVectors", getComputePrincipalDirectionVectors()) );
  setComputeGaussianCurvature( reader->readValue("ComputeGaussianCurvature", getComputeGaussianCurvature()) );
  setComputeMeanCurvature( reader->readValue("ComputeMeanCurvature", getComputeMeanCurvature()) );
  setUseNormalsForCurveFitting( reader->readValue("UseNormalsForCurveFitting", getUseNormalsForCurveFitting()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FeatureFaceCurvatureFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(FaceAttributeMatrixPath)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshPrincipalCurvature1sArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshPrincipalCurvature2sArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshPrincipalDirection1sArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshPrincipalDirection2sArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshGaussianCurvaturesArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshMeanCurvaturesArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshTriangleCentroidsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshFaceNormalsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshFaceLabelsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshFeatureFaceIdsArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(NRing)
  SIMPL_FILTER_WRITE_PARAMETER(ComputePrincipalDirectionVectors)
  SIMPL_FILTER_WRITE_PARAMETER(ComputeGaussianCurvature)
  SIMPL_FILTER_WRITE_PARAMETER(ComputeMeanCurvature)
  SIMPL_FILTER_WRITE_PARAMETER(UseNormalsForCurveFitting)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureFaceCurvatureFilter::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  TriangleGeom::Pointer triangles = getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());

  QVector<IDataArray::Pointer> dataArrays;

  if(getErrorCondition() >= 0) { dataArrays.push_back(triangles->getTriangles()); }

  DataContainer::Pointer sm = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath().getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }


  // We do not know the size of the array so we can not use the macro so we just manually call
  // the needed methods that will propagate these array additions to the pipeline
  QVector<size_t> cDims(1, 1);
  tempPath = getFaceAttributeMatrixPath();
  tempPath.setDataArrayName(getSurfaceMeshPrincipalCurvature1sArrayName());
  m_SurfaceMeshPrincipalCurvature1sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshPrincipalCurvature1sPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshPrincipalCurvature1s = m_SurfaceMeshPrincipalCurvature1sPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrays.push_back(m_SurfaceMeshPrincipalCurvature1sPtr.lock()); }

  tempPath.setDataArrayName(getSurfaceMeshPrincipalCurvature2sArrayName());
  m_SurfaceMeshPrincipalCurvature2sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshPrincipalCurvature2sPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshPrincipalCurvature2s = m_SurfaceMeshPrincipalCurvature2sPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if (m_ComputeGaussianCurvature == true)
  {
    tempPath.setDataArrayName(getSurfaceMeshGaussianCurvaturesArrayName());
    m_SurfaceMeshGaussianCurvaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshGaussianCurvaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshGaussianCurvatures = m_SurfaceMeshGaussianCurvaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  if (m_ComputeMeanCurvature == true)
  {
    tempPath.setDataArrayName(getSurfaceMeshMeanCurvaturesArrayName());
    m_SurfaceMeshMeanCurvaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshMeanCurvaturesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshMeanCurvatures = m_SurfaceMeshMeanCurvaturesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  if (m_ComputePrincipalDirectionVectors == true)
  {
    cDims[0] = 3;
    tempPath.setDataArrayName(getSurfaceMeshPrincipalDirection1sArrayName());
    m_SurfaceMeshPrincipalDirection1sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshPrincipalDirection1sPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshPrincipalDirection1s = m_SurfaceMeshPrincipalDirection1sPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

    tempPath.setDataArrayName(getSurfaceMeshPrincipalDirection2sArrayName());
    m_SurfaceMeshPrincipalDirection2sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_SurfaceMeshPrincipalDirection2sPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SurfaceMeshPrincipalDirection2s = m_SurfaceMeshPrincipalDirection2sPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  cDims[0] = 1;
  m_SurfaceMeshFeatureFaceIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFeatureFaceIdsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFeatureFaceIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {m_SurfaceMeshFeatureFaceIds = m_SurfaceMeshFeatureFaceIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrays.push_back(m_SurfaceMeshFeatureFaceIdsPtr.lock()); }

  cDims[0] = 2;
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrays.push_back(m_SurfaceMeshFaceLabelsPtr.lock()); }

  cDims[0] = 3;
  m_SurfaceMeshFaceNormalsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, getSurfaceMeshFaceNormalsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceNormalsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceNormals = m_SurfaceMeshFaceNormalsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrays.push_back(m_SurfaceMeshFaceNormalsPtr.lock()); }

  m_SurfaceMeshTriangleCentroidsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, getSurfaceMeshTriangleCentroidsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshTriangleCentroidsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshTriangleCentroids = m_SurfaceMeshTriangleCentroidsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrays.push_back(m_SurfaceMeshTriangleCentroidsPtr.lock()); }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrays);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureFaceCurvatureFilter::preflight()
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
void FeatureFaceCurvatureFilter::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());

  // Get our Reference counted Array of Face Structures
  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();

  // Just to double check we have everything.
  int64_t numTriangles = triangleGeom->getNumberOfTris();

  // Make sure the Face Connectivity is created because the FindNRing algorithm needs this and will
  // assert if the data is NOT in the SurfaceMesh Data Container
  ElementDynamicList::Pointer vertLinks = triangleGeom->getElementsContainingVert();
  if (NULL == vertLinks.get())
  {
    triangleGeom->findElementsContainingVert();
  }

  // get the QMap from the SharedFeatureFaces filter
  SharedFeatureFaces_t sharedFeatureFaces;

  int32_t maxFaceId = 0;
  for (int64_t t = 0; t < numTriangles; ++t)
  {
    if (m_SurfaceMeshFeatureFaceIds[t] > maxFaceId) { maxFaceId = m_SurfaceMeshFeatureFaceIds[t]; }
  }
  std::vector<int32_t> faceSizes(maxFaceId + 1, 0);
  // Loop through all the Triangles and assign each one to a unique Feature Face Id.
  for (int64_t t = 0; t < numTriangles; ++t)
  {
    faceSizes[m_SurfaceMeshFeatureFaceIds[t]]++;
  }

  // Allocate all the vectors that we need
  for (size_t iter = 0; iter < faceSizes.size(); ++iter)
  {
    FaceIds_t v;
    v.reserve(faceSizes[iter]);
    sharedFeatureFaces[iter] = v;
  }

  // Loop through all the Triangles and assign each one to a unique Feature Face Id.
  for(int64_t t = 0; t < numTriangles; ++t)
  {
    sharedFeatureFaces[m_SurfaceMeshFeatureFaceIds[t]].push_back(t);
  }

  m_TotalFeatureFaces = sharedFeatureFaces.size();
  m_CompletedFeatureFaces = 0;

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif


#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_group* g = new tbb::task_group;
#else

#endif
  // typedef here for conveneince
  typedef SharedFeatureFaces_t::iterator SharedFeatureFaceIterator_t;

  for(SharedFeatureFaceIterator_t iter = sharedFeatureFaces.begin(); iter != sharedFeatureFaces.end(); ++iter)
  {
    QString ss = QObject::tr("Working on Face Id %1/%2").arg((*iter).first).arg(maxFaceId);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

    FaceIds_t& triangleIds = (*iter).second;
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
    if (doParallel == true)
    {
      g->run(CalculateTriangleGroupCurvatures(m_NRing, triangleIds, m_UseNormalsForCurveFitting,
                                              m_SurfaceMeshPrincipalCurvature1sPtr.lock(), m_SurfaceMeshPrincipalCurvature2sPtr.lock(),
                                              m_SurfaceMeshPrincipalDirection1sPtr.lock(), m_SurfaceMeshPrincipalDirection2sPtr.lock(),
                                              m_SurfaceMeshGaussianCurvaturesPtr.lock(), m_SurfaceMeshMeanCurvaturesPtr.lock(), triangleGeom,
                                              m_SurfaceMeshFaceLabelsPtr.lock(),
                                              m_SurfaceMeshFaceNormalsPtr.lock(),
                                              m_SurfaceMeshTriangleCentroidsPtr.lock(),
                                              this ) );
    }
    else
#endif
    {
      CalculateTriangleGroupCurvatures curvature(m_NRing, triangleIds, m_UseNormalsForCurveFitting,
                                                 m_SurfaceMeshPrincipalCurvature1sPtr.lock(), m_SurfaceMeshPrincipalCurvature2sPtr.lock(),
                                                 m_SurfaceMeshPrincipalDirection1sPtr.lock(), m_SurfaceMeshPrincipalDirection2sPtr.lock(),
                                                 m_SurfaceMeshGaussianCurvaturesPtr.lock(), m_SurfaceMeshMeanCurvaturesPtr.lock(), triangleGeom,
                                                 m_SurfaceMeshFaceLabelsPtr.lock(),
                                                 m_SurfaceMeshFaceNormalsPtr.lock(),
                                                 m_SurfaceMeshTriangleCentroidsPtr.lock(),
                                                 this );
      curvature();
    }
  }
  // *********************** END END END END END END  ********************************************************************

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  g->wait(); // Wait for all the threads to complete before moving on.
  delete g;
#endif

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
void FeatureFaceCurvatureFilter::tbbTaskProgress()
{
  m_CompletedFeatureFaces++;

  QString ss = QObject::tr("%1/%2 Complete").arg(m_CompletedFeatureFaces).arg(m_TotalFeatureFaces);
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
}

#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FeatureFaceCurvatureFilter::newFilterInstance(bool copyFilterParameters)
{
  FeatureFaceCurvatureFilter::Pointer filter = FeatureFaceCurvatureFilter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureFaceCurvatureFilter::getCompiledLibraryName()
{ return SurfaceMeshingConstants::SurfaceMeshingBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureFaceCurvatureFilter::getGroupName()
{ return DREAM3D::FilterGroups::SurfaceMeshingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureFaceCurvatureFilter::getSubGroupName()
{ return DREAM3D::FilterSubGroups::CurvatureFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureFaceCurvatureFilter::getHumanLabel()
{ return "Find Feature Face Curvature"; }
