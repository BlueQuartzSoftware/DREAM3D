/*
 * Your License or Copyright Information can go here
 */

#include "GenericExample.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericExample::GenericExample() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_StlFilePrefix(""),
  m_MaxIterations(0),
  m_MisorientationTolerance(0),
  m_InputFile(""),
  m_InputPath(""),
  m_OutputFile(""),
  m_OutputPath(""),
  m_WriteAlignmentShifts(false),
  m_ConversionType(0)
//  m_CrystalSymmetryRotations(FIX_ME<<<<<<<<),
//  m_StrVector(FIX_ME<<<<<<<<),
//  m_SelectedVolumeVertexArrayName(FIX_ME<<<<<<<<),
//  m_SelectedVolumeEdgeArrayName(FIX_ME<<<<<<<<),
//  m_SelectedVolumeFaceArrayName(FIX_ME<<<<<<<<),
//  m_SelectedVolumeCellArrayName(FIX_ME<<<<<<<<),
//  m_SelectedVolumeFeatureArrayName(FIX_ME<<<<<<<<),
//  m_SelectedVolumeEnsembleArrayName(FIX_ME<<<<<<<<),
//  m_SelectedSurfaceVertexArrayName(FIX_ME<<<<<<<<),
//  m_SelectedSurfaceEdgeArrayName(FIX_ME<<<<<<<<),
//  m_SelectedSurfaceFaceArrayName(FIX_ME<<<<<<<<),
//  m_SelectedSurfaceFeatureArrayName(FIX_ME<<<<<<<<),
//  m_SelectedSurfaceEnsembleArrayName(FIX_ME<<<<<<<<),
//  m_SelectedEdgeVertexArrayName(FIX_ME<<<<<<<<),
//  m_SelectedEdgeEdgeArrayName(FIX_ME<<<<<<<<),
//  m_SelectedEdgeFeatureArrayName(FIX_ME<<<<<<<<),
//  m_SelectedEdgeEnsembleArrayName(FIX_ME<<<<<<<<),
//  m_SelectedVertexVertexArrayName(FIX_ME<<<<<<<<),
//  m_SelectedVertexFeatureArrayName(FIX_ME<<<<<<<<),
//  m_SelectedVertexEnsembleArrayName(FIX_ME<<<<<<<<)
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
    option->setWidgetType(FilterParameterWidgetType::StringWidget);
    option->setValueType("string");
    options.push_back(option);
  }
  /*  For an Integer use this code*/
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Max Iterations");
    option->setPropertyName("MaxIterations");
    option->setWidgetType(FilterParameterWidgetType::IntWidget);
    option->setValueType("int");
    options.push_back(option);
  }
  /*  For a Floating point value use this code*/
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Misorientation Tolerance");
    option->setPropertyName("MisorientationTolerance");
    option->setWidgetType(FilterParameterWidgetType::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    options.push_back(option);
  }
  /*   For an input file use this code*/
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input File");
    option->setPropertyName("InputFile");
    option->setWidgetType(FilterParameterWidgetType::InputFileWidget);
    option->setValueType("string");
    options.push_back(option);
  }
  /*   For an input path use this code*/
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input Path");
    option->setPropertyName("InputPath");
    option->setWidgetType(FilterParameterWidgetType::InputPathWidget);
    option->setValueType("string");
    options.push_back(option);
  }

  /*   For an output file use this code*/
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFile");
    option->setWidgetType(FilterParameterWidgetType::OutputFileWidget);
    option->setValueType("string");
    options.push_back(option);
  }
  /*   For an output path use this code*/
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output Path");
    option->setPropertyName("OutputPath");
    option->setWidgetType(FilterParameterWidgetType::OutputPathWidget);
    option->setValueType("string");
    options.push_back(option);
  }
  /*   For a simple true/false boolean use this code*/
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Alignment Shift File");
    option->setPropertyName("WriteAlignmentShifts");
    option->setWidgetType(FilterParameterWidgetType::BooleanWidget);
    option->setValueType("bool");
    options.push_back(option);
  }
  /*   For presenting a set of choices to the user use this code*/
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Conversion Type");
    option->setPropertyName("ConversionType");
    option->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
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
    option->setWidgetType(FilterParameterWidgetType::IntVec3Widget);
    option->setValueType("IntVec3Widget_t");
    option->setUnits("XYZ");
    options.push_back(option);
  }
  /* Display a group of 3 text boxes to collect 3 float values */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Origin");
    option->setPropertyName("Origin");
    option->setWidgetType(FilterParameterWidgetType::FloatVec3Widget);
    option->setValueType("FloatVec3Widget_t");
    option->setUnits("XYZ");
    options.push_back(option);
  }


  /* To Display a Combobox with a list of current Volume Cell Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Volume Vertex Array Name");
    option->setPropertyName("SelectedVolumeVertexArrayName");
    option->setWidgetType(FilterParameterWidgetType::VolumeVertexArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Volume Feature Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Volume Edge Array Name");
    option->setPropertyName("SelectedVolumeEdgeArrayName");
    option->setWidgetType(FilterParameterWidgetType::VolumeEdgeArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Volume Ensemble Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Volume Face Array Name");
    option->setPropertyName("SelectedVolumeFaceArrayName");
    option->setWidgetType(FilterParameterWidgetType::VolumeFaceArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Volume Cell Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Volume Cell Array Name");
    option->setPropertyName("SelectedVolumeCellArrayName");
    option->setWidgetType(FilterParameterWidgetType::VolumeCellArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Volume Feature Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Volume Feature Array Name");
    option->setPropertyName("SelectedVolumeFeatureArrayName");
    option->setWidgetType(FilterParameterWidgetType::VolumeFeatureArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Volume Ensemble Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Volume Ensemble Array Name");
    option->setPropertyName("SelectedVolumeEnsembleArrayName");
    option->setWidgetType(FilterParameterWidgetType::VolumeEnsembleArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }

  /* To Display a Combobox with a list of current SurfaceMesh Point Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Surface Vertex Array Name");
    option->setPropertyName("SelectedSurfaceVertexArrayName");
    option->setWidgetType(FilterParameterWidgetType::SurfaceVertexArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Surface Edge Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Surface Edge Array Name");
    option->setPropertyName("SelectedSurfaceEdgeArrayName");
    option->setWidgetType(FilterParameterWidgetType::SurfaceEdgeArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Surface Face Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Surface Face Array Name");
    option->setPropertyName("SelectedSurfaceFaceArrayName");
    option->setWidgetType(FilterParameterWidgetType::SurfaceFaceArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Surface Edge Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Surface Feature Array Name");
    option->setPropertyName("SelectedSurfaceFeatureArrayName");
    option->setWidgetType(FilterParameterWidgetType::SurfaceFeatureArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Surface Face Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Surface Ensemble Array Name");
    option->setPropertyName("SelectedSurfaceEnsembleArrayName");
    option->setWidgetType(FilterParameterWidgetType::SurfaceEnsembleArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }

  /* To Display a Combobox with a list of current EdgeMesh Point Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Edge Vertex Array Name");
    option->setPropertyName("SelectedEdgeVertexArrayName");
    option->setWidgetType(FilterParameterWidgetType::EdgeVertexArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Edge Edge Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Edge Edge Array Name");
    option->setPropertyName("SelectedEdgeEdgeArrayName");
    option->setWidgetType(FilterParameterWidgetType::EdgeEdgeArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Edge Edge Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Edge Feature Array Name");
    option->setPropertyName("SelectedEdgeFeatureArrayName");
    option->setWidgetType(FilterParameterWidgetType::EdgeFeatureArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Edge Face Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Edge Ensemble Array Name");
    option->setPropertyName("SelectedEdgeEnsembleArrayName");
    option->setWidgetType(FilterParameterWidgetType::EdgeEnsembleArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }

  /* To Display a Combobox with a list of current Vertex Point Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Vertex Vertex Array Name");
    option->setPropertyName("SelectedVertexVertexArrayName");
    option->setWidgetType(FilterParameterWidgetType::VertexVertexArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Vertex Face Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Vertex Feature Array Name");
    option->setPropertyName("SelectedVertexFeatureArrayName");
    option->setWidgetType(FilterParameterWidgetType::VertexFeatureArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Vertex Edge Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Vertex Ensemble Array Name");
    option->setPropertyName("SelectedVertexEnsembleArrayName");
    option->setWidgetType(FilterParameterWidgetType::VertexEnsembleArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }

  /* Display the AxisAngleWidget to collect Axis-Angle pairs from the user */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Crystal Rotations");
    option->setPropertyName("CrystalSymmetryRotations");
    option->setWidgetType(FilterParameterWidgetType::AxisAngleWidget);
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


  setStrVector( reader->readStrings("StrVector", getStrVector() ) );
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
  dataCheck();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExample::execute()
{
  int err = 0;
  setErrorCondition(err);

  dataCheck();
  if(getErrorCondition() < 0) { return; }
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  /* Place all your code to execute your filter here. */

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

