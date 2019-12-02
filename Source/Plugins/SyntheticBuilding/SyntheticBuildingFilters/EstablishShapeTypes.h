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
#include "SIMPLib/Common/ShapeType.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "SyntheticBuilding/SyntheticBuildingDLLExport.h"

/**
 * @brief The EstablishShapeTypes class. See [Filter documentation](@ref establishshapetypes) for details.
 */
class SyntheticBuilding_EXPORT EstablishShapeTypes : public AbstractFilter
{
  Q_OBJECT

#ifdef SIMPL_ENABLE_PYTHON
  PYB11_CREATE_BINDINGS(EstablishShapeTypes SUPERCLASS AbstractFilter)
  PYB11_SHARED_POINTERS(EstablishShapeTypes)
  PYB11_FILTER_NEW_MACRO(EstablishShapeTypes)
  PYB11_FILTER_PARAMETER(DataArrayPath, InputPhaseTypesArrayPath)
  PYB11_FILTER_PARAMETER(QString, ShapeTypesArrayName)
  PYB11_FILTER_PARAMETER(ShapeType::Types, ShapeTypeData)
  PYB11_PROPERTY(DataArrayPath InputPhaseTypesArrayPath READ getInputPhaseTypesArrayPath WRITE setInputPhaseTypesArrayPath)
  PYB11_PROPERTY(QString ShapeTypesArrayName READ getShapeTypesArrayName WRITE setShapeTypesArrayName)
  PYB11_PROPERTY(ShapeType::Types ShapeTypeData READ getShapeTypeData WRITE setShapeTypeData)
#endif

public:
  using Self = EstablishShapeTypes;
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
   * @brief Returns the name of the class for EstablishShapeTypes
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for EstablishShapeTypes
   */
  static QString ClassName();

  ~EstablishShapeTypes() override;

  /**
   * @brief Setter property for InputPhaseTypesArrayPath
   */
  void setInputPhaseTypesArrayPath(const DataArrayPath& value);
  /**
   * @brief Getter property for InputPhaseTypesArrayPath
   * @return Value of InputPhaseTypesArrayPath
   */
  DataArrayPath getInputPhaseTypesArrayPath() const;

  Q_PROPERTY(DataArrayPath InputPhaseTypesArrayPath READ getInputPhaseTypesArrayPath WRITE setInputPhaseTypesArrayPath)

  /**
   * @brief Setter property for ShapeTypesArrayName
   */
  void setShapeTypesArrayName(const QString& value);
  /**
   * @brief Getter property for ShapeTypesArrayName
   * @return Value of ShapeTypesArrayName
   */
  QString getShapeTypesArrayName() const;

  Q_PROPERTY(QString ShapeTypesArrayName READ getShapeTypesArrayName WRITE setShapeTypesArrayName)

  /**
   * @brief Setter property for ShapeTypeData
   */
  void setShapeTypeData(const ShapeType::Types& value);
  /**
   * @brief Getter property for ShapeTypeData
   * @return Value of ShapeTypeData
   */
  ShapeType::Types getShapeTypeData() const;

  Q_PROPERTY(ShapeType::Types ShapeTypeData READ getShapeTypeData WRITE setShapeTypeData)

  int getPhaseCount();
  Q_PROPERTY(int PhaseCount READ getPhaseCount)

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
  EstablishShapeTypes();
  /**
   * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
   */
  void dataCheck();

  /**
   * @brief Initializes all the private instance variables.
   */
  void initialize();

private:
  std::weak_ptr<DataArray<uint32_t>> m_PhaseTypesPtr;
  uint32_t* m_PhaseTypes = nullptr;
  std::weak_ptr<DataArray<ShapeType::EnumType>> m_ShapeTypesPtr;
  ShapeType::EnumType* m_ShapeTypes = nullptr;

  DataArrayPath m_InputPhaseTypesArrayPath = {};
  QString m_ShapeTypesArrayName = {};
  ShapeType::Types m_ShapeTypeData = {};

public:
  EstablishShapeTypes(const EstablishShapeTypes&) = delete; // Copy Constructor Not Implemented
  EstablishShapeTypes(EstablishShapeTypes&&) = delete;      // Move Constructor Not Implemented
  EstablishShapeTypes& operator=(const EstablishShapeTypes&) = delete; // Copy Assignment Not Implemented
  EstablishShapeTypes& operator=(EstablishShapeTypes&&) = delete;      // Move Assignment Not Implemented
};

