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

#pragma once

#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "OrientationLib/OrientationLib.h"
#include "OrientationLib/Core/Orientation.hpp"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @brief The EMsoftSO3Sampler class. See [Filter documentation](@ref emsoftso3sampler) for details.
 */
class OrientationAnalysis_EXPORT EMsoftSO3Sampler : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(EMsoftSO3Sampler SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(EMsoftSO3Sampler)
  PYB11_FILTER_NEW_MACRO(EMsoftSO3Sampler)
  PYB11_FILTER_PARAMETER(int, sampleModeSelector)
  PYB11_FILTER_PARAMETER(int, PointGroup)
  PYB11_FILTER_PARAMETER(int, Numsp)
  PYB11_FILTER_PARAMETER(double, MisOr)
  PYB11_FILTER_PARAMETER(FloatVec3Type, RefOr)
  PYB11_FILTER_PARAMETER(double, MisOrFull)
  PYB11_FILTER_PARAMETER(FloatVec3Type, RefOrFull)
  PYB11_FILTER_PARAMETER(bool, OffsetGrid)
  PYB11_FILTER_PARAMETER(QString, EulerAnglesArrayName)
  PYB11_FILTER_PARAMETER(DataArrayPath, DataContainerName)
  PYB11_FILTER_PARAMETER(QString, EMsoftAttributeMatrixName)
  PYB11_PROPERTY(int sampleModeSelector READ getsampleModeSelector WRITE setsampleModeSelector)
  PYB11_PROPERTY(int PointGroup READ getPointGroup WRITE setPointGroup)
  PYB11_PROPERTY(int Numsp READ getNumsp WRITE setNumsp)
  PYB11_PROPERTY(double MisOr READ getMisOr WRITE setMisOr)
  PYB11_PROPERTY(FloatVec3Type RefOr READ getRefOr WRITE setRefOr)
  PYB11_PROPERTY(double MisOrFull READ getMisOrFull WRITE setMisOrFull)
  PYB11_PROPERTY(FloatVec3Type RefOrFull READ getRefOrFull WRITE setRefOrFull)
  PYB11_PROPERTY(bool OffsetGrid READ getOffsetGrid WRITE setOffsetGrid)
  PYB11_PROPERTY(QString EulerAnglesArrayName READ getEulerAnglesArrayName WRITE setEulerAnglesArrayName)
  PYB11_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)
  PYB11_PROPERTY(QString EMsoftAttributeMatrixName READ getEMsoftAttributeMatrixName WRITE setEMsoftAttributeMatrixName)
#endif

