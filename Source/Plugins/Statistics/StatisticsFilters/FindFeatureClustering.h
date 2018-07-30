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

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "Statistics/StatisticsDLLExport.h"

/**
 * @brief The FindFeatureClustering class. See [Filter documentation](@ref findfeatureclustering) for details.
 */
class Statistics_EXPORT FindFeatureClustering : public AbstractFilter
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(FindFeatureClustering SUPERCLASS AbstractFilter)
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
public:
  SIMPL_SHARED_POINTERS(FindFeatureClustering)
  SIMPL_FILTER_NEW_MACRO(FindFeatureClustering)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(FindFeatureClustering, AbstractFilter)

  ~FindFeatureClustering() override;

  SIMPL_INSTANCE_STRING_PROPERTY(ErrorOutputFile)

  SIMPL_FILTER_PARAMETER(int, NumberOfBins)
  Q_PROPERTY(int NumberOfBins READ getNumberOfBins WRITE setNumberOfBins)

  SIMPL_FILTER_PARAMETER(int, PhaseNumber)
  Q_PROPERTY(int PhaseNumber READ getPhaseNumber WRITE setPhaseNumber)

  SIMPL_FILTER_PARAMETER(DataArrayPath, CellEnsembleAttributeMatrixName)
  Q_PROPERTY(DataArrayPath CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)

  SIMPL_FILTER_PARAMETER(bool, RemoveBiasedFeatures)
  Q_PROPERTY(bool RemoveBiasedFeatures READ getRemoveBiasedFeatures WRITE setRemoveBiasedFeatures)

  SIMPL_FILTER_PARAMETER(DataArrayPath, BiasedFeaturesArrayPath)
  Q_PROPERTY(DataArrayPath BiasedFeaturesArrayPath READ getBiasedFeaturesArrayPath WRITE setBiasedFeaturesArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, EquivalentDiametersArrayPath)
  Q_PROPERTY(DataArrayPath EquivalentDiametersArrayPath READ getEquivalentDiametersArrayPath WRITE setEquivalentDiametersArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
  Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, CentroidsArrayPath)
  Q_PROPERTY(DataArrayPath CentroidsArrayPath READ getCentroidsArrayPath WRITE setCentroidsArrayPath)

  SIMPL_FILTER_PARAMETER(QString, ClusteringListArrayName)
  Q_PROPERTY(QString ClusteringListArrayName READ getClusteringListArrayName WRITE setClusteringListArrayName)

  SIMPL_FILTER_PARAMETER(QString, NewEnsembleArrayArrayName)
  Q_PROPERTY(QString NewEnsembleArrayArrayName READ getNewEnsembleArrayArrayName WRITE setNewEnsembleArrayArrayName)

  SIMPL_FILTER_PARAMETER(QString, MaxMinArrayName)
  Q_PROPERTY(QString MaxMinArrayName READ getMaxMinArrayName WRITE setMaxMinArrayName)

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
  DEFINE_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
  DEFINE_DATAARRAY_VARIABLE(float, Centroids)
  DEFINE_DATAARRAY_VARIABLE(float, EquivalentDiameters)
  DEFINE_DATAARRAY_VARIABLE(float, NewEnsembleArray)
  DEFINE_DATAARRAY_VARIABLE(float, MaxMinArray)
  DEFINE_DATAARRAY_VARIABLE(bool, BiasedFeatures)

  NeighborList<float>::WeakPointer m_ClusteringList;
  std::vector<float> m_RandomCentroids;

public:
  FindFeatureClustering(const FindFeatureClustering&) = delete; // Copy Constructor Not Implemented
  FindFeatureClustering(FindFeatureClustering&&) = delete;      // Move Constructor Not Implemented
  FindFeatureClustering& operator=(const FindFeatureClustering&) = delete; // Copy Assignment Not Implemented
  FindFeatureClustering& operator=(FindFeatureClustering&&) = delete;      // Move Assignment Not Implemented
};

