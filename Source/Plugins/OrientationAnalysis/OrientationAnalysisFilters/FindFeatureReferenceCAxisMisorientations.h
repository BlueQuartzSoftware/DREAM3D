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

#include "OrientationLib/LaueOps/LaueOps.h"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @brief The FindFeatureReferenceCAxisMisorientations class. See [Filter documentation](@ref findfeaturereferencecaxismisorientations) for details.
 */
class OrientationAnalysis_EXPORT FindFeatureReferenceCAxisMisorientations : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(FindFeatureReferenceCAxisMisorientations SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(FindFeatureReferenceCAxisMisorientations)
  PYB11_FILTER_NEW_MACRO(FindFeatureReferenceCAxisMisorientations)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, AvgCAxesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, QuatsArrayPath)
  PYB11_FILTER_PARAMETER(QString, FeatureAvgCAxisMisorientationsArrayName)
  PYB11_FILTER_PARAMETER(QString, FeatureStdevCAxisMisorientationsArrayName)
  PYB11_FILTER_PARAMETER(QString, FeatureReferenceCAxisMisorientationsArrayName)
  PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
  PYB11_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath AvgCAxesArrayPath READ getAvgCAxesArrayPath WRITE setAvgCAxesArrayPath)
  PYB11_PROPERTY(DataArrayPath QuatsArrayPath READ getQuatsArrayPath WRITE setQuatsArrayPath)
  PYB11_PROPERTY(QString FeatureAvgCAxisMisorientationsArrayName READ getFeatureAvgCAxisMisorientationsArrayName WRITE setFeatureAvgCAxisMisorientationsArrayName)
  PYB11_PROPERTY(QString FeatureStdevCAxisMisorientationsArrayName READ getFeatureStdevCAxisMisorientationsArrayName WRITE setFeatureStdevCAxisMisorientationsArrayName)
  PYB11_PROPERTY(QString FeatureReferenceCAxisMisorientationsArrayName READ getFeatureReferenceCAxisMisorientationsArrayName WRITE setFeatureReferenceCAxisMisorientationsArrayName)
#endif

public:
  using Self = FindFeatureReferenceCAxisMisorientations;
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
   * @brief Returns the name of the class for FindFeatureReferenceCAxisMisorientations
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FindFeatureReferenceCAxisMisorientations
   */
  static QString ClassName();

  ~FindFeatureReferenceCAxisMisorientations() override;

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
   * @brief Setter property for CellPhasesArrayPath
   */
  void setCellPhasesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CellPhasesArrayPath
   * @return Value of CellPhasesArrayPath
   */
  DataArrayPath getCellPhasesArrayPath() const;

  Q_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)

  /**
   * @brief Setter property for AvgCAxesArrayPath
   */
  void setAvgCAxesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for AvgCAxesArrayPath
   * @return Value of AvgCAxesArrayPath
   */
  DataArrayPath getAvgCAxesArrayPath() const;

  Q_PROPERTY(DataArrayPath AvgCAxesArrayPath READ getAvgCAxesArrayPath WRITE setAvgCAxesArrayPath)

  /**
   * @brief Setter property for QuatsArrayPath
   */
  void setQuatsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for QuatsArrayPath
   * @return Value of QuatsArrayPath
   */
  DataArrayPath getQuatsArrayPath() const;

  Q_PROPERTY(DataArrayPath QuatsArrayPath READ getQuatsArrayPath WRITE setQuatsArrayPath)

  /**
   * @brief Setter property for FeatureAvgCAxisMisorientationsArrayName
   */
  void setFeatureAvgCAxisMisorientationsArrayName(const QString& value);
  /**
   * @brief Getter property for FeatureAvgCAxisMisorientationsArrayName
   * @return Value of FeatureAvgCAxisMisorientationsArrayName
   */
  QString getFeatureAvgCAxisMisorientationsArrayName() const;

  Q_PROPERTY(QString FeatureAvgCAxisMisorientationsArrayName READ getFeatureAvgCAxisMisorientationsArrayName WRITE setFeatureAvgCAxisMisorientationsArrayName)

  /**
   * @brief Setter property for FeatureStdevCAxisMisorientationsArrayName
   */
  void setFeatureStdevCAxisMisorientationsArrayName(const QString& value);
  /**
   * @brief Getter property for FeatureStdevCAxisMisorientationsArrayName
   * @return Value of FeatureStdevCAxisMisorientationsArrayName
   */
  QString getFeatureStdevCAxisMisorientationsArrayName() const;

  Q_PROPERTY(QString FeatureStdevCAxisMisorientationsArrayName READ getFeatureStdevCAxisMisorientationsArrayName WRITE setFeatureStdevCAxisMisorientationsArrayName)

  /**
   * @brief Setter property for FeatureReferenceCAxisMisorientationsArrayName
   */
  void setFeatureReferenceCAxisMisorientationsArrayName(const QString& value);
  /**
   * @brief Getter property for FeatureReferenceCAxisMisorientationsArrayName
   * @return Value of FeatureReferenceCAxisMisorientationsArrayName
   */
  QString getFeatureReferenceCAxisMisorientationsArrayName() const;

  Q_PROPERTY(QString FeatureReferenceCAxisMisorientationsArrayName READ getFeatureReferenceCAxisMisorientationsArrayName WRITE setFeatureReferenceCAxisMisorientationsArrayName)

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
  FindFeatureReferenceCAxisMisorientations();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  std::weak_ptr<DataArray<int32_t>> m_FeatureIdsPtr;
  int32_t* m_FeatureIds = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_CellPhasesPtr;
  int32_t* m_CellPhases = nullptr;
  std::weak_ptr<DataArray<float>> m_QuatsPtr;
  float* m_Quats = nullptr;
  std::weak_ptr<DataArray<float>> m_AvgCAxesPtr;
  float* m_AvgCAxes = nullptr;
  std::weak_ptr<DataArray<float>> m_FeatureReferenceCAxisMisorientationsPtr;
  float* m_FeatureReferenceCAxisMisorientations = nullptr;
  std::weak_ptr<DataArray<float>> m_FeatureAvgCAxisMisorientationsPtr;
  float* m_FeatureAvgCAxisMisorientations = nullptr;
  std::weak_ptr<DataArray<float>> m_FeatureStdevCAxisMisorientationsPtr;
  float* m_FeatureStdevCAxisMisorientations = nullptr;

  DataArrayPath m_FeatureIdsArrayPath = {};
  DataArrayPath m_CellPhasesArrayPath = {};
  DataArrayPath m_AvgCAxesArrayPath = {};
  DataArrayPath m_QuatsArrayPath = {};
  QString m_FeatureAvgCAxisMisorientationsArrayName = {};
  QString m_FeatureStdevCAxisMisorientationsArrayName = {};
  QString m_FeatureReferenceCAxisMisorientationsArrayName = {};

  QVector<LaueOps::Pointer> m_OrientationOps;

public:
  FindFeatureReferenceCAxisMisorientations(const FindFeatureReferenceCAxisMisorientations&) = delete; // Copy Constructor Not Implemented
  FindFeatureReferenceCAxisMisorientations(FindFeatureReferenceCAxisMisorientations&&) = delete;      // Move Constructor Not Implemented
  FindFeatureReferenceCAxisMisorientations& operator=(const FindFeatureReferenceCAxisMisorientations&) = delete; // Copy Assignment Not Implemented
  FindFeatureReferenceCAxisMisorientations& operator=(FindFeatureReferenceCAxisMisorientations&&) = delete;      // Move Assignment Not Implemented
};

