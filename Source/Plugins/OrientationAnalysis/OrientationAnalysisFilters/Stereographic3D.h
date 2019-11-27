/* ============================================================================
 * Copyright (c) 2016 BlueQuartz Software, LLC
 * All rights reserved.
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <memory>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "OrientationAnalysis/OrientationAnalysisDLLExport.h"

/**
 * @brief The Stereographic3D class. See [Filter documentation](@ref stereographic3d) for details.
 */
class OrientationAnalysis_EXPORT Stereographic3D : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(Stereographic3D SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(Stereographic3D)
  PYB11_FILTER_NEW_MACRO(Stereographic3D)
  PYB11_FILTER_PARAMETER(DataArrayPath, QuatsArrayPath)
  PYB11_FILTER_PARAMETER(QString, CoordinatesArrayName)
  PYB11_PROPERTY(DataArrayPath QuatsArrayPath READ getQuatsArrayPath WRITE setQuatsArrayPath)
  PYB11_PROPERTY(QString CoordinatesArrayName READ getCoordinatesArrayName WRITE setCoordinatesArrayName)
#endif

public:
  using Self = Stereographic3D;
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
   * @brief Returns the name of the class for Stereographic3D
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for Stereographic3D
   */
  static QString ClassName();

  ~Stereographic3D() override;

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
   * @brief Setter property for CoordinatesArrayName
   */
  void setCoordinatesArrayName(const QString& value);
  /**
   * @brief Getter property for CoordinatesArrayName
   * @return Value of CoordinatesArrayName
   */
  QString getCoordinatesArrayName() const;

  Q_PROPERTY(QString CoordinatesArrayName READ getCoordinatesArrayName WRITE setCoordinatesArrayName)

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
  Stereographic3D();

  /**
  * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
  */
  void dataCheck();

  /**
  * @brief Initializes all the private instance variables.
  */
  void initialize();

private:
  std::weak_ptr<DataArray<float>> m_QuatsPtr;
  float* m_Quats = nullptr;
  std::weak_ptr<DataArray<float>> m_CellCoordinatesPtr;
  float* m_CellCoordinates = nullptr;

  DataArrayPath m_QuatsArrayPath = {};
  QString m_CoordinatesArrayName = {};

public:
  Stereographic3D(const Stereographic3D&) = delete; // Copy Constructor Not Implemented
  Stereographic3D(Stereographic3D&&) = delete;      // Move Constructor Not Implemented
  Stereographic3D& operator=(const Stereographic3D&) = delete; // Copy Assignment Not Implemented
  Stereographic3D& operator=(Stereographic3D&&) = delete;      // Move Assignment Not Implemented
};

