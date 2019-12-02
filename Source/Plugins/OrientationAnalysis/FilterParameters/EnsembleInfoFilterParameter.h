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

#include <QtCore/QJsonObject>

#include "SIMPLib/Common/EnsembleInfo.h"
#include "SIMPLib/DataContainers/AttributeMatrix.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/Geometry/IGeometry.h"

/**
 * @brief SIMPL_NEW_COMP_SEL_FP This macro is a short-form way of instantiating an instance of
 * EnsembleInfoFilterParameter. There are 6 required parameters and 1 optional parameter
 * that are always passed to this macro in the following order: HumanLabel, PropertyName, Category,
 * FilterName (class name), Choices, ShowOperators, GroupIndex (optional).
 *
 * Therefore, the macro should be written like this (this is a concrete example):
 * SIMPL_NEW_COMP_SEL_FP("HumanLabel", PropertyName, Category, FilterName, Choices, ShowOperators, GroupIndex)
 *
 * Example 1 (instantiated within a filter called [GenericExample](@ref genericexample), without optional GroupIndex parameter):
 * SIMPL_NEW_COMP_SEL_FP("Select Arrays to Threshold", SelectedThresholds, FilterParameter::Parameter, GenericExample, choices, true);
 */
#define SIMPL_NEW_ENS_INFO_FP(...)                                                                                                                                                                     \
  SIMPL_EXPAND(_FP_GET_OVERRIDE(__VA_ARGS__, SIMPL_NEW_FP_9, SIMPL_NEW_FP_8, SIMPL_NEW_FP_7, SIMPL_NEW_FP_6, SIMPL_NEW_FP_5, SIMPL_NEW_FP_4)(EnsembleInfoFilterParameter, __VA_ARGS__))

/**
 * @brief The EnsembleInfoFilterParameter class is used by filters to instantiate an ComparisonSelectionWidget.  By instantiating an instance of
 * this class in a filter's setupFilterParameters() method, a ComparisonSelectionWidget will appear in the filter's "filter input" section in the DREAM3D GUI.
 */
class EnsembleInfoFilterParameter : public FilterParameter
{
public:
  using Self = EnsembleInfoFilterParameter;
  using Pointer = std::shared_ptr<Self>;
  using ConstPointer = std::shared_ptr<const Self>;
  using WeakPointer = std::weak_ptr<Self>;
  using ConstWeakPointer = std::weak_ptr<const Self>;
  static Pointer NullPointer();

  static Pointer New();

  /**
   * @brief Returns the name of the class for EnsembleInfoFilterParameter
   */
  QString getNameOfClass() const override;
  /**
   * @brief Returns the name of the class for EnsembleInfoFilterParameter
   */
  static QString ClassName();

  typedef std::function<void(EnsembleInfo)> SetterCallbackType;
  typedef std::function<EnsembleInfo(void)> GetterCallbackType;

  /**
   * @brief New This function instantiates an instance of the EnsembleInfoFilterParameter. Although this function is available to be used,
   * the preferable way to instantiate an instance of this class is to use the SIMPL_NEW_COMP_SEL_FP(...) macro at the top of this file.

   * @param humanLabel The name that the users of DREAM.3D see for this filter parameter
   * @param propertyName The internal property name for this filter parameter.
   * @param defaultValue The value that this filter parameter will be initialized to by default.
   * @param category The category for the filter parameter in the DREAM.3D user interface.  There
   * are three categories: Parameter, Required Arrays, and Created Arrays.
   * @param setterCallback The method in the AbstractFilter subclass that <i>sets</i> the value of the property
  * that this FilterParameter subclass represents.
   * @param getterCallback The method in the AbstractFilter subclass that <i>gets</i> the value of the property
  * that this FilterParameter subclass represents.
  * @param choices The selections to choose from in the ComparisonSelectionWidget.
  * @param showOperators Boolean that determines whether to display the operators or not.
   * @param groupIndex Integer that specifies the group that this filter parameter will be placed in.
   * @return
   */
  static Pointer New(const QString& humanLabel, const QString& propertyName, EnsembleInfo defaultValue, Category category, const SetterCallbackType& setterCallback,
                     const GetterCallbackType& getterCallback, QVector<QString> choices, bool showOperators, int groupIndex = -1);

