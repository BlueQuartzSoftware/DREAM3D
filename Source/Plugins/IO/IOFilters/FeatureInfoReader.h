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
#include "SIMPLib/CoreFilters/FileReader.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "IO/IODLLExport.h"

/**
 * @brief The FeatureInfoReader class. See [Filter documentation](@ref featureinforeader) for details.
 */
class IO_EXPORT FeatureInfoReader : public FileReader
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(FeatureInfoReader SUPERCLASS FileReader)
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
public:
  SIMPL_SHARED_POINTERS(FeatureInfoReader)
  SIMPL_FILTER_NEW_MACRO(FeatureInfoReader)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(FeatureInfoReader, FileReader)

  ~FeatureInfoReader() override;

  SIMPL_FILTER_PARAMETER(DataArrayPath, CellAttributeMatrixName)
  Q_PROPERTY(DataArrayPath CellAttributeMatrixName READ getCellAttributeMatrixName WRITE setCellAttributeMatrixName)

  SIMPL_FILTER_PARAMETER(QString, CellFeatureAttributeMatrixName)
  Q_PROPERTY(QString CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)

  /* Input Parameters */
  SIMPL_FILTER_PARAMETER(QString, InputFile)
  Q_PROPERTY(QString InputFile READ getInputFile WRITE setInputFile)

  SIMPL_FILTER_PARAMETER(bool, CreateCellLevelArrays)
  Q_PROPERTY(bool CreateCellLevelArrays READ getCreateCellLevelArrays WRITE setCreateCellLevelArrays)

  SIMPL_FILTER_PARAMETER(bool, RenumberFeatures)
  Q_PROPERTY(bool RenumberFeatures READ getRenumberFeatures WRITE setRenumberFeatures)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

  SIMPL_FILTER_PARAMETER(QString, CellPhasesArrayName)
  Q_PROPERTY(QString CellPhasesArrayName READ getCellPhasesArrayName WRITE setCellPhasesArrayName)

  SIMPL_FILTER_PARAMETER(QString, CellEulerAnglesArrayName)
  Q_PROPERTY(QString CellEulerAnglesArrayName READ getCellEulerAnglesArrayName WRITE setCellEulerAnglesArrayName)

  SIMPL_FILTER_PARAMETER(QString, FeaturePhasesArrayName)
  Q_PROPERTY(QString FeaturePhasesArrayName READ getFeaturePhasesArrayName WRITE setFeaturePhasesArrayName)

  SIMPL_FILTER_PARAMETER(QString, FeatureEulerAnglesArrayName)
  Q_PROPERTY(QString FeatureEulerAnglesArrayName READ getFeatureEulerAnglesArrayName WRITE setFeatureEulerAnglesArrayName)

  SIMPL_FILTER_PARAMETER(int, Delimiter)
  Q_PROPERTY(int Delimiter READ getDelimiter WRITE setDelimiter)

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
  DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)

  DEFINE_DATAARRAY_VARIABLE(int32_t, CellPhases)
  DEFINE_DATAARRAY_VARIABLE(float, CellEulerAngles)
  DEFINE_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
  DEFINE_DATAARRAY_VARIABLE(float, FeatureEulerAngles)

public:
  FeatureInfoReader(const FeatureInfoReader&) = delete; // Copy Constructor Not Implemented
  FeatureInfoReader(FeatureInfoReader&&) = delete;      // Move Constructor Not Implemented
  FeatureInfoReader& operator=(const FeatureInfoReader&) = delete; // Copy Assignment Not Implemented
  FeatureInfoReader& operator=(FeatureInfoReader&&) = delete;      // Move Assignment Not Implemented
};

