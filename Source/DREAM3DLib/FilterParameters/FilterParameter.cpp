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
  m_Advanced(false),
  m_Units(""),
  m_ReadOnly(false)
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter::Pointer FilterParameter::New(const QString& humanLabel, const QString& propertyName,
                                              const QString& widgetType, const QVariant& defaultValue,
                                              bool advanced,
                                              const QString& units)
{

  FilterParameter::Pointer ptr = FilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  ptr->setDefaultValue(defaultValue);
  ptr->setAdvanced(advanced);
  ptr->setUnits(units);
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
                                              bool advanced,
                                              const QString& units)
{

  FilterParameter::Pointer ptr = FilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setAdvanced(advanced);
  ptr->setUnits(units);
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
                                              bool advanced,
                                              const QString& units)
{

  FilterParameter::Pointer ptr = FilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setAdvanced(advanced);
  ptr->setUnits(units);
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
                                              bool advanced,
                                              const QString& units)
{

  FilterParameter::Pointer ptr = FilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setAdvanced(advanced);
  ptr->setUnits(units);
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
                                              bool advanced,
                                              const QString& units)
{
  FilterParameter::Pointer ptr = FilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setAdvanced(advanced);
  ptr->setUnits(units);
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
                                              bool advanced)
{
  FilterParameter::Pointer ptr = FilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setAdvanced(advanced);

  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }
  return ptr;
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter::Pointer FilterParameter::NewConditional(const QString& humanLabel, const QString& propertyName,
                                                         const QString& widgetType, const QVariant& defaultValue,
                                                         bool advanced,
                                                         QStringList linkedProperties)
{
  FilterParameter::Pointer ptr = FilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  ptr->setDefaultValue(defaultValue);
  ptr->setAdvanced(advanced);
  ptr->setUnits("");
  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }
  return ptr;
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameter::~FilterParameter()
{}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedBooleanFilterParameter::LinkedBooleanFilterParameter() :
  FilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedBooleanFilterParameter::~LinkedBooleanFilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedBooleanFilterParameter::Pointer LinkedBooleanFilterParameter::New(const QString& humanLabel, const QString& propertyName,
    const QVariant& defaultValue,
    QStringList conditionalProperties,
    bool advanced)
{
  LinkedBooleanFilterParameter::Pointer ptr = LinkedBooleanFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(FilterParameterWidgetType::LinkedBooleanWidget);
  ptr->setDefaultValue(defaultValue);
  ptr->setAdvanced(advanced);
  ptr->setConditionalProperties(conditionalProperties);
  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedChoicesFilterParameter::LinkedChoicesFilterParameter() :
  FilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedChoicesFilterParameter::~LinkedChoicesFilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LinkedChoicesFilterParameter::Pointer LinkedChoicesFilterParameter::New(const QString& humanLabel, const QString& propertyName,
    const QVariant& defaultValue,
    QStringList conditionalProperties,
    bool advanced)
{
  LinkedChoicesFilterParameter::Pointer ptr = LinkedChoicesFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(FilterParameterWidgetType::LinkedBooleanWidget);
  ptr->setDefaultValue(defaultValue);
  ptr->setAdvanced(advanced);
  ptr->setConditionalProperties(conditionalProperties);
  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }
  return ptr;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileSystemFilterParameter::FileSystemFilterParameter() :
  FilterParameter(),
  m_FileExtension(""),
  m_FileType("")
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileSystemFilterParameter::~FileSystemFilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileSystemFilterParameter::Pointer FileSystemFilterParameter::New(const QString& humanLabel, const QString& propertyName,
    const QString& widgetType, const QVariant& defaultValue,
    bool advanced,
    const QString& units,
    const QString& fileExtension,
    const QString& fileType)
{
  FileSystemFilterParameter::Pointer ptr = FileSystemFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  ptr->setDefaultValue(defaultValue);
  ptr->setAdvanced(advanced);
  ptr->setUnits(units);
  ptr->setFileExtension(fileExtension);
  ptr->setFileType(fileType);
  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }
  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeInfoFilterParameter::VolumeInfoFilterParameter() :
  FilterParameter(),
  m_DimensionsProperty(""),
  m_ResolutionProperty("")
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeInfoFilterParameter::~VolumeInfoFilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VolumeInfoFilterParameter::Pointer VolumeInfoFilterParameter::New(const QString& humanLabel, const QString& dimsProperty,
    const QString& widgetType, const IntVec3_t& defaultValue,
    bool advanced,
    const QString& units,
    const QString& resProperty)
{
  VolumeInfoFilterParameter::Pointer ptr = VolumeInfoFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(dimsProperty);
  ptr->setWidgetType(widgetType);
  QVariant v;
  v.setValue(defaultValue);
  ptr->setDefaultValue(v);
  ptr->setAdvanced(advanced);
  ptr->setUnits(units);
  ptr->setDimensionsProperty(dimsProperty);
  ptr->setResolutionProperty(resProperty);
  ptr->setReadOnly(true);
  return ptr;
}


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
ChoiceFilterParameter::Pointer ChoiceFilterParameter::New(const QString& humanLabel, const QString& propertyName, const QVariant& defaultValue,
                                                          QVector<QString> choices,
                                                          bool editable,
                                                          bool advanced)

{
  ChoiceFilterParameter::Pointer ptr = ChoiceFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
  ptr->setDefaultValue(defaultValue);
  ptr->setAdvanced(advanced);
  ptr->setUnits("");
  ptr->setChoices(choices);
  ptr->setEditable(editable);

  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }

  return ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicChoiceFilterParameter::DynamicChoiceFilterParameter() :
  FilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicChoiceFilterParameter::~DynamicChoiceFilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicChoiceFilterParameter::Pointer DynamicChoiceFilterParameter::New(const QString& humanLabel,
                        const QString& propertyName,
                       const QVariant& defaultValue,
                       const QString& listProperty,
                       bool advanced)
{
  DynamicChoiceFilterParameter::Pointer ptr = DynamicChoiceFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(FilterParameterWidgetType::DynamicChoiceWidget);
  ptr->setDefaultValue(defaultValue);
  ptr->setAdvanced(advanced);
  ptr->setUnits("");
  ptr->setListProperty(listProperty);

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
    const QString& widgetType, const QVariant& defaultValue,
    QVector<QString> choices,
    bool showOperators,
    bool advanced)

{
  ComparisonFilterParameter::Pointer ptr = ComparisonFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  ptr->setDefaultValue(defaultValue);
  ptr->setAdvanced(advanced);
  ptr->setUnits("");
  ptr->setChoices(choices);
  ptr->setShowOperators(showOperators);
  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }
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
    const QString& widgetType, const QVariant& defaultValue,
    const QString& phaseTypeCountProperty,
    const QString& phaseTypeArrayPathProperty,
    bool advanced)
{
  ShapeTypesFilterParameter::Pointer ptr = ShapeTypesFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  ptr->setDefaultValue(defaultValue);
  ptr->setAdvanced(advanced);
  ptr->setUnits("");

  ptr->setPhaseTypeCountProperty(phaseTypeCountProperty);
  ptr->setPhaseTypeArrayPathProperty(phaseTypeArrayPathProperty);
  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }
  return ptr;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseTypesFilterParameter::PhaseTypesFilterParameter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseTypesFilterParameter::~PhaseTypesFilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseTypesFilterParameter::Pointer PhaseTypesFilterParameter::New(const QString& humanLabel, const QString& propertyName,
    const QString& widgetType, const QVariant& defaultValue,
    const QString& phaseTypeCountProperty,
    const QString& phaseTypeArrayPathProperty,
    bool advanced)
{
  PhaseTypesFilterParameter::Pointer ptr = PhaseTypesFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  ptr->setDefaultValue(defaultValue);
  ptr->setAdvanced(advanced);
  ptr->setUnits("");

  ptr->setPhaseTypeCountProperty(phaseTypeCountProperty);
  ptr->setPhaseTypeArrayPathProperty(phaseTypeArrayPathProperty);
  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }
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
                                                          const QString& widgetType, const QVariant& defaultValue,
                                                          bool advanced)
{
  PreflightUpdatedValue::Pointer ptr = PreflightUpdatedValue::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  ptr->setDefaultValue(defaultValue);
  ptr->setAdvanced(advanced);
  ptr->setUnits("");
  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }
  return ptr;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxyFilterParameter::DataContainerArrayProxyFilterParameter() :
  FilterParameter(),
  m_DefaultFlagValue(Qt::Checked)
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
    const QString& widgetType, const QVariant& defaultValue,
    DataContainerArrayProxy proxy, Qt::CheckState defValue,
    bool advanced)
{
  DataContainerArrayProxyFilterParameter::Pointer ptr = DataContainerArrayProxyFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  ptr->setDefaultValue(defaultValue);
  ptr->setAdvanced(advanced);
  ptr->setUnits("");
  ptr->setDefaultFlagValue(defValue);
  ptr->setDataContainerArrayProxy(proxy);

  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }
  return ptr;
}




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerReaderFilterParameter::DataContainerReaderFilterParameter() :
  FilterParameter(),
  m_DefaultFlagValue(Qt::Checked)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerReaderFilterParameter::~DataContainerReaderFilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerReaderFilterParameter::Pointer DataContainerReaderFilterParameter::New(const QString& humanLabel, const QString& propertyName,
    const QString& widgetType, const QVariant& defaultValue,
    bool advanced)
{
  DataContainerReaderFilterParameter::Pointer ptr = DataContainerReaderFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setWidgetType(widgetType);
  ptr->setDefaultValue(defaultValue);
  ptr->setAdvanced(advanced);
  ptr->setUnits("");

  if(ptr->getWidgetType().compare(FilterParameterWidgetType::SeparatorWidget) == 0)
  {
    ptr->setReadOnly(true);
  }
  return ptr;
}



