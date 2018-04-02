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

#ifndef _matchcrystallography_h_
#define _matchcrystallography_h_

#include "OrientationLib/LaueOps/LaueOps.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "SyntheticBuilding/SyntheticBuildingConstants.h"
#include "SyntheticBuilding/SyntheticBuildingVersion.h"

/**
 * @brief The MatchCrystallography class. See [Filter documentation](@ref matchcrystallography) for details.
 */
class MatchCrystallography : public AbstractFilter
{
  Q_OBJECT
public:
  SIMPL_SHARED_POINTERS(MatchCrystallography)
  SIMPL_FILTER_NEW_MACRO(MatchCrystallography)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(MatchCrystallography, AbstractFilter)

  ~MatchCrystallography() override;

  // Input data from the StatsGenerator Data Container (or something equivalent)
  SIMPL_FILTER_PARAMETER(DataArrayPath, InputStatsArrayPath)
  Q_PROPERTY(DataArrayPath InputStatsArrayPath READ getInputStatsArrayPath WRITE setInputStatsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, PhaseTypesArrayPath)
  Q_PROPERTY(DataArrayPath PhaseTypesArrayPath READ getPhaseTypesArrayPath WRITE setPhaseTypesArrayPath)

  // Input data from the Synthetic Data Container (or something equivalent)
  SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
  Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceFeaturesArrayPath)
  Q_PROPERTY(DataArrayPath SurfaceFeaturesArrayPath READ getSurfaceFeaturesArrayPath WRITE setSurfaceFeaturesArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, NeighborListArrayPath)
  Q_PROPERTY(DataArrayPath NeighborListArrayPath READ getNeighborListArrayPath WRITE setNeighborListArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, SharedSurfaceAreaListArrayPath)
  Q_PROPERTY(DataArrayPath SharedSurfaceAreaListArrayPath READ getSharedSurfaceAreaListArrayPath WRITE setSharedSurfaceAreaListArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, NumFeaturesArrayPath)
  Q_PROPERTY(DataArrayPath NumFeaturesArrayPath READ getNumFeaturesArrayPath WRITE setNumFeaturesArrayPath)

  // Created Data Arrays
  SIMPL_FILTER_PARAMETER(QString, CellEulerAnglesArrayName)
  Q_PROPERTY(QString CellEulerAnglesArrayName READ getCellEulerAnglesArrayName WRITE setCellEulerAnglesArrayName)

  SIMPL_FILTER_PARAMETER(QString, VolumesArrayName)
  Q_PROPERTY(QString VolumesArrayName READ getVolumesArrayName WRITE setVolumesArrayName)

  SIMPL_FILTER_PARAMETER(QString, FeatureEulerAnglesArrayName)
  Q_PROPERTY(QString FeatureEulerAnglesArrayName READ getFeatureEulerAnglesArrayName WRITE setFeatureEulerAnglesArrayName)

  SIMPL_FILTER_PARAMETER(QString, AvgQuatsArrayName)
  Q_PROPERTY(QString AvgQuatsArrayName READ getAvgQuatsArrayName WRITE setAvgQuatsArrayName)

  SIMPL_FILTER_PARAMETER(int, MaxIterations)
  Q_PROPERTY(int MaxIterations READ getMaxIterations WRITE setMaxIterations)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  const QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
  */
  const QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  const QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  const QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  const QUuid getUuid() override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  const QString getHumanLabel() const override;

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  void setupFilterParameters() override;

  /**
   * @brief readFilterParameters Reimplemented from @see AbstractFilter class
   */
  void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

  /**
  * @brief preflight Reimplemented from @see AbstractFilter class
  */
  void preflight() override;

signals:
  /**
   * @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
   * be pushed from a user-facing control (such as a widget)
   * @param filter Filter instance pointer
   */
  void updateFilterParameters(AbstractFilter* filter);

  /**
   * @brief parametersChanged Emitted when any Filter parameter is changed internally
   */
  void parametersChanged();

  /**
   * @brief preflightAboutToExecute Emitted just before calling dataCheck()
   */
  void preflightAboutToExecute();

  /**
   * @brief preflightExecuted Emitted just after calling dataCheck()
   */
  void preflightExecuted();

