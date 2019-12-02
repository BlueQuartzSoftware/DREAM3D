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

#include <QtCore/QString>
#include <vector>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

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

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(FindEllipsoidError SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(FindEllipsoidError)
  PYB11_FILTER_NEW_MACRO(FindEllipsoidError)
  PYB11_FILTER_PARAMETER(DataArrayPath, CellFeatureAttributeMatrixName)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CentroidsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, NumCellsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, AxisLengthsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, AxisEulerAnglesArrayPath)
  PYB11_FILTER_PARAMETER(QString, IdealFeatureIdsArrayName)
  PYB11_FILTER_PARAMETER(QString, EllipsoidErrorArrayName)
  PYB11_FILTER_PARAMETER(bool, WriteIdealEllipseFeatureIds)
  PYB11_PROPERTY(DataArrayPath CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)
  PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
  PYB11_PROPERTY(DataArrayPath CentroidsArrayPath READ getCentroidsArrayPath WRITE setCentroidsArrayPath)
  PYB11_PROPERTY(DataArrayPath NumCellsArrayPath READ getNumCellsArrayPath WRITE setNumCellsArrayPath)
  PYB11_PROPERTY(DataArrayPath AxisLengthsArrayPath READ getAxisLengthsArrayPath WRITE setAxisLengthsArrayPath)
  PYB11_PROPERTY(DataArrayPath AxisEulerAnglesArrayPath READ getAxisEulerAnglesArrayPath WRITE setAxisEulerAnglesArrayPath)
  PYB11_PROPERTY(QString IdealFeatureIdsArrayName READ getIdealFeatureIdsArrayName WRITE setIdealFeatureIdsArrayName)
  PYB11_PROPERTY(QString EllipsoidErrorArrayName READ getEllipsoidErrorArrayName WRITE setEllipsoidErrorArrayName)
  PYB11_PROPERTY(bool WriteIdealEllipseFeatureIds READ getWriteIdealEllipseFeatureIds WRITE setWriteIdealEllipseFeatureIds)
#endif

