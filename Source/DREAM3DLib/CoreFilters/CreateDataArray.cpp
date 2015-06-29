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

#include "CreateDataArray.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/TemplateHelpers.hpp"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/FilterParameters/ChoiceFilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateDataArray::CreateDataArray() :
  AbstractFilter(),
  m_ScalarType(0),
  m_NumberOfComponents(-1),
  m_NewArray("", "", ""),
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
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(FilterParameter::New("Number of Components", "NumberOfComponents", FilterParameterWidgetType::IntWidget, getNumberOfComponents(), FilterParameter::Parameter));
  parameters.push_back(FilterParameter::New("Initialization Value", "InitializationValue", FilterParameterWidgetType::DoubleWidget, getInitializationValue(), FilterParameter::Parameter));

  parameters.push_back(FilterParameter::New("Created Attribute Array", "NewArray", FilterParameterWidgetType::DataArrayCreationWidget, getNewArray(), FilterParameter::CreatedArray));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateDataArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setScalarType( reader->readValue("ScalarType", getScalarType()) );
  setNumberOfComponents( reader->readValue("NumberOfComponents", getNumberOfComponents()) );
  setNewArray(reader->readDataArrayPath("NewArray", getNewArray()));
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
  DREAM3D_FILTER_WRITE_PARAMETER(ScalarType)
  DREAM3D_FILTER_WRITE_PARAMETER(NumberOfComponents)
  DREAM3D_FILTER_WRITE_PARAMETER(NewArray)
  DREAM3D_FILTER_WRITE_PARAMETER(InitializationValue)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void checkInitializationInt(AbstractFilter* filter, double initValue, int32_t err)
{
  filter->setErrorCondition(0);
  QString ss;
  typename DataArray<T>::Pointer var = DataArray<T>::CreateArray(1, "_INTERNAL_USE_ONLY_DO_NOT_USE"); // temporary for use of getTypeAsString()
  QString strType = var->getTypeAsString();
  strType.remove("_t");

  if (!((initValue >= std::numeric_limits<T>::min()) && (initValue <= std::numeric_limits<T>::max()))){
    filter->setErrorCondition(err);
    ss = QObject::tr("The %1 initialization value was invalid. The valid range is %2 to %3").arg(strType).arg(std::numeric_limits<T>::min()).arg(std::numeric_limits<T>::max());
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
void checkInitializationFloatDouble(AbstractFilter* filter, double initValue, int32_t err)
{
  filter->setErrorCondition(0);
  QString ss;
  typename DataArray<T>::Pointer var = DataArray<T>::CreateArray(1, "_INTERNAL_USE_ONLY_DO_NOT_USE"); // temporary for use of getTypeAsString()
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
      QString ss = QObject::tr("Incorrect data scalar type");
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

  checkInitialization(); // check the initialization value range for that data type
  if (getErrorCondition() < 0) { return; }

  QVector<size_t> cDims(1, getNumberOfComponents());
  m_OutputArrayPtr = TemplateHelpers::CreateNonPrereqArrayFromTypeEnum()(this, getNewArray(), cDims, getScalarType(), getInitializationValue());
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
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  { return; }

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
{ return Core::CoreBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateDataArray::getGroupName()
{ return DREAM3D::FilterGroups::CoreFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateDataArray::getSubGroupName()
{ return DREAM3D::FilterSubGroups::GenerationFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateDataArray::getHumanLabel()
{ return "Create Data Array"; }
