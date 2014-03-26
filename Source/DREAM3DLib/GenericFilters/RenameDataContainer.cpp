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

#include "RenameDataContainer.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenameDataContainer::RenameDataContainer() :
  AbstractFilter(),
  m_SelectedDataContainerName(""),
  m_NewDataContainerName(""),
  m_ReplaceExistingDataContainer(false)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenameDataContainer::~RenameDataContainer()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameDataContainer::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("DataContainer to Rename");
    parameter->setPropertyName("SelectedDataContainerName");
    parameter->setWidgetType(FilterParameterWidgetType::DataContainerSelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("New DataContainer Name");
    parameter->setPropertyName("NewDataContainerName");
    parameter->setWidgetType(FilterParameterWidgetType::StringWidget);
    parameter->setValueType("QString");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Replace Existing DataContainer");
    parameter->setPropertyName("ReplaceExistingDataContainer");
    parameter->setWidgetType(FilterParameterWidgetType::BooleanWidget);
    parameter->setValueType("bool");
    parameters.push_back(parameter);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameDataContainer::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setSelectedDataContainerName( reader->readString("SelectedDataContainerName", getSelectedDataContainerName()) );
  setNewDataContainerName( reader->readString( "NewDataContainerName", getNewDataContainerName() ) );
  setReplaceExistingDataContainer(reader->readValue("ReplaceExistingDataContainer", getReplaceExistingDataContainer() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RenameDataContainer::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedDataContainerName", getSelectedDataContainerName() );
  writer->writeValue("NewDataContainerName", getNewDataContainerName() );
  writer->writeValue("ReplaceExistingDataContainer", getReplaceExistingDataContainer() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameDataContainer::dataCheck()
{
  setErrorCondition(0);

  if(getNewDataContainerName().isEmpty() == true)
  {
    setErrorCondition(-11000);
    QString ss = QObject::tr("The New Data Container name can not be empty. Please set a value.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (getSelectedDataContainerName().isEmpty() == true)
  {
    setErrorCondition(-11001);
    QString ss = QObject::tr("The complete path to the Data Container can not be empty. Please set an appropriate name.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else
  {
    DataContainerArray::Pointer dca = getDataContainerArray();
    if (NULL == dca.get() ) { return; }

    bool check = dca->renameDataContainer(getSelectedDataContainerName(), getNewDataContainerName(), getReplaceExistingDataContainer());
    if(check == false)
    {
      setErrorCondition(-11006);
      QString ss = QObject::tr("Attempt to rename DataContainer '%1' to '%2' Failed.").arg(getSelectedDataContainerName()).arg(getNewDataContainerName());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }

  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameDataContainer::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameDataContainer::execute()
{
  setErrorCondition(0);

  dataCheck(); // calling the dataCheck will rename the array, so nothing is required here
  if(getErrorCondition() < 0) { return; }

  notifyStatusMessage(getHumanLabel(), "Complete");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RenameDataContainer::newFilterInstance(bool copyFilterParameters)
{
  /*
  * SelectedAttributeMatrixPath
  * NewAttributeMatrix
  */
  RenameDataContainer::Pointer filter = RenameDataContainer::New();
  if(true == copyFilterParameters)
  {
    filter->setSelectedDataContainerName( getSelectedDataContainerName() );
    filter->setNewDataContainerName( getNewDataContainerName() );
    filter->setReplaceExistingDataContainer( getReplaceExistingDataContainer() );
  }
  return filter;
}
