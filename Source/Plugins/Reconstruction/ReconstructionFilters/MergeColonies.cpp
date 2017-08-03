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

#include "MergeColonies.h"

#include <random>
#include <chrono>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/Utilities/SIMPLibRandom.h"

#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"

#include "Reconstruction/ReconstructionConstants.h"
#include "Reconstruction/ReconstructionVersion.h"

#include "EbsdLib/EbsdConstants.h"

namespace
{
static const float unit110 = 1.0 / sqrtf(2.0);
static const float unit111 = 1.0 / sqrtf(3.0);
static const float unit112_1 = 1.0 / sqrtf(6.0);
static const float unit112_2 = 2.0 / sqrtf(6.0);

float crystalDirections[12][3][3] = {{{unit111, unit112_1, unit110}, {-unit111, -unit112_1, unit110}, {unit111, -unit112_2, 0}},

                                     {{-unit111, unit112_1, unit110}, {unit111, -unit112_1, unit110}, {unit111, unit112_2, 0}},

                                     {{unit111, -unit112_1, unit110}, {unit111, -unit112_1, -unit110}, {unit111, unit112_2, 0}},

                                     {{unit111, unit112_1, unit110}, {unit111, unit112_1, -unit110}, {-unit111, unit112_2, 0}},

                                     {{unit111, unit112_1, unit110}, {unit111, -unit112_2, 0}, {unit111, unit112_1, -unit110}},

                                     {{unit111, -unit112_1, unit110}, {-unit111, -unit112_2, 0}, {unit111, -unit112_1, -unit110}},

                                     {{unit111, -unit112_1, unit110}, {unit111, unit112_2, 0}, {-unit111, unit112_1, unit110}},

                                     {{-unit111, -unit112_1, unit110}, {unit111, -unit112_2, 0}, {unit111, unit112_1, unit110}},

                                     {{unit111, -unit112_2, 0}, {unit111, unit112_1, unit110}, {-unit111, -unit112_1, unit110}},

                                     {{unit111, unit112_2, 0}, {-unit111, unit112_1, unit110}, {unit111, -unit112_1, unit110}},

                                     {{unit111, unit112_2, 0}, {unit111, -unit112_1, unit110}, {unit111, -unit112_1, -unit110}},

                                     {{-unit111, unit112_2, 0}, {unit111, unit112_1, unit110}, {unit111, unit112_1, -unit110}}};
}

