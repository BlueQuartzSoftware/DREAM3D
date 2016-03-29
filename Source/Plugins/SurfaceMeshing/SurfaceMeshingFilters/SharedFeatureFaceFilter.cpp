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

#include "SharedFeatureFaceFilter.h"

#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"

// Include the MOC generated file for this class
#include "moc_SharedFeatureFaceFilter.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedFeatureFaceFilter::SharedFeatureFaceFilter() :
  SurfaceMeshFilter(),
  m_FaceFeatureAttributeMatrixName(SIMPL::Defaults::FaceFeatureAttributeMatrixName),
  m_SurfaceMeshFeatureFaceIdsArrayName(SIMPL::FaceData::SurfaceMeshFeatureFaceId),
  m_SurfaceMeshFeatureFaceLabelsArrayName(SIMPL::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFeatureFaceNumTrianglesArrayName("NumTriangles"),
  m_SurfaceMeshFaceLabelsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabels(NULL),
  m_SurfaceMeshFeatureFaceIds(NULL),
  m_SurfaceMeshFeatureFaceLabels(NULL),
  m_SurfaceMeshFeatureFaceNumTriangles(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedFeatureFaceFilter::~SharedFeatureFaceFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, SIMPL::AttributeMatrixType::Face, SIMPL::GeometryType::TriangleGeometry);
    parameters.push_back(DataArraySelectionFilterParameter::New("Face Labels", "SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath(), FilterParameter::RequiredArray, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Feature Face Ids", "SurfaceMeshFeatureFaceIdsArrayName", getSurfaceMeshFeatureFaceIdsArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(SeparatorFilterParameter::New("Face Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Face Feature Attribute Matrix", "FaceFeatureAttributeMatrixName", getFaceFeatureAttributeMatrixName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Face Labels", "SurfaceMeshFeatureFaceLabelsArrayName", getSurfaceMeshFeatureFaceLabelsArrayName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Number of Triangles", "SurfaceMeshFeatureFaceNumTrianglesArrayName", getSurfaceMeshFeatureFaceNumTrianglesArrayName(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFaceFeatureAttributeMatrixName(reader->readString("FaceFeatureAttributeMatrixName", getFaceFeatureAttributeMatrixName() ) );
  setSurfaceMeshFeatureFaceIdsArrayName(reader->readString("SurfaceMeshFeatureFaceIdsArrayName", getSurfaceMeshFeatureFaceIdsArrayName() ) );
  setSurfaceMeshFeatureFaceLabelsArrayName(reader->readString("SurfaceMeshFeatureFaceLabelsArrayName", getSurfaceMeshFeatureFaceLabelsArrayName() ) );
  setSurfaceMeshFeatureFaceNumTrianglesArrayName(reader->readString("SurfaceMeshFeatureFaceNumTrianglesArrayName", getSurfaceMeshFeatureFaceNumTrianglesArrayName() ) );
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SharedFeatureFaceFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(FaceFeatureAttributeMatrixName)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshFeatureFaceIdsArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshFeatureFaceLabelsArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshFeatureFaceNumTrianglesArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(SurfaceMeshFaceLabelsArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());

  DataContainer::Pointer sm = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer facefeatureAttrMat = sm->createNonPrereqAttributeMatrix<AbstractFilter>(this, getFaceFeatureAttributeMatrixName(), tDims, SIMPL::AttributeMatrixType::FaceFeature);
  if(getErrorCondition() < 0 || NULL == facefeatureAttrMat.get()) { return; }

  QVector<size_t> cDims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 1;
  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), m_SurfaceMeshFaceLabelsArrayPath.getAttributeMatrixName(), getSurfaceMeshFeatureFaceIdsArrayName() );
  m_SurfaceMeshFeatureFaceIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFeatureFaceIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFeatureFaceIds = m_SurfaceMeshFeatureFaceIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), getFaceFeatureAttributeMatrixName(), getSurfaceMeshFeatureFaceNumTrianglesArrayName() );
  m_SurfaceMeshFeatureFaceNumTrianglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFeatureFaceNumTrianglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFeatureFaceNumTriangles = m_SurfaceMeshFeatureFaceNumTrianglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 2;
  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), getFaceFeatureAttributeMatrixName(), getSurfaceMeshFeatureFaceLabelsArrayName() );
  m_SurfaceMeshFeatureFaceLabelsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFeatureFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFeatureFaceLabels = m_SurfaceMeshFeatureFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::preflight()
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
void SharedFeatureFaceFilter::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName());
  AttributeMatrix::Pointer faceFeatureAttrMat = sm->getAttributeMatrix(getFaceFeatureAttributeMatrixName());

  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();
  int64_t totalPoints = triangleGeom->getNumberOfTris();

  QMap<uint64_t, int32_t> faceSizeMap;
  QMap<uint64_t, int32_t> faceIdMap; // This maps a unique 64 bit integer to an increasing 32 bit integer
  int32_t index = 1;
  struct { int g; int r; } faceId;
  uint64_t* faceId_64 = reinterpret_cast<uint64_t*>(&faceId);
  int32_t fl0 = -1;
  int32_t fl1 = -1;

  std::vector< std::pair<int32_t, int32_t> > faceLabelMap;
  faceLabelMap.push_back( std::pair<int32_t, int32_t>(0, 0) );

  // Loop through all the Triangles and figure out how many triangles we have in each one.
  for (int64_t t = 0; t < totalPoints; ++t)
  {
    fl0 = m_SurfaceMeshFaceLabels[t * 2];
    fl1 = m_SurfaceMeshFaceLabels[t * 2 + 1];
    if (fl0 < fl1)
    {
      faceId.g = fl0;
      faceId.r = fl1;
    }
    else
    {
      faceId.g = fl1;
      faceId.r = fl0;
    }

    QMap<uint64_t, int32_t>::iterator iter = faceSizeMap.find(*faceId_64);
    if (iter == faceSizeMap.end())
    {
      faceSizeMap[*faceId_64] = 1;
      faceIdMap[*faceId_64] = index;
      m_SurfaceMeshFeatureFaceIds[t] = index;
      faceLabelMap.push_back( std::pair<int32_t, int32_t>(faceId.g, faceId.r) );
      ++index;
    }
    else
    {
      iter.value()++;
      m_SurfaceMeshFeatureFaceIds[t] = faceIdMap[*faceId_64];
    }
  }

  // resize + update pointers
  QVector<size_t> tDims(1, index);
  faceFeatureAttrMat->resizeAttributeArrays(tDims);
  m_SurfaceMeshFeatureFaceLabels = m_SurfaceMeshFeatureFaceLabelsPtr.lock()->getPointer(0);
  m_SurfaceMeshFeatureFaceNumTriangles = m_SurfaceMeshFeatureFaceNumTrianglesPtr.lock()->getPointer(0);

  for (int32_t i = 0; i < index; i++)
  {
    // get feature face labels
    m_SurfaceMeshFeatureFaceLabels[2 * i + 0] = faceLabelMap[i].first;
    m_SurfaceMeshFeatureFaceLabels[2 * i + 1] = faceLabelMap[i].second;

    // get feature triangle count
    faceId.g = faceLabelMap[i].first;
    faceId.r = faceLabelMap[i].second;
    m_SurfaceMeshFeatureFaceNumTriangles[i] = faceSizeMap[*faceId_64];
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer SharedFeatureFaceFilter::newFilterInstance(bool copyFilterParameters)
{
  SharedFeatureFaceFilter::Pointer filter = SharedFeatureFaceFilter::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SharedFeatureFaceFilter::getCompiledLibraryName()
{
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SharedFeatureFaceFilter::getBrandingString()
{
  return "SurfaceMeshing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SharedFeatureFaceFilter::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SharedFeatureFaceFilter::getGroupName()
{ return SIMPL::FilterGroups::SurfaceMeshingFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SharedFeatureFaceFilter::getSubGroupName()
{ return SIMPL::FilterSubGroups::ConnectivityArrangementFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SharedFeatureFaceFilter::getHumanLabel()
{ return "Generate Triangle Face Ids"; }
