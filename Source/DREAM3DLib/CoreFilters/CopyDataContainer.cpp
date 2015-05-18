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


#include "CopyDataContainer.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CopyDataContainer::CopyDataContainer() :
  AbstractFilter(),
  m_SelectedDataContainerName(""),
  m_NewDataContainerName("")
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CopyDataContainer::~CopyDataContainer()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyDataContainer::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("DataContainer to Copy", "SelectedDataContainerName", FilterParameterWidgetType::DataContainerSelectionWidget, getSelectedDataContainerName(), false));
  parameters.push_back(FilterParameter::New("New DataContainer Name", "NewDataContainerName", FilterParameterWidgetType::StringWidget, getNewDataContainerName(), false));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyDataContainer::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedDataContainerName( reader->readString("SelectedDataContainerName", getSelectedDataContainerName()) );
  setNewDataContainerName( reader->readString( "NewDataContainerName", getNewDataContainerName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CopyDataContainer::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedDataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(NewDataContainerName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyDataContainer::dataCheck()
{
  setErrorCondition(0);

  if(getSelectedDataContainerName().isEmpty() == true)
  {
    setErrorCondition(-11000);
    QString ss = QObject::tr("The Selected Data Container name can not be empty. Please set a value.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(getNewDataContainerName().isEmpty() == true)
  {
    setErrorCondition(-11001);
    QString ss = QObject::tr("The New Data Container name can not be empty. Please set a value.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  //If either one of the strings are empty we can not go on.
  if(getErrorCondition() < 0)
  {
    return;
  }

  DataContainerArray::Pointer dca = getDataContainerArray();
  if (NULL == dca.get() )
  {
    setErrorCondition(-11003);
    QString ss = QObject::tr("The DataContainerArray was not found. Please contact the DREAM3D developers for more information.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  DataContainer::Pointer dc = dca->getDataContainer(getSelectedDataContainerName());
  if(NULL == dc.get())
  {
    setErrorCondition(-11002);
    QString ss = QObject::tr("The Selected Data Container was not found in the DataContainerArray. Are you sure it exists? The Spelling and Capitalization must also match.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  DataContainer::Pointer dcNew = dc->deepCopy();
  dcNew->setName(getNewDataContainerName());
  dca->addDataContainer(dcNew);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyDataContainer::preflight()
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
void CopyDataContainer::execute()
{
  setErrorCondition(0);

  dataCheck(); // calling the dataCheck will copy the array, so nothing is required here
  if(getErrorCondition() < 0)
  {
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CopyDataContainer::newFilterInstance(bool copyFilterParameters)
{
  /*
  * SelectedAttributeMatrixPath
  * NewAttributeMatrix
  */
  CopyDataContainer::Pointer filter = CopyDataContainer::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyDataContainer::getCompiledLibraryName()
{
  return Core::CoreBaseName;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyDataContainer::getGroupName()
{
  return DREAM3D::FilterGroups::CoreFilters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyDataContainer::getSubGroupName()
{
  return DREAM3D::FilterSubGroups::MemoryManagementFilters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyDataContainer::getHumanLabel()
{
  return "Copy Data Container";
}

