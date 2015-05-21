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


#include "ExtractComponentAsArray.h"

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExtractComponentAsArray::ExtractComponentAsArray() :
  AbstractFilter(),
  m_SelectedArrayPath("", "", ""),
  m_CompNumber(0),
  m_NewArrayArrayName(""),
  m_NewArray(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ExtractComponentAsArray::~ExtractComponentAsArray()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractComponentAsArray::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("MultiComponent Array", "SelectedArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedArrayPath(), false));
  parameters.push_back(FilterParameter::New("Component Number to Extract", "CompNumber", FilterParameterWidgetType::IntWidget, getCompNumber(), false));
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("New Array Name", "NewArrayArrayName", FilterParameterWidgetType::StringWidget, getNewArrayArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractComponentAsArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNewArrayArrayName(reader->readString("NewArrayArrayName", getNewArrayArrayName() ) );
  setCompNumber(reader->readValue("CompNumber", getCompNumber() ) );
  setSelectedArrayPath( reader->readDataArrayPath( "SelectedArrayPath", getSelectedArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ExtractComponentAsArray::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(CompNumber)
  DREAM3D_FILTER_WRITE_PARAMETER(NewArrayArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedArrayPath)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractComponentAsArray::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  if(m_SelectedArrayPath.isEmpty() == true)
  {
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), "An array from the Volume DataContainer must be selected.", getErrorCondition());
  }
  else
  {
    IDataArray::Pointer inputData = getDataContainerArray()->getDataContainer(m_SelectedArrayPath.getDataContainerName())->getAttributeMatrix(m_SelectedArrayPath.getAttributeMatrixName())->getAttributeArray(m_SelectedArrayPath.getDataArrayName());
    if (NULL == inputData.get())
    {
      QString ss = QObject::tr("Data array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_SelectedArrayPath.getDataArrayName());
      setErrorCondition(-11001);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
    else
    {
      if(inputData->getNumberOfComponents() < 2)
      {
        QString ss = QObject::tr("Data array '%1' MUST have more than 1 component").arg(m_SelectedArrayPath.getDataArrayName());
        setErrorCondition(-11002);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
      if(m_NewArrayArrayName.isEmpty() == true)
      {
        setErrorCondition(-11003);
        notifyErrorMessage(getHumanLabel(), "New array name must be set.", getErrorCondition());
        return;
      }
      if(m_CompNumber >= inputData->getNumberOfComponents())
      {
        setErrorCondition(-11003);
        QString ss = QObject::tr("Component to extract is larger than the number of components for array selected: '%1'").arg(m_SelectedArrayPath.getDataArrayName());
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }
      QVector<size_t> dims(1, 1);
      IDataArray::Pointer newData = inputData->createNewArray(inputData->getNumberOfTuples(), dims, m_NewArrayArrayName, !getInPreflight());
      getDataContainerArray()->getDataContainer(m_SelectedArrayPath.getDataContainerName())->getAttributeMatrix(m_SelectedArrayPath.getAttributeMatrixName())->addAttributeArray(m_NewArrayArrayName, newData);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractComponentAsArray::preflight()
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
void extractComponent(IDataArray::Pointer inputData, IDataArray::Pointer newData, int compNumber)
{
  DataArray<T>* inputArrayPtr = DataArray<T>::SafePointerDownCast(inputData.get());
  DataArray<T>* newArrayPtr = DataArray<T>::SafePointerDownCast(newData.get());
  if (NULL == inputArrayPtr || NULL == newArrayPtr)
  {
    return;
  }

  T* inputArray = inputArrayPtr->getPointer(0);
  T* newArray = newArrayPtr->getPointer(0);
  size_t numPoints = inputArrayPtr->getNumberOfTuples();
  size_t numComps = inputArrayPtr->getNumberOfComponents();

  for (size_t i = 0; i < numPoints; i++)
  {
    newArray[i] = inputArray[numComps*i + compNumber];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ExtractComponentAsArray::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_SelectedArrayPath.getDataContainerName());

  QString ss;

  IDataArray::Pointer inputData = getDataContainerArray()->getDataContainer(m_SelectedArrayPath.getDataContainerName())->getAttributeMatrix(m_SelectedArrayPath.getAttributeMatrixName())->getAttributeArray(m_SelectedArrayPath.getDataArrayName());
  IDataArray::Pointer newData = getDataContainerArray()->getDataContainer(m_SelectedArrayPath.getDataContainerName())->getAttributeMatrix(m_SelectedArrayPath.getAttributeMatrixName())->getAttributeArray(m_NewArrayArrayName);

  QString dType = inputData->getTypeAsString();
  IDataArray::Pointer p = IDataArray::NullPointer();
  if (dType.compare("int8_t") == 0)
  {
    extractComponent<int8_t>(inputData, newData, m_CompNumber);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    extractComponent<uint8_t>(inputData, newData, m_CompNumber);
  }
  else if (dType.compare("int16_t") == 0)
  {
    extractComponent<int16_t>(inputData, newData, m_CompNumber);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    extractComponent<uint16_t>(inputData, newData, m_CompNumber);
  }
  else if (dType.compare("int32_t") == 0)
  {
    extractComponent<int32_t>(inputData, newData, m_CompNumber);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    extractComponent<uint32_t>(inputData, newData, m_CompNumber);
  }
  else if (dType.compare("int64_t") == 0)
  {
    extractComponent<int64_t>(inputData, newData, m_CompNumber);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    extractComponent<uint64_t>(inputData, newData, m_CompNumber);
  }
  else if (dType.compare("float") == 0)
  {
    extractComponent<float>(inputData, newData, m_CompNumber);
  }
  else if (dType.compare("double") == 0)
  {
    extractComponent<double>(inputData, newData, m_CompNumber);
  }
  else if (dType.compare("bool") == 0)
  {
    extractComponent<bool>(inputData, newData, m_CompNumber);
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ExtractComponentAsArray::newFilterInstance(bool copyFilterParameters)
{
  ExtractComponentAsArray::Pointer filter = ExtractComponentAsArray::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractComponentAsArray::getCompiledLibraryName()
{ return Core::CoreBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractComponentAsArray::getGroupName()
{ return DREAM3D::FilterGroups::CoreFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractComponentAsArray::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MemoryManagementFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ExtractComponentAsArray::getHumanLabel()
{ return "Extract Component As Attribute Array"; }

