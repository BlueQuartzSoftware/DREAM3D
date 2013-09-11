/*
 * Your License or Copyright Information can go here
 */

#include "GenericFilter.h"
#include "DREAM3DLib/HDF5/H5FilterParametersConstants.h"

#define StlFilePrefixDefaultValue "StlFilePrefixDefaultValue"
#define MaxIterationsDefaultValue 92
#define MisorientationToleranceDefaultValue 21
#define InputFileDefaultValue "InputFileDefaultValue.html"
#define InputPathDefaultValue "InputPathDefaultValue.html"
#define OutputFileDefaultValue "OutputFileDefaultValue.html"
#define OutputPathDefaultValue "OutputPathDefaultValue.html"
#define WriteAlignmentShiftsDefaultValue false
#define ConversionTypeDefaultValue 78
#define SelectedCellArrayNameDefaultValue "SelectedCellArrayNameDefaultValue"
#define SelectedFieldArrayNameDefaultValue "SelectedFieldArrayNameDefaultValue"
#define SelectedEnsembleArrayNameDefaultValue "SelectedEnsembleArrayNameDefaultValue"
#define SurfaceMeshPointArrayNameDefaultValue "SurfaceMeshPointArrayNameDefaultValue"
#define SurfaceMeshFaceArrayNameDefaultValue "SurfaceMeshFaceArrayNameDefaultValue"
#define SurfaceMeshEdgeArrayNameDefaultValue "SurfaceMeshEdgeArrayNameDefaultValue"
#define SolidMeshPointArrayNameDefaultValue "SolidMeshPointArrayNameDefaultValue"
#define SolidMeshFaceArrayNameDefaultValue "SolidMeshFaceArrayNameDefaultValue"
#define SolidMeshEdgeArrayNameDefaultValue "SolidMeshEdgeArrayNameDefaultValue"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericFilter::GenericFilter() :
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
GenericFilter::~GenericFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericFilter::setupFilterParameters()
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
    std::vector<QString> choices;
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
    option->setWidgetType(FilterParameter::SurfaceMeshVertexArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current SurfaceMesh Face Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("SurfaceMesh Face Array Name");
    option->setPropertyName("SurfaceMeshFaceArrayName");
    option->setWidgetType(FilterParameter::SurfaceMeshFaceArrayNameSelectionWidget);
    option->setValueType("string");
    option->setUnits("");
    options.push_back(option);
  }
  /* To Display a Combobox with a list of current SurfaceMesh Edge Arrays in it */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("SurfaceMesh Edge Array Name");
    option->setPropertyName("SurfaceMeshEdgeArrayName");
    option->setWidgetType(FilterParameter::SurfaceMeshEdgeArrayNameSelectionWidget);
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
  /* To Compare Arrays like a threshold filter */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Voxel Cell Arrays to Threshold");
    option->setPropertyName("CellComparisonInputs");
    option->setWidgetType(FilterParameter::CellArrayComparisonSelectionWidget);
    option->setValueType("std::vector<ComparisonInput_t>");
    options.push_back(option);
  }
  /* To Compare Arrays like a threshold filter */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Voxel Field Arrays to Threshold");
    parameter->setPropertyName("FieldComparisonInputs");
    parameter->setWidgetType(FilterParameter::FieldArrayComparisonSelectionWidget);
    parameter->setValueType("std::vector<ComparisonInput_t>");
    options.push_back(parameter);
  }
  /* To Compare Arrays like a threshold filter */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Voxel Ensemble Arrays to Threshold");
    parameter->setPropertyName("EnsembleComparisonInputs");
    parameter->setWidgetType(FilterParameter::EnsembleArrayComparisonSelectionWidget);
    parameter->setValueType("std::vector<ComparisonInput_t>");
    options.push_back(parameter);
  }
  /* To Compare Arrays like a threshold filter */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Surface Mesh Point Arrays to Threshold");
    parameter->setPropertyName("PointComparisonInputs");
    parameter->setWidgetType(FilterParameter::PointArrayComparisonSelectionWidget);
    parameter->setValueType("std::vector<ComparisonInput_t>");
    options.push_back(parameter);
  }
  /* To Compare Arrays like a threshold filter */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Surface Mesh Face Arrays to Threshold");
    parameter->setPropertyName("FaceComparisonInputs");
    parameter->setWidgetType(FilterParameter::FaceArrayComparisonSelectionWidget);
    parameter->setValueType("std::vector<ComparisonInput_t>");
    options.push_back(parameter);
  }
  /* To Compare Arrays like a threshold filter */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Surface Mesh Edge Arrays to Threshold");
    parameter->setPropertyName("EdgeComparisonInputs");
    parameter->setWidgetType(FilterParameter::EdgeArrayComparisonSelectionWidget);
    parameter->setValueType("std::vector<ComparisonInput_t>");
    options.push_back(parameter);
  }
  /* Display the AxisAngleWidget to collect Axis-Angle pairs from the user */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Crystal Rotations");
    option->setPropertyName("AxisAngleRotations");
    option->setWidgetType(FilterParameter::AxisAngleWidget);
    options.push_back(option);
  }
  /* To select arrays */
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Arrays to Delete");
    option->setPropertyName("ArraysToDelete");
    option->setWidgetType(FilterParameter::ArraySelectionWidget);
    options.push_back(option);
  }

  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericFilter::readFilterParameters(AbstractFilterParametersReader* reader)
{
  setStlFilePrefix( reader->readValue(H5FilterParameter::StlFilePrefixConstant, StlFilePrefixDefaultValue) );
  setMaxIterations( reader->readValue(H5FilterParameter::MaxIterationsConstant, MaxIterationsDefaultValue) );
  setMisorientationTolerance( reader->readValue(H5FilterParameter::MisorientationToleranceConstant, MisorientationToleranceDefaultValue) );
  setInputFile( reader->readValue(H5FilterParameter::InputFileConstant, InputFileDefaultValue) );
  setInputPath( reader->readValue(H5FilterParameter::InputPathConstant, InputPathDefaultValue) );
  setOutputFile( reader->readValue(H5FilterParameter::OutputFileConstant, OutputFileDefaultValue) );
  setOutputPath( reader->readValue(H5FilterParameter::OutputPathConstant, OutputPathDefaultValue) );
  setWriteAlignmentShifts( reader->readValue(H5FilterParameter::WriteAlignmentShiftsConstant, WriteAlignmentShiftsDefaultValue) );
  setConversionType( reader->readValue(H5FilterParameter::ConversionTypeConstant, ConversionTypeDefaultValue) );
  setSelectedCellArrayName( reader->readValue(H5FilterParameter::SelectedCellArrayNameConstant, SelectedCellArrayNameDefaultValue) );
  setSelectedFieldArrayName( reader->readValue(H5FilterParameter::SelectedFieldArrayNameConstant, SelectedFieldArrayNameDefaultValue) );
  setSelectedEnsembleArrayName( reader->readValue(H5FilterParameter::SelectedEnsembleArrayNameConstant, SelectedEnsembleArrayNameDefaultValue) );
  setSurfaceMeshPointArrayName( reader->readValue(H5FilterParameter::SurfaceMeshPointArrayNameConstant, SurfaceMeshPointArrayNameDefaultValue) );
  setSurfaceMeshFaceArrayName( reader->readValue(H5FilterParameter::SurfaceMeshFaceArrayNameConstant, SurfaceMeshFaceArrayNameDefaultValue) );
  setSurfaceMeshEdgeArrayName( reader->readValue(H5FilterParameter::SurfaceMeshEdgeArrayNameConstant, SurfaceMeshEdgeArrayNameDefaultValue) );
  setSolidMeshPointArrayName( reader->readValue(H5FilterParameter::SolidMeshPointArrayNameConstant, SolidMeshPointArrayNameDefaultValue) );
  setSolidMeshFaceArrayName( reader->readValue(H5FilterParameter::SolidMeshFaceArrayNameConstant, SolidMeshFaceArrayNameDefaultValue) );
  setSolidMeshEdgeArrayName( reader->readValue(H5FilterParameter::SolidMeshEdgeArrayNameConstant, SolidMeshEdgeArrayNameDefaultValue) );

  setDimensions( reader->readValue(H5FilterParameter::GenericTestDimensionsConstant, m_Dimensions) );
  setOrigin( reader->readValue(H5FilterParameter::GenericTestOriginConstant, m_Origin) );

  setCellComparisonInputs( reader->readValue(H5FilterParameter::CellComparisonInputsConstant, m_CellComparisonInputs) );
  setFieldComparisonInputs( reader->readValue(H5FilterParameter::FieldComparisonInputsConstant, m_FieldComparisonInputs) );
  setEnsembleComparisonInputs( reader->readValue(H5FilterParameter::EnsembleComparisonInputsConstant, m_EnsembleComparisonInputs) );
  setPointComparisonInputs( reader->readValue(H5FilterParameter::PointComparisonInputsConstant, m_PointComparisonInputs) );
  setFaceComparisonInputs( reader->readValue(H5FilterParameter::FaceComparisonInputsConstant, m_FaceComparisonInputs) );
  setEdgeComparisonInputs( reader->readValue(H5FilterParameter::EdgeComparisonInputsConstant, m_EdgeComparisonInputs) );

  setAxisAngleRotations( reader->readValue(H5FilterParameter::AxisAngleInputsConstant, m_AxisAngleRotations) );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GenericFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
 /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  writer->writeValue(H5FilterParameter::StlFilePrefixConstant, getStlFilePrefix() );
  writer->writeValue(H5FilterParameter::MaxIterationsConstant, getMaxIterations() );
  writer->writeValue(H5FilterParameter::MisorientationToleranceConstant, getMisorientationTolerance() );
  writer->writeValue(H5FilterParameter::InputFileConstant, getInputFile() );
  writer->writeValue(H5FilterParameter::InputPathConstant, getInputPath() );
  writer->writeValue(H5FilterParameter::OutputFileConstant, getOutputFile() );
  writer->writeValue(H5FilterParameter::OutputPathConstant, getOutputPath() );
  writer->writeValue(H5FilterParameter::WriteAlignmentShiftsConstant, getWriteAlignmentShifts() );
  writer->writeValue(H5FilterParameter::ConversionTypeConstant, getConversionType() );
  writer->writeValue(H5FilterParameter::SelectedCellArrayNameConstant, getSelectedCellArrayName() );
  writer->writeValue(H5FilterParameter::SelectedFieldArrayNameConstant, getSelectedFieldArrayName() );
  writer->writeValue(H5FilterParameter::SelectedEnsembleArrayNameConstant, getSelectedEnsembleArrayName() );
  writer->writeValue(H5FilterParameter::SurfaceMeshPointArrayNameConstant, getSurfaceMeshPointArrayName() );
  writer->writeValue(H5FilterParameter::SurfaceMeshFaceArrayNameConstant, getSurfaceMeshFaceArrayName() );
  writer->writeValue(H5FilterParameter::SurfaceMeshEdgeArrayNameConstant, getSurfaceMeshEdgeArrayName() );
  writer->writeValue(H5FilterParameter::SolidMeshPointArrayNameConstant, getSolidMeshPointArrayName() );
  writer->writeValue(H5FilterParameter::SolidMeshFaceArrayNameConstant, getSolidMeshFaceArrayName() );
  writer->writeValue(H5FilterParameter::SolidMeshEdgeArrayNameConstant, getSolidMeshEdgeArrayName() );

  writer->writeValue(H5FilterParameter::GenericTestDimensionsConstant, getDimensions() );
  writer->writeValue(H5FilterParameter::GenericTestOriginConstant, getOrigin() );

  writer->writeValue(H5FilterParameter::CellComparisonInputsConstant, getCellComparisonInputs() );
  writer->writeValue(H5FilterParameter::FieldComparisonInputsConstant, getFieldComparisonInputs() );
  writer->writeValue(H5FilterParameter::EnsembleComparisonInputsConstant, getEnsembleComparisonInputs() );
  writer->writeValue(H5FilterParameter::PointComparisonInputsConstant, getPointComparisonInputs() );
  writer->writeValue(H5FilterParameter::FaceComparisonInputsConstant, getFaceComparisonInputs() );
  writer->writeValue(H5FilterParameter::EdgeComparisonInputsConstant, getEdgeComparisonInputs() );

  writer->writeValue(H5FilterParameter::AxisAngleInputsConstant, getAxisAngleRotations() );

  writer->closeFilterGroup();
  return index;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericFilter::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
void GenericFilter::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericFilter::execute()
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
