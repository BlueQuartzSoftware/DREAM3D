/* ============================================================================
 * Copyright (c) 2009-2018 BlueQuartz Software, LLC
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

#include "FindSurfaceAreaToVolume.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"

#include "Statistics/StatisticsConstants.h"
#include "Statistics/StatisticsVersion.h"

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
enum createdPathID : RenameDataPath::DataID_t
{
  DataArrayID30 = 30,
  DataArrayID31 = 31,
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSurfaceAreaToVolume::FindSurfaceAreaToVolume()
: m_FeatureIdsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
, m_NumCellsArrayPath(SIMPL::Defaults::ImageDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::NumElements)
, m_SurfaceAreaVolumeRatioArrayName(SIMPL::FeatureData::SurfaceAreaVol)
, m_SphericityArrayName("Sphericity")
, m_CalculateSphericity(true)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSurfaceAreaToVolume::~FindSurfaceAreaToVolume() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceAreaToVolume::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::Cell, IGeometry::Type::Image);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, FindSurfaceAreaToVolume, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Type::CellFeature, IGeometry::Type::Image);

    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Number of Cells", NumCellsArrayPath, FilterParameter::RequiredArray, FindSurfaceAreaToVolume, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Surface Area to Volume Ratio", SurfaceAreaVolumeRatioArrayName, NumCellsArrayPath, NumCellsArrayPath, FilterParameter::CreatedArray, FindSurfaceAreaToVolume));

  QStringList linkedProps("SphericityArrayName");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Calculate Sphericity", CalculateSphericity, FilterParameter::Parameter, FindSurfaceAreaToVolume, linkedProps));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Sphericity Array Name", SphericityArrayName, NumCellsArrayPath, NumCellsArrayPath, FilterParameter::CreatedArray, FindSurfaceAreaToVolume));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceAreaToVolume::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setNumCellsArrayPath(reader->readDataArrayPath("NumCellsArrayPath", getNumCellsArrayPath()));
  setSurfaceAreaVolumeRatioArrayName(reader->readString("SurfaceAreaVolumeRatioArrayName", getSurfaceAreaVolumeRatioArrayName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceAreaToVolume::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceAreaToVolume::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  DataArrayPath tempPath;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  std::vector<size_t> cDims(1, 1);
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 1;
  m_NumCellsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getNumCellsArrayPath(), cDims);
  if(nullptr != m_NumCellsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_NumCells = m_NumCellsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 1;
  tempPath.update(getNumCellsArrayPath().getDataContainerName(), getNumCellsArrayPath().getAttributeMatrixName(), getSurfaceAreaVolumeRatioArrayName());
  m_SurfaceAreaVolumeRatioPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, tempPath, 0, cDims);                          /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceAreaVolumeRatioPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceAreaVolumeRatio = m_SurfaceAreaVolumeRatioPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(getCalculateSphericity())
  {
    tempPath.setDataArrayName(getSphericityArrayName());
    m_SphericityPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims, "", DataArrayID31);
    if(nullptr != m_SphericityPtr.lock())
    {
      m_Sphericity = m_SphericityPtr.lock()->getPointer(0);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSurfaceAreaToVolume::preflight()
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
void FindSurfaceAreaToVolume::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  // Validate that the selected InArray has tuples equal to the largest
  // Feature Id; the filter would not crash otherwise, but the user should
  // be notified of unanticipated behavior ; this cannot be done in the dataCheck since
  // we don't have acces to the data yet
  int32_t numFeatures = static_cast<int32_t>(m_NumCellsPtr.lock()->getNumberOfTuples());
  bool mismatchedFeatures = false;
  int32_t largestFeature = 0;
  size_t numTuples = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  for(size_t i = 0; i < numTuples; i++)
  {
    if(m_FeatureIds[i] > largestFeature)
    {
      largestFeature = m_FeatureIds[i];
      if(largestFeature >= numFeatures)
      {
        mismatchedFeatures = true;
        break;
      }
    }
  }

  if(mismatchedFeatures)
  {
    QString ss = QObject::tr("The number of Features in the NumCells array (%1) is larger than the largest Feature Id in the FeatureIds array").arg(numFeatures);
    setErrorCondition(-5555, ss);
    return;
  }

  if(largestFeature != (numFeatures - 1))
  {
    QString ss = QObject::tr("The number of Features in the NumCells array (%1) does not match the largest Feature Id in the FeatureIds array").arg(numFeatures);
    setErrorCondition(-5555, ss);
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_NumCellsArrayPath.getDataContainerName());

  ImageGeom::Pointer imageGeom = m->getGeometryAs<ImageGeom>();
  FloatVec3Type spacing = imageGeom->getSpacing();

  int64_t xPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getXPoints());
  int64_t yPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getYPoints());
  int64_t zPoints = static_cast<int64_t>(m->getGeometryAs<ImageGeom>()->getZPoints());

  float voxelVol = spacing[0] * spacing[1] * spacing[2];

  std::vector<float> featureSurfaceArea(static_cast<size_t>(numFeatures), 0.0f);

  int64_t neighpoints[6] = {0, 0, 0, 0, 0, 0};
  neighpoints[0] = -xPoints * yPoints;
  neighpoints[1] = -xPoints;
  neighpoints[2] = -1;
  neighpoints[3] = 1;
  neighpoints[4] = xPoints;
  neighpoints[5] = xPoints * yPoints;

  int32_t feature = 0;
  float onsurf = 0.0f;
  bool good = false;
  int64_t neighbor = 0;

  int64_t zStride = 0, yStride = 0;
  for(int64_t i = 0; i < zPoints; i++)
  {
    zStride = i * xPoints * yPoints;
    for(int64_t j = 0; j < yPoints; j++)
    {
      yStride = j * xPoints;
      for(int64_t k = 0; k < xPoints; k++)
      {
        onsurf = 0.0f;
        feature = m_FeatureIds[zStride + yStride + k];
        if(feature > 0)
        {
          for(int32_t l = 0; l < 6; l++)
          {
            good = true;
            neighbor = zStride + yStride + k + neighpoints[l];
            if(l == 0 && i == 0)
            {
              good = false;
            }
            if(l == 5 && i == (zPoints - 1))
            {
              good = false;
            }
            if(l == 1 && j == 0)
            {
              good = false;
            }
            if(l == 4 && j == (yPoints - 1))
            {
              good = false;
            }
            if(l == 2 && k == 0)
            {
              good = false;
            }
            if(l == 3 && k == (xPoints - 1))
            {
              good = false;
            }
            if(good && m_FeatureIds[neighbor] != feature)
            {
              if(l == 0 || l == 5) // XY face shared
              {
                onsurf = onsurf + spacing[0] * spacing[1];
              }
              if(l == 1 || l == 4) // YZ face shared
              {
                onsurf = onsurf + spacing[1] * spacing[2];
              }
              if(l == 2 || l == 3) // XZ face shared
              {
                onsurf = onsurf + spacing[2] * spacing[0];
              }
            }
          }
        }

        featureSurfaceArea[m_FeatureIds[zStride + yStride + k]] = featureSurfaceArea[m_FeatureIds[zStride + yStride + k]] + onsurf;
      }
    }
  }

  const float thirdRootPi = std::pow(SIMPLib::Constants::k_Pif, 0.333333f);
  for(size_t i = 1; i < static_cast<size_t>(numFeatures); i++)
  {
    float featureVolume = voxelVol * m_NumCells[i];
    m_SurfaceAreaVolumeRatio[i] = featureSurfaceArea[i] / featureVolume;
    // Calc the sphericity
    m_Sphericity[i] = (thirdRootPi * std::pow((6.0f * featureVolume), 0.66666f)) / featureSurfaceArea[i];
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindSurfaceAreaToVolume::newFilterInstance(bool copyFilterParameters) const
{
  FindSurfaceAreaToVolume::Pointer filter = FindSurfaceAreaToVolume::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindSurfaceAreaToVolume::getCompiledLibraryName() const
{
  return StatisticsConstants::StatisticsBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindSurfaceAreaToVolume::getBrandingString() const
{
  return "Statistics";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindSurfaceAreaToVolume::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << Statistics::Version::Major() << "." << Statistics::Version::Minor() << "." << Statistics::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindSurfaceAreaToVolume::getGroupName() const
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid FindSurfaceAreaToVolume::getUuid() const
{
  return QUuid("{5d586366-6b59-566e-8de1-57aa9ae8a91c}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindSurfaceAreaToVolume::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::MorphologicalFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindSurfaceAreaToVolume::getHumanLabel() const
{
  return "Find Surface Area to Volume & Sphericity";
}

// -----------------------------------------------------------------------------
FindSurfaceAreaToVolume::Pointer FindSurfaceAreaToVolume::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<FindSurfaceAreaToVolume> FindSurfaceAreaToVolume::New()
{
  struct make_shared_enabler : public FindSurfaceAreaToVolume
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString FindSurfaceAreaToVolume::getNameOfClass() const
{
  return QString("FindSurfaceAreaToVolume");
}

// -----------------------------------------------------------------------------
QString FindSurfaceAreaToVolume::ClassName()
{
  return QString("FindSurfaceAreaToVolume");
}

// -----------------------------------------------------------------------------
void FindSurfaceAreaToVolume::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindSurfaceAreaToVolume::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
void FindSurfaceAreaToVolume::setNumCellsArrayPath(const DataArrayPath& value)
{
  m_NumCellsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindSurfaceAreaToVolume::getNumCellsArrayPath() const
{
  return m_NumCellsArrayPath;
}

// -----------------------------------------------------------------------------
void FindSurfaceAreaToVolume::setSurfaceAreaVolumeRatioArrayName(const QString& value)
{
  m_SurfaceAreaVolumeRatioArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindSurfaceAreaToVolume::getSurfaceAreaVolumeRatioArrayName() const
{
  return m_SurfaceAreaVolumeRatioArrayName;
}

// -----------------------------------------------------------------------------
void FindSurfaceAreaToVolume::setSphericityArrayName(const QString& value)
{
  m_SphericityArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindSurfaceAreaToVolume::getSphericityArrayName() const
{
  return m_SphericityArrayName;
}

// -----------------------------------------------------------------------------
void FindSurfaceAreaToVolume::setCalculateSphericity(bool value)
{
  m_CalculateSphericity = value;
}

// -----------------------------------------------------------------------------
bool FindSurfaceAreaToVolume::getCalculateSphericity() const
{
  return m_CalculateSphericity;
}
