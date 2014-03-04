/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "DiscreteGaussianBlur.h"

#include "ITKUtilities.h"
#include "itkDiscreteGaussianImageFilter.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DiscreteGaussianBlur::DiscreteGaussianBlur() :
AbstractFilter(),
m_RawImageDataArrayName("RawImageData"),
m_ProcessedImageDataArrayName("ProcessedData"),
m_SelectedCellArrayName(""),
m_NewCellArrayName("ProcessedArray"),
m_OverwriteArray(true),
m_RawImageData(NULL),
m_ProcessedImageData(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DiscreteGaussianBlur::~DiscreteGaussianBlur()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DiscreteGaussianBlur::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> options;
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Array to Process");
    parameter->setPropertyName("SelectedCellArrayName");
    parameter->setWidgetType(FilterParameter::VoxelCellArrayNameSelectionWidget);
    parameter->setValueType("string");
    parameter->setUnits("");
    options.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Overwrite Array");
    parameter->setPropertyName("OverwriteArray");
    parameter->setWidgetType(FilterParameter::BooleanWidget);
    parameter->setValueType("bool");
    options.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Created Array Name");
    parameter->setPropertyName("NewCellArrayName");
    parameter->setWidgetType(FilterParameter::StringWidget);
    parameter->setValueType("string");
    options.push_back(parameter);
  }
  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DiscreteGaussianBlur::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayName( reader->readValue( "SelectedCellArrayName", getSelectedCellArrayName() ) );
  setNewCellArrayName( reader->readValue( "NewCellArrayName", getNewCellArrayName() ) );
  setOverwriteArray( reader->readValue( "OverwriteArray", getOverwriteArray() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DiscreteGaussianBlur::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)

{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedCellArrayName", getSelectedCellArrayName() );
  writer->writeValue("NewCellArrayName", getNewCellArrayName() );
  writer->writeValue("OverwriteArray", getOverwriteArray() );
  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DiscreteGaussianBlur::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  if(m_SelectedCellArrayName.empty() == true)
  {
    setErrorCondition(-11000);
    ss << "An array from the Voxel Data Container must be selected.";
    addErrorMessage(getHumanLabel(),ss.str(),getErrorCondition());
  }
  else
  {
    m_RawImageDataArrayName=m_SelectedCellArrayName;
    GET_PREREQ_DATA(m, DREAM3D, CellData, RawImageData, ss, -300, uint8_t, UInt8ArrayType, voxels, 1)

    if(m_OverwriteArray)
    {

    }
    else
    {
      CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, ProcessedImageData, ss, float, FloatArrayType, 0, voxels, 1)
      m->renameCellData(m_ProcessedImageDataArrayName, m_NewCellArrayName);
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DiscreteGaussianBlur::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DiscreteGaussianBlur::execute()
{
  std::stringstream ss;
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
  int64_t totalPoints = m->getTotalPoints();
  size_t totalFields = m->getNumFieldTuples();
  size_t totalEnsembles = m->getNumEnsembleTuples();
  dataCheck(false, totalPoints, totalFields, totalEnsembles);
  if(m_OverwriteArray)
  {
    CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, ProcessedImageData, ss, float, FloatArrayType, 0, totalPoints, 1)
  }
  if (getErrorCondition() < 0)
  {
    return;
  }

  //get filter to convert m_RawImageData to itk::image
  ImageProcessing::ImportUInt8FilterType::Pointer importFilter=ITKUtilities::Dream3DUInt8toITK(m, m_RawImageData);

  //create guassian blur filter
  typedef itk::DiscreteGaussianImageFilter< ImageProcessing::UInt8ImageType,  ImageProcessing::FloatImageType > GuassianFilterType;
  GuassianFilterType::Pointer guassianFilter = GuassianFilterType::New();
  guassianFilter->SetInput(importFilter->GetOutput());
  guassianFilter->SetVariance(4);

  //have filter write to dream3d array instead of creating its own buffer
  ITKUtilities::SetITKFloatOutput(guassianFilter->GetOutput(), m_ProcessedImageData, totalPoints);

  //execute filter
  guassianFilter->Update();

  ///if we wanted to let itk allocate its own memory instead and then copy to a buffer
  //ITKUtilities::ITKFloattoDream3D(guassianFilter->GetOutput(), m_ProcessedImageData);

  //array name changing/cleanup
  if(m_OverwriteArray)
  {
    m->removeCellData(m_SelectedCellArrayName);
    bool check = m->renameCellData(m_ProcessedImageDataArrayName, m_SelectedCellArrayName);
  }

  /* Let the GUI know we are done with this filter */
   notifyStatusMessage("Complete");
}
