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

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/FilterParameters/DataContainerSelectionFilterParameter.h"
#include "DREAM3DLib/FilterParameters/StringFilterParameter.h"

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

  {
    DataContainerSelectionFilterParameter::RequirementType req;
    parameters.push_back(DataContainerSelectionFilterParameter::New("Data Container to Copy", "SelectedDataContainerName", getSelectedDataContainerName(), FilterParameter::RequiredArray, req));
  }

  parameters.push_back(StringFilterParameter::New("Copied Data Container", "NewDataContainerName", getNewDataContainerName(), FilterParameter::CreatedArray));

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

  if (getNewDataContainerName().isEmpty() == true)
  {
    setErrorCondition(-11001);
    QString ss = QObject::tr("The new Data Container name must be set");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  DataContainer::Pointer dc = getDataContainerArray()->getPrereqDataContainer<AbstractFilter>(this, getSelectedDataContainerName());
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer dcNew = dc->deepCopy();
  dcNew->setName(getNewDataContainerName());
  getDataContainerArray()->addDataContainer(dcNew);
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
  if(getErrorCondition() < 0) { return; }

  notifyStatusMessage(getHumanLabel(), "Complete");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CopyDataContainer::newFilterInstance(bool copyFilterParameters)
{
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
{ return Core::CoreBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyDataContainer::getGroupName()
{ return DREAM3D::FilterGroups::CoreFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyDataContainer::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MemoryManagementFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyDataContainer::getHumanLabel()
{ return "Copy Data Container"; }
