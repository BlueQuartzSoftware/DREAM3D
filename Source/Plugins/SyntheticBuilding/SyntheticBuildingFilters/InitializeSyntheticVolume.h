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
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SyntheticBuilding/SyntheticBuildingDLLExport.h"

/**
 * @brief The InitializeSyntheticVolume class. See [Filter documentation](@ref initializesyntheticvolume) for details.
 */
class SyntheticBuilding_EXPORT InitializeSyntheticVolume : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(InitializeSyntheticVolume SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(InitializeSyntheticVolume)
  PYB11_FILTER_NEW_MACRO(InitializeSyntheticVolume)
  PYB11_FILTER_PARAMETER(DataArrayPath, DataContainerName)
  PYB11_FILTER_PARAMETER(QString, CellAttributeMatrixName)
  PYB11_FILTER_PARAMETER(QString, EnsembleAttributeMatrixName)
  PYB11_FILTER_PARAMETER(int32_t, LengthUnit)
  PYB11_FILTER_PARAMETER(IntVec3Type, Dimensions)
  PYB11_FILTER_PARAMETER(FloatVec3Type, Spacing)
  PYB11_FILTER_PARAMETER(FloatVec3Type, Origin)
  PYB11_FILTER_PARAMETER(DataArrayPath, InputStatsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, InputPhaseTypesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, InputPhaseNamesArrayPath)
  PYB11_FILTER_PARAMETER(bool, EstimateNumberOfFeatures)
  PYB11_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)
  PYB11_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)
  PYB11_PROPERTY(QString EnsembleAttributeMatrixName READ getEnsembleAttributeMatrixName WRITE setEnsembleAttributeMatrixName)
  PYB11_PROPERTY(int32_t LengthUnit READ getLengthUnit WRITE setLengthUnit)
  PYB11_PROPERTY(IntVec3Type Dimensions READ getDimensions WRITE setDimensions)
  PYB11_PROPERTY(FloatVec3Type Spacing READ getSpacing WRITE setSpacing)
  PYB11_PROPERTY(FloatVec3Type Origin READ getOrigin WRITE setOrigin)
  PYB11_PROPERTY(DataArrayPath InputStatsArrayPath READ getInputStatsArrayPath WRITE setInputStatsArrayPath)
  PYB11_PROPERTY(DataArrayPath InputPhaseTypesArrayPath READ getInputPhaseTypesArrayPath WRITE setInputPhaseTypesArrayPath)
  PYB11_PROPERTY(DataArrayPath InputPhaseNamesArrayPath READ getInputPhaseNamesArrayPath WRITE setInputPhaseNamesArrayPath)
  PYB11_PROPERTY(bool EstimateNumberOfFeatures READ getEstimateNumberOfFeatures WRITE setEstimateNumberOfFeatures)
#endif

