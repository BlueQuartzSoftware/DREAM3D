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
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"


#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @brief The FindSchmids class. See [Filter documentation](@ref findschmids) for details.
 */
class OrientationAnalysis_EXPORT FindSchmids : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(FindSchmids SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(FindSchmids)
  PYB11_FILTER_NEW_MACRO(FindSchmids)
  PYB11_FILTER_PARAMETER(DataArrayPath, FeaturePhasesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, CrystalStructuresArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, AvgQuatsArrayPath)
  PYB11_FILTER_PARAMETER(QString, SchmidsArrayName)
  PYB11_FILTER_PARAMETER(QString, SlipSystemsArrayName)
  PYB11_FILTER_PARAMETER(QString, PolesArrayName)
  PYB11_FILTER_PARAMETER(QString, PhisArrayName)
  PYB11_FILTER_PARAMETER(QString, LambdasArrayName)
  PYB11_FILTER_PARAMETER(FloatVec3Type, LoadingDirection)
  PYB11_FILTER_PARAMETER(bool, StoreAngleComponents)
  PYB11_FILTER_PARAMETER(bool, OverrideSystem)
  PYB11_FILTER_PARAMETER(FloatVec3Type, SlipPlane)
  PYB11_FILTER_PARAMETER(FloatVec3Type, SlipDirection)
  PYB11_PROPERTY(DataArrayPath FeaturePhasesArrayPath READ getFeaturePhasesArrayPath WRITE setFeaturePhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath CrystalStructuresArrayPath READ getCrystalStructuresArrayPath WRITE setCrystalStructuresArrayPath)
  PYB11_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)
  PYB11_PROPERTY(QString SchmidsArrayName READ getSchmidsArrayName WRITE setSchmidsArrayName)
  PYB11_PROPERTY(QString SlipSystemsArrayName READ getSlipSystemsArrayName WRITE setSlipSystemsArrayName)
  PYB11_PROPERTY(QString PolesArrayName READ getPolesArrayName WRITE setPolesArrayName)
  PYB11_PROPERTY(QString PhisArrayName READ getPhisArrayName WRITE setPhisArrayName)
  PYB11_PROPERTY(QString LambdasArrayName READ getLambdasArrayName WRITE setLambdasArrayName)
  PYB11_PROPERTY(FloatVec3Type LoadingDirection READ getLoadingDirection WRITE setLoadingDirection)
  PYB11_PROPERTY(bool StoreAngleComponents READ getStoreAngleComponents WRITE setStoreAngleComponents)
  PYB11_PROPERTY(bool OverrideSystem READ getOverrideSystem WRITE setOverrideSystem)
  PYB11_PROPERTY(FloatVec3Type SlipPlane READ getSlipPlane WRITE setSlipPlane)
  PYB11_PROPERTY(FloatVec3Type SlipDirection READ getSlipDirection WRITE setSlipDirection)
#endif

