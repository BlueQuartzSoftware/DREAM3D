/*
 * Your License or Copyright Information can go here
 */

#include "GenericExample.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericExample::GenericExample() :
  AbstractFilter(),
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
  /* To Display a Combobox with a list of current Volume Feature Arrays in it */
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
  /* To Display a Combobox with a list of current Volume Feature Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Volume Feature Array Name");
    option->setPropertyName("SelectedVolumeFeatureArrayName");
    option->setWidgetType(FilterParameter::VolumeFeatureArrayNameSelectionWidget);
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
    option->setHumanLabel("Surface Feature Array Name");
    option->setPropertyName("SelectedSurfaceFeatureArrayName");
    option->setWidgetType(FilterParameter::SurfaceFeatureArrayNameSelectionWidget);
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
    option->setHumanLabel("Edge Feature Array Name");
    option->setPropertyName("SelectedEdgeFeatureArrayName");
    option->setWidgetType(FilterParameter::EdgeFeatureArrayNameSelectionWidget);
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
    option->setHumanLabel("Vertex Feature Array Name");
    option->setPropertyName("SelectedVertexFeatureArrayName");
    option->setWidgetType(FilterParameter::VertexFeatureArrayNameSelectionWidget);
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
  setStlFilePrefix( reader->readValue("StlFilePrefix", getStlFilePrefix()) );
  setMaxIterations( reader->readValue("MaxIterations", getMaxIterations()) );
  setMisorientationTolerance( reader->readValue("MisorientationTolerance", getMisorientationTolerance()) );
  setInputFile( reader->readValue("InputFile", getInputFile()) );
  setInputPath( reader->readValue("InputPath", getInputPath()) );
  setOutputFile( reader->readString("OutputFile", getOutputFile()) );
  setOutputPath( reader->readValue("OutputPath", getOutputPath()) );
  setWriteAlignmentShifts( reader->readValue("WriteAlignmentShifts", getWriteAlignmentShifts()) );
  setConversionType( reader->readValue("ConversionType", getConversionType()) );
  setDimensions( reader->readValue("Dimensions", getDimensions()) );
  setOrigin( reader->readValue("Origin", getOrigin()) );
  setCrystalSymmetryRotations( reader->readValue("CrystalSymmetryRotations", getCrystalSymmetryRotations()) );

  setSelectedVolumeVertexArrayName( reader->readValue("SelectedVolumeVertexArrayName", getSelectedVolumeVertexArrayName()) );
  setSelectedVolumeEdgeArrayName( reader->readValue("SelectedVolumeEdgeArrayName", getSelectedVolumeEdgeArrayName()) );
  setSelectedVolumeFaceArrayName( reader->readValue("SelectedVolumeFaceArrayName", getSelectedVolumeFaceArrayName()) );
  setSelectedVolumeCellArrayName( reader->readValue("SelectedVolumeCellArrayName", getSelectedVolumeCellArrayName()) );
  setSelectedVolumeFeatureArrayName( reader->readValue("SelectedVolumeFeatureArrayName", getSelectedVolumeFeatureArrayName()) );
  setSelectedVolumeEnsembleArrayName( reader->readValue("SelectedVolumeEnsembleArrayName", getSelectedVolumeEnsembleArrayName()) );
  setSelectedSurfaceVertexArrayName( reader->readValue("SelectedSurfaceVertexArrayName", getSelectedSurfaceVertexArrayName()) );
  setSelectedSurfaceEdgeArrayName( reader->readValue("SelectedSurfaceEdgeArrayName", getSelectedSurfaceEdgeArrayName()) );
  setSelectedSurfaceFaceArrayName( reader->readValue("SelectedSurfaceFaceArrayName", getSelectedSurfaceFaceArrayName()) );
  setSelectedSurfaceFeatureArrayName( reader->readValue("SelectedSurfaceFeatureArrayName", getSelectedSurfaceFeatureArrayName()) );
  setSelectedSurfaceEnsembleArrayName( reader->readValue("SelectedSurfaceEnsembleArrayName", getSelectedSurfaceEnsembleArrayName()) );
  setSelectedEdgeVertexArrayName( reader->readValue("SelectedEdgeVertexArrayName", getSelectedEdgeVertexArrayName()) );
  setSelectedEdgeEdgeArrayName( reader->readValue("SelectedEdgeEdgeArrayName", getSelectedEdgeEdgeArrayName()) );
  setSelectedEdgeFeatureArrayName( reader->readValue("SelectedEdgeFeatureArrayName", getSelectedEdgeFeatureArrayName()) );
  setSelectedEdgeEnsembleArrayName( reader->readValue("SelectedEdgeEnsembleArrayName", getSelectedEdgeEnsembleArrayName()) );
  setSelectedVertexVertexArrayName( reader->readValue("SelectedVertexVertexArrayName", getSelectedVertexVertexArrayName()) );
  setSelectedVertexFeatureArrayName( reader->readValue("SelectedVertexFeatureArrayName", getSelectedVertexFeatureArrayName()) );
  setSelectedVertexEnsembleArrayName( reader->readValue("SelectedVertexEnsembleArrayName", getSelectedVertexEnsembleArrayName()) );


  setStrVector( reader->readValue("StrVector", getStrVector() ) );
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
void GenericExample::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
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
  VolumeDataContainer* m = getVolumeDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", getErrorCondition());
    return;
  }
  setErrorCondition(0);

  /* Place all your code to execute your filter here. */

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
