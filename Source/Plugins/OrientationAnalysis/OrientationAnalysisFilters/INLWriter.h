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
#include "SIMPLib/CoreFilters/FileWriter.h"
#include "SIMPLib/DataArrays/StringDataArray.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @brief The INLWriter class. See [Filter documentation](@ref inlwriter) for details.
 */
class OrientationAnalysis_EXPORT INLWriter : public FileWriter
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(INLWriter SUPERCLASS FileWriter)
	PYB11_PROPERTY(QString OutputFile READ getOutputFile WRITE setOutputFile)
    PYB11_PROPERTY(DataArrayPath MaterialNameArrayPath READ getMaterialNameArrayPath WRITE setMaterialNameArrayPath)
    PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
    PYB11_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)
    PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
    PYB11_PROPERTY(DataArrayPath NumFeaturesArrayPath READ getNumFeaturesArrayPath WRITE setNumFeaturesArrayPath)
    PYB11_PROPERTY(DataArrayPath CellEulerAnglesArrayPath READ getCellEulerAnglesArrayPath WRITE setCellEulerAnglesArrayPath)
public:
  SIMPL_SHARED_POINTERS(INLWriter)
  SIMPL_FILTER_NEW_MACRO(INLWriter)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(INLWriter, FileWriter)

  virtual ~INLWriter();

  SIMPL_FILTER_PARAMETER(DataArrayPath, MaterialNameArrayPath)
  Q_PROPERTY(DataArrayPath MaterialNameArrayPath READ getMaterialNameArrayPath WRITE setMaterialNameArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
  Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, NumFeaturesArrayPath)
  Q_PROPERTY(DataArrayPath NumFeaturesArrayPath READ getNumFeaturesArrayPath WRITE setNumFeaturesArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, CellEulerAnglesArrayPath)
  Q_PROPERTY(DataArrayPath CellEulerAnglesArrayPath READ getCellEulerAnglesArrayPath WRITE setCellEulerAnglesArrayPath)

  SIMPL_INSTANCE_STRING_PROPERTY(MaterialNameArrayName)

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
  INLWriter();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief writeHeader Reimplemented from @see FileReader class
   */
  virtual int32_t writeHeader();

  /**
   * @brief writeFile Reimplemented from @see FileReader class
   */
  virtual int32_t writeFile();

private:
  DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)
  DEFINE_DATAARRAY_VARIABLE(int32_t, CellPhases)
  DEFINE_DATAARRAY_VARIABLE(float, CellEulerAngles)
  DEFINE_DATAARRAY_VARIABLE(unsigned int, CrystalStructures)
  DEFINE_DATAARRAY_VARIABLE(int32_t, NumFeatures)

  StringDataArray::WeakPointer m_MaterialNamePtr;

public:
  INLWriter(const INLWriter&) = delete;            // Copy Constructor Not Implemented
  INLWriter(INLWriter&&) = delete;                 // Move Constructor Not Implemented
  INLWriter& operator=(const INLWriter&) = delete; // Copy Assignment Not Implemented
  INLWriter& operator=(INLWriter&&) = delete;      // Move Assignment Not Implemented
};

