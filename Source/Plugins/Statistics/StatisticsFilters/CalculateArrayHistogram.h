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

class IDataArray;
using IDataArrayWkPtrType = std::weak_ptr<IDataArray>;

#include "Statistics/StatisticsDLLExport.h"

/**
 * @brief The CalculateArrayHistogram class. See [Filter documentation](@ref calculatearrayhistogram) for details.
 */
class Statistics_EXPORT CalculateArrayHistogram : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(CalculateArrayHistogram SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(CalculateArrayHistogram)
  PYB11_FILTER_NEW_MACRO(CalculateArrayHistogram)
  PYB11_FILTER_PARAMETER(DataArrayPath, SelectedArrayPath)
  PYB11_FILTER_PARAMETER(int, NumberOfBins)
  PYB11_FILTER_PARAMETER(double, MinRange)
  PYB11_FILTER_PARAMETER(double, MaxRange)
  PYB11_FILTER_PARAMETER(bool, UserDefinedRange)
  PYB11_FILTER_PARAMETER(bool, Normalize)
  PYB11_FILTER_PARAMETER(QString, NewAttributeMatrixName)
  PYB11_FILTER_PARAMETER(QString, NewDataArrayName)
  PYB11_FILTER_PARAMETER(bool, NewDataContainer)
  PYB11_FILTER_PARAMETER(DataArrayPath, NewDataContainerName)
  PYB11_PROPERTY(DataArrayPath SelectedArrayPath READ getSelectedArrayPath WRITE setSelectedArrayPath)
  PYB11_PROPERTY(int NumberOfBins READ getNumberOfBins WRITE setNumberOfBins)
  PYB11_PROPERTY(double MinRange READ getMinRange WRITE setMinRange)
  PYB11_PROPERTY(double MaxRange READ getMaxRange WRITE setMaxRange)
  PYB11_PROPERTY(bool UserDefinedRange READ getUserDefinedRange WRITE setUserDefinedRange)
  PYB11_PROPERTY(bool Normalize READ getNormalize WRITE setNormalize)
  PYB11_PROPERTY(QString NewAttributeMatrixName READ getNewAttributeMatrixName WRITE setNewAttributeMatrixName)
  PYB11_PROPERTY(QString NewDataArrayName READ getNewDataArrayName WRITE setNewDataArrayName)
  PYB11_PROPERTY(bool NewDataContainer READ getNewDataContainer WRITE setNewDataContainer)
  PYB11_PROPERTY(DataArrayPath NewDataContainerName READ getNewDataContainerName WRITE setNewDataContainerName)
#endif

public:
  using Self = CalculateArrayHistogram;
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
   * @brief Returns the name of the class for CalculateArrayHistogram
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for CalculateArrayHistogram
   */
  static QString ClassName();

  ~CalculateArrayHistogram() override;

  /**
   * @brief Setter property for SelectedArrayPath
   */
  void setSelectedArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SelectedArrayPath
   * @return Value of SelectedArrayPath
   */
  DataArrayPath getSelectedArrayPath() const;

  Q_PROPERTY(DataArrayPath SelectedArrayPath READ getSelectedArrayPath WRITE setSelectedArrayPath)

  /**
   * @brief Setter property for NumberOfBins
   */
  void setNumberOfBins(int value);
  /**
   * @brief Getter property for NumberOfBins
   * @return Value of NumberOfBins
   */
  int getNumberOfBins() const;

  Q_PROPERTY(int NumberOfBins READ getNumberOfBins WRITE setNumberOfBins)

  /**
   * @brief Setter property for MinRange
   */
  void setMinRange(double value);
  /**
   * @brief Getter property for MinRange
   * @return Value of MinRange
   */
  double getMinRange() const;

  Q_PROPERTY(double MinRange READ getMinRange WRITE setMinRange)

  /**
   * @brief Setter property for MaxRange
   */
  void setMaxRange(double value);
  /**
   * @brief Getter property for MaxRange
   * @return Value of MaxRange
   */
  double getMaxRange() const;

  Q_PROPERTY(double MaxRange READ getMaxRange WRITE setMaxRange)

  /**
   * @brief Setter property for UserDefinedRange
   */
  void setUserDefinedRange(bool value);
  /**
   * @brief Getter property for UserDefinedRange
   * @return Value of UserDefinedRange
   */
  bool getUserDefinedRange() const;

  Q_PROPERTY(bool UserDefinedRange READ getUserDefinedRange WRITE setUserDefinedRange)

  /**
   * @brief Setter property for Normalize
   */
  void setNormalize(bool value);
  /**
   * @brief Getter property for Normalize
   * @return Value of Normalize
   */
  bool getNormalize() const;

  Q_PROPERTY(bool Normalize READ getNormalize WRITE setNormalize)

  /**
   * @brief Setter property for NewAttributeMatrixName
   */
  void setNewAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for NewAttributeMatrixName
   * @return Value of NewAttributeMatrixName
   */
  QString getNewAttributeMatrixName() const;

  Q_PROPERTY(QString NewAttributeMatrixName READ getNewAttributeMatrixName WRITE setNewAttributeMatrixName)

  /**
   * @brief Setter property for NewDataArrayName
   */
  void setNewDataArrayName(const QString& value);
  /**
   * @brief Getter property for NewDataArrayName
   * @return Value of NewDataArrayName
   */
  QString getNewDataArrayName() const;

  Q_PROPERTY(QString NewDataArrayName READ getNewDataArrayName WRITE setNewDataArrayName)

  /**
   * @brief Setter property for NewDataContainer
   */
  void setNewDataContainer(bool value);
  /**
   * @brief Getter property for NewDataContainer
   * @return Value of NewDataContainer
   */
  bool getNewDataContainer() const;

  Q_PROPERTY(bool NewDataContainer READ getNewDataContainer WRITE setNewDataContainer)

  /**
   * @brief Setter property for NewDataContainerName
   */
  void setNewDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for NewDataContainerName
   * @return Value of NewDataContainerName
   */
  DataArrayPath getNewDataContainerName() const;

  Q_PROPERTY(DataArrayPath NewDataContainerName READ getNewDataContainerName WRITE setNewDataContainerName)

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
   * @brief readFilterParametersFromJson Reads the filter parameters from a file
   * @param reader Reader that is used to read the parameters from a file
   */
  void readFilterParameters(QJsonObject& obj) override;

  /**
   * @brief writeFilterParametersToJson Writes the filter parameters to a file
   * @param root The root json object
   */
  void writeFilterParameters(QJsonObject& obj) const override;

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
  CalculateArrayHistogram();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  IDataArrayWkPtrType m_InDataArrayPtr;
  void* m_InDataArray = nullptr;

  std::weak_ptr<DataArray<double>> m_NewDataArrayPtr;
  double* m_NewDataArray = nullptr;

  DataArrayPath m_SelectedArrayPath = {};
  int m_NumberOfBins = {};
  double m_MinRange = {};
  double m_MaxRange = {};
  bool m_UserDefinedRange = {};
  bool m_Normalize = {};
  QString m_NewAttributeMatrixName = {};
  QString m_NewDataArrayName = {};
  bool m_NewDataContainer = {};
  DataArrayPath m_NewDataContainerName = {};

public:
  CalculateArrayHistogram(const CalculateArrayHistogram&) = delete;            // Copy Constructor Not Implemented
  CalculateArrayHistogram(CalculateArrayHistogram&&) = delete;                 // Move Constructor Not Implemented
  CalculateArrayHistogram& operator=(const CalculateArrayHistogram&) = delete; // Copy Assignment Not Implemented
  CalculateArrayHistogram& operator=(CalculateArrayHistogram&&) = delete;      // Move Assignment Not Implemented
};

