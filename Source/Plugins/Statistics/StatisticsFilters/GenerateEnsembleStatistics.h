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

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"


#include "Statistics/DistributionAnalysisOps/DistributionAnalysisOps.h"

#include "Statistics/StatisticsDLLExport.h"

/**
 * @brief The GenerateEnsembleStatistics class. See [Filter documentation](@ref generateensemblestatistics) for details.
 */
class Statistics_EXPORT GenerateEnsembleStatistics : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(GenerateEnsembleStatistics SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(GenerateEnsembleStatistics)
  PYB11_FILTER_NEW_MACRO(GenerateEnsembleStatistics)
  PYB11_FILTER_PARAMETER(DataArrayPath, CellEnsembleAttributeMatrixPath)
  PYB11_FILTER_PARAMETER(QString, PhaseTypesArrayName)
  PYB11_FILTER_PARAMETER(PhaseType::Types, PhaseTypeArray)
  PYB11_FILTER_PARAMETER(DataArrayPath, NeighborListArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, SharedSurfaceAreaListArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, BiasedFeaturesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, EquivalentDiametersArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, NeighborhoodsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, AspectRatiosArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, Omega3sArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, AxisEulerAnglesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, SurfaceFeaturesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, VolumesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, RDFArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, MaxMinRDFArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeatureEulerAnglesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, AvgQuatsArrayPath)
  PYB11_FILTER_PARAMETER(QString, StatisticsArrayName)
  PYB11_FILTER_PARAMETER(bool, IncludeRadialDistFunc)
  PYB11_FILTER_PARAMETER(bool, CalculateMorphologicalStats)
  PYB11_FILTER_PARAMETER(int, SizeDistributionFitType)
  PYB11_FILTER_PARAMETER(bool, ComputeAspectRatioDistribution)
  PYB11_FILTER_PARAMETER(int, AspectRatioDistributionFitType)
  PYB11_FILTER_PARAMETER(bool, ComputeOmega3Distribution)
  PYB11_FILTER_PARAMETER(int, Omega3DistributionFitType)
  PYB11_FILTER_PARAMETER(bool, ComputeNeighborhoodDistribution)
  PYB11_FILTER_PARAMETER(int, NeighborhoodDistributionFitType)
  PYB11_FILTER_PARAMETER(bool, CalculateCrystallographicStats)
  PYB11_FILTER_PARAMETER(bool, CalculateODF)
  PYB11_FILTER_PARAMETER(bool, CalculateMDF)
  PYB11_FILTER_PARAMETER(bool, CalculateAxisODF)
  PYB11_FILTER_PARAMETER(float, SizeCorrelationResolution)
  PYB11_PROPERTY(DataArrayPath CellEnsembleAttributeMatrixPath READ getCellEnsembleAttributeMatrixPath WRITE setCellEnsembleAttributeMatrixPath)
  PYB11_PROPERTY(QString PhaseTypesArrayName READ getPhaseTypesArrayName WRITE setPhaseTypesArrayName)
  PYB11_PROPERTY(PhaseType::Types PhaseTypeArray READ getPhaseTypeArray WRITE setPhaseTypeArray)
  PYB11_PROPERTY(PhaseType::Types PhaseTypeData READ getPhaseTypeData WRITE setPhaseTypeData)
  PYB11_PROPERTY(DataArrayPath NeighborListArrayPath READ getNeighborListArrayPath WRITE setNeighborListArrayPath)
  PYB11_PROPERTY(DataArrayPath SharedSurfaceAreaListArrayPath READ getSharedSurfaceAreaListArrayPath WRITE setSharedSurfaceAreaListArrayPath)
  PYB11_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath BiasedFeaturesArrayPath READ getBiasedFeaturesArrayPath WRITE setBiasedFeaturesArrayPath)
  PYB11_PROPERTY(DataArrayPath EquivalentDiametersArrayPath READ getEquivalentDiametersArrayPath WRITE setEquivalentDiametersArrayPath)
  PYB11_PROPERTY(DataArrayPath NeighborhoodsArrayPath READ getNeighborhoodsArrayPath WRITE setNeighborhoodsArrayPath)
  PYB11_PROPERTY(DataArrayPath AspectRatiosArrayPath READ getAspectRatiosArrayPath WRITE setAspectRatiosArrayPath)
  PYB11_PROPERTY(DataArrayPath Omega3sArrayPath READ getOmega3sArrayPath WRITE setOmega3sArrayPath)
  PYB11_PROPERTY(DataArrayPath AxisEulerAnglesArrayPath READ getAxisEulerAnglesArrayPath WRITE setAxisEulerAnglesArrayPath)
  PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
  PYB11_PROPERTY(DataArrayPath SurfaceFeaturesArrayPath READ getSurfaceFeaturesArrayPath WRITE setSurfaceFeaturesArrayPath)
  PYB11_PROPERTY(DataArrayPath VolumesArrayPath READ getVolumesArrayPath WRITE setVolumesArrayPath)
  PYB11_PROPERTY(DataArrayPath RDFArrayPath READ getRDFArrayPath WRITE setRDFArrayPath)
  PYB11_PROPERTY(DataArrayPath MaxMinRDFArrayPath READ getMaxMinRDFArrayPath WRITE setMaxMinRDFArrayPath)
  PYB11_PROPERTY(DataArrayPath FeatureEulerAnglesArrayPath READ getFeatureEulerAnglesArrayPath WRITE setFeatureEulerAnglesArrayPath)
  PYB11_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)
  PYB11_PROPERTY(QString StatisticsArrayName READ getStatisticsArrayName WRITE setStatisticsArrayName)
  PYB11_PROPERTY(bool IncludeRadialDistFunc READ getIncludeRadialDistFunc WRITE setIncludeRadialDistFunc)
  PYB11_PROPERTY(bool CalculateMorphologicalStats READ getCalculateMorphologicalStats WRITE setCalculateMorphologicalStats)
  PYB11_PROPERTY(int SizeDistributionFitType READ getSizeDistributionFitType WRITE setSizeDistributionFitType)
  PYB11_PROPERTY(bool ComputeAspectRatioDistribution READ getComputeAspectRatioDistribution WRITE setComputeAspectRatioDistribution)
  PYB11_PROPERTY(int AspectRatioDistributionFitType READ getAspectRatioDistributionFitType WRITE setAspectRatioDistributionFitType)
  PYB11_PROPERTY(bool ComputeOmega3Distribution READ getComputeOmega3Distribution WRITE setComputeOmega3Distribution)
  PYB11_PROPERTY(int Omega3DistributionFitType READ getOmega3DistributionFitType WRITE setOmega3DistributionFitType)
  PYB11_PROPERTY(bool ComputeNeighborhoodDistribution READ getComputeNeighborhoodDistribution WRITE setComputeNeighborhoodDistribution)
  PYB11_PROPERTY(int NeighborhoodDistributionFitType READ getNeighborhoodDistributionFitType WRITE setNeighborhoodDistributionFitType)
  PYB11_PROPERTY(bool CalculateCrystallographicStats READ getCalculateCrystallographicStats WRITE setCalculateCrystallographicStats)
  PYB11_PROPERTY(bool CalculateODF READ getCalculateODF WRITE setCalculateODF)
  PYB11_PROPERTY(bool CalculateMDF READ getCalculateMDF WRITE setCalculateMDF)
  PYB11_PROPERTY(bool CalculateAxisODF READ getCalculateAxisODF WRITE setCalculateAxisODF)
  PYB11_PROPERTY(float SizeCorrelationResolution READ getSizeCorrelationResolution WRITE setSizeCorrelationResolution)
