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

#include <QtCore/QTextStream>

#include "SIMPLib/SIMPLib.h"

#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @class WriteStatsGenOdfAngleFile WriteStatsGenOdfAngleFile.h /IOFilters/WriteStatsGenOdfAngleFile.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class OrientationAnalysis_EXPORT WriteStatsGenOdfAngleFile : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(WriteStatsGenOdfAngleFile SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(WriteStatsGenOdfAngleFile)
  PYB11_FILTER_NEW_MACRO(WriteStatsGenOdfAngleFile)
  PYB11_FILTER_PARAMETER(QString, OutputFile)
  PYB11_FILTER_PARAMETER(float, Weight)
  PYB11_FILTER_PARAMETER(int, Sigma)
  PYB11_FILTER_PARAMETER(int, Delimiter)
  PYB11_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CellEulerAnglesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, GoodVoxelsArrayPath)
  PYB11_FILTER_PARAMETER(bool, ConvertToDegrees)
  PYB11_FILTER_PARAMETER(bool, UseGoodVoxels)
  PYB11_PROPERTY(QString OutputFile READ getOutputFile WRITE setOutputFile)
  PYB11_PROPERTY(float Weight READ getWeight WRITE setWeight)
  PYB11_PROPERTY(int Sigma READ getSigma WRITE setSigma)
  PYB11_PROPERTY(int Delimiter READ getDelimiter WRITE setDelimiter)
  PYB11_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath CellEulerAnglesArrayPath READ getCellEulerAnglesArrayPath WRITE setCellEulerAnglesArrayPath)
  PYB11_PROPERTY(DataArrayPath GoodVoxelsArrayPath READ getGoodVoxelsArrayPath WRITE setGoodVoxelsArrayPath)
  PYB11_PROPERTY(bool ConvertToDegrees READ getConvertToDegrees WRITE setConvertToDegrees)
  PYB11_PROPERTY(bool UseGoodVoxels READ getUseGoodVoxels WRITE setUseGoodVoxels)
#endif

public:
  using Self = WriteStatsGenOdfAngleFile;
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
   * @brief Returns the name of the class for WriteStatsGenOdfAngleFile
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for WriteStatsGenOdfAngleFile
   */
  static QString ClassName();

  ~WriteStatsGenOdfAngleFile() override;

  /**
   * @brief Setter property for OutputFile
   */
  void setOutputFile(const QString& value);
  /**
   * @brief Getter property for OutputFile
   * @return Value of OutputFile
   */
  QString getOutputFile() const;

  Q_PROPERTY(QString OutputFile READ getOutputFile WRITE setOutputFile)

  /**
   * @brief Setter property for Weight
   */
  void setWeight(float value);
  /**
   * @brief Getter property for Weight
   * @return Value of Weight
   */
  float getWeight() const;

  Q_PROPERTY(float Weight READ getWeight WRITE setWeight)

  /**
   * @brief Setter property for Sigma
   */
  void setSigma(int value);
  /**
   * @brief Getter property for Sigma
   * @return Value of Sigma
   */
  int getSigma() const;

  Q_PROPERTY(int Sigma READ getSigma WRITE setSigma)

  /**
   * @brief Setter property for Delimiter
   */
  void setDelimiter(int value);
  /**
   * @brief Getter property for Delimiter
   * @return Value of Delimiter
   */
  int getDelimiter() const;

  Q_PROPERTY(int Delimiter READ getDelimiter WRITE setDelimiter)

  /**
   * @brief Setter property for CellPhasesArrayPath
   */
  void setCellPhasesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CellPhasesArrayPath
   * @return Value of CellPhasesArrayPath
   */
  DataArrayPath getCellPhasesArrayPath() const;

  Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

  /**
   * @brief Setter property for CellEulerAnglesArrayPath
   */
  void setCellEulerAnglesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CellEulerAnglesArrayPath
   * @return Value of CellEulerAnglesArrayPath
   */
  DataArrayPath getCellEulerAnglesArrayPath() const;

  Q_PROPERTY(DataArrayPath CellEulerAnglesArrayPath READ getCellEulerAnglesArrayPath WRITE setCellEulerAnglesArrayPath)

  /**
   * @brief Setter property for GoodVoxelsArrayPath
   */
  void setGoodVoxelsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for GoodVoxelsArrayPath
   * @return Value of GoodVoxelsArrayPath
   */
  DataArrayPath getGoodVoxelsArrayPath() const;

  Q_PROPERTY(DataArrayPath GoodVoxelsArrayPath READ getGoodVoxelsArrayPath WRITE setGoodVoxelsArrayPath)

  /**
   * @brief Setter property for ConvertToDegrees
   */
  void setConvertToDegrees(bool value);
  /**
   * @brief Getter property for ConvertToDegrees
   * @return Value of ConvertToDegrees
   */
  bool getConvertToDegrees() const;

  Q_PROPERTY(bool ConvertToDegrees READ getConvertToDegrees WRITE setConvertToDegrees)

  /**
   * @brief Setter property for UseGoodVoxels
   */
  void setUseGoodVoxels(bool value);
  /**
   * @brief Getter property for UseGoodVoxels
   * @return Value of UseGoodVoxels
   */
  bool getUseGoodVoxels() const;

  Q_PROPERTY(bool UseGoodVoxels READ getUseGoodVoxels WRITE setUseGoodVoxels)

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
  WriteStatsGenOdfAngleFile();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  int determineOutputLineCount(int64_t totalPoints, int32_t phase);

  int writeOutputFile(QTextStream& out, int32_t lineCount, int64_t totalPoints, int32_t phase);

private:
  std::weak_ptr<DataArray<float>> m_CellEulerAnglesPtr;
  float* m_CellEulerAngles = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_CellPhasesPtr;
  int32_t* m_CellPhases = nullptr;
  std::weak_ptr<DataArray<uint32_t>> m_CrystalStructuresPtr;
  uint32_t* m_CrystalStructures = nullptr;
  std::weak_ptr<DataArray<bool>> m_GoodVoxelsPtr;
  bool* m_GoodVoxels = nullptr;

  QString m_OutputFile = {};
  float m_Weight = {};
  int m_Sigma = {};
  int m_Delimiter = {};
  DataArrayPath m_CellPhasesArrayPath = {};
  DataArrayPath m_CellEulerAnglesArrayPath = {};
  DataArrayPath m_GoodVoxelsArrayPath = {};
  bool m_ConvertToDegrees = {};
  bool m_UseGoodVoxels = {};

public:
  WriteStatsGenOdfAngleFile(const WriteStatsGenOdfAngleFile&) = delete; // Copy Constructor Not Implemented
  WriteStatsGenOdfAngleFile(WriteStatsGenOdfAngleFile&&) = delete;      // Move Constructor Not Implemented
  WriteStatsGenOdfAngleFile& operator=(const WriteStatsGenOdfAngleFile&) = delete; // Copy Assignment Not Implemented
  WriteStatsGenOdfAngleFile& operator=(WriteStatsGenOdfAngleFile&&) = delete;      // Move Assignment Not Implemented
};