public:
  using Self = FindEllipsoidError;
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
   * @brief Returns the name of the class for FindEllipsoidError
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FindEllipsoidError
   */
  static QString ClassName();

  ~FindEllipsoidError() override;
  /**
   * @brief Setter property for CellFeatureAttributeMatrixName
   */
  void setCellFeatureAttributeMatrixName(const DataArrayPath& value);
  /**
   * @brief Getter property for CellFeatureAttributeMatrixName
   * @return Value of CellFeatureAttributeMatrixName
   */
  DataArrayPath getCellFeatureAttributeMatrixName() const;

  Q_PROPERTY(DataArrayPath CellFeatureAttributeMatrixName READ getCellFeatureAttributeMatrixName WRITE setCellFeatureAttributeMatrixName)

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
   * @brief Setter property for CentroidsArrayPath
   */
  void setCentroidsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CentroidsArrayPath
   * @return Value of CentroidsArrayPath
   */
  DataArrayPath getCentroidsArrayPath() const;

  Q_PROPERTY(DataArrayPath CentroidsArrayPath READ getCentroidsArrayPath WRITE setCentroidsArrayPath)

  /**
   * @brief Setter property for NumCellsArrayPath
   */
  void setNumCellsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for NumCellsArrayPath
   * @return Value of NumCellsArrayPath
   */
  DataArrayPath getNumCellsArrayPath() const;

  Q_PROPERTY(DataArrayPath NumCellsArrayPath READ getNumCellsArrayPath WRITE setNumCellsArrayPath)

  /**
   * @brief Setter property for AxisLengthsArrayPath
   */
  void setAxisLengthsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for AxisLengthsArrayPath
   * @return Value of AxisLengthsArrayPath
   */
  DataArrayPath getAxisLengthsArrayPath() const;

  Q_PROPERTY(DataArrayPath AxisLengthsArrayPath READ getAxisLengthsArrayPath WRITE setAxisLengthsArrayPath)

  /**
   * @brief Setter property for AxisEulerAnglesArrayPath
   */
  void setAxisEulerAnglesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for AxisEulerAnglesArrayPath
   * @return Value of AxisEulerAnglesArrayPath
   */
  DataArrayPath getAxisEulerAnglesArrayPath() const;

  Q_PROPERTY(DataArrayPath AxisEulerAnglesArrayPath READ getAxisEulerAnglesArrayPath WRITE setAxisEulerAnglesArrayPath)

  /**
   * @brief Setter property for IdealFeatureIdsArrayName
   */
  void setIdealFeatureIdsArrayName(const QString& value);
  /**
   * @brief Getter property for IdealFeatureIdsArrayName
   * @return Value of IdealFeatureIdsArrayName
   */
  QString getIdealFeatureIdsArrayName() const;

  Q_PROPERTY(QString IdealFeatureIdsArrayName READ getIdealFeatureIdsArrayName WRITE setIdealFeatureIdsArrayName)

  /**
   * @brief Setter property for EllipsoidErrorArrayName
   */
  void setEllipsoidErrorArrayName(const QString& value);
  /**
   * @brief Getter property for EllipsoidErrorArrayName
   * @return Value of EllipsoidErrorArrayName
   */
  QString getEllipsoidErrorArrayName() const;

  Q_PROPERTY(QString EllipsoidErrorArrayName READ getEllipsoidErrorArrayName WRITE setEllipsoidErrorArrayName)

  /**
   * @brief Setter property for WriteIdealEllipseFeatureIds
   */
  void setWriteIdealEllipseFeatureIds(bool value);
  /**
   * @brief Getter property for WriteIdealEllipseFeatureIds
   * @return Value of WriteIdealEllipseFeatureIds
   */
  bool getWriteIdealEllipseFeatureIds() const;

  Q_PROPERTY(bool WriteIdealEllipseFeatureIds READ getWriteIdealEllipseFeatureIds WRITE setWriteIdealEllipseFeatureIds)

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
  std::weak_ptr<DataArray<int32_t>> m_FeatureIdsPtr;
  int32_t* m_FeatureIds = nullptr;
  std::weak_ptr<DataArray<float>> m_AxisEulerAnglesPtr;
  float* m_AxisEulerAngles = nullptr;
  std::weak_ptr<DataArray<float>> m_CentroidsPtr;
  float* m_Centroids = nullptr;
  std::weak_ptr<DataArray<float>> m_AxisLengthsPtr;
  float* m_AxisLengths = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_NumCellsPtr;
  int32_t* m_NumCells = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_IdealFeatureIdsPtr;
  int32_t* m_IdealFeatureIds = nullptr;
  std::weak_ptr<DataArray<float>> m_EllipsoidErrorPtr;
  float* m_EllipsoidError = nullptr;

  DataArrayPath m_CellFeatureAttributeMatrixName = {};
  DataArrayPath m_FeatureIdsArrayPath = {};
  DataArrayPath m_CentroidsArrayPath = {};
  DataArrayPath m_NumCellsArrayPath = {};
  DataArrayPath m_AxisLengthsArrayPath = {};
  DataArrayPath m_AxisEulerAnglesArrayPath = {};
  QString m_IdealFeatureIdsArrayName = {};
  QString m_EllipsoidErrorArrayName = {};
  bool m_WriteIdealEllipseFeatureIds = {};

  double m_ScaleFator;

public:
  FindEllipsoidError(const FindEllipsoidError&) = delete; // Copy Constructor Not Implemented
  FindEllipsoidError(FindEllipsoidError&&) = delete;      // Move Constructor Not Implemented
  FindEllipsoidError& operator=(const FindEllipsoidError&) = delete; // Copy Assignment Not Implemented
  FindEllipsoidError& operator=(FindEllipsoidError&&) = delete;      // Move Assignment Not Implemented
};

