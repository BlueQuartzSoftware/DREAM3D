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

#include "FindSchmids.h"

#include <QtCore/QTextStream>

#include "SIMPLib/Common/Constants.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedPathCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"

#include "OrientationLib/Core/Orientation.hpp"
#include "OrientationLib/Core/OrientationTransformation.hpp"
#include "OrientationLib/Core/Quaternion.hpp"
#include "OrientationLib/LaueOps/LaueOps.h"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

#include "EbsdLib/EbsdConstants.h"

/* Create Enumerations to allow the created Attribute Arrays to take part in renaming */
enum createdPathID : RenameDataPath::DataID_t
{
  DataArrayID30 = 30,
  DataArrayID31 = 31,
  DataArrayID32 = 32,
  DataArrayID33 = 33,
  DataArrayID34 = 34,
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSchmids::FindSchmids()
: m_FeaturePhasesArrayPath("", "", "")
, m_CrystalStructuresArrayPath("", "", "")
, m_AvgQuatsArrayPath("", "", "")
, m_SchmidsArrayName(SIMPL::FeatureData::Schmids)
, m_SlipSystemsArrayName(SIMPL::FeatureData::SlipSystems)
, m_PolesArrayName(SIMPL::FeatureData::Poles)
, m_PhisArrayName("SchmidPhis")
, m_LambdasArrayName("SchmidLambdas")
, m_StoreAngleComponents(false)
, m_OverrideSystem(false)
{
  m_LoadingDirection[0] = 1.0f;
  m_LoadingDirection[1] = 1.0f;
  m_LoadingDirection[2] = 1.0f;

  m_SlipPlane[0] = 0.0f;
  m_SlipPlane[1] = 0.0f;
  m_SlipPlane[2] = 1.0f;

  m_SlipDirection[0] = 1.0f;
  m_SlipDirection[1] = 0.0f;
  m_SlipDirection[2] = 0.0f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSchmids::~FindSchmids() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSchmids::setupFilterParameters()
{
  FilterParameterVectorType parameters;

  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Loading Direction", LoadingDirection, FilterParameter::Parameter, FindSchmids));

  QStringList linkedProps;
  linkedProps << "PhisArrayName"
              << "LambdasArrayName";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Store Angle Components of Schmid Factor", StoreAngleComponents, FilterParameter::Parameter, FindSchmids, linkedProps));
  QStringList linkedProps2;
  linkedProps2 << "SlipPlane"
               << "SlipDirection";
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Override Default Slip System", OverrideSystem, FilterParameter::Parameter, FindSchmids, linkedProps2));
  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Slip Plane", SlipPlane, FilterParameter::Parameter, FindSchmids));

  parameters.push_back(SIMPL_NEW_FLOAT_VEC3_FP("Slip Direction", SlipDirection, FilterParameter::Parameter, FindSchmids));

  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", FeaturePhasesArrayPath, FilterParameter::RequiredArray, FindSchmids, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Float, 4, AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Average Quaternions", AvgQuatsArrayPath, FilterParameter::RequiredArray, FindSchmids, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Category::Ensemble);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, FindSchmids, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Schmids", SchmidsArrayName, FeaturePhasesArrayPath, FeaturePhasesArrayPath, FilterParameter::CreatedArray, FindSchmids));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Slip Systems", SlipSystemsArrayName, FeaturePhasesArrayPath, FeaturePhasesArrayPath, FilterParameter::CreatedArray, FindSchmids));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Poles", PolesArrayName, FeaturePhasesArrayPath, FeaturePhasesArrayPath, FilterParameter::CreatedArray, FindSchmids));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Phis", PhisArrayName, FeaturePhasesArrayPath, FeaturePhasesArrayPath, FilterParameter::CreatedArray, FindSchmids));
  parameters.push_back(SIMPL_NEW_DA_WITH_LINKED_AM_FP("Lambdas", LambdasArrayName, FeaturePhasesArrayPath, FeaturePhasesArrayPath, FilterParameter::CreatedArray, FindSchmids));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSchmids::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setLambdasArrayName(reader->readString("LambdasArrayName", getLambdasArrayName()));
  setPhisArrayName(reader->readString("PhisArrayName", getPhisArrayName()));
  setPolesArrayName(reader->readString("PolesArrayName", getPolesArrayName()));
  setSlipSystemsArrayName(reader->readString("SlipSystemsArrayName", getSlipSystemsArrayName()));
  setSchmidsArrayName(reader->readString("SchmidsArrayName", getSchmidsArrayName()));
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath()));
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath()));
  setLoadingDirection(reader->readFloatVec3("LoadingDirection", getLoadingDirection()));
  setStoreAngleComponents(reader->readValue("StoreAngleComponents", getStoreAngleComponents()));
  setOverrideSystem(reader->readValue("OverrideSystem", getOverrideSystem()));
  setSlipPlane(reader->readFloatVec3("SlipPlane", getSlipPlane()));
  setSlipDirection(reader->readFloatVec3("SlipDirection", getSlipDirection()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSchmids::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSchmids::dataCheck()
{
  clearErrorCode();
  clearWarningCode();
  DataArrayPath tempPath;

  std::vector<size_t> cDims(1, 1);

  QVector<DataArrayPath> dataArrayPaths;

  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getSchmidsArrayName());
  m_SchmidsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0, cDims, "", DataArrayID31);
  if(nullptr != m_SchmidsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Schmids = m_SchmidsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(),
                                                                                                           cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeaturePhasesPtr.lock())                                                                         /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getFeaturePhasesArrayPath());
  }

  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getSlipSystemsArrayName());
  m_SlipSystemsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, cDims);               /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SlipSystemsPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SlipSystems = m_SlipSystemsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getCrystalStructuresArrayPath(),
                                                                                                                cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CrystalStructuresPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 3;
  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getPolesArrayName());
  m_PolesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, cDims, "", DataArrayID32);
  if(nullptr != m_PolesPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Poles = m_PolesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 4;
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(),
                                                                                                    cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_AvgQuatsPtr.lock())                                                                       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCode() >= 0)
  {
    dataArrayPaths.push_back(getAvgQuatsArrayPath());
  }

  if(m_StoreAngleComponents)
  {
    cDims[0] = 1;
    tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getPhisArrayName());
    m_PhisPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, -301, cDims, "", DataArrayID33);
    if(nullptr != m_PhisPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_Phis = m_PhisPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */

    tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getLambdasArrayName());
    m_LambdasPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, -301, cDims, "", DataArrayID34);
    if(nullptr != m_LambdasPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_Lambdas = m_LambdasPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  if(m_OverrideSystem)
  {
    // make sure direction lies in plane
    float cosVec = m_SlipPlane[0] * m_SlipDirection[0] + m_SlipPlane[1] * m_SlipDirection[1] + m_SlipPlane[2] * m_SlipDirection[2];
    if(0 != cosVec)
    {
      QString ss = QObject::tr("Slip Plane and Slip Direction must be normal");
      setErrorCondition(-1001, ss);
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSchmids::preflight()
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
void FindSchmids::execute()
{
  clearErrorCode();
  clearWarningCode();
  dataCheck();
  if(getErrorCode() < 0)
  {
    return;
  }
  QVector<LaueOps::Pointer> orientationOps = LaueOps::getOrientationOpsQVector();

  size_t totalFeatures = m_SchmidsPtr.lock()->getNumberOfTuples();

  int32_t ss = 0;
  // QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);
  FloatArrayType::Pointer avgQuatPtr = m_AvgQuatsPtr.lock();

  double g[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  double sampleLoading[3] = {0.0f, 0.0f, 0.0f};
  double crystalLoading[3] = {0.0f, 0.0f, 0.0f};
  double angleComps[2] = {0.0f, 0.0f};
  double schmid = 0.0f;

  sampleLoading[0] = m_LoadingDirection[0];
  sampleLoading[1] = m_LoadingDirection[1];
  sampleLoading[2] = m_LoadingDirection[2];
  MatrixMath::Normalize3x1(sampleLoading);
  double plane[3] = {0.0f, 0.0f};
  double direction[3] = {0.0f, 0.0f};

  if(m_OverrideSystem)
  {
    plane[0] = m_SlipPlane[0];
    plane[1] = m_SlipPlane[1];
    plane[2] = m_SlipPlane[2];
    MatrixMath::Normalize3x1(plane);

    direction[0] = m_SlipDirection[0];
    direction[1] = m_SlipDirection[1];
    direction[2] = m_SlipDirection[2];
    MatrixMath::Normalize3x1(direction);
  }

  for(size_t i = 1; i < totalFeatures; i++)
  {
    QuatF q1(avgQuatPtr->getTuplePointer(i));
    OrientationTransformation::qu2om<QuatF, OrientationD>(q1).toGMatrix(g);

    MatrixMath::Multiply3x3with3x1(g, sampleLoading, crystalLoading);

    uint32_t xtal = m_CrystalStructures[m_FeaturePhases[i]];
    if(xtal < Ebsd::CrystalStructure::LaueGroupEnd)
    {
      if(!m_OverrideSystem)
      {
        orientationOps[xtal]->getSchmidFactorAndSS(crystalLoading, schmid, angleComps, ss);
      }
      else
      {
        orientationOps[xtal]->getSchmidFactorAndSS(crystalLoading, plane, direction, schmid, angleComps, ss);
      }

      m_Schmids[i] = schmid;
      if(m_StoreAngleComponents)
      {
        m_Phis[i] = angleComps[0];
        m_Lambdas[i] = angleComps[1];
      }
      m_Poles[3 * i] = int32_t(crystalLoading[0] * 100);
      m_Poles[3 * i + 1] = int32_t(crystalLoading[1] * 100);
      m_Poles[3 * i + 2] = int32_t(crystalLoading[2] * 100);
      m_SlipSystems[i] = ss;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindSchmids::newFilterInstance(bool copyFilterParameters) const
{
  FindSchmids::Pointer filter = FindSchmids::New();
  if(copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindSchmids::getCompiledLibraryName() const
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindSchmids::getBrandingString() const
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindSchmids::getFilterVersion() const
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindSchmids::getGroupName() const
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid FindSchmids::getUuid() const
{
  return QUuid("{e67ca06a-176f-58fc-a676-d6ee5553511a}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindSchmids::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString FindSchmids::getHumanLabel() const
{
  return "Find Schmid Factors";
}

// -----------------------------------------------------------------------------
FindSchmids::Pointer FindSchmids::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
std::shared_ptr<FindSchmids> FindSchmids::New()
{
  struct make_shared_enabler : public FindSchmids
  {
  };
  std::shared_ptr<make_shared_enabler> val = std::make_shared<make_shared_enabler>();
  val->setupFilterParameters();
  return val;
}

// -----------------------------------------------------------------------------
QString FindSchmids::getNameOfClass() const
{
  return QString("FindSchmids");
}

// -----------------------------------------------------------------------------
QString FindSchmids::ClassName()
{
  return QString("FindSchmids");
}

// -----------------------------------------------------------------------------
void FindSchmids::setFeaturePhasesArrayPath(const DataArrayPath& value)
{
  m_FeaturePhasesArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindSchmids::getFeaturePhasesArrayPath() const
{
  return m_FeaturePhasesArrayPath;
}

// -----------------------------------------------------------------------------
void FindSchmids::setCrystalStructuresArrayPath(const DataArrayPath& value)
{
  m_CrystalStructuresArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindSchmids::getCrystalStructuresArrayPath() const
{
  return m_CrystalStructuresArrayPath;
}

// -----------------------------------------------------------------------------
void FindSchmids::setAvgQuatsArrayPath(const DataArrayPath& value)
{
  m_AvgQuatsArrayPath = value;
}

// -----------------------------------------------------------------------------
DataArrayPath FindSchmids::getAvgQuatsArrayPath() const
{
  return m_AvgQuatsArrayPath;
}

// -----------------------------------------------------------------------------
void FindSchmids::setSchmidsArrayName(const QString& value)
{
  m_SchmidsArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindSchmids::getSchmidsArrayName() const
{
  return m_SchmidsArrayName;
}

// -----------------------------------------------------------------------------
void FindSchmids::setSlipSystemsArrayName(const QString& value)
{
  m_SlipSystemsArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindSchmids::getSlipSystemsArrayName() const
{
  return m_SlipSystemsArrayName;
}

// -----------------------------------------------------------------------------
void FindSchmids::setPolesArrayName(const QString& value)
{
  m_PolesArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindSchmids::getPolesArrayName() const
{
  return m_PolesArrayName;
}

// -----------------------------------------------------------------------------
void FindSchmids::setPhisArrayName(const QString& value)
{
  m_PhisArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindSchmids::getPhisArrayName() const
{
  return m_PhisArrayName;
}

// -----------------------------------------------------------------------------
void FindSchmids::setLambdasArrayName(const QString& value)
{
  m_LambdasArrayName = value;
}

// -----------------------------------------------------------------------------
QString FindSchmids::getLambdasArrayName() const
{
  return m_LambdasArrayName;
}

// -----------------------------------------------------------------------------
void FindSchmids::setLoadingDirection(const FloatVec3Type& value)
{
  m_LoadingDirection = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type FindSchmids::getLoadingDirection() const
{
  return m_LoadingDirection;
}

// -----------------------------------------------------------------------------
void FindSchmids::setStoreAngleComponents(bool value)
{
  m_StoreAngleComponents = value;
}

// -----------------------------------------------------------------------------
bool FindSchmids::getStoreAngleComponents() const
{
  return m_StoreAngleComponents;
}

// -----------------------------------------------------------------------------
void FindSchmids::setOverrideSystem(bool value)
{
  m_OverrideSystem = value;
}

// -----------------------------------------------------------------------------
bool FindSchmids::getOverrideSystem() const
{
  return m_OverrideSystem;
}

// -----------------------------------------------------------------------------
void FindSchmids::setSlipPlane(const FloatVec3Type& value)
{
  m_SlipPlane = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type FindSchmids::getSlipPlane() const
{
  return m_SlipPlane;
}

// -----------------------------------------------------------------------------
void FindSchmids::setSlipDirection(const FloatVec3Type& value)
{
  m_SlipDirection = value;
}

// -----------------------------------------------------------------------------
FloatVec3Type FindSchmids::getSlipDirection() const
{
  return m_SlipDirection;
}
