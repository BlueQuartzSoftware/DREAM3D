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

#include "GroupMicroTextureRegions.h"

#include <chrono>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/BooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/FloatFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Math/GeometryMath.h"
#include "SIMPLib/Math/MatrixMath.h"
#include "SIMPLib/Utilities/SIMPLibRandom.h"

#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"

#include "EbsdLib/EbsdConstants.h"

#include "Reconstruction/ReconstructionConstants.h"
#include "Reconstruction/ReconstructionVersion.h"

// Include the MOC generated file for this class
#include "moc_GroupMicroTextureRegions.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GroupMicroTextureRegions::GroupMicroTextureRegions()
: GroupFeatures()
, m_NewCellFeatureAttributeMatrixName(SIMPL::Defaults::NewCellFeatureAttributeMatrixName)
, m_CAxisTolerance(1.0f)
, m_UseRunningAverage(false)
, m_RandomizeParentIds(true)
, m_FeatureIdsArrayPath("", "", "")
, m_FeaturePhasesArrayPath("", "", "")
, m_VolumesArrayPath("", "", "")
, m_AvgQuatsArrayPath("", "", "")
, m_CrystalStructuresArrayPath("", "", "")
, m_CellParentIdsArrayName(SIMPL::CellData::ParentIds)
, m_FeatureParentIdsArrayName(SIMPL::FeatureData::ParentIds)
, m_ActiveArrayName(SIMPL::FeatureData::Active)
, m_FeatureIds(nullptr)
, m_AvgQuats(nullptr)
, m_FeaturePhases(nullptr)
, m_Volumes(nullptr)
, m_CrystalStructures(nullptr)
, m_Active(nullptr)
, m_CellParentIds(nullptr)
, m_FeatureParentIds(nullptr)
{
  m_OrientationOps = LaueOps::getOrientationOpsQVector();
  m_AvgCAxes[0] = 0.0f;
  m_AvgCAxes[1] = 0.0f;
  m_AvgCAxes[2] = 0.0f;
  m_CAxisToleranceRad = 0.0f;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GroupMicroTextureRegions::~GroupMicroTextureRegions()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupMicroTextureRegions::setupFilterParameters()
{
  FilterParameterVector parameters = getFilterParameters();

  parameters.push_back(SIMPL_NEW_BOOL_FP("Group C-Axes With Running Average", UseRunningAverage, FilterParameter::Parameter, GroupMicroTextureRegions));
  parameters.push_back(SIMPL_NEW_FLOAT_FP("C-Axis Alignment Tolerance (Degrees)", CAxisTolerance, FilterParameter::Parameter, GroupMicroTextureRegions));

  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, GroupMicroTextureRegions, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Phases", FeaturePhasesArrayPath, FilterParameter::RequiredArray, GroupMicroTextureRegions, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Volumes", VolumesArrayPath, FilterParameter::RequiredArray, GroupMicroTextureRegions, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Average Quaternions", AvgQuatsArrayPath, FilterParameter::RequiredArray, GroupMicroTextureRegions, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, GroupMicroTextureRegions, req));
  }

  parameters.push_back(SIMPL_NEW_STRING_FP("New Cell Feature Attribute Matrix Name", NewCellFeatureAttributeMatrixName, FilterParameter::CreatedArray, GroupMicroTextureRegions));
  parameters.push_back(SIMPL_NEW_STRING_FP("Parent Ids", CellParentIdsArrayName, FilterParameter::CreatedArray, GroupMicroTextureRegions));
  parameters.push_back(SIMPL_NEW_STRING_FP("Feature Parent Ids", FeatureParentIdsArrayName, FilterParameter::CreatedArray, GroupMicroTextureRegions));
  parameters.push_back(SIMPL_NEW_STRING_FP("Active", ActiveArrayName, FilterParameter::CreatedArray, GroupMicroTextureRegions));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupMicroTextureRegions::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  GroupFeatures::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setNewCellFeatureAttributeMatrixName(reader->readString("NewCellFeatureAttributeMatrixName", getNewCellFeatureAttributeMatrixName()));
  setActiveArrayName(reader->readString("ActiveArrayName", getActiveArrayName()));
  setFeatureParentIdsArrayName(reader->readString("FeatureParentIdsArrayName", getFeatureParentIdsArrayName()));
  setCellParentIdsArrayName(reader->readString("CellParentIdsArrayName", getCellParentIdsArrayName()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath()));
  setAvgQuatsArrayPath(reader->readDataArrayPath("AvgQuatsArrayPath", getAvgQuatsArrayPath()));
  setVolumesArrayPath(reader->readDataArrayPath("VolumesArrayPath", getVolumesArrayPath()));
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setCAxisTolerance(reader->readValue("CAxisTolerance", getCAxisTolerance()));
  setUseRunningAverage(reader->readValue("UseRunningAverage", getUseRunningAverage()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupMicroTextureRegions::updateFeatureInstancePointers()
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
void GroupMicroTextureRegions::initialize()
{
  m_AvgCAxes[0] = 0.0f;
  m_AvgCAxes[1] = 0.0f;
  m_AvgCAxes[2] = 0.0f;
  m_CAxisToleranceRad = 0.0f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupMicroTextureRegions::dataCheck()
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
  if(getErrorCondition() < 0 || nullptr == m)
  {
    return;
  }

  QVector<size_t> tDims(1, 0);
  m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getNewCellFeatureAttributeMatrixName(), tDims, AttributeMatrix::Type::CellFeature);

  QVector<DataArrayPath> dataArrayPaths;

  QVector<size_t> cDims(1, 1);
  // Cell Data
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(m_FeatureIdsArrayPath.getDataContainerName(), m_FeatureIdsArrayPath.getAttributeMatrixName(), getCellParentIdsArrayName());
  m_CellParentIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, -1, cDims);                /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CellParentIdsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellParentIds = m_CellParentIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Feature Data
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

  tempPath.update(m_FeaturePhasesArrayPath.getDataContainerName(), m_FeaturePhasesArrayPath.getAttributeMatrixName(), getFeatureParentIdsArrayName());
  m_FeatureParentIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
      this, tempPath, -1, cDims);                   /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureParentIdsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureParentIds = m_FeatureParentIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_VolumesPtr =
      getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getVolumesArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_VolumesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Volumes = m_VolumesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getVolumesArrayPath());
  }

  cDims[0] = 4;
  m_AvgQuatsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getAvgQuatsArrayPath(),
                                                                                                    cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_AvgQuatsPtr.lock().get())                                                                 /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(getErrorCondition() >= 0)
  {
    dataArrayPaths.push_back(getVolumesArrayPath());
  }

  // New Feature Data
  cDims[0] = 1;
  tempPath.update(m_FeatureIdsArrayPath.getDataContainerName(), getNewCellFeatureAttributeMatrixName(), getActiveArrayName());
  m_ActivePtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, true,
                                                                                                             cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_ActivePtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Active = m_ActivePtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Ensemble Data
  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter>(this, getCrystalStructuresArrayPath(),
                                                                                                                cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_CrystalStructuresPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupMicroTextureRegions::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  GroupFeatures::preflight();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupMicroTextureRegions::randomizeFeatureIds(int64_t totalPoints, int64_t totalFeatures)
{
  notifyStatusMessage(getHumanLabel(), "Randomizing Parent Ids");
  // Generate an even distribution of numbers between the min and max range
  const int32_t rangeMin = 0;
  const int32_t rangeMax = totalFeatures - 1;
  initializeVoxelSeedGenerator(rangeMin, rangeMax);

  DataArray<int32_t>::Pointer rndNumbers = DataArray<int32_t>::CreateArray(totalFeatures, "_INTERNAL_USE_ONLY_NewFeatureIds");

  int32_t* gid = rndNumbers->getPointer(0);
  gid[0] = 0;
  for(int64_t i = 1; i < totalFeatures; ++i)
  {
    gid[i] = i;
  }

  int32_t r = 0;
  int32_t temp;

  //--- Shuffle elements by randomly exchanging each with one other.
  for(int64_t i = 1; i < totalFeatures; i++)
  {
    r = m_Distribution(m_Generator); // Random remaining position.
    if(r >= totalFeatures)
    {
      continue;
    }
    temp = gid[i];
    gid[i] = gid[r];
    gid[r] = temp;
  }

  // Now adjust all the Grain Id values for each Voxel
  for(int64_t i = 0; i < totalPoints; ++i)
  {
    m_CellParentIds[i] = gid[m_CellParentIds[i]];
    m_FeatureParentIds[m_FeatureIds[i]] = m_CellParentIds[i];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t GroupMicroTextureRegions::getSeed(int32_t newFid)
{
  setErrorCondition(0);
  setWarningCondition(0);

  int32_t numfeatures = static_cast<int32_t>(m_FeaturePhasesPtr.lock()->getNumberOfTuples());

  float c1[3] = {0.0f, 0.0f, 0.0f};
  uint32_t phase1 = 0;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);
  float caxis[3] = {0.0f, 0.0f, 1.0f};
  QuatF q1 = QuaternionMathF::New();
  float g1[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float g1t[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

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

    if(m_UseRunningAverage == true)
    {
      QuaternionMathF::Copy(avgQuats[seed], q1);
      phase1 = m_CrystalStructures[m_FeaturePhases[seed]];
      FOrientArrayType om(9);
      FOrientTransformsType::qu2om(FOrientArrayType(q1), om);
      om.toGMatrix(g1);
      // transpose the g matrix so when caxis is multiplied by it
      // it will give the sample direction that the caxis is along
      MatrixMath::Transpose3x3(g1, g1t);
      MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
      // normalize so that the dot product can be taken below without
      // dividing by the magnitudes (they would be 1)
      MatrixMath::Normalize3x1(c1);
      MatrixMath::Copy3x1(c1, m_AvgCAxes);
      MatrixMath::Multiply3x1withConstant(m_AvgCAxes, m_Volumes[seed]);
    }
  }
  return seed;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GroupMicroTextureRegions::determineGrouping(int32_t referenceFeature, int32_t neighborFeature, int32_t newFid)
{
  uint32_t phase1 = 0, phase2 = 0;
  float w = 0.0f;
  float g1[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float g2[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float g1t[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float g2t[3][3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
  float c1[3] = {0.0f, 0.0f, 0.0f};
  float c2[3] = {0.0f, 0.0f, 0.0f};
  float caxis[3] = {0.0f, 0.0f, 1.0f};
  QuatF q1 = QuaternionMathF::New(0.0f, 0.0f, 0.0f, 0.0f);
  QuatF q2 = QuaternionMathF::New(0.0f, 0.0f, 0.0f, 0.0f);
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  if(m_FeatureParentIds[neighborFeature] == -1 && m_FeaturePhases[referenceFeature] > 0 && m_FeaturePhases[neighborFeature] > 0)
  {
    if(m_UseRunningAverage == false)
    {
      QuaternionMathF::Copy(avgQuats[referenceFeature], q1);
      phase1 = m_CrystalStructures[m_FeaturePhases[referenceFeature]];
      FOrientArrayType om(9);
      FOrientTransformsType::qu2om(FOrientArrayType(q1), om);
      om.toGMatrix(g1);
      // transpose the g matrix so when caxis is multiplied by it
      // it will give the sample direction that the caxis is along
      MatrixMath::Transpose3x3(g1, g1t);
      MatrixMath::Multiply3x3with3x1(g1t, caxis, c1);
      // normalize so that the dot product can be taken below without
      // dividing by the magnitudes (they would be 1)
      MatrixMath::Normalize3x1(c1);
    }
    phase2 = m_CrystalStructures[m_FeaturePhases[neighborFeature]];
    if(phase1 == phase2 && (phase1 == Ebsd::CrystalStructure::Hexagonal_High))
    {
      QuaternionMathF::Copy(avgQuats[neighborFeature], q2);
      FOrientArrayType om(9);
      FOrientTransformsType::qu2om(FOrientArrayType(q2), om);
      om.toGMatrix(g2);
      // transpose the g matrix so when caxis is multiplied by it
      // it will give the sample direction that the caxis is along
      MatrixMath::Transpose3x3(g2, g2t);
      MatrixMath::Multiply3x3with3x1(g2t, caxis, c2);
      // normalize so that the dot product can be taken below without
      // dividing by the magnitudes (they would be 1)
      MatrixMath::Normalize3x1(c2);

      if(m_UseRunningAverage == true)
      {
        w = GeometryMath::CosThetaBetweenVectors(m_AvgCAxes, c2);
      }
      else
      {
        w = GeometryMath::CosThetaBetweenVectors(c1, c2);
      }
      SIMPLibMath::boundF(w, -1, 1);
      w = acosf(w);
      if(w <= m_CAxisToleranceRad || (SIMPLib::Constants::k_Pi - w) <= m_CAxisToleranceRad)
      {
        m_FeatureParentIds[neighborFeature] = newFid;
        if(m_UseRunningAverage == true)
        {
          MatrixMath::Multiply3x1withConstant(c2, m_Volumes[neighborFeature]);
          MatrixMath::Add3x1s(m_AvgCAxes, c2, m_AvgCAxes);
        }
        return true;
      }
    }
  }
  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupMicroTextureRegions::initializeVoxelSeedGenerator(const int32_t rangeMin, const int32_t rangeMax)
{
  std::mt19937_64::result_type seed = static_cast<std::mt19937_64::result_type>(std::chrono::steady_clock::now().time_since_epoch().count());
  m_Generator.seed(seed);
  m_Distribution = std::uniform_int_distribution<int64_t>(rangeMin, rangeMax);
  m_TotalRandomNumbersGenerated = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GroupMicroTextureRegions::execute()
{
  setErrorCondition(0);
  setWarningCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  // Convert user defined tolerance to radians.
  m_CAxisToleranceRad = m_CAxisTolerance * SIMPLib::Constants::k_Pi / 180.0f;

  m_AvgCAxes[0] = 0.0f;
  m_AvgCAxes[1] = 0.0f;
  m_AvgCAxes[2] = 0.0f;

  GroupFeatures::execute();

  size_t totalFeatures = m_ActivePtr.lock()->getNumberOfTuples();
  if(totalFeatures < 2)
  {
    setErrorCondition(-87000);
    notifyErrorMessage(getHumanLabel(), "The number of grouped Features was 0 or 1 which means no grouped Features were detected. A grouping value may be set too high", getErrorCondition());
    return;
  }

  int64_t totalPoints = static_cast<int64_t>(m_FeatureIdsPtr.lock()->getNumberOfTuples());
  for(int64_t k = 0; k < totalPoints; k++)
  {
    int32_t featurename = m_FeatureIds[k];
    m_CellParentIds[k] = m_FeatureParentIds[featurename];
  }

  // By default we randomize grains
  if(true == m_RandomizeParentIds)
  {
    randomizeFeatureIds(totalPoints, totalFeatures);
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GroupMicroTextureRegions::newFilterInstance(bool copyFilterParameters)
{
  GroupMicroTextureRegions::Pointer filter = GroupMicroTextureRegions::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GroupMicroTextureRegions::getCompiledLibraryName()
{
  return ReconstructionConstants::ReconstructionBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GroupMicroTextureRegions::getBrandingString()
{
  return "Reconstruction";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GroupMicroTextureRegions::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << Reconstruction::Version::Major() << "." << Reconstruction::Version::Minor() << "." << Reconstruction::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GroupMicroTextureRegions::getGroupName()
{
  return SIMPL::FilterGroups::ReconstructionFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GroupMicroTextureRegions::getSubGroupName()
{
  return SIMPL::FilterSubGroups::GroupingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GroupMicroTextureRegions::getHumanLabel()
{
  return "Group MicroTexture Regions";
}
