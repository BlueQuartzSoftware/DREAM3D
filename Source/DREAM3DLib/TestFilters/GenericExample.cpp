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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "GenericExample.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericExample::GenericExample() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_StlFilePrefix(""),
  m_MaxIterations(0),
  m_MisorientationTolerance(0),
  m_InputFile(""),
  m_InputPath(""),
  m_OutputFile(""),
  m_OutputPath(""),
  m_WriteAlignmentShifts(false),
  m_ConversionType(0),
  m_Int8Value(0),
  m_Int16Value(0),
  m_Int32Value(0),
  m_Int64Value(0),
  m_UInt8Value(0),
  m_UInt16Value(0),
  m_UInt32Value(0),
  m_UInt64Value(0),
  m_FloatValue(0.0),
  m_DoubleValue(0.0),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_CellPatternQualityArrayName("Pattern Quality"),
  m_CellPatternQuality(NULL),
  m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
  m_CellEulerAngles(NULL),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_GoodVoxels(NULL)
{
  m_Dimensions.x = 0;
  m_Dimensions.y = 0;
  m_Dimensions.z = 0;

  m_Origin.x = 0.0;
  m_Origin.y = 0.0;
  m_Origin.z = 0.0;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericExample::~GenericExample()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExample::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
  /* Place all your option initialization code here */
  /* For String input use this code */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("STL Output Prefix");
    parameter->setPropertyName("StlFilePrefix");
    parameter->setWidgetType(FilterParameterWidgetType::StringWidget);
    parameter->setValueType("QString");
    parameters.push_back(parameter);
  }
  /*  For an Integer use this code*/
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Max Iterations");
    parameter->setPropertyName("MaxIterations");
    parameter->setWidgetType(FilterParameterWidgetType::IntWidget);
    parameter->setValueType("int");
    parameters.push_back(parameter);
  }
  /*  For a Floating point value use this code*/
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Misorientation Tolerance");
    parameter->setPropertyName("MisorientationTolerance");
    parameter->setWidgetType(FilterParameterWidgetType::DoubleWidget);
    parameter->setValueType("float");
    parameter->setCastableValueType("double");
    parameters.push_back(parameter);
  }
  /*   For an input file use this code*/
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Input File");
    parameter->setPropertyName("InputFile");
    parameter->setWidgetType(FilterParameterWidgetType::InputFileWidget);
    parameter->setValueType("QString");
    parameters.push_back(parameter);
  }
  /*   For an input path use this code*/
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Input Path");
    parameter->setPropertyName("InputPath");
    parameter->setWidgetType(FilterParameterWidgetType::InputPathWidget);
    parameter->setValueType("QString");
    parameters.push_back(parameter);
  }

  /*   For an output file use this code*/
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Output File");
    parameter->setPropertyName("OutputFile");
    parameter->setWidgetType(FilterParameterWidgetType::OutputFileWidget);
    parameter->setValueType("QString");
    parameters.push_back(parameter);
  }
  /*   For an output path use this code*/
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Output Path");
    parameter->setPropertyName("OutputPath");
    parameter->setWidgetType(FilterParameterWidgetType::OutputPathWidget);
    parameter->setValueType("QString");
    parameters.push_back(parameter);
  }
  /*   For a simple true/false boolean use this code*/
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Write Alignment Shift File");
    parameter->setPropertyName("WriteAlignmentShifts");
    parameter->setWidgetType(FilterParameterWidgetType::BooleanWidget);
    parameter->setValueType("bool");
    parameters.push_back(parameter);
  }
  /*   For presenting a set of choices to the user use this code*/
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Conversion Type");
    parameter->setPropertyName("ConversionType");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    parameter->setValueType("unsigned int");
    QVector<QString> choices;
    choices.push_back("Degrees To Radians");
    choices.push_back("Radians To Degrees");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }


  /* Display a group of 3 text boxes to collect 3 integer values */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Dimensions");
    parameter->setPropertyName("Dimensions");
    parameter->setWidgetType(FilterParameterWidgetType::IntVec3Widget);
    parameter->setValueType("IntVec3_t");
    parameter->setUnits("XYZ");
    parameters.push_back(parameter);
  }
  /* Display a group of 3 text boxes to collect 3 float values */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Origin");
    parameter->setPropertyName("Origin");
    parameter->setWidgetType(FilterParameterWidgetType::FloatVec3Widget);
    parameter->setValueType("FloatVec3_t");
    parameter->setUnits("XYZ");
    parameters.push_back(parameter);
  }


  /* To Display a Combobox with a list of current Volume Cell Arrays in it */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Volume Vertex Array Name");
    parameter->setPropertyName("SelectedVolumeVertexArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  /* To Display a Combobox with a list of current Volume Feature Arrays in it */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Volume Edge Array Name");
    parameter->setPropertyName("SelectedVolumeEdgeArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  /* To Display a Combobox with a list of current Volume Ensemble Arrays in it */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Volume Face Array Name");
    parameter->setPropertyName("SelectedVolumeFaceArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  /* To Display a Combobox with a list of current Volume Cell Arrays in it */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Volume Cell Array Name");
    parameter->setPropertyName("SelectedVolumeCellArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  /* To Display a Combobox with a list of current Volume Feature Arrays in it */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Volume Feature Array Name");
    parameter->setPropertyName("SelectedVolumeFeatureArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  /* To Display a Combobox with a list of current Volume Ensemble Arrays in it */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Volume Ensemble Array Name");
    parameter->setPropertyName("SelectedVolumeEnsembleArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }

  /* To Display a Combobox with a list of current SurfaceMesh Point Arrays in it */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Surface Vertex Array Name");
    parameter->setPropertyName("SelectedSurfaceVertexArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  /* To Display a Combobox with a list of current Surface Edge Arrays in it */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Surface Edge Array Name");
    parameter->setPropertyName("SelectedSurfaceEdgeArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  /* To Display a Combobox with a list of current Surface Face Arrays in it */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Surface Face Array Name");
    parameter->setPropertyName("SelectedSurfaceFaceArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  /* To Display a Combobox with a list of current Surface Edge Arrays in it */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Surface Feature Array Name");
    parameter->setPropertyName("SelectedSurfaceFeatureArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  /* To Display a Combobox with a list of current Surface Face Arrays in it */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Surface Ensemble Array Name");
    parameter->setPropertyName("SelectedSurfaceEnsembleArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }

  /* To Display a Combobox with a list of current EdgeMesh Point Arrays in it */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Edge Vertex Array Name");
    parameter->setPropertyName("SelectedEdgeVertexArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  /* To Display a Combobox with a list of current Edge Edge Arrays in it */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Edge Edge Array Name");
    parameter->setPropertyName("SelectedEdgeEdgeArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  /* To Display a Combobox with a list of current Edge Edge Arrays in it */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Edge Feature Array Name");
    parameter->setPropertyName("SelectedEdgeFeatureArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  /* To Display a Combobox with a list of current Edge Face Arrays in it */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Edge Ensemble Array Name");
    parameter->setPropertyName("SelectedEdgeEnsembleArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }

  /* To Display a Combobox with a list of current Vertex Point Arrays in it */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Vertex Vertex Array Name");
    parameter->setPropertyName("SelectedVertexVertexArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  /* To Display a Combobox with a list of current Vertex Face Arrays in it */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Vertex Feature Array Name");
    parameter->setPropertyName("SelectedVertexFeatureArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  /* To Display a Combobox with a list of current Vertex Edge Arrays in it */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Vertex Ensemble Array Name");
    parameter->setPropertyName("SelectedVertexEnsembleArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }

  /* Display the AxisAngleWidget to collect Axis-Angle pairs from the user */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Crystal Rotations");
    parameter->setPropertyName("CrystalSymmetryRotations");
    parameter->setWidgetType(FilterParameterWidgetType::AxisAngleWidget);
    parameters.push_back(parameter);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExample::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/

  reader->openFilterGroup(this, index);
  setStlFilePrefix( reader->readString("StlFilePrefix", getStlFilePrefix()) );
  setMaxIterations( reader->readValue("MaxIterations", getMaxIterations()) );
  setMisorientationTolerance( reader->readValue("MisorientationTolerance", getMisorientationTolerance()) );
  setInputFile( reader->readString("InputFile", getInputFile()) );
  setInputPath( reader->readString("InputPath", getInputPath()) );
  setOutputFile( reader->readString("OutputFile", getOutputFile()) );
  setOutputPath( reader->readString("OutputPath", getOutputPath()) );
  setWriteAlignmentShifts( reader->readValue("WriteAlignmentShifts", getWriteAlignmentShifts()) );
  setConversionType( reader->readValue("ConversionType", getConversionType()) );
  setDimensions( reader->readIntVec3("Dimensions", getDimensions()) );
  setOrigin( reader->readFloatVec3("Origin", getOrigin()) );
  setAxisAngleInputs(reader->readAxisAngles("AxisAngleInputs", getAxisAngleInputs()) );

  setSelectedVolumeVertexArrayName( reader->readString("SelectedVolumeVertexArrayName", getSelectedVolumeVertexArrayName()) );
  setSelectedVolumeEdgeArrayName( reader->readString("SelectedVolumeEdgeArrayName", getSelectedVolumeEdgeArrayName()) );
  setSelectedVolumeFaceArrayName( reader->readString("SelectedVolumeFaceArrayName", getSelectedVolumeFaceArrayName()) );
  setSelectedVolumeCellArrayName( reader->readString("SelectedVolumeCellArrayName", getSelectedVolumeCellArrayName()) );
  setSelectedVolumeFeatureArrayName( reader->readString("SelectedVolumeFeatureArrayName", getSelectedVolumeFeatureArrayName()) );
  setSelectedVolumeEnsembleArrayName( reader->readString("SelectedVolumeEnsembleArrayName", getSelectedVolumeEnsembleArrayName()) );
  setSelectedSurfaceVertexArrayName( reader->readString("SelectedSurfaceVertexArrayName", getSelectedSurfaceVertexArrayName()) );
  setSelectedSurfaceEdgeArrayName( reader->readString("SelectedSurfaceEdgeArrayName", getSelectedSurfaceEdgeArrayName()) );
  setSelectedSurfaceFaceArrayName( reader->readString("SelectedSurfaceFaceArrayName", getSelectedSurfaceFaceArrayName()) );
  setSelectedSurfaceFeatureArrayName( reader->readString("SelectedSurfaceFeatureArrayName", getSelectedSurfaceFeatureArrayName()) );
  setSelectedSurfaceEnsembleArrayName( reader->readString("SelectedSurfaceEnsembleArrayName", getSelectedSurfaceEnsembleArrayName()) );
  setSelectedEdgeVertexArrayName( reader->readString("SelectedEdgeVertexArrayName", getSelectedEdgeVertexArrayName()) );
  setSelectedEdgeEdgeArrayName( reader->readString("SelectedEdgeEdgeArrayName", getSelectedEdgeEdgeArrayName()) );
  setSelectedEdgeFeatureArrayName( reader->readString("SelectedEdgeFeatureArrayName", getSelectedEdgeFeatureArrayName()) );
  setSelectedEdgeEnsembleArrayName( reader->readString("SelectedEdgeEnsembleArrayName", getSelectedEdgeEnsembleArrayName()) );
  setSelectedVertexVertexArrayName( reader->readString("SelectedVertexVertexArrayName", getSelectedVertexVertexArrayName()) );
  setSelectedVertexFeatureArrayName( reader->readString("SelectedVertexFeatureArrayName", getSelectedVertexFeatureArrayName()) );
  setSelectedVertexEnsembleArrayName( reader->readString("SelectedVertexEnsembleArrayName", getSelectedVertexEnsembleArrayName()) );


  setInt8Array(reader->readArray("Int8Array", getInt8Array()) );
  setInt16Array(reader->readArray("Int16Array", getInt16Array()) );
  setInt32Array(reader->readArray("Int32Array", getInt32Array()) );
  setInt64Array(reader->readArray("Int64Array", getInt64Array()) );
  setUInt8Array(reader->readArray("UInt8Array", getUInt8Array()) );
  setUInt16Array(reader->readArray("UInt16Array", getUInt16Array()) );
  setUInt32Array(reader->readArray("UInt32Array", getUInt32Array()) );
  setUInt64Array(reader->readArray("UInt64Array", getUInt64Array()) );
  setFloatArray(reader->readArray("FloatArray", getFloatArray()) );
  setDoubleArray(reader->readArray("DoubleArray", getDoubleArray()) );


  setInt8Value(reader->readValue("Int8Value", getInt8Value()) );
  setInt16Value(reader->readValue("Int16Value", getInt16Value()) );
  setInt32Value(reader->readValue("Int32Value", getInt32Value()) );
  setInt64Value(reader->readValue("Int64Value", getInt64Value()) );
  setUInt8Value(reader->readValue("UInt8Value", getUInt8Value()) );
  setUInt16Value(reader->readValue("UInt16Value", getUInt16Value()) );
  setUInt32Value(reader->readValue("UInt32Value", getUInt32Value()) );
  setUInt64Value(reader->readValue("UInt64Value", getUInt64Value()) );
  setFloatValue(reader->readValue("FloatValue", getFloatValue()) );
  setDoubleValue(reader->readValue("DoubleValue", getDoubleValue()) );

  setStrVector( reader->readStrings("StrVector", getStrVector()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GenericExample::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  writer->writeValue("StlFilePrefix", getStlFilePrefix());
  writer->writeValue("MaxIterations", getMaxIterations());
  writer->writeValue("MisorientationTolerance", getMisorientationTolerance());
  writer->writeValue("InputFile", getInputFile());
  writer->writeValue("InputPath", getInputPath());
  writer->writeValue("OutputFile", getOutputFile());
  writer->writeValue("OutputPath", getOutputPath());
  writer->writeValue("WriteAlignmentShifts", getWriteAlignmentShifts());
  writer->writeValue("ConversionType", getConversionType());
  writer->writeValue("Dimensions", getDimensions());
  writer->writeValue("Origin", getOrigin());
  writer->writeValue("AxisAngleInputs", getAxisAngleInputs());

  writer->writeValue("SelectedVolumeVertexArrayName", getSelectedVolumeVertexArrayName());
  writer->writeValue("SelectedVolumeEdgeArrayName", getSelectedVolumeEdgeArrayName());
  writer->writeValue("SelectedVolumeFaceArrayName", getSelectedVolumeFaceArrayName());
  writer->writeValue("SelectedVolumeCellArrayName", getSelectedVolumeCellArrayName());
  writer->writeValue("SelectedVolumeFeatureArrayName", getSelectedVolumeFeatureArrayName());
  writer->writeValue("SelectedVolumeEnsembleArrayName", getSelectedVolumeEnsembleArrayName());
  writer->writeValue("SelectedSurfaceVertexArrayName", getSelectedSurfaceVertexArrayName());
  writer->writeValue("SelectedSurfaceEdgeArrayName", getSelectedSurfaceEdgeArrayName());
  writer->writeValue("SelectedSurfaceFaceArrayName", getSelectedSurfaceFaceArrayName());
  writer->writeValue("SelectedSurfaceFeatureArrayName", getSelectedSurfaceFeatureArrayName());
  writer->writeValue("SelectedSurfaceEnsembleArrayName", getSelectedSurfaceEnsembleArrayName());
  writer->writeValue("SelectedEdgeVertexArrayName", getSelectedEdgeVertexArrayName());
  writer->writeValue("SelectedEdgeEdgeArrayName", getSelectedEdgeEdgeArrayName());
  writer->writeValue("SelectedEdgeFeatureArrayName", getSelectedEdgeFeatureArrayName());
  writer->writeValue("SelectedEdgeEnsembleArrayName", getSelectedEdgeEnsembleArrayName());
  writer->writeValue("SelectedVertexVertexArrayName", getSelectedVertexVertexArrayName());
  writer->writeValue("SelectedVertexFeatureArrayName", getSelectedVertexFeatureArrayName());
  writer->writeValue("SelectedVertexEnsembleArrayName", getSelectedVertexEnsembleArrayName());

  writer->writeValue("Int8Array", getInt8Array());
  writer->writeValue("Int16Array", getInt16Array());
  writer->writeValue("Int32Array", getInt32Array());
  writer->writeValue("Int64Array", getInt64Array());
  writer->writeValue("UInt8Array", getUInt8Array());
  writer->writeValue("UInt16Array", getUInt16Array());
  writer->writeValue("UInt32Array", getUInt32Array());
  writer->writeValue("UInt64Array", getUInt64Array());
  writer->writeValue("FloatArray", getFloatArray());
  writer->writeValue("DoubleArray", getDoubleArray());

  writer->writeValue("Int8Value", getInt8Value());
  writer->writeValue("Int16Value", getInt16Value());
  writer->writeValue("Int32Value", getInt32Value());
  writer->writeValue("Int64Value", getInt64Value());
  writer->writeValue("UInt8Value", getUInt8Value());
  writer->writeValue("UInt16Value", getUInt16Value());
  writer->writeValue("UInt32Value", getUInt32Value());
  writer->writeValue("UInt64Value", getUInt64Value());
  writer->writeValue("FloatValue", getFloatValue());
  writer->writeValue("DoubleValue", getDoubleValue());


  writer->writeValue("StrVector", getStrVector());

  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExample::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0 || NULL == cellAttrMat.get() ) { return; }

  QVector<size_t> dims(1, 1);
  // Require the following Cell Data
  m_FeatureIdsPtr = cellAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_FeatureIdsArrayName, -301, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_CellPatternQualityPtr = cellAttrMat->getPrereqArray<DataArray<float>, AbstractFilter>(this, m_CellPatternQualityArrayName, -302, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPatternQualityPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPatternQuality = m_CellPatternQualityPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Create the following array
  dims[0] = 3; // We are going to create a [1x3] component for each Cell.
  m_CellEulerAnglesPtr = cellAttrMat->createNonPrereqArray<DataArray<float>, GenericExample>(this, m_CellEulerAnglesArrayName, -303, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // The good voxels array is optional, If it is available we are going to use it, otherwise we are going to create it
  dims[0] = 1;
  m_GoodVoxelsPtr = cellAttrMat->getPrereqArray<DataArray<bool>, AbstractFilter>(this, m_GoodVoxelsArrayName, -304, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(NULL == m_GoodVoxelsPtr.lock().get() )  // The Good Voxels array was NOT available so create it
  {
    m_GoodVoxelsPtr = cellAttrMat->createNonPrereqArray<DataArray<bool>, GenericExample, bool>(this, m_GoodVoxelsArrayName, false, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExample::dataCheck2()
{
  VolumeDataContainer* vdc = getDataContainerArray()->createNonPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, QString("Test Container") );
  if(getErrorCondition() < 0) { return; }

  vdc = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(QString("Test Container"));

  DataContainer::Pointer removedDataContainer = getDataContainerArray()->removeDataContainer("Test Container");
  removedDataContainer = DataContainer::NullPointer(); // this is just here to quiet the compiler about unused variables.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExample::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  dataCheck2();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void computeEulerAngle(float pq, float* eulerAngle)
{

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExample::execute()
{
  int err = 0;
  setErrorCondition(err);

  // Run the data check to get references to all of our data arrays initialized to the values stored in memory
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  // Get the number of cells
  int64_t numCells = m->getTotalPoints();

  // We want to work with the raw point for shear speed so initialize some local variables. We have to temporarily turn
  // the weak shared pointer into a shared_ptr<> using the ".lock()" then we can use the normal -> semantics to access
  // the DataArray methods.
  float* pq = m_CellPatternQualityPtr.lock()->getPointer(0);
  float* eulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);

  for(int64_t c = 0; c < numCells; c++)
  {
    computeEulerAngle(pq[c], eulerAngles + c);

  }



  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