public:
  using Self = InitializeSyntheticVolume;
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
   * @brief Returns the name of the class for InitializeSyntheticVolume
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for InitializeSyntheticVolume
   */
  static QString ClassName();

  ~InitializeSyntheticVolume() override;

  /**
   * @brief Setter property for DataContainerName
   */
  void setDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for DataContainerName
   * @return Value of DataContainerName
   */
  DataArrayPath getDataContainerName() const;

  Q_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)

  /**
   * @brief Setter property for CellAttributeMatrixName
   */
  void setCellAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for CellAttributeMatrixName
   * @return Value of CellAttributeMatrixName
   */
  QString getCellAttributeMatrixName() const;

  Q_PROPERTY(QString CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

  /**
   * @brief Setter property for EnsembleAttributeMatrixName
   */
  void setEnsembleAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for EnsembleAttributeMatrixName
   * @return Value of EnsembleAttributeMatrixName
   */
  QString getEnsembleAttributeMatrixName() const;

  Q_PROPERTY(QString EnsembleAttributeMatrixName READ getEnsembleAttributeMatrixName WRITE setEnsembleAttributeMatrixName)

  /**
   * @brief Setter property for LengthUnit
   */
  void setLengthUnit(int32_t value);
  /**
   * @brief Getter property for LengthUnit
   * @return Value of LengthUnit
   */
  int32_t getLengthUnit() const;

  Q_PROPERTY(int32_t LengthUnit READ getLengthUnit WRITE setLengthUnit)

  /**
   * @brief Setter property for Dimensions
   */
  void setDimensions(const IntVec3Type& value);
  /**
   * @brief Getter property for Dimensions
   * @return Value of Dimensions
   */
  IntVec3Type getDimensions() const;

  Q_PROPERTY(IntVec3Type Dimensions READ getDimensions WRITE setDimensions)

  /**
   * @brief Setter property for Spacing
   */
  void setSpacing(const FloatVec3Type& value);
  /**
   * @brief Getter property for Spacing
   * @return Value of Spacing
   */
  FloatVec3Type getSpacing() const;

  Q_PROPERTY(FloatVec3Type Spacing READ getSpacing WRITE setSpacing)

  /**
   * @brief Setter property for Origin
   */
  void setOrigin(const FloatVec3Type& value);
  /**
   * @brief Getter property for Origin
   * @return Value of Origin
   */
  FloatVec3Type getOrigin() const;

  Q_PROPERTY(FloatVec3Type Origin READ getOrigin WRITE setOrigin)

  // These are for estimating the number of features that will be generated farther down the line.

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
   * @brief Setter property for InputPhaseTypesArrayPath
   */
  void setInputPhaseTypesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for InputPhaseTypesArrayPath
   * @return Value of InputPhaseTypesArrayPath
   */
  DataArrayPath getInputPhaseTypesArrayPath() const;

  Q_PROPERTY(DataArrayPath InputPhaseTypesArrayPath READ getInputPhaseTypesArrayPath WRITE setInputPhaseTypesArrayPath)

  /**
   * @brief Setter property for InputPhaseNamesArrayPath
   */
  void setInputPhaseNamesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for InputPhaseNamesArrayPath
   * @return Value of InputPhaseNamesArrayPath
   */
  DataArrayPath getInputPhaseNamesArrayPath() const;

  Q_PROPERTY(DataArrayPath InputPhaseNamesArrayPath READ getInputPhaseNamesArrayPath WRITE setInputPhaseNamesArrayPath)

  /**
   * @brief Setter property for EstimateNumberOfFeatures
   */
  void setEstimateNumberOfFeatures(bool value);
  /**
   * @brief Getter property for EstimateNumberOfFeatures
   * @return Value of EstimateNumberOfFeatures
   */
  bool getEstimateNumberOfFeatures() const;

  Q_PROPERTY(bool EstimateNumberOfFeatures READ getEstimateNumberOfFeatures WRITE setEstimateNumberOfFeatures)

  QString getEstimatedPrimaryFeatures();
  Q_PROPERTY(QString EstimatedPrimaryFeatures READ getEstimatedPrimaryFeatures)

  QString getBoxDimensions();
  Q_PROPERTY(QString BoxDimensions READ getBoxDimensions)

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
  InitializeSyntheticVolume();

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief estimateNumFeatures Estimates the number of Features that will be generated based on the supplied statistics
   * @param dimensions Vector containing extents of the volume dimensions
   * @param res Vector containing resolution of the volume
   * @return String Number of estimated features as a string
   */
  QString estimateNumFeatures(IntVec3Type dimensions, FloatVec3Type res);

private:
  DataArrayPath m_DataContainerName = {};
  QString m_CellAttributeMatrixName = {};
  QString m_EnsembleAttributeMatrixName = {};
  int32_t m_LengthUnit = {};
  IntVec3Type m_Dimensions = {};
  FloatVec3Type m_Spacing = {};
  FloatVec3Type m_Origin = {};
  DataArrayPath m_InputStatsArrayPath = {};
  DataArrayPath m_InputPhaseTypesArrayPath = {};
  DataArrayPath m_InputPhaseNamesArrayPath = {};
  bool m_EstimateNumberOfFeatures = {};

  QString m_EstimatedPrimaryFeatures;

public:
  InitializeSyntheticVolume(const InitializeSyntheticVolume&) = delete; // Copy Constructor Not Implemented
  InitializeSyntheticVolume(InitializeSyntheticVolume&&) = delete;      // Move Constructor Not Implemented
  InitializeSyntheticVolume& operator=(const InitializeSyntheticVolume&) = delete; // Copy Assignment Not Implemented
  InitializeSyntheticVolume& operator=(InitializeSyntheticVolume&&) = delete;      // Move Assignment Not Implemented
};

