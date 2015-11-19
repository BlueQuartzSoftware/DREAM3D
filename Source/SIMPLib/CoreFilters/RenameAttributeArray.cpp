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


#include "RenameAttributeArray.h"

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"

// Include the MOC generated file for this class
#include "moc_RenameAttributeArray.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenameAttributeArray::RenameAttributeArray() :
  AbstractFilter(),
  m_SelectedArrayPath("", "", ""),
  m_NewArrayName("")
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenameAttributeArray::~RenameAttributeArray()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameAttributeArray::setupFilterParameters()
{
  FilterParameterVector parameters;

  DataArraySelectionFilterParameter::RequirementType req;
  parameters.push_back(DataArraySelectionFilterParameter::New("Attribute Array to Rename", "SelectedArrayPath", getSelectedArrayPath(), FilterParameter::RequiredArray, req));

  parameters.push_back(StringFilterParameter::New("New Attribute Array Name", "NewArrayName", getNewArrayName(), FilterParameter::Parameter));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameAttributeArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedArrayPath( reader->readDataArrayPath("SelectedArrayPath", getSelectedArrayPath()) );
  setNewArrayName( reader->readString( "NewArrayName", getNewArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RenameAttributeArray::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(SelectedArrayPath)
  SIMPL_FILTER_WRITE_PARAMETER(NewArrayName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameAttributeArray::dataCheck()
{
  setErrorCondition(0);

  if (m_NewArrayName.isEmpty() == true)
  {
    setErrorCondition(-11009);
    QString ss = QObject::tr("The new Attribute Array name must be set");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  QString daName = getSelectedArrayPath().getDataArrayName();

  AttributeMatrix::Pointer attrMat = getDataContainerArray()->getPrereqAttributeMatrixFromPath<AbstractFilter>(this, getSelectedArrayPath(), -301);
  if(getErrorCondition() < 0) { return; }

  RenameErrorCodes code = attrMat->renameAttributeArray(daName, m_NewArrayName, false);
  switch (code)
  {
    case OLD_DOES_NOT_EXIST:
    {
      setErrorCondition(-11016);
      QString ss = QObject::tr("A DataArray with the name '%1' was not found in the AttributeMatrix").arg(daName);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
    case NEW_EXISTS:
    {
      setErrorCondition(-11017);
      QString ss = QObject::tr("A DataArray with the name '%1' already exists in the AttributeMatrix").arg(m_NewArrayName);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
    case SUCCESS:
    {
      setErrorCondition(0);
      return;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameAttributeArray::preflight()
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
void RenameAttributeArray::execute()
{
  setErrorCondition(0);
  dataCheck(); // calling the dataCheck will rename the array, so nothing is required here
  if(getErrorCondition() < 0) { return; }

  notifyStatusMessage(getHumanLabel(), "Complete");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RenameAttributeArray::newFilterInstance(bool copyFilterParameters)
{
  RenameAttributeArray::Pointer filter = RenameAttributeArray::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenameAttributeArray::getCompiledLibraryName()
{ return Core::CoreBaseName; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenameAttributeArray::getBrandingString()
{
  return "SIMPLib Core Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenameAttributeArray::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenameAttributeArray::getGroupName()
{ return DREAM3D::FilterGroups::CoreFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenameAttributeArray::getSubGroupName()
{ return DREAM3D::FilterSubGroups::MemoryManagementFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenameAttributeArray::getHumanLabel()
{ return "Rename Attribute Array"; }
