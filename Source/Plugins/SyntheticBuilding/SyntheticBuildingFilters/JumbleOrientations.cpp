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

#include "JumbleOrientations.h"

#include <random>
#include <chrono>

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "OrientationLib/Core/Orientation.hpp"
#include "OrientationLib/Core/OrientationTransformation.hpp"
#include "OrientationLib/Core/Quaternion.hpp"

#include "SyntheticBuilding/SyntheticBuildingConstants.h"
#include "SyntheticBuilding/SyntheticBuildingVersion.h"

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
enum createdPathID : RenameDataPath::DataID_t
{
  DataArrayID30 = 30,
  DataArrayID31 = 31,
  DataArrayID32 = 32,
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
JumbleOrientations::JumbleOrientations()
: m_FeatureIdsArrayPath("", "", "")
, m_CellEulerAnglesArrayName(SIMPL::CellData::EulerAngles)
, m_FeaturePhasesArrayPath("", "", "")
, m_FeatureEulerAnglesArrayPath("", "", "")
, m_AvgQuatsArrayName(SIMPL::FeatureData::AvgQuats)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
JumbleOrientations::~JumbleOrientations() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void JumbleOrientations::setupFilterParameters()
{
  FilterParameterVectorType parameters;
  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Category::Element);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, JumbleOrientations, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Float, 3, AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Average Euler Angles", FeatureEulerAnglesArrayPath, FilterParameter::RequiredArray, JumbleOrientations, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", FeaturePhasesArrayPath, FilterParameter::RequiredArray, JumbleOrientations, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Euler Angles", CellEulerAnglesArrayName, FeatureIdsArrayPath, FeatureIdsArrayPath, FilterParameter::CreatedArray, JumbleOrientations));
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Average Quaternions", AvgQuatsArrayName, FeaturePhasesArrayPath, FeaturePhasesArrayPath, FilterParameter::CreatedArray, JumbleOrientations));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void JumbleOrientations::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setAvgQuatsArrayName(reader->readString("AvgQuatsArrayName", getAvgQuatsArrayName()));
  setFeatureEulerAnglesArrayPath(reader->readDataArrayPath("FeatureEulerAnglesArrayPath", getFeatureEulerAnglesArrayPath()));
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath()));
  setCellEulerAnglesArrayName(reader->readString("CellEulerAnglesArrayName", getCellEulerAnglesArrayName()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void JumbleOrientations::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void JumbleOrientations::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  DataArrayPath tempPath;

  std::vector<size_t> cDims(1, 1);
  // Cell Data
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 3;
  tempPath.update(getFeatureIdsArrayPath().getDataContainerName(), getFeatureIdsArrayPath().getAttributeMatrixName(), getCellEulerAnglesArrayName());
  m_CellEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, tempPath, 0.0, cDims, "", DataArrayID31);
  if(nullptr != m_CellEulerAnglesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Feature Data
  cDims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(),
                                                                                                           cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeaturePhasesPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 3;
  m_FeatureEulerAnglesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getFeatureEulerAnglesArrayPath(),
                                                                                                              cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureEulerAnglesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 4;
  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getAvgQuatsArrayName());
  m_AvgQuatsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims, "", DataArrayID32);
  if(nullptr != m_AvgQuatsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void JumbleOrientations::preflight()
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
void JumbleOrientations::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }

  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  int32_t totalFeatures = static_cast<int32_t>(m_FeaturePhasesPtr.lock()->getNumberOfTuples());

  // Generate all the numbers up front
  const int32_t rangeMin = 1;
  const int32_t rangeMax = totalFeatures - 1;

  std::random_device randomDevice;           // Will be used to obtain a seed for the random number engine
  std::mt19937_64 generator(randomDevice()); // Standard mersenne_twister_engine seeded with rd()
  std::mt19937_64::result_type seed = static_cast<std::mt19937_64::result_type>(std::chrono::steady_clock::now().time_since_epoch().count());
  generator.seed(seed);
  std::uniform_int_distribution<int32_t> distribution(rangeMin, rangeMax);

  int32_t r = 0;
  float temp1 = 0.0f, temp2 = 0.0f, temp3 = 0.0f;
  //--- Shuffle elements by randomly exchanging each with one other.
  for(int32_t i = 1; i < totalFeatures; i++)
  {
    bool good = false;
    while(!good)
    {
      good = true;
      r = distribution(generator); // Random remaining position.
      if(r >= totalFeatures)
      {
        good = false;
      }
      if(m_FeaturePhases[i] != m_FeaturePhases[r])
      {
        good = false;
      }
    }
    temp1 = m_FeatureEulerAngles[3 * i];
    temp2 = m_FeatureEulerAngles[3 * i + 1];
    temp3 = m_FeatureEulerAngles[3 * i + 2];
    m_FeatureEulerAngles[3 * i] = m_FeatureEulerAngles[3 * r];
    m_FeatureEulerAngles[3 * i + 1] = m_FeatureEulerAngles[3 * r + 1];
    m_FeatureEulerAngles[3 * i + 2] = m_FeatureEulerAngles[3 * r + 2];
    m_FeatureEulerAngles[3 * r] = temp1;
    m_FeatureEulerAngles[3 * r + 1] = temp2;
    m_FeatureEulerAngles[3 * r + 2] = temp3;
  }

  // Now adjust all the Euler angle values for each Voxel
  for(size_t i = 0; i < totalPoints; ++i)
  {
    m_CellEulerAngles[3 * i] = m_FeatureEulerAngles[3 * (m_FeatureIds[i])];
    m_CellEulerAngles[3 * i + 1] = m_FeatureEulerAngles[3 * (m_FeatureIds[i]) + 1];
    m_CellEulerAngles[3 * i + 2] = m_FeatureEulerAngles[3 * (m_FeatureIds[i]) + 2];
  }
  // QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  for(int32_t i = 1; i < totalFeatures; i++)
  {
    QuatF quat(m_AvgQuats + i * 4);
    quat = OrientationTransformation::eu2qu<OrientationF, QuatF>(OrientationF(m_FeatureEulerAngles + (3 * i), 3));
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer JumbleOrientations::newFilterInstance(bool copyFilterParameters) const
{
  JumbleOrientations::Pointer filter = JumbleOrientations::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString JumbleOrientations::getCompiledLibraryName() const
{
  return SyntheticBuildingConstants::SyntheticBuildingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString JumbleOrientations::getBrandingString() const
{
  return "SyntheticBuilding";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString JumbleOrientations::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << SyntheticBuilding::Version::Major() << "." << SyntheticBuilding::Version::Minor() << "." << SyntheticBuilding::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString JumbleOrientations::getGroupName() const
{
  return SIMPL::FilterGroups::SyntheticBuildingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid JumbleOrientations::getUuid() const
{
  return QUuid("{b7301dbf-27d5-5335-b86e-563d573f002b}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString JumbleOrientations::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString JumbleOrientations::getHumanLabel() const
{
  return "Jumble Orientations";
}

// -----------------------------------------------------------------------------
JumbleOrientations::Pointer JumbleOrientations::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<JumbleOrientations> JumbleOrientations::New()
{
  struct make_shared_enabler : public JumbleOrientations
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString JumbleOrientations::getNameOfClass() const
{
  return QString("JumbleOrientations");
}

// -----------------------------------------------------------------------------
QString JumbleOrientations::ClassName()
{
  return QString("JumbleOrientations");
}

// -----------------------------------------------------------------------------
void JumbleOrientations::setFeatureIdsArrayPath(const DataArrayPath& value)
{
  m_FeatureIdsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath JumbleOrientations::getFeatureIdsArrayPath() const
{
  return m_FeatureIdsArrayPath;
}

// -----------------------------------------------------------------------------
void JumbleOrientations::setCellEulerAnglesArrayName(const QString& value)
{
  m_CellEulerAnglesArrayName = value;
}

// -----------------------------------------------------------------------------
QString JumbleOrientations::getCellEulerAnglesArrayName() const
{
  return m_CellEulerAnglesArrayName;
}

// -----------------------------------------------------------------------------
void JumbleOrientations::setFeaturePhasesArrayPath(const DataArrayPath& value)
{
  m_FeaturePhasesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath JumbleOrientations::getFeaturePhasesArrayPath() const
{
  return m_FeaturePhasesArrayPath;
}

// -----------------------------------------------------------------------------
void JumbleOrientations::setFeatureEulerAnglesArrayPath(const DataArrayPath& value)
{
  m_FeatureEulerAnglesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath JumbleOrientations::getFeatureEulerAnglesArrayPath() const
{
  return m_FeatureEulerAnglesArrayPath;
}

// -----------------------------------------------------------------------------
void JumbleOrientations::setAvgQuatsArrayName(const QString& value)
{
  m_AvgQuatsArrayName = value;
}

// -----------------------------------------------------------------------------
QString JumbleOrientations::getAvgQuatsArrayName() const
{
  return m_AvgQuatsArrayName;
}