public:
  using Self = EMsoftSO3Sampler;
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
   * @brief Returns the name of the class for EMsoftSO3Sampler
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for EMsoftSO3Sampler
   */
  static QString ClassName();

  ~EMsoftSO3Sampler() override;

  /**
   * EMsoftSO3Sampler filter parameters
   */

  /**
   * @brief Setter property for sampleModeSelector
   */
  void setsampleModeSelector(int value);
  /**
   * @brief Getter property for sampleModeSelector
   * @return Value of sampleModeSelector
   */
  int getsampleModeSelector() const;

  Q_PROPERTY(int sampleModeSelector READ getsampleModeSelector WRITE setsampleModeSelector)

  /**
   * @brief Setter property for PointGroup
   */
  void setPointGroup(int value);
  /**
   * @brief Getter property for PointGroup
   * @return Value of PointGroup
   */
  int getPointGroup() const;

  Q_PROPERTY(int PointGroup READ getPointGroup WRITE setPointGroup)

  /**
   * @brief Setter property for Numsp
   */
  void setNumsp(int value);
  /**
   * @brief Getter property for Numsp
   * @return Value of Numsp
   */
  int getNumsp() const;

  Q_PROPERTY(int Numsp READ getNumsp WRITE setNumsp)

  /**
   * @brief Setter property for MisOr
   */
  void setMisOr(double value);
  /**
   * @brief Getter property for MisOr
   * @return Value of MisOr
   */
  double getMisOr() const;

  Q_PROPERTY(int MisOr READ getMisOr WRITE setMisOr)

  /**
   * @brief Setter property for RefOr
   */
  void setRefOr(const FloatVec3Type& value);
  /**
   * @brief Getter property for RefOr
   * @return Value of RefOr
   */
  FloatVec3Type getRefOr() const;

  Q_PROPERTY(FloatVec3Type RefOr READ getRefOr WRITE setRefOr)

  /**
   * @brief Setter property for MisOrFull
   */
  void setMisOrFull(double value);
  /**
   * @brief Getter property for MisOrFull
   * @return Value of MisOrFull
   */
  double getMisOrFull() const;

  Q_PROPERTY(int MisOrFull READ getMisOrFull WRITE setMisOrFull)

  /**
   * @brief Setter property for RefOrFull
   */
  void setRefOrFull(const FloatVec3Type& value);
  /**
   * @brief Getter property for RefOrFull
   * @return Value of RefOrFull
   */
  FloatVec3Type getRefOrFull() const;

  Q_PROPERTY(FloatVec3Type RefOrFull READ getRefOrFull WRITE setRefOrFull)

  /**
   * @brief Setter property for OffsetGrid
   */
  void setOffsetGrid(bool value);
  /**
   * @brief Getter property for OffsetGrid
   * @return Value of OffsetGrid
   */
  bool getOffsetGrid() const;

  Q_PROPERTY(bool OffsetGrid READ getOffsetGrid WRITE setOffsetGrid)

  /**
   * @brief Setter property for EulerAnglesArrayName
   */
  void setEulerAnglesArrayName(const QString& value);
  /**
   * @brief Getter property for EulerAnglesArrayName
   * @return Value of EulerAnglesArrayName
   */
  QString getEulerAnglesArrayName() const;

  Q_PROPERTY(QString EulerAnglesArrayName READ getEulerAnglesArrayName WRITE setEulerAnglesArrayName)

  /**
   * @brief Setter property for DataContainerName
   */
  void setDataContainerName(const DataArrayPath& value);
  /**
   * @brief Getter property for DataContainerName
   * @return Value of DataContainerName
   */
  DataArrayPath getDataContainerName() const;

  Q_PROPERTY(DataArrayPath DataContainerName READ getDataContainerName WRITE setDataContainerName)

  /**
   * @brief Setter property for EMsoftAttributeMatrixName
   */
  void setEMsoftAttributeMatrixName(const QString& value);
  /**
   * @brief Getter property for EMsoftAttributeMatrixName
   * @return Value of EMsoftAttributeMatrixName
   */
  QString getEMsoftAttributeMatrixName() const;

  Q_PROPERTY(QString EMsoftAttributeMatrixName READ getEMsoftAttributeMatrixName WRITE setEMsoftAttributeMatrixName)

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
   * @brief execute Reimplemented from @see AbstractFilter class
   */
  void execute() override;

  /**
  * @brief preflight Reimplemented from @see AbstractFilter class
  */
  void preflight() override;

  /**
   * @brief RodriguesComposition
   * @param sigma
   * @param rod
   */
  void RodriguesComposition(OrientationD sigma, OrientationD& rod);

  /**
   * @brief OrientationListArrayType
   */
  typedef std::list<OrientationD> OrientationListArrayType;

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
  std::weak_ptr<DataArray<float>> m_EulerAnglesPtr;
  float* m_EulerAngles = nullptr;

  int m_sampleModeSelector = {};
  int m_PointGroup = {};
  int m_Numsp = {};
  double m_MisOr = {};
  FloatVec3Type m_RefOr = {};
  double m_MisOrFull = {};
  FloatVec3Type m_RefOrFull = {};
  bool m_OffsetGrid = {};
  QString m_EulerAnglesArrayName = {};
  DataArrayPath m_DataContainerName = {};
  QString m_EMsoftAttributeMatrixName = {};

public:
  EMsoftSO3Sampler(const EMsoftSO3Sampler&) = delete; // Copy Constructor Not Implemented
  EMsoftSO3Sampler(EMsoftSO3Sampler&&) = delete;      // Move Constructor Not Implemented
  EMsoftSO3Sampler& operator=(const EMsoftSO3Sampler&) = delete; // Copy Assignment Not Implemented
  EMsoftSO3Sampler& operator=(EMsoftSO3Sampler&&) = delete;      // Move Assignment Not Implemented
};

