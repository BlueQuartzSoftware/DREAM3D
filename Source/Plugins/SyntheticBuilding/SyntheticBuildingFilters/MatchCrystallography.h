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

#pragma once

#include <memory>

#include "OrientationLib/LaueOps/LaueOps.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "SyntheticBuilding/SyntheticBuildingConstants.h"
#include "SyntheticBuilding/SyntheticBuildingVersion.h"

#include "SyntheticBuilding/SyntheticBuildingDLLExport.h"

/**
 * @brief The MatchCrystallography class. See [Filter documentation](@ref matchcrystallography) for details.
 */
class SyntheticBuilding_EXPORT MatchCrystallography : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(MatchCrystallography SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(MatchCrystallography)
  PYB11_FILTER_NEW_MACRO(MatchCrystallography)
  PYB11_FILTER_PARAMETER(DataArrayPath, InputStatsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, PhaseTypesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, SurfaceFeaturesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, NeighborListArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, SharedSurfaceAreaListArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, NumFeaturesArrayPath)
  PYB11_FILTER_PARAMETER(QString, CellEulerAnglesArrayName)
  PYB11_FILTER_PARAMETER(QString, VolumesArrayName)
  PYB11_FILTER_PARAMETER(QString, FeatureEulerAnglesArrayName)
  PYB11_FILTER_PARAMETER(QString, AvgQuatsArrayName)
  PYB11_FILTER_PARAMETER(int, MaxIterations)
  PYB11_PROPERTY(DataArrayPath InputStatsArrayPath READ getInputStatsArrayPath WRITE setInputStatsArrayPath)
  PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
  PYB11_PROPERTY(DataArrayPath PhaseTypesArrayPath READ getPhaseTypesArrayPath WRITE setPhaseTypesArrayPath)
  PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
  PYB11_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath SurfaceFeaturesArrayPath READ getSurfaceFeaturesArrayPath WRITE setSurfaceFeaturesArrayPath)
  PYB11_PROPERTY(DataArrayPath NeighborListArrayPath READ getNeighborListArrayPath WRITE setNeighborListArrayPath)
  PYB11_PROPERTY(DataArrayPath SharedSurfaceAreaListArrayPath READ getSharedSurfaceAreaListArrayPath WRITE setSharedSurfaceAreaListArrayPath)
  PYB11_PROPERTY(DataArrayPath NumFeaturesArrayPath READ getNumFeaturesArrayPath WRITE setNumFeaturesArrayPath)
  PYB11_PROPERTY(QString CellEulerAnglesArrayName READ getCellEulerAnglesArrayName WRITE setCellEulerAnglesArrayName)
  PYB11_PROPERTY(QString VolumesArrayName READ getVolumesArrayName WRITE setVolumesArrayName)
  PYB11_PROPERTY(QString FeatureEulerAnglesArrayName READ getFeatureEulerAnglesArrayName WRITE setFeatureEulerAnglesArrayName)
  PYB11_PROPERTY(QString AvgQuatsArrayName READ getAvgQuatsArrayName WRITE setAvgQuatsArrayName)
  PYB11_PROPERTY(int MaxIterations READ getMaxIterations WRITE setMaxIterations)
#endif

