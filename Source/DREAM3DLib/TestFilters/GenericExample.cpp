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
  m_GoodVoxels(NULL),
/*[]*/m_FeatureIdsArrayPath(DREAM3D::Defaults::SomePath),
/*[]*/m_CellPatternQualityArrayPath(DREAM3D::Defaults::SomePath),
/*[]*/m_GoodVoxelsArrayPath(DREAM3D::Defaults::SomePath)
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
  parameters.push_back(FilterParameter::New("STL Output Prefix", "StlFilePrefix", FilterParameterWidgetType::StringWidget,"QString", false));
  /*  For an Integer use this code*/
  parameters.push_back(FilterParameter::New("Max Iterations", "MaxIterations", FilterParameterWidgetType::IntWidget,"int", false));
  /*  For a Floating point value use this code*/
  parameters.push_back(FilterParameter::New("Misorientation Tolerance", "MisorientationTolerance", FilterParameterWidgetType::DoubleWidget,"float", false));
  /*   For an input file use this code*/
  parameters.push_back(FilterParameter::New("Input File", "InputFile", FilterParameterWidgetType::InputFileWidget,"QString", false));
  /*   For an input path use this code*/
  parameters.push_back(FilterParameter::New("Input Path", "InputPath", FilterParameterWidgetType::InputPathWidget,"QString", false));

  /*   For an output file use this code*/
  parameters.push_back(FilterParameter::New("Output File", "OutputFile", FilterParameterWidgetType::OutputFileWidget,"QString", false));
  /*   For an output path use this code*/
  parameters.push_back(FilterParameter::New("Output Path", "OutputPath", FilterParameterWidgetType::OutputPathWidget,"QString", false));
  /*   For a simple true/false boolean use this code*/
  parameters.push_back(FilterParameter::New("Write Alignment Shift File", "WriteAlignmentShifts", FilterParameterWidgetType::BooleanWidget,"bool", false));

  parameters.push_back(FilterParameter::New("Choice Example", "", FilterParameterWidgetType::SeparatorWidget, "QString", false));


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
  parameters.push_back(FilterParameter::New("Dimensions", "Dimensions", FilterParameterWidgetType::IntVec3Widget,"IntVec3_t", false, "XYZ"));
  /* Display a group of 3 text boxes to collect 3 float values */
  parameters.push_back(FilterParameter::New("Origin", "Origin", FilterParameterWidgetType::FloatVec3Widget,"FloatVec3_t", false, "XYZ"));


  /* To Display a Combobox with a list of current Volume Cell Arrays in it */
  parameters.push_back(FilterParameter::New("Volume Vertex Array Name", "SelectedVolumeVertexArrayName", FilterParameterWidgetType::DataArraySelectionWidget,"QString", false));
  /* To Display a Combobox with a list of current Volume Feature Arrays in it */
  parameters.push_back(FilterParameter::New("Volume Edge Array Name", "SelectedVolumeEdgeArrayName", FilterParameterWidgetType::DataArraySelectionWidget,"QString", false));
  /* To Display a Combobox with a list of current Volume Ensemble Arrays in it */
  parameters.push_back(FilterParameter::New("Volume Face Array Name", "SelectedVolumeFaceArrayName", FilterParameterWidgetType::DataArraySelectionWidget,"QString", false));
  /* To Display a Combobox with a list of current Volume Cell Arrays in it */
  parameters.push_back(FilterParameter::New("Volume Cell Array Name", "SelectedVolumeCellArrayName", FilterParameterWidgetType::DataArraySelectionWidget,"QString", false));
  /* To Display a Combobox with a list of current Volume Feature Arrays in it */
  parameters.push_back(FilterParameter::New("Volume Feature Array Name", "SelectedVolumeFeatureArrayName", FilterParameterWidgetType::DataArraySelectionWidget,"QString", false));
  /* To Display a Combobox with a list of current Volume Ensemble Arrays in it */
  parameters.push_back(FilterParameter::New("Volume Ensemble Array Name", "SelectedVolumeEnsembleArrayName", FilterParameterWidgetType::DataArraySelectionWidget,"QString", false));

  /* To Display a Combobox with a list of current SurfaceMesh Point Arrays in it */
  parameters.push_back(FilterParameter::New("Surface Vertex Array Name", "SelectedSurfaceVertexArrayName", FilterParameterWidgetType::DataArraySelectionWidget,"QString", false));
  /* To Display a Combobox with a list of current Surface Edge Arrays in it */
  parameters.push_back(FilterParameter::New("Surface Edge Array Name", "SelectedSurfaceEdgeArrayName", FilterParameterWidgetType::DataArraySelectionWidget,"QString", false));
  /* To Display a Combobox with a list of current Surface Face Arrays in it */
  parameters.push_back(FilterParameter::New("Surface Face Array Name", "SelectedSurfaceFaceArrayName", FilterParameterWidgetType::DataArraySelectionWidget,"QString", false));
  /* To Display a Combobox with a list of current Surface Edge Arrays in it */
  parameters.push_back(FilterParameter::New("Surface Feature Array Name", "SelectedSurfaceFeatureArrayName", FilterParameterWidgetType::DataArraySelectionWidget,"QString", false));
  /* To Display a Combobox with a list of current Surface Face Arrays in it */
  parameters.push_back(FilterParameter::New("Surface Ensemble Array Name", "SelectedSurfaceEnsembleArrayName", FilterParameterWidgetType::DataArraySelectionWidget,"QString", false));

  /* To Display a Combobox with a list of current EdgeMesh Point Arrays in it */
  parameters.push_back(FilterParameter::New("Edge Vertex Array Name", "SelectedEdgeVertexArrayName", FilterParameterWidgetType::DataArraySelectionWidget,"QString", false));
  /* To Display a Combobox with a list of current Edge Edge Arrays in it */
  parameters.push_back(FilterParameter::New("Edge Edge Array Name", "SelectedEdgeEdgeArrayName", FilterParameterWidgetType::DataArraySelectionWidget,"QString", false));
  /* To Display a Combobox with a list of current Edge Edge Arrays in it */
  parameters.push_back(FilterParameter::New("Edge Feature Array Name", "SelectedEdgeFeatureArrayName", FilterParameterWidgetType::DataArraySelectionWidget,"QString", false));
  /* To Display a Combobox with a list of current Edge Face Arrays in it */
  parameters.push_back(FilterParameter::New("Edge Ensemble Array Name", "SelectedEdgeEnsembleArrayName", FilterParameterWidgetType::DataArraySelectionWidget,"QString", false));

  /* To Display a Combobox with a list of current Vertex Point Arrays in it */
  parameters.push_back(FilterParameter::New("Vertex Vertex Array Name", "SelectedVertexVertexArrayName", FilterParameterWidgetType::DataArraySelectionWidget,"QString", false));
  /* To Display a Combobox with a list of current Vertex Face Arrays in it */
  parameters.push_back(FilterParameter::New("Vertex Feature Array Name", "SelectedVertexFeatureArrayName", FilterParameterWidgetType::DataArraySelectionWidget,"QString", false));
  /* To Display a Combobox with a list of current Vertex Edge Arrays in it */
  parameters.push_back(FilterParameter::New("Vertex Ensemble Array Name", "SelectedVertexEnsembleArrayName", FilterParameterWidgetType::DataArraySelectionWidget,"QString", false));

  /* Display the AxisAngleWidget to collect Axis-Angle pairs from the user */
  parameters.push_back(FilterParameter::New("Crystal Rotations", "CrystalSymmetryRotations", FilterParameterWidgetType::AxisAngleWidget,"", false));

  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
