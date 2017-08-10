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

#include "FindTriangleGeomSizes.h"

#include <set>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Math/MatrixMath.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"

// Include the MOC generated file for this class
#include "moc_FindTriangleGeomSizes.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTriangleGeomSizes::FindTriangleGeomSizes() :
  AbstractFilter(),
  m_FaceLabelsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels)
, m_FeatureAttributeMatrixName(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceFeatureAttributeMatrixName, "")
, m_VolumesArrayName(SIMPL::FeatureData::Volumes)
, m_FaceLabels(nullptr)
, m_Volumes(nullptr)
{
  initialize();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTriangleGeomSizes::~FindTriangleGeomSizes()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomSizes::initialize()
{
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomSizes::setupFilterParameters()
{
  FilterParameterVector parameters;
  DataArraySelectionFilterParameter::RequirementType dasReq = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Labels", FaceLabelsArrayPath, FilterParameter::RequiredArray, FindTriangleGeomSizes, dasReq));
  parameters.push_back(SeparatorFilterParameter::New("Face Feature Data", FilterParameter::RequiredArray));
  AttributeMatrixSelectionFilterParameter::RequirementType amsReq = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::FaceFeature, IGeometry::Type::Triangle);
  parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Face Feature Attribute Matrix", FeatureAttributeMatrixName, FilterParameter::RequiredArray, FindTriangleGeomSizes, amsReq));
  parameters.push_back(SeparatorFilterParameter::New("Face Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Volumes", VolumesArrayName, FilterParameter::CreatedArray, FindTriangleGeomSizes));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomSizes::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  QVector<IDataArray::Pointer> dataArrays;

  TriangleGeom::Pointer triangles = getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, getFaceLabelsArrayPath().getDataContainerName());

  if(getErrorCondition() < 0) { return; }

  dataArrays.push_back(triangles->getTriangles());

  QVector<size_t> cDims(1, 2);

  m_FaceLabelsPtr =
      getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFaceLabelsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FaceLabelsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FaceLabels = m_FaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0) { dataArrays.push_back(m_FaceLabelsPtr.lock()); }

  getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getFeatureAttributeMatrixName(), -301);

  DataArrayPath path(getFaceLabelsArrayPath().getDataContainerName(), getFeatureAttributeMatrixName().getAttributeMatrixName(), getVolumesArrayName());

  cDims[0] = 1;

  m_VolumesPtr =
      getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, path, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_VolumesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Volumes = m_VolumesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomSizes::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float FindTriangleGeomSizes::findTetrahedronVolume(int64_t vertIds[3], float* vertPtr)
{
  float vertMatrix[3][3] = { { vertPtr[3 * vertIds[1] + 0] - vertPtr[3 * vertIds[0] + 0],
                               vertPtr[3 * vertIds[2] + 0] - vertPtr[3 * vertIds[0] + 0],
                                                      0.0f - vertPtr[3 * vertIds[0] + 0] },
                             { vertPtr[3 * vertIds[1] + 1] - vertPtr[3 * vertIds[0] + 1],
                               vertPtr[3 * vertIds[2] + 1] - vertPtr[3 * vertIds[0] + 1],
                                                      0.0f - vertPtr[3 * vertIds[0] + 1] },
                             { vertPtr[3 * vertIds[1] + 2] - vertPtr[3 * vertIds[0] + 2],
                               vertPtr[3 * vertIds[2] + 2] - vertPtr[3 * vertIds[0] + 2],
                                                      0.0f - vertPtr[3 * vertIds[0] + 2] }
                           };

  return (MatrixMath::Determinant3x3(vertMatrix) / 6.0f);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomSizes::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  TriangleGeom::Pointer triangles = getDataContainerArray()->getDataContainer(m_FaceLabelsArrayPath.getDataContainerName())->getGeometryAs<TriangleGeom>();
  float* vertPtr = triangles->getVertexPointer(0);

  std::set<int32_t> featureSet;
  int64_t numTriangles = triangles->getNumberOfTris();

  for(int64_t i = 0; i < numTriangles; i++)
  {
    if(m_FaceLabels[2 * i + 0] > 0) { featureSet.insert(m_FaceLabels[2 * i + 0]); }
    if(m_FaceLabels[2 * i + 1] > 0) { featureSet.insert(m_FaceLabels[2 * i + 1]); }
  }

  QVector<size_t> tDims(1, featureSet.size() + 1);
  AttributeMatrix::Pointer featAttrMat = getDataContainerArray()->getDataContainer(m_FaceLabelsArrayPath.getDataContainerName())->getAttributeMatrix(m_FeatureAttributeMatrixName);
  featAttrMat->resizeAttributeArrays(tDims);
  m_Volumes = m_VolumesPtr.lock()->getPointer(0);

  int64_t vertsAtTri[3] = {0, 0, 0};

  for(int64_t i = 0; i < numTriangles; i++)
  {
    triangles->getVertsAtTri(i, vertsAtTri);
    if(m_FaceLabels[2 * i + 0] == -1)
    {
      std::swap(vertsAtTri[2], vertsAtTri[1]);
      m_Volumes[m_FaceLabels[2 * i + 1]] += findTetrahedronVolume(vertsAtTri, vertPtr);
    }
    else if(m_FaceLabels[2 * i + 1] == -1)
    {
      m_Volumes[m_FaceLabels[2 * i + 0]] += findTetrahedronVolume(vertsAtTri, vertPtr);
    }
    else
    {
      m_Volumes[m_FaceLabels[2 * i + 0]] += findTetrahedronVolume(vertsAtTri, vertPtr);
      std::swap(vertsAtTri[2], vertsAtTri[1]);
      m_Volumes[m_FaceLabels[2 * i + 1]] += findTetrahedronVolume(vertsAtTri, vertPtr);
    }
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindTriangleGeomSizes::newFilterInstance(bool copyFilterParameters)
{
  FindTriangleGeomSizes::Pointer filter = FindTriangleGeomSizes::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomSizes::getCompiledLibraryName()
{ return SurfaceMeshingConstants::SurfaceMeshingBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomSizes::getBrandingString()
{ return "SurfaceMeshing"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomSizes::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomSizes::getGroupName()
{ return SIMPL::FilterGroups::StatisticsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomSizes::getSubGroupName()
{ return SIMPL::FilterSubGroups::MorphologicalFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomSizes::getHumanLabel()
{ return "Find Feature Volumes from Triangle Geometry"; }

