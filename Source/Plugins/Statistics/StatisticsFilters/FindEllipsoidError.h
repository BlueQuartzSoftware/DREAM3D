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

#include <QtCore/QString>
#include <vector>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "Statistics/StatisticsConstants.h"
#include "Statistics/StatisticsVersion.h"

#include "Statistics/StatisticsDLLExport.h"

/**
 * @class FindEllipsoidError FindEllipsoidError.h DREAM3DLib/GenericFilters/FindEllipsoidError.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class Statistics_EXPORT FindEllipsoidError : public AbstractFilter
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(FindEllipsoidError SUPERCLASS AbstractFilter)
    PYB11_PROPERTY(DataArrayPath CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)
    PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
    PYB11_PROPERTY(DataArrayPath CentroidsArrayPath READ getCentroidsArrayPath WRITE setCentroidsArrayPath)
    PYB11_PROPERTY(DataArrayPath NumCellsArrayPath READ getNumCellsArrayPath WRITE setNumCellsArrayPath)
    PYB11_PROPERTY(DataArrayPath AxisLengthsArrayPath READ getAxisLengthsArrayPath WRITE setAxisLengthsArrayPath)
    PYB11_PROPERTY(DataArrayPath AxisEulerAnglesArrayPath READ getAxisEulerAnglesArrayPath WRITE setAxisEulerAnglesArrayPath)
    PYB11_PROPERTY(QString IdealFeatureIdsArrayName READ getIdealFeatureIdsArrayName WRITE setIdealFeatureIdsArrayName)
    PYB11_PROPERTY(QString EllipsoidErrorArrayName READ getEllipsoidErrorArrayName WRITE setEllipsoidErrorArrayName)
    PYB11_PROPERTY(bool WriteIdealEllipseFeatureIds READ getWriteIdealEllipseFeatureIds WRITE setWriteIdealEllipseFeatureIds)
public:
  SIMPL_SHARED_POINTERS(FindEllipsoidError)
  SIMPL_FILTER_NEW_MACRO(FindEllipsoidError)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(FindEllipsoidError, AbstractFilter)

  ~FindEllipsoidError() override;
  SIMPL_FILTER_PARAMETER(DataArrayPath, CellFeatureAttributeMatrixName)
  Q_PROPERTY(DataArrayPath CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, CentroidsArrayPath)
  Q_PROPERTY(DataArrayPath CentroidsArrayPath READ getCentroidsArrayPath WRITE setCentroidsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, NumCellsArrayPath)
  Q_PROPERTY(DataArrayPath NumCellsArrayPath READ getNumCellsArrayPath WRITE setNumCellsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, AxisLengthsArrayPath)
  Q_PROPERTY(DataArrayPath AxisLengthsArrayPath READ getAxisLengthsArrayPath WRITE setAxisLengthsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, AxisEulerAnglesArrayPath)
  Q_PROPERTY(DataArrayPath AxisEulerAnglesArrayPath READ getAxisEulerAnglesArrayPath WRITE setAxisEulerAnglesArrayPath)

  SIMPL_FILTER_PARAMETER(QString, IdealFeatureIdsArrayName)
  Q_PROPERTY(QString IdealFeatureIdsArrayName READ getIdealFeatureIdsArrayName WRITE setIdealFeatureIdsArrayName)

  SIMPL_FILTER_PARAMETER(QString, EllipsoidErrorArrayName)
  Q_PROPERTY(QString EllipsoidErrorArrayName READ getEllipsoidErrorArrayName WRITE setEllipsoidErrorArrayName)

  SIMPL_FILTER_PARAMETER(bool, WriteIdealEllipseFeatureIds)
  Q_PROPERTY(bool WriteIdealEllipseFeatureIds READ getWriteIdealEllipseFeatureIds WRITE setWriteIdealEllipseFeatureIds)

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
  FindEllipsoidError();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  void find_error2D();

private:
  DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)
  DEFINE_DATAARRAY_VARIABLE(float, AxisEulerAngles)
  DEFINE_DATAARRAY_VARIABLE(float, Centroids)
  DEFINE_DATAARRAY_VARIABLE(float, AxisLengths)
  DEFINE_DATAARRAY_VARIABLE(int32_t, NumCells)
  DEFINE_DATAARRAY_VARIABLE(int32_t, IdealFeatureIds)
  DEFINE_DATAARRAY_VARIABLE(float, EllipsoidError)

  double m_ScaleFator;

public:
  FindEllipsoidError(const FindEllipsoidError&) = delete; // Copy Constructor Not Implemented
  FindEllipsoidError(FindEllipsoidError&&) = delete;      // Move Constructor Not Implemented
  FindEllipsoidError& operator=(const FindEllipsoidError&) = delete; // Copy Assignment Not Implemented
  FindEllipsoidError& operator=(FindEllipsoidError&&) = delete;      // Move Assignment Not Implemented
};