/*[]*/parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
/*[]*/parameters.push_back(FilterParameter::New("CellPatternQuality", "CellPatternQualityArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
/*[]*/parameters.push_back(FilterParameter::New("GoodVoxels", "GoodVoxelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "QString", true));
/*##*/parameters.push_back(FilterParameter::New("CellEulerAngles", "CellEulerAnglesArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
/*##*/parameters.push_back(FilterParameter::New("GoodVoxels", "GoodVoxelsArrayName", FilterParameterWidgetType::StringWidget, "QString", true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExample::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{

  reader->openFilterGroup(this, index);
/*[]*/setGoodVoxelsArrayName(reader->readString("GoodVoxelsArrayName", getGoodVoxelsArrayName() ) );
/*[]*/setCellEulerAnglesArrayName(reader->readString("CellEulerAnglesArrayName", getCellEulerAnglesArrayName() ) );
  setGoodVoxelsArrayPath(reader->readDataArrayPath("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() ) );
  setCellPatternQualityArrayPath(reader->readDataArrayPath("CellPatternQualityArrayPath", getCellPatternQualityArrayPath() ) );
  setFeatureIdsArrayPath(reader->readDataArrayPath("FeatureIdsArrayPath", getFeatureIdsArrayPath() ) );
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

  setSelectedVolumeVertexArrayName( reader->readDataArrayPath("SelectedVolumeVertexArrayName", getSelectedVolumeVertexArrayName()) );
  setSelectedVolumeEdgeArrayName( reader->readDataArrayPath("SelectedVolumeEdgeArrayName", getSelectedVolumeEdgeArrayName()) );
  setSelectedVolumeFaceArrayName( reader->readDataArrayPath("SelectedVolumeFaceArrayName", getSelectedVolumeFaceArrayName()) );
  setSelectedVolumeCellArrayName( reader->readDataArrayPath("SelectedVolumeCellArrayName", getSelectedVolumeCellArrayName()) );
  setSelectedVolumeFeatureArrayName( reader->readDataArrayPath("SelectedVolumeFeatureArrayName", getSelectedVolumeFeatureArrayName()) );
  setSelectedVolumeEnsembleArrayName( reader->readDataArrayPath("SelectedVolumeEnsembleArrayName", getSelectedVolumeEnsembleArrayName()) );
  setSelectedSurfaceVertexArrayName( reader->readDataArrayPath("SelectedSurfaceVertexArrayName", getSelectedSurfaceVertexArrayName()) );
  setSelectedSurfaceEdgeArrayName( reader->readDataArrayPath("SelectedSurfaceEdgeArrayName", getSelectedSurfaceEdgeArrayName()) );
  setSelectedSurfaceFaceArrayName( reader->readDataArrayPath("SelectedSurfaceFaceArrayName", getSelectedSurfaceFaceArrayName()) );
  setSelectedSurfaceFeatureArrayName( reader->readDataArrayPath("SelectedSurfaceFeatureArrayName", getSelectedSurfaceFeatureArrayName()) );
  setSelectedSurfaceEnsembleArrayName( reader->readDataArrayPath("SelectedSurfaceEnsembleArrayName", getSelectedSurfaceEnsembleArrayName()) );
  setSelectedEdgeVertexArrayName( reader->readDataArrayPath("SelectedEdgeVertexArrayName", getSelectedEdgeVertexArrayName()) );
  setSelectedEdgeEdgeArrayName( reader->readDataArrayPath("SelectedEdgeEdgeArrayName", getSelectedEdgeEdgeArrayName()) );
  setSelectedEdgeFeatureArrayName( reader->readDataArrayPath("SelectedEdgeFeatureArrayName", getSelectedEdgeFeatureArrayName()) );
  setSelectedEdgeEnsembleArrayName( reader->readDataArrayPath("SelectedEdgeEnsembleArrayName", getSelectedEdgeEnsembleArrayName()) );
  setSelectedVertexVertexArrayName( reader->readDataArrayPath("SelectedVertexVertexArrayName", getSelectedVertexVertexArrayName()) );
  setSelectedVertexFeatureArrayName( reader->readDataArrayPath("SelectedVertexFeatureArrayName", getSelectedVertexFeatureArrayName()) );
  setSelectedVertexEnsembleArrayName( reader->readDataArrayPath("SelectedVertexEnsembleArrayName", getSelectedVertexEnsembleArrayName()) );


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
/*[]*/writer->writeValue("GoodVoxelsArrayName", getGoodVoxelsArrayName() );
/*[]*/writer->writeValue("CellEulerAnglesArrayName", getCellEulerAnglesArrayName() );
  writer->writeValue("GoodVoxelsArrayPath", getGoodVoxelsArrayPath() );
  writer->writeValue("CellPatternQualityArrayPath", getCellPatternQualityArrayPath() );
  writer->writeValue("FeatureIdsArrayPath", getFeatureIdsArrayPath() );
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
  DataArrayPath tempPath;
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0 || NULL == cellAttrMat.get() ) { return; }

  QVector<size_t> dims(1, 1);
  // Require the following Cell Data
  m_FeatureIdsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getFeatureIdsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_CellPatternQualityPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getCellPatternQualityArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellPatternQualityPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellPatternQuality = m_CellPatternQualityPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // Create the following array
  dims[0] = 3; // We are going to create a [1x3] component for each Cell.
  m_CellEulerAnglesPtr = cellAttrMat->createNonPrereqArray<DataArray<float>, GenericExample>(this, m_CellEulerAnglesArrayName, -303, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getCellEulerAnglesArrayName() );
////==>MIKE_GROEBER_FIX m_CellEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, m_CellEulerAnglesArrayName, -303, dims); /* Assigns the shared_ptr<>(this, tempPath, -303, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_CellEulerAnglesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // The good voxels array is optional, If it is available we are going to use it, otherwise we are going to create it
  dims[0] = 1;
  m_GoodVoxelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getGoodVoxelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  if(NULL == m_GoodVoxelsPtr.lock().get() )  // The Good Voxels array was NOT available so create it
  {
    m_GoodVoxelsPtr = cellAttrMat->createNonPrereqArray<DataArray<bool>, GenericExample, bool>(this, m_GoodVoxelsArrayName, false, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
////==>MIKE_GROEBER_FIX tempPath.update(DATACONTAINER_NAME, ATTRIBUTEMATRIX_NAME, getGoodVoxelsArrayName() );
////==>MIKE_GROEBER_FIX m_GoodVoxelsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, bool>(this, tempPath, false, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GenericExample::newFilterInstance(bool copyFilterParameters)
{
  GenericExample::Pointer filter = GenericExample::New();
  if(true == copyFilterParameters)
  {
    filter->setGoodVoxelsArrayName(getGoodVoxelsArrayName());
    filter->setCellEulerAnglesArrayName(getCellEulerAnglesArrayName());
    filter->setGoodVoxelsArrayPath(getGoodVoxelsArrayPath());
    filter->setCellPatternQualityArrayPath(getCellPatternQualityArrayPath());
    filter->setFeatureIdsArrayPath(getFeatureIdsArrayPath());
    filter->setStlFilePrefix( getStlFilePrefix() );
    filter->setMaxIterations( getMaxIterations() );
    filter->setMisorientationTolerance( getMisorientationTolerance() );
    filter->setInputFile( getInputFile() );
    filter->setInputPath( getInputPath() );
    filter->setOutputFile( getOutputFile() );
    filter->setOutputPath( getOutputPath() );
    filter->setWriteAlignmentShifts( getWriteAlignmentShifts() );
    filter->setConversionType( getConversionType() );
    filter->setDimensions( getDimensions() );
    filter->setOrigin( getOrigin() );
    filter->setSelectedVolumeVertexArrayName( getSelectedVolumeVertexArrayName() );
    filter->setSelectedVolumeEdgeArrayName( getSelectedVolumeEdgeArrayName() );
    filter->setSelectedVolumeFaceArrayName( getSelectedVolumeFaceArrayName() );
    filter->setSelectedVolumeCellArrayName( getSelectedVolumeCellArrayName() );
    filter->setSelectedVolumeFeatureArrayName( getSelectedVolumeFeatureArrayName() );
    filter->setSelectedVolumeEnsembleArrayName( getSelectedVolumeEnsembleArrayName() );
    filter->setSelectedSurfaceVertexArrayName( getSelectedSurfaceVertexArrayName() );
    filter->setSelectedSurfaceEdgeArrayName( getSelectedSurfaceEdgeArrayName() );
    filter->setSelectedSurfaceFaceArrayName( getSelectedSurfaceFaceArrayName() );
    filter->setSelectedSurfaceFeatureArrayName( getSelectedSurfaceFeatureArrayName() );
    filter->setSelectedSurfaceEnsembleArrayName( getSelectedSurfaceEnsembleArrayName() );
    filter->setSelectedEdgeVertexArrayName( getSelectedEdgeVertexArrayName() );
    filter->setSelectedEdgeEdgeArrayName( getSelectedEdgeEdgeArrayName() );
    filter->setSelectedEdgeFeatureArrayName( getSelectedEdgeFeatureArrayName() );
    filter->setSelectedEdgeEnsembleArrayName( getSelectedEdgeEnsembleArrayName() );
    filter->setSelectedVertexVertexArrayName( getSelectedVertexVertexArrayName() );
    filter->setSelectedVertexFeatureArrayName( getSelectedVertexFeatureArrayName() );
    filter->setSelectedVertexEnsembleArrayName( getSelectedVertexEnsembleArrayName() );
    filter->setCrystalSymmetryRotations( getCrystalSymmetryRotations() );
  }
  return filter;
}
