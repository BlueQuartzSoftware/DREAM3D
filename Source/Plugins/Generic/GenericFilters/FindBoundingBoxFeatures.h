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

#include "Generic/GenericDLLExport.h"

/**
 * @brief The FindBoundingBoxFeatures class. See [Filter documentation](@ref findboundingboxfeatures) for details.
 */
class Generic_EXPORT FindBoundingBoxFeatures : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(FindBoundingBoxFeatures SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(FindBoundingBoxFeatures)
  PYB11_FILTER_NEW_MACRO(FindBoundingBoxFeatures)
  PYB11_FILTER_PARAMETER(bool, CalcByPhase)
  PYB11_FILTER_PARAMETER(DataArrayPath, CentroidsArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, PhasesArrayPath)
  PYB11_FILTER_PARAMETER(DataArrayPath, SurfaceFeaturesArrayPath)
  PYB11_FILTER_PARAMETER(QString, BiasedFeaturesArrayName)
  PYB11_PROPERTY(bool CalcByPhase READ getCalcByPhase WRITE setCalcByPhase)
  PYB11_PROPERTY(DataArrayPath CentroidsArrayPath READ getCentroidsArrayPath WRITE setCentroidsArrayPath)
  PYB11_PROPERTY(DataArrayPath PhasesArrayPath READ getPhasesArrayPath WRITE setPhasesArrayPath)
  PYB11_PROPERTY(DataArrayPath SurfaceFeaturesArrayPath READ getSurfaceFeaturesArrayPath WRITE setSurfaceFeaturesArrayPath)
  PYB11_PROPERTY(QString BiasedFeaturesArrayName READ getBiasedFeaturesArrayName WRITE setBiasedFeaturesArrayName)
#endif

public:
  using Self = FindBoundingBoxFeatures;
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
   * @brief Returns the name of the class for FindBoundingBoxFeatures
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for FindBoundingBoxFeatures
   */
  static QString ClassName();

  ~FindBoundingBoxFeatures() override;

  /**
   * @brief Setter property for CalcByPhase
   */
  void setCalcByPhase(bool value);
  /**
   * @brief Getter property for CalcByPhase
   * @return Value of CalcByPhase
   */
  bool getCalcByPhase() const;

  Q_PROPERTY(bool CalcByPhase READ getCalcByPhase WRITE setCalcByPhase)

  /**
   * @brief Setter property for CentroidsArrayPath
   */
  void setCentroidsArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CentroidsArrayPath
   * @return Value of CentroidsArrayPath
   */
  DataArrayPath getCentroidsArrayPath() const;

  Q_PROPERTY(DataArrayPath CentroidsArrayPath READ getCentroidsArrayPath WRITE setCentroidsArrayPath)

  /**
   * @brief Setter property for PhasesArrayPath
   */
  void setPhasesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for PhasesArrayPath
   * @return Value of PhasesArrayPath
   */
  DataArrayPath getPhasesArrayPath() const;

  Q_PROPERTY(DataArrayPath PhasesArrayPath READ getPhasesArrayPath WRITE setPhasesArrayPath)

  /**
   * @brief Setter property for SurfaceFeaturesArrayPath
   */
  void setSurfaceFeaturesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for SurfaceFeaturesArrayPath
   * @return Value of SurfaceFeaturesArrayPath
   */
  DataArrayPath getSurfaceFeaturesArrayPath() const;

  Q_PROPERTY(DataArrayPath SurfaceFeaturesArrayPath READ getSurfaceFeaturesArrayPath WRITE setSurfaceFeaturesArrayPath)

  /**
   * @brief Setter property for BiasedFeaturesArrayName
   */
  void setBiasedFeaturesArrayName(const QString& value);
  /**
   * @brief Getter property for BiasedFeaturesArrayName
   * @return Value of BiasedFeaturesArrayName
   */
  QString getBiasedFeaturesArrayName() const;

  Q_PROPERTY(QString BiasedFeaturesArrayName READ getBiasedFeaturesArrayName WRITE setBiasedFeaturesArrayName)

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
  FindBoundingBoxFeatures();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  std::weak_ptr<DataArray<int32_t>> m_PhasesPtr;
  int32_t* m_Phases = nullptr;
  std::weak_ptr<DataArray<float>> m_CentroidsPtr;
  float* m_Centroids = nullptr;
  std::weak_ptr<DataArray<bool>> m_SurfaceFeaturesPtr;
  bool* m_SurfaceFeatures = nullptr;
  std::weak_ptr<DataArray<bool>> m_BiasedFeaturesPtr;
  bool* m_BiasedFeatures = nullptr;

  bool m_CalcByPhase = {};
  DataArrayPath m_CentroidsArrayPath = {};
  DataArrayPath m_PhasesArrayPath = {};
  DataArrayPath m_SurfaceFeaturesArrayPath = {};
  QString m_BiasedFeaturesArrayName = {};

  /**
   * @brief find_boundingboxfeatures Determines which Features are biased by the outer surface of a 3D volume.
   */
  void find_boundingboxfeatures();

  /**
   * @brief find_boundingboxfeatures2D Determines which Features are biased by the outer surface of a 2D area.
   */
  void find_boundingboxfeatures2D();

public:
  FindBoundingBoxFeatures(const FindBoundingBoxFeatures&) = delete; // Copy Constructor Not Implemented
  FindBoundingBoxFeatures(FindBoundingBoxFeatures&&) = delete;      // Move Constructor Not Implemented
  FindBoundingBoxFeatures& operator=(const FindBoundingBoxFeatures&) = delete; // Copy Assignment Not Implemented
  FindBoundingBoxFeatures& operator=(FindBoundingBoxFeatures&&) = delete;      // Move Assignment Not Implemented
};

