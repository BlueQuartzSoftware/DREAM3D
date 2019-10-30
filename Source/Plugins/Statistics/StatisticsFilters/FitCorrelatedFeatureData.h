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

#include <QtCore/QString>
#include <set>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "Statistics/DistributionAnalysisOps/DistributionAnalysisOps.h"

#include "Statistics/StatisticsConstants.h"
#include "Statistics/StatisticsVersion.h"

#include "Statistics/StatisticsDLLExport.h"

/**
 * @class FitCorrelatedFeatureData FitCorrelatedFeatureData.h /FilterCategoryFilters/FitCorrelatedFeatureData.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class Statistics_EXPORT FitCorrelatedFeatureData : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(FitCorrelatedFeatureData SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(FitCorrelatedFeatureData)
  PYB11_FILTER_NEW_MACRO(FitCorrelatedFeatureData)
  PYB11_FILTER_PARAMETER(DataArrayPath, SelectedFeatureArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CorrelatedFeatureArrayPath)
  PYB11_FILTER_PARAMETER(unsigned int, DistributionType)
  PYB11_FILTER_PARAMETER(int, NumberOfCorrelatedBins)
  PYB11_FILTER_PARAMETER(bool, RemoveBiasedFeatures)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, BiasedFeaturesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, NewEnsembleArrayArrayPath)
  PYB11_PROPERTY(DataArrayPath SelectedFeatureArrayPath READ getSelectedFeatureArrayPath WRITE setSelectedFeatureArrayPath)
  PYB11_PROPERTY(DataArrayPath CorrelatedFeatureArrayPath READ getCorrelatedFeatureArrayPath WRITE setCorrelatedFeatureArrayPath)
  PYB11_PROPERTY(unsigned int DistributionType READ getDistributionType WRITE setDistributionType)
  PYB11_PROPERTY(int NumberOfCorrelatedBins READ getNumberOfCorrelatedBins WRITE setNumberOfCorrelatedBins)
  PYB11_PROPERTY(bool RemoveBiasedFeatures READ getRemoveBiasedFeatures WRITE setRemoveBiasedFeatures)
  PYB11_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath BiasedFeaturesArrayPath READ getBiasedFeaturesArrayPath WRITE setBiasedFeaturesArrayPath)
  PYB11_PROPERTY(DataArrayPath NewEnsembleArrayArrayPath READ getNewEnsembleArrayArrayPath WRITE setNewEnsembleArrayArrayPath)
#endif

public:
  using Self = FitCorrelatedFeatureData;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<Self>;
  
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
   * @brief Returns the name of the class for FitCorrelatedFeatureData
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FitCorrelatedFeatureData
   */
  static QString ClassName();

  ~FitCorrelatedFeatureData() override;

  /**
   * @brief Setter property for SelectedFeatureArrayPath
   */
  void setSelectedFeatureArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SelectedFeatureArrayPath
   * @return Value of SelectedFeatureArrayPath
   */
  DataArrayPath getSelectedFeatureArrayPath() const;

  Q_PROPERTY(DataArrayPath SelectedFeatureArrayPath READ getSelectedFeatureArrayPath WRITE setSelectedFeatureArrayPath)
  /**
   * @brief Setter property for CorrelatedFeatureArrayPath
   */
  void setCorrelatedFeatureArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CorrelatedFeatureArrayPath
   * @return Value of CorrelatedFeatureArrayPath
   */
  DataArrayPath getCorrelatedFeatureArrayPath() const;

  Q_PROPERTY(DataArrayPath CorrelatedFeatureArrayPath READ getCorrelatedFeatureArrayPath WRITE setCorrelatedFeatureArrayPath)

  /**
   * @brief Setter property for DistributionType
   */
  void setDistributionType(unsigned int value);
  /**
   * @brief Getter property for DistributionType
   * @return Value of DistributionType
   */
  unsigned int getDistributionType() const;

  Q_PROPERTY(unsigned int DistributionType READ getDistributionType WRITE setDistributionType)
  /**
   * @brief Setter property for NumberOfCorrelatedBins
   */
  void setNumberOfCorrelatedBins(int value);
  /**
   * @brief Getter property for NumberOfCorrelatedBins
   * @return Value of NumberOfCorrelatedBins
   */
  int getNumberOfCorrelatedBins() const;

  Q_PROPERTY(int NumberOfCorrelatedBins READ getNumberOfCorrelatedBins WRITE setNumberOfCorrelatedBins)
  /**
   * @brief Setter property for RemoveBiasedFeatures
   */
  void setRemoveBiasedFeatures(bool value);
  /**
   * @brief Getter property for RemoveBiasedFeatures
   * @return Value of RemoveBiasedFeatures
   */
  bool getRemoveBiasedFeatures() const;

  Q_PROPERTY(bool RemoveBiasedFeatures READ getRemoveBiasedFeatures WRITE setRemoveBiasedFeatures)

  /**
  * @brief This returns the group that the filter belonds to. You can select
  * a different group if you want. The string returned here will be displayed
  * in the GUI for the filter
  */
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
   * @brief Setter property for NewEnsembleArrayArrayPath
   */
  void setNewEnsembleArrayArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for NewEnsembleArrayArrayPath
   * @return Value of NewEnsembleArrayArrayPath
   */
  DataArrayPath getNewEnsembleArrayArrayPath() const;

  Q_PROPERTY(DataArrayPath NewEnsembleArrayArrayPath READ getNewEnsembleArrayArrayPath WRITE setNewEnsembleArrayArrayPath)

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
  FitCorrelatedFeatureData();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  std::weak_ptr<DataArray<bool>> m_BiasedFeaturesPtr;
  bool* m_BiasedFeatures = nullptr;
  std::weak_ptr<DataArray<float>> m_NewEnsembleArrayPtr;
  float* m_NewEnsembleArray = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_FeaturePhasesPtr;
  int32_t* m_FeaturePhases = nullptr;

  DataArrayPath m_SelectedFeatureArrayPath = {};
  DataArrayPath m_CorrelatedFeatureArrayPath = {};
  unsigned int m_DistributionType = {};
  int m_NumberOfCorrelatedBins = {};
  bool m_RemoveBiasedFeatures = {};
  DataArrayPath m_FeaturePhasesArrayPath = {};
  DataArrayPath m_BiasedFeaturesArrayPath = {};
  DataArrayPath m_NewEnsembleArrayArrayPath = {};

public:
  FitCorrelatedFeatureData(const FitCorrelatedFeatureData&) = delete; // Copy Constructor Not Implemented
  FitCorrelatedFeatureData(FitCorrelatedFeatureData&&) = delete;      // Move Constructor Not Implemented
  FitCorrelatedFeatureData& operator=(const FitCorrelatedFeatureData&) = delete; // Copy Assignment Not Implemented
  FitCorrelatedFeatureData& operator=(FitCorrelatedFeatureData&&) = delete;      // Move Assignment Not Implemented
};

