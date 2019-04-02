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
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "EMMPMFilter.h"

#include "EMMPM/EMMPMDLLExport.h"

class MultiEmmpmFilterMessageHandler;

/**
 * @brief The MultiEmmpmFilter class. See [Filter documentation](@ref multiemmpmfilter) for details.
 */
class EMMPM_EXPORT MultiEmmpmFilter : public EMMPMFilter
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(MultiEmmpmFilter SUPERCLASS EMMPMFilter)
    PYB11_PROPERTY(QVector<DataArrayPath> InputDataArrayVector READ getInputDataArrayVector WRITE setInputDataArrayVector)
    PYB11_PROPERTY(QString OutputAttributeMatrixName READ getOutputAttributeMatrixName WRITE setOutputAttributeMatrixName)
    PYB11_PROPERTY(QString OutputArrayPrefix READ getOutputArrayPrefix WRITE setOutputArrayPrefix)
    PYB11_PROPERTY(bool UsePreviousMuSigma READ getUsePreviousMuSigma WRITE setUsePreviousMuSigma)

public:
  SIMPL_SHARED_POINTERS(MultiEmmpmFilter)
  SIMPL_FILTER_NEW_MACRO(MultiEmmpmFilter)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(MultiEmmpmFilter, AbstractFilter)

  virtual ~MultiEmmpmFilter();

  friend MultiEmmpmFilterMessageHandler;

  SIMPL_FILTER_PARAMETER(QVector<DataArrayPath>, InputDataArrayVector)
  Q_PROPERTY(QVector<DataArrayPath> InputDataArrayVector READ getInputDataArrayVector WRITE setInputDataArrayVector)

  SIMPL_FILTER_PARAMETER(QString, OutputAttributeMatrixName)
  Q_PROPERTY(QString OutputAttributeMatrixName READ getOutputAttributeMatrixName WRITE setOutputAttributeMatrixName)

  SIMPL_FILTER_PARAMETER(QString, OutputArrayPrefix)
  Q_PROPERTY(QString OutputArrayPrefix READ getOutputArrayPrefix WRITE setOutputArrayPrefix)

  SIMPL_FILTER_PARAMETER(bool, UsePreviousMuSigma)
  Q_PROPERTY(bool UsePreviousMuSigma READ getUsePreviousMuSigma WRITE setUsePreviousMuSigma)

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
  DEFINE_DATAARRAY_VARIABLE(uint8_t, InputImage)
  DEFINE_DATAARRAY_VARIABLE(uint8_t, OutputImage)

  int m_ArrayCount;
  int m_CurrentArrayIndex = 1;

public:
  MultiEmmpmFilter(const MultiEmmpmFilter&) = delete; // Copy Constructor Not Implemented
  MultiEmmpmFilter(MultiEmmpmFilter&&) = delete;      // Move Constructor Not Implemented
  MultiEmmpmFilter& operator=(const MultiEmmpmFilter&) = delete; // Copy Assignment Not Implemented
  MultiEmmpmFilter& operator=(MultiEmmpmFilter&&) = delete;      // Move Assignment Not Implemented
};

