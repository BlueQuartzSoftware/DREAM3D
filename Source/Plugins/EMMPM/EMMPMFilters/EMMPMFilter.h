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

#ifndef _emmpmfilter_h_
#define _emmpmfilter_h_

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/FilterParameters/DynamicTableData.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "EMMPM/EMMPMLib/Core/EMMPM_Data.h"
#include "EMMPMLib/Core/EMMPM_Constants.h"

/**
 * @brief The EMMPMFilter class. See [Filter documentation](@ref emmpmfilter) for details.
 */
class EMMPMFilter : public AbstractFilter
{
  Q_OBJECT

public:
  SIMPL_SHARED_POINTERS(EMMPMFilter)
  SIMPL_STATIC_NEW_MACRO(EMMPMFilter)
  SIMPL_TYPE_MACRO_SUPER(EMMPMFilter, AbstractFilter)

  virtual ~EMMPMFilter();

  SIMPL_FILTER_PARAMETER(DataArrayPath, InputDataArrayPath)
  Q_PROPERTY(DataArrayPath InputDataArrayPath READ getInputDataArrayPath WRITE setInputDataArrayPath)

  SIMPL_FILTER_PARAMETER(bool, UseOneBasedValues)
  Q_PROPERTY(bool UseOneBasedValues READ getUseOneBasedValues WRITE setUseOneBasedValues)

  SIMPL_FILTER_PARAMETER(int, NumClasses)
  Q_PROPERTY(int NumClasses READ getNumClasses WRITE setNumClasses)

  SIMPL_FILTER_PARAMETER(float, ExchangeEnergy)
  Q_PROPERTY(float ExchangeEnergy READ getExchangeEnergy WRITE setExchangeEnergy)

  SIMPL_FILTER_PARAMETER(int, HistogramLoops)
  Q_PROPERTY(int HistogramLoops READ getHistogramLoops WRITE setHistogramLoops)

  SIMPL_FILTER_PARAMETER(int, SegmentationLoops)
  Q_PROPERTY(int SegmentationLoops READ getSegmentationLoops WRITE setSegmentationLoops)

  SIMPL_FILTER_PARAMETER(DynamicTableData, EMMPMTableData)
  Q_PROPERTY(DynamicTableData EMMPMTableData READ getEMMPMTableData WRITE setEMMPMTableData)

  SIMPL_FILTER_PARAMETER(bool, UseSimulatedAnnealing)
  Q_PROPERTY(bool UseSimulatedAnnealing READ getUseSimulatedAnnealing WRITE setUseSimulatedAnnealing)

  SIMPL_FILTER_PARAMETER(bool, UseGradientPenalty)
  Q_PROPERTY(bool UseGradientPenalty READ getUseGradientPenalty WRITE setUseGradientPenalty)

  SIMPL_FILTER_PARAMETER(double, GradientBetaE)
  Q_PROPERTY(double GradientBetaE READ getGradientBetaE WRITE setGradientBetaE)

  SIMPL_FILTER_PARAMETER(bool, UseCurvaturePenalty)
  Q_PROPERTY(bool UseCurvaturePenalty READ getUseCurvaturePenalty WRITE setUseCurvaturePenalty)

  SIMPL_FILTER_PARAMETER(double, CurvatureBetaC)
  Q_PROPERTY(double CurvatureBetaC READ getCurvatureBetaC WRITE setCurvatureBetaC)

  SIMPL_FILTER_PARAMETER(double, CurvatureRMax)
  Q_PROPERTY(double CurvatureRMax READ getCurvatureRMax WRITE setCurvatureRMax)

  SIMPL_FILTER_PARAMETER(int, CurvatureEMLoopDelay)
  Q_PROPERTY(int CurvatureEMLoopDelay READ getCurvatureEMLoopDelay WRITE setCurvatureEMLoopDelay)

  SIMPL_FILTER_PARAMETER(DataArrayPath, OutputDataArrayPath)
  Q_PROPERTY(DataArrayPath OutputDataArrayPath READ getOutputDataArrayPath WRITE setOutputDataArrayPath)

  SIMPL_INSTANCE_PROPERTY(EMMPM_InitializationType, EmmpmInitType)

  /**
   * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
   */
  virtual const QString getCompiledLibraryName();

  /**
   * @brief getBrandingString Returns the branding string for the filter, which is a tag
   * used to denote the filter's association with specific plugins
   * @return Branding string
  */
  virtual const QString getBrandingString();

  /**
   * @brief getFilterVersion Returns a version string for this filter. Default
   * value is an empty string.
   * @return
   */
  virtual const QString getFilterVersion();

  /**
   * @brief newFilterInstance Reimplemented from @see AbstractFilter class
   */
  virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);

  /**
   * @brief getGroupName Reimplemented from @see AbstractFilter class
   */
  virtual const QString getGroupName();

  /**
   * @brief getSubGroupName Reimplemented from @see AbstractFilter class
   */
  virtual const QString getSubGroupName();

  /**
   * @brief getHumanLabel Reimplemented from @see AbstractFilter class
   */
  virtual const QString getHumanLabel();

  /**
   * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
   */
  virtual void setupFilterParameters();

  /**
   * @brief readFilterParameters Reimplemented from @see AbstractFilter class
   */
  virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

  /**
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  virtual void execute();

  /**
  * @brief preflight Reimplemented from @see AbstractFilter class
  */
  virtual void preflight();

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

private:
  DEFINE_DATAARRAY_VARIABLE(uint8_t, InputImage)
  DEFINE_DATAARRAY_VARIABLE(uint8_t, OutputImage)

  std::vector<float> m_PreviousMu;
  std::vector<float> m_PreviousSigma;

  EMMPM_Data::Pointer m_Data;

  EMMPMFilter(const EMMPMFilter&);    // Copy Constructor Not Implemented
  void operator=(const EMMPMFilter&); // Operator '=' Not Implemented
};

#endif /* _EMMPMFilter_H_ */
