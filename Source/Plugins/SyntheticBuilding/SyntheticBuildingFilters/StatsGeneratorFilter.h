
/* ============================================================================
 * Copyright (c) 2015 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <QtCore/QObject>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "SyntheticBuilding/SyntheticBuildingDLLExport.h"

/**
 * @brief The StatsGeneratorFilter class. See [Filter documentation](@ref statsgeneratorfilter) for details.
 */
class SyntheticBuilding_EXPORT StatsGeneratorFilter : public AbstractFilter
{
  Q_OBJECT
  PYB11_CREATE_BINDINGS(StatsGeneratorFilter SUPERCLASS AbstractFilter)
  PYB11_PROPERTY(QString StatsGeneratorDataContainerName READ getStatsGeneratorDataContainerName WRITE setStatsGeneratorDataContainerName)
  PYB11_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)
  PYB11_PROPERTY(QString StatsDataArrayName READ getStatsDataArrayName WRITE setStatsDataArrayName)
  PYB11_PROPERTY(QString CrystalStructuresArrayName READ getCrystalStructuresArrayName WRITE setCrystalStructuresArrayName)
  PYB11_PROPERTY(QString PhaseTypesArrayName READ getPhaseTypesArrayName WRITE setPhaseTypesArrayName)
  PYB11_PROPERTY(QString PhaseNamesArrayName READ getPhaseNamesArrayName WRITE setPhaseNamesArrayName)
  PYB11_PROPERTY(StatsDataArray::Pointer StatsDataArray READ getStatsDataArray WRITE setStatsDataArray)
  PYB11_PROPERTY(UInt32ArrayType::Pointer CrystalStructures READ getCrystalStructures WRITE setCrystalStructures)
  PYB11_PROPERTY(UInt32ArrayType::Pointer PhaseTypes READ getPhaseTypes WRITE setPhaseTypes)
  PYB11_PROPERTY(StringDataArray::Pointer PhaseNames READ getPhaseNames WRITE setPhaseNames)
  PYB11_PROPERTY(DataArrayPath Path READ getPath WRITE setPath)

public:
  SIMPL_SHARED_POINTERS(StatsGeneratorFilter)
  SIMPL_FILTER_NEW_MACRO(StatsGeneratorFilter)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(StatsGeneratorFilter, AbstractFilter)

  ~StatsGeneratorFilter() override;

  SIMPL_FILTER_PARAMETER(QString, StatsGeneratorDataContainerName)
  Q_PROPERTY(QString StatsGeneratorDataContainerName READ getStatsGeneratorDataContainerName WRITE setStatsGeneratorDataContainerName)

  SIMPL_FILTER_PARAMETER(QString, CellEnsembleAttributeMatrixName)
  Q_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)

  SIMPL_FILTER_PARAMETER(QString, StatsDataArrayName)
  Q_PROPERTY(QString StatsDataArrayName READ getStatsDataArrayName WRITE setStatsDataArrayName)

  SIMPL_FILTER_PARAMETER(QString, CrystalStructuresArrayName)
  Q_PROPERTY(QString CrystalStructuresArrayName READ getCrystalStructuresArrayName WRITE setCrystalStructuresArrayName)

  SIMPL_FILTER_PARAMETER(QString, PhaseTypesArrayName)
  Q_PROPERTY(QString PhaseTypesArrayName READ getPhaseTypesArrayName WRITE setPhaseTypesArrayName)

  SIMPL_FILTER_PARAMETER(QString, PhaseNamesArrayName)
  Q_PROPERTY(QString PhaseNamesArrayName READ getPhaseNamesArrayName WRITE setPhaseNamesArrayName)

  SIMPL_FILTER_PARAMETER(StatsDataArray::Pointer, StatsDataArray)
  Q_PROPERTY(StatsDataArray::Pointer StatsDataArray READ getStatsDataArray WRITE setStatsDataArray)

  SIMPL_FILTER_PARAMETER(UInt32ArrayType::Pointer, CrystalStructures)
  Q_PROPERTY(UInt32ArrayType::Pointer CrystalStructures READ getCrystalStructures WRITE setCrystalStructures)

  SIMPL_FILTER_PARAMETER(UInt32ArrayType::Pointer, PhaseTypes)
  Q_PROPERTY(UInt32ArrayType::Pointer PhaseTypes READ getPhaseTypes WRITE setPhaseTypes)

  SIMPL_FILTER_PARAMETER(StringDataArray::Pointer, PhaseNames)
  Q_PROPERTY(StringDataArray::Pointer PhaseNames READ getPhaseNames WRITE setPhaseNames)

  SIMPL_FILTER_PARAMETER(DataArrayPath, Path)
  Q_PROPERTY(DataArrayPath Path READ getPath WRITE setPath)

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
   * @brief readFilterParameters
   * @param obj
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
  StatsGeneratorFilter();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief readArray
   * @param arrayName
   * @param jsonKey
   * @return
   */
  void readArray(const QJsonObject& jsonRoot, size_t numTuples);

public:
  StatsGeneratorFilter(const StatsGeneratorFilter&) = delete;            // Copy Constructor Not Implemented
  StatsGeneratorFilter(StatsGeneratorFilter&&) = delete;                 // Move Constructor Not Implemented
  StatsGeneratorFilter& operator=(const StatsGeneratorFilter&) = delete; // Copy Assignment Not Implemented
  StatsGeneratorFilter& operator=(StatsGeneratorFilter&&) = delete;      // Move Assignment Not Implemented
};

