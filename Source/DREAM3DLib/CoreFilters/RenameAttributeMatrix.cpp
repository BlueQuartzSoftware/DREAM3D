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

#include "RenameAttributeMatrix.h"

#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenameAttributeMatrix::RenameAttributeMatrix() :
  AbstractFilter(),
  m_SelectedAttributeMatrixPath("", "", ""),
  m_NewAttributeMatrix("")
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenameAttributeMatrix::~RenameAttributeMatrix()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameAttributeMatrix::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("AttributeMatrix to Rename", "SelectedAttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getSelectedAttributeMatrixPath(), false));
  parameters.push_back(FilterParameter::New("New AttributeMatrix Name", "NewAttributeMatrix", FilterParameterWidgetType::StringWidget, getNewAttributeMatrix(), false));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameAttributeMatrix::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedAttributeMatrixPath( reader->readDataArrayPath("SelectedAttributeMatrixPath", getSelectedAttributeMatrixPath()) );
  setNewAttributeMatrix( reader->readString( "NewAttributeMatrix", getNewAttributeMatrix() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RenameAttributeMatrix::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedAttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(NewAttributeMatrix)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameAttributeMatrix::dataCheck()
{
  setErrorCondition(0);

  if(m_NewAttributeMatrix.isEmpty() == true)
  {
    setErrorCondition(-11004);
    QString ss = QObject::tr("The New Attribute Matrix name can not be empty. Please set a value.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (m_SelectedAttributeMatrixPath.isEmpty() == true)
  {
    setErrorCondition(-11005);
    QString ss = QObject::tr("The complete path to the Attribute Matrix can not be empty. Please set an appropriate path.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  else
  {
    QString dcName = m_SelectedAttributeMatrixPath.getDataContainerName();
    QString amName = m_SelectedAttributeMatrixPath.getAttributeMatrixName();

    DataContainerArray::Pointer dca = getDataContainerArray();

    if (NULL == dca.get())
    {
      setErrorCondition(-11003);
      QString ss = QObject::tr("The DataContainerArray was not found. Please contact the DREAM3D developers for more information.");
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(dcName);

    if(NULL == dc.get())
    {
      setErrorCondition(-11007);
      QString ss = QObject::tr("The DataContainer '%1' was not found in the DataContainerArray").arg(dcName);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    AttributeMatrix::Pointer attrMat = dc->getAttributeMatrix(amName);
    if(NULL == attrMat.get())
    {
      setErrorCondition(-11008);
      QString ss = QObject::tr("The AttributeMatrix '%1' was not found in the DataContainer '%2'").arg(amName).arg(dcName);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    bool check = dc->renameAttributeMatrix(amName, getNewAttributeMatrix() );
    if(check == false)
    {
      setErrorCondition(-11006);
      QString ss = QObject::tr("Attempt to rename AttributeMatrix '%1' to '%2' Failed.").arg(amName).arg(getNewAttributeMatrix());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameAttributeMatrix::preflight()
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
void RenameAttributeMatrix::execute()
{
  setErrorCondition(0);

  dataCheck(); // calling the dataCheck will rename the array, so nothing is required here
  if(getErrorCondition() < 0)
  {
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer RenameAttributeMatrix::newFilterInstance(bool copyFilterParameters)
{
  /*
  * SelectedAttributeMatrixPath
  * NewAttributeMatrix
  */
  RenameAttributeMatrix::Pointer filter = RenameAttributeMatrix::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenameAttributeMatrix::getCompiledLibraryName()
{
  return Core::CoreBaseName;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenameAttributeMatrix::getGroupName()
{
  return DREAM3D::FilterGroups::CoreFilters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenameAttributeMatrix::getSubGroupName()
{
  return DREAM3D::FilterSubGroups::MemoryManagementFilters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString RenameAttributeMatrix::getHumanLabel()
{
  return "Rename Attribute Matrix";
}

