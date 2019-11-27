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
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "Statistics/StatisticsDLLExport.h"

/**
 * @brief The FindFeatureClustering class. See [Filter documentation](@ref findfeatureclustering) for details.
 */
class Statistics_EXPORT FindFeatureClustering : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(FindFeatureClustering SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(FindFeatureClustering)
  PYB11_FILTER_NEW_MACRO(FindFeatureClustering)
  PYB11_FILTER_PARAMETER(int, NumberOfBins)
  PYB11_FILTER_PARAMETER(int, PhaseNumber)
  PYB11_FILTER_PARAMETER(DataArrayPath, CellEnsembleAttributeMatrixName)
  PYB11_FILTER_PARAMETER(bool, RemoveBiasedFeatures)
  PYB11_FILTER_PARAMETER(DataArrayPath, BiasedFeaturesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, EquivalentDiametersArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CentroidsArrayPath)
  PYB11_FILTER_PARAMETER(QString, ClusteringListArrayName)
  PYB11_FILTER_PARAMETER(QString, NewEnsembleArrayArrayName)
  PYB11_FILTER_PARAMETER(QString, MaxMinArrayName)
  PYB11_PROPERTY(int NumberOfBins READ getNumberOfBins WRITE setNumberOfBins)
  PYB11_PROPERTY(int PhaseNumber READ getPhaseNumber WRITE setPhaseNumber)
  PYB11_PROPERTY(DataArrayPath CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)
  PYB11_PROPERTY(bool RemoveBiasedFeatures READ getRemoveBiasedFeatures WRITE setRemoveBiasedFeatures)
  PYB11_PROPERTY(DataArrayPath BiasedFeaturesArrayPath READ getBiasedFeaturesArrayPath WRITE setBiasedFeaturesArrayPath)
  PYB11_PROPERTY(DataArrayPath EquivalentDiametersArrayPath READ getEquivalentDiametersArrayPath WRITE setEquivalentDiametersArrayPath)
  PYB11_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath CentroidsArrayPath READ getCentroidsArrayPath WRITE setCentroidsArrayPath)
  PYB11_PROPERTY(QString ClusteringListArrayName READ getClusteringListArrayName WRITE setClusteringListArrayName)
  PYB11_PROPERTY(QString NewEnsembleArrayArrayName READ getNewEnsembleArrayArrayName WRITE setNewEnsembleArrayArrayName)
  PYB11_PROPERTY(QString MaxMinArrayName READ getMaxMinArrayName WRITE setMaxMinArrayName)
#endif

