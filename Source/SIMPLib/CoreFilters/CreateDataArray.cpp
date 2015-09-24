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

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArrayCreationFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"

// Include the MOC generated file for this class
#include "moc_CreateDataArray.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateDataArray::CreateDataArray() :
  AbstractFilter(),
  m_ScalarType(0),
  m_NumberOfComponents(0),
  m_NewArray("", "", ""),
  m_InitializationValue("0")
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
  parameters.push_back(IntFilterParameter::New("Number of Components", "NumberOfComponents", getNumberOfComponents(), FilterParameter::Parameter));
  parameters.push_back(StringFilterParameter::New("Initialization Value", "InitializationValue", getInitializationValue(), FilterParameter::Parameter));

  {
    DataArrayCreationFilterParameter::RequirementType req;
    parameters.push_back(DataArrayCreationFilterParameter::New("Created Attribute Array", "NewArray", getNewArray(), FilterParameter::CreatedArray, req));
  }

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
  setInitializationValue(reader->readString("InitializationValue", getInitializationValue()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CreateDataArray::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion);
  SIMPL_FILTER_WRITE_PARAMETER(ScalarType);
  SIMPL_FILTER_WRITE_PARAMETER(NumberOfComponents);
  SIMPL_FILTER_WRITE_PARAMETER(NewArray);
  SIMPL_FILTER_WRITE_PARAMETER(InitializationValue);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void checkInitializationInt(AbstractFilter* filter, IDataArray::Pointer iDataArray, T initValue, bool ok, int32_t err)
{
  filter->setErrorCondition(0);
  QString ss;
  typename DataArray<T>::Pointer var = DataArray<T>::CreateArray(1, "_INTERNAL_USE_ONLY_DO_NOT_USE"); // temporary for use of getTypeAsString()
  QString strType = var->getTypeAsString();
  strType.remove("_t");

  if(!ok)
  {
    filter->setErrorCondition(err);
    ss = QObject::tr("The string could not be converted to type '%1'. The valid range is %2 to %3").arg(strType).arg(std::numeric_limits<T>::min()).arg(std::numeric_limits<T>::max());
  }

  if (filter->getErrorCondition() < 0)
  {
    filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
  }

  // Now set the initial Value into the array
  typename DataArray<T>::Pointer array = boost::dynamic_pointer_cast<DataArray<T> >(iDataArray);
  if(NULL != array.get())
  {
    array->initializeWithValue(initValue, 0);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void checkInitializationFloatDouble(AbstractFilter* filter, IDataArray::Pointer iDataArray, T initValue, bool ok, int32_t err)
{
  filter->setErrorCondition(0);
  QString ss;
  typename DataArray<T>::Pointer var = DataArray<T>::CreateArray(1, "_INTERNAL_USE_ONLY_DO_NOT_USE"); // temporary for use of getTypeAsString()
  QString strType = var->getTypeAsString();

  if(!ok)
  {
    filter->setErrorCondition(err);
    ss = QObject::tr("The %1 initialization value was invalid. The valid ranges are -%3 to -%2, 0, %2 to %3").arg(strType).arg(std::numeric_limits<T>::min()).arg(std::numeric_limits<T>::max());
    filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
  }

  /* This is special because floats/doubles can not represent numbers very close to Zero (+-). */
  if (!(((initValue >= static_cast<T>(-1) * std::numeric_limits<T>::max()) && (initValue <= static_cast<T>(-1) * std::numeric_limits<T>::min())) ||
        (initValue == 0) || ((initValue >= std::numeric_limits<T>::min()) && (initValue <= std::numeric_limits<T>::max()))))
  {
    filter->setErrorCondition(err);
    ss = QObject::tr("The %1 initialization value was invalid. The valid ranges are -%3 to -%2, 0, %2 to %3").arg(strType).arg(std::numeric_limits<T>::min()).arg(std::numeric_limits<T>::max());
    filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());

  }

  typename DataArray<T>::Pointer array = boost::dynamic_pointer_cast<DataArray<T> >(iDataArray);
  if(NULL != array.get())
  {
    array->initializeWithValue(initValue, 0);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateDataArray::checkInitialization()
{
  setErrorCondition(0);
  bool ok = false;
  switch (m_ScalarType) // check user input value to data type
  {
    case DREAM3D::TypeEnums::Int8:
    {
      int val = m_InitializationValue.toInt(&ok);
      if(val < -128 || val > 127) { ok = false; }
      checkInitializationInt<int8_t>(this, m_OutputArrayPtr.lock(), static_cast<int8_t>(val), ok, -4050);
      break;
    }
    case DREAM3D::TypeEnums::UInt8:
    {
      uint32_t val = m_InitializationValue.toUInt(&ok);
      if( val > 255) { ok = false; }
      checkInitializationInt<uint8_t>(this, m_OutputArrayPtr.lock(), static_cast<uint8_t>(val), ok, -4051);
      break;
    }
    case DREAM3D::TypeEnums::Int16:
    {
      int16_t i16 = static_cast<int16_t>(m_InitializationValue.toShort(&ok));
      checkInitializationInt<int16_t>(this, m_OutputArrayPtr.lock(), i16, ok, -4052);
      break;
    }
    case DREAM3D::TypeEnums::UInt16:
    {
      uint16_t ui16 = static_cast<uint16_t>(m_InitializationValue.toUShort(&ok));
      checkInitializationInt<uint16_t>(this, m_OutputArrayPtr.lock(), ui16, ok, -4053);
      break;
    }
    case DREAM3D::TypeEnums::Int32:
    {
      int32_t i32 = static_cast<int32_t>(m_InitializationValue.toInt(&ok));
      checkInitializationInt<int32_t>(this, m_OutputArrayPtr.lock(), i32, ok, -4054);
      break;
    }
    case DREAM3D::TypeEnums::UInt32:
    {
      uint32_t ui32 = static_cast<uint32_t>(m_InitializationValue.toUInt(&ok));
      checkInitializationInt<uint32_t>(this, m_OutputArrayPtr.lock(), ui32, ok, -4055);
      break;
    }
    case DREAM3D::TypeEnums::Int64:
    {
      int64_t i64 = static_cast<int64_t>(m_InitializationValue.toLongLong(&ok));
      checkInitializationInt<int64_t>(this, m_OutputArrayPtr.lock(), i64, ok, -4056);
      break;
    }
    case DREAM3D::TypeEnums::UInt64:
    {
      uint64_t ui64 = static_cast<uint64_t>(m_InitializationValue.toULongLong(&ok));
      checkInitializationInt<uint64_t>(this, m_OutputArrayPtr.lock(), ui64, ok, -4057);
      break;
    }
    case DREAM3D::TypeEnums::Float:
    {
      float f = static_cast<float>(m_InitializationValue.toFloat(&ok));
      checkInitializationFloatDouble<float>(this, m_OutputArrayPtr.lock(), f, ok, -4058);
      break;
    }
    case DREAM3D::TypeEnums::Double:
    {
      double d = static_cast<double>(m_InitializationValue.toFloat(&ok));
      checkInitializationFloatDouble<double>(this, m_OutputArrayPtr.lock(), d, ok, -4059);
      break;
    }
    case DREAM3D::TypeEnums::Bool:
    {
      int8_t b = static_cast<int8_t>(m_InitializationValue.toInt(&ok));
      if (b != 0)
      {
        m_InitializationValue = 1; // anything that is not a zero is a one
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

  if (getErrorCondition() < 0) { return; }

  if (getNumberOfComponents() < 0)
  {
    setErrorCondition(-8050);
    QString ss = QObject::tr("The number of components must non-negative");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  if(getNumberOfComponents() == 0)
  {
    setErrorCondition(0);
    QString ss = QObject::tr("The number of components is Zero. This will result in an array that has no memory allocated. Are you sure you wanted to do this?");
    notifyWarningMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  QVector<size_t> cDims(1, getNumberOfComponents());

  m_OutputArrayPtr = TemplateHelpers::CreateNonPrereqArrayFromTypeEnum()(this, getNewArray(), cDims, getScalarType(), 0.0);

  checkInitialization(); // check the initialization value range for that data type

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
