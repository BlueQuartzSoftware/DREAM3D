/*
 * ###################################################################
 * Copyright (c) 2016, Marc De Graef/Carnegie Mellon University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *     - Redistributions of source code must retain the above copyright notice, this list
 *        of conditions and the following disclaimer.
 *     - Redistributions in binary form must reproduce the above copyright notice, this
 *        list of conditions and the following disclaimer in the documentation and/or
 *        other materials provided with the distribution.
 *     - Neither the names of Marc De Graef, Carnegie Mellon University nor the names
 *        of its contributors may be used to endorse or promote products derived from
 *        this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ###################################################################
 */

#ifndef _emsoftso3sampler_h_
#define _emsoftso3sampler_h_

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/SIMPLib.h"

#include "OrientationLib/OrientationLib.h"
#include "OrientationLib/OrientationMath/OrientationArray.hpp"

/**
 * @brief The EMsoftSO3Sampler class. See [Filter documentation](@ref emsoftso3sampler) for details.
 */
class EMsoftSO3Sampler : public AbstractFilter
{
  Q_OBJECT

public:
  SIMPL_SHARED_POINTERS(EMsoftSO3Sampler)
  SIMPL_STATIC_NEW_MACRO(EMsoftSO3Sampler)
  SIMPL_TYPE_MACRO_SUPER(EMsoftSO3Sampler, AbstractFilter)

  virtual ~EMsoftSO3Sampler();

  /**
   * EMsoftSO3Sampler filter parameters
   */

  SIMPL_FILTER_PARAMETER(int, sampleModeSelector)
  Q_PROPERTY(int sampleModeSelector READ getsampleModeSelector WRITE setsampleModeSelector)

  SIMPL_FILTER_PARAMETER(int, PointGroup)
  Q_PROPERTY(int PointGroup READ getPointGroup WRITE setPointGroup)

  SIMPL_FILTER_PARAMETER(int, Numsp)
  Q_PROPERTY(int Numsp READ getNumsp WRITE setNumsp)

  SIMPL_FILTER_PARAMETER(double, MisOr)
  Q_PROPERTY(int MisOr READ getMisOr WRITE setMisOr)

  SIMPL_FILTER_PARAMETER(FloatVec3_t, RefOr)
  Q_PROPERTY(FloatVec3_t RefOr READ getRefOr WRITE setRefOr)

  SIMPL_FILTER_PARAMETER(double, MisOrFull)
  Q_PROPERTY(int MisOrFull READ getMisOrFull WRITE setMisOrFull)

  SIMPL_FILTER_PARAMETER(FloatVec3_t, RefOrFull)
  Q_PROPERTY(FloatVec3_t RefOrFull READ getRefOrFull WRITE setRefOrFull)

  SIMPL_FILTER_PARAMETER(bool, OffsetGrid)
  Q_PROPERTY(bool OffsetGrid READ getOffsetGrid WRITE setOffsetGrid)

  SIMPL_FILTER_PARAMETER(QString, EulerAnglesArrayName)
  Q_PROPERTY(QString EulerAnglesArrayName READ getEulerAnglesArrayName WRITE setEulerAnglesArrayName)

  SIMPL_FILTER_PARAMETER(QString, DataContainerName)
  Q_PROPERTY(QString DataContainerName READ getDataContainerName WRITE setDataContainerName)

  SIMPL_FILTER_PARAMETER(QString, EMsoftAttributeMatrixName)
  Q_PROPERTY(QString EMsoftAttributeMatrixName READ getEMsoftAttributeMatrixName WRITE setEMsoftAttributeMatrixName)

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
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  virtual void execute();

  /**
  * @brief preflight Reimplemented from @see AbstractFilter class
  */
  virtual void preflight();

  /**
   * @brief RodriguesComposition
   * @param sigma
   * @param rod
   */
  void RodriguesComposition(DOrientArrayType sigma, DOrientArrayType& rod);

  /**
   * @brief OrientationListArrayType
   */
  typedef std::list<DOrientArrayType> OrientationListArrayType;

  /**
   * @brief IsinsideFZ
   * @param rod
   * @param FZtype
   * @param FZorder
   * @return
   */
  bool IsinsideFZ(double* rod, int FZtype, int FZorder);

  /**
   * @brief insideCubicFZ
   * @param rod
   * @param symType
   * @return
   */
  bool insideCubicFZ(double* rod, int symType);

  /**
   * @brief insideCyclicFZ
   * @param rod
   * @param order
   * @return
   */
  bool insideCyclicFZ(double* rod, int order);

  /**
   * @brief insideDihedralFZ
   * @param rod
   * @param order
   * @return
   */
  bool insideDihedralFZ(double* rod, int order);

  /**
   * @brief setUpdateProgress
   * @param tuplesCompleted Number of Euler angle tuples completed so far....
   */
  void setUpdateProgress(int tuplesCompleted);

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
  EMsoftSO3Sampler();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  DEFINE_DATAARRAY_VARIABLE(float, EulerAngles)

  EMsoftSO3Sampler(const EMsoftSO3Sampler&); // Copy Constructor Not Implemented
  void operator=(const EMsoftSO3Sampler&);   // Operator '=' Not Implemented
};

#endif /* _EMsoftSO3Sampler_H_ */
