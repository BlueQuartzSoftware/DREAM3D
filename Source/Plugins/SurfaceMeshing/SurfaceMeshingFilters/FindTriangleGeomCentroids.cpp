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
#include "FindTriangleGeomCentroids.h"

#include <set>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AttributeMatrixSelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Math/MatrixMath.h"

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
FindTriangleGeomCentroids::FindTriangleGeomCentroids()
{
  initialize();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTriangleGeomCentroids::~FindTriangleGeomCentroids() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomCentroids::initialize()
{
  clearErrorCode();
  clearWarningCode();
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomCentroids::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SeparatorFilterParameter::Create("Face Data", FilterParameter::Category::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType dasReq =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Labels", FaceLabelsArrayPath, FilterParameter::Category::RequiredArray, FindTriangleGeomCentroids, dasReq));
  }
  parameters.push_back(SeparatorFilterParameter::Create("Face Feature Data", FilterParameter::Category::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::FaceFeature, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Face Feature Attribute Matrix", FeatureAttributeMatrixName, FilterParameter::Category::RequiredArray, FindTriangleGeomCentroids, req));
  }
  parameters.push_back(SeparatorFilterParameter::Create("Face Feature Data", FilterParameter::Category::CreatedArray));
  parameters.push_back(
      SIMPL_NEW_DA_WITH_LINKED_AM_FP("Centroids", CentroidsArrayName, FeatureAttributeMatrixName, FeatureAttributeMatrixName, FilterParameter::Category::CreatedArray, FindTriangleGeomCentroids));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomCentroids::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  QVector<IDataArray::Pointer> dataArrays;

  TriangleGeom::Pointer triangles = getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom>(this, getFaceLabelsArrayPath().getDataContainerName());

  if(getErrorCode() < 0)
  {
    return;
  }

  dataArrays.push_back(triangles->getTriangles());

  std::vector<size_t> cDims(1, 2);

  m_FaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>>(this, getFaceLabelsArrayPath(), cDims);
  if(nullptr != m_FaceLabelsPtr.lock())
  {
    m_FaceLabels = m_FaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrays.push_back(m_FaceLabelsPtr.lock());
  }

  DataContainer::Pointer tdc = getDataContainerArray()->getPrereqDataContainer(this, getFaceLabelsArrayPath().getDataContainerName());
  if(getErrorCode() < 0)
  {
    return;
  }

  DataArrayPath path(getFeatureAttributeMatrixName().getDataContainerName(), getFeatureAttributeMatrixName().getAttributeMatrixName(), getCentroidsArrayName());
  cDims[0] = 3;
  m_CentroidsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>>(this, path, 0.0, cDims, "", DataArrayID31);
  if(nullptr != m_CentroidsPtr.lock())
  {
    m_Centroids = m_CentroidsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomCentroids::execute()
{
  initialize();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  TriangleGeom::Pointer triangles = getDataContainerArray()->getDataContainer(m_FaceLabelsArrayPath.getDataContainerName())->getGeometryAs<TriangleGeom>();
  float* vertPtr = triangles->getVertexPointer(0);

  MeshIndexType numTriangles = triangles->getNumberOfTris();
  MeshIndexType* tris = triangles->getTriPointer(0);

  MeshIndexType numFeatures = m_CentroidsPtr.lock()->getNumberOfTuples();
  std::vector<std::set<MeshIndexType>> vertexSets(numFeatures + 1);

  for(MeshIndexType i = 0; i < numTriangles; i++)
  {
    if ((m_FaceLabels[2 * i + 0] > 0 && m_FaceLabels[2 * i + 0] >= numFeatures + 1) 
    || (m_FaceLabels[2 * i + 1] > 0 && m_FaceLabels[2 * i + 1] >= numFeatures + 1))
    {
      QString msg = QString("FaceLabels have a value %1 or %2 which is larger then the number of features %3").arg(m_FaceLabels[2 * i + 0]).arg(m_FaceLabels[2 * i + 1]).arg(numFeatures);
      setErrorCondition(-99500, msg);
      return;
    }
    if(m_FaceLabels[2 * i + 0] > 0)
    {
      vertexSets[m_FaceLabels[2 * i + 0]].insert(tris[3 * i + 0]);
      vertexSets[m_FaceLabels[2 * i + 0]].insert(tris[3 * i + 1]);
      vertexSets[m_FaceLabels[2 * i + 0]].insert(tris[3 * i + 2]);
    }
    if(m_FaceLabels[2 * i + 1] > 0)
    {
      vertexSets[m_FaceLabels[2 * i + 1]].insert(tris[3 * i + 0]);
      vertexSets[m_FaceLabels[2 * i + 1]].insert(tris[3 * i + 1]);
      vertexSets[m_FaceLabels[2 * i + 1]].insert(tris[3 * i + 2]);
    }
    if(getCancel())
    {
      break;
    }
  }

  std::set<MeshIndexType>::iterator it;
  for(MeshIndexType i = 0; i < numFeatures; i++)
  {
    for(it = vertexSets[i].begin(); it != vertexSets[i].end(); ++it)
    {
      int32_t vert = *it;
      m_Centroids[3 * i + 0] += vertPtr[3 * vert + 0];
      m_Centroids[3 * i + 1] += vertPtr[3 * vert + 1];
      m_Centroids[3 * i + 2] += vertPtr[3 * vert + 2];
    }
    if(!vertexSets[i].empty())
    {
      m_Centroids[3 * i + 0] /= vertexSets[i].size();
      m_Centroids[3 * i + 1] /= vertexSets[i].size();
      m_Centroids[3 * i + 2] /= vertexSets[i].size();
    }
    vertexSets[i].clear();

    if(getCancel())
    {
      break;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindTriangleGeomCentroids::newFilterInstance(bool copyFilterParameters) const
{
  FindTriangleGeomCentroids::Pointer filter = FindTriangleGeomCentroids::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindTriangleGeomCentroids::getCompiledLibraryName() const
{
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindTriangleGeomCentroids::getBrandingString() const
{
  return "SurfaceMeshing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindTriangleGeomCentroids::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindTriangleGeomCentroids::getGroupName() const
{
  return SIMPL::FilterGroups::Generic;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid FindTriangleGeomCentroids::getUuid() const
{
  return QUuid("{a5cff82b-9fe4-5a8c-90c9-6db74b6dcd50}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindTriangleGeomCentroids::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MorphologicalFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindTriangleGeomCentroids::getHumanLabel() const
{
  return "Find Feature Centroids from Triangle Geometry";
}

// -----------------------------------------------------------------------------
FindTriangleGeomCentroids::Pointer FindTriangleGeomCentroids::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<FindTriangleGeomCentroids> FindTriangleGeomCentroids::New()
{
  struct make_shared_enabler : public FindTriangleGeomCentroids
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString FindTriangleGeomCentroids::getNameOfClass() const
{
  return QString("FindTriangleGeomCentroids");
}

// -----------------------------------------------------------------------------
QString FindTriangleGeomCentroids::ClassName()
{
  return QString("FindTriangleGeomCentroids");
}

// -----------------------------------------------------------------------------
void FindTriangleGeomCentroids::setFeatureAttributeMatrixName(const DataArrayPath& value)
{
  m_FeatureAttributeMatrixName = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindTriangleGeomCentroids::getFeatureAttributeMatrixName() const
{
  return m_FeatureAttributeMatrixName;
}

// -----------------------------------------------------------------------------
void FindTriangleGeomCentroids::setFaceLabelsArrayPath(const DataArrayPath& value)
{
  m_FaceLabelsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindTriangleGeomCentroids::getFaceLabelsArrayPath() const
{
  return m_FaceLabelsArrayPath;
}

// -----------------------------------------------------------------------------
void FindTriangleGeomCentroids::setCentroidsArrayName(const QString& value)
{
  m_CentroidsArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindTriangleGeomCentroids::getCentroidsArrayName() const
{
  return m_CentroidsArrayName;
}
