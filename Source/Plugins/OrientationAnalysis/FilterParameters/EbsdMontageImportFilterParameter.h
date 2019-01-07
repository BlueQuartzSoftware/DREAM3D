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

#include <QtCore/QJsonObject>
#include <QtCore/QMetaType>
#include <QtCore/QString>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/SIMPLib.h"

using EbsdMontageListInfo_t = struct
{
  qint32 PaddingDigits = 3;
  quint32 Ordering = 0; /* Ordering=0 = RowColumn, Ordering=1 = ColumnRow */
  qint32 RowStart = 0;
  qint32 RowEnd = 2;
  qint32 ColStart = 0;
  qint32 ColEnd = 2;
  qint32 IncrementIndex = 1;
  QString InputPath;
  QString FilePrefix;
  QString FileSuffix;
  QString FileExtension;

  void writeJson(QJsonObject& json)
  {
    json["PaddingDigits"] = static_cast<double>(PaddingDigits);
    json["Ordering"] = static_cast<double>(Ordering);
    json["RowStart"] = static_cast<double>(RowStart);
    json["ColStart"] = static_cast<double>(ColStart);
    json["RowEnd"] = static_cast<double>(RowEnd);
    json["ColEnd"] = static_cast<double>(ColEnd);
    json["IncrementIndex"] = static_cast<double>(IncrementIndex);
    json["InputPath"] = InputPath;
    json["FilePrefix"] = FilePrefix;
    json["FileSuffix"] = FileSuffix;
    json["FileExtension"] = FileExtension;
  }

  bool readJson(QJsonObject& json)
  {
    if(json["PaddingDigits"].isDouble() && json["Ordering"].isDouble() && json["RowStart"].isDouble() && json["ColStart"].isDouble() && json["IncrementIndex"].isDouble() &&
       json["InputPath"].isString() && json["FilePrefix"].isString() && json["FileSuffix"].isString() && json["FileExtension"].isString())
    {
      PaddingDigits = static_cast<qint32>(json["PaddingDigits"].toDouble());
      Ordering = static_cast<quint32>(json["Ordering"].toDouble());
      RowStart = static_cast<qint32>(json["RowStart"].toDouble());
      ColStart = static_cast<qint32>(json["ColStart"].toDouble());
      RowEnd = static_cast<qint32>(json["RowEnd"].toDouble());
      ColEnd = static_cast<qint32>(json["ColEnd"].toDouble());
      IncrementIndex = static_cast<qint32>(json["IncrementIndex"].toDouble());
      InputPath = json["InputPath"].toString();
      FilePrefix = json["FilePrefix"].toString();
      FileSuffix = json["FileSuffix"].toString();
      FileExtension = json["FileExtension"].toString();
      return true;
    }
    return false;
  }
};

Q_DECLARE_METATYPE(EbsdMontageListInfo_t)

/**
 * @brief SIMPL_NEW_EbsdMontageListInfo_FP This macro is a short-form way of instantiating an instance of
 * EbsdMontageImportFilterParameter. There are 4 required parameters that are always passed to this macro
 * in the following order: HumanLabel, PropertyName, Category, FilterName (class name).
 *
 * Therefore, the macro should be written like this (this is a concrete example):
 * SIMPL_NEW_EbsdMontageListInfo_FP("HumanLabel", PropertyName, Category, FilterName)
 *
 * Example 1 (instantiated within a filter called [GenericExample](@ref genericexample)):
 * SIMPL_NEW_EbsdMontageListInfo_FP("Input File List", InputEbsdMontageListInfo, FilterParameter::Parameter, GenericExample);
 */
#define SIMPL_NEW_EbsdMontageListInfo_FP(...)                                                                                                                                                          \
  SIMPL_EXPAND(_FP_GET_OVERRIDE(__VA_ARGS__, SIMPL_NEW_FP_9, SIMPL_NEW_FP_8, SIMPL_NEW_FP_7, SIMPL_NEW_FP_6, SIMPL_NEW_FP_5, SIMPL_NEW_FP_4)(EbsdMontageImportFilterParameter, __VA_ARGS__))