  virtual ~EnsembleInfoFilterParameter();

  /**
   * @brief Setter property for Choices
   */
  void setChoices(const QVector<QString>& value);
  /**
   * @brief Getter property for Choices
   * @return Value of Choices
   */
  QVector<QString> getChoices() const;

  /**
   * @brief Setter property for ShowOperators
   */
  void setShowOperators(bool value);
  /**
   * @brief Getter property for ShowOperators
   * @return Value of ShowOperators
   */
  bool getShowOperators() const;

  /**
   * @brief Setter property for DefaultGeometryTypes
   */
  void setDefaultGeometryTypes(const IGeometry::Types& value);
  /**
   * @brief Getter property for DefaultGeometryTypes
   * @return Value of DefaultGeometryTypes
   */
  IGeometry::Types getDefaultGeometryTypes() const;

  /**
   * @brief Setter property for DefaultAttributeMatrixTypes
   */
  void setDefaultAttributeMatrixTypes(const AttributeMatrix::Types& value);
  /**
   * @brief Getter property for DefaultAttributeMatrixTypes
   * @return Value of DefaultAttributeMatrixTypes
   */
  AttributeMatrix::Types getDefaultAttributeMatrixTypes() const;

  /**
 * @brief getWidgetType Returns the type of widget that displays and controls
 * this FilterParameter subclass
 * @return
 */
  QString getWidgetType() const override;

  /**
   * @brief readJson Reads this filter parameter's corresponding property out of a QJsonObject.
   * @param json The QJsonObject that the filter parameter reads from.
   */
  void readJson(const QJsonObject& json);

  /**
   * @brief writeJson Writes this filter parameter's corresponding property to a QJsonObject.
   * @param json The QJsonObject that the filter parameter writes to.
   */
  void writeJson(QJsonObject& json);

  /**
  * @param SetterCallback The method in the AbstractFilter subclass that <i>sets</i> the value of the property
  * that this FilterParameter subclass represents.
  * from the filter parameter.
  */
  /**
   * @brief Setter property for SetterCallback
   */
  void setSetterCallback(const EnsembleInfoFilterParameter::SetterCallbackType& value);
  /**
   * @brief Getter property for SetterCallback
   * @return Value of SetterCallback
   */
  EnsembleInfoFilterParameter::SetterCallbackType getSetterCallback() const;

  /**
  * @param GetterCallback The method in the AbstractFilter subclass that <i>gets</i> the value of the property
  * that this FilterParameter subclass represents.
  * @return The GetterCallback
  */
  /**
   * @brief Setter property for GetterCallback
   */
  void setGetterCallback(const EnsembleInfoFilterParameter::GetterCallbackType& value);
  /**
   * @brief Getter property for GetterCallback
   * @return Value of GetterCallback
   */
  EnsembleInfoFilterParameter::GetterCallbackType getGetterCallback() const;

protected:
  /**
   * @brief EnsembleInfoFilterParameter The default constructor.  It is protected because this
   * filter parameter should only be instantiated using its New(...) function or short-form macro.
   */
  EnsembleInfoFilterParameter();

public:
  EnsembleInfoFilterParameter(const EnsembleInfoFilterParameter&) = delete; // Copy Constructor Not Implemented
  EnsembleInfoFilterParameter(EnsembleInfoFilterParameter&&) = delete;      // Move Constructor Not Implemented
  EnsembleInfoFilterParameter& operator=(const EnsembleInfoFilterParameter&) = delete; // Copy Assignment Not Implemented
  EnsembleInfoFilterParameter& operator=(EnsembleInfoFilterParameter&&) = delete;      // Move Assignment Not Implemented

private:
  QVector<QString> m_Choices = {};
  bool m_ShowOperators = {};
  IGeometry::Types m_DefaultGeometryTypes = {};
  AttributeMatrix::Types m_DefaultAttributeMatrixTypes = {};
  EnsembleInfoFilterParameter::SetterCallbackType m_SetterCallback = {};
  EnsembleInfoFilterParameter::GetterCallbackType m_GetterCallback = {};
};