public:
  using Self = FindSchmids;
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
   * @brief Returns the name of the class for FindSchmids
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FindSchmids
   */
  static QString ClassName();

  ~FindSchmids() override;

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
   * @brief Setter property for SchmidsArrayName
   */
  void setSchmidsArrayName(const QString& value);
  /**
   * @brief Getter property for SchmidsArrayName
   * @return Value of SchmidsArrayName
   */
  QString getSchmidsArrayName() const;

  Q_PROPERTY(QString SchmidsArrayName READ getSchmidsArrayName WRITE setSchmidsArrayName)

  /**
   * @brief Setter property for SlipSystemsArrayName
   */
  void setSlipSystemsArrayName(const QString& value);
  /**
   * @brief Getter property for SlipSystemsArrayName
   * @return Value of SlipSystemsArrayName
   */
  QString getSlipSystemsArrayName() const;

  Q_PROPERTY(QString SlipSystemsArrayName READ getSlipSystemsArrayName WRITE setSlipSystemsArrayName)

  /**
   * @brief Setter property for PolesArrayName
   */
  void setPolesArrayName(const QString& value);
  /**
   * @brief Getter property for PolesArrayName
   * @return Value of PolesArrayName
   */
  QString getPolesArrayName() const;

  Q_PROPERTY(QString PolesArrayName READ getPolesArrayName WRITE setPolesArrayName)

  /**
   * @brief Setter property for PhisArrayName
   */
  void setPhisArrayName(const QString& value);
  /**
   * @brief Getter property for PhisArrayName
   * @return Value of PhisArrayName
   */
  QString getPhisArrayName() const;

  Q_PROPERTY(QString PhisArrayName READ getPhisArrayName WRITE setPhisArrayName)

  /**
   * @brief Setter property for LambdasArrayName
   */
  void setLambdasArrayName(const QString& value);
  /**
   * @brief Getter property for LambdasArrayName
   * @return Value of LambdasArrayName
   */
  QString getLambdasArrayName() const;

  Q_PROPERTY(QString LambdasArrayName READ getLambdasArrayName WRITE setLambdasArrayName)

  /**
   * @brief Setter property for LoadingDirection
   */
  void setLoadingDirection(const FloatVec3Type& value);
  /**
   * @brief Getter property for LoadingDirection
   * @return Value of LoadingDirection
   */
  FloatVec3Type getLoadingDirection() const;

  Q_PROPERTY(FloatVec3Type LoadingDirection READ getLoadingDirection WRITE setLoadingDirection)

  /**
   * @brief Setter property for StoreAngleComponents
   */
  void setStoreAngleComponents(bool value);
  /**
   * @brief Getter property for StoreAngleComponents
   * @return Value of StoreAngleComponents
   */
  bool getStoreAngleComponents() const;

  Q_PROPERTY(bool StoreAngleComponents READ getStoreAngleComponents WRITE setStoreAngleComponents)

  /**
   * @brief Setter property for OverrideSystem
   */
  void setOverrideSystem(bool value);
  /**
   * @brief Getter property for OverrideSystem
   * @return Value of OverrideSystem
   */
  bool getOverrideSystem() const;

  Q_PROPERTY(bool OverrideSystem READ getOverrideSystem WRITE setOverrideSystem)

  /**
   * @brief Setter property for SlipPlane
   */
  void setSlipPlane(const FloatVec3Type& value);
  /**
   * @brief Getter property for SlipPlane
   * @return Value of SlipPlane
   */
  FloatVec3Type getSlipPlane() const;

  Q_PROPERTY(FloatVec3Type SlipPlane READ getSlipPlane WRITE setSlipPlane)

  /**
   * @brief Setter property for SlipDirection
   */
  void setSlipDirection(const FloatVec3Type& value);
  /**
   * @brief Getter property for SlipDirection
   * @return Value of SlipDirection
   */
  FloatVec3Type getSlipDirection() const;

  Q_PROPERTY(FloatVec3Type SlipDirection READ getSlipDirection WRITE setSlipDirection)

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
  FindSchmids();
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
  std::weak_ptr<DataArray<unsigned int>> m_CrystalStructuresPtr;
  unsigned int* m_CrystalStructures = nullptr;
  std::weak_ptr<DataArray<float>> m_SchmidsPtr;
  float* m_Schmids = nullptr;
  std::weak_ptr<DataArray<float>> m_PhisPtr;
  float* m_Phis = nullptr;
  std::weak_ptr<DataArray<float>> m_LambdasPtr;
  float* m_Lambdas = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_PolesPtr;
  int32_t* m_Poles = nullptr;
  std::weak_ptr<DataArray<int32_t>> m_SlipSystemsPtr;
  int32_t* m_SlipSystems = nullptr;

  DataArrayPath m_FeaturePhasesArrayPath = {};
  DataArrayPath m_CrystalStructuresArrayPath = {};
  DataArrayPath m_AvgQuatsArrayPath = {};
  QString m_SchmidsArrayName = {};
  QString m_SlipSystemsArrayName = {};
  QString m_PolesArrayName = {};
  QString m_PhisArrayName = {};
  QString m_LambdasArrayName = {};
  FloatVec3Type m_LoadingDirection = {};
  bool m_StoreAngleComponents = {};
  bool m_OverrideSystem = {};
  FloatVec3Type m_SlipPlane = {};
  FloatVec3Type m_SlipDirection = {};


public:
  FindSchmids(const FindSchmids&) = delete;            // Copy Constructor Not Implemented
  FindSchmids(FindSchmids&&) = delete;                 // Move Constructor Not Implemented
  FindSchmids& operator=(const FindSchmids&) = delete; // Copy Assignment Not Implemented
  FindSchmids& operator=(FindSchmids&&) = delete;      // Move Assignment Not Implemented
};

