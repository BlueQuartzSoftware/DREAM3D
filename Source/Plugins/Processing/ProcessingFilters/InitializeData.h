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

#include <QtCore/QVector>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/FilterParameters/RangeFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "Processing/ProcessingDLLExport.h"

/**
 * @brief The InitializeData class. See [Filter documentation](@ref initializedata) for details.
 */
class Processing_EXPORT InitializeData : public AbstractFilter
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(InitializeData SUPERCLASS AbstractFilter)
    PYB11_PROPERTY(QVector<DataArrayPath> CellAttributeMatrixPaths READ getCellAttributeMatrixPaths WRITE setCellAttributeMatrixPaths)
    PYB11_PROPERTY(int XMin READ getXMin WRITE setXMin)
    PYB11_PROPERTY(int YMin READ getYMin WRITE setYMin)
    PYB11_PROPERTY(int ZMin READ getZMin WRITE setZMin)
    PYB11_PROPERTY(int XMax READ getXMax WRITE setXMax)
    PYB11_PROPERTY(int YMax READ getYMax WRITE setYMax)
    PYB11_PROPERTY(int ZMax READ getZMax WRITE setZMax)
    PYB11_PROPERTY(int InitType READ getInitType WRITE setInitType)
    PYB11_PROPERTY(bool Random READ getRandom WRITE setRandom)
    PYB11_PROPERTY(double InitValue READ getInitValue WRITE setInitValue)
    PYB11_PROPERTY(FPRangePair InitRange READ getInitRange WRITE setInitRange)
public:
  SIMPL_SHARED_POINTERS(InitializeData)
  SIMPL_FILTER_NEW_MACRO(InitializeData)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(InitializeData, AbstractFilter)

  ~InitializeData() override;

  SIMPL_FILTER_PARAMETER(QVector<DataArrayPath>, CellAttributeMatrixPaths)
  Q_PROPERTY(QVector<DataArrayPath> CellAttributeMatrixPaths READ getCellAttributeMatrixPaths WRITE setCellAttributeMatrixPaths)

  SIMPL_FILTER_PARAMETER(int, XMin)
  Q_PROPERTY(int XMin READ getXMin WRITE setXMin)

  SIMPL_FILTER_PARAMETER(int, YMin)
  Q_PROPERTY(int YMin READ getYMin WRITE setYMin)

  SIMPL_FILTER_PARAMETER(int, ZMin)
  Q_PROPERTY(int ZMin READ getZMin WRITE setZMin)

  SIMPL_FILTER_PARAMETER(int, XMax)
  Q_PROPERTY(int XMax READ getXMax WRITE setXMax)

  SIMPL_FILTER_PARAMETER(int, YMax)
  Q_PROPERTY(int YMax READ getYMax WRITE setYMax)

  SIMPL_FILTER_PARAMETER(int, ZMax)
  Q_PROPERTY(int ZMax READ getZMax WRITE setZMax)

  SIMPL_FILTER_PARAMETER(int, InitType)
  Q_PROPERTY(int InitType READ getInitType WRITE setInitType)

  SIMPL_FILTER_PARAMETER(bool, Random)
  Q_PROPERTY(bool Random READ getRandom WRITE setRandom)

  SIMPL_FILTER_PARAMETER(double, InitValue)
  Q_PROPERTY(double InitValue READ getInitValue WRITE setInitValue)

  SIMPL_FILTER_PARAMETER(FPRangePair, InitRange)
  Q_PROPERTY(FPRangePair InitRange READ getInitRange WRITE setInitRange)

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
  InitializeData();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  enum InitChoices
  {
    Manual,
    Random,
    RandomWithRange
  };

  /**
  * @brief initializeArrayWithInts Initializes the array p with integers, either from the
  * manual value entered in the filter, or with a random number.  This function does not
  * check that the template type actually is an integer, so it will most likely cause
  * unexpected results when passing anything other than an integer as a template parameter.
  * @param p The array that will be initialized
  * @param dims The dimensions of the array p
  */
  template <typename T> void initializeArrayWithInts(IDataArray::Pointer p, int64_t dims[3]);

  /**
  * @brief initializeArrayWithReals Initializes the array p with real numbers, either from the
  * manual value entered in the filter, or with a random number.  This function does not
  * check that the template type actually is a non-integer, so it will most likely cause
  * unexpected results when passing anything other than a float or double as a template
  * parameter.
  * @param p The array that will be initialized
  * @param dims The dimensions of the array p
  */
  template <typename T> void initializeArrayWithReals(IDataArray::Pointer p, int64_t dims[3]);

  /**
  * @brief checkInitialization Checks that the chosen initialization value/range is inside
  * the bounds of the array type
  */
  template <typename T> void checkInitialization(IDataArray::Pointer p);

public:
  InitializeData(const InitializeData&) = delete; // Copy Constructor Not Implemented
  InitializeData(InitializeData&&) = delete;      // Move Constructor Not Implemented
  InitializeData& operator=(const InitializeData&) = delete; // Copy Assignment Not Implemented
  InitializeData& operator=(InitializeData&&) = delete;      // Move Assignment
};

