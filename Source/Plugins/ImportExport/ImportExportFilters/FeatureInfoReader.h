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

#include "SIMPLib/CoreFilters/FileReader.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "ImportExport/ImportExportDLLExport.h"

/**
 * @brief The FeatureInfoReader class. See [Filter documentation](@ref featureinforeader) for details.
 */
class ImportExport_EXPORT FeatureInfoReader : public FileReader
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(FeatureInfoReader SUPERCLASS FileReader)
  PYB11_SHARED_POINTERS(FeatureInfoReader)
  PYB11_FILTER_NEW_MACRO(FeatureInfoReader)
  PYB11_FILTER_PARAMETER(DataArrayPath, CellAttributeMatrixName)
  PYB11_FILTER_PARAMETER(QString, CellFeatureAttributeMatrixName)
  PYB11_FILTER_PARAMETER(QString, InputFile)
  PYB11_FILTER_PARAMETER(bool, CreateCellLevelArrays)
  PYB11_FILTER_PARAMETER(bool, RenumberFeatures)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  PYB11_FILTER_PARAMETER(QString, CellPhasesArrayName)
  PYB11_FILTER_PARAMETER(QString, CellEulerAnglesArrayName)
  PYB11_FILTER_PARAMETER(QString, FeaturePhasesArrayName)
  PYB11_FILTER_PARAMETER(QString, FeatureEulerAnglesArrayName)
  PYB11_FILTER_PARAMETER(int, Delimiter)
  PYB11_PROPERTY(DataArrayPath CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)
  PYB11_PROPERTY(QString CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)
  PYB11_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)
  PYB11_PROPERTY(bool CreateCellLevelArrays READ getCreateCellLevelArrays WRITE setCreateCellLevelArrays)
  PYB11_PROPERTY(bool RenumberFeatures READ getRenumberFeatures WRITE setRenumberFeatures)
  PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
  PYB11_PROPERTY(QString CellPhasesArrayName READ getCellPhasesArrayName WRITE setCellPhasesArrayName)
  PYB11_PROPERTY(QString CellEulerAnglesArrayName READ getCellEulerAnglesArrayName WRITE setCellEulerAnglesArrayName)
  PYB11_PROPERTY(QString FeaturePhasesArrayName READ getFeaturePhasesArrayName WRITE setFeaturePhasesArrayName)
  PYB11_PROPERTY(QString FeatureEulerAnglesArrayName READ getFeatureEulerAnglesArrayName WRITE setFeatureEulerAnglesArrayName)
#endif

public:
  using Self = FeatureInfoReader;
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
   * @brief Returns the name of the class for FeatureInfoReader
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FeatureInfoReader
   */
  static QString ClassName();

  ~FeatureInfoReader() override;

  /**
   * @brief Setter property for CellAttributeMatrixName
   */
  void setCellAttributeMatrixName(const DataArrayPath& value);
  /**
   * @brief Getter property for CellAttributeMatrixName
   * @return Value of CellAttributeMatrixName
   */
  DataArrayPath getCellAttributeMatrixName() const;

  Q_PROPERTY(DataArrayPath CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

  /**
   * @brief Setter property for CellFeatureAttributeMatrixName
   */
  void setCellFeatureAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for CellFeatureAttributeMatrixName
   * @return Value of CellFeatureAttributeMatrixName
   */
  QString getCellFeatureAttributeMatrixName() const;

  Q_PROPERTY(QString CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)

  /* Input Parameters */
  /**
   * @brief Setter property for InputFile
   */
  void setInputFile(const QString& value);
  /**
   * @brief Getter property for InputFile
   * @return Value of InputFile
   */
  QString getInputFile() const;

  Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

  /**
   * @brief Setter property for CreateCellLevelArrays
   */
  void setCreateCellLevelArrays(bool value);
  /**
   * @brief Getter property for CreateCellLevelArrays
   * @return Value of CreateCellLevelArrays
   */
  bool getCreateCellLevelArrays() const;

  Q_PROPERTY(bool CreateCellLevelArrays READ getCreateCellLevelArrays WRITE setCreateCellLevelArrays)

  /**
   * @brief Setter property for RenumberFeatures
   */
  void setRenumberFeatures(bool value);
  /**
   * @brief Getter property for RenumberFeatures
   * @return Value of RenumberFeatures
   */
  bool getRenumberFeatures() const;

  Q_PROPERTY(bool RenumberFeatures READ getRenumberFeatures WRITE setRenumberFeatures)

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
   * @brief Setter property for CellPhasesArrayName
   */
  void setCellPhasesArrayName(const QString& value);
  /**
   * @brief Getter property for CellPhasesArrayName
   * @return Value of CellPhasesArrayName
   */
  QString getCellPhasesArrayName() const;

  Q_PROPERTY(QString CellPhasesArrayName READ getCellPhasesArrayName WRITE setCellPhasesArrayName)

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
   * @brief Setter property for FeaturePhasesArrayName
   */
  void setFeaturePhasesArrayName(const QString& value);
  /**
   * @brief Getter property for FeaturePhasesArrayName
   * @return Value of FeaturePhasesArrayName
   */
  QString getFeaturePhasesArrayName() const;

  Q_PROPERTY(QString FeaturePhasesArrayName READ getFeaturePhasesArrayName WRITE setFeaturePhasesArrayName)

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
   * @brief preflight Reimplemented from @see AbstractFilter class
   */
  void preflight() override;

protected:
  FeatureInfoReader();

  /**
   * @brief readHeader Reimplemented from @see FileReader class
   */
  int32_t readHeader() override;

  /**
   * @brief readFile Reimplemented from @see FileReader class
   */
  int32_t readFile() override;

  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck() override;

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief updateFeatureInstancePointers Updates raw feature pointers
   */
  void updateFeatureInstancePointers();

private:
  std::weak_ptr<DataArray<int32_t>> m_FeatureIdsPtr;
  int32_t* m_FeatureIds = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_CellPhasesPtr;
  int32_t* m_CellPhases = nullptr;
  std::weak_ptr<DataArray<float>> m_CellEulerAnglesPtr;
  float* m_CellEulerAngles = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_FeaturePhasesPtr;
  int32_t* m_FeaturePhases = nullptr;
  std::weak_ptr<DataArray<float>> m_FeatureEulerAnglesPtr;
  float* m_FeatureEulerAngles = nullptr;

  DataArrayPath m_CellAttributeMatrixName = {};
  QString m_CellFeatureAttributeMatrixName = {};
  QString m_InputFile = {};
  bool m_CreateCellLevelArrays = {};
  bool m_RenumberFeatures = {};
  DataArrayPath m_FeatureIdsArrayPath = {};
  QString m_CellPhasesArrayName = {};
  QString m_CellEulerAnglesArrayName = {};
  QString m_FeaturePhasesArrayName = {};
  QString m_FeatureEulerAnglesArrayName = {};
  int m_Delimiter = {};

public:
  FeatureInfoReader(const FeatureInfoReader&) = delete;            // Copy Constructor Not Implemented
  FeatureInfoReader(FeatureInfoReader&&) = delete;                 // Move Constructor Not Implemented
  FeatureInfoReader& operator=(const FeatureInfoReader&) = delete; // Copy Assignment Not Implemented
  FeatureInfoReader& operator=(FeatureInfoReader&&) = delete;      // Move Assignment Not Implemented
};
