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

#include "MatchCrystallography.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/StatsData/PrecipitateStatsData.h"
#include "SIMPLib/StatsData/PrimaryStatsData.h"
#include "SIMPLib/Utilities/SIMPLibRandom.h"
#include "SIMPLib/Utilities/TimeUtilities.h"

#include "OrientationLib/SpaceGroupOps/CubicOps.h"
#include "OrientationLib/SpaceGroupOps/HexagonalOps.h"
#include "OrientationLib/SpaceGroupOps/OrthoRhombicOps.h"
#include "OrientationLib/SpaceGroupOps/SpaceGroupOps.h"
#include "OrientationLib/Texture/Texture.hpp"

#include "SyntheticBuilding/SyntheticBuildingConstants.h"
#include "SyntheticBuilding/SyntheticBuildingVersion.h"

#include "EbsdLib/EbsdConstants.h"

// Include the MOC generated file for this class
#include "moc_MatchCrystallography.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MatchCrystallography::MatchCrystallography()
: AbstractFilter()
,
    // These are from the Data Container that has the statistics, crystal structures and Phase Types (ideally),
    m_InputStatsArrayPath(SIMPL::Defaults::StatsGenerator, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::Statistics)
, m_CrystalStructuresArrayPath(SIMPL::Defaults::StatsGenerator, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::CrystalStructures)
, m_PhaseTypesArrayPath(SIMPL::Defaults::StatsGenerator, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::PhaseTypes)
, m_FeatureIdsArrayPath(SIMPL::Defaults::SyntheticVolumeDataContainerName, SIMPL::Defaults::CellAttributeMatrixName, SIMPL::CellData::FeatureIds)
, m_FeaturePhasesArrayPath(SIMPL::Defaults::SyntheticVolumeDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::Phases)
, m_SurfaceFeaturesArrayPath(SIMPL::Defaults::SyntheticVolumeDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::SurfaceFeatures)
, m_NeighborListArrayPath(SIMPL::Defaults::SyntheticVolumeDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::NeighborList)
, m_SharedSurfaceAreaListArrayPath(SIMPL::Defaults::SyntheticVolumeDataContainerName, SIMPL::Defaults::CellFeatureAttributeMatrixName, SIMPL::FeatureData::SharedSurfaceAreaList)
, m_NumFeaturesArrayPath(SIMPL::Defaults::SyntheticVolumeDataContainerName, SIMPL::Defaults::CellEnsembleAttributeMatrixName, SIMPL::EnsembleData::NumFeatures)
, m_CellEulerAnglesArrayName(SIMPL::CellData::EulerAngles)
, m_VolumesArrayName(SIMPL::FeatureData::Volumes)
, m_FeatureEulerAnglesArrayName(SIMPL::FeatureData::EulerAngles)
, m_AvgQuatsArrayName(SIMPL::FeatureData::AvgQuats)
, m_MaxIterations(1)
, m_FeatureIds(nullptr)
, m_CellEulerAngles(nullptr)
, m_SurfaceFeatures(nullptr)
, m_FeaturePhases(nullptr)
, m_Volumes(nullptr)
, m_FeatureEulerAngles(nullptr)
, m_AvgQuats(nullptr)
, m_SyntheticCrystalStructures(nullptr)
, m_CrystalStructures(nullptr)
, m_PhaseTypes(nullptr)
, m_NumFeatures(nullptr)
{
  m_NeighborList = NeighborList<int32_t>::NullPointer();
  m_SharedSurfaceAreaList = NeighborList<float>::NullPointer();
  m_StatsDataArray = StatsDataArray::NullPointer();

  m_MdfChange = m_OdfChange = 0.0f;

  m_ActualOdf = FloatArrayType::NullPointer();
  m_SimOdf = FloatArrayType::NullPointer();
  m_ActualMdf = FloatArrayType::NullPointer();
  m_SimMdf = FloatArrayType::NullPointer();

  m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MatchCrystallography::~MatchCrystallography()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_INTEGER_FP("Maximum Number of Iterations (Swaps)", MaxIterations, FilterParameter::Parameter, MatchCrystallography));

  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Feature Ids", FeatureIdsArrayPath, FilterParameter::RequiredArray, MatchCrystallography, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phases", FeaturePhasesArrayPath, FilterParameter::RequiredArray, MatchCrystallography, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Bool, 1, SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Surface Features", SurfaceFeaturesArrayPath, FilterParameter::RequiredArray, MatchCrystallography, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::NeighborList, 1, SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Neighbor List", NeighborListArrayPath, FilterParameter::RequiredArray, MatchCrystallography, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::NeighborList, 1, SIMPL::AttributeMatrixType::CellFeature, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Shared Surface Area List", SharedSurfaceAreaListArrayPath, FilterParameter::RequiredArray, MatchCrystallography, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::StatsDataArray, 1, SIMPL::AttributeMatrixType::CellEnsemble, SIMPL::Defaults::AnyGeometry);
    QVector<uint32_t> geomTypes;
    geomTypes.push_back(SIMPL::GeometryType::ImageGeometry);
    geomTypes.push_back(SIMPL::GeometryType::UnknownGeometry);
    req.dcGeometryTypes = geomTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Statistics", InputStatsArrayPath, FilterParameter::RequiredArray, MatchCrystallography, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, SIMPL::AttributeMatrixType::CellEnsemble, SIMPL::Defaults::AnyGeometry);
    QVector<uint32_t> geomTypes;
    geomTypes.push_back(SIMPL::GeometryType::ImageGeometry);
    geomTypes.push_back(SIMPL::GeometryType::UnknownGeometry);
    req.dcGeometryTypes = geomTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Crystal Structures", CrystalStructuresArrayPath, FilterParameter::RequiredArray, MatchCrystallography, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt32, 1, SIMPL::AttributeMatrixType::CellEnsemble, SIMPL::Defaults::AnyGeometry);
    QVector<uint32_t> geomTypes;
    geomTypes.push_back(SIMPL::GeometryType::ImageGeometry);
    geomTypes.push_back(SIMPL::GeometryType::UnknownGeometry);
    req.dcGeometryTypes = geomTypes;
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Phase Types", PhaseTypesArrayPath, FilterParameter::RequiredArray, MatchCrystallography, req));
  }
  {
    DataArraySelectionFilterParameter::RequirementType req =
        DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::Int32, 1, SIMPL::AttributeMatrixType::CellEnsemble, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Number of Features", NumFeaturesArrayPath, FilterParameter::RequiredArray, MatchCrystallography, req));
  }
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Euler Angles", CellEulerAnglesArrayName, FilterParameter::CreatedArray, MatchCrystallography));
  parameters.push_back(SeparatorFilterParameter::New("Cell Feature Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Volumes", VolumesArrayName, FilterParameter::CreatedArray, MatchCrystallography));
  parameters.push_back(SIMPL_NEW_STRING_FP("Average Euler Angles", FeatureEulerAnglesArrayName, FilterParameter::CreatedArray, MatchCrystallography));
  parameters.push_back(SIMPL_NEW_STRING_FP("Average Quaternions", AvgQuatsArrayName, FilterParameter::CreatedArray, MatchCrystallography));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setMaxIterations(reader->readValue("MaxIterations", getMaxIterations()));
  setInputStatsArrayPath(reader->readDataArrayPath("InputStatsArrayPath", getInputStatsArrayPath()));
  setCrystalStructuresArrayPath(reader->readDataArrayPath("CrystalStructuresArrayPath", getCrystalStructuresArrayPath()));
  setPhaseTypesArrayPath(reader->readDataArrayPath("PhaseTypesArrayPath", getPhaseTypesArrayPath()));
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath()));
  setFeaturePhasesArrayPath(reader->readDataArrayPath("FeaturePhasesArrayPath", getFeaturePhasesArrayPath()));
  setSurfaceFeaturesArrayPath(reader->readDataArrayPath("SurfaceFeaturesArrayPath", getSurfaceFeaturesArrayPath()));
  setNeighborListArrayPath(reader->readDataArrayPath("NeighborListArrayPath", getNeighborListArrayPath()));
  setNumFeaturesArrayPath(reader->readDataArrayPath("NumFeaturesArrayPath", getNumFeaturesArrayPath()));
  setSharedSurfaceAreaListArrayPath(reader->readDataArrayPath("SharedSurfaceAreaListArrayPath", getSharedSurfaceAreaListArrayPath()));
  setCellEulerAnglesArrayName(reader->readString("CellEulerAnglesArrayName", getCellEulerAnglesArrayName()));
  setVolumesArrayName(reader->readString("VolumesArrayName", getVolumesArrayName()));
  setFeatureEulerAnglesArrayName(reader->readString("FeatureEulerAnglesArrayName", getFeatureEulerAnglesArrayName()));
  setAvgQuatsArrayName(reader->readString("AvgQuatsArrayName", getAvgQuatsArrayName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::initialize()
{
  m_NeighborList = NeighborList<int32_t>::NullPointer();
  m_SharedSurfaceAreaList = NeighborList<float>::NullPointer();
  m_StatsDataArray = StatsDataArray::NullPointer();

  m_MdfChange = m_OdfChange = 0.0f;
  m_UnbiasedVolume.clear();
  m_TotalSurfaceArea.clear();

  m_ActualOdf = FloatArrayType::NullPointer();
  m_SimOdf = FloatArrayType::NullPointer();
  m_ActualMdf = FloatArrayType::NullPointer();
  m_SimMdf = FloatArrayType::NullPointer();
  m_MisorientationLists.clear();

  m_OrientationOps = SpaceGroupOps::getOrientationOpsQVector();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::dataCheck()
{
  setErrorCondition(0);
  initialize();
  DataArrayPath tempPath;

  getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getFeatureIdsArrayPath().getDataContainerName());

  QVector<size_t> cDims(1, 1);
  // Cell Data
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(),
                                                                                                        cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureIdsPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 3;
  tempPath.update(getFeatureIdsArrayPath().getDataContainerName(), getFeatureIdsArrayPath().getAttributeMatrixName(), getCellEulerAnglesArrayName());
  m_CellEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<FloatArrayType, AbstractFilter>(this, tempPath, 0, cDims);
  if(nullptr != m_CellEulerAnglesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Feature Data
  cDims[0] = 1;
  m_FeaturePhasesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeaturePhasesArrayPath(),
                                                                                                           cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeaturePhasesPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeaturePhases = m_FeaturePhasesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_SurfaceFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getSurfaceFeaturesArrayPath(),
                                                                                                          cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SurfaceFeaturesPtr.lock().get())                                                                /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SurfaceFeatures = m_SurfaceFeaturesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getVolumesArrayName());
  m_VolumesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0,
                                                                                                                cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_VolumesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_Volumes = m_VolumesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 3;
  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getFeatureEulerAnglesArrayName());
  m_FeatureEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
      this, tempPath, 0, cDims);                      /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_FeatureEulerAnglesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_FeatureEulerAngles = m_FeatureEulerAnglesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 4;
  tempPath.update(getFeaturePhasesArrayPath().getDataContainerName(), getFeaturePhasesArrayPath().getAttributeMatrixName(), getAvgQuatsArrayName());
  m_AvgQuatsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, tempPath, 0,
                                                                                                                 cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_AvgQuatsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_AvgQuats = m_AvgQuatsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  cDims[0] = 1;
  // Now we are going to get a "Pointer" to the NeighborList object out of the DataContainer
  m_NeighborList = getDataContainerArray()->getPrereqArrayFromPath<NeighborList<int>, AbstractFilter>(this, getNeighborListArrayPath(), cDims);
  m_SharedSurfaceAreaList = getDataContainerArray()->getPrereqArrayFromPath<NeighborList<float>, AbstractFilter>(this, getSharedSurfaceAreaListArrayPath(), cDims);

  // Ensemble Data
  m_NumFeaturesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getNumFeaturesArrayPath(),
                                                                                                         cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_NumFeaturesPtr.lock().get())                                                                   /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_NumFeatures = m_NumFeaturesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_StatsDataArray = getDataContainerArray()->getPrereqArrayFromPath<StatsDataArray, AbstractFilter>(this, getInputStatsArrayPath(), cDims);
  if(m_StatsDataArray.lock() == nullptr)
  {
    QString ss = QObject::tr("Statistics array is not initialized correctly. The path is %1").arg(getInputStatsArrayPath().serialize());
    setErrorCondition(-308);
    notifyErrorMessage(getHumanLabel(), ss, -308);
  }

  m_CrystalStructuresPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getCrystalStructuresArrayPath(), cDims);
  if(nullptr != m_CrystalStructuresPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  }

  m_PhaseTypesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<unsigned int>, AbstractFilter>(this, getPhaseTypesArrayPath(),
                                                                                                             cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_PhaseTypesPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_PhaseTypes = m_PhaseTypesPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Now create the output CrystalStructures Array
  tempPath.update(getNumFeaturesArrayPath().getDataContainerName(), getNumFeaturesArrayPath().getAttributeMatrixName(), "CrystalStructures");
  m_SyntheticCrystalStructuresPtr = getDataContainerArray()->createNonPrereqArrayFromPath<UInt32ArrayType, AbstractFilter>(
      this, tempPath, true, cDims);                           /* Assigns the shared_ptr<>(this, tempPath, true, dims); Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if(nullptr != m_SyntheticCrystalStructuresPtr.lock().get()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_SyntheticCrystalStructures = m_SyntheticCrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::preflight()
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
void MatchCrystallography::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  size_t totalEnsembles = m_CrystalStructuresPtr.lock()->getNumberOfTuples();

  QString ss;
  ss = QObject::tr("Determining Volumes");
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
  determine_volumes();
  if(getCancel() == true)
  {
    return;
  }

  ss = QObject::tr("Determining Boundary Areas");
  notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
  determine_boundary_areas();
  if(getCancel() == true)
  {
    return;
  }

  m_SyntheticCrystalStructures[0] = m_CrystalStructures[0];
  for(size_t i = 1; i < totalEnsembles; ++i)
  {
    if(m_PhaseTypes[i] == SIMPL::PhaseType::PrimaryPhase || m_PhaseTypes[i] == SIMPL::PhaseType::PrecipitatePhase)
    {
      ss = QObject::tr("Initializing Arrays of Phase %1").arg(i);
      notifyStatusMessage(getHumanLabel(), "Initializing Arrays");
      initializeArrays(i);
      if(getErrorCondition() < 0)
      {
        return;
      }
      if(getCancel() == true)
      {
        return;
      }

      ss = QObject::tr("Assigning Eulers to Phase %1").arg(i);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      assign_eulers(i);
      if(getErrorCondition() < 0)
      {
        return;
      }
      if(getCancel() == true)
      {
        return;
      }

      ss = QObject::tr("Measuring Misorientations of Phase %1").arg(i);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      measure_misorientations(i);
      if(getCancel() == true)
      {
        return;
      }

      ss = QObject::tr("Matching Crystallography of Phase %1").arg(i);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
      matchCrystallography(i);
      if(getCancel() == true)
      {
        return;
      }
    }

    m_SyntheticCrystalStructures[i] = m_CrystalStructures[i]; // Copy over the crystal structures from the statsfile into the synthetic file
  }

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::initializeArrays(size_t ensem)
{
  StatsDataArray& statsDataArray = *(m_StatsDataArray.lock());

  if(m_PhaseTypes[ensem] == SIMPL::PhaseType::PrecipitatePhase)
  {
    PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsDataArray[ensem].get());
    if(nullptr == pp)
    {
      QString ss = QObject::tr("Tried to cast a statsDataArray[%1].get() to a PrecipitateStatsData* "
                               "pointer but this resulted in a nullptr pointer. The value at m_PhaseTypes[%2] = %3 does not match up "
                               "with the type of pointer stored in the StatsDataArray (PrecipitateStatsData)\n")
                       .arg(ensem)
                       .arg(ensem)
                       .arg(m_PhaseTypes[ensem]);
      setErrorCondition(-666);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
    m_ActualOdf = pp->getODF();
    m_ActualMdf = pp->getMisorientationBins();
  }
  else if(m_PhaseTypes[ensem] == SIMPL::PhaseType::PrimaryPhase)
  {
    PrimaryStatsData* pp = PrimaryStatsData::SafePointerDownCast(statsDataArray[ensem].get());
    if(nullptr == pp)
    {
      QString ss = QObject::tr("Tried to cast a statsDataArray[%1].get() to a PrimaryStatsData* "
                               "pointer but this resulted in a nullptr pointer. The value at m_PhaseTypes[%2] = %3 does not match up "
                               "with the type of pointer stored in the StatsDataArray (PrimaryStatsData)\n")
                       .arg(ensem)
                       .arg(ensem)
                       .arg(m_PhaseTypes[ensem]);
      setErrorCondition(-666);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
    m_ActualOdf = pp->getODF();
    m_ActualMdf = pp->getMisorientationBins();
  }
  else
  {
    setErrorCondition(-55000);
    QString ss;
    ss = QObject::tr("Improper phase type (%1) for matching crystallography").arg(m_PhaseTypes[ensem]);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  m_SimOdf = FloatArrayType::CreateArray(m_ActualOdf->getSize(), SIMPL::StringConstants::ODF);
  m_SimMdf = FloatArrayType::CreateArray(m_ActualMdf->getSize(), SIMPL::StringConstants::MisorientationBins);
  for(size_t j = 0; j < m_SimOdf->getSize(); j++)
  {
    m_SimOdf->setValue(j, 0.0);
  }
  for(size_t j = 0; j < m_SimMdf->getSize(); j++)
  {
    m_SimMdf->setValue(j, 0.0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::determine_volumes()
{
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_FeatureIdsArrayPath.getDataContainerName());

  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t totalFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
  size_t totalEnsembles = m_CrystalStructuresPtr.lock()->getNumberOfTuples();

  m_UnbiasedVolume.resize(totalEnsembles);
  for(size_t i = 1; i < totalFeatures; i++)
  {
    m_Volumes[i] = 0.0;
  }
  for(size_t i = 0; i < totalPoints; i++)
  {
    m_Volumes[m_FeatureIds[i]]++;
  }
  float res_scalar = m->getGeometryAs<ImageGeom>()->getXRes() * m->getGeometryAs<ImageGeom>()->getYRes() * m->getGeometryAs<ImageGeom>()->getZRes();
  for(size_t i = 1; i < totalFeatures; i++)
  {
    m_Volumes[i] = m_Volumes[i] * res_scalar;
    if(m_SurfaceFeatures[i] == false)
    {
      m_UnbiasedVolume[m_FeaturePhases[i]] = m_UnbiasedVolume[m_FeaturePhases[i]] + m_Volumes[i];
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::determine_boundary_areas()
{
  NeighborList<int32_t>& neighborlist = *(m_NeighborList.lock());
  NeighborList<float>& neighborsurfacearealist = *(m_SharedSurfaceAreaList.lock());

  size_t totalFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();
  size_t totalEnsembles = m_CrystalStructuresPtr.lock()->getNumberOfTuples();

  m_TotalSurfaceArea.assign(totalEnsembles, 0.0f);

  int32_t phase1 = 0, phase2 = 0;
  for(size_t i = 1; i < totalFeatures; i++)
  {
    phase1 = m_FeaturePhases[i];
    size_t size = 0;
    if(neighborlist[i].size() != 0 && neighborsurfacearealist[i].size() == neighborlist[i].size())
    {
      size = neighborlist[i].size();
    }

    for(size_t j = 0; j < size; j++)
    {
      int32_t nname = neighborlist[i][j];
      float neighsurfarea = neighborsurfacearealist[i][j];
      phase2 = m_FeaturePhases[nname];
      if(phase1 == phase2)
      {
        m_TotalSurfaceArea[phase1] = m_TotalSurfaceArea[phase1] + neighsurfarea;
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::assign_eulers(size_t ensem)
{
  uint64_t m_Seed = QDateTime::currentMSecsSinceEpoch();
  SIMPL_RANDOMNG_NEW_SEEDED(m_Seed);

  int32_t numbins = 0;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);
  float random = 0.0f;
  int32_t choose = 0, phase = 0;

  size_t totalFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

  CubicOps cOps;
  HexagonalOps hOps;

  for(size_t i = 1; i < totalFeatures; i++)
  {
    phase = m_FeaturePhases[i];
    if(phase == ensem)
    {
      m_Seed++;
      random = static_cast<float>(rg.genrand_res53());

      if(Ebsd::CrystalStructure::Cubic_High == m_CrystalStructures[phase])
      {
        numbins = cOps.getODFSize();
      };
      if(Ebsd::CrystalStructure::Hexagonal_High == m_CrystalStructures[phase])
      {
        numbins = hOps.getODFSize();
      }

      // If we get to here and numbins is still zero, then an unknown or unsupported crystal structure
      // was used, so we bail
      if(numbins == 0)
      {
        QString ss = QObject::tr("Unkown crystal structure (%1) for phase %2").arg(m_CrystalStructures[phase]).arg(phase);
        setErrorCondition(-666);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }

      choose = pick_euler(random, numbins);

      FOrientArrayType eulers = m_OrientationOps[m_CrystalStructures[ensem]]->determineEulerAngles(m_Seed, choose);
      eulers = m_OrientationOps[m_CrystalStructures[ensem]]->randomizeEulerAngles(eulers);
      m_FeatureEulerAngles[3 * i] = eulers[0];
      m_FeatureEulerAngles[3 * i + 1] = eulers[1];
      m_FeatureEulerAngles[3 * i + 2] = eulers[2];

      FOrientArrayType q(4, 0.0);
      FOrientTransformsType::eu2qu(FOrientArrayType(&(m_FeatureEulerAngles[3 * i]), 3), q);
      QuaternionMathF::Copy(q.toQuaternion(), avgQuats[i]);
      if(m_SurfaceFeatures[i] == false)
      {
        m_SimOdf->setValue(choose, (m_SimOdf->getValue(choose) + m_Volumes[i] / m_UnbiasedVolume[ensem]));
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t MatchCrystallography::pick_euler(float random, int32_t numbins)
{
  int32_t choose = 0;
  float totaldensity = 0.0f;

  for(int32_t j = 0; j < numbins; j++)
  {
    float density = m_ActualOdf->getValue(j);
    float td1 = totaldensity;
    totaldensity = totaldensity + density;
    if(random < totaldensity && random >= td1)
    {
      choose = j;
      break;
    }
  }
  return choose;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::MC_LoopBody1(int32_t feature, size_t ensem, size_t j, float neighsurfarea, uint32_t sym, QuatF& q1, QuatF& q2)
{
  float w = 0.0f;
  float n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;
  float curmiso1 = 0.0f, curmiso2 = 0.0f, curmiso3 = 0.0f;
  size_t curmisobin = 0, newmisobin = 0;

  curmiso1 = m_MisorientationLists[feature][3 * j];
  curmiso2 = m_MisorientationLists[feature][3 * j + 1];
  curmiso3 = m_MisorientationLists[feature][3 * j + 2];

  FOrientArrayType rod(curmiso1, curmiso2, curmiso3, 0.0f);
  float mag = sqrt(curmiso1 * curmiso1 + curmiso2 * curmiso2 + curmiso3 * curmiso3);
  if(mag == 0.0f)
  {
    rod[3] = std::numeric_limits<float>::infinity();
  }
  else
  {
    rod[3] = mag;
    rod[0] = rod[0] / rod[3];
    rod[1] = rod[1] / rod[3];
    rod[2] = rod[2] / rod[3];
  }

  curmisobin = m_OrientationOps[sym]->getMisoBin(rod);
  w = m_OrientationOps[sym]->getMisoQuat(q1, q2, n1, n2, n3);

  FOrientTransformsType::ax2ro(FOrientArrayType(n1, n2, n3, w), rod);
  newmisobin = m_OrientationOps[sym]->getMisoBin(rod);
  m_MdfChange = m_MdfChange + (((m_ActualMdf->getValue(curmisobin) - m_SimMdf->getValue(curmisobin)) * (m_ActualMdf->getValue(curmisobin) - m_SimMdf->getValue(curmisobin))) -
                               ((m_ActualMdf->getValue(curmisobin) - (m_SimMdf->getValue(curmisobin) - (neighsurfarea / m_TotalSurfaceArea[ensem]))) *
                                (m_ActualMdf->getValue(curmisobin) - (m_SimMdf->getValue(curmisobin) - (neighsurfarea / m_TotalSurfaceArea[ensem])))));
  m_MdfChange = m_MdfChange + (((m_ActualMdf->getValue(newmisobin) - m_SimMdf->getValue(newmisobin)) * (m_ActualMdf->getValue(newmisobin) - m_SimMdf->getValue(newmisobin))) -
                               ((m_ActualMdf->getValue(newmisobin) - (m_SimMdf->getValue(newmisobin) + (neighsurfarea / m_TotalSurfaceArea[ensem]))) *
                                (m_ActualMdf->getValue(newmisobin) - (m_SimMdf->getValue(newmisobin) + (neighsurfarea / m_TotalSurfaceArea[ensem])))));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::MC_LoopBody2(int32_t feature, size_t ensem, size_t j, float neighsurfarea, uint32_t sym, QuatF& q1, QuatF& q2)
{
  float w = 0.0f;
  float n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;
  float curmiso1 = 0.0f, curmiso2 = 0.0f, curmiso3 = 0.0f;
  size_t curmisobin = 0, newmisobin = 0;
  float miso1 = 0.0f, miso2 = 0.0f, miso3 = 0.0f;

  curmiso1 = m_MisorientationLists[feature][3 * j];
  curmiso2 = m_MisorientationLists[feature][3 * j + 1];
  curmiso3 = m_MisorientationLists[feature][3 * j + 2];

  FOrientArrayType rod(curmiso1, curmiso2, curmiso3, 0.0f);
  float mag = sqrt(curmiso1 * curmiso1 + curmiso2 * curmiso2 + curmiso3 * curmiso3);
  if(mag == 0.0f)
  {
    rod[3] = std::numeric_limits<float>::infinity();
  }
  else
  {
    rod[3] = mag;
    rod[0] = rod[0] / rod[3];
    rod[1] = rod[1] / rod[3];
    rod[2] = rod[2] / rod[3];
  }

  curmisobin = m_OrientationOps[sym]->getMisoBin(rod);
  w = m_OrientationOps[sym]->getMisoQuat(q1, q2, n1, n2, n3);

  FOrientTransformsType::ax2ro(FOrientArrayType(n1, n2, n3, w), rod);
  newmisobin = m_OrientationOps[sym]->getMisoBin(rod);
  m_MisorientationLists[feature][3 * j] = miso1;
  m_MisorientationLists[feature][3 * j + 1] = miso2;
  m_MisorientationLists[feature][3 * j + 2] = miso3;
  m_SimMdf->setValue(curmisobin, (m_SimMdf->getValue(curmisobin) - (neighsurfarea / m_TotalSurfaceArea[ensem])));
  m_SimMdf->setValue(newmisobin, (m_SimMdf->getValue(newmisobin) + (neighsurfarea / m_TotalSurfaceArea[ensem])));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::matchCrystallography(size_t ensem)
{
  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int32_t>& neighborlist = *(m_NeighborList.lock());
  NeighborList<float>& neighborsurfacearealist = *(m_SharedSurfaceAreaList.lock());
  size_t totalPoints = m_FeatureIdsPtr.lock()->getNumberOfTuples();
  size_t totalFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

  uint64_t m_Seed = QDateTime::currentMSecsSinceEpoch();
  SIMPL_RANDOMNG_NEW_SEEDED(m_Seed);

  int32_t numbins = 0;
  int32_t iterations = 0, badtrycount = 0;
  float random = 0.0f;
  size_t counter = 0;

  QuatF q1;
  QuatF q2;
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  float ea1 = 0.0f, ea2 = 0.0f, ea3 = 0.0f;
  float g1ea1 = 0.0f, g1ea2 = 0.0f, g1ea3 = 0.0f, g2ea1 = 0.0f, g2ea2 = 0.0f, g2ea3 = 0.0f;
  int32_t g1odfbin = 0, g2odfbin = 0;
  float deltaerror = 0.0f;
  float currentodferror = 0.0f, currentmdferror = 0.0f;
  int32_t selectedfeature1 = 0, selectedfeature2 = 0;
  iterations = 0;
  badtrycount = 0;
  if(Ebsd::CrystalStructure::Cubic_High == m_CrystalStructures[ensem])
  {
    numbins = 18 * 18 * 18;
  }
  if(Ebsd::CrystalStructure::Hexagonal_High == m_CrystalStructures[ensem])
  {
    numbins = 36 * 36 * 12;
  }

  uint64_t millis = QDateTime::currentMSecsSinceEpoch();
  uint64_t startMillis = millis;
  int32_t lastIteration = 0;
  while(badtrycount < (m_MaxIterations / 10) && iterations < m_MaxIterations)
  {
    m_Seed++;
    uint64_t currentMillis = QDateTime::currentMSecsSinceEpoch();
    if(currentMillis - millis > 1000)
    {
      QString ss = QObject::tr("Swapping/Switching Orientations Iteration %1/%2").arg(iterations).arg(m_MaxIterations);
      float timeDiff = ((float)iterations / (float)(currentMillis - startMillis));
      float estimatedTime = (float)(m_MaxIterations - iterations) / timeDiff;

      ss += QObject::tr(" || Est. Time Remain: %1 || Iterations/Sec: %2").arg(DREAM3D::convertMillisToHrsMinSecs(estimatedTime)).arg(timeDiff * 1000);
      notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);

      millis = QDateTime::currentMSecsSinceEpoch();
      lastIteration = iterations;
    }
    currentodferror = 0;
    currentmdferror = 0;
    float* actualOdfPtr = m_ActualOdf->getPointer(0);
    float* simOdfPtr = m_SimOdf->getPointer(0);
    float delta = 0.0f;
    for(int32_t i = 0; i < numbins; i++)
    {
      delta = actualOdfPtr[i] - simOdfPtr[i];
      currentodferror = currentodferror + (delta * delta);
    }
    for(int32_t i = 0; i < (numbins); i++)
    {
      currentmdferror = currentmdferror + ((m_ActualMdf->getValue(i) - m_SimMdf->getValue(i)) * (m_ActualMdf->getValue(i) - m_SimMdf->getValue(i)));
    }
    iterations++;
    badtrycount++;
    random = static_cast<float>(rg.genrand_res53());

    if(getCancel() == true)
    {
      return;
    }

    if(random < 0.5) // SwapOutOrientation
    {
      counter = 0;
      selectedfeature1 = int32_t(rg.genrand_res53() * totalFeatures);
      if(selectedfeature1 >= totalFeatures)
      {
        selectedfeature1 = selectedfeature1 - totalFeatures;
      }
      while((m_SurfaceFeatures[selectedfeature1] == true || m_FeaturePhases[selectedfeature1] != static_cast<int32_t>(ensem)) && counter < totalFeatures)
      {
        if(selectedfeature1 >= totalFeatures)
        {
          selectedfeature1 = selectedfeature1 - totalFeatures;
        }
        selectedfeature1++;
        counter++;
      }
      if(selectedfeature1 >= totalFeatures)
      {
        selectedfeature1 = selectedfeature1 - totalFeatures;
      }
      if(counter == totalFeatures)
      {
        badtrycount = 10 * m_NumFeatures[ensem];
      }
      else
      {
        ea1 = m_FeatureEulerAngles[3 * selectedfeature1];
        ea2 = m_FeatureEulerAngles[3 * selectedfeature1 + 1];
        ea3 = m_FeatureEulerAngles[3 * selectedfeature1 + 2];
        FOrientArrayType rod(4, 0.0);
        FOrientTransformsType::eu2ro(FOrientArrayType(&(m_FeatureEulerAngles[3 * selectedfeature1]), 3), rod);

        g1odfbin = m_OrientationOps[m_CrystalStructures[ensem]]->getOdfBin(rod);
        random = static_cast<float>(rg.genrand_res53());
        int32_t choose = 0;

        choose = pick_euler(random, numbins);

        FOrientArrayType g1ea = m_OrientationOps[m_CrystalStructures[ensem]]->determineEulerAngles(m_Seed, choose);
        g1ea = m_OrientationOps[m_CrystalStructures[ensem]]->randomizeEulerAngles(g1ea);
        FOrientArrayType quat(4, 0.0);
        FOrientTransformsType::eu2qu(g1ea, quat);
        q1 = quat.toQuaternion();

        m_OdfChange = ((m_ActualOdf->getValue(choose) - m_SimOdf->getValue(choose)) * (m_ActualOdf->getValue(choose) - m_SimOdf->getValue(choose))) -
                      ((m_ActualOdf->getValue(choose) - (m_SimOdf->getValue(choose) + (m_Volumes[selectedfeature1] / m_UnbiasedVolume[ensem]))) *
                       (m_ActualOdf->getValue(choose) - (m_SimOdf->getValue(choose) + (m_Volumes[selectedfeature1] / m_UnbiasedVolume[ensem]))));
        m_OdfChange = m_OdfChange + (((m_ActualOdf->getValue(g1odfbin) - m_SimOdf->getValue(g1odfbin)) * (m_ActualOdf->getValue(g1odfbin) - m_SimOdf->getValue(g1odfbin))) -
                                     ((m_ActualOdf->getValue(g1odfbin) - (m_SimOdf->getValue(g1odfbin) - (m_Volumes[selectedfeature1] / m_UnbiasedVolume[ensem]))) *
                                      (m_ActualOdf->getValue(g1odfbin) - (m_SimOdf->getValue(g1odfbin) - (m_Volumes[selectedfeature1] / m_UnbiasedVolume[ensem])))));

        m_MdfChange = 0;
        size_t size = 0;
        if(neighborlist[selectedfeature1].size() != 0)
        {
          size = neighborlist[selectedfeature1].size();
        }
        for(size_t j = 0; j < size; j++)
        {
          int32_t neighbor = neighborlist[selectedfeature1][j];
          FOrientTransformsType::eu2qu(FOrientArrayType(&(m_FeatureEulerAngles[3 * neighbor]), 3), quat);
          q1 = quat.toQuaternion();
          float neighsurfarea = neighborsurfacearealist[selectedfeature1][j];
          MC_LoopBody1(selectedfeature1, ensem, j, neighsurfarea, m_CrystalStructures[ensem], q1, q2);
        }

        deltaerror = (m_OdfChange / currentodferror) + (m_MdfChange / currentmdferror);
        if(deltaerror > 0)
        {
          badtrycount = 0;
          m_FeatureEulerAngles[3 * selectedfeature1] = g1ea1;
          m_FeatureEulerAngles[3 * selectedfeature1 + 1] = g1ea2;
          m_FeatureEulerAngles[3 * selectedfeature1 + 2] = g1ea3;
          QuaternionMathF::Copy(q1, avgQuats[selectedfeature1]);
          m_SimOdf->setValue(choose, (m_SimOdf->getValue(choose) + (m_Volumes[selectedfeature1] / m_UnbiasedVolume[ensem])));
          m_SimOdf->setValue(g1odfbin, (m_SimOdf->getValue(g1odfbin) - (m_Volumes[selectedfeature1] / m_UnbiasedVolume[ensem])));
          size = 0;
          if(neighborlist[selectedfeature1].size() != 0)
          {
            size = neighborlist[selectedfeature1].size();
          }
          for(size_t j = 0; j < size; j++)
          {
            int neighbor = neighborlist[selectedfeature1][j];
            FOrientArrayType quat(4);
            FOrientTransformsType::eu2qu(FOrientArrayType(&(m_FeatureEulerAngles[3 * neighbor]), 3), quat);
            q2 = quat.toQuaternion();
            float neighsurfarea = neighborsurfacearealist[selectedfeature1][j];
            MC_LoopBody2(selectedfeature1, ensem, j, neighsurfarea, m_CrystalStructures[ensem], q1, q2);
          }
        }
      }
      if(getCancel() == true)
      {
        return;
      }
    }
    else // SwitchOrientation
    {
      counter = 0;
      selectedfeature1 = int32_t(rg.genrand_res53() * totalFeatures);
      if(selectedfeature1 >= totalFeatures)
      {
        selectedfeature1 = selectedfeature1 - totalFeatures;
      }
      while((m_SurfaceFeatures[selectedfeature1] == true || m_FeaturePhases[selectedfeature1] != static_cast<int32_t>(ensem)) && counter < totalFeatures)
      {
        if(selectedfeature1 >= totalFeatures)
        {
          selectedfeature1 = selectedfeature1 - totalFeatures;
        }
        selectedfeature1++;
        counter++;
      }
      if(selectedfeature1 >= totalFeatures)
      {
        selectedfeature1 = selectedfeature1 - totalFeatures;
      }
      if(counter == totalFeatures)
      {
        badtrycount = 10 * m_NumFeatures[ensem];
      }
      else
      {
        counter = 0;
        selectedfeature2 = int32_t(rg.genrand_res53() * totalFeatures);
        if(selectedfeature2 >= totalFeatures)
        {
          selectedfeature2 = selectedfeature2 - totalFeatures;
        }
        while((m_SurfaceFeatures[selectedfeature2] == true || m_FeaturePhases[selectedfeature2] != static_cast<int32_t>(ensem) || selectedfeature2 == selectedfeature1) && counter < totalFeatures)
        {
          if(selectedfeature2 >= totalFeatures)
          {
            selectedfeature2 = selectedfeature2 - totalFeatures;
          }
          selectedfeature2++;
          counter++;
        }
        if(selectedfeature2 >= totalFeatures)
        {
          selectedfeature2 = selectedfeature2 - totalFeatures;
        }
        if(counter == totalFeatures)
        {
          badtrycount = 10 * m_NumFeatures[ensem];
        }
        else
        {
          g1ea1 = m_FeatureEulerAngles[3 * selectedfeature1];
          g1ea2 = m_FeatureEulerAngles[3 * selectedfeature1 + 1];
          g1ea3 = m_FeatureEulerAngles[3 * selectedfeature1 + 2];
          g2ea1 = m_FeatureEulerAngles[3 * selectedfeature2];
          g2ea2 = m_FeatureEulerAngles[3 * selectedfeature2 + 1];
          g2ea3 = m_FeatureEulerAngles[3 * selectedfeature2 + 2];
          QuaternionMathF::Copy(q1, avgQuats[selectedfeature1]);
          FOrientArrayType rod(4);
          FOrientTransformsType::eu2ro(FOrientArrayType(&(m_FeatureEulerAngles[3 * selectedfeature1]), 3), rod);

          g1odfbin = m_OrientationOps[m_CrystalStructures[ensem]]->getOdfBin(rod);
          QuaternionMathF::Copy(q1, avgQuats[selectedfeature2]);

          FOrientTransformsType::eu2ro(FOrientArrayType(&(m_FeatureEulerAngles[3 * selectedfeature2]), 3), rod);
          g2odfbin = m_OrientationOps[m_CrystalStructures[ensem]]->getOdfBin(rod);

          m_OdfChange =
              ((m_ActualOdf->getValue(g1odfbin) - m_SimOdf->getValue(g1odfbin)) * (m_ActualOdf->getValue(g1odfbin) - m_SimOdf->getValue(g1odfbin))) -
              ((m_ActualOdf->getValue(g1odfbin) - (m_SimOdf->getValue(g1odfbin) - (m_Volumes[selectedfeature1] / m_UnbiasedVolume[ensem]) + (m_Volumes[selectedfeature2] / m_UnbiasedVolume[ensem]))) *
               (m_ActualOdf->getValue(g1odfbin) - (m_SimOdf->getValue(g1odfbin) - (m_Volumes[selectedfeature1] / m_UnbiasedVolume[ensem]) + (m_Volumes[selectedfeature2] / m_UnbiasedVolume[ensem]))));
          m_OdfChange =
              m_OdfChange +
              (((m_ActualOdf->getValue(g2odfbin) - m_SimOdf->getValue(g2odfbin)) * (m_ActualOdf->getValue(g2odfbin) - m_SimOdf->getValue(g2odfbin))) -
               ((m_ActualOdf->getValue(g2odfbin) - (m_SimOdf->getValue(g2odfbin) - (m_Volumes[selectedfeature2] / m_UnbiasedVolume[ensem]) + (m_Volumes[selectedfeature1] / m_UnbiasedVolume[ensem]))) *
                (m_ActualOdf->getValue(g2odfbin) -
                 (m_SimOdf->getValue(g2odfbin) - (m_Volumes[selectedfeature2] / m_UnbiasedVolume[ensem]) + (m_Volumes[selectedfeature1] / m_UnbiasedVolume[ensem])))));

          m_MdfChange = 0;

          FOrientArrayType quat(4);
          FOrientTransformsType::eu2qu(FOrientArrayType(g2ea1, g2ea2, g2ea3), quat);
          q1 = quat.toQuaternion();

          size_t size = 0;
          if(neighborlist[selectedfeature1].size() != 0)
          {
            size = neighborlist[selectedfeature1].size();
          }
          for(size_t j = 0; j < size; j++)
          {
            int32_t neighbor = neighborlist[selectedfeature1][j];
            FOrientTransformsType::eu2qu(FOrientArrayType(&(m_FeatureEulerAngles[3 * neighbor]), 3), quat);
            q2 = quat.toQuaternion();
            float neighsurfarea = neighborsurfacearealist[selectedfeature1][j];
            if(neighbor != static_cast<int>(selectedfeature2))
            {
              MC_LoopBody1(selectedfeature1, ensem, j, neighsurfarea, m_CrystalStructures[ensem], q1, q2);
            }
          }

          FOrientTransformsType::eu2qu(FOrientArrayType(g1ea1, g1ea2, g1ea3), quat);
          q1 = quat.toQuaternion();
          size = 0;
          if(neighborlist[selectedfeature2].size() != 0)
          {
            size = neighborlist[selectedfeature2].size();
          }
          for(size_t j = 0; j < size; j++)
          {
            size_t neighbor = neighborlist[selectedfeature2][j];

            FOrientTransformsType::eu2qu(FOrientArrayType(&(m_FeatureEulerAngles[3 * neighbor]), 3), quat);
            q2 = quat.toQuaternion();
            float neighsurfarea = neighborsurfacearealist[selectedfeature2][j];
            if(neighbor != selectedfeature1)
            {
              MC_LoopBody1(selectedfeature2, ensem, j, neighsurfarea, m_CrystalStructures[ensem], q1, q2);
            }
          }

          deltaerror = (m_OdfChange / currentodferror) + (m_MdfChange / currentmdferror);
          if(deltaerror > 0)
          {

            badtrycount = 0;
            m_FeatureEulerAngles[3 * selectedfeature1] = g2ea1;
            m_FeatureEulerAngles[3 * selectedfeature1 + 1] = g2ea2;
            m_FeatureEulerAngles[3 * selectedfeature1 + 2] = g2ea3;
            m_FeatureEulerAngles[3 * selectedfeature2] = g1ea1;
            m_FeatureEulerAngles[3 * selectedfeature2 + 1] = g1ea2;
            m_FeatureEulerAngles[3 * selectedfeature2 + 2] = g1ea3;
            m_SimOdf->setValue(g1odfbin, (m_SimOdf->getValue(g1odfbin) + (m_Volumes[selectedfeature2] / m_UnbiasedVolume[ensem]) - (m_Volumes[selectedfeature1] / m_UnbiasedVolume[ensem])));
            m_SimOdf->setValue(g2odfbin, (m_SimOdf->getValue(g2odfbin) + (m_Volumes[selectedfeature1] / m_UnbiasedVolume[ensem]) - (m_Volumes[selectedfeature2] / m_UnbiasedVolume[ensem])));

            FOrientTransformsType::eu2qu(FOrientArrayType(g1ea1, g1ea2, g1ea3), quat);
            q1 = quat.toQuaternion();
            QuaternionMathF::Copy(q1, avgQuats[selectedfeature1]);
            size = 0;
            if(neighborlist[selectedfeature1].size() != 0)
            {
              size = neighborlist[selectedfeature1].size();
            }
            for(size_t j = 0; j < size; j++)
            {
              size_t neighbor = neighborlist[selectedfeature1][j];
              ea1 = m_FeatureEulerAngles[3 * neighbor];
              ea2 = m_FeatureEulerAngles[3 * neighbor + 1];
              ea3 = m_FeatureEulerAngles[3 * neighbor + 2];
              FOrientTransformsType::eu2qu(FOrientArrayType(ea1, ea2, ea3), quat);
              q2 = quat.toQuaternion();
              float neighsurfarea = neighborsurfacearealist[selectedfeature1][j];
              if(neighbor != selectedfeature2)
              {
                MC_LoopBody2(selectedfeature1, ensem, j, neighsurfarea, m_CrystalStructures[ensem], q1, q2);
              }
            }

            FOrientTransformsType::eu2qu(FOrientArrayType(g1ea1, g1ea2, g1ea3), quat);
            q1 = quat.toQuaternion();
            QuaternionMathF::Copy(q1, avgQuats[selectedfeature2]);
            size = 0;
            if(neighborlist[selectedfeature2].size() != 0)
            {
              size = neighborlist[selectedfeature2].size();
            }
            for(size_t j = 0; j < size; j++)
            {
              size_t neighbor = neighborlist[selectedfeature2][j];
              ea1 = m_FeatureEulerAngles[3 * neighbor];
              ea2 = m_FeatureEulerAngles[3 * neighbor + 1];
              ea3 = m_FeatureEulerAngles[3 * neighbor + 2];
              FOrientTransformsType::eu2qu(FOrientArrayType(ea1, ea2, ea3), quat);
              q2 = quat.toQuaternion();
              float neighsurfarea = neighborsurfacearealist[selectedfeature2][j];
              if(neighbor != selectedfeature1)
              {
                MC_LoopBody2(selectedfeature2, ensem, j, neighsurfarea, m_CrystalStructures[ensem], q1, q2);
              }
            }
          }
        }
      }
    }
    if(getCancel() == true)
    {
      return;
    }
  }

  if(getCancel() == true)
  {
    return;
  }

  for(size_t i = 0; i < totalPoints; i++)
  {
    m_CellEulerAngles[3 * i] = m_FeatureEulerAngles[3 * m_FeatureIds[i]];
    m_CellEulerAngles[3 * i + 1] = m_FeatureEulerAngles[3 * m_FeatureIds[i] + 1];
    m_CellEulerAngles[3 * i + 2] = m_FeatureEulerAngles[3 * m_FeatureIds[i] + 2];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatchCrystallography::measure_misorientations(size_t ensem)
{
  // But since a pointer is difficult to use operators with we will now create a
  // reference variable to the pointer with the correct variable name that allows
  // us to use the same syntax as the "vector of vectors"
  NeighborList<int32_t>& neighborlist = *(m_NeighborList.lock());
  NeighborList<float>& neighborsurfacearealist = *(m_SharedSurfaceAreaList.lock());
  size_t totalFeatures = m_FeaturePhasesPtr.lock()->getNumberOfTuples();

  float w = 0.0f;
  float n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;
  QuatF q1 = QuaternionMathF::New();
  QuatF q2 = QuaternionMathF::New();
  QuatF* avgQuats = reinterpret_cast<QuatF*>(m_AvgQuats);

  uint32_t crys1 = 0;
  int32_t mbin = 0;

  m_MisorientationLists.resize(totalFeatures);

  for(size_t i = 1; i < totalFeatures; i++)
  {
    if(m_FeaturePhases[i] == ensem)
    {
      if(m_MisorientationLists[i].size() != 0)
      {
        m_MisorientationLists[i].clear();
      }
      if(neighborlist[i].size() != 0)
      {
        m_MisorientationLists[i].assign(neighborlist[i].size() * 3, 0.0f);
      }
      QuaternionMathF::Copy(avgQuats[i], q1);
      crys1 = m_CrystalStructures[ensem];
      size_t size = 0;
      if(neighborlist[i].size() != 0 && neighborsurfacearealist[i].size() == neighborlist[i].size())
      {
        size = neighborlist[i].size();
      }

      for(size_t j = 0; j < size; j++)
      {
        int32_t nname = neighborlist[i][j];
        if(m_FeaturePhases[nname] == ensem)
        {
          w = 10000.0f;
          float neighsurfarea = neighborsurfacearealist[i][j];
          QuaternionMathF::Copy(avgQuats[nname], q2);
          w = m_OrientationOps[crys1]->getMisoQuat(q1, q2, n1, n2, n3);
          FOrientArrayType rod(4);
          FOrientTransformsType::ax2ro(FOrientArrayType(n1, n2, n3, w), rod);
          m_MisorientationLists[i][3 * j] = rod[0];
          m_MisorientationLists[i][3 * j + 1] = rod[1];
          m_MisorientationLists[i][3 * j + 2] = rod[2];
          mbin = m_OrientationOps[crys1]->getMisoBin(rod);
          if(m_SurfaceFeatures[i] == false && (nname > static_cast<int32_t>(i) || m_SurfaceFeatures[nname] == true))
          {
            m_SimMdf->setValue(mbin, (m_SimMdf->getValue(mbin) + (neighsurfarea / m_TotalSurfaceArea[m_FeaturePhases[i]])));
          }
        }
        else
        {
          m_MisorientationLists[i][3 * j] = -100;
          m_MisorientationLists[i][3 * j + 1] = -100;
          m_MisorientationLists[i][3 * j + 2] = -100;
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer MatchCrystallography::newFilterInstance(bool copyFilterParameters)
{
  MatchCrystallography::Pointer filter = MatchCrystallography::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MatchCrystallography::getCompiledLibraryName()
{
  return SyntheticBuildingConstants::SyntheticBuildingBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MatchCrystallography::getBrandingString()
{
  return "SyntheticBuilding";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MatchCrystallography::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream << SyntheticBuilding::Version::Major() << "." << SyntheticBuilding::Version::Minor() << "." << SyntheticBuilding::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MatchCrystallography::getGroupName()
{
  return SIMPL::FilterGroups::SyntheticBuildingFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MatchCrystallography::getSubGroupName()
{
  return SIMPL::FilterSubGroups::CrystallographyFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MatchCrystallography::getHumanLabel()
{
  return "Match Crystallography";
}
