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
#include <memory>

#include "EbsdLib/EbsdConstants.h"

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/FilterParameters/AxisAngleInput.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @brief The EbsdToH5Ebsd class. See [Filter documentation](@ref ebsdtoh5ebsd) for details.
 */
class OrientationAnalysis_EXPORT EbsdToH5Ebsd : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(EbsdToH5Ebsd SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(EbsdToH5Ebsd)
  PYB11_FILTER_NEW_MACRO(EbsdToH5Ebsd)
  PYB11_FILTER_PARAMETER(QString, InputPath)
  PYB11_FILTER_PARAMETER(QString, FilePrefix)
  PYB11_FILTER_PARAMETER(QString, FileSuffix)
  PYB11_FILTER_PARAMETER(QString, FileExtension)
  PYB11_FILTER_PARAMETER(int, PaddingDigits)
  PYB11_FILTER_PARAMETER(AxisAngleInput_t, SampleTransformation)
  PYB11_FILTER_PARAMETER(AxisAngleInput_t, EulerTransformation)
  PYB11_PROPERTY(QString InputPath READ getInputPath WRITE setInputPath)
  PYB11_PROPERTY(QString OutputFile READ getOutputFile WRITE setOutputFile)
  PYB11_PROPERTY(QString FilePrefix READ getFilePrefix WRITE setFilePrefix)
  PYB11_PROPERTY(QString FileSuffix READ getFileSuffix WRITE setFileSuffix)
  PYB11_PROPERTY(QString FileExtension READ getFileExtension WRITE setFileExtension)
  PYB11_PROPERTY(int PaddingDigits READ getPaddingDigits WRITE setPaddingDigits)
  PYB11_PROPERTY(int64_t ZStartIndex READ getZStartIndex WRITE setZStartIndex)
  PYB11_PROPERTY(int64_t ZEndIndex READ getZEndIndex WRITE setZEndIndex)
  PYB11_PROPERTY(float ZResolution READ getZResolution WRITE setZResolution)
  PYB11_PROPERTY(AxisAngleInput_t SampleTransformation READ getSampleTransformation WRITE setSampleTransformation)
  PYB11_PROPERTY(AxisAngleInput_t EulerTransformation READ getEulerTransformation WRITE setEulerTransformation)
#endif

public:
  using Self = EbsdToH5Ebsd;
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
   * @brief Returns the name of the class for EbsdToH5Ebsd
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for EbsdToH5Ebsd
   */
  static QString ClassName();

  ~EbsdToH5Ebsd() override;

  /**
   * @brief Setter property for OutputFile
   */
  void setOutputFile(const QString& value);
  /**
   * @brief Getter property for OutputFile
   * @return Value of OutputFile
   */
  QString getOutputFile() const;

  /**
   * @brief Setter property for ZStartIndex
   */
  void setZStartIndex(int64_t value);
  /**
   * @brief Getter property for ZStartIndex
   * @return Value of ZStartIndex
   */
  int64_t getZStartIndex() const;

  /**
   * @brief Setter property for ZEndIndex
   */
  void setZEndIndex(int64_t value);
  /**
   * @brief Getter property for ZEndIndex
   * @return Value of ZEndIndex
   */
  int64_t getZEndIndex() const;

  /**
   * @brief Setter property for ZResolution
   */
  void setZResolution(float value);
  /**
   * @brief Getter property for ZResolution
   * @return Value of ZResolution
   */
  float getZResolution() const;

  /**
   * @brief Setter property for RefFrameZDir
   */
  void setRefFrameZDir(uint32_t value);
  /**
   * @brief Getter property for RefFrameZDir
   * @return Value of RefFrameZDir
   */
  uint32_t getRefFrameZDir() const;

  /**
   * @brief Setter property for InputPath
   */
  void setInputPath(const QString& value);
  /**
   * @brief Getter property for InputPath
   * @return Value of InputPath
   */
  QString getInputPath() const;

  /**
   * @brief Setter property for FilePrefix
   */
  void setFilePrefix(const QString& value);
  /**
   * @brief Getter property for FilePrefix
   * @return Value of FilePrefix
   */
  QString getFilePrefix() const;

  /**
   * @brief Setter property for FileSuffix
   */
  void setFileSuffix(const QString& value);
  /**
   * @brief Getter property for FileSuffix
   * @return Value of FileSuffix
   */
  QString getFileSuffix() const;

  /**
   * @brief Setter property for FileExtension
   */
  void setFileExtension(const QString& value);
  /**
   * @brief Getter property for FileExtension
   * @return Value of FileExtension
   */
  QString getFileExtension() const;

  /**
   * @brief Setter property for PaddingDigits
   */
  void setPaddingDigits(int value);
  /**
   * @brief Getter property for PaddingDigits
   * @return Value of PaddingDigits
   */
  int getPaddingDigits() const;

  /**
   * @brief Setter property for SampleTransformation
   */
  void setSampleTransformation(const AxisAngleInput_t& value);
  /**
   * @brief Getter property for SampleTransformation
   * @return Value of SampleTransformation
   */
  AxisAngleInput_t getSampleTransformation() const;

  /**
   * @brief Setter property for EulerTransformation
   */
  void setEulerTransformation(const AxisAngleInput_t& value);
  /**
   * @brief Getter property for EulerTransformation
   * @return Value of EulerTransformation
   */
  AxisAngleInput_t getEulerTransformation() const;

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

private:
  QString m_OutputFile = {};
  int64_t m_ZStartIndex = {};
  int64_t m_ZEndIndex = {};
  float m_ZResolution = {};
  uint32_t m_RefFrameZDir = {};
  QString m_InputPath = {};
  QString m_FilePrefix = {};
  QString m_FileSuffix = {};
  QString m_FileExtension = {};
  int m_PaddingDigits = {};
  AxisAngleInput_t m_SampleTransformation = {};
  AxisAngleInput_t m_EulerTransformation = {};
};

