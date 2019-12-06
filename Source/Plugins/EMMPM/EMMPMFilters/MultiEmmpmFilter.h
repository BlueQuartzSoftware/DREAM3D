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
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "EMMPMFilter.h"

#include "EMMPM/EMMPMDLLExport.h"

class MultiEmmpmFilterMessageHandler;

/**
 * @brief The MultiEmmpmFilter class. See [Filter documentation](@ref multiemmpmfilter) for details.
 */
class EMMPM_EXPORT MultiEmmpmFilter : public EMMPMFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(MultiEmmpmFilter SUPERCLASS EMMPMFilter)
  PYB11_SHARED_POINTERS(MultiEmmpmFilter)
  PYB11_FILTER_NEW_MACRO(MultiEmmpmFilter)
  PYB11_FILTER_PARAMETER(QVector<DataArrayPath>, InputDataArrayVector)
  PYB11_FILTER_PARAMETER(QString, OutputAttributeMatrixName)
  PYB11_FILTER_PARAMETER(QString, OutputArrayPrefix)
  PYB11_FILTER_PARAMETER(bool, UsePreviousMuSigma)
  PYB11_PROPERTY(QVector<DataArrayPath> InputDataArrayVector READ getInputDataArrayVector WRITE setInputDataArrayVector)
  PYB11_PROPERTY(QString OutputAttributeMatrixName READ getOutputAttributeMatrixName WRITE setOutputAttributeMatrixName)
  PYB11_PROPERTY(QString OutputArrayPrefix READ getOutputArrayPrefix WRITE setOutputArrayPrefix)
  PYB11_PROPERTY(bool UsePreviousMuSigma READ getUsePreviousMuSigma WRITE setUsePreviousMuSigma)
#endif

public:
  using Self = MultiEmmpmFilter;
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
   * @brief Returns the name of the class for MultiEmmpmFilter
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for MultiEmmpmFilter
   */
  static QString ClassName();

  virtual ~MultiEmmpmFilter();

  friend MultiEmmpmFilterMessageHandler;

  /**
   * @brief Setter property for InputDataArrayVector
   */
  void setInputDataArrayVector(const QVector<DataArrayPath>& value);
  /**
   * @brief Getter property for InputDataArrayVector
   * @return Value of InputDataArrayVector
   */
  QVector<DataArrayPath> getInputDataArrayVector() const;

  Q_PROPERTY(QVector<DataArrayPath> InputDataArrayVector READ getInputDataArrayVector WRITE setInputDataArrayVector)

  /**
   * @brief Setter property for OutputAttributeMatrixName
   */
  void setOutputAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for OutputAttributeMatrixName
   * @return Value of OutputAttributeMatrixName
   */
  QString getOutputAttributeMatrixName() const;

  Q_PROPERTY(QString OutputAttributeMatrixName READ getOutputAttributeMatrixName WRITE setOutputAttributeMatrixName)

  /**
   * @brief Setter property for OutputArrayPrefix
   */
  void setOutputArrayPrefix(const QString& value);
  /**
   * @brief Getter property for OutputArrayPrefix
   * @return Value of OutputArrayPrefix
   */
  QString getOutputArrayPrefix() const;

  Q_PROPERTY(QString OutputArrayPrefix READ getOutputArrayPrefix WRITE setOutputArrayPrefix)

  /**
   * @brief Setter property for UsePreviousMuSigma
   */
  void setUsePreviousMuSigma(bool value);
  /**
   * @brief Getter property for UsePreviousMuSigma
   * @return Value of UsePreviousMuSigma
   */
  bool getUsePreviousMuSigma() const;

  Q_PROPERTY(bool UsePreviousMuSigma READ getUsePreviousMuSigma WRITE setUsePreviousMuSigma)

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

protected:
  MultiEmmpmFilter();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

protected slots:
  /**
   * @brief generateEmmpmMessage
   * @param msg
   */
  virtual void handleEmmpmMessage(const AbstractMessage::Pointer& msg);

private:
  std::weak_ptr<DataArray<uint8_t>> m_InputImagePtr;
  uint8_t* m_InputImage = nullptr;
  std::weak_ptr<DataArray<uint8_t>> m_OutputImagePtr;
  uint8_t* m_OutputImage = nullptr;

  QVector<DataArrayPath> m_InputDataArrayVector = {};
  QString m_OutputAttributeMatrixName = {};
  QString m_OutputArrayPrefix = {};
  bool m_UsePreviousMuSigma = {};

  int m_ArrayCount;
  int m_CurrentArrayIndex = 1;

public:
  MultiEmmpmFilter(const MultiEmmpmFilter&) = delete; // Copy Constructor Not Implemented
  MultiEmmpmFilter(MultiEmmpmFilter&&) = delete;      // Move Constructor Not Implemented
  MultiEmmpmFilter& operator=(const MultiEmmpmFilter&) = delete; // Copy Assignment Not Implemented
  MultiEmmpmFilter& operator=(MultiEmmpmFilter&&) = delete;      // Move Assignment Not Implemented
};

