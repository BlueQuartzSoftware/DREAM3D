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

#include <memory>

#include "FindTriangleGeomSizes.h"

#include <set>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Math/MatrixMath.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
enum createdPathID : RenameDataPath::DataID_t
{
  DataArrayID30 = 30,
  DataArrayID31 = 31,
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTriangleGeomSizes::FindTriangleGeomSizes()
: m_FaceLabelsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels)
, m_FeatureAttributeMatrixName(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceFeatureAttributeMatrixName, "")
, m_VolumesArrayName(SIMPL::FeatureData::Volumes)
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTriangleGeomSizes::~FindTriangleGeomSizes() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomSizes::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomSizes::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  DataArraySelectionFilterParameter::RequirementType dasReq = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Labels", FaceLabelsArrayPath, FilterParameter::RequiredArray, FindTriangleGeomSizes, dasReq));
  parameters.push_back(SeparatorFilterParameter::New("Face Feature Data", FilterParameter::RequiredArray));
  AttributeMatrixSelectionFilterParameter::RequirementType amsReq = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::FaceFeature, IGeometry::Type::Triangle);
  parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Face Feature Attribute Matrix", FeatureAttributeMatrixName, FilterParameter::RequiredArray, FindTriangleGeomSizes, amsReq));
  parameters.push_back(SeparatorFilterParameter::New("Face Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Volumes", VolumesArrayName, FaceLabelsArrayPath, FeatureAttributeMatrixName, FilterParameter::CreatedArray, FindTriangleGeomSizes));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomSizes::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  QVector<IDataArray::Pointer> dataArrays;

  TriangleGeom::Pointer triangles = getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, getFaceLabelsArrayPath().getDataContainerName());

  if(getErrorCode() < 0)
  {
    return;
  }

  dataArrays.push_back(triangles->getTriangles());

  std::vector<size_t> cDims(1, 2);

  m_FaceLabelsPtr =
      getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFaceLabelsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FaceLabelsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FaceLabels = m_FaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrays.push_back(m_FaceLabelsPtr.lock());
  }

  getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getFeatureAttributeMatrixName(), -301);

  DataArrayPath path(getFaceLabelsArrayPath().getDataContainerName(), getFeatureAttributeMatrixName().getAttributeMatrixName(), getVolumesArrayName());

  cDims[0] = 1;

  m_VolumesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, path, 0, cDims, "", DataArrayID31);
  if(nullptr != m_VolumesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
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
float FindTriangleGeomSizes::findTetrahedronVolume(MeshIndexType vertIds[3], float* vertPtr)
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
  if(getErrorCode() < 0)
  {
    return;
  }

  TriangleGeom::Pointer triangles = getDataContainerArray()->getDataContainer(m_FaceLabelsArrayPath.getDataContainerName())->getGeometryAs<TriangleGeom>();
  float* vertPtr = triangles->getVertexPointer(0);

  std::set<int32_t> featureSet;
  MeshIndexType numTriangles = triangles->getNumberOfTris();

  for(MeshIndexType i = 0; i < numTriangles; i++)
  {
    if(m_FaceLabels[2 * i + 0] > 0) { featureSet.insert(m_FaceLabels[2 * i + 0]); }
    if(m_FaceLabels[2 * i + 1] > 0) { featureSet.insert(m_FaceLabels[2 * i + 1]); }
  }

  std::vector<size_t> tDims(1, featureSet.size() + 1);
  AttributeMatrix::Pointer featAttrMat = getDataContainerArray()->getDataContainer(m_FaceLabelsArrayPath.getDataContainerName())->getAttributeMatrix(m_FeatureAttributeMatrixName);
  featAttrMat->resizeAttributeArrays(tDims);
  m_Volumes = m_VolumesPtr.lock()->getPointer(0);

  MeshIndexType vertsAtTri[3] = {0, 0, 0};

  for(MeshIndexType i = 0; i < numTriangles; i++)
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

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindTriangleGeomSizes::newFilterInstance(bool copyFilterParameters) const
{
  FindTriangleGeomSizes::Pointer filter = FindTriangleGeomSizes::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindTriangleGeomSizes::getCompiledLibraryName() const
{ return SurfaceMeshingConstants::SurfaceMeshingBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindTriangleGeomSizes::getBrandingString() const
{ return "SurfaceMeshing"; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindTriangleGeomSizes::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindTriangleGeomSizes::getGroupName() const
{ return SIMPL::FilterGroups::StatisticsFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid FindTriangleGeomSizes::getUuid() const
{
  return QUuid("{9157ef1c-7cbc-5840-b6e7-26089c0b0f88}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindTriangleGeomSizes::getSubGroupName() const
{ return SIMPL::FilterSubGroups::MorphologicalFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindTriangleGeomSizes::getHumanLabel() const
{ return "Find Feature Volumes from Triangle Geometry"; }

// -----------------------------------------------------------------------------
FindTriangleGeomSizes::Pointer FindTriangleGeomSizes::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<FindTriangleGeomSizes> FindTriangleGeomSizes::New()
{
  struct make_shared_enabler : public FindTriangleGeomSizes
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString FindTriangleGeomSizes::getNameOfClass() const
{
  return QString("FindTriangleGeomSizes");
}

// -----------------------------------------------------------------------------
QString FindTriangleGeomSizes::ClassName()
{
  return QString("FindTriangleGeomSizes");
}

// -----------------------------------------------------------------------------
void FindTriangleGeomSizes::setFaceLabelsArrayPath(const DataArrayPath& value)
{
  m_FaceLabelsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindTriangleGeomSizes::getFaceLabelsArrayPath() const
{
  return m_FaceLabelsArrayPath;
}

// -----------------------------------------------------------------------------
void FindTriangleGeomSizes::setFeatureAttributeMatrixName(const DataArrayPath& value)
{
  m_FeatureAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindTriangleGeomSizes::getFeatureAttributeMatrixName() const
{
  return m_FeatureAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void FindTriangleGeomSizes::setVolumesArrayName(const QString& value)
{
  m_VolumesArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindTriangleGeomSizes::getVolumesArrayName() const
{
  return m_VolumesArrayName;
}
