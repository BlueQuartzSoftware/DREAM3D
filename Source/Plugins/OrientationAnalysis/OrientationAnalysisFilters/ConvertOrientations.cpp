/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "ConvertOrientations.h"

#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "DREAM3DLib/FilterParameters/LinkedChoicesFilterParameter.h"


#include "OrientationLib/Math/OrientationMath.h"
#include "OrientationLib/Math/OrientationConverter.hpp"
#include "OrientationLib/Math/OrientationTransforms.hpp"


#include "OrientationAnalysis/OrientationAnalysisConstants.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertOrientations::ConvertOrientations() :
  AbstractFilter(),
  m_InputType(0),
  m_OutputType(1)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ConvertOrientations::~ConvertOrientations()
{
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertOrientations::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", false));
  parameters.push_back(FilterParameter::New("Input Orientations", "InputOrientationArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getInputOrientationArrayPath(), false, "", 0));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Input Orientation Type");
    parameter->setPropertyName("InputType");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    QVector<QString> inputChoices = OrientationConverter<float>::GetOrientationTypeStrings();
    parameter->setChoices(inputChoices);
    parameters.push_back(parameter);
  }

  parameters.push_back(FilterParameter::New("Output Orientation Array Name", "OutputOrientationArrayName", FilterParameterWidgetType::StringWidget, getOutputOrientationArrayName(), false, "", 0));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Output Orientation Type");
    parameter->setPropertyName("OutputType");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    QVector<QString> inputChoices = OrientationConverter<float>::GetOrientationTypeStrings();
    parameter->setChoices(inputChoices);
    parameters.push_back(parameter);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void ConvertOrientations::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setInputType(reader->readValue("InputType", getInputType() ) );
  setOutputType(reader->readValue("OutputType", getOutputType() ) );
  setInputOrientationArrayPath(reader->readDataArrayPath("InputOrientationArrayPath", getInputOrientationArrayPath()));
  setOutputOrientationArrayName(reader->readString("OutputOrientationArrayName", getOutputOrientationArrayName()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ConvertOrientations::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(InputType)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputType)
  DREAM3D_FILTER_WRITE_PARAMETER(InputOrientationArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputOrientationArrayName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertOrientations::dataCheck()
{
  setErrorCondition(0);


  if(getInputType() == getOutputType())
  {
    QString ss = QObject::tr("Input and output orientation representation types must be different");
    setErrorCondition(-1000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if( getInputType() < OrientationConverter<float>::GetMinIndex() || getInputType() > OrientationConverter<float>::GetMaxIndex() )
  {
    QString ss = QObject::tr("There was an error with teh selection of the input orientation type. The valid values range from 0 to %1.").arg(OrientationConverter<float>::GetMaxIndex());
    setErrorCondition(-1001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if( getOutputType() < OrientationConverter<float>::GetMinIndex() || getOutputType() > OrientationConverter<float>::GetMaxIndex() )
  {
    QString ss = QObject::tr("There was an error with the selection of the output orientation type. The valid values range from 0 to %1.").arg(OrientationConverter<float>::GetMaxIndex());
    setErrorCondition(-1002);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  // We need to return NOW because the next lines assume we have and index that is within
  // the valid bounds
  if(getErrorCondition() < 0) { return; }

  // Get the input data
  QVector<int> componentCounts = OrientationConverter<float>::GetComponentCounts();
  QVector<size_t> inputCDims(1, componentCounts[getInputType()]);
  m_InputOrientationsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getInputOrientationArrayPath(), inputCDims);
  if( NULL != m_InputOrientationsPtr.lock().get() )
  { m_InputOrientations = m_InputOrientationsPtr.lock()->getPointer(0); }


  // Create the output data
  DataArrayPath outputArrayPath = getInputOrientationArrayPath();
  outputArrayPath.setDataArrayName(getOutputOrientationArrayName());
  QVector<size_t> outputCDims(1, componentCounts[getOutputType()]);
  m_OutputOrientationsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this, outputArrayPath, 0, outputCDims);
  if( NULL != m_OutputOrientationsPtr.lock().get() )
  { m_OutputOrientations = m_OutputOrientationsPtr.lock()->getPointer(0); }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ConvertOrientations::preflight()
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
void ConvertOrientations::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  typedef OrientationConverter<float> OCType;
  QVector<OCType::Pointer> converters(7);

  converters[0] = EulerConvertor<float>::New();
  converters[1] = OrientationMatrixConvertor<float>::New();
  converters[2] = QuaternionConvertor<float>::New();
  converters[3] = AxisAngleConvertor<float>::New();
  converters[4] = RodriguesConvertor<float>::New();
  converters[5] = HomochoricConvertor<float>::New();
  converters[6] = CubochoricConvertor<float>::New();

  QVector<OCType::OrientationType> ocTypes = OCType::GetOrientationTypes();

  converters[getInputType()]->setInputData(m_InputOrientationsPtr.lock());
  converters[getInputType()]->convertRepresentationTo(ocTypes[getOutputType()]);

  DataArray<float>::Pointer output = converters[getInputType()]->getOutputData();

  if(!output->copyIntoArray(m_OutputOrientationsPtr.lock()) )
  {
    QString ss = QObject::tr("There was an error copying the final results into the output array.");
    setErrorCondition(-1003);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertOrientations::getCompiledLibraryName()
{ return OrientationAnalysisConstants::OrientationAnalysisBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertOrientations::getGroupName()
{ return DREAM3D::FilterGroups::OrientationAnalysisFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertOrientations::getSubGroupName()
{ return DREAM3D::FilterSubGroups::ConversionFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ConvertOrientations::getHumanLabel()
{ return "Convert Orientation Representation"; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ConvertOrientations::newFilterInstance(bool copyFilterParameters)
{
  ConvertOrientations::Pointer filter = ConvertOrientations::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

