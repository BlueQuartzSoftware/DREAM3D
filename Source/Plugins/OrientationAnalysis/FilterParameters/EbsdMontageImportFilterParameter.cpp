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

#include "EbsdMontageImportFilterParameter.h"

#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EbsdMontageImportFilterParameter::EbsdMontageImportFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EbsdMontageImportFilterParameter::~EbsdMontageImportFilterParameter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EbsdMontageImportFilterParameter::Pointer EbsdMontageImportFilterParameter::New(const QString& humanLabel, const QString& propertyName, const EbsdMontageListInfo& defaultValue,
                                                                                FilterParameter::Category category, SetterCallbackType setterCallback, GetterCallbackType getterCallback)
{

  EbsdMontageImportFilterParameter::Pointer ptr = EbsdMontageImportFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setCategory(category);
  ptr->setSetterCallback(setterCallback);
  ptr->setGetterCallback(getterCallback);

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EbsdMontageImportFilterParameter::getWidgetType() const
{
  return QString("EbsdMontageImportWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdMontageImportFilterParameter::readJson(const QJsonObject& json)
{
  QJsonValue jsonValue = json[getPropertyName()];
  if(!jsonValue.isUndefined() && m_SetterCallback)
  {
    QJsonObject jsonObj = jsonValue.toObject();
    EbsdMontageListInfo fileListInfo;
    fileListInfo.readJson(jsonObj);
    m_SetterCallback(fileListInfo);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdMontageImportFilterParameter::writeJson(QJsonObject& json)
{
  if(m_GetterCallback)
  {
    EbsdMontageListInfo fileListInfo = m_GetterCallback();
    QJsonObject jsonObj;
    fileListInfo.writeJson(jsonObj);
    json[getPropertyName()] = jsonObj;
  }
}

// -----------------------------------------------------------------------------
EbsdMontageImportFilterParameter::Pointer EbsdMontageImportFilterParameter::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
EbsdMontageImportFilterParameter::Pointer EbsdMontageImportFilterParameter::New()
{
  Pointer sharedPtr(new(EbsdMontageImportFilterParameter));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString EbsdMontageImportFilterParameter::getNameOfClass() const
{
  return QString("EbsdMontageImportFilterParameter");
}

// -----------------------------------------------------------------------------
QString EbsdMontageImportFilterParameter::ClassName()
{
  return QString("EbsdMontageImportFilterParameter");
}

// -----------------------------------------------------------------------------
void EbsdMontageImportFilterParameter::setSetterCallback(const EbsdMontageImportFilterParameter::SetterCallbackType& value)
{
  m_SetterCallback = value;
}

// -----------------------------------------------------------------------------
EbsdMontageImportFilterParameter::SetterCallbackType EbsdMontageImportFilterParameter::getSetterCallback() const
{
  return m_SetterCallback;
}

// -----------------------------------------------------------------------------
void EbsdMontageImportFilterParameter::setGetterCallback(const EbsdMontageImportFilterParameter::GetterCallbackType& value)
{
  m_GetterCallback = value;
}

// -----------------------------------------------------------------------------
EbsdMontageImportFilterParameter::GetterCallbackType EbsdMontageImportFilterParameter::getGetterCallback() const
{
  return m_GetterCallback;
}
