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
#include "FindFeatureCentroids.h"

#include <array>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "Generic/GenericConstants.h"
#include "Generic/GenericVersion.h"

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
enum createdPathID : RenameDataPath::DataID_t
{
  DataArrayID30 = 30,
  DataArrayID31 = 31,
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureCentroids::FindFeatureCentroids() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindFeatureCentroids::~FindFeatureCentroids() = default;
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureCentroids::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SeparatorFilterParameter::Create("Cell Data", FilterParameter::Category::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::Category::RequiredArray, FindFeatureCentroids, req));
  }
  parameters.push_back(SeparatorFilterParameter::Create("Cell Feature Data", FilterParameter::Category::CreatedArray));
  {
    DataArrayCreationFilterParameter::RequirementType req = DataArrayCreationFilterParameter::CreateRequirement(AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_CREATION_FP("Centroids", CentroidsArrayPath, FilterParameter::Category::CreatedArray, FindFeatureCentroids, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureCentroids::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setCentroidsArrayPath(reader->readDataArrayPath("CentroidsArrayPath", getCentroidsArrayPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureCentroids::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureCentroids::dataCheck()
{
  clearErrorCode();
  clearWarningCode();

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom>(this, getFeatureIdsArrayPath().getDataContainerName());

  std::vector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>>(this, getFeatureIdsArrayPath(), cDims);
  if(nullptr != m_FeatureIdsPtr.lock())
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  }

  cDims[0] = 3;
  m_CentroidsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>>(this, getCentroidsArrayPath(), 0, cDims, "", DataArrayID31);
  if(nullptr != m_CentroidsPtr.lock())
  {
    m_Centroids = m_CentroidsPtr.lock()->getPointer(0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureCentroids::find_centroids()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getFeatureIdsArrayPath().getDataContainerName());
  ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();

  size_t totalFeatures = m_CentroidsPtr.lock()->getNumberOfTuples();

  size_t xPoints = imageGeom->getXPoints();
  size_t yPoints = imageGeom->getYPoints();
  size_t zPoints = imageGeom->getZPoints();

  //Copy the Coords into an array and then compute the sum

  std::vector<double> sum(totalFeatures * 3, 0.0);
  std::vector<double> c(totalFeatures * 3, 0.0);
  std::vector<size_t> count(totalFeatures * 3, 0.0);

  std::array<float, 3> voxel_center = {0.0f, 0.0f, 0.0f};

  size_t zStride = 0;
  size_t yStride = 0;
  for(size_t i = 0; i < zPoints; i++)
  {
    zStride = i * xPoints * yPoints;
    for(size_t j = 0; j < yPoints; j++)
    {
      yStride = j * xPoints;
      for(size_t k = 0; k < xPoints; k++)
      {
        int32_t featureId = m_FeatureIds[zStride + yStride + k]; // Get the current FeatureId

        imageGeom->getCoords(k, j, i, voxel_center.data()); // Get the voxel center based on XYZ index from Image Geom

        //Kahan Sum for X Coord
        size_t featureId_idx = static_cast<size_t>(featureId * 3);
        double y = static_cast<double>(voxel_center[0] - c[featureId_idx]);
        double t = sum[featureId_idx] + y;
        c[featureId_idx] = (t - sum[featureId_idx]) - y;
        sum[featureId_idx] = t;
        count[featureId_idx]++;

        //Kahan Sum for Y Coord
        featureId_idx = static_cast<size_t>(featureId * 3 + 1);
        y = static_cast<double>(voxel_center[1] - c[featureId_idx]);
        t = sum[featureId_idx] + y;
        c[featureId_idx] = (t - sum[featureId_idx]) - y;
        sum[featureId_idx] = t;
        count[featureId_idx]++;

        //Kahan Sum for Z Coord
        featureId_idx = static_cast<size_t>(featureId * 3 + 2);
        y = static_cast<double>(voxel_center[2] - c[featureId_idx]);
        t = sum[featureId_idx] + y;
        c[featureId_idx] = (t - sum[featureId_idx]) - y;
        sum[featureId_idx] = t;
        count[featureId_idx]++;
      }
    }
  }

  for(size_t featureId = 0; featureId < totalFeatures; featureId++)
  {
    size_t featureId_idx = static_cast<size_t>(featureId * 3);
    if(count[featureId_idx] > 0.0f)
    {
      m_Centroids[featureId_idx] = sum[featureId_idx] / static_cast<double>(count[featureId_idx]);
    }

    featureId_idx = static_cast<size_t>(featureId * 3 + 1);
    if(count[featureId_idx] > 0.0f)
    {
      m_Centroids[featureId_idx] = sum[featureId_idx] / static_cast<double>(count[featureId_idx]);
    }

    featureId_idx = static_cast<size_t>(featureId * 3 + 2);
    if(count[featureId_idx] > 0.0f)
    {
      m_Centroids[featureId_idx] = sum[featureId_idx] / static_cast<double>(count[featureId_idx]);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindFeatureCentroids::execute()
{
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  find_centroids();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindFeatureCentroids::newFilterInstance(bool copyFilterParameters) const
{
  FindFeatureCentroids::Pointer filter = FindFeatureCentroids::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindFeatureCentroids::getCompiledLibraryName() const
{
  return GenericConstants::GenericBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindFeatureCentroids::getBrandingString() const
{
  return "Generic";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindFeatureCentroids::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Generic::Version::Major() << "." << Generic::Version::Minor() << "." << Generic::Version::Patch();
  return version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindFeatureCentroids::getGroupName() const
{
  return SIMPL::FilterGroups::Generic;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid FindFeatureCentroids::getUuid() const
{
  return QUuid("{6f8ca36f-2995-5bd3-8672-6b0b80d5b2ca}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindFeatureCentroids::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MorphologicalFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindFeatureCentroids::getHumanLabel() const
{
  return "Find Feature Centroids";
}

// -----------------------------------------------------------------------------
FindFeatureCentroids::Pointer FindFeatureCentroids::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<FindFeatureCentroids> FindFeatureCentroids::New()
{
  struct make_shared_enabler : public FindFeatureCentroids
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString FindFeatureCentroids::getNameOfClass() const
{
  return QString("FindFeatureCentroids");
}

// -----------------------------------------------------------------------------
QString FindFeatureCentroids::ClassName()
{
  return QString("FindFeatureCentroids");
}

// -----------------------------------------------------------------------------
void FindFeatureCentroids::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindFeatureCentroids::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
void FindFeatureCentroids::setCentroidsArrayPath(const DataArrayPath& value)
{
  m_CentroidsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindFeatureCentroids::getCentroidsArrayPath() const
{
  return m_CentroidsArrayPath;
}
