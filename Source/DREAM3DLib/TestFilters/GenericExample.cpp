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
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_StlFilePrefix(""),
  m_MaxIterations(0),
  m_MisorientationTolerance(0)
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
  QVector<FilterParameter::Pointer> options;
  /* Place all your option initialization code here */
  /* For String input use this code */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("STL Output Prefix");
    option->setPropertyName("StlFilePrefix");
    option->setWidgetType(FilterParameter::StringWidget);
    option->setValueType("string");
    options.push_back(option);
  }
  /*  For an Integer use this code*/
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Max Iterations");
    option->setPropertyName("MaxIterations");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    options.push_back(option);
  }
  /*  For a Floating point value use this code*/
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Misorientation Tolerance");
    option->setPropertyName("MisorientationTolerance");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    options.push_back(option);
  }
  /*   For an input file use this code*/
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input File");
    option->setPropertyName("InputFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setValueType("string");
    options.push_back(option);
  }
  /*   For an input path use this code*/
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input Path");
    option->setPropertyName("InputPath");
    option->setWidgetType(FilterParameter::InputPathWidget);
    option->setValueType("string");
    options.push_back(option);
  }

  /*   For an output file use this code*/
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setValueType("string");
    options.push_back(option);
  }
  /*   For an output path use this code*/
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output Path");
    option->setPropertyName("OutputPath");
    option->setWidgetType(FilterParameter::OutputPathWidget);
    option->setValueType("string");
    options.push_back(option);
  }
  /*   For a simple true/false boolean use this code*/
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Alignment Shift File");
    option->setPropertyName("WriteAlignmentShifts");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    options.push_back(option);
  }
  /*   For presenting a set of choices to the user use this code*/
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Conversion Type");
    option->setPropertyName("ConversionType");
    option->setWidgetType(FilterParameter::ChoiceWidget);
    option->setValueType("unsigned int");
    QVector<QString> choices;
    choices.push_back("Degrees To Radians");
    choices.push_back("Radians To Degrees");
    option->setChoices(choices);
    options.push_back(option);
  }


  /* Display a group of 3 text boxes to collect 3 integer values */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Dimensions");
    option->setPropertyName("Dimensions");
    option->setWidgetType(FilterParameter::IntVec3Widget);
    option->setValueType("IntVec3Widget_t");
    option->setUnits("XYZ");
    options.push_back(option);
  }
  /* Display a group of 3 text boxes to collect 3 float values */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Origin");
    option->setPropertyName("Origin");
    option->setWidgetType(FilterParameter::FloatVec3Widget);
    option->setValueType("FloatVec3Widget_t");
    option->setUnits("XYZ");
    options.push_back(option);
  }


  /* To Display a Combobox with a list of current Volume Cell Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Volume Vertex Array Name");
    option->setPropertyName("SelectedVolumeVertexArrayName");
    option->setWidgetType(FilterParameter::VolumeVertexArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Volume Field Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Volume Edge Array Name");
    option->setPropertyName("SelectedVolumeEdgeArrayName");
    option->setWidgetType(FilterParameter::VolumeEdgeArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Volume Ensemble Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Volume Face Array Name");
    option->setPropertyName("SelectedVolumeFaceArrayName");
    option->setWidgetType(FilterParameter::VolumeFaceArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Volume Cell Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Volume Cell Array Name");
    option->setPropertyName("SelectedVolumeCellArrayName");
    option->setWidgetType(FilterParameter::VolumeCellArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Volume Field Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Volume Field Array Name");
    option->setPropertyName("SelectedVolumeFieldArrayName");
    option->setWidgetType(FilterParameter::VolumeFieldArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Volume Ensemble Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Volume Ensemble Array Name");
    option->setPropertyName("SelectedVolumeEnsembleArrayName");
    option->setWidgetType(FilterParameter::VolumeEnsembleArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }

  /* To Display a Combobox with a list of current SurfaceMesh Point Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Surface Vertex Array Name");
    option->setPropertyName("SelectedSurfaceVertexArrayName");
    option->setWidgetType(FilterParameter::SurfaceVertexArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Surface Edge Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Surface Edge Array Name");
    option->setPropertyName("SelectedSurfaceEdgeArrayName");
    option->setWidgetType(FilterParameter::SurfaceEdgeArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Surface Face Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Surface Face Array Name");
    option->setPropertyName("SelectedSurfaceFaceArrayName");
    option->setWidgetType(FilterParameter::SurfaceFaceArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Surface Edge Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Surface Field Array Name");
    option->setPropertyName("SelectedSurfaceFieldArrayName");
    option->setWidgetType(FilterParameter::SurfaceFieldArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Surface Face Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Surface Ensemble Array Name");
    option->setPropertyName("SelectedSurfaceEnsembleArrayName");
    option->setWidgetType(FilterParameter::SurfaceEnsembleArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }

  /* To Display a Combobox with a list of current EdgeMesh Point Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Edge Vertex Array Name");
    option->setPropertyName("SelectedEdgeVertexArrayName");
    option->setWidgetType(FilterParameter::EdgeVertexArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Edge Edge Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Edge Edge Array Name");
    option->setPropertyName("SelectedEdgeEdgeArrayName");
    option->setWidgetType(FilterParameter::EdgeEdgeArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Edge Edge Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Edge Field Array Name");
    option->setPropertyName("SelectedEdgeFieldArrayName");
    option->setWidgetType(FilterParameter::EdgeFieldArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Edge Face Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Edge Ensemble Array Name");
    option->setPropertyName("SelectedEdgeEnsembleArrayName");
    option->setWidgetType(FilterParameter::EdgeEnsembleArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }

  /* To Display a Combobox with a list of current Vertex Point Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Vertex Vertex Array Name");
    option->setPropertyName("SelectedVertexVertexArrayName");
    option->setWidgetType(FilterParameter::VertexVertexArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Vertex Face Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Vertex Field Array Name");
    option->setPropertyName("SelectedVertexFieldArrayName");
    option->setWidgetType(FilterParameter::VertexFieldArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Vertex Edge Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Vertex Ensemble Array Name");
    option->setPropertyName("SelectedVertexEnsembleArrayName");
    option->setWidgetType(FilterParameter::VertexEnsembleArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }

  /* Display the AxisAngleWidget to collect Axis-Angle pairs from the user */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Crystal Rotations");
    option->setPropertyName("CrystalSymmetryRotations");
    option->setWidgetType(FilterParameter::AxisAngleWidget);
    options.push_back(option);
  }

  setFilterParameters(options);
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
  setCrystalSymmetryRotations( reader->readAxisAngles("CrystalSymmetryRotations", getCrystalSymmetryRotations()) );

  setSelectedVolumeVertexArrayName( reader->readString("SelectedVolumeVertexArrayName", getSelectedVolumeVertexArrayName()) );
  setSelectedVolumeEdgeArrayName( reader->readString("SelectedVolumeEdgeArrayName", getSelectedVolumeEdgeArrayName()) );
  setSelectedVolumeFaceArrayName( reader->readString("SelectedVolumeFaceArrayName", getSelectedVolumeFaceArrayName()) );
  setSelectedVolumeCellArrayName( reader->readString("SelectedVolumeCellArrayName", getSelectedVolumeCellArrayName()) );
  setSelectedVolumeFieldArrayName( reader->readString("SelectedVolumeFieldArrayName", getSelectedVolumeFieldArrayName()) );
  setSelectedVolumeEnsembleArrayName( reader->readString("SelectedVolumeEnsembleArrayName", getSelectedVolumeEnsembleArrayName()) );
  setSelectedSurfaceVertexArrayName( reader->readString("SelectedSurfaceVertexArrayName", getSelectedSurfaceVertexArrayName()) );
  setSelectedSurfaceEdgeArrayName( reader->readString("SelectedSurfaceEdgeArrayName", getSelectedSurfaceEdgeArrayName()) );
  setSelectedSurfaceFaceArrayName( reader->readString("SelectedSurfaceFaceArrayName", getSelectedSurfaceFaceArrayName()) );
  setSelectedSurfaceFieldArrayName( reader->readString("SelectedSurfaceFieldArrayName", getSelectedSurfaceFieldArrayName()) );
  setSelectedSurfaceEnsembleArrayName( reader->readString("SelectedSurfaceEnsembleArrayName", getSelectedSurfaceEnsembleArrayName()) );
  setSelectedEdgeVertexArrayName( reader->readString("SelectedEdgeVertexArrayName", getSelectedEdgeVertexArrayName()) );
  setSelectedEdgeEdgeArrayName( reader->readString("SelectedEdgeEdgeArrayName", getSelectedEdgeEdgeArrayName()) );
  setSelectedEdgeFieldArrayName( reader->readString("SelectedEdgeFieldArrayName", getSelectedEdgeFieldArrayName()) );
  setSelectedEdgeEnsembleArrayName( reader->readString("SelectedEdgeEnsembleArrayName", getSelectedEdgeEnsembleArrayName()) );
  setSelectedVertexVertexArrayName( reader->readString("SelectedVertexVertexArrayName", getSelectedVertexVertexArrayName()) );
  setSelectedVertexFieldArrayName( reader->readString("SelectedVertexFieldArrayName", getSelectedVertexFieldArrayName()) );
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
  writer->writeValue("CrystalSymmetryRotations", getCrystalSymmetryRotations());

  writer->writeValue("SelectedVolumeVertexArrayName", getSelectedVolumeVertexArrayName());
  writer->writeValue("SelectedVolumeEdgeArrayName", getSelectedVolumeEdgeArrayName());
  writer->writeValue("SelectedVolumeFaceArrayName", getSelectedVolumeFaceArrayName());
  writer->writeValue("SelectedVolumeCellArrayName", getSelectedVolumeCellArrayName());
  writer->writeValue("SelectedVolumeFieldArrayName", getSelectedVolumeFieldArrayName());
  writer->writeValue("SelectedVolumeEnsembleArrayName", getSelectedVolumeEnsembleArrayName());
  writer->writeValue("SelectedSurfaceVertexArrayName", getSelectedSurfaceVertexArrayName());
  writer->writeValue("SelectedSurfaceEdgeArrayName", getSelectedSurfaceEdgeArrayName());
  writer->writeValue("SelectedSurfaceFaceArrayName", getSelectedSurfaceFaceArrayName());
  writer->writeValue("SelectedSurfaceFieldArrayName", getSelectedSurfaceFieldArrayName());
  writer->writeValue("SelectedSurfaceEnsembleArrayName", getSelectedSurfaceEnsembleArrayName());
  writer->writeValue("SelectedEdgeVertexArrayName", getSelectedEdgeVertexArrayName());
  writer->writeValue("SelectedEdgeEdgeArrayName", getSelectedEdgeEdgeArrayName());
  writer->writeValue("SelectedEdgeFieldArrayName", getSelectedEdgeFieldArrayName());
  writer->writeValue("SelectedEdgeEnsembleArrayName", getSelectedEdgeEnsembleArrayName());
  writer->writeValue("SelectedVertexVertexArrayName", getSelectedVertexVertexArrayName());
  writer->writeValue("SelectedVertexFieldArrayName", getSelectedVertexFieldArrayName());
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
void GenericExample::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);

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
void GenericExample::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExample::execute()
{
  int err = 0;
  setErrorCondition(err);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if (NULL == m)
  {
    setErrorCondition(-1);
    QString ss = QObject::tr(" DataContainer was NULL");
    notifyErrorMessage(QObject::tr("VolumeDataContainer was NULL. Returning from Execute Method for filter %1").arg(getHumanLabel()), -1);
    return;
  }
  setErrorCondition(0);

  /* Place all your code to execute your filter here. */

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
