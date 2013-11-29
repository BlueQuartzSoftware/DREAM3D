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

#include "ThresholdExample.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ThresholdExample::ThresholdExample() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ThresholdExample::~ThresholdExample()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThresholdExample::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> options;

  /* To Compare Arrays like a threshold filter */
  {
    ComparisonFilterParameter::Pointer parameter = ComparisonFilterParameter::New();
    parameter->setHumanLabel("Voxel Cell Arrays to Threshold");
    parameter->setPropertyName("CellComparisonInputs");
    parameter->setWidgetType(FilterParameter::CellArrayComparisonSelectionWidget);
    parameter->setValueType("QVector<ComparisonInput_t>");
    options.push_back(parameter);
  }
  /* To Compare Arrays like a threshold filter */
  {
    ComparisonFilterParameter::Pointer parameter = ComparisonFilterParameter::New();
    parameter->setHumanLabel("Voxel Feature Arrays to Threshold");
    parameter->setPropertyName("FeatureComparisonInputs");
    parameter->setWidgetType(FilterParameter::FeatureArrayComparisonSelectionWidget);
    parameter->setValueType("QVector<ComparisonInput_t>");
    options.push_back(parameter);
  }
  /* To Compare Arrays like a threshold filter */
  {
    ComparisonFilterParameter::Pointer parameter = ComparisonFilterParameter::New();
    parameter->setHumanLabel("Voxel Ensemble Arrays to Threshold");
    parameter->setPropertyName("EnsembleComparisonInputs");
    parameter->setWidgetType(FilterParameter::EnsembleArrayComparisonSelectionWidget);
    parameter->setValueType("QVector<ComparisonInput_t>");
    options.push_back(parameter);
  }
  /* To Compare Arrays like a threshold filter */
  {
    ComparisonFilterParameter::Pointer parameter = ComparisonFilterParameter::New();
    parameter->setHumanLabel("Surface Mesh Point Arrays to Threshold");
    parameter->setPropertyName("PointComparisonInputs");
    parameter->setWidgetType(FilterParameter::VertexArrayComparisonSelectionWidget);
    parameter->setValueType("QVector<ComparisonInput_t>");
    options.push_back(parameter);
  }
  /* To Compare Arrays like a threshold filter */
  {
    ComparisonFilterParameter::Pointer parameter = ComparisonFilterParameter::New();
    parameter->setHumanLabel("Surface Mesh Face Arrays to Threshold");
    parameter->setPropertyName("FaceComparisonInputs");
    parameter->setWidgetType(FilterParameter::FaceArrayComparisonSelectionWidget);
    parameter->setValueType("QVector<ComparisonInput_t>");
    options.push_back(parameter);
  }
  /* To Compare Arrays like a threshold filter */
  {
    ComparisonFilterParameter::Pointer parameter = ComparisonFilterParameter::New();
    parameter->setHumanLabel("Surface Mesh Edge Arrays to Threshold");
    parameter->setPropertyName("EdgeComparisonInputs");
    parameter->setWidgetType(FilterParameter::EdgeArrayComparisonSelectionWidget);
    parameter->setValueType("QVector<ComparisonInput_t>");
    options.push_back(parameter);
  }

  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThresholdExample::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/

  reader->openFilterGroup(this, index);
  setCellComparisonInputs(reader->readComparisonInputs("CellComparisonInputs", getCellComparisonInputs()));
  setFeatureComparisonInputs(reader->readComparisonInputs("FeatureComparisonInputs", getFeatureComparisonInputs()));
  setEnsembleComparisonInputs(reader->readComparisonInputs("EnsembleComparisonInputs", getEnsembleComparisonInputs()));
  setPointComparisonInputs(reader->readComparisonInputs("PointComparisonInputs", getPointComparisonInputs()));
  setFaceComparisonInputs(reader->readComparisonInputs("FaceComparisonInputs", getFaceComparisonInputs()));
  setEdgeComparisonInputs(reader->readComparisonInputs("EdgeComparisonInputs", getEdgeComparisonInputs()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ThresholdExample::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */

  /* --- CellArrayComparisonSelectionWidget --- */
  writer->writeValue( "CellComparisonInputs", getCellComparisonInputs() );

  /* --- FeatureArrayComparisonSelectionWidget --- */
  writer->writeValue( "FeatureComparisonInputs", getFeatureComparisonInputs() );

  /* --- EnsembleArrayComparisonSelectionWidget --- */
  writer->writeValue("EnsembleComparisonInputs", getEnsembleComparisonInputs() );

  /* --- PointArrayComparisonSelectionWidget --- */
  writer->writeValue("PointComparisonInputs", getPointComparisonInputs() );

  /* --- FaceArrayComparisonSelectionWidget --- */
  writer->writeValue("FaceComparisonInputs", getFaceComparisonInputs() );

  /* --- EdgeArrayComparisonSelectionWidget --- */
  writer->writeValue("EdgeComparisonInputs", getEdgeComparisonInputs() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThresholdExample::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(getDataContainerName(), false, this);
  if(getErrorCondition() < 0) { return; }
  /* Example code for preflighting looking for a valid string for the output file
   * but not necessarily the fact that the file exists: Example code to make sure
   * we have something in a string before proceeding.*/
  /*
  if (m_OutputFile.isEmpty() == true)
  {
    ss << "The output file must be set before executing this filter.";
    PipelineMessage em(getNameOfClass(), "There was an error", -666);
    addErrorMessage(em);
    setErrorCondition(-1);
  }
  */


}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThresholdExample::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThresholdExample::execute()
{
  dataCheck(false, 0,0,0);
  /* Place all your code to execute your filter here. */

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