/**
 * @brief The EbsdMontageImportFilterParameter class is used by filters to instantiate an EbsdMontageListInfoWidget.  By instantiating an instance of
 * this class in a filter's setupFilterParameters() method, a EbsdMontageListInfoWidget will appear in the filter's "filter input" section in the DREAM3D GUI.
 */
class EbsdMontageImportFilterParameter : public FilterParameter
{
public:
  SIMPL_SHARED_POINTERS(EbsdMontageImportFilterParameter)
  SIMPL_STATIC_NEW_MACRO(EbsdMontageImportFilterParameter)
  SIMPL_TYPE_MACRO_SUPER_OVERRIDE(EbsdMontageImportFilterParameter, FilterParameter)

  typedef std::function<void(EbsdMontageListInfo_t)> SetterCallbackType;
  typedef std::function<EbsdMontageListInfo_t(void)> GetterCallbackType;

  /**
   * @brief New This function instantiates an instance of the EbsdMontageImportFilterParameter. Although this
   * function is available to be used, the preferable way to instantiate an instance of this class is to use the
   * SIMPL_NEW_EbsdMontageListInfo_FP(...) macro at the top of this file.

   * @param humanLabel The name that the users of DREAM.3D see for this filter parameter
   * @param propertyName The internal property name for this filter parameter.
   * @param defaultValue The value that this filter parameter will be initialized to by default.
   * @param category The category for the filter parameter in the DREAM.3D user interface.  There
   * are three categories: Parameter, Required Arrays, and Created Arrays.
   * @param setterCallback The method in the AbstractFilter subclass that <i>sets</i> the value of the property
  * that this FilterParameter subclass represents.
   * @param getterCallback The method in the AbstractFilter subclass that <i>gets</i> the value of the property
  * that this FilterParameter subclass represents.
   * @return
   */
  static Pointer New(const QString& humanLabel, const QString& propertyName, const EbsdMontageListInfo_t& defaultValue, Category category, SetterCallbackType setterCallback,
                     GetterCallbackType getterCallback);

  ~EbsdMontageImportFilterParameter() override;

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
  void readJson(const QJsonObject& json) override;

  /**
   * @brief writeJson Writes this filter parameter's corresponding property to a QJsonObject.
   * @param json The QJsonObject that the filter parameter writes to.
   */
  void writeJson(QJsonObject& json) override;

  /**
   * @param SetterCallback The method in the AbstractFilter subclass that <i>sets</i> the value of the property
   * that this FilterParameter subclass represents.
   * from the filter parameter.
   */
  SIMPL_INSTANCE_PROPERTY(SetterCallbackType, SetterCallback)

  /**
   * @param GetterCallback The method in the AbstractFilter subclass that <i>gets</i> the value of the property
   * that this FilterParameter subclass represents.
   * @return The GetterCallback
   */
  SIMPL_INSTANCE_PROPERTY(GetterCallbackType, GetterCallback)

protected:
  /**
   * @brief EbsdMontageImportFilterParameter The default constructor.  It is protected because this
   * filter parameter should only be instantiated using its New(...) function or short-form macro.
   */
  EbsdMontageImportFilterParameter();

public:
  EbsdMontageImportFilterParameter(const EbsdMontageImportFilterParameter&) = delete;            // Copy Constructor Not Implemented
  EbsdMontageImportFilterParameter(EbsdMontageImportFilterParameter&&) = delete;                 // Move Constructor Not Implemented
  EbsdMontageImportFilterParameter& operator=(const EbsdMontageImportFilterParameter&) = delete; // Copy Assignment Not Implemented
  EbsdMontageImportFilterParameter& operator=(EbsdMontageImportFilterParameter&&) = delete;      // Move Assignment Not Implemented
};
