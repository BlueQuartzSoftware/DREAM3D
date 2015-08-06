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

#include "ConditionalSetValue.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/TemplateHelpers.hpp"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/FilterParameters/DoubleFilterParameter.h"
#include "DREAM3DLib/FilterParameters/DataArraySelectionFilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConditionalSetValue::ConditionalSetValue() :
  AbstractFilter(),
  m_SelectedArrayPath("", "", ""),
  m_ConditionalArrayPath("", "", ""),
  m_ReplaceValue(0.0),
  m_Array(NULL),
  m_ConditionalArray(NULL)
  {
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConditionalSetValue::~ConditionalSetValue()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConditionalSetValue::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(DoubleFilterParameter::New("New Value", "ReplaceValue", getReplaceValue(), FilterParameter::Parameter));
  parameters.push_back(DataArraySelectionFilterParameter::New("Conditional Array", "ConditionalArrayPath", getConditionalArrayPath(), FilterParameter::RequiredArray));
  parameters.push_back(DataArraySelectionFilterParameter::New("Attribute Array", "SelectedArrayPath", getSelectedArrayPath(), FilterParameter::RequiredArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConditionalSetValue::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setConditionalArrayPath(reader->readDataArrayPath("ConditionalArrayPath", getConditionalArrayPath()));
  setSelectedArrayPath(reader->readDataArrayPath("SelectedArrayPath", getSelectedArrayPath()));
  setReplaceValue(reader->readValue("ReplaceValue", getReplaceValue()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ConditionalSetValue::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(ConditionalArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ReplaceValue)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void checkValuesInt(AbstractFilter* filter, double replaceValue, QString strType)
{
  QString ss;

  if (!((replaceValue >= std::numeric_limits<T>::min()) && (replaceValue <= std::numeric_limits<T>::max())))
  {
    ss = QObject::tr("The %1 replace value was invalid. The valid range is %2 to %3").arg(strType).arg(std::numeric_limits<T>::min()).arg(std::numeric_limits<T>::max());
    filter->setErrorCondition(-100);
    filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void checkValuesFloatDouble(AbstractFilter* filter, double replaceValue, QString strType)
{
  QString ss;

  if (!(((replaceValue >= static_cast<T>(-1) * std::numeric_limits<T>::max()) && (replaceValue <= static_cast<T>(-1) * std::numeric_limits<T>::min())) ||
    (replaceValue == 0) || ((replaceValue >= std::numeric_limits<T>::min()) && (replaceValue <= std::numeric_limits<T>::max()))))
  {
    ss = QObject::tr("The %1 replace value was invalid. The valid ranges are -%3 to -%2, 0, %2 to %3").arg(strType).arg(std::numeric_limits<T>::min()).arg(std::numeric_limits<T>::max());
    filter->setErrorCondition(-101);
    filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

template<typename T>
void replaceValue(AbstractFilter* filter, IDataArray::Pointer inDataPtr, BoolArrayType::Pointer condDataPtr, double replaceValue)
{
  typename DataArray<T>::Pointer inputArrayPtr = boost::dynamic_pointer_cast<DataArray<T> >(inDataPtr);

  T replaceVal = static_cast<T>(replaceValue);

  T* inData = inputArrayPtr->getPointer(0);
  bool* condData = condDataPtr->getPointer(0);
  size_t numTuples = inputArrayPtr->getNumberOfTuples();

  for (size_t iter = 0; iter < numTuples; iter++)
  {
    if (condData[iter] == true) { inData[iter] = replaceVal; }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConditionalSetValue::dataCheck()
{
  setErrorCondition(0);

  QVector<DataArrayPath> dataArrayPaths;

  m_ArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getSelectedArrayPath());
  if (getErrorCondition() < 0) { return; }
  if (getErrorCondition() >= 0) { dataArrayPaths.push_back(getSelectedArrayPath()); }

  if (m_ArrayPtr.lock()->getNumberOfComponents() > 1)
  {
    QString ss = QObject::tr("Selected array '%1' must be a scalar array (1 component). The number of components is %2").arg(getSelectedArrayPath().getDataArrayName()).arg(m_ArrayPtr.lock()->getNumberOfComponents());
    setErrorCondition(-11002);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<size_t> cDims(1, 1);
  m_ConditionalArrayPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getConditionalArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if (NULL != m_ConditionalArrayPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
    m_ConditionalArray = m_ConditionalArrayPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  if (getErrorCondition() >= 0) { dataArrayPaths.push_back(getConditionalArrayPath()); }

  getDataContainerArray()->validateNumberOfTuples<AbstractFilter>(this, dataArrayPaths);

  QString dType = m_ArrayPtr.lock()->getTypeAsString();
  if (dType.compare(DREAM3D::TypeNames::Int8) == 0) { checkValuesInt<int8_t>(this, m_ReplaceValue, DREAM3D::TypeNames::Int8); }
  else if (dType.compare(DREAM3D::TypeNames::UInt8) == 0) { checkValuesInt<uint8_t>(this, m_ReplaceValue, DREAM3D::TypeNames::UInt8); }
  else if (dType.compare(DREAM3D::TypeNames::Int16) == 0) { checkValuesInt<int16_t>(this, m_ReplaceValue, DREAM3D::TypeNames::Int16); }
  else if (dType.compare(DREAM3D::TypeNames::UInt16) == 0) { checkValuesInt<uint16_t>(this, m_ReplaceValue, DREAM3D::TypeNames::UInt16); }
  else if (dType.compare(DREAM3D::TypeNames::Int32) == 0) { checkValuesInt<int32_t>(this, m_ReplaceValue, DREAM3D::TypeNames::Int32); }
  else if (dType.compare(DREAM3D::TypeNames::UInt32) == 0) { checkValuesInt<uint32_t>(this, m_ReplaceValue, DREAM3D::TypeNames::UInt32); }
  else if (dType.compare(DREAM3D::TypeNames::Int64) == 0) { checkValuesInt<int64_t>(this, m_ReplaceValue, DREAM3D::TypeNames::Int64); }
  else if (dType.compare(DREAM3D::TypeNames::UInt64) == 0) { checkValuesInt<uint64_t>(this, m_ReplaceValue, DREAM3D::TypeNames::UInt64); }
  else if (dType.compare(DREAM3D::TypeNames::Float) == 0) { checkValuesFloatDouble<float>(this, m_ReplaceValue, DREAM3D::TypeNames::Float); }
  else if (dType.compare(DREAM3D::TypeNames::Double) == 0) { checkValuesFloatDouble<double>(this, m_ReplaceValue, DREAM3D::TypeNames::Double); }
  else if (dType.compare(DREAM3D::TypeNames::Bool) == 0)
  {
    if (m_ReplaceValue != 0.0)
    {
      m_ReplaceValue = 1.0; // anything that is not a zero is a one
    }
  }
  else
  {
    setErrorCondition(-4060);
    QString ss = QObject::tr("Incorrect data scalar type");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConditionalSetValue::preflight()
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
void ConditionalSetValue::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0)
  { return; }

  EXECUTE_FUNCTION_TEMPLATE(this, replaceValue, m_ArrayPtr.lock(), this, m_ArrayPtr.lock(), m_ConditionalArrayPtr.lock(), m_ReplaceValue)

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ConditionalSetValue::newFilterInstance(bool copyFilterParameters)
{
  ConditionalSetValue::Pointer filter = ConditionalSetValue::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConditionalSetValue::getCompiledLibraryName()
{ return Core::CoreBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConditionalSetValue::getGroupName()
{ return DREAM3D::FilterGroups::CoreFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConditionalSetValue::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MemoryManagementFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConditionalSetValue::getHumanLabel()
{ return "Replace Value in Array (Conditional)"; }
