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
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @brief The FindSlipTransmissionMetrics class. See [Filter documentation](@ref findsliptransmissionmetrics) for details.
 */
class OrientationAnalysis_EXPORT FindSlipTransmissionMetrics : public AbstractFilter
{
  Q_OBJECT
    PYB11_CREATE_BINDINGS(FindSlipTransmissionMetrics SUPERCLASS AbstractFilter)
    PYB11_PROPERTY(DataArrayPath NeighborListArrayPath READ getNeighborListArrayPath WRITE setNeighborListArrayPath)
    PYB11_PROPERTY(QString F1ListArrayName READ getF1ListArrayName WRITE setF1ListArrayName)
    PYB11_PROPERTY(QString F1sptListArrayName READ getF1sptListArrayName WRITE setF1sptListArrayName)
    PYB11_PROPERTY(QString F7ListArrayName READ getF7ListArrayName WRITE setF7ListArrayName)
    PYB11_PROPERTY(QString mPrimeListArrayName READ getmPrimeListArrayName WRITE setmPrimeListArrayName)
    PYB11_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)
    PYB11_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)
    PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
public:
  SIMPL_SHARED_POINTERS(FindSlipTransmissionMetrics)
  SIMPL_FILTER_NEW_MACRO(FindSlipTransmissionMetrics)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(FindSlipTransmissionMetrics, AbstractFilter)

  ~FindSlipTransmissionMetrics() override;

  SIMPL_FILTER_PARAMETER(DataArrayPath, NeighborListArrayPath)
  Q_PROPERTY(DataArrayPath NeighborListArrayPath READ getNeighborListArrayPath WRITE setNeighborListArrayPath)

  SIMPL_FILTER_PARAMETER(QString, F1ListArrayName)
  Q_PROPERTY(QString F1ListArrayName READ getF1ListArrayName WRITE setF1ListArrayName)

  SIMPL_FILTER_PARAMETER(QString, F1sptListArrayName)
  Q_PROPERTY(QString F1sptListArrayName READ getF1sptListArrayName WRITE setF1sptListArrayName)

  SIMPL_FILTER_PARAMETER(QString, F7ListArrayName)
  Q_PROPERTY(QString F7ListArrayName READ getF7ListArrayName WRITE setF7ListArrayName)

  SIMPL_FILTER_PARAMETER(QString, mPrimeListArrayName)
  Q_PROPERTY(QString mPrimeListArrayName READ getmPrimeListArrayName WRITE setmPrimeListArrayName)

  SIMPL_FILTER_PARAMETER(DataArrayPath, AvgQuatsArrayPath)
  Q_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
  Q_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)

  SIMPL_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  Q_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)

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

  DEFINE_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
  DEFINE_DATAARRAY_VARIABLE(float, AvgQuats)
  DEFINE_DATAARRAY_VARIABLE(uint32_t, CrystalStructures)

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

