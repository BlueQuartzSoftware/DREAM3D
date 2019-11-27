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
#include "SIMPLib/FilterParameters/DynamicTableData.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "EMMPM/EMMPMLib/Core/EMMPM_Data.h"
#include "EMMPMLib/Core/EMMPM_Constants.h"

#include "EMMPM/EMMPMDLLExport.h"

class EMMPMFilterMessageHandler;

/**
 * @brief The EMMPMFilter class. See [Filter documentation](@ref emmpmfilter) for details.
 */
class EMMPM_EXPORT EMMPMFilter : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(EMMPMFilter SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(EMMPMFilter)
  PYB11_FILTER_NEW_MACRO(EMMPMFilter)
  PYB11_FILTER_PARAMETER(DataArrayPath, InputDataArrayPath)
  PYB11_FILTER_PARAMETER(bool, UseOneBasedValues)
  PYB11_FILTER_PARAMETER(int, NumClasses)
  PYB11_FILTER_PARAMETER(float, ExchangeEnergy)
  PYB11_FILTER_PARAMETER(int, HistogramLoops)
  PYB11_FILTER_PARAMETER(int, SegmentationLoops)
  PYB11_FILTER_PARAMETER(DynamicTableData, EMMPMTableData)
  PYB11_FILTER_PARAMETER(bool, UseSimulatedAnnealing)
  PYB11_FILTER_PARAMETER(bool, UseGradientPenalty)
  PYB11_FILTER_PARAMETER(double, GradientBetaE)
  PYB11_FILTER_PARAMETER(bool, UseCurvaturePenalty)
  PYB11_FILTER_PARAMETER(double, CurvatureBetaC)
  PYB11_FILTER_PARAMETER(double, CurvatureRMax)
  PYB11_FILTER_PARAMETER(int, CurvatureEMLoopDelay)
  PYB11_FILTER_PARAMETER(DataArrayPath, OutputDataArrayPath)
  PYB11_PROPERTY(DataArrayPath InputDataArrayPath READ getInputDataArrayPath WRITE setInputDataArrayPath)
  PYB11_PROPERTY(bool UseOneBasedValues READ getUseOneBasedValues WRITE setUseOneBasedValues)
  PYB11_PROPERTY(int NumClasses READ getNumClasses WRITE setNumClasses)
  PYB11_PROPERTY(float ExchangeEnergy READ getExchangeEnergy WRITE setExchangeEnergy)
  PYB11_PROPERTY(int HistogramLoops READ getHistogramLoops WRITE setHistogramLoops)
  PYB11_PROPERTY(int SegmentationLoops READ getSegmentationLoops WRITE setSegmentationLoops)
  PYB11_PROPERTY(DynamicTableData EMMPMTableData READ getEMMPMTableData WRITE setEMMPMTableData)
  PYB11_PROPERTY(bool UseSimulatedAnnealing READ getUseSimulatedAnnealing WRITE setUseSimulatedAnnealing)
  PYB11_PROPERTY(bool UseGradientPenalty READ getUseGradientPenalty WRITE setUseGradientPenalty)
  PYB11_PROPERTY(double GradientBetaE READ getGradientBetaE WRITE setGradientBetaE)
  PYB11_PROPERTY(bool UseCurvaturePenalty READ getUseCurvaturePenalty WRITE setUseCurvaturePenalty)
  PYB11_PROPERTY(double CurvatureBetaC READ getCurvatureBetaC WRITE setCurvatureBetaC)
  PYB11_PROPERTY(double CurvatureRMax READ getCurvatureRMax WRITE setCurvatureRMax)
  PYB11_PROPERTY(int CurvatureEMLoopDelay READ getCurvatureEMLoopDelay WRITE setCurvatureEMLoopDelay)
  PYB11_PROPERTY(DataArrayPath OutputDataArrayPath READ getOutputDataArrayPath WRITE setOutputDataArrayPath)
#endif

public:
  using Self = EMMPMFilter;
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
   * @brief Returns the name of the class for EMMPMFilter
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for EMMPMFilter
   */
  static QString ClassName();

  ~EMMPMFilter() override;

  friend EMMPMFilterMessageHandler;

  /**
   * @brief Setter property for InputDataArrayPath
   */
  void setInputDataArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for InputDataArrayPath
   * @return Value of InputDataArrayPath
   */
  DataArrayPath getInputDataArrayPath() const;

  Q_PROPERTY(DataArrayPath InputDataArrayPath READ getInputDataArrayPath WRITE setInputDataArrayPath)

  /**
   * @brief Setter property for UseOneBasedValues
   */
  void setUseOneBasedValues(bool value);
  /**
   * @brief Getter property for UseOneBasedValues
   * @return Value of UseOneBasedValues
   */
  bool getUseOneBasedValues() const;

  Q_PROPERTY(bool UseOneBasedValues READ getUseOneBasedValues WRITE setUseOneBasedValues)

  /**
   * @brief Setter property for NumClasses
   */
  void setNumClasses(int value);
  /**
   * @brief Getter property for NumClasses
   * @return Value of NumClasses
   */
  int getNumClasses() const;

  Q_PROPERTY(int NumClasses READ getNumClasses WRITE setNumClasses)

  /**
   * @brief Setter property for ExchangeEnergy
   */
  void setExchangeEnergy(float value);
  /**
   * @brief Getter property for ExchangeEnergy
   * @return Value of ExchangeEnergy
   */
  float getExchangeEnergy() const;

  Q_PROPERTY(float ExchangeEnergy READ getExchangeEnergy WRITE setExchangeEnergy)

  /**
   * @brief Setter property for HistogramLoops
   */
  void setHistogramLoops(int value);
  /**
   * @brief Getter property for HistogramLoops
   * @return Value of HistogramLoops
   */
  int getHistogramLoops() const;

  Q_PROPERTY(int HistogramLoops READ getHistogramLoops WRITE setHistogramLoops)

  /**
   * @brief Setter property for SegmentationLoops
   */
  void setSegmentationLoops(int value);
  /**
   * @brief Getter property for SegmentationLoops
   * @return Value of SegmentationLoops
   */
  int getSegmentationLoops() const;

  Q_PROPERTY(int SegmentationLoops READ getSegmentationLoops WRITE setSegmentationLoops)

  /**
   * @brief Setter property for EMMPMTableData
   */
  void setEMMPMTableData(const DynamicTableData& value);
  /**
   * @brief Getter property for EMMPMTableData
   * @return Value of EMMPMTableData
   */
  DynamicTableData getEMMPMTableData() const;

  Q_PROPERTY(DynamicTableData EMMPMTableData READ getEMMPMTableData WRITE setEMMPMTableData)

  /**
   * @brief Setter property for UseSimulatedAnnealing
   */
  void setUseSimulatedAnnealing(bool value);
  /**
   * @brief Getter property for UseSimulatedAnnealing
   * @return Value of UseSimulatedAnnealing
   */
  bool getUseSimulatedAnnealing() const;

  Q_PROPERTY(bool UseSimulatedAnnealing READ getUseSimulatedAnnealing WRITE setUseSimulatedAnnealing)

  /**
   * @brief Setter property for UseGradientPenalty
   */
  void setUseGradientPenalty(bool value);
  /**
   * @brief Getter property for UseGradientPenalty
   * @return Value of UseGradientPenalty
   */
  bool getUseGradientPenalty() const;

  Q_PROPERTY(bool UseGradientPenalty READ getUseGradientPenalty WRITE setUseGradientPenalty)

  /**
   * @brief Setter property for GradientBetaE
   */
  void setGradientBetaE(double value);
  /**
   * @brief Getter property for GradientBetaE
   * @return Value of GradientBetaE
   */
  double getGradientBetaE() const;

  Q_PROPERTY(double GradientBetaE READ getGradientBetaE WRITE setGradientBetaE)

  /**
   * @brief Setter property for UseCurvaturePenalty
   */
  void setUseCurvaturePenalty(bool value);
  /**
   * @brief Getter property for UseCurvaturePenalty
   * @return Value of UseCurvaturePenalty
   */
  bool getUseCurvaturePenalty() const;

  Q_PROPERTY(bool UseCurvaturePenalty READ getUseCurvaturePenalty WRITE setUseCurvaturePenalty)

  /**
   * @brief Setter property for CurvatureBetaC
   */
  void setCurvatureBetaC(double value);
  /**
   * @brief Getter property for CurvatureBetaC
   * @return Value of CurvatureBetaC
   */
  double getCurvatureBetaC() const;

  Q_PROPERTY(double CurvatureBetaC READ getCurvatureBetaC WRITE setCurvatureBetaC)

  /**
   * @brief Setter property for CurvatureRMax
   */
  void setCurvatureRMax(double value);
  /**
   * @brief Getter property for CurvatureRMax
   * @return Value of CurvatureRMax
   */
  double getCurvatureRMax() const;

  Q_PROPERTY(double CurvatureRMax READ getCurvatureRMax WRITE setCurvatureRMax)

  /**
   * @brief Setter property for CurvatureEMLoopDelay
   */
  void setCurvatureEMLoopDelay(int value);
  /**
   * @brief Getter property for CurvatureEMLoopDelay
   * @return Value of CurvatureEMLoopDelay
   */
  int getCurvatureEMLoopDelay() const;

  Q_PROPERTY(int CurvatureEMLoopDelay READ getCurvatureEMLoopDelay WRITE setCurvatureEMLoopDelay)

  /**
   * @brief Setter property for OutputDataArrayPath
   */
  void setOutputDataArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for OutputDataArrayPath
   * @return Value of OutputDataArrayPath
   */
  DataArrayPath getOutputDataArrayPath() const;

  Q_PROPERTY(DataArrayPath OutputDataArrayPath READ getOutputDataArrayPath WRITE setOutputDataArrayPath)

  /**
   * @brief Setter property for EmmpmInitType
   */
  void setEmmpmInitType(const EMMPM_InitializationType& value);
  /**
   * @brief Getter property for EmmpmInitType
   * @return Value of EmmpmInitType
   */
  EMMPM_InitializationType getEmmpmInitType() const;

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
  EMMPMFilter();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

  /**
   * @brief segment Performs the EMMPM segmentation routine
   * @param initType Enumeration of EMMPM initialization types
   */
  virtual void segment(EMMPM_InitializationType initType);

  /**
   * @brief getPreviousMu
   * @return
   */
  std::vector<float> getPreviousMu();

  /**
   * @brief setPreviousMu
   * @param prevMu
   */
  void setPreviousMu(std::vector<float> prevMu);

  /**
   * @brief getPreviousSigma
   * @return
   */
  std::vector<float> getPreviousSigma();

  /**
   * @brief setPreviousSigma
   * @param prevSigma
   */
  void setPreviousSigma(std::vector<float> prevSigma);

  /**
   * @brief handleEmmpmMessage
   * @param msg
   */
  virtual void handleEmmpmMessage(const AbstractMessage::Pointer &msg);

private:
  std::weak_ptr<DataArray<uint8_t>> m_InputImagePtr;
  uint8_t* m_InputImage = nullptr;
  std::weak_ptr<DataArray<uint8_t>> m_OutputImagePtr;
  uint8_t* m_OutputImage = nullptr;

  DataArrayPath m_InputDataArrayPath = {};
  bool m_UseOneBasedValues = {};
  int m_NumClasses = {};
  float m_ExchangeEnergy = {};
  int m_HistogramLoops = {};
  int m_SegmentationLoops = {};
  DynamicTableData m_EMMPMTableData = {};
  bool m_UseSimulatedAnnealing = {};
  bool m_UseGradientPenalty = {};
  double m_GradientBetaE = {};
  bool m_UseCurvaturePenalty = {};
  double m_CurvatureBetaC = {};
  double m_CurvatureRMax = {};
  int m_CurvatureEMLoopDelay = {};
  DataArrayPath m_OutputDataArrayPath = {};
  EMMPM_InitializationType m_EmmpmInitType = {};

  std::vector<float> m_PreviousMu;
  std::vector<float> m_PreviousSigma;

  EMMPM_Data::Pointer m_Data;

public:
  EMMPMFilter(const EMMPMFilter&) = delete;            // Copy Constructor Not Implemented
  EMMPMFilter(EMMPMFilter&&) = delete;                 // Move Constructor Not Implemented
  EMMPMFilter& operator=(const EMMPMFilter&) = delete; // Copy Assignment Not Implemented
  EMMPMFilter& operator=(EMMPMFilter&&) = delete;      // Move Assignment Not Implemented
};

