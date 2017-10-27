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

#include "FindSchmids.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"

#include "OrientationAnalysis/OrientationAnalysisConstants.h"
#include "OrientationAnalysis/OrientationAnalysisVersion.h"

#include "EbsdLib/EbsdConstants.h"

// Include the MOC generated file for this class
#include "moc_FindSchmids.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSchmids::FindSchmids()
: AbstractFilter()
, m_FeaturePhasesArrayPath("", "", "")
, m_CrystalStructuresArrayPath("", "", "")
, m_AvgQuatsArrayPath("", "", "")
, m_SchmidsArrayName(SIMPL::FeatureData::Schmids)
, m_SlipSystemsArrayName(SIMPL::FeatureData::SlipSystems)
, m_PolesArrayName(SIMPL::FeatureData::Poles)
, m_PhisArrayName("SchmidPhis")
, m_LambdasArrayName("SchmidLambdas")
, m_StoreAngleComponents(false)
, m_OverrideSystem(false)
, m_FeaturePhases(nullptr)
, m_AvgQuats(nullptr)
, m_CrystalStructures(nullptr)
, m_Schmids(nullptr)
, m_Phis(nullptr)
, m_Lambdas(nullptr)
, m_Poles(nullptr)
, m_SlipSystems(nullptr)
{
  m_LoadingDirection.x = 1.0f;
  m_LoadingDirection.y = 1.0f;
  m_LoadingDirection.z = 1.0f;

  m_SlipPlane.x = 0.0f;
  m_SlipPlane.y = 0.0f;
  m_SlipPlane.z = 1.0f;

  m_SlipDirection.x = 1.0f;
  m_SlipDirection.y = 0.0f;
  m_SlipDirection.z = 0.0f;

  m_OrientationOps = LaueOps::getOrientationOpsQVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindSchmids::~FindSchmids()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindSchmids::setupFilterParameters()
{
  FilterParameterVector parameters;

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
  parameters.push_back(SIMPL_NEW_STRING_FP("Schmids", SchmidsArrayName, FilterParameter::CreatedArray, FindSchmids));
  parameters.push_back(SIMPL_NEW_STRING_FP("Slip Systems", SlipSystemsArrayName, FilterParameter::CreatedArray, FindSchmids));
  parameters.push_back(SIMPL_NEW_STRING_FP("Poles", PolesArrayName, FilterParameter::CreatedArray, FindSchmids));
  parameters.push_back(SIMPL_NEW_STRING_FP("Phis", PhisArrayName, FilterParameter::CreatedArray, FindSchmids));
  parameters.push_back(SIMPL_NEW_STRING_FP("Lambdas", LambdasArrayName, FilterParameter::CreatedArray, FindSchmids));
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
  setErrorCondition(0);
  setWarningCondition(0);
  DataArrayPath tempPath;

  QVector<size_t> cDims(1, 1);

  QVector<DataArrayPath> dataArrayPaths;

  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getSchmidsArrayName());
  m_SchmidsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0,
                                                                                                                cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SchmidsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Schmids = m_SchmidsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(),
                                                                                                           cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeaturePhasesPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getFeaturePhasesArrayPath());
  }

  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getSlipSystemsArrayName());
  m_SlipSystemsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, 0, cDims);               /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SlipSystemsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SlipSystems = m_SlipSystemsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getCrystalStructuresArrayPath(),
                                                                                                                cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CrystalStructuresPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 3;
  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getPolesArrayName());
  m_PolesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0,
                                                                                                                  cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_PolesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Poles = m_PolesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 4;
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(),
                                                                                                    cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_AvgQuatsPtr.lock().get())                                                                 /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getAvgQuatsArrayPath());
  }

  if(m_StoreAngleComponents == true)
  {
    cDims[0] = 1;
    tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getPhisArrayName());
    m_PhisPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, -301,
                                                                                                               cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_PhisPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_Phis = m_PhisPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */

    tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getLambdasArrayName());
    m_LambdasPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, -301,
                                                                                                                  cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_LambdasPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_Lambdas = m_LambdasPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  if(m_OverrideSystem)
  {
    // make sure direction lies in plane
    float cosVec = m_SlipPlane.x * m_SlipDirection.x + m_SlipPlane.y * m_SlipDirection.y + m_SlipPlane.z * m_SlipDirection.z;
    if(0 != cosVec)
    {
      QString ss = QObject::tr("Slip Plane and Slip Direction must be normal");
      setErrorCondition(-1001);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
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
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  size_t totalFeatures = m_SchmidsPtr.lock()->getNumberOfTuples();

  int32_t ss = 0;
  QuatF q1 = QuaternionMathF::New();
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  float g[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float sampleLoading[3] = {0.0f, 0.0f, 0.0f};
  float crystalLoading[3] = {0.0f, 0.0f, 0.0f};
  float angleComps[2] = {0.0f, 0.0f};
  float schmid = 0.0f;

  sampleLoading[0] = m_LoadingDirection.x;
  sampleLoading[1] = m_LoadingDirection.y;
  sampleLoading[2] = m_LoadingDirection.z;
  MatrixMath::Normalize3x1(sampleLoading);
  float plane[3] = {0.0f, 0.0f};
  float direction[3] = {0.0f, 0.0f};

  if(m_OverrideSystem)
  {
    plane[0] = m_SlipPlane.x;
    plane[1] = m_SlipPlane.y;
    plane[2] = m_SlipPlane.z;
    MatrixMath::Normalize3x1(plane);

    direction[0] = m_SlipDirection.x;
    direction[1] = m_SlipDirection.y;
    direction[2] = m_SlipDirection.z;
    MatrixMath::Normalize3x1(direction);
  }

  for(size_t i = 1; i < totalFeatures; i++)
  {
    QuaternionMathF::Copy(avgQuats[i], q1);
    FOrientArrayType om(9);
    FOrientTransformsType::qu2om(FOrientArrayType(q1), om);
    om.toGMatrix(g);

    MatrixMath::Multiply3x3with3x1(g, sampleLoading, crystalLoading);

    uint32_t xtal = m_CrystalStructures[m_FeaturePhases[i]];
    if(xtal < Ebsd::CrystalStructure::LaueGroupEnd)
    {
      if(!m_OverrideSystem)
      {
        m_OrientationOps[xtal]->getSchmidFactorAndSS(crystalLoading, schmid, angleComps, ss);
      }
      else
      {
        m_OrientationOps[xtal]->getSchmidFactorAndSS(crystalLoading, plane, direction, schmid, angleComps, ss);
      }

      m_Schmids[i] = schmid;
      if(m_StoreAngleComponents == true)
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

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer FindSchmids::newFilterInstance(bool copyFilterParameters)
{
  FindSchmids::Pointer filter = FindSchmids::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSchmids::getCompiledLibraryName()
{
  return OrientationAnalysisConstants::OrientationAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSchmids::getBrandingString()
{
  return "OrientationAnalysis";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSchmids::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << OrientationAnalysis::Version::Major() << "." << OrientationAnalysis::Version::Minor() << "." << OrientationAnalysis::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSchmids::getGroupName()
{
  return SIMPL::FilterGroups::StatisticsFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSchmids::getSubGroupName()
{
  return SIMPL::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString FindSchmids::getHumanLabel()
{
  return "Find Schmid Factors";
}
