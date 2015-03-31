/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "CreateDataArray.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/TemplateHelpers.hpp"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateDataArray::CreateDataArray() :
  AbstractFilter(),
  m_AttributeMatrixPath(),
  m_ScalarType(0),
  m_NumberOfComponents(-1),
  m_OutputArrayName(""),
  m_InitializationValue(0.0)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateDataArray::~CreateDataArray()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateDataArray::setupFilterParameters()
{
  FilterParameterVector parameters;
  /* Place all your option initialization code here */
  parameters.push_back(FilterParameter::New("Input Information", "", FilterParameterWidgetType::SeparatorWidget, "", false));
  parameters.push_back(FilterParameter::New("AttributeMatrix Path", "AttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getAttributeMatrixPath(), false));
  parameters.push_back(FilterParameter::New("Output Array Name", "OutputArrayName", FilterParameterWidgetType::StringWidget, getOutputArrayName(), false));

  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Scalar Type");
    parameter->setPropertyName("ScalarType");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    QVector<QString> choices;
    choices.push_back("signed   int 8  bit");
    choices.push_back("unsigned int 8  bit");
    choices.push_back("signed   int 16 bit");
    choices.push_back("unsigned int 16 bit");
    choices.push_back("signed   int 32 bit");
    choices.push_back("unsigned int 32 bit");
    choices.push_back("signed   int 64 bit");
    choices.push_back("unsigned int 64 bit");
    choices.push_back("       Float 32 bit");
    choices.push_back("      Double 64 bit");
    choices.push_back("bool");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }

  parameters.push_back(FilterParameter::New("Number Of Components", "NumberOfComponents", FilterParameterWidgetType::IntWidget, getNumberOfComponents(), false));
  parameters.push_back(FilterParameter::New("Initialization Value", "InitializationValue", FilterParameterWidgetType::DoubleWidget, getInitializationValue(), false));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateDataArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setAttributeMatrixPath(reader->readDataArrayPath("AttributeMatrixPath", getAttributeMatrixPath() ) );
  setScalarType( reader->readValue("ScalarType", getScalarType()) );
  setNumberOfComponents( reader->readValue("NumberOfComponents", getNumberOfComponents()) );
  setOutputArrayName( reader->readString( "OutputArrayName", getOutputArrayName() ) );
  setInitializationValue(reader->readValue("InitializationValue", getInitializationValue()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CreateDataArray::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(AttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ScalarType)
  DREAM3D_FILTER_WRITE_PARAMETER(NumberOfComponents)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(InitializationValue)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void checkInitializationInt(AbstractFilter* filter, double initValue, int err)
{
  filter->setErrorCondition(0);
  QString ss;
  typename DataArray<T>::Pointer var = DataArray<T>::CreateArray(1, "DO NOT USE"); // temporary for use of getTypeAsString()
  QString strType = var->getTypeAsString();
  strType.remove("_t");

  if (!((initValue >= std::numeric_limits<T>::min()) && (initValue <= std::numeric_limits<T>::max()))){
    filter->setErrorCondition(err);
    ss = QObject::tr("The %1 initialization value was invalid. The valid range is %2 to %3.").arg(strType).arg(std::numeric_limits<T>::min()).arg(std::numeric_limits<T>::max());
  }

  if (filter->getErrorCondition() < 0)
  {
    filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void checkInitializationFloatDouble(AbstractFilter* filter, double initValue, int err)
{
  filter->setErrorCondition(0);
  QString ss;
  typename DataArray<T>::Pointer var = DataArray<T>::CreateArray(1, "DO NOT USE"); // temporary for use of getTypeAsString()
  QString strType = var->getTypeAsString();

  if (!(((initValue >= static_cast<T>(-1) * std::numeric_limits<T>::max()) && (initValue <= static_cast<T>(-1) * std::numeric_limits<T>::min())) ||
    (initValue == 0) || ((initValue >= std::numeric_limits<T>::min()) && (initValue <= std::numeric_limits<T>::max())))){
    filter->setErrorCondition(err);
    ss = QObject::tr("The %1 initialization value was invalid. The valid ranges are -%3 to -%2, 0, %2 to %3").arg(strType).arg(std::numeric_limits<T>::min()).arg(std::numeric_limits<T>::max());
  }

  if (filter->getErrorCondition() < 0)
  {
    filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateDataArray::checkInitialization()
{
  setErrorCondition(0);

  switch (m_ScalarType) // check user input value to data type
  {
    case DREAM3D::TypeEnums::Int8:
      checkInitializationInt<int8_t>(this, m_InitializationValue, -4050);
      break;
    case DREAM3D::TypeEnums::UInt8:
      checkInitializationInt<uint8_t>(this, m_InitializationValue, -4051);
      break;
    case DREAM3D::TypeEnums::Int16:
      checkInitializationInt<int16_t>(this, m_InitializationValue, -4052);
      break;
    case DREAM3D::TypeEnums::UInt16:
      checkInitializationInt<uint16_t>(this, m_InitializationValue, -4053);
      break;
    case DREAM3D::TypeEnums::Int32:
      checkInitializationInt<int32_t>(this, m_InitializationValue, -4054);
      break;
    case DREAM3D::TypeEnums::UInt32:
      checkInitializationInt<uint32_t>(this, m_InitializationValue, -4055);
      break;
    case DREAM3D::TypeEnums::Int64:
      checkInitializationInt<int64_t>(this, m_InitializationValue, -4056);
      break;
    case DREAM3D::TypeEnums::UInt64:
      checkInitializationInt<uint64_t>(this, m_InitializationValue, -4057);
      break;
    case DREAM3D::TypeEnums::Float:
      checkInitializationFloatDouble<float>(this, m_InitializationValue, -4058);
      break;
    case DREAM3D::TypeEnums::Double:
      checkInitializationFloatDouble<double>(this, m_InitializationValue, -4059);
      break;
    case DREAM3D::TypeEnums::Bool:
    {
      if (m_InitializationValue != 0.0)
      {
        m_InitializationValue = 1.0; // anything that is not a zero is a one
      }
      break;
    }
    default:
    {
      setErrorCondition(-4060);
      QString ss = QObject::tr("Incorrect data scalar type.");
      break;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateDataArray::dataCheck()
{
  setErrorCondition(0);
  QString ss;
  if(getAttributeMatrixPath().getDataContainerName().isEmpty() == true || getAttributeMatrixPath().getAttributeMatrixName().isEmpty() == true)
  {
    ss = QObject::tr("The AttributeMatrix Path has empty elements: DataContainer->'%1'  AttributeMatrix->'%2'").arg(getAttributeMatrixPath().getDataContainerName()).arg(getAttributeMatrixPath().getAttributeMatrixName());
    setErrorCondition(-4000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(getScalarType() < 0 || getScalarType() > 10)
  {
    ss = QObject::tr("The scalar type was invalid. Valid values range from 0 to 10.");
    setErrorCondition(-4002);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (getNumberOfComponents() <= 0)
  {
    setErrorCondition(-4003);
    QString ss = QObject::tr("The Number of Components can not be zero or less. The current value is '%1'. Please set a value greater than zero.").arg(m_NumberOfComponents);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(getOutputArrayName().isEmpty() == true)
  {
    ss = QObject::tr("The Output Array Name is blank (empty) and a value must be filled in for the pipeline to complete.");
    setErrorCondition(-4001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  if (getErrorCondition() < 0) { return; }

  checkInitialization(); // check the initialization value range for that data type
  if (getErrorCondition() < 0) { return; }

  DataArrayPath tempPath = getAttributeMatrixPath();
  QVector<size_t> cDims(1, getNumberOfComponents());
  tempPath.setDataArrayName(getOutputArrayName());

  m_OutputArrayPtr = TemplateHelpers::CreateNonPrereqArrayFromTypeEnum()(this, tempPath, cDims, getScalarType(), getInitializationValue());
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateDataArray::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateDataArray::execute()
{
  int err = 0;
  QString ss;
  setErrorCondition(err);

  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CreateDataArray::newFilterInstance(bool copyFilterParameters)
{
  CreateDataArray::Pointer filter = CreateDataArray::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateDataArray::getCompiledLibraryName()
{
  return Core::CoreBaseName;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateDataArray::getGroupName()
{
  return DREAM3D::FilterGroups::CoreFilters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateDataArray::getSubGroupName()
{
  return DREAM3D::FilterSubGroups::GenerationFilters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateDataArray::getHumanLabel()
{
  return "Create Data Array";
}

