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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RenameAttributeMatrix::RenameAttributeMatrix() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_AttributeMatrixName(DREAM3D::Defaults::AttributeMatrixName),
  m_SelectedAttributeMatrixPath(""),
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
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("AttributeMatrix to Rename");
    parameter->setPropertyName("SelectedAttributeMatrixPath");
    parameter->setWidgetType(FilterParameterWidgetType::AttributeMatrixSelectionWidget);
    parameter->setValueType("DataArrayPath");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("New AttributeMatrix Name");
    parameter->setPropertyName("NewAttributeMatrix");
    parameter->setWidgetType(FilterParameterWidgetType::StringWidget);
    parameter->setValueType("QString");
    parameters.push_back(parameter);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameAttributeMatrix::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  //  setDataContainerName( reader->readString("DataContainerName", getDataContainerName()) );
  //  setAttributeMatrixName( reader->readString("AttributeMatrixName", getAttributeMatrixName()) );
  setSelectedAttributeMatrixPath( reader->readDataArrayPath("SelectedAttributeMatrixPath", getSelectedAttributeMatrixPath()) );
  setNewAttributeMatrix( reader->readString( "NewAttributeMatrix", getNewAttributeMatrix() ) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int RenameAttributeMatrix::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  //  writer->writeValue("DataContainerName", getDataContainerName());
  //  writer->writeValue("AttributeMatrixName", getAttributeMatrixName());
  writer->writeValue("SelectedAttributeMatrixPath", getSelectedAttributeMatrixPath() );
  writer->writeValue("NewAttributeMatrix", getNewAttributeMatrix() );
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
    setErrorCondition(-11000);
    QString ss = QObject::tr("The New Attribute Array name can not be empty. Please set a value.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if (m_SelectedAttributeMatrixPath.isEmpty() == true)
  {
    setErrorCondition(-11001);
    QString ss = QObject::tr("The complete path to the Attribute Array can not be empty. Please set an appropriate path.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else
  {

    QString dcName;
    QString amName;

    QStringList tokens = m_SelectedAttributeMatrixPath.split(DREAM3D::PathSep);
    // We should end up with 3 Tokens
    if(tokens.size() != 2)
    {
      setErrorCondition(-11002);
      QString ss = QObject::tr("The path to the Attribute Array is malformed. Each part should be separated by a '|' character.");
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
    else
    {
      dcName = tokens.at(0);
      amName = tokens.at(1);

      DataContainerArray::Pointer dca = getDataContainerArray();
      if (NULL == dca.get() ) { return; }
      DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(dcName);
      if(NULL == dc.get())
      {
        setErrorCondition(-11003);
        QString ss = QObject::tr("The DataContainer '%1' was not found in the DataContainerArray").arg(dcName);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }

      AttributeMatrix::Pointer attrMat = dc->getAttributeMatrix(amName);
      if(NULL == attrMat.get())
      {
        setErrorCondition(-11004);
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
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameAttributeMatrix::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameAttributeMatrix::execute()
{
  setErrorCondition(0);

  dataCheck(); // calling the dataCheck will rename the array, so nothing is required here
  if(getErrorCondition() < 0) { return; }

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
    filter->setSelectedAttributeMatrixPath( getSelectedAttributeMatrixPath() );
    filter->setNewAttributeMatrix( getNewAttributeMatrix() );
  }
  return filter;
}
