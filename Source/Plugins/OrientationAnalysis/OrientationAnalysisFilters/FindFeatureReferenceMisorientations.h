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
 * @brief The FindFeatureReferenceMisorientations class. See [Filter documentation](@ref findfeaturereferencemisorientations) for details.
 */
class OrientationAnalysis_EXPORT FindFeatureReferenceMisorientations : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(FindFeatureReferenceMisorientations SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(FindFeatureReferenceMisorientations)
  PYB11_FILTER_NEW_MACRO(FindFeatureReferenceMisorientations)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CellPhasesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, QuatsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, AvgQuatsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, GBEuclideanDistancesArrayPath)
  PYB11_FILTER_PARAMETER(QString, FeatureAvgMisorientationsArrayName)
  PYB11_FILTER_PARAMETER(QString, FeatureReferenceMisorientationsArrayName)
  PYB11_FILTER_PARAMETER(int, ReferenceOrientation)
  PYB11_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)
  PYB11_PROPERTY(DataArrayPath CellPhasesArrayPath READ getCellPhasesArrayPath WRITE setCellPhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
  PYB11_PROPERTY(DataArrayPath QuatsArrayPath READ getQuatsArrayPath WRITE setQuatsArrayPath)
  PYB11_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)
  PYB11_PROPERTY(DataArrayPath GBEuclideanDistancesArrayPath READ getGBEuclideanDistancesArrayPath WRITE setGBEuclideanDistancesArrayPath)
  PYB11_PROPERTY(QString FeatureAvgMisorientationsArrayName READ getFeatureAvgMisorientationsArrayName WRITE setFeatureAvgMisorientationsArrayName)
  PYB11_PROPERTY(QString FeatureReferenceMisorientationsArrayName READ getFeatureReferenceMisorientationsArrayName WRITE setFeatureReferenceMisorientationsArrayName)
  PYB11_PROPERTY(int ReferenceOrientation READ getReferenceOrientation WRITE setReferenceOrientation)
#endif

public:
  using Self = FindFeatureReferenceMisorientations;
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
   * @brief Returns the name of the class for FindFeatureReferenceMisorientations
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FindFeatureReferenceMisorientations
   */
  static QString ClassName();

  ~FindFeatureReferenceMisorientations() override;

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
   * @brief Setter property for CrystalStructuresArrayPath
   */
  void setCrystalStructuresArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CrystalStructuresArrayPath
   * @return Value of CrystalStructuresArrayPath
   */
  DataArrayPath getCrystalStructuresArrayPath() const;

  Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

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
   * @brief Setter property for AvgQuatsArrayPath
   */
  void setAvgQuatsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for AvgQuatsArrayPath
   * @return Value of AvgQuatsArrayPath
   */
  DataArrayPath getAvgQuatsArrayPath() const;

  Q_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)

  /**
   * @brief Setter property for GBEuclideanDistancesArrayPath
   */
  void setGBEuclideanDistancesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for GBEuclideanDistancesArrayPath
   * @return Value of GBEuclideanDistancesArrayPath
   */
  DataArrayPath getGBEuclideanDistancesArrayPath() const;

  Q_PROPERTY(DataArrayPath GBEuclideanDistancesArrayPath READ getGBEuclideanDistancesArrayPath WRITE setGBEuclideanDistancesArrayPath)

  /**
   * @brief Setter property for FeatureAvgMisorientationsArrayName
   */
  void setFeatureAvgMisorientationsArrayName(const QString& value);
  /**
   * @brief Getter property for FeatureAvgMisorientationsArrayName
   * @return Value of FeatureAvgMisorientationsArrayName
   */
  QString getFeatureAvgMisorientationsArrayName() const;

  Q_PROPERTY(QString FeatureAvgMisorientationsArrayName READ getFeatureAvgMisorientationsArrayName WRITE setFeatureAvgMisorientationsArrayName)

  /**
   * @brief Setter property for FeatureReferenceMisorientationsArrayName
   */
  void setFeatureReferenceMisorientationsArrayName(const QString& value);
  /**
   * @brief Getter property for FeatureReferenceMisorientationsArrayName
   * @return Value of FeatureReferenceMisorientationsArrayName
   */
  QString getFeatureReferenceMisorientationsArrayName() const;

  Q_PROPERTY(QString FeatureReferenceMisorientationsArrayName READ getFeatureReferenceMisorientationsArrayName WRITE setFeatureReferenceMisorientationsArrayName)

  /**
   * @brief Setter property for ReferenceOrientation
   */
  void setReferenceOrientation(int value);
  /**
   * @brief Getter property for ReferenceOrientation
   * @return Value of ReferenceOrientation
   */
  int getReferenceOrientation() const;

  Q_PROPERTY(int ReferenceOrientation READ getReferenceOrientation WRITE setReferenceOrientation)

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
  FindFeatureReferenceMisorientations();
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
  std::weak_ptr<DataArray<float>> m_GBEuclideanDistancesPtr;
  float* m_GBEuclideanDistances = nullptr;
  std::weak_ptr<DataArray<float>> m_AvgQuatsPtr;
  float* m_AvgQuats = nullptr;
  std::weak_ptr<DataArray<float>> m_QuatsPtr;
  float* m_Quats = nullptr;
  std::weak_ptr<DataArray<uint32_t>> m_CrystalStructuresPtr;
  uint32_t* m_CrystalStructures = nullptr;
  std::weak_ptr<DataArray<float>> m_FeatureReferenceMisorientationsPtr;
  float* m_FeatureReferenceMisorientations = nullptr;
  std::weak_ptr<DataArray<float>> m_FeatureAvgMisorientationsPtr;
  float* m_FeatureAvgMisorientations = nullptr;

  DataArrayPath m_FeatureIdsArrayPath = {};
  DataArrayPath m_CellPhasesArrayPath = {};
  DataArrayPath m_CrystalStructuresArrayPath = {};
  DataArrayPath m_QuatsArrayPath = {};
  DataArrayPath m_AvgQuatsArrayPath = {};
  DataArrayPath m_GBEuclideanDistancesArrayPath = {};
  QString m_FeatureAvgMisorientationsArrayName = {};
  QString m_FeatureReferenceMisorientationsArrayName = {};
  int m_ReferenceOrientation = {};

  QVector<LaueOps::Pointer> m_OrientationOps;

public:
  FindFeatureReferenceMisorientations(const FindFeatureReferenceMisorientations&) = delete; // Copy Constructor Not Implemented
  FindFeatureReferenceMisorientations(FindFeatureReferenceMisorientations&&) = delete;      // Move Constructor Not Implemented
  FindFeatureReferenceMisorientations& operator=(const FindFeatureReferenceMisorientations&) = delete; // Copy Assignment Not Implemented
  FindFeatureReferenceMisorientations& operator=(FindFeatureReferenceMisorientations&&) = delete;      // Move Assignment Not Implemented
};

