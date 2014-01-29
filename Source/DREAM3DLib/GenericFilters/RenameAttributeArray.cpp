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

#include "RenameAttributeArray.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenameAttributeArray::RenameAttributeArray() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_AttributeMatrixName(DREAM3D::Defaults::AttributeMatrixName),
  m_SelectedArrayName(""),
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
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Array to Rename");
    parameter->setPropertyName("SelectedArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::ArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("New Array Name");
    parameter->setPropertyName("NewArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::StringWidget);
    parameter->setValueType("QString");
    parameters.push_back(parameter);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameAttributeArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setDataContainerName( reader->readString("DataContainerName", getDataContainerName()) );
  setAttributeMatrixName( reader->readString("AttributeMatrixName", getAttributeMatrixName()) );
  setSelectedArrayName( reader->readString("SelectedArrayName", getSelectedArrayName()) );
  setNewArrayName( reader->readString( "NewArrayName", getNewArrayName() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RenameAttributeArray::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("DataContainerName", getDataContainerName());
  writer->writeValue("AttributeMatrixName", getAttributeMatrixName());
  writer->writeValue("SelectedArrayName", getSelectedArrayName());
  writer->writeValue("NewArrayName", getNewArrayName() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameAttributeArray::dataCheck()
{
  setErrorCondition(0);

  //  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  //  if(getErrorCondition() < 0 || NULL == m) { return; }
  //  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getAttributeMatrixName(), -301);
  //  if(getErrorCondition() < 0 || NULL == cellAttrMat.get() ) { return; }

  if(m_SelectedArrayName.isEmpty() == true)
  {
    setErrorCondition(-11000);
    QString ss = QObject::tr("An array from the Volume DataContainer must be selected.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else
  {

    // The name of the array is really the path
    DataContainer::Pointer dc = getDataContainerArray()->getDataContainer( getDataContainerName() );
    if (NULL == dc.get())
    {
      setErrorCondition(-11001);
      QString ss = QObject::tr("A DataContainer with the name '%1' was not found in the DataContainerArray").arg(getDataContainerName() );
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
    // We have the DataContainer, now get the AttributeMatrix
    AttributeMatrix::Pointer attrMat = dc->getAttributeMatrix(getAttributeMatrixName());
    if (NULL == attrMat.get())
    {
      setErrorCondition(-11002);
      QString ss = QObject::tr("An AttributeMatrix with the name '%1' was not found in the DataContainer").arg(getAttributeMatrixName());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    // We the AttributeMatrix, now lets try to get the AttributeArray object and rename it if it exists
    IDataArray::Pointer dataArray = attrMat->getAttributeArray(getSelectedArrayName());

    if(NULL == dataArray.get() )
    {
      setErrorCondition(-11003);
      QString ss = QObject::tr("A DataArray with the name '%1' was not found in the AttributeMatrix").arg(getSelectedArrayName());
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
    bool check = attrMat->renameAttributeArray(getSelectedArrayName(), m_NewArrayName);
    if(check == false)
    {
      setErrorCondition(-11004);
      QString ss = QObject::tr("Attempt to rename AttributeArray '%1' to '%2' Failed.").arg(getSelectedArrayName()).arg(m_NewArrayName);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
  }

  if(m_NewArrayName.isEmpty() == true)
  {
    setErrorCondition(-11000);
    QString ss = QObject::tr("The New Attribute Array name can not be empty. Please set a value.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameAttributeArray::preflight()
{
  emit preflightAboutToExecute();
  dataCheck();
  emit preflightExecuted();
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
