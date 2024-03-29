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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "SharedFeatureFaceFilter.h"

#include <QtCore/QTextStream>

#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
enum createdPathID : RenameDataPath::DataID_t
{
  AttributeMatrixID20 = 20,
  AttributeMatrixID21 = 21,
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedFeatureFaceFilter::SharedFeatureFaceFilter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedFeatureFaceFilter::~SharedFeatureFaceFilter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SeparatorFilterParameter::Create("Face Data", FilterParameter::Category::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Labels", SurfaceMeshFaceLabelsArrayPath, FilterParameter::Category::RequiredArray, SharedFeatureFaceFilter, req));
  }
  parameters.push_back(SeparatorFilterParameter::Create("Face Data", FilterParameter::Category::CreatedArray));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Feature Face Ids", SurfaceMeshFeatureFaceIdsArrayName, SurfaceMeshFaceLabelsArrayPath, SurfaceMeshFaceLabelsArrayPath,
                                                      FilterParameter::Category::CreatedArray, SharedFeatureFaceFilter));
  parameters.push_back(SeparatorFilterParameter::Create("Face Feature Data", FilterParameter::Category::CreatedArray));
  parameters.push_back(SIMPL_NEW_AM_WITH_LINKED_DC_FP("Face Feature Attribute Matrix", FaceFeatureAttributeMatrixName, SurfaceMeshFaceLabelsArrayPath, FilterParameter::Category::CreatedArray,
                                                      SharedFeatureFaceFilter));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Face Labels", SurfaceMeshFeatureFaceLabelsArrayName, SurfaceMeshFaceLabelsArrayPath, FaceFeatureAttributeMatrixName,
                                                      FilterParameter::Category::CreatedArray, SharedFeatureFaceFilter));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Number of Triangles", SurfaceMeshFeatureFaceNumTrianglesArrayName, SurfaceMeshFaceLabelsArrayPath, FaceFeatureAttributeMatrixName,
                                                      FilterParameter::Category::CreatedArray, SharedFeatureFaceFilter));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFaceFeatureAttributeMatrixName(reader->readString("FaceFeatureAttributeMatrixName", getFaceFeatureAttributeMatrixName()));
  setSurfaceMeshFeatureFaceIdsArrayName(reader->readString("SurfaceMeshFeatureFaceIdsArrayName", getSurfaceMeshFeatureFaceIdsArrayName()));
  setSurfaceMeshFeatureFaceLabelsArrayName(reader->readString("SurfaceMeshFeatureFaceLabelsArrayName", getSurfaceMeshFeatureFaceLabelsArrayName()));
  setSurfaceMeshFeatureFaceNumTrianglesArrayName(reader->readString("SurfaceMeshFeatureFaceNumTrianglesArrayName", getSurfaceMeshFeatureFaceNumTrianglesArrayName()));
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  DataArrayPath tempPath;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom>(this, getSurfaceMeshFaceLabelsArrayPath().getDataContainerName());

  DataContainer::Pointer sm = getDataContainerArray()->getPrereqDataContainer(this, m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), false);
  if(getErrorCode() < 0)
  {
    return;
  }

  std::vector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer facefeatureAttrMat = sm->createNonPrereqAttributeMatrix(this, getFaceFeatureAttributeMatrixName(), tDims, AttributeMatrix::Type::FaceFeature, AttributeMatrixID21);
  if(getErrorCode() < 0 || nullptr == facefeatureAttrMat.get())
  {
    return;
  }

  std::vector<size_t> cDims(1, 2);
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>>(this, getSurfaceMeshFaceLabelsArrayPath(), cDims);
  if(nullptr != m_SurfaceMeshFaceLabelsPtr.lock())
  {
    m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 1;
  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), m_SurfaceMeshFaceLabelsArrayPath.getAttributeMatrixName(), getSurfaceMeshFeatureFaceIdsArrayName());
  m_SurfaceMeshFeatureFaceIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>>(this, tempPath, 0, cDims);
  if(nullptr != m_SurfaceMeshFeatureFaceIdsPtr.lock())
  {
    m_SurfaceMeshFeatureFaceIds = m_SurfaceMeshFeatureFaceIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), getFaceFeatureAttributeMatrixName(), getSurfaceMeshFeatureFaceNumTrianglesArrayName());
  m_SurfaceMeshFeatureFaceNumTrianglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>>(this, tempPath, 0, cDims);
  if(nullptr != m_SurfaceMeshFeatureFaceNumTrianglesPtr.lock())
  {
    m_SurfaceMeshFeatureFaceNumTriangles = m_SurfaceMeshFeatureFaceNumTrianglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 2;
  tempPath.update(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName(), getFaceFeatureAttributeMatrixName(), getSurfaceMeshFeatureFaceLabelsArrayName());
  m_SurfaceMeshFeatureFaceLabelsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>>(this, tempPath, 0, cDims);
  if(nullptr != m_SurfaceMeshFeatureFaceLabelsPtr.lock())
  {
    m_SurfaceMeshFeatureFaceLabels = m_SurfaceMeshFeatureFaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::execute()
{
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  DataContainer::Pointer sm = getDataContainerArray()->getDataContainer(m_SurfaceMeshFaceLabelsArrayPath.getDataContainerName());
  AttributeMatrix::Pointer faceFeatureAttrMat = sm->getAttributeMatrix(getFaceFeatureAttributeMatrixName());

  TriangleGeom::Pointer triangleGeom = sm->getGeometryAs<TriangleGeom>();
  int64_t totalPoints = triangleGeom->getNumberOfTris();

  QMap<uint64_t, int32_t> faceSizeMap;
  QMap<uint64_t, int32_t> faceIdMap; // This maps a unique 64 bit integer to an increasing 32 bit integer
  int32_t index = 1;
  struct
  {
    int g;
    int r;
  } faceId;
  uint64_t* faceId_64 = reinterpret_cast<uint64_t*>(&faceId);
  int32_t fl0 = -1;
  int32_t fl1 = -1;

  std::vector<std::pair<int32_t, int32_t>> faceLabelMap;
  faceLabelMap.push_back(std::pair<int32_t, int32_t>(0, 0));

  // Loop through all the Triangles and figure out how many triangles we have in each one.
  for(int64_t t = 0; t < totalPoints; ++t)
  {
    fl0 = m_SurfaceMeshFaceLabels[t * 2];
    fl1 = m_SurfaceMeshFaceLabels[t * 2 + 1];
    if(fl0 < fl1)
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
    if(iter == faceSizeMap.end())
    {
      faceSizeMap[*faceId_64] = 1;
      faceIdMap[*faceId_64] = index;
      m_SurfaceMeshFeatureFaceIds[t] = index;
      faceLabelMap.push_back(std::pair<int32_t, int32_t>(faceId.g, faceId.r));
      ++index;
    }
    else
    {
      iter.value()++;
      m_SurfaceMeshFeatureFaceIds[t] = faceIdMap[*faceId_64];
    }
  }

  // resize + update pointers
  std::vector<size_t> tDims(1, index);
  faceFeatureAttrMat->resizeAttributeArrays(tDims);
  m_SurfaceMeshFeatureFaceLabels = m_SurfaceMeshFeatureFaceLabelsPtr.lock()->getPointer(0);
  m_SurfaceMeshFeatureFaceNumTriangles = m_SurfaceMeshFeatureFaceNumTrianglesPtr.lock()->getPointer(0);

  for(int32_t i = 0; i < index; i++)
  {
    // get feature face labels
    m_SurfaceMeshFeatureFaceLabels[2 * i + 0] = faceLabelMap[i].first;
    m_SurfaceMeshFeatureFaceLabels[2 * i + 1] = faceLabelMap[i].second;

    // get feature triangle count
    faceId.g = faceLabelMap[i].first;
    faceId.r = faceLabelMap[i].second;
    m_SurfaceMeshFeatureFaceNumTriangles[i] = faceSizeMap[*faceId_64];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer SharedFeatureFaceFilter::newFilterInstance(bool copyFilterParameters) const
{
  SharedFeatureFaceFilter::Pointer filter = SharedFeatureFaceFilter::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SharedFeatureFaceFilter::getCompiledLibraryName() const
{
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SharedFeatureFaceFilter::getBrandingString() const
{
  return "SurfaceMeshing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SharedFeatureFaceFilter::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SharedFeatureFaceFilter::getGroupName() const
{
  return SIMPL::FilterGroups::SurfaceMeshingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid SharedFeatureFaceFilter::getUuid() const
{
  return QUuid("{15c743db-5936-53a2-92cf-edf00526486d}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SharedFeatureFaceFilter::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::ConnectivityArrangementFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SharedFeatureFaceFilter::getHumanLabel() const
{
  return "Generate Triangle Face Ids";
}

// -----------------------------------------------------------------------------
SharedFeatureFaceFilter::Pointer SharedFeatureFaceFilter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<SharedFeatureFaceFilter> SharedFeatureFaceFilter::New()
{
  struct make_shared_enabler : public SharedFeatureFaceFilter
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString SharedFeatureFaceFilter::getNameOfClass() const
{
  return QString("SharedFeatureFaceFilter");
}

// -----------------------------------------------------------------------------
QString SharedFeatureFaceFilter::ClassName()
{
  return QString("SharedFeatureFaceFilter");
}

// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::setFaceFeatureAttributeMatrixName(const QString& value)
{
  m_FaceFeatureAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
QString SharedFeatureFaceFilter::getFaceFeatureAttributeMatrixName() const
{
  return m_FaceFeatureAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::setSurfaceMeshFeatureFaceIdsArrayName(const QString& value)
{
  m_SurfaceMeshFeatureFaceIdsArrayName = value;
}

// -----------------------------------------------------------------------------
QString SharedFeatureFaceFilter::getSurfaceMeshFeatureFaceIdsArrayName() const
{
  return m_SurfaceMeshFeatureFaceIdsArrayName;
}

// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::setSurfaceMeshFeatureFaceLabelsArrayName(const QString& value)
{
  m_SurfaceMeshFeatureFaceLabelsArrayName = value;
}

// -----------------------------------------------------------------------------
QString SharedFeatureFaceFilter::getSurfaceMeshFeatureFaceLabelsArrayName() const
{
  return m_SurfaceMeshFeatureFaceLabelsArrayName;
}

// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::setSurfaceMeshFeatureFaceNumTrianglesArrayName(const QString& value)
{
  m_SurfaceMeshFeatureFaceNumTrianglesArrayName = value;
}

// -----------------------------------------------------------------------------
QString SharedFeatureFaceFilter::getSurfaceMeshFeatureFaceNumTrianglesArrayName() const
{
  return m_SurfaceMeshFeatureFaceNumTrianglesArrayName;
}

// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::setSurfaceMeshFaceLabelsArrayPath(const DataArrayPath& value)
{
  m_SurfaceMeshFaceLabelsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath SharedFeatureFaceFilter::getSurfaceMeshFaceLabelsArrayPath() const
{
  return m_SurfaceMeshFaceLabelsArrayPath;
}