protected:
  MatchCrystallography();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief initializeArrays Initializes the ODF and MDF arrays for each Ensemble
   * @param ensem Ensemble index to initialize
   */
  void initializeArrays(size_t ensem);

  /**
   * @brief determine_volumes Determines the unbiased volume for each Ensemble
   */
  void determine_volumes();

  /**
   * @brief determine_boundary_areas Determines the total Feature surface area
   * for each Ensemble
   */
  void determine_boundary_areas();

  /**
   * @brief assign_eulers Randomly samples orientation space to assign orientations to
   * each Feature based on the incoming statistics
   * @param ensem Ensemble index of the current phase
   */
  void assign_eulers(size_t ensem);

  /**
   * @brief pick_euler Picks a random bin from the incoming orientation statistics
   * @param random Key random value to compare for sampling
   * @param numbins Number of possible bins to sample
   * @return Integer value for bin index
   */
  int32_t pick_euler(float random, int32_t numbins);

  /**
   * @brief MC_LoopBody1 Determines the misorientation change after performing a swap
   * @param feature Feature Id of Feature that has been swapped
   * @param ensem Ensemble index of the current phase
   * @param j Neighbor index for the Feature
   * @param neighsurfarea Surface area for the Feature neighbor
   * @param sym Crystal structure index
   * @param q1 Quaterions for the Feature
   * @param q2 Quaterions for the Feature neighbor
   */
  void MC_LoopBody1(int32_t feature, size_t ensem, size_t j, float neighsurfarea, uint32_t sym, QuatF& q1, QuatF& q2);

  /**
   * @brief MC_LoopBody2 Reinserts the swapped orientation if the swap did not improve the fit
   * @param feature Feature Id of the Feature that has been swapped
   * @param phase Ensemble index of the current phase
   * @param j Neighbor index for the Feature
   * @param neighsurfarea Surface area for the Feature neighbor
   * @param sym Crystal structure index
   * @param q1 Quaternions for the Feature
   * @param q2 Quaternions for the Feature neighbor
   */
  void MC_LoopBody2(int32_t feature, size_t phase, size_t j, float neighsurfarea, uint32_t sym, QuatF& q1, QuatF& q2);

  /**
   * @brief matchCrystallography Swaps orientations for Features unitl convergence to
   * the input statistics
   * @param ensem Ensemble index of the current phase
   */
  void matchCrystallography(size_t ensem);

  /**
   * @brief measure_misorientations Determines the misorientations between each Feature
   * @param ensem Ensemle index of the current phase
   */
  void measure_misorientations(size_t ensem);

private:
  // Cell Data
  DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)
  DEFINE_DATAARRAY_VARIABLE(float, CellEulerAngles)

  // Feature Data
  DEFINE_DATAARRAY_VARIABLE(bool, SurfaceFeatures)
  DEFINE_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
  DEFINE_DATAARRAY_VARIABLE(float, Volumes)
  DEFINE_DATAARRAY_VARIABLE(float, FeatureEulerAngles)
  DEFINE_DATAARRAY_VARIABLE(float, AvgQuats)
  DEFINE_DATAARRAY_VARIABLE(uint32_t, SyntheticCrystalStructures)
  NeighborList<int32_t>::WeakPointer m_NeighborList;
  NeighborList<float>::WeakPointer m_SharedSurfaceAreaList;

  // Ensemble Data
  DEFINE_DATAARRAY_VARIABLE(uint32_t, CrystalStructures)
  DEFINE_DATAARRAY_VARIABLE(PhaseType::EnumType, PhaseTypes)
  DEFINE_DATAARRAY_VARIABLE(int32_t, NumFeatures)
  StatsDataArray::WeakPointer m_StatsDataArray;

  // All other private instance variables
  float m_MdfChange;
  float m_OdfChange;

  std::vector<float> m_UnbiasedVolume;
  std::vector<float> m_TotalSurfaceArea;

  FloatArrayType::Pointer m_ActualOdf;
  FloatArrayType::Pointer m_SimOdf;
  FloatArrayType::Pointer m_ActualMdf;
  FloatArrayType::Pointer m_SimMdf;

  std::vector<std::vector<float>> m_MisorientationLists;

  QVector<LaueOps::Pointer> m_OrientationOps;

public:
  MatchCrystallography(const MatchCrystallography&) = delete; // Copy Constructor Not Implemented
  MatchCrystallography(MatchCrystallography&&) = delete;      // Move Constructor
  MatchCrystallography& operator=(const MatchCrystallography&) = delete; // Copy Assignment Not Implemented
  MatchCrystallography& operator=(MatchCrystallography&&) = delete;      // Move Assignment Not Implemented
};

#endif /* MATCHCRYSTALLOGRAPHY_H_ */
