/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include "FilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter::FilterParameter() :
  m_HumanLabel(""),
  m_PropertyName(""),
  m_WidgetType(""),
  m_Category(Uncategorized),
  m_Units(""),
  m_ReadOnly(false),
  m_GroupIndex(-1)
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter::Pointer FilterParameter::New(const QString& humanLabel, const QString& propertyName,
                                              const QString& widgetType, const QVariant& defaultValue,
                                              Category category,
                                              const QString& units, int groupIndex)
{

  FilterParameter::Pointer ptr = FilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  ptr->setDefaultValue(defaultValue);
  ptr->setCategory(category);
  ptr->setUnits(units);
  ptr->setGroupIndex(groupIndex);
  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }
  return ptr;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter::Pointer FilterParameter::New(const QString& humanLabel, const QString& propertyName,
                                              const QString& widgetType, const FloatVec3_t& defaultValue,
                                              Category category,
                                              const QString& units, int groupIndex)
{

  FilterParameter::Pointer ptr = FilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setCategory(category);
  ptr->setUnits(units);
  ptr->setGroupIndex(groupIndex);
  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }
  return ptr;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter::Pointer FilterParameter::New(const QString& humanLabel, const QString& propertyName,
                                              const QString& widgetType, const FloatVec4_t& defaultValue,
                                              Category category,
                                              const QString& units, int groupIndex)
{

  FilterParameter::Pointer ptr = FilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setCategory(category);
  ptr->setUnits(units);
  ptr->setGroupIndex(groupIndex);
  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter::Pointer FilterParameter::New(const QString& humanLabel, const QString& propertyName,
                                              const QString& widgetType, const FloatVec21_t& defaultValue,
                                              Category category,
                                              const QString& units, int groupIndex)
{

  FilterParameter::Pointer ptr = FilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setCategory(category);
  ptr->setUnits(units);
  ptr->setGroupIndex(groupIndex);
  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter::Pointer FilterParameter::New(const QString& humanLabel, const QString& propertyName,
                                              const QString& widgetType, const Float2ndOrderPoly_t& defaultValue,
                                              Category category,
                                              const QString& units, int groupIndex)
{

  FilterParameter::Pointer ptr = FilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setCategory(category);
  ptr->setUnits(units);
  ptr->setGroupIndex(groupIndex);
  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter::Pointer FilterParameter::New(const QString& humanLabel, const QString& propertyName,
                                              const QString& widgetType, const Float3rdOrderPoly_t& defaultValue,
                                              Category category,
                                              const QString& units, int groupIndex)
{

  FilterParameter::Pointer ptr = FilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setCategory(category);
  ptr->setUnits(units);
  ptr->setGroupIndex(groupIndex);
  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter::Pointer FilterParameter::New(const QString& humanLabel, const QString& propertyName,
                                              const QString& widgetType, const Float4thOrderPoly_t& defaultValue,
                                              Category category,
                                              const QString& units, int groupIndex)
{

  FilterParameter::Pointer ptr = FilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setCategory(category);
  ptr->setUnits(units);
  ptr->setGroupIndex(groupIndex);
  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter::Pointer FilterParameter::New(const QString& humanLabel, const QString& propertyName,
                                              const QString& widgetType, const IntVec3_t& defaultValue,
                                              Category category,
                                              const QString& units, int groupIndex)
{

  FilterParameter::Pointer ptr = FilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setCategory(category);
  ptr->setUnits(units);
  ptr->setGroupIndex(groupIndex);
  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter::Pointer FilterParameter::New(const QString& humanLabel, const QString& propertyName,
                                              const QString& widgetType, const DataArrayPath& defaultValue,
                                              Category category,
                                              const QString& units, int groupIndex)
{

  FilterParameter::Pointer ptr = FilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setCategory(category);
  ptr->setUnits(units);
  ptr->setGroupIndex(groupIndex);
  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter::Pointer FilterParameter::New(const QString& humanLabel, const QString& propertyName,
                                              const QString& widgetType, const AxisAngleInput_t& defaultValue,
                                              Category category,
                                              const QString& units, int groupIndex)
{
  FilterParameter::Pointer ptr = FilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setCategory(category);
  ptr->setUnits(units);
  ptr->setGroupIndex(groupIndex);
  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter::Pointer FilterParameter::New(const QString& humanLabel, const QString& propertyName,
                                              const QString& widgetType, const UInt32Vector_t& defaultValue,
                                              Category category, const QString& units, int groupIndex)
{
  FilterParameter::Pointer ptr = FilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setCategory(category);
  ptr->setUnits(units);
  ptr->setGroupIndex(groupIndex);

  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }
  return ptr;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter::~FilterParameter()
{}
