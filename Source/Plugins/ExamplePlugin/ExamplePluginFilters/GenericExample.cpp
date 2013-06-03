/*
 * Your License or Copyright Information can go here
 */

#include "GenericExample.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericExample::GenericExample() :
AbstractFilter()
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
  std::vector<FilterParameter::Pointer> options;
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
    std::vector<std::string> choices;
    choices.push_back("Degrees To Radians");
    choices.push_back("Radians To Degrees");
    option->setChoices(choices);
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Voxel Cell Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Voxel Cell Array Name");
    option->setPropertyName("SelectedCellArrayName");
    option->setWidgetType(FilterParameter::VoxelCellArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Voxel Field Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Voxel Field Array Name");
    option->setPropertyName("SelectedFieldArrayName");
    option->setWidgetType(FilterParameter::VoxelFieldArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current Voxel Ensemble Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Voxel Ensemble Array Name");
    option->setPropertyName("SelectedEnsembleArrayName");
    option->setWidgetType(FilterParameter::VoxelEnsembleArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }

  /* To Display a Combobox with a list of current SurfaceMesh Point Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("SurfaceMesh Point Array Name");
    option->setPropertyName("SurfaceMeshPointArrayName");
    option->setWidgetType(FilterParameter::SolidMeshVertexArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current SurfaceMesh Face Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("SurfaceMesh Face Array Name");
    option->setPropertyName("SurfaceMeshFaceArrayName");
    option->setWidgetType(FilterParameter::SolidMeshFaceArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current SurfaceMesh Edge Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("SurfaceMesh Edge Array Name");
    option->setPropertyName("SurfaceMeshEdgeArrayName");
    option->setWidgetType(FilterParameter::SolidMeshEdgeArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }


  /* To Display a Combobox with a list of current SolidMesh Point Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("SolidMesh Point Array Name");
    option->setPropertyName("SolidMeshPointArrayName");
    option->setWidgetType(FilterParameter::SolidMeshVertexArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current SolidMesh Face Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("SolidMesh Face Array Name");
    option->setPropertyName("SolidMeshFaceArrayName");
    option->setWidgetType(FilterParameter::SolidMeshFaceArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current SolidMesh Edge Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("SolidMesh Edge Array Name");
    option->setPropertyName("SolidMeshEdgeArrayName");
    option->setWidgetType(FilterParameter::SolidMeshEdgeArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
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

  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericExample::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
 /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  writer->writeValue("StlFilePrefix", getStlFilePrefix() );
  writer->writeValue("MisorientationTolerance", getMisorientationTolerance() );

  writer->writeValue("InputFile", getInputFile() );
  writer->writeValue("InputPath", getInputPath() );
  writer->writeValue("OutputFile", getOutputFile() );
  writer->writeValue("OutputPath", getOutputPath() );
  writer->writeValue("WriteAlignmentShifts", getWriteAlignmentShifts() );
  writer->writeValue("ConversionType", getConversionType() );

  writer->writeValue("Dimensions", getDimensions() );
  writer->writeValue("Origin", getOrigin() );


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
  if (m_OutputFile.empty() == true)
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
  VoxelDataContainer* m = getVoxelDataContainer();
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
