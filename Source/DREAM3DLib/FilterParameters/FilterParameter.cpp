/* ============================================================================
 * Copyright (c) 2014 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2014 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-10-D-5210
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
  m_ValueType(""),
  m_Advanced(false),
  m_Units(""),
  m_FileExtension(""),
  m_FileType(""),
  m_CastableValueType(""),
  m_Conditional(false),
  m_ConditionalProperty(""),
  m_ConditionalLabel("")
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter::Pointer FilterParameter::New(const QString& humanLabel, const QString& propertyName,
                                              const QString& widgetType, const QString& valueType,
                                              bool advanced,
                                              const QString& units,
                                              const QString& fileExtension,
                                              const QString& fileType,
                                              const QString& castableValueType)
{

  FilterParameter::Pointer ptr = FilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  ptr->setValueType(valueType);
  ptr->setAdvanced(advanced);
  ptr->setUnits(units);
  ptr->setFileExtension(fileExtension);
  ptr->setFileType(fileType);
  ptr->setCastableValueType(castableValueType);
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter::Pointer FilterParameter::New(const QString& humanLabel, const QString& propertyName,
                       const QString& widgetType, const QString& valueType,
                       bool advanced,
                       bool isConditional,
                       const QString& conditionalProperty,
                       const QString& conditionalLabel)
{
  FilterParameter::Pointer ptr = FilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  ptr->setValueType(valueType);
  ptr->setAdvanced(advanced);
  ptr->setConditional(isConditional);
  ptr->setConditionalProperty(conditionalProperty);
  ptr->setConditionalLabel(conditionalLabel);
  ptr->setUnits("");
  ptr->setFileExtension("");
  ptr->setFileType("");
  ptr->setCastableValueType("");
  return ptr;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter::~FilterParameter()
{}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ChoiceFilterParameter::ChoiceFilterParameter() :
  FilterParameter(),
  m_Editable(false)
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ChoiceFilterParameter::~ChoiceFilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ChoiceFilterParameter::Pointer ChoiceFilterParameter::New(const QString& humanLabel, const QString& propertyName,
                                                          const QString& widgetType, const QString& valueType,
                                                          QVector<QString> choices,
                                                          bool editable,
                                                          bool advanced)

{
  ChoiceFilterParameter::Pointer ptr = ChoiceFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  ptr->setValueType(valueType);
  ptr->setAdvanced(advanced);
  ptr->setUnits("");
  ptr->setFileExtension("");
  ptr->setFileType("");
  ptr->setCastableValueType("");

  ptr->setChoices(choices);
  ptr->setEditable(editable);

  return ptr;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonFilterParameter::ComparisonFilterParameter() :
  m_ShowOperators(true) {}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonFilterParameter::~ComparisonFilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ComparisonFilterParameter::Pointer ComparisonFilterParameter::New(const QString& humanLabel, const QString& propertyName,
                                                                  const QString& widgetType, const QString& valueType,
                                                                  QVector<QString> choices,
                                                                  bool showOperators,
                                                                  bool advanced)

{
  ComparisonFilterParameter::Pointer ptr = ComparisonFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  ptr->setValueType(valueType);
  ptr->setAdvanced(advanced);
  ptr->setUnits("");
  ptr->setFileExtension("");
  ptr->setFileType("");
  ptr->setCastableValueType("");

  ptr->setChoices(choices);
  ptr->setShowOperators(showOperators);

  return ptr;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ShapeTypesFilterParameter::ShapeTypesFilterParameter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ShapeTypesFilterParameter::~ShapeTypesFilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ShapeTypesFilterParameter::Pointer ShapeTypesFilterParameter::New(const QString& humanLabel, const QString& propertyName,
                                                                  const QString& widgetType, const QString& valueType,
                                                                  const QString& phaseTypeCountProperty,
                                                                  const QString& phaseTypeArrayPathProperty,
                                                                  bool advanced)
{
  ShapeTypesFilterParameter::Pointer ptr = ShapeTypesFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  ptr->setValueType(valueType);
  ptr->setAdvanced(advanced);
  ptr->setUnits("");
  ptr->setFileExtension("");
  ptr->setFileType("");
  ptr->setCastableValueType("");

  ptr->setPhaseTypeCountProperty(phaseTypeCountProperty);
  ptr->setPhaseTypeArrayPathProperty(phaseTypeArrayPathProperty);

  return ptr;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PreflightUpdatedValue::PreflightUpdatedValue()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PreflightUpdatedValue::~PreflightUpdatedValue()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PreflightUpdatedValue::Pointer PreflightUpdatedValue::New(const QString& humanLabel, const QString& propertyName,
                                                                  const QString& widgetType, const QString& valueType,
                                                                  bool advanced)
{
  PreflightUpdatedValue::Pointer ptr = PreflightUpdatedValue::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  ptr->setValueType(valueType);
  ptr->setAdvanced(advanced);
  ptr->setUnits("");
  ptr->setFileExtension("");
  ptr->setFileType("");
  ptr->setCastableValueType("");

  return ptr;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxyFilterParameter::DataContainerArrayProxyFilterParameter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxyFilterParameter::~DataContainerArrayProxyFilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxyFilterParameter::Pointer DataContainerArrayProxyFilterParameter::New(const QString& humanLabel, const QString& propertyName,
                                                                  const QString& widgetType, const QString& valueType,
                                                                  DataContainerArrayProxy proxy,
                                                                  bool advanced)
{
  DataContainerArrayProxyFilterParameter::Pointer ptr = DataContainerArrayProxyFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  ptr->setValueType(valueType);
  ptr->setAdvanced(advanced);
  ptr->setUnits("");
  ptr->setFileExtension("");
  ptr->setFileType("");
  ptr->setCastableValueType("");

  ptr->setDataContainerArrayProxy(proxy);
  return ptr;
}