public:
  using Self = FindFeatureClustering;
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
   * @brief Returns the name of the class for FindFeatureClustering
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FindFeatureClustering
   */
  static QString ClassName();

  ~FindFeatureClustering() override;

  /**
   * @brief Setter property for ErrorOutputFile
   */
  void setErrorOutputFile(const QString& value);
  /**
   * @brief Getter property for ErrorOutputFile
   * @return Value of ErrorOutputFile
   */
  QString getErrorOutputFile() const;

  /**
   * @brief Setter property for NumberOfBins
   */
  void setNumberOfBins(int value);
  /**
   * @brief Getter property for NumberOfBins
   * @return Value of NumberOfBins
   */
  int getNumberOfBins() const;

  Q_PROPERTY(int NumberOfBins READ getNumberOfBins WRITE setNumberOfBins)

  /**
   * @brief Setter property for PhaseNumber
   */
  void setPhaseNumber(int value);
  /**
   * @brief Getter property for PhaseNumber
   * @return Value of PhaseNumber
   */
  int getPhaseNumber() const;

  Q_PROPERTY(int PhaseNumber READ getPhaseNumber WRITE setPhaseNumber)

  /**
   * @brief Setter property for CellEnsembleAttributeMatrixName
   */
  void setCellEnsembleAttributeMatrixName(const DataArrayPath& value);
  /**
   * @brief Getter property for CellEnsembleAttributeMatrixName
   * @return Value of CellEnsembleAttributeMatrixName
   */
  DataArrayPath getCellEnsembleAttributeMatrixName() const;

  Q_PROPERTY(DataArrayPath CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)

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
   * @brief Setter property for CentroidsArrayPath
   */
  void setCentroidsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CentroidsArrayPath
   * @return Value of CentroidsArrayPath
   */
  DataArrayPath getCentroidsArrayPath() const;

  Q_PROPERTY(DataArrayPath CentroidsArrayPath READ getCentroidsArrayPath WRITE setCentroidsArrayPath)

  /**
   * @brief Setter property for ClusteringListArrayName
   */
  void setClusteringListArrayName(const QString& value);
  /**
   * @brief Getter property for ClusteringListArrayName
   * @return Value of ClusteringListArrayName
   */
  QString getClusteringListArrayName() const;

  Q_PROPERTY(QString ClusteringListArrayName READ getClusteringListArrayName WRITE setClusteringListArrayName)

  /**
   * @brief Setter property for NewEnsembleArrayArrayName
   */
  void setNewEnsembleArrayArrayName(const QString& value);
  /**
   * @brief Getter property for NewEnsembleArrayArrayName
   * @return Value of NewEnsembleArrayArrayName
   */
  QString getNewEnsembleArrayArrayName() const;

  Q_PROPERTY(QString NewEnsembleArrayArrayName READ getNewEnsembleArrayArrayName WRITE setNewEnsembleArrayArrayName)

  /**
   * @brief Setter property for MaxMinArrayName
   */
  void setMaxMinArrayName(const QString& value);
  /**
   * @brief Getter property for MaxMinArrayName
   * @return Value of MaxMinArrayName
   */
  QString getMaxMinArrayName() const;

  Q_PROPERTY(QString MaxMinArrayName READ getMaxMinArrayName WRITE setMaxMinArrayName)

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
  FindFeatureClustering();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief find_clustering Determines the Feature clustering
   */
  void find_clustering();

private:
  std::weak_ptr<DataArray<int32_t>> m_FeaturePhasesPtr;
  int32_t* m_FeaturePhases = nullptr;
  std::weak_ptr<DataArray<float>> m_CentroidsPtr;
  float* m_Centroids = nullptr;
  std::weak_ptr<DataArray<float>> m_EquivalentDiametersPtr;
  float* m_EquivalentDiameters = nullptr;
  std::weak_ptr<DataArray<float>> m_NewEnsembleArrayPtr;
  float* m_NewEnsembleArray = nullptr;
  std::weak_ptr<DataArray<float>> m_MaxMinArrayPtr;
  float* m_MaxMinArray = nullptr;
  std::weak_ptr<DataArray<bool>> m_BiasedFeaturesPtr;
  bool* m_BiasedFeatures = nullptr;

  QString m_ErrorOutputFile = {};
  int m_NumberOfBins = {};
  int m_PhaseNumber = {};
  DataArrayPath m_CellEnsembleAttributeMatrixName = {};
  bool m_RemoveBiasedFeatures = {};
  DataArrayPath m_BiasedFeaturesArrayPath = {};
  DataArrayPath m_EquivalentDiametersArrayPath = {};
  DataArrayPath m_FeaturePhasesArrayPath = {};
  DataArrayPath m_CentroidsArrayPath = {};
  QString m_ClusteringListArrayName = {};
  QString m_NewEnsembleArrayArrayName = {};
  QString m_MaxMinArrayName = {};

  NeighborList<float>::WeakPointer m_ClusteringList;
  std::vector<float> m_RandomCentroids;

public:
  FindFeatureClustering(const FindFeatureClustering&) = delete; // Copy Constructor Not Implemented
  FindFeatureClustering(FindFeatureClustering&&) = delete;      // Move Constructor Not Implemented
  FindFeatureClustering& operator=(const FindFeatureClustering&) = delete; // Copy Assignment Not Implemented
  FindFeatureClustering& operator=(FindFeatureClustering&&) = delete;      // Move Assignment Not Implemented
};