#endif

public:
  using Self = GenerateEnsembleStatistics;
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
   * @brief Returns the name of the class for GenerateEnsembleStatistics
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for GenerateEnsembleStatistics
   */
  static QString ClassName();

  ~GenerateEnsembleStatistics() override;

  /**
   * @brief Setter property for CellEnsembleAttributeMatrixPath
   */
  void setCellEnsembleAttributeMatrixPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CellEnsembleAttributeMatrixPath
   * @return Value of CellEnsembleAttributeMatrixPath
   */
  DataArrayPath getCellEnsembleAttributeMatrixPath() const;

  Q_PROPERTY(DataArrayPath CellEnsembleAttributeMatrixPath READ getCellEnsembleAttributeMatrixPath WRITE setCellEnsembleAttributeMatrixPath)

  /**
   * @brief Setter property for PhaseTypesArrayName
   */
  void setPhaseTypesArrayName(const QString& value);
  /**
   * @brief Getter property for PhaseTypesArrayName
   * @return Value of PhaseTypesArrayName
   */
  QString getPhaseTypesArrayName() const;

  Q_PROPERTY(QString PhaseTypesArrayName READ getPhaseTypesArrayName WRITE setPhaseTypesArrayName)

  /**
   * @brief Setter property for PhaseTypeArray
   */
  void setPhaseTypeArray(const PhaseType::Types& value);
  /**
   * @brief Getter property for PhaseTypeArray
   * @return Value of PhaseTypeArray
   */
  PhaseType::Types getPhaseTypeArray() const;

  Q_PROPERTY(PhaseType::Types PhaseTypeArray READ getPhaseTypeArray WRITE setPhaseTypeArray)

  /**
   * @brief Setter property for PhaseTypeData
   */
  void setPhaseTypeData(const PhaseType::Types& value);
  /**
   * @brief Getter property for PhaseTypeData
   * @return Value of PhaseTypeData
   */
  PhaseType::Types getPhaseTypeData() const;

  Q_PROPERTY(PhaseType::Types PhaseTypeData READ getPhaseTypeData WRITE setPhaseTypeData)

  int getPhaseCount();
  Q_PROPERTY(int PhaseCount READ getPhaseCount)

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
   * @brief Setter property for BiasedFeaturesArrayPath
   */
  void setBiasedFeaturesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for BiasedFeaturesArrayPath
   * @return Value of BiasedFeaturesArrayPath
   */
  DataArrayPath getBiasedFeaturesArrayPath() const;

  Q_PROPERTY(DataArrayPath BiasedFeaturesArrayPath READ getBiasedFeaturesArrayPath WRITE setBiasedFeaturesArrayPath)

  /**
   * @brief Setter property for EquivalentDiametersArrayPath
   */
  void setEquivalentDiametersArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for EquivalentDiametersArrayPath
   * @return Value of EquivalentDiametersArrayPath
   */
  DataArrayPath getEquivalentDiametersArrayPath() const;

  Q_PROPERTY(DataArrayPath EquivalentDiametersArrayPath READ getEquivalentDiametersArrayPath WRITE setEquivalentDiametersArrayPath)

  /**
   * @brief Setter property for NeighborhoodsArrayPath
   */
  void setNeighborhoodsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for NeighborhoodsArrayPath
   * @return Value of NeighborhoodsArrayPath
   */
  DataArrayPath getNeighborhoodsArrayPath() const;

  Q_PROPERTY(DataArrayPath NeighborhoodsArrayPath READ getNeighborhoodsArrayPath WRITE setNeighborhoodsArrayPath)

  /**
   * @brief Setter property for AspectRatiosArrayPath
   */
  void setAspectRatiosArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for AspectRatiosArrayPath
   * @return Value of AspectRatiosArrayPath
   */
  DataArrayPath getAspectRatiosArrayPath() const;

  Q_PROPERTY(DataArrayPath AspectRatiosArrayPath READ getAspectRatiosArrayPath WRITE setAspectRatiosArrayPath)

  /**
   * @brief Setter property for Omega3sArrayPath
   */
  void setOmega3sArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for Omega3sArrayPath
   * @return Value of Omega3sArrayPath
   */
  DataArrayPath getOmega3sArrayPath() const;

  Q_PROPERTY(DataArrayPath Omega3sArrayPath READ getOmega3sArrayPath WRITE setOmega3sArrayPath)

  /**
   * @brief Setter property for AxisEulerAnglesArrayPath
   */
  void setAxisEulerAnglesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for AxisEulerAnglesArrayPath
   * @return Value of AxisEulerAnglesArrayPath
   */
  DataArrayPath getAxisEulerAnglesArrayPath() const;

  Q_PROPERTY(DataArrayPath AxisEulerAnglesArrayPath READ getAxisEulerAnglesArrayPath WRITE setAxisEulerAnglesArrayPath)

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
   * @brief Setter property for VolumesArrayPath
   */
  void setVolumesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for VolumesArrayPath
   * @return Value of VolumesArrayPath
   */
  DataArrayPath getVolumesArrayPath() const;

  Q_PROPERTY(DataArrayPath VolumesArrayPath READ getVolumesArrayPath WRITE setVolumesArrayPath)

  // These filter parameters are tied to RDF data, but the filter that can calculate them (FindFeatureClustering)
  // is not slated for public release yet, so turning these off for now
  /**
   * @brief Setter property for RDFArrayPath
   */
  void setRDFArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for RDFArrayPath
   * @return Value of RDFArrayPath
   */
  DataArrayPath getRDFArrayPath() const;

  Q_PROPERTY(DataArrayPath RDFArrayPath READ getRDFArrayPath WRITE setRDFArrayPath)

  /**
   * @brief Setter property for MaxMinRDFArrayPath
   */
  void setMaxMinRDFArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for MaxMinRDFArrayPath
   * @return Value of MaxMinRDFArrayPath
   */
  DataArrayPath getMaxMinRDFArrayPath() const;

  Q_PROPERTY(DataArrayPath MaxMinRDFArrayPath READ getMaxMinRDFArrayPath WRITE setMaxMinRDFArrayPath)

  /**
   * @brief Setter property for FeatureEulerAnglesArrayPath
   */
  void setFeatureEulerAnglesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FeatureEulerAnglesArrayPath
   * @return Value of FeatureEulerAnglesArrayPath
   */
  DataArrayPath getFeatureEulerAnglesArrayPath() const;

  Q_PROPERTY(DataArrayPath FeatureEulerAnglesArrayPath READ getFeatureEulerAnglesArrayPath WRITE setFeatureEulerAnglesArrayPath)

  /**
   * @brief Setter property for AvgQuatsArrayPath
   */
  void setAvgQuatsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for AvgQuatsArrayPath
   * @return Value of AvgQuatsArrayPath
   */
  DataArrayPath getAvgQuatsArrayPath() const;

  Q_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)

  /**
   * @brief Setter property for StatisticsArrayName
   */
  void setStatisticsArrayName(const QString& value);
  /**
   * @brief Getter property for StatisticsArrayName
   * @return Value of StatisticsArrayName
   */
  QString getStatisticsArrayName() const;

  Q_PROPERTY(QString StatisticsArrayName READ getStatisticsArrayName WRITE setStatisticsArrayName)

  /**
   * @brief Setter property for IncludeRadialDistFunc
   */
  void setIncludeRadialDistFunc(bool value);
  /**
   * @brief Getter property for IncludeRadialDistFunc
   * @return Value of IncludeRadialDistFunc
   */
  bool getIncludeRadialDistFunc() const;

  Q_PROPERTY(bool IncludeRadialDistFunc READ getIncludeRadialDistFunc WRITE setIncludeRadialDistFunc)

  /**
   * @brief Setter property for CalculateMorphologicalStats
   */
  void setCalculateMorphologicalStats(bool value);
  /**
   * @brief Getter property for CalculateMorphologicalStats
   * @return Value of CalculateMorphologicalStats
   */
  bool getCalculateMorphologicalStats() const;

  Q_PROPERTY(bool CalculateMorphologicalStats READ getCalculateMorphologicalStats WRITE setCalculateMorphologicalStats)

  /**
   * @brief Setter property for ComputeSizeDistribution
   */
  void setComputeSizeDistribution(bool value);
  /**
   * @brief Getter property for ComputeSizeDistribution
   * @return Value of ComputeSizeDistribution
   */
  bool getComputeSizeDistribution() const;

  // Q_PROPERTY(bool ComputeSizeDistribution READ getComputeSizeDistribution WRITE setComputeSizeDistribution)

  /**
   * @brief Setter property for SizeDistributionFitType
   */
  void setSizeDistributionFitType(int value);
  /**
   * @brief Getter property for SizeDistributionFitType
   * @return Value of SizeDistributionFitType
   */
  int getSizeDistributionFitType() const;

  Q_PROPERTY(int SizeDistributionFitType READ getSizeDistributionFitType WRITE setSizeDistributionFitType)

  /**
   * @brief Setter property for ComputeAspectRatioDistribution
   */
  void setComputeAspectRatioDistribution(bool value);
  /**
   * @brief Getter property for ComputeAspectRatioDistribution
   * @return Value of ComputeAspectRatioDistribution
   */
  bool getComputeAspectRatioDistribution() const;

  // Q_PROPERTY(bool ComputeAspectRatioDistribution READ getComputeAspectRatioDistribution WRITE setComputeAspectRatioDistribution)

  /**
   * @brief Setter property for AspectRatioDistributionFitType
   */
  void setAspectRatioDistributionFitType(int value);
  /**
   * @brief Getter property for AspectRatioDistributionFitType
   * @return Value of AspectRatioDistributionFitType
   */
  int getAspectRatioDistributionFitType() const;

  Q_PROPERTY(int AspectRatioDistributionFitType READ getAspectRatioDistributionFitType WRITE setAspectRatioDistributionFitType)

  /**
   * @brief Setter property for ComputeOmega3Distribution
   */
  void setComputeOmega3Distribution(bool value);
  /**
   * @brief Getter property for ComputeOmega3Distribution
   * @return Value of ComputeOmega3Distribution
   */
  bool getComputeOmega3Distribution() const;

  // Q_PROPERTY(bool ComputeOmega3Distribution READ getComputeOmega3Distribution WRITE setComputeOmega3Distribution)

  /**
   * @brief Setter property for Omega3DistributionFitType
   */
  void setOmega3DistributionFitType(int value);
  /**
   * @brief Getter property for Omega3DistributionFitType
   * @return Value of Omega3DistributionFitType
   */
  int getOmega3DistributionFitType() const;

  Q_PROPERTY(int Omega3DistributionFitType READ getOmega3DistributionFitType WRITE setOmega3DistributionFitType)

  /**
   * @brief Setter property for ComputeNeighborhoodDistribution
   */
  void setComputeNeighborhoodDistribution(bool value);
  /**
   * @brief Getter property for ComputeNeighborhoodDistribution
   * @return Value of ComputeNeighborhoodDistribution
   */
  bool getComputeNeighborhoodDistribution() const;

  // Q_PROPERTY(bool ComputeNeighborhoodDistribution READ getComputeNeighborhoodDistribution WRITE setComputeNeighborhoodDistribution)

  /**
   * @brief Setter property for NeighborhoodDistributionFitType
   */
  void setNeighborhoodDistributionFitType(int value);
  /**
   * @brief Getter property for NeighborhoodDistributionFitType
   * @return Value of NeighborhoodDistributionFitType
   */
  int getNeighborhoodDistributionFitType() const;

  Q_PROPERTY(int NeighborhoodDistributionFitType READ getNeighborhoodDistributionFitType WRITE setNeighborhoodDistributionFitType)

  /**
   * @brief Setter property for CalculateCrystallographicStats
   */
  void setCalculateCrystallographicStats(bool value);
  /**
   * @brief Getter property for CalculateCrystallographicStats
   * @return Value of CalculateCrystallographicStats
   */
  bool getCalculateCrystallographicStats() const;

  Q_PROPERTY(bool CalculateCrystallographicStats READ getCalculateCrystallographicStats WRITE setCalculateCrystallographicStats)

  /**
   * @brief Setter property for CalculateODF
   */
  void setCalculateODF(bool value);
  /**
   * @brief Getter property for CalculateODF
   * @return Value of CalculateODF
   */
  bool getCalculateODF() const;

  // Q_PROPERTY(bool CalculateODF READ getCalculateODF WRITE setCalculateODF)

  /**
   * @brief Setter property for CalculateMDF
   */
  void setCalculateMDF(bool value);
  /**
   * @brief Getter property for CalculateMDF
   * @return Value of CalculateMDF
   */
  bool getCalculateMDF() const;

  // Q_PROPERTY(bool CalculateMDF READ getCalculateMDF WRITE setCalculateMDF)

  /**
   * @brief Setter property for CalculateAxisODF
   */
  void setCalculateAxisODF(bool value);
  /**
   * @brief Getter property for CalculateAxisODF
   * @return Value of CalculateAxisODF
   */
  bool getCalculateAxisODF() const;

  // Q_PROPERTY(bool CalculateAxisODF READ getCalculateAxisODF WRITE setCalculateAxisODF)

  /**
   * @brief Setter property for SizeCorrelationResolution
   */
  void setSizeCorrelationResolution(float value);
  /**
   * @brief Getter property for SizeCorrelationResolution
   * @return Value of SizeCorrelationResolution
   */
  float getSizeCorrelationResolution() const;

  Q_PROPERTY(float SizeCorrelationResolution READ getSizeCorrelationResolution WRITE setSizeCorrelationResolution)

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
  * @brief readFilterParametersFromJson Reads the filter parameters from a file
  * @param reader Reader that is used to read the parameters from a file
  */
  void readFilterParameters(QJsonObject& obj) override;

  /**
  * @brief writeFilterParametersToJson Writes the filter parameters to a file
  * @param root The root json object
  */
  void writeFilterParameters(QJsonObject& obj) const override;

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
  GenerateEnsembleStatistics();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief gatherSizeStats Consolidates Feature size statistics
   */
  void gatherSizeStats();

  /**
   * @brief gatherAspectRatioStats Consolidates Feature aspect ratio statistics
   */
  void gatherAspectRatioStats();

  /**
   * @brief gatherOmega3Stats Consolidates Feature Omega3 statistics
   */
  void gatherOmega3Stats();

  /**
   * @brief gatherNeighborhoodStats Consolidates Feature neighborhood statistics
   */
  void gatherNeighborhoodStats();

  /**
   * @brief gatherMDFStats Consolidates Feature MDF statistics
   */
  void gatherMDFStats();

  /**
   * @brief gatherODFStats Consolidates Feature ODF statistics
   */
  void gatherODFStats();

  /**
   * @brief gatherAxisODFStats Consolidates Feature axis ODF statistics
   */
  void gatherAxisODFStats();

  /**
   * @brief gatherRadialDistFunc Consolidates Feature RDF statistics
   */
  void gatherRadialDistFunc();

  /**
   * @brief calculatePPTBoundaryFrac Consolidates Feature precipitate boundary fraction statistics
   */
  void calculatePPTBoundaryFrac();

