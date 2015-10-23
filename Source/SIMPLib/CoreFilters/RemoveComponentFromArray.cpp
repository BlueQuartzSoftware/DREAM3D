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


#include "RemoveComponentFromArray.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Common/TemplateHelpers.hpp"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/IntFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"

// Include the MOC generated file for this class
#include "moc_RemoveComponentFromArray.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveComponentFromArray::RemoveComponentFromArray() :
  AbstractFilter(),
  m_SelectedArrayPath("", "", ""),
  m_CompNumber(0),
  m_SaveRemovedComponent(false),
  m_NewArrayArrayName(""),
  m_ReducedArrayArrayName(""),
  m_InArray(NULL),
  m_NewArray(NULL),
  m_ReducedArray(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RemoveComponentFromArray::~RemoveComponentFromArray()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveComponentFromArray::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(IntFilterParameter::New("Component Number to Remove", "CompNumber", getCompNumber(), FilterParameter::Parameter));

  DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateCategoryRequirement(DREAM3D::Defaults::AnyPrimitive, DREAM3D::Defaults::AnyComponentSize, DREAM3D::AttributeMatrixObjectType::Any);
  parameters.push_back(DataArraySelectionFilterParameter::New("Multicomponent Attribute Array", "SelectedArrayPath", getSelectedArrayPath(), FilterParameter::RequiredArray, req));

  parameters.push_back(StringFilterParameter::New("Removed Component Attribute Array", "NewArrayArrayName", getNewArrayArrayName(), FilterParameter::CreatedArray));

  parameters.push_back(StringFilterParameter::New("Reduced Attribute Array", "ReducedArrayArrayName", getReducedArrayArrayName(), FilterParameter::CreatedArray));

  QStringList linkedProps;
  linkedProps.clear();
  linkedProps << "NewArrayArrayName";
  parameters.push_back(LinkedBooleanFilterParameter::New("Save Removed Component in New Array", "SaveRemovedComponent", getSaveRemovedComponent(), linkedProps, FilterParameter::Parameter));



  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveComponentFromArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNewArrayArrayName(reader->readString("NewArrayArrayName", getNewArrayArrayName() ) );
  setNewArrayArrayName(reader->readString("ReducedArrayArrayName", getReducedArrayArrayName() ) );
  setCompNumber(reader->readValue("CompNumber", getCompNumber() ) );
  setSelectedArrayPath( reader->readDataArrayPath( "SelectedArrayPath", getSelectedArrayPath() ) );
  setSaveRemovedComponent(reader->readValue("SaveRemovedComponent", getSaveRemovedComponent()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RemoveComponentFromArray::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(CompNumber)
  SIMPL_FILTER_WRITE_PARAMETER(NewArrayArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(ReducedArrayArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(SelectedArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(SaveRemovedComponent)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveComponentFromArray::dataCheck()
{
  setErrorCondition(0);

  m_InArrayPtr = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getSelectedArrayPath());

  if (m_SaveRemovedComponent == true)
  {
    if (m_NewArrayArrayName.isEmpty() == true)
    {
      setErrorCondition(-11001);
      notifyErrorMessage(getHumanLabel(), "Removed Component array name must be set.", getErrorCondition());
      return;
    }
  }

  if (m_ReducedArrayArrayName.isEmpty() == true)
  {
    setErrorCondition(-11002);
    notifyErrorMessage(getHumanLabel(), "Reduced array name must be set.", getErrorCondition());
    return;
  }

  if(getErrorCondition() < 0) { return; }

  if (m_InArrayPtr.lock()->getNumberOfComponents() < 2)
  {
    setErrorCondition(-11003);
    QString ss = QObject::tr("Selected array '%1' must have more than 1 component. The number of components is %2").arg(getSelectedArrayPath().getDataArrayName()).arg(m_InArrayPtr.lock()->getNumberOfComponents());
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if(m_CompNumber >= m_InArrayPtr.lock()->getNumberOfComponents())
  {
    setErrorCondition(-11004);
    QString ss = QObject::tr("Component to extract (%1) is larger than the number of components (%2) for array selected: '%1'").arg(m_CompNumber).arg(m_InArrayPtr.lock()->getNumberOfComponents()).arg(getSelectedArrayPath().getDataArrayName());
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QVector<size_t> cDims(1, 1);
  if (m_SaveRemovedComponent == true)
  {
    DataArrayPath tempPath(getSelectedArrayPath().getDataContainerName(), getSelectedArrayPath().getAttributeMatrixName(), getNewArrayArrayName());
    m_NewArrayPtr = TemplateHelpers::CreateNonPrereqArrayFromArrayType()(this, tempPath, cDims, m_InArrayPtr.lock());
  }

  cDims[0] = m_InArrayPtr.lock()->getNumberOfComponents() - 1;
  DataArrayPath tempPath2(getSelectedArrayPath().getDataContainerName(), getSelectedArrayPath().getAttributeMatrixName(), getReducedArrayArrayName());
  m_ReducedArrayPtr = TemplateHelpers::CreateNonPrereqArrayFromArrayType()(this, tempPath2, cDims, m_InArrayPtr.lock());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveComponentFromArray::preflight()
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
template<typename T>
void extractComponent(IDataArray::Pointer inputData, IDataArray::Pointer newData, IDataArray::Pointer reducedData, int compNumber)
{
  typename DataArray<T>::Pointer inputArrayPtr = boost::dynamic_pointer_cast<DataArray<T> >(inputData);
  typename DataArray<T>::Pointer newArrayPtr = boost::dynamic_pointer_cast<DataArray<T> >(newData);
  typename DataArray<T>::Pointer reducedArrayPtr = boost::dynamic_pointer_cast<DataArray<T> >(reducedData);

  if (NULL == inputArrayPtr || NULL == newArrayPtr) { return; }

  T* inputArray = inputArrayPtr->getPointer(0);
  T* newArray = newArrayPtr->getPointer(0);
  T* reducedArray = reducedArrayPtr->getPointer(0);

  size_t numPoints = inputArrayPtr->getNumberOfTuples();
  size_t numComps = inputArrayPtr->getNumberOfComponents();

  for (size_t i = 0; i < numPoints; i++)
  {
    for (size_t j = 0; j < numComps; j++)
    {
      if (j == compNumber)
      {
        newArray[i] = inputArray[numComps * i + j];
      }
      else if (j > compNumber)
      {
        reducedArray[(numComps - 1)*i + j - 1] = inputArray[numComps * i + j];
      }
      else if (j < compNumber)
      {
        reducedArray[(numComps - 1)*i + j] = inputArray[numComps * i + j];
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void reduceArrayOnly(IDataArray::Pointer inputData, IDataArray::Pointer reducedData, int compNumber)
{
  typename DataArray<T>::Pointer inputArrayPtr = boost::dynamic_pointer_cast<DataArray<T> >(inputData);
  typename DataArray<T>::Pointer reducedArrayPtr = boost::dynamic_pointer_cast<DataArray<T> >(reducedData);

  if (NULL == inputArrayPtr) { return; }

  T* inputArray = inputArrayPtr->getPointer(0);
  T* reducedArray = reducedArrayPtr->getPointer(0);

  size_t numPoints = inputArrayPtr->getNumberOfTuples();
  size_t numComps = inputArrayPtr->getNumberOfComponents();

  for (size_t i = 0; i < numPoints; i++)
  {
    for (size_t j = 0; j < numComps; j++)
    {

      if (j > compNumber)
      {
        reducedArray[(numComps - 1)*i + j - 1] = inputArray[numComps * i + j];
      }
      else if (j < compNumber)
      {
        reducedArray[(numComps - 1)*i + j] = inputArray[numComps * i + j];
      }
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveComponentFromArray::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  if (m_SaveRemovedComponent == true)
  {
    EXECUTE_FUNCTION_TEMPLATE(this, extractComponent, m_InArrayPtr.lock(), m_InArrayPtr.lock(), m_NewArrayPtr.lock(), m_ReducedArrayPtr.lock(), m_CompNumber)
  }
  else if (m_SaveRemovedComponent == false)
  {
    EXECUTE_FUNCTION_TEMPLATE(this, reduceArrayOnly, m_InArrayPtr.lock(), m_InArrayPtr.lock(), m_ReducedArrayPtr.lock(), m_CompNumber)
  }


  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RemoveComponentFromArray::newFilterInstance(bool copyFilterParameters)
{
  RemoveComponentFromArray::Pointer filter = RemoveComponentFromArray::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveComponentFromArray::getCompiledLibraryName()
{ return Core::CoreBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveComponentFromArray::getGroupName()
{ return DREAM3D::FilterGroups::CoreFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveComponentFromArray::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MemoryManagementFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RemoveComponentFromArray::getHumanLabel()
{ return "Remove Component From Array"; }
