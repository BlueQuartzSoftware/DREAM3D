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
#include "FeatureFaceCurvatureFilter.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/task.h>
#include <tbb/task_group.h>
#include <tbb/task_scheduler_init.h>
#endif

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

#include "CalculateTriangleGroupCurvatures.h"

// Include the MOC generated file for this class
#include "moc_FeatureFaceCurvatureFilter.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FeatureFaceCurvatureFilter::FeatureFaceCurvatureFilter()
: SurfaceMeshFilter()
, m_FaceAttributeMatrixPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, "")
, m_SurfaceMeshPrincipalCurvature1sArrayName(SIMPL::FaceData::SurfaceMeshPrincipalCurvature1)
, m_SurfaceMeshPrincipalCurvature2sArrayName(SIMPL::FaceData::SurfaceMeshPrincipalCurvature2)
, m_SurfaceMeshPrincipalDirection1sArrayName(SIMPL::FaceData::SurfaceMeshPrincipalDirection1)
, m_SurfaceMeshPrincipalDirection2sArrayName(SIMPL::FaceData::SurfaceMeshPrincipalDirection2)
, m_SurfaceMeshGaussianCurvaturesArrayName(SIMPL::FaceData::SurfaceMeshGaussianCurvatures)
, m_SurfaceMeshMeanCurvaturesArrayName(SIMPL::FaceData::SurfaceMeshMeanCurvatures)
, m_NRing(3)
, m_ComputePrincipalDirectionVectors(true)
, m_ComputeMeanCurvature(false)
, m_ComputeGaussianCurvature(false)
, m_UseNormalsForCurveFitting(true)
, m_SurfaceMeshFaceLabelsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels)
, m_SurfaceMeshFeatureFaceIdsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFeatureFaceId)
, m_SurfaceMeshFaceNormalsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceNormals)
, m_SurfaceMeshTriangleCentroidsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceCentroids)
, m_SurfaceMeshFaceLabels(nullptr)
, m_SurfaceMeshTriangleCentroids(nullptr)
, m_SurfaceMeshFaceNormals(nullptr)
, m_SurfaceMeshPrincipalCurvature1s(nullptr)
, m_SurfaceMeshPrincipalCurvature2s(nullptr)
, m_SurfaceMeshPrincipalDirection1s(nullptr)
, m_SurfaceMeshPrincipalDirection2s(nullptr)
, m_SurfaceMeshGaussianCurvatures(nullptr)
, m_SurfaceMeshMeanCurvatures(nullptr)
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

  parameters.push_back(SIMPL_NEW_INTEGER_FP("Neighborhood Ring Count", NRing, FilterParameter::Parameter, FeatureFaceCurvatureFilter));
  QStringList linkedProps;
  linkedProps << "SurfaceMeshPrincipalCurvature1sArrayName"
              << "SurfaceMeshPrincipalCurvature2sArrayName";
  linkedProps << "SurfaceMeshPrincipalDirection1sArrayName"
              << "SurfaceMeshPrincipalDirection2sArrayName";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Compute Principal Direction Vectors", ComputePrincipalDirectionVectors, FilterParameter::Parameter, FeatureFaceCurvatureFilter, linkedProps));
  linkedProps.clear();
  linkedProps << "SurfaceMeshGaussianCurvaturesArrayName";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Compute Gaussian Curvature", ComputeGaussianCurvature, FilterParameter::Parameter, FeatureFaceCurvatureFilter, linkedProps));
  linkedProps.clear();
  linkedProps << "SurfaceMeshMeanCurvaturesArrayName";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Compute Mean Curvature", ComputeMeanCurvature, FilterParameter::Parameter, FeatureFaceCurvatureFilter, linkedProps));
  linkedProps.clear();
  linkedProps << "SurfaceMeshFaceNormalsArrayPath";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Use Face Normals for Curve Fitting", UseNormalsForCurveFitting, FilterParameter::Parameter, FeatureFaceCurvatureFilter, linkedProps));

  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Face Attribute Matrix", FaceAttributeMatrixPath, FilterParameter::RequiredArray, FeatureFaceCurvatureFilter, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Labels", SurfaceMeshFaceLabelsArrayPath, FilterParameter::RequiredArray, FeatureFaceCurvatureFilter, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Face Ids", SurfaceMeshFeatureFaceIdsArrayPath, FilterParameter::RequiredArray, FeatureFaceCurvatureFilter, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Double, 3, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Normals", SurfaceMeshFaceNormalsArrayPath, FilterParameter::RequiredArray, FeatureFaceCurvatureFilter, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Double, 3, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Centroids", SurfaceMeshTriangleCentroidsArrayPath, FilterParameter::RequiredArray, FeatureFaceCurvatureFilter, req));
  }

  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Principal Curvature 1", SurfaceMeshPrincipalCurvature1sArrayName, FilterParameter::CreatedArray, FeatureFaceCurvatureFilter));
  parameters.push_back(SIMPL_NEW_STRING_FP("Principal Curvature 2", SurfaceMeshPrincipalCurvature2sArrayName, FilterParameter::CreatedArray, FeatureFaceCurvatureFilter));
  parameters.push_back(SIMPL_NEW_STRING_FP("Principal Direction 1", SurfaceMeshPrincipalDirection1sArrayName, FilterParameter::CreatedArray, FeatureFaceCurvatureFilter));
  parameters.push_back(SIMPL_NEW_STRING_FP("Principal Direction 2", SurfaceMeshPrincipalDirection2sArrayName, FilterParameter::CreatedArray, FeatureFaceCurvatureFilter));
  parameters.push_back(SIMPL_NEW_STRING_FP("Gaussian Curvature", SurfaceMeshGaussianCurvaturesArrayName, FilterParameter::CreatedArray, FeatureFaceCurvatureFilter));
  parameters.push_back(SIMPL_NEW_STRING_FP("Mean Curvature", SurfaceMeshMeanCurvaturesArrayName, FilterParameter::CreatedArray, FeatureFaceCurvatureFilter));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureFaceCurvatureFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFaceAttributeMatrixPath(reader->readDataArrayPath("FaceAttributeMatrixPath", getFaceAttributeMatrixPath()));
  setSurfaceMeshPrincipalCurvature1sArrayName(reader->readString("SurfaceMeshPrincipalCurvature1sArrayName", getSurfaceMeshPrincipalCurvature1sArrayName()));
  setSurfaceMeshPrincipalCurvature2sArrayName(reader->readString("SurfaceMeshPrincipalCurvature2sArrayName", getSurfaceMeshPrincipalCurvature2sArrayName()));
  setSurfaceMeshPrincipalDirection1sArrayName(reader->readString("SurfaceMeshPrincipalDirection1sArrayName", getSurfaceMeshPrincipalDirection1sArrayName()));
  setSurfaceMeshPrincipalDirection2sArrayName(reader->readString("SurfaceMeshPrincipalDirection2sArrayName", getSurfaceMeshPrincipalDirection2sArrayName()));
  setSurfaceMeshGaussianCurvaturesArrayName(reader->readString("SurfaceMeshGaussianCurvaturesArrayName", getSurfaceMeshGaussianCurvaturesArrayName()));
  setSurfaceMeshMeanCurvaturesArrayName(reader->readString("SurfaceMeshMeanCurvaturesArrayName", getSurfaceMeshMeanCurvaturesArrayName()));
  setSurfaceMeshTriangleCentroidsArrayPath(reader->readDataArrayPath("SurfaceMeshTriangleCentroidsArrayPath", getSurfaceMeshTriangleCentroidsArrayPath()));
  setSurfaceMeshFaceNormalsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceNormalsArrayPath", getSurfaceMeshFaceNormalsArrayPath()));
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath()));
  setSurfaceMeshFeatureFaceIdsArrayPath(reader->readDataArrayPath("SurfaceMeshFeatureFaceIdsArrayPath", getSurfaceMeshFeatureFaceIdsArrayPath()));
  setNRing(reader->readValue("NRing", getNRing()));
  setComputePrincipalDirectionVectors(reader->readValue("ComputePrincipalDirectionVectors", getComputePrincipalDirectionVectors()));
  setComputeGaussianCurvature(reader->readValue("ComputeGaussianCurvature", getComputeGaussianCurvature()));
  setComputeMeanCurvature(reader->readValue("ComputeMeanCurvature", getComputeMeanCurvature()));
  setUseNormalsForCurveFitting(reader->readValue("UseNormalsForCurveFitting", getUseNormalsForCurveFitting()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureFaceCurvatureFilter::initialize()
{
  m_SurfaceMeshFaceEdges = nullptr;
  m_TotalFeatureFaces = -1;
  m_CompletedFeatureFaces = -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FeatureFaceCurvatureFilter::dataCheck()
{
  setErrorCondition(0);
  initialize();

  DataArrayPath tempPath;

  TriangleGeom::Pointer triangles = getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());

  QVector<IDataArray::Pointer> dataArrays;

  if(getErrorCondition() >= 0)
  {
    dataArrays.push_back(triangles->getTriangles());
  }

  DataContainer::Pointer sm = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath().getDataContainerName(), false);
  if(getErrorCondition() < 0)
  {
    return;
  }

  // We do not know the size of the array so we can not use the macro so we just manually call
  // the needed methods that will propagate these array additions to the pipeline
  QVector<size_t> cDims(1, 1);
  tempPath = getFaceAttributeMatrixPath();
  tempPath.setDataArrayName(getSurfaceMeshPrincipalCurvature1sArrayName());
  m_SurfaceMeshPrincipalCurvature1sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
      this, tempPath, 0, cDims);                                   /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshPrincipalCurvature1sPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshPrincipalCurvature1s = m_SurfaceMeshPrincipalCurvature1sPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrays.push_back(m_SurfaceMeshPrincipalCurvature1sPtr.lock());
  }

  tempPath.setDataArrayName(getSurfaceMeshPrincipalCurvature2sArrayName());
  m_SurfaceMeshPrincipalCurvature2sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
      this, tempPath, 0, cDims);                                   /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshPrincipalCurvature2sPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshPrincipalCurvature2s = m_SurfaceMeshPrincipalCurvature2sPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_ComputeGaussianCurvature == true)
  {
    tempPath.setDataArrayName(getSurfaceMeshGaussianCurvaturesArrayName());
    m_SurfaceMeshGaussianCurvaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
        this, tempPath, 0, cDims);                                 /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_SurfaceMeshGaussianCurvaturesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_SurfaceMeshGaussianCurvatures = m_SurfaceMeshGaussianCurvaturesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  if(m_ComputeMeanCurvature == true)
  {
    tempPath.setDataArrayName(getSurfaceMeshMeanCurvaturesArrayName());
    m_SurfaceMeshMeanCurvaturesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
        this, tempPath, 0, cDims);                             /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_SurfaceMeshMeanCurvaturesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_SurfaceMeshMeanCurvatures = m_SurfaceMeshMeanCurvaturesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  if(m_ComputePrincipalDirectionVectors == true)
  {
    cDims[0] = 3;
    tempPath.setDataArrayName(getSurfaceMeshPrincipalDirection1sArrayName());
    m_SurfaceMeshPrincipalDirection1sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
        this, tempPath, 0, cDims);                                   /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_SurfaceMeshPrincipalDirection1sPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_SurfaceMeshPrincipalDirection1s = m_SurfaceMeshPrincipalDirection1sPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */

    tempPath.setDataArrayName(getSurfaceMeshPrincipalDirection2sArrayName());
    m_SurfaceMeshPrincipalDirection2sPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<double>, AbstractFilter, double>(
        this, tempPath, 0, cDims);                                   /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_SurfaceMeshPrincipalDirection2sPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_SurfaceMeshPrincipalDirection2s = m_SurfaceMeshPrincipalDirection2sPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  cDims[0] = 1;
  m_SurfaceMeshFeatureFaceIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(
      this, getSurfaceMeshFeatureFaceIdsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshFeatureFaceIdsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshFeatureFaceIds = m_SurfaceMeshFeatureFaceIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrays.push_back(m_SurfaceMeshFeatureFaceIdsPtr.lock());
  }

  cDims[0] = 2;
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(),
                                                                                                                   cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshFaceLabelsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrays.push_back(m_SurfaceMeshFaceLabelsPtr.lock());
  }

  cDims[0] = 3;
  m_SurfaceMeshFaceNormalsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(this, getSurfaceMeshFaceNormalsArrayPath(),
                                                                                                                   cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshFaceNormalsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshFaceNormals = m_SurfaceMeshFaceNormalsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrays.push_back(m_SurfaceMeshFaceNormalsPtr.lock());
  }

  m_SurfaceMeshTriangleCentroidsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<double>, AbstractFilter>(
      this, getSurfaceMeshTriangleCentroidsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceMeshTriangleCentroidsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceMeshTriangleCentroids = m_SurfaceMeshTriangleCentroidsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrays.push_back(m_SurfaceMeshTriangleCentroidsPtr.lock());
  }

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
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());

  // Get our Reference counted Array of Face Structures
  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();

  // Just to double check we have everything.
  int64_t numTriangles = triangleGeom->getNumberOfTris();

  // Make sure the Face Connectivity is created because the FindNRing algorithm needs this and will
  // assert if the data is NOT in the SurfaceMesh Data Container
  ElementDynamicList::Pointer vertLinks = triangleGeom->getElementsContainingVert();
  if(nullptr == vertLinks.get())
  {
    triangleGeom->findElementsContainingVert();
  }

  // get the QMap from the SharedFeatureFaces filter
  SharedFeatureFaces_t sharedFeatureFaces;

  int32_t maxFaceId = 0;
  for(int64_t t = 0; t < numTriangles; ++t)
  {
    if(m_SurfaceMeshFeatureFaceIds[t] > maxFaceId)
    {
      maxFaceId = m_SurfaceMeshFeatureFaceIds[t];
    }
  }
  std::vector<int32_t> faceSizes(maxFaceId + 1, 0);
  // Loop through all the Triangles and assign each one to a unique Feature Face Id.
  for(int64_t t = 0; t < numTriangles; ++t)
  {
    faceSizes[m_SurfaceMeshFeatureFaceIds[t]]++;
  }

  // Allocate all the vectors that we need
  for(size_t iter = 0; iter < faceSizes.size(); ++iter)
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
    if(doParallel == true)
    {
      g->run(CalculateTriangleGroupCurvatures(m_NRing, triangleIds, m_UseNormalsForCurveFitting, m_SurfaceMeshPrincipalCurvature1sPtr.lock(), m_SurfaceMeshPrincipalCurvature2sPtr.lock(),
                                              m_SurfaceMeshPrincipalDirection1sPtr.lock(), m_SurfaceMeshPrincipalDirection2sPtr.lock(), m_SurfaceMeshGaussianCurvaturesPtr.lock(),
                                              m_SurfaceMeshMeanCurvaturesPtr.lock(), triangleGeom, m_SurfaceMeshFaceLabelsPtr.lock(), m_SurfaceMeshFaceNormalsPtr.lock(),
                                              m_SurfaceMeshTriangleCentroidsPtr.lock(), this));
    }
    else
