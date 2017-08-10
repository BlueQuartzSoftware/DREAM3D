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

#include "FindTriangleGeomCentroids.h"

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
#include "moc_FindTriangleGeomCentroids.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTriangleGeomCentroids::FindTriangleGeomCentroids()
: AbstractFilter()
, m_FeatureAttributeMatrixName(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceFeatureAttributeMatrixName, "")
, m_FaceLabelsArrayPath(SIMPL::Defaults::TriangleDataContainerName, SIMPL::Defaults::FaceAttributeMatrixName, SIMPL::FaceData::SurfaceMeshFaceLabels)
, m_CentroidsArrayName(SIMPL::FeatureData::Centroids)
, m_Centroids(nullptr)
{
  initialize();
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindTriangleGeomCentroids::~FindTriangleGeomCentroids()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomCentroids::initialize()
{
  setErrorCondition(0);
  setWarningCondition(0);
  setCancel(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomCentroids::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SeparatorFilterParameter::New("Face Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType dasReq =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 2, AttributeMatrix::Type::Face, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Face Labels", FaceLabelsArrayPath, FilterParameter::RequiredArray, FindTriangleGeomCentroids, dasReq));
  }
  parameters.push_back(SeparatorFilterParameter::New("Face Feature Data", FilterParameter::RequiredArray));
  {
    AttributeMatrixSelectionFilterParameter::RequirementType req = AttributeMatrixSelectionFilterParameter::CreateRequirement(AttributeMatrix::Type::FaceFeature, IGeometry::Type::Triangle);
    parameters.push_back(SIMPL_NEW_AM_SELECTION_FP("Face Feature Attribute Matrix", FeatureAttributeMatrixName, FilterParameter::RequiredArray, FindTriangleGeomCentroids, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Face Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Centroids", CentroidsArrayName, FilterParameter::CreatedArray, FindTriangleGeomCentroids));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomCentroids::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);

  QVector<IDataArray::Pointer> dataArrays;

  TriangleGeom::Pointer triangles = getDataContainerArray()->getPrereqGeometryFromDataContainer<TriangleGeom, AbstractFilter>(this, getFaceLabelsArrayPath().getDataContainerName());

  if(getErrorCondition() < 0)
  {
    return;
  }

  dataArrays.push_back(triangles->getTriangles());

  QVector<size_t> cDims(1, 2);

  m_FaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFaceLabelsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FaceLabelsPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FaceLabels = m_FaceLabelsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrays.push_back(m_FaceLabelsPtr.lock());
  }

  DataContainer::Pointer tdc = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getFaceLabelsArrayPath().getDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataArrayPath path(getFeatureAttributeMatrixName().getDataContainerName(), getFeatureAttributeMatrixName().getAttributeMatrixName(), getCentroidsArrayName());
  cDims[0] = 3;
  m_CentroidsPtr =
      getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, path, 0.0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CentroidsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Centroids = m_CentroidsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomCentroids::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true);              // Set the fact that we are preflighting.
  emit preflightAboutToExecute();    // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck();                       // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted();          // We are done preflighting this filter
  setInPreflight(false);             // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindTriangleGeomCentroids::execute()
{
  initialize();
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  TriangleGeom::Pointer triangles = getDataContainerArray()->getDataContainer(m_FaceLabelsArrayPath.getDataContainerName())->getGeometryAs<TriangleGeom>();
  float* vertPtr = triangles->getVertexPointer(0);

  int64_t numTriangles = triangles->getNumberOfTris();
  int64_t* tris = triangles->getTriPointer(0);

  int64_t numFeatures = m_CentroidsPtr.lock()->getNumberOfTuples();
  std::vector<std::set<int64_t>> vertexSets(numFeatures);

  for(int64_t i = 0; i < numTriangles; i++)
  {
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
  }

  std::set<int64_t>::iterator it;
  for(int64_t i = 0; i < numFeatures; i++)
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
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindTriangleGeomCentroids::newFilterInstance(bool copyFilterParameters)
{
  FindTriangleGeomCentroids::Pointer filter = FindTriangleGeomCentroids::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomCentroids::getCompiledLibraryName()
{
  return SurfaceMeshingConstants::SurfaceMeshingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomCentroids::getBrandingString()
{
  return "SurfaceMeshing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomCentroids::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SurfaceMeshing::Version::Major() << "." << SurfaceMeshing::Version::Minor() << "." << SurfaceMeshing::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomCentroids::getGroupName()
{
  return SIMPL::FilterGroups::Generic;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomCentroids::getSubGroupName()
{
  return SIMPL::FilterSubGroups::MorphologicalFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindTriangleGeomCentroids::getHumanLabel()
{
  return "Find Feature Centroids from Triangle Geometry";
}
