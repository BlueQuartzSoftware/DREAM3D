/*
 * Your License or Copyright Information can go here
 */

#include "ThresholdExample.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ThresholdExample::ThresholdExample() :
AbstractFilter()
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
  std::vector<FilterParameter::Pointer> options;

  /* To Compare Arrays like a threshold filter */
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Voxel Cell Arrays to Threshold");
    parameter->setPropertyName("CellComparisonInputs");
    parameter->setWidgetType(FilterParameter::CellArrayComparisonSelectionWidget);
    parameter->setValueType("std::vector<ComparisonInput_t>");
    options.push_back(parameter);
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




  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThresholdExample::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
 /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */


  /* --- CellArrayComparisonSelectionWidget --- */
  {
    int numQFilters = static_cast<int>( m_CellComparisonInputs.size() );
    writer->writeValue("NumComparisons0",  numQFilters);
    std::stringstream ss;
    for(int i = 0; i < numQFilters; i++)
    {
      ss << "Comparison-" << i;
      writer->writeValue(ss.str(), m_CellComparisonInputs[i]);
      ss.str("");
    }
  }

  /* --- FieldArrayComparisonSelectionWidget --- */
  {
    int numQFilters = static_cast<int>( m_FieldComparisonInputs.size() );
    writer->writeValue("NumComparisons1",  numQFilters);
    std::stringstream ss;
    for(int i = 0; i < numQFilters; i++)
    {
      ss << "Comparison-" << i;
      writer->writeValue(ss.str(), m_FieldComparisonInputs[i]);
      ss.str("");
    }
  }

  /* --- EnsembleArrayComparisonSelectionWidget --- */
  {
    int numQFilters = static_cast<int>( m_EnsembleComparisonInputs.size() );
    writer->writeValue("NumComparisons2",  numQFilters);
    std::stringstream ss;
    for(int i = 0; i < numQFilters; i++)
    {
      ss << "Comparison-" << i;
      writer->writeValue(ss.str(), m_EnsembleComparisonInputs[i]);
      ss.str("");
    }
  }

  /* --- PointArrayComparisonSelectionWidget --- */
  {
    int numQFilters = static_cast<int>( m_PointComparisonInputs.size() );
    writer->writeValue("NumComparisons3",  numQFilters);
    std::stringstream ss;
    for(int i = 0; i < numQFilters; i++)
    {
      ss << "Comparison-" << i;
      writer->writeValue(ss.str(), m_PointComparisonInputs[i]);
      ss.str("");
    }
  }

  /* --- FaceArrayComparisonSelectionWidget --- */
  {
    int numQFilters = static_cast<int>( m_FaceComparisonInputs.size() );
    writer->writeValue("NumComparisons4",  numQFilters);
    std::stringstream ss;
    for(int i = 0; i < numQFilters; i++)
    {
      ss << "Comparison-" << i;
      writer->writeValue(ss.str(), m_FaceComparisonInputs[i]);
      ss.str("");
    }
  }

  /* --- EdgeArrayComparisonSelectionWidget --- */
  {
    int numQFilters = static_cast<int>( m_EdgeComparisonInputs.size() );
    writer->writeValue("NumComparisons5",  numQFilters);
    std::stringstream ss;
    for(int i = 0; i < numQFilters; i++)
    {
      ss << "Comparison-" << i;
      writer->writeValue(ss.str(), m_EdgeComparisonInputs[i]);
      ss.str("");
    }
  }

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThresholdExample::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
void ThresholdExample::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ThresholdExample::execute()
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