// Include the MOC generated file for this class
#include "moc_MergeColonies.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MergeColonies::MergeColonies()
: GroupFeatures()
, m_NewCellFeatureAttributeMatrixName("NewFeatureData")
, m_FeatureIdsArrayPath("", "", "")
, m_CellPhasesArrayPath("", "", "")
, m_FeaturePhasesArrayPath("", "", "")
, m_AvgQuatsArrayPath("", "", "")
, m_CrystalStructuresArrayPath("", "", "")
, m_CellParentIdsArrayName(SIMPL::CellData::ParentIds)
, m_GlobAlphaArrayName(SIMPL::CellData::GlobAlpha)
, m_FeatureParentIdsArrayName(SIMPL::FeatureData::ParentIds)
, m_ActiveArrayName(SIMPL::FeatureData::Active)
, m_AxisTolerance(1.0f)
, m_AngleTolerance(1.0f)
, m_RandomizeParentIds(true)
, m_IdentifyGlobAlpha(false)
, m_FeatureIds(nullptr)
, m_CellPhases(nullptr)
, m_AvgQuats(nullptr)
, m_FeaturePhases(nullptr)
, m_CrystalStructures(nullptr)
, m_CellParentIds(nullptr)
, m_FeatureParentIds(nullptr)
, m_GlobAlpha(nullptr)
, m_Active(nullptr)
{
  m_OrientationOps = LaueOps::getOrientationOpsQVector();

  m_AxisToleranceRad = 0.0f;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MergeColonies::~MergeColonies()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::setupFilterParameters()
{
  FilterParameterVector parameters = getFilterParameters();
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Axis Tolerance (Degrees)", AxisTolerance, FilterParameter::Parameter, MergeColonies));
  parameters.push_back(SIMPL_NEW_FLOAT_FP("Angle Tolerance (Degrees)", AngleTolerance, FilterParameter::Parameter, MergeColonies));
  QStringList linkedProps("GlobAlphaArrayName");
  parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("Identify Glob Alpha", IdentifyGlobAlpha, FilterParameter::Parameter, MergeColonies, linkedProps));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Phases", FeaturePhasesArrayPath, FilterParameter::RequiredArray, MergeColonies, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Float, 4, AttributeMatrix::Category::Feature);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Average Quaternions", AvgQuatsArrayPath, FilterParameter::RequiredArray, MergeColonies, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Category::Element);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, MergeColonies, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::Int32, 1, AttributeMatrix::Category::Element);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", CellPhasesArrayPath, FilterParameter::RequiredArray, MergeColonies, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(SIMPL::TypeNames::UInt32, 1, AttributeMatrix::Category::Ensemble);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, MergeColonies, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Element Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Parent Ids", CellParentIdsArrayName, FilterParameter::CreatedArray, MergeColonies));
  parameters.push_back(SIMPL_NEW_STRING_FP("Glob Alpha", GlobAlphaArrayName, FilterParameter::CreatedArray, MergeColonies));
  parameters.push_back(SeparatorFilterParameter::New("Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Feature Attribute Matrix", NewCellFeatureAttributeMatrixName, FilterParameter::CreatedArray, MergeColonies));
  parameters.push_back(SIMPL_NEW_STRING_FP("Parent Ids", FeatureParentIdsArrayName, FilterParameter::CreatedArray, MergeColonies));
  parameters.push_back(SIMPL_NEW_STRING_FP("Active", ActiveArrayName, FilterParameter::CreatedArray, MergeColonies));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  GroupFeatures::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setNewCellFeatureAttributeMatrixName(reader->readString("NewCellFeatureAttributeMatrixName", getNewCellFeatureAttributeMatrixName()));
  setActiveArrayName(reader->readString("ActiveArrayName", getActiveArrayName()));
  setFeatureParentIdsArrayName(reader->readString("FeatureParentIdsArrayName", getFeatureParentIdsArrayName()));
  setGlobAlphaArrayName(reader->readString("GlobAlphaArrayName", getGlobAlphaArrayName()));
  setCellParentIdsArrayName(reader->readString("CellParentIdsArrayName", getCellParentIdsArrayName()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath()));
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath()));
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath()));
  setCellPhasesArrayPath(reader->readDataArrayPath("CellPhasesArrayPath", getCellPhasesArrayPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setAxisTolerance(reader->readValue("AxisTolerance", getAxisTolerance()));
  setAngleTolerance(reader->readValue("AngleTolerance", getAngleTolerance()));
  setIdentifyGlobAlpha(reader->readValue("IdentifyGlobAlpha", getIdentifyGlobAlpha()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::updateFeatureInstancePointers()
{
  setErrorCondition(0);
  setWarningCondition(0);

  if(nullptr != m_ActivePtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Active = m_ActivePtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::initialize()
{
  m_AxisToleranceRad = 0.0f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::dataCheck()
{
  setErrorCondition(0);
  setWarningCondition(0);
  initialize();

  DataArrayPath tempPath;

  GroupFeatures::dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainer::Pointer m = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, m_FeatureIdsArrayPath.getDataContainerName(), false);
  if(getErrorCondition() < 0 || nullptr == m.get())
  {
    return;
  }

  QVector<size_t> tDims(1, 0);
  m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getNewCellFeatureAttributeMatrixName(), tDims, AttributeMatrix::Type::CellFeature);

  QVector<size_t> cDims(1, 1);

  QVector<DataArrayPath> cellDataArrayPaths;
  QVector<DataArrayPath> featureDataArrayPaths;

  // Cell Data
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    cellDataArrayPaths.push_back(getFeatureIdsArrayPath());
  }

  m_CellPhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getCellPhasesArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellPhasesPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    cellDataArrayPaths.push_back(getCellPhasesArrayPath());
  }

  tempPath.update(getFeatureIdsArrayPath().getDataContainerName(), getFeatureIdsArrayPath().getAttributeMatrixName(), getCellParentIdsArrayName());
  m_CellParentIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, -1, cDims);                /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellParentIdsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellParentIds = m_CellParentIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_IdentifyGlobAlpha == true)
  {
    tempPath.update(getFeatureIdsArrayPath().getDataContainerName(), getFeatureIdsArrayPath().getAttributeMatrixName(), getGlobAlphaArrayName());
    m_GlobAlphaPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
        this, tempPath, 0, cDims);             /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_GlobAlphaPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_GlobAlpha = m_GlobAlphaPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  // Feature Data
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(),
                                                                                                           cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeaturePhasesPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    featureDataArrayPaths.push_back(getFeaturePhasesArrayPath());
  }

  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getFeatureParentIdsArrayName());
  m_FeatureParentIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, -1, cDims);                   /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureParentIdsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureParentIds = m_FeatureParentIdsPtr.lock()->getPointer(0);
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
    featureDataArrayPaths.push_back(getAvgQuatsArrayPath());
  }

  // NewFeature Data
  cDims[0] = 1;
  tempPath.update(m_FeatureIdsArrayPath.getDataContainerName(), getNewCellFeatureAttributeMatrixName(), getActiveArrayName());
  m_ActivePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, true,
                                                                                                             cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_ActivePtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Active = m_ActivePtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Ensemble Data
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(),
                                                                                                                    cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CrystalStructuresPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::preflight()
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
int32_t MergeColonies::getSeed(int32_t newFid)
{
  setErrorCondition(0);
  setWarningCondition(0);

  int32_t numfeatures = static_cast<int32_t>(m_FeaturePhasesPtr.lock()->getNumberOfTuples());

  SIMPL_RANDOMNG_NEW()
  int32_t seed = -1;
  int32_t randfeature = 0;

  // Precalculate some constants
  int32_t totalFMinus1 = numfeatures - 1;

  size_t counter = 0;
  randfeature = int32_t(float(rg.genrand_res53()) * float(totalFMinus1));
  while(seed == -1 && counter < numfeatures)
  {
    if(randfeature > totalFMinus1)
    {
      randfeature = randfeature - numfeatures;
    }
    if(m_FeatureParentIds[randfeature] == -1)
    {
      seed = randfeature;
    }
    randfeature++;
    counter++;
  }
  if(seed >= 0)
  {
    m_FeatureParentIds[seed] = newFid;
    QVector<size_t> tDims(1, newFid + 1);
    getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName())->getAttributeMatrix(getNewCellFeatureAttributeMatrixName())->resizeAttributeArrays(tDims);
    updateFeatureInstancePointers();
  }
  return seed;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MergeColonies::determineGrouping(int32_t referenceFeature, int32_t neighborFeature, int32_t newFid)
{
  float w = 0.0f;
  float n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;
  bool colony = false;
  QuatF q1 = QuaternionMathF::New();
  QuatF q2 = QuaternionMathF::New();
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  if(m_FeatureParentIds[neighborFeature] == -1 && m_FeaturePhases[referenceFeature] > 0 && m_FeaturePhases[neighborFeature] > 0)
  {
    w = std::numeric_limits<float>::max();
    QuaternionMathF::Copy(avgQuats[referenceFeature], q1);
    uint32_t phase1 = m_CrystalStructures[m_FeaturePhases[referenceFeature]];
    QuaternionMathF::Copy(avgQuats[neighborFeature], q2);
    uint32_t phase2 = m_CrystalStructures[m_FeaturePhases[neighborFeature]];
    if(phase1 == phase2 && (phase1 == Ebsd::CrystalStructure::Hexagonal_High))
    {
      w = m_OrientationOps[phase1]->getMisoQuat(q1, q2, n1, n2, n3);

      FOrientArrayType ax(n1, n2, n3, w);
      FOrientArrayType rod(4);
      OrientationTransforms<FOrientArrayType, float>::ax2ro(ax, rod);
      rod = m_OrientationOps[phase1]->getMDFFZRod(rod);
      OrientationTransforms<FOrientArrayType, float>::ro2ax(rod, ax);
      ax.toAxisAngle(n1, n2, n3, w);

      w = w * (180.0f / SIMPLib::Constants::k_Pi);
      float angdiff1 = fabsf(w - 10.53f);
      float axisdiff1 = acosf(fabsf(n1) * 0.0000f + fabsf(n2) * 0.0000f + fabsf(n3) * 1.0000f);
      if(angdiff1 < m_AngleTolerance && axisdiff1 < m_AxisToleranceRad)
      {
        colony = true;
      }
      float angdiff2 = fabsf(w - 90.00f);
      float axisdiff2 = acosf(fabsf(n1) * 0.9958f + fabsf(n2) * 0.0917f + fabsf(n3) * 0.0000f);
      if(angdiff2 < m_AngleTolerance && axisdiff2 < m_AxisToleranceRad)
      {
        colony = true;
      }
      float angdiff3 = fabsf(w - 60.00f);
      float axisdiff3 = acosf(fabsf(n1) * 1.0000f + fabsf(n2) * 0.0000f + fabsf(n3) * 0.0000f);
      if(angdiff3 < m_AngleTolerance && axisdiff3 < m_AxisToleranceRad)
      {
        colony = true;
      }
      float angdiff4 = fabsf(w - 60.83f);
      float axisdiff4 = acosf(fabsf(n1) * 0.9834f + fabsf(n2) * 0.0905f + fabsf(n3) * 0.1570f);
      if(angdiff4 < m_AngleTolerance && axisdiff4 < m_AxisToleranceRad)
      {
        colony = true;
      }
      float angdiff5 = fabsf(w - 63.26f);
      float axisdiff5 = acosf(fabsf(n1) * 0.9549f + fabsf(n2) * 0.0000f + fabsf(n3) * 0.2969f);
      if(angdiff5 < m_AngleTolerance && axisdiff5 < m_AxisToleranceRad)
      {
        colony = true;
      }
      if(colony == true)
      {
        m_FeatureParentIds[neighborFeature] = newFid;
        return true;
      }
    }
    else if(Ebsd::CrystalStructure::Cubic_High == phase2 && Ebsd::CrystalStructure::Hexagonal_High == phase1)
    {
      colony = check_for_burgers(q2, q1);
      if(colony == true)
      {
        m_FeatureParentIds[neighborFeature] = newFid;
        return true;
      }
    }
    else if(Ebsd::CrystalStructure::Cubic_High == phase1 && Ebsd::CrystalStructure::Hexagonal_High == phase2)
    {
      colony = check_for_burgers(q1, q2);
      if(colony == true)
      {
        m_FeatureParentIds[neighborFeature] = newFid;
        return true;
      }
    }
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::characterize_colonies()
{
  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MergeColonies::check_for_burgers(QuatF betaQuat, QuatF alphaQuat)
{
  float dP = 0.0f;
  float angle = 0.0f;
  float radToDeg = 180.0f / SIMPLib::Constants::k_Pi;

  float gBeta[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float gBetaT[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  FOrientArrayType om(9);
  FOrientTransformsType::qu2om(FOrientArrayType(betaQuat), om);
  om.toGMatrix(gBeta);
  // transpose gBeta so the sample direction is the output when
  // gBeta is multiplied by the crystal directions below
  MatrixMath::Transpose3x3(gBeta, gBetaT);

  float gAlpha[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float gAlphaT[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  FOrientTransformsType::qu2om(FOrientArrayType(alphaQuat), om);
  om.toGMatrix(gAlpha);
  // transpose gBeta so the sample direction is the output when
  // gBeta is multiplied by the crystal directions below
  MatrixMath::Transpose3x3(gAlpha, gAlphaT);

  float mat[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float a[3] = {0.0f, 0.0f, 0.0f};
  float b[3] = {0.0f, 0.0f, 0.0f};
  for(int32_t i = 0; i < 12; i++)
  {
    MatrixMath::Multiply3x3with3x3(gBetaT, crystalDirections[i], mat);
    a[0] = mat[0][2];
    a[1] = mat[1][2];
    a[2] = mat[2][2];
    b[0] = gAlphaT[0][2];
    b[1] = gAlphaT[1][2];
    b[2] = gAlphaT[2][2];
    dP = GeometryMath::CosThetaBetweenVectors(a, b);
    angle = acosf(dP);
    if((angle * radToDeg) < m_AngleTolerance || (180.0f - (angle * radToDeg)) < m_AngleTolerance)
    {
      a[0] = mat[0][0];
      a[1] = mat[1][0];
      a[2] = mat[2][0];
      b[0] = gAlphaT[0][0];
      b[1] = gAlphaT[1][0];
      b[2] = gAlphaT[2][0];
      dP = GeometryMath::CosThetaBetweenVectors(a, b);
      angle = acosf(dP);
      if((angle * radToDeg) < m_AngleTolerance)
      {
        return true;
      }
      if((180.0 - (angle * radToDeg)) < m_AngleTolerance)
      {
        return true;
      }
      b[0] = -0.5 * gAlphaT[0][0] + 0.866025 * gAlphaT[0][1];
      b[1] = -0.5 * gAlphaT[1][0] + 0.866025 * gAlphaT[1][1];
      b[2] = -0.5 * gAlphaT[2][0] + 0.866025 * gAlphaT[2][1];
      dP = GeometryMath::CosThetaBetweenVectors(a, b);
      angle = acosf(dP);
      if((angle * radToDeg) < m_AngleTolerance)
      {
        return true;
      }
      if((180.0 - (angle * radToDeg)) < m_AngleTolerance)
      {
        return true;
      }
      b[0] = -0.5 * gAlphaT[0][0] - 0.866025 * gAlphaT[0][1];
      b[1] = -0.5 * gAlphaT[1][0] - 0.866025 * gAlphaT[1][1];
      b[2] = -0.5 * gAlphaT[2][0] - 0.866025 * gAlphaT[2][1];
      dP = GeometryMath::CosThetaBetweenVectors(a, b);
      angle = acosf(dP);
      if((angle * radToDeg) < m_AngleTolerance)
      {
        return true;
      }
      if((180.0 - (angle * radToDeg)) < m_AngleTolerance)
      {
        return true;
      }
    }
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::identify_globAlpha()
{
  // DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  // int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumberOfTuples();
  // QVector<int> betaSize(numParents, 0);
  // QVector<int> totalSize(numParents, 0);
  // for (int64_t i = 0; i < totalPoints; i++)
  //{
  //  int pnum = m_CellParentIds[i];
  //  totalSize[pnum]++;
  //  if(Ebsd::CrystalStructure::Cubic_High == m_CrystalStructures[m_CellPhases[i]] )
  //  { betaSize[pnum]++; }
  //}
  // for (int64_t i = 0; i < totalPoints; i++)
  //{
  //  int pnum = m_CellParentIds[i];
  //  float ratio = float(betaSize[pnum]) / float(totalSize[pnum]);
  //  if(ratio > 0.0) { m_GlobAlpha[i] = 0; }
  //  else { m_GlobAlpha[i] = 1; }
  //}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeColonies::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  m_AxisToleranceRad = m_AxisTolerance * SIMPLib::Constants::k_Pi / 180.0f;

  GroupFeatures::execute();

  size_t totalFeatures = m_ActivePtr.lock()->getNumberOfTuples();
  if(totalFeatures < 2)
  {
    setErrorCondition(-87000);
    notifyErrorMessage(getHumanLabel(), "The number of Grouped Features was 0 or 1 which means no grouped Features were detected. A grouping value may be set too high", getErrorCondition());
    return;
  }

  int32_t numParents = 0;
  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  for(size_t k = 0; k < totalPoints; k++)
  {
    int32_t featurename = m_FeatureIds[k];
    m_CellParentIds[k] = m_FeatureParentIds[featurename];
    if(m_FeatureParentIds[featurename] > numParents)
    {
      numParents = m_FeatureParentIds[featurename];
    }
  }
  numParents += 1;

  notifyStatusMessage(getHumanLabel(), "Characterizing Colonies");
  characterize_colonies();

  if(true == m_RandomizeParentIds)
  {
    // Generate all the numbers up front
    const int32_t rangeMin = 1;
    const int32_t rangeMax = numParents - 1;
    std::mt19937_64::result_type seed = static_cast<std::mt19937_64::result_type>(std::chrono::steady_clock::now().time_since_epoch().count());
    std::mt19937_64 generator(seed); // Standard mersenne_twister_engine seeded with milliseconds
    std::uniform_int_distribution<int32_t> distribution(rangeMin, rangeMax);

    DataArray<int32_t>::Pointer rndNumbers = DataArray<int32_t>::CreateArray(numParents, "_INTERNAL_USE_ONLY_NewParentIds");
    int32_t* pid = rndNumbers->getPointer(0);
    pid[0] = 0;
    QSet<int32_t> parentIdSet;
    parentIdSet.insert(0);
    for(int32_t i = 1; i < numParents; ++i)
    {
      pid[i] = i; // numberGenerator();
      parentIdSet.insert(pid[i]);
    }

    int32_t r = 0;
    int32_t temp = 0;

    //--- Shuffle elements by randomly exchanging each with one other.
    for(int32_t i = 1; i < numParents; i++)
    {
      r = distribution(generator); // Random remaining position.
      if(r >= numParents)
      {
        continue;
      }
      temp = pid[i];
      pid[i] = pid[r];
      pid[r] = temp;
    }

    // Now adjust all the Feature Id values for each Voxel
    for(size_t i = 0; i < totalPoints; ++i)
    {
      m_CellParentIds[i] = pid[m_CellParentIds[i]];
      m_FeatureParentIds[m_FeatureIds[i]] = m_CellParentIds[i];
    }
  }

  if(m_IdentifyGlobAlpha == true)
  {
    identify_globAlpha();
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MergeColonies::newFilterInstance(bool copyFilterParameters)
{
  MergeColonies::Pointer filter = MergeColonies::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MergeColonies::getCompiledLibraryName()
{
  return ReconstructionConstants::ReconstructionBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MergeColonies::getBrandingString()
{
  return "Reconstruction";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MergeColonies::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << Reconstruction::Version::Major() << "." << Reconstruction::Version::Minor() << "." << Reconstruction::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MergeColonies::getGroupName()
{
  return SIMPL::FilterGroups::ReconstructionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MergeColonies::getSubGroupName()
{
  return SIMPL::FilterSubGroups::GroupingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MergeColonies::getHumanLabel()
{
  return "Merge Colonies";
}