private:
  std::weak_ptr<DataArray<float>> m_AvgQuatsPtr;
  float* m_AvgQuats = nullptr;
  std::weak_ptr<DataArray<float>> m_FeatureEulerAnglesPtr;
  float* m_FeatureEulerAngles = nullptr;
  std::weak_ptr<DataArray<float>> m_VolumesPtr;
  float* m_Volumes = nullptr;
  std::weak_ptr<DataArray<bool>> m_BiasedFeaturesPtr;
  bool* m_BiasedFeatures = nullptr;
  std::weak_ptr<DataArray<bool>> m_SurfaceFeaturesPtr;
  bool* m_SurfaceFeatures = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_FeaturePhasesPtr;
  int32_t* m_FeaturePhases = nullptr;
  std::weak_ptr<DataArray<float>> m_AxisEulerAnglesPtr;
  float* m_AxisEulerAngles = nullptr;
  std::weak_ptr<DataArray<float>> m_RadialDistFuncPtr;
  float* m_RadialDistFunc = nullptr;
  std::weak_ptr<DataArray<float>> m_MaxMinRadialDistFuncPtr;
  float* m_MaxMinRadialDistFunc = nullptr;
  std::weak_ptr<DataArray<float>> m_Omega3sPtr;
  float* m_Omega3s = nullptr;
  std::weak_ptr<DataArray<float>> m_AspectRatiosPtr;
  float* m_AspectRatios = nullptr;
  std::weak_ptr<DataArray<float>> m_EquivalentDiametersPtr;
  float* m_EquivalentDiameters = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_NeighborhoodsPtr;
  int32_t* m_Neighborhoods = nullptr;
  std::weak_ptr<DataArray<unsigned int>> m_CrystalStructuresPtr;
  unsigned int* m_CrystalStructures = nullptr;
  std::weak_ptr<DataArray<PhaseType::EnumType>> m_PhaseTypesPtr;
  PhaseType::EnumType* m_PhaseTypes = nullptr;

  DataArrayPath m_CellEnsembleAttributeMatrixPath = {};
  QString m_PhaseTypesArrayName = {};
  PhaseType::Types m_PhaseTypeArray = {};
  PhaseType::Types m_PhaseTypeData = {};
  DataArrayPath m_NeighborListArrayPath = {};
  DataArrayPath m_SharedSurfaceAreaListArrayPath = {};
  DataArrayPath m_FeaturePhasesArrayPath = {};
  DataArrayPath m_BiasedFeaturesArrayPath = {};
  DataArrayPath m_EquivalentDiametersArrayPath = {};
  DataArrayPath m_NeighborhoodsArrayPath = {};
  DataArrayPath m_AspectRatiosArrayPath = {};
  DataArrayPath m_Omega3sArrayPath = {};
  DataArrayPath m_AxisEulerAnglesArrayPath = {};
  DataArrayPath m_CrystalStructuresArrayPath = {};
  DataArrayPath m_SurfaceFeaturesArrayPath = {};
  DataArrayPath m_VolumesArrayPath = {};
  DataArrayPath m_RDFArrayPath = {};
  DataArrayPath m_MaxMinRDFArrayPath = {};
  DataArrayPath m_FeatureEulerAnglesArrayPath = {};
  DataArrayPath m_AvgQuatsArrayPath = {};
  QString m_StatisticsArrayName = {};
  bool m_IncludeRadialDistFunc = {};
  bool m_CalculateMorphologicalStats = {};
  bool m_ComputeSizeDistribution = {};
  int m_SizeDistributionFitType = {};
  bool m_ComputeAspectRatioDistribution = {};
  int m_AspectRatioDistributionFitType = {};
  bool m_ComputeOmega3Distribution = {};
  int m_Omega3DistributionFitType = {};
  bool m_ComputeNeighborhoodDistribution = {};
  int m_NeighborhoodDistributionFitType = {};
  bool m_CalculateCrystallographicStats = {};
  bool m_CalculateODF = {};
  bool m_CalculateMDF = {};
  bool m_CalculateAxisODF = {};
  float m_SizeCorrelationResolution = {};


  NeighborList<int32_t>::WeakPointer m_NeighborList;
  NeighborList<float>::WeakPointer m_SharedSurfaceAreaList;

  StatsDataArray::Pointer m_StatsDataArray;

  QVector<DistributionAnalysisOps::Pointer> m_DistributionAnalysis;

public:
  GenerateEnsembleStatistics(const GenerateEnsembleStatistics&) = delete; // Copy Constructor Not Implemented
  GenerateEnsembleStatistics(GenerateEnsembleStatistics&&) = delete;      // Move Constructor Not Implemented
  GenerateEnsembleStatistics& operator=(const GenerateEnsembleStatistics&) = delete; // Copy Assignment Not Implemented
  GenerateEnsembleStatistics& operator=(GenerateEnsembleStatistics&&) = delete;      // Move Assignment Not Implemented
};

