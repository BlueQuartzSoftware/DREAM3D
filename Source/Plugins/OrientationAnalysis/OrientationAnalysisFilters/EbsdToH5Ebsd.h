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

// Needed for AxisAngle_t
#include "EbsdLib/EbsdConstants.h"

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/FilterParameters/AxisAngleInput.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @brief The EbsdToH5Ebsd class. See [Filter documentation](@ref ebsdtoh5ebsd) for details.
 */
class OrientationAnalysis_EXPORT EbsdToH5Ebsd : public AbstractFilter
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(EbsdToH5Ebsd SUPERCLASS AbstractFilter)
    PYB11_PROPERTY(QString InputPath READ getInputPath WRITE setInputPath)
    PYB11_PROPERTY(QString FilePrefix READ getFilePrefix WRITE setFilePrefix)
    PYB11_PROPERTY(QString FileSuffix READ getFileSuffix WRITE setFileSuffix)
    PYB11_PROPERTY(QString FileExtension READ getFileExtension WRITE setFileExtension)
    PYB11_PROPERTY(int PaddingDigits READ getPaddingDigits WRITE setPaddingDigits)
    PYB11_PROPERTY(AxisAngleInput_t SampleTransformation READ getSampleTransformation WRITE setSampleTransformation)
    PYB11_PROPERTY(AxisAngleInput_t EulerTransformation READ getEulerTransformation WRITE setEulerTransformation)
public:
  SIMPL_SHARED_POINTERS(EbsdToH5Ebsd)
  SIMPL_FILTER_NEW_MACRO(EbsdToH5Ebsd)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(EbsdToH5Ebsd, AbstractFilter)

  ~EbsdToH5Ebsd() override;

  SIMPL_INSTANCE_STRING_PROPERTY(OutputFile)

  SIMPL_INSTANCE_PROPERTY(int64_t, ZStartIndex)

  SIMPL_INSTANCE_PROPERTY(int64_t, ZEndIndex)

  SIMPL_INSTANCE_PROPERTY(float, ZResolution)

  SIMPL_INSTANCE_PROPERTY(uint32_t, RefFrameZDir)

  SIMPL_FILTER_PARAMETER(QString, InputPath)

  SIMPL_FILTER_PARAMETER(QString, FilePrefix)

  SIMPL_FILTER_PARAMETER(QString, FileSuffix)

  SIMPL_FILTER_PARAMETER(QString, FileExtension)

  SIMPL_FILTER_PARAMETER(int, PaddingDigits)

  SIMPL_FILTER_PARAMETER(AxisAngleInput_t, SampleTransformation)

  SIMPL_FILTER_PARAMETER(AxisAngleInput_t, EulerTransformation)

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
  EbsdToH5Ebsd();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

public:
  EbsdToH5Ebsd(const EbsdToH5Ebsd&) = delete;            // Copy Constructor Not Implemented
  EbsdToH5Ebsd(EbsdToH5Ebsd&&) = delete;                 // Move Constructor Not Implemented
  EbsdToH5Ebsd& operator=(const EbsdToH5Ebsd&) = delete; // Copy Assignment Not Implemented
  EbsdToH5Ebsd& operator=(EbsdToH5Ebsd&&) = delete;      // Move Assignment Not Implemented
};

