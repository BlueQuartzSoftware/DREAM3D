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
#include "CreateDataArray.h"


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/TemplateHelpers.hpp"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateDataArray::CreateDataArray() :
  AbstractFilter(),
  m_AttributeMatrixPath(),
  m_ScalarType(0),
  m_NumberOfComponents(0),
  m_OutputArrayName(""),
  m_InitializationValue(0.0)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CreateDataArray::~CreateDataArray()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateDataArray::setupFilterParameters()
{
  FilterParameterVector parameters;
  /* Place all your option initialization code here */
  parameters.push_back(FilterParameter::New("Input Information", "", FilterParameterWidgetType::SeparatorWidget, "", false));
  parameters.push_back(FilterParameter::New("AttributeMatrix Path", "AttributeMatrixPath", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getAttributeMatrixPath(), false));
  parameters.push_back(FilterParameter::New("Output Array Name", "OutputArrayName", FilterParameterWidgetType::StringWidget, getOutputArrayName(), false));

  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Scalar Type");
    parameter->setPropertyName("ScalarType");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    QVector<QString> choices;
    choices.push_back("signed   int 8  bit");
    choices.push_back("unsigned int 8  bit");
    choices.push_back("signed   int 16 bit");
    choices.push_back("unsigned int 16 bit");
    choices.push_back("signed   int 32 bit");
    choices.push_back("unsigned int 32 bit");
    choices.push_back("signed   int 64 bit");
    choices.push_back("unsigned int 64 bit");
    choices.push_back("       Float 32 bit");
    choices.push_back("      Double 64 bit");
    choices.push_back("bool");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }

  parameters.push_back(FilterParameter::New("Number Of Components", "NumberOfComponents", FilterParameterWidgetType::IntWidget, getNumberOfComponents(), false));
  parameters.push_back(FilterParameter::New("Initialization Value", "InitializationValue", FilterParameterWidgetType::DoubleWidget, getInitializationValue(), false));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateDataArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setAttributeMatrixPath(reader->readDataArrayPath("AttributeMatrixPath", getAttributeMatrixPath() ) );
  setScalarType( reader->readValue("ScalarType", getScalarType()) );
  setNumberOfComponents( reader->readValue("NumberOfComponents", getNumberOfComponents()) );
  setOutputArrayName( reader->readString( "OutputArrayName", getOutputArrayName() ) );
  setInitializationValue(reader->readValue("InitializationValue", getInitializationValue()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CreateDataArray::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(AttributeMatrixPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ScalarType)
  DREAM3D_FILTER_WRITE_PARAMETER(NumberOfComponents)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(InitializationValue)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateDataArray::dataCheck()
{
  setErrorCondition(0);

  if(getAttributeMatrixPath().getDataContainerName().isEmpty() == true || getAttributeMatrixPath().getAttributeMatrixName().isEmpty() == true)
  {
    QString ss = QObject::tr("The AttributeMatrix Path has empty elements: DataContainer->'%1'  AttributeMatrix->'%2'").arg(getAttributeMatrixPath().getDataContainerName()).arg(getAttributeMatrixPath().getAttributeMatrixName());
    setErrorCondition(-4000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(getScalarType() < 0 || getScalarType() > 9)
  {
    QString ss = QObject::tr("The scalar type was invalid. Valid values range from 0 to 9.");
    setErrorCondition(-4002);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(getNumberOfComponents() < 1)
  {
    QString ss = QObject::tr("The number of components must be 1 or greater.");
    setErrorCondition(-4003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(getOutputArrayName().isEmpty() == true)
  {
    QString ss = QObject::tr("The Output Array Name is blank (empty) and a value must be filled in for the pipeline to complete.");
    setErrorCondition(-4001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  DataArrayPath tempPath = getAttributeMatrixPath();
  QVector<size_t> cDims(1, getNumberOfComponents());
  tempPath.setDataArrayName(getOutputArrayName());

  m_OutputArrayPtr = TemplateHelpers::CreateNonPrereqArrayFromTypeEnum()(this, tempPath, cDims, getScalarType(), getInitializationValue());
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CreateDataArray::preflight()
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
void CreateDataArray::execute()
{
  int err = 0;
  QString ss;
  setErrorCondition(err);

  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CreateDataArray::newFilterInstance(bool copyFilterParameters)
{
  CreateDataArray::Pointer filter = CreateDataArray::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateDataArray::getCompiledLibraryName()
{
  return Core::CoreBaseName;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateDataArray::getGroupName()
{
  return DREAM3D::FilterGroups::CoreFilters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateDataArray::getSubGroupName()
{
  return DREAM3D::FilterSubGroups::GenerationFilters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CreateDataArray::getHumanLabel()
{
  return "Create Data Array";
}

