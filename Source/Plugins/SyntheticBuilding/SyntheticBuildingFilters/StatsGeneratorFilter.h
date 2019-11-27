
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

#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "SyntheticBuilding/SyntheticBuildingDLLExport.h"

/**
 * @brief The StatsGeneratorFilter class. See [Filter documentation](@ref statsgeneratorfilter) for details.
 */
class SyntheticBuilding_EXPORT StatsGeneratorFilter : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(StatsGeneratorFilter SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(StatsGeneratorFilter)
  PYB11_FILTER_NEW_MACRO(StatsGeneratorFilter)
  PYB11_FILTER_PARAMETER(DataArrayPath, StatsGeneratorDataContainerName)
  PYB11_FILTER_PARAMETER(QString, CellEnsembleAttributeMatrixName)
  PYB11_FILTER_PARAMETER(QString, StatsDataArrayName)
  PYB11_FILTER_PARAMETER(QString, CrystalStructuresArrayName)
  PYB11_FILTER_PARAMETER(QString, PhaseTypesArrayName)
  PYB11_FILTER_PARAMETER(QString, PhaseNamesArrayName)
  PYB11_FILTER_PARAMETER(StatsDataArray::Pointer, StatsDataArray)
  PYB11_FILTER_PARAMETER(UInt32ArrayType::Pointer, CrystalStructures)
  PYB11_FILTER_PARAMETER(UInt32ArrayType::Pointer, PhaseTypes)
  PYB11_FILTER_PARAMETER(StringDataArray::Pointer, PhaseNames)
  PYB11_FILTER_PARAMETER(DataArrayPath, Path)
  PYB11_PROPERTY(DataArrayPath StatsGeneratorDataContainerName READ getStatsGeneratorDataContainerName WRITE setStatsGeneratorDataContainerName)
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
#endif

public:
  using Self = StatsGeneratorFilter;
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
   * @brief Returns the name of the class for StatsGeneratorFilter
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for StatsGeneratorFilter
   */
  static QString ClassName();

  ~StatsGeneratorFilter() override;

  /**
   * @brief Setter property for StatsGeneratorDataContainerName
   */
  void setStatsGeneratorDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for StatsGeneratorDataContainerName
   * @return Value of StatsGeneratorDataContainerName
   */
  DataArrayPath getStatsGeneratorDataContainerName() const;

  Q_PROPERTY(DataArrayPath StatsGeneratorDataContainerName READ getStatsGeneratorDataContainerName WRITE setStatsGeneratorDataContainerName)

  /**
   * @brief Setter property for CellEnsembleAttributeMatrixName
   */
  void setCellEnsembleAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for CellEnsembleAttributeMatrixName
   * @return Value of CellEnsembleAttributeMatrixName
   */
  QString getCellEnsembleAttributeMatrixName() const;

  Q_PROPERTY(QString CellEnsembleAttributeMatrixName READ getCellEnsembleAttributeMatrixName WRITE setCellEnsembleAttributeMatrixName)

  /**
   * @brief Setter property for StatsDataArrayName
   */
  void setStatsDataArrayName(const QString& value);
  /**
   * @brief Getter property for StatsDataArrayName
   * @return Value of StatsDataArrayName
   */
  QString getStatsDataArrayName() const;

  Q_PROPERTY(QString StatsDataArrayName READ getStatsDataArrayName WRITE setStatsDataArrayName)

  /**
   * @brief Setter property for CrystalStructuresArrayName
   */
  void setCrystalStructuresArrayName(const QString& value);
  /**
   * @brief Getter property for CrystalStructuresArrayName
   * @return Value of CrystalStructuresArrayName
   */
  QString getCrystalStructuresArrayName() const;

  Q_PROPERTY(QString CrystalStructuresArrayName READ getCrystalStructuresArrayName WRITE setCrystalStructuresArrayName)

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
   * @brief Setter property for PhaseNamesArrayName
   */
  void setPhaseNamesArrayName(const QString& value);
  /**
   * @brief Getter property for PhaseNamesArrayName
   * @return Value of PhaseNamesArrayName
   */
  QString getPhaseNamesArrayName() const;

  Q_PROPERTY(QString PhaseNamesArrayName READ getPhaseNamesArrayName WRITE setPhaseNamesArrayName)

  /**
   * @brief Setter property for StatsDataArray
   */
  void setStatsDataArray(const StatsDataArray::Pointer& value);
  /**
   * @brief Getter property for StatsDataArray
   * @return Value of StatsDataArray
   */
  StatsDataArray::Pointer getStatsDataArray() const;

  Q_PROPERTY(StatsDataArray::Pointer StatsDataArray READ getStatsDataArray WRITE setStatsDataArray)

  /**
   * @brief Setter property for CrystalStructures
   */
  void setCrystalStructures(const UInt32ArrayType::Pointer& value);
  /**
   * @brief Getter property for CrystalStructures
   * @return Value of CrystalStructures
   */
  UInt32ArrayType::Pointer getCrystalStructures() const;

  Q_PROPERTY(UInt32ArrayType::Pointer CrystalStructures READ getCrystalStructures WRITE setCrystalStructures)

  /**
   * @brief Setter property for PhaseTypes
   */
  void setPhaseTypes(const UInt32ArrayType::Pointer& value);
  /**
   * @brief Getter property for PhaseTypes
   * @return Value of PhaseTypes
   */
  UInt32ArrayType::Pointer getPhaseTypes() const;

  Q_PROPERTY(UInt32ArrayType::Pointer PhaseTypes READ getPhaseTypes WRITE setPhaseTypes)

  /**
   * @brief Setter property for PhaseNames
   */
  void setPhaseNames(const StringDataArray::Pointer& value);
  /**
   * @brief Getter property for PhaseNames
   * @return Value of PhaseNames
   */
  StringDataArray::Pointer getPhaseNames() const;

  Q_PROPERTY(StringDataArray::Pointer PhaseNames READ getPhaseNames WRITE setPhaseNames)

  /**
   * @brief Setter property for Path
   */
  void setPath(const DataArrayPath& value);
  /**
   * @brief Getter property for Path
   * @return Value of Path
   */
  DataArrayPath getPath() const;

  Q_PROPERTY(DataArrayPath Path READ getPath WRITE setPath)

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

private:
  DataArrayPath m_StatsGeneratorDataContainerName = {};
  QString m_CellEnsembleAttributeMatrixName = {};
  QString m_StatsDataArrayName = {};
  QString m_CrystalStructuresArrayName = {};
  QString m_PhaseTypesArrayName = {};
  QString m_PhaseNamesArrayName = {};
  StatsDataArray::Pointer m_StatsDataArray = {};
  UInt32ArrayType::Pointer m_CrystalStructures = {};
  UInt32ArrayType::Pointer m_PhaseTypes = {};
  StringDataArray::Pointer m_PhaseNames = {};
  DataArrayPath m_Path = {};
};