public:
  using Self = MatchCrystallography;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  
  /**
   * @brief Returns a NullPointer wrapped by a shared_ptr<>
   * @return
   */
  static Pointer NullPointer();

  /**
   * @brief Creates a new object wrapped in a shared_ptr<>
   * @return
   */
  static Pointer New();

  /**
   * @brief Returns the name of the class for MatchCrystallography
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for MatchCrystallography
   */
  static QString ClassName();

  ~MatchCrystallography() override;

  // Input data from the StatsGenerator Data Container (or something equivalent)
  /**
   * @brief Setter property for InputStatsArrayPath
   */
  void setInputStatsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for InputStatsArrayPath
   * @return Value of InputStatsArrayPath
   */
  DataArrayPath getInputStatsArrayPath() const;

  Q_PROPERTY(DataArrayPath InputStatsArrayPath READ getInputStatsArrayPath WRITE setInputStatsArrayPath)

  /**
   * @brief Setter property for CrystalStructuresArrayPath
   */
  void setCrystalStructuresArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CrystalStructuresArrayPath
   * @return Value of CrystalStructuresArrayPath
   */
  DataArrayPath getCrystalStructuresArrayPath() const;

  Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

  /**
   * @brief Setter property for PhaseTypesArrayPath
   */
  void setPhaseTypesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for PhaseTypesArrayPath
   * @return Value of PhaseTypesArrayPath
   */
  DataArrayPath getPhaseTypesArrayPath() const;

  Q_PROPERTY(DataArrayPath PhaseTypesArrayPath READ getPhaseTypesArrayPath WRITE setPhaseTypesArrayPath)

  // Input data from the Synthetic Data Container (or something equivalent)
  /**
   * @brief Setter property for FeatureIdsArrayPath
   */
  void setFeatureIdsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FeatureIdsArrayPath
   * @return Value of FeatureIdsArrayPath
   */
  DataArrayPath getFeatureIdsArrayPath() const;

  Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

  /**
   * @brief Setter property for FeaturePhasesArrayPath
   */
  void setFeaturePhasesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FeaturePhasesArrayPath
   * @return Value of FeaturePhasesArrayPath
   */
  DataArrayPath getFeaturePhasesArrayPath() const;

  Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

  /**
   * @brief Setter property for SurfaceFeaturesArrayPath
   */
  void setSurfaceFeaturesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SurfaceFeaturesArrayPath
   * @return Value of SurfaceFeaturesArrayPath
   */
  DataArrayPath getSurfaceFeaturesArrayPath() const;

  Q_PROPERTY(DataArrayPath SurfaceFeaturesArrayPath READ getSurfaceFeaturesArrayPath WRITE setSurfaceFeaturesArrayPath)

  /**
   * @brief Setter property for NeighborListArrayPath
   */
  void setNeighborListArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for NeighborListArrayPath
   * @return Value of NeighborListArrayPath
   */
  DataArrayPath getNeighborListArrayPath() const;

  Q_PROPERTY(DataArrayPath NeighborListArrayPath READ getNeighborListArrayPath WRITE setNeighborListArrayPath)

  /**
   * @brief Setter property for SharedSurfaceAreaListArrayPath
   */
  void setSharedSurfaceAreaListArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SharedSurfaceAreaListArrayPath
   * @return Value of SharedSurfaceAreaListArrayPath
   */
  DataArrayPath getSharedSurfaceAreaListArrayPath() const;

  Q_PROPERTY(DataArrayPath SharedSurfaceAreaListArrayPath READ getSharedSurfaceAreaListArrayPath WRITE setSharedSurfaceAreaListArrayPath)

  /**
   * @brief Setter property for NumFeaturesArrayPath
   */
  void setNumFeaturesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for NumFeaturesArrayPath
   * @return Value of NumFeaturesArrayPath
   */
  DataArrayPath getNumFeaturesArrayPath() const;

  Q_PROPERTY(DataArrayPath NumFeaturesArrayPath READ getNumFeaturesArrayPath WRITE setNumFeaturesArrayPath)

  // Created Data Arrays
  /**
   * @brief Setter property for CellEulerAnglesArrayName
   */
  void setCellEulerAnglesArrayName(const QString& value);
  /**
   * @brief Getter property for CellEulerAnglesArrayName
   * @return Value of CellEulerAnglesArrayName
   */
  QString getCellEulerAnglesArrayName() const;

  Q_PROPERTY(QString CellEulerAnglesArrayName READ getCellEulerAnglesArrayName WRITE setCellEulerAnglesArrayName)

  /**
   * @brief Setter property for VolumesArrayName
   */
  void setVolumesArrayName(const QString& value);
  /**
   * @brief Getter property for VolumesArrayName
   * @return Value of VolumesArrayName
   */
  QString getVolumesArrayName() const;

  Q_PROPERTY(QString VolumesArrayName READ getVolumesArrayName WRITE setVolumesArrayName)

  /**
   * @brief Setter property for FeatureEulerAnglesArrayName
   */
  void setFeatureEulerAnglesArrayName(const QString& value);
  /**
   * @brief Getter property for FeatureEulerAnglesArrayName
   * @return Value of FeatureEulerAnglesArrayName
   */
  QString getFeatureEulerAnglesArrayName() const;

  Q_PROPERTY(QString FeatureEulerAnglesArrayName READ getFeatureEulerAnglesArrayName WRITE setFeatureEulerAnglesArrayName)

  /**
   * @brief Setter property for AvgQuatsArrayName
   */
  void setAvgQuatsArrayName(const QString& value);
  /**
   * @brief Getter property for AvgQuatsArrayName
   * @return Value of AvgQuatsArrayName
   */
  QString getAvgQuatsArrayName() const;

  Q_PROPERTY(QString AvgQuatsArrayName READ getAvgQuatsArrayName WRITE setAvgQuatsArrayName)

  /**
   * @brief Setter property for MaxIterations
   */
  void setMaxIterations(int value);
  /**
   * @brief Getter property for MaxIterations
   * @return Value of MaxIterations
   */
  int getMaxIterations() const;

  Q_PROPERTY(int MaxIterations READ getMaxIterations WRITE setMaxIterations)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  QString getCompiledLibraryName() const override;

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
  */
  QString getBrandingString() const override;

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  QString getFilterVersion() const override;

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  QString getGroupName() const override;

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  QString getSubGroupName() const override;

  /**
   * @brief getUuid Return the unique identifier for this filter.
   * @return A QUuid object.
   */
  QUuid getUuid() const override;

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  QString getHumanLabel() const override;

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
  void MC_LoopBody1(int32_t feature, size_t ensem, size_t j, float neighsurfarea, uint32_t sym, const QuatF& q1, const QuatF& q2);

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
  std::weak_ptr<DataArray<int32_t>> m_FeatureIdsPtr;
  int32_t* m_FeatureIds = nullptr;
  std::weak_ptr<DataArray<float>> m_CellEulerAnglesPtr;
  float* m_CellEulerAngles = nullptr;
  std::weak_ptr<DataArray<bool>> m_SurfaceFeaturesPtr;
  bool* m_SurfaceFeatures = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_FeaturePhasesPtr;
  int32_t* m_FeaturePhases = nullptr;
  std::weak_ptr<DataArray<float>> m_VolumesPtr;
  float* m_Volumes = nullptr;
  std::weak_ptr<DataArray<float>> m_FeatureEulerAnglesPtr;
  float* m_FeatureEulerAngles = nullptr;
  std::weak_ptr<DataArray<float>> m_AvgQuatsPtr;
  float* m_AvgQuats = nullptr;
  std::weak_ptr<DataArray<uint32_t>> m_SyntheticCrystalStructuresPtr;
  uint32_t* m_SyntheticCrystalStructures = nullptr;
  std::weak_ptr<DataArray<uint32_t>> m_CrystalStructuresPtr;
  uint32_t* m_CrystalStructures = nullptr;
  std::weak_ptr<DataArray<PhaseType::EnumType>> m_PhaseTypesPtr;
  PhaseType::EnumType* m_PhaseTypes = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_NumFeaturesPtr;
  int32_t* m_NumFeatures = nullptr;

  DataArrayPath m_InputStatsArrayPath = {};
  DataArrayPath m_CrystalStructuresArrayPath = {};
  DataArrayPath m_PhaseTypesArrayPath = {};
  DataArrayPath m_FeatureIdsArrayPath = {};
  DataArrayPath m_FeaturePhasesArrayPath = {};
  DataArrayPath m_SurfaceFeaturesArrayPath = {};
  DataArrayPath m_NeighborListArrayPath = {};
  DataArrayPath m_SharedSurfaceAreaListArrayPath = {};
  DataArrayPath m_NumFeaturesArrayPath = {};
  QString m_CellEulerAnglesArrayName = {};
  QString m_VolumesArrayName = {};
  QString m_FeatureEulerAnglesArrayName = {};
  QString m_AvgQuatsArrayName = {};
  int m_MaxIterations = {};

  // Cell Data

  // Feature Data

  NeighborList<int32_t>::WeakPointer m_NeighborList;
  NeighborList<float>::WeakPointer m_SharedSurfaceAreaList;

  // Ensemble Data

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
  MatchCrystallography(MatchCrystallography&&) = delete;      // Move Constructor Not Implemented
  MatchCrystallography& operator=(const MatchCrystallography&) = delete; // Copy Assignment Not Implemented
  MatchCrystallography& operator=(MatchCrystallography&&) = delete;      // Move Assignment Not Implemented
};

