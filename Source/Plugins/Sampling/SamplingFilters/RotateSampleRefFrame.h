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

#include "Sampling/SamplingDLLExport.h"

/**
 * @brief The RotateSampleRefFrame class. See [Filter documentation](@ref rotatesamplerefframe) for details.
 */
class Sampling_EXPORT RotateSampleRefFrame : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(RotateSampleRefFrame SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(RotateSampleRefFrame)
  PYB11_FILTER_NEW_MACRO(RotateSampleRefFrame)
  PYB11_FILTER_PARAMETER(DataArrayPath, CellAttributeMatrixPath)
  PYB11_FILTER_PARAMETER(FloatVec3Type, RotationAxis)
  PYB11_FILTER_PARAMETER(float, RotationAngle)
  PYB11_FILTER_PARAMETER(bool, SliceBySlice)
  PYB11_PROPERTY(DataArrayPath CellAttributeMatrixPath READ getCellAttributeMatrixPath WRITE setCellAttributeMatrixPath)
  PYB11_PROPERTY(FloatVec3Type RotationAxis READ getRotationAxis WRITE setRotationAxis)
  PYB11_PROPERTY(float RotationAngle READ getRotationAngle WRITE setRotationAngle)
  PYB11_PROPERTY(bool SliceBySlice READ getSliceBySlice WRITE setSliceBySlice)
#endif

public:
  using Self = RotateSampleRefFrame;
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
   * @brief Returns the name of the class for RotateSampleRefFrame
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for RotateSampleRefFrame
   */
  static QString ClassName();

  ~RotateSampleRefFrame() override;

  /**
   * @brief Setter property for CellAttributeMatrixPath
   */
  void setCellAttributeMatrixPath(const DataArrayPath& value);
  /**
   * @brief Getter property for CellAttributeMatrixPath
   * @return Value of CellAttributeMatrixPath
   */
  DataArrayPath getCellAttributeMatrixPath() const;

  Q_PROPERTY(DataArrayPath CellAttributeMatrixPath READ getCellAttributeMatrixPath WRITE setCellAttributeMatrixPath)

  /**
   * @brief Setter property for RotationAxis
   */
  void setRotationAxis(const FloatVec3Type& value);
  /**
   * @brief Getter property for RotationAxis
   * @return Value of RotationAxis
   */
  FloatVec3Type getRotationAxis() const;

  Q_PROPERTY(FloatVec3Type RotationAxis READ getRotationAxis WRITE setRotationAxis)

  /**
   * @brief Setter property for RotationAngle
   */
  void setRotationAngle(float value);
  /**
   * @brief Getter property for RotationAngle
   * @return Value of RotationAngle
   */
  float getRotationAngle() const;

  Q_PROPERTY(float RotationAngle READ getRotationAngle WRITE setRotationAngle)

  // This is getting exposed because other filters that are calling this filter needs to set this value
  /**
   * @brief Setter property for SliceBySlice
   */
  void setSliceBySlice(bool value);
  /**
   * @brief Getter property for SliceBySlice
   * @return Value of SliceBySlice
   */
  bool getSliceBySlice() const;

  Q_PROPERTY(bool SliceBySlice READ getSliceBySlice WRITE setSliceBySlice)

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
  RotateSampleRefFrame();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

public:
  RotateSampleRefFrame(const RotateSampleRefFrame&) = delete; // Copy Constructor Not Implemented
  RotateSampleRefFrame(RotateSampleRefFrame&&) = delete;      // Move Constructor Not Implemented
  RotateSampleRefFrame& operator=(const RotateSampleRefFrame&) = delete; // Copy Assignment Not Implemented
  RotateSampleRefFrame& operator=(RotateSampleRefFrame&&) = delete;      // Move Assignment Not Implemented

private:
  DataArrayPath m_CellAttributeMatrixPath = {};
  FloatVec3Type m_RotationAxis = {};
  float m_RotationAngle = {};
  bool m_SliceBySlice = {};
};