#endif
    {
      CalculateTriangleGroupCurvatures curvature(m_NRing, triangleIds, m_UseNormalsForCurveFitting, m_SurfaceMeshPrincipalCurvature1sPtr.lock(), m_SurfaceMeshPrincipalCurvature2sPtr.lock(),
                                                 m_SurfaceMeshPrincipalDirection1sPtr.lock(), m_SurfaceMeshPrincipalDirection2sPtr.lock(), m_SurfaceMeshGaussianCurvaturesPtr.lock(),
                                                 m_SurfaceMeshMeanCurvaturesPtr.lock(), triangleGeom, m_SurfaceMeshFaceLabelsPtr.lock(), m_SurfaceMeshFaceNormalsPtr.lock(),
                                                 m_SurfaceMeshTriangleCentroidsPtr.lock(), this);
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
{
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureFaceCurvatureFilter::getBrandingString()
{
  return "SurfaceMeshing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureFaceCurvatureFilter::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureFaceCurvatureFilter::getGroupName()
{
  return SIMPL::FilterGroups::SurfaceMeshingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureFaceCurvatureFilter::getSubGroupName()
{
  return SIMPL::FilterSubGroups::CurvatureFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FeatureFaceCurvatureFilter::getHumanLabel()
{
  return "Find Feature Face Curvature";
}
