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
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "OrientationLib/LaueOps/LaueOps.h"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @brief The FindSlipTransmissionMetrics class. See [Filter documentation](@ref findsliptransmissionmetrics) for details.
 */
class OrientationAnalysis_EXPORT FindSlipTransmissionMetrics : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(FindSlipTransmissionMetrics SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(FindSlipTransmissionMetrics)
  PYB11_FILTER_NEW_MACRO(FindSlipTransmissionMetrics)
  PYB11_FILTER_PARAMETER(DataArrayPath, NeighborListArrayPath)
  PYB11_FILTER_PARAMETER(QString, F1ListArrayName)
  PYB11_FILTER_PARAMETER(QString, F1sptListArrayName)
  PYB11_FILTER_PARAMETER(QString, F7ListArrayName)
  PYB11_FILTER_PARAMETER(QString, mPrimeListArrayName)
  PYB11_FILTER_PARAMETER(DataArrayPath, AvgQuatsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  PYB11_PROPERTY(DataArrayPath NeighborListArrayPath READ getNeighborListArrayPath WRITE setNeighborListArrayPath)
  PYB11_PROPERTY(QString F1ListArrayName READ getF1ListArrayName WRITE setF1ListArrayName)
  PYB11_PROPERTY(QString F1sptListArrayName READ getF1sptListArrayName WRITE setF1sptListArrayName)
  PYB11_PROPERTY(QString F7ListArrayName READ getF7ListArrayName WRITE setF7ListArrayName)
  PYB11_PROPERTY(QString mPrimeListArrayName READ getmPrimeListArrayName WRITE setmPrimeListArrayName)
  PYB11_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)
  PYB11_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
#endif

public:
  using Self = FindSlipTransmissionMetrics;
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
   * @brief Returns the name of the class for FindSlipTransmissionMetrics
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FindSlipTransmissionMetrics
   */
  static QString ClassName();

  ~FindSlipTransmissionMetrics() override;

  /**
   * @brief Setter property for NeighborListArrayPath
   */
  void setNeighborListArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for NeighborListArrayPath
   * @return Value of NeighborListArrayPath
   */
  DataArrayPath getNeighborListArrayPath() const;

  Q_PROPERTY(DataArrayPath NeighborListArrayPath READ getNeighborListArrayPath WRITE setNeighborListArrayPath)

  /**
   * @brief Setter property for F1ListArrayName
   */
  void setF1ListArrayName(const QString& value);
  /**
   * @brief Getter property for F1ListArrayName
   * @return Value of F1ListArrayName
   */
  QString getF1ListArrayName() const;

  Q_PROPERTY(QString F1ListArrayName READ getF1ListArrayName WRITE setF1ListArrayName)

  /**
   * @brief Setter property for F1sptListArrayName
   */
  void setF1sptListArrayName(const QString& value);
  /**
   * @brief Getter property for F1sptListArrayName
   * @return Value of F1sptListArrayName
   */
  QString getF1sptListArrayName() const;

  Q_PROPERTY(QString F1sptListArrayName READ getF1sptListArrayName WRITE setF1sptListArrayName)

  /**
   * @brief Setter property for F7ListArrayName
   */
  void setF7ListArrayName(const QString& value);
  /**
   * @brief Getter property for F7ListArrayName
   * @return Value of F7ListArrayName
   */
  QString getF7ListArrayName() const;

  Q_PROPERTY(QString F7ListArrayName READ getF7ListArrayName WRITE setF7ListArrayName)

  /**
   * @brief Setter property for mPrimeListArrayName
   */
  void setmPrimeListArrayName(const QString& value);
  /**
   * @brief Getter property for mPrimeListArrayName
   * @return Value of mPrimeListArrayName
   */
  QString getmPrimeListArrayName() const;

  Q_PROPERTY(QString mPrimeListArrayName READ getmPrimeListArrayName WRITE setmPrimeListArrayName)

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
   * @brief Setter property for FeaturePhasesArrayPath
   */
  void setFeaturePhasesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for FeaturePhasesArrayPath
   * @return Value of FeaturePhasesArrayPath
   */
  DataArrayPath getFeaturePhasesArrayPath() const;

  Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

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
  FindSlipTransmissionMetrics();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  std::weak_ptr<DataArray<int32_t>> m_FeaturePhasesPtr;
  int32_t* m_FeaturePhases = nullptr;
  std::weak_ptr<DataArray<float>> m_AvgQuatsPtr;
  float* m_AvgQuats = nullptr;
  std::weak_ptr<DataArray<uint32_t>> m_CrystalStructuresPtr;
  uint32_t* m_CrystalStructures = nullptr;

  DataArrayPath m_NeighborListArrayPath = {};
  QString m_F1ListArrayName = {};
  QString m_F1sptListArrayName = {};
  QString m_F7ListArrayName = {};
  QString m_mPrimeListArrayName = {};
  DataArrayPath m_AvgQuatsArrayPath = {};
  DataArrayPath m_FeaturePhasesArrayPath = {};
  DataArrayPath m_CrystalStructuresArrayPath = {};

  QVector<LaueOps::Pointer> m_OrientationOps;

  NeighborList<float>::WeakPointer m_F1List;
  NeighborList<float>::WeakPointer m_F1sptList;
  NeighborList<float>::WeakPointer m_F7List;
  NeighborList<float>::WeakPointer m_mPrimeList;
  NeighborList<int32_t>::WeakPointer m_NeighborList;

public:
  FindSlipTransmissionMetrics(const FindSlipTransmissionMetrics&) = delete; // Copy Constructor Not Implemented
  FindSlipTransmissionMetrics(FindSlipTransmissionMetrics&&) = delete;      // Move Constructor Not Implemented
  FindSlipTransmissionMetrics& operator=(const FindSlipTransmissionMetrics&) = delete; // Copy Assignment Not Implemented
  FindSlipTransmissionMetrics& operator=(FindSlipTransmissionMetrics&&) = delete;      // Move Assignment Not Implemented
};

