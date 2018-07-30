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

#if defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#endif

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @brief The ConvertHexGridToSquareGrid class. See [Filter documentation](@ref converthexgridtosquaregrid) for details.
 */
class OrientationAnalysis_EXPORT ConvertHexGridToSquareGrid : public AbstractFilter
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(ConvertHexGridToSquareGrid SUPERCLASS AbstractFilter)
    PYB11_PROPERTY(QString InputPath READ getInputPath WRITE setInputPath)
    PYB11_PROPERTY(QString OutputPath READ getOutputPath WRITE setOutputPath)
    PYB11_PROPERTY(QString OutputPrefix READ getOutputPrefix WRITE setOutputPrefix)
    PYB11_PROPERTY(QString FilePrefix READ getFilePrefix WRITE setFilePrefix)
    PYB11_PROPERTY(QString FileSuffix READ getFileSuffix WRITE setFileSuffix)
    PYB11_PROPERTY(QString FileExtension READ getFileExtension WRITE setFileExtension)
    PYB11_PROPERTY(int PaddingDigits READ getPaddingDigits WRITE setPaddingDigits)
    PYB11_PROPERTY(int HexGridStack READ getHexGridStack WRITE setHexGridStack)
public:
  SIMPL_SHARED_POINTERS(ConvertHexGridToSquareGrid)
  SIMPL_FILTER_NEW_MACRO(ConvertHexGridToSquareGrid)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(ConvertHexGridToSquareGrid, AbstractFilter)

  ~ConvertHexGridToSquareGrid() override;

  SIMPL_INSTANCE_PROPERTY(int64_t, ZStartIndex)

  SIMPL_INSTANCE_PROPERTY(int64_t, ZEndIndex)

  SIMPL_INSTANCE_PROPERTY(float, XResolution)

  SIMPL_INSTANCE_PROPERTY(float, YResolution)

  SIMPL_FILTER_PARAMETER(QString, InputPath)

  SIMPL_FILTER_PARAMETER(QString, OutputPath)

  SIMPL_FILTER_PARAMETER(QString, OutputPrefix)

  SIMPL_FILTER_PARAMETER(QString, FilePrefix)

  SIMPL_FILTER_PARAMETER(QString, FileSuffix)

  SIMPL_FILTER_PARAMETER(QString, FileExtension)

  SIMPL_FILTER_PARAMETER(int, PaddingDigits)

  SIMPL_INSTANCE_PROPERTY(int, NumCols)

  SIMPL_INSTANCE_PROPERTY(int, NumRows)

  SIMPL_INSTANCE_PROPERTY(bool, HeaderIsComplete)

  SIMPL_FILTER_PARAMETER(int, HexGridStack)
  Q_PROPERTY(int HexGridStack READ getHexGridStack WRITE setHexGridStack)

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
  ConvertHexGridToSquareGrid();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  /**
   * @brief modifyAngHeaderLine Modifies a single line of the header section of the TSL .ang file if necessary
   * @param buf The line to possibly modify
   * @return
   */
  QString modifyAngHeaderLine(QString& buf);

public:
  ConvertHexGridToSquareGrid(const ConvertHexGridToSquareGrid&) = delete; // Copy Constructor Not Implemented
  ConvertHexGridToSquareGrid(ConvertHexGridToSquareGrid&&) = delete;      // Move Constructor Not Implemented
  ConvertHexGridToSquareGrid& operator=(const ConvertHexGridToSquareGrid&) = delete; // Copy Assignment Not Implemented
  ConvertHexGridToSquareGrid& operator=(ConvertHexGridToSquareGrid&&) = delete;      // Move Assignment Not Implemented
};

