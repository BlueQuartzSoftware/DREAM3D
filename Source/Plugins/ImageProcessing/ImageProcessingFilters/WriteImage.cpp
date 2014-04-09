/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "WriteImage.h"

#include "ITKUtilities.h"
#include "itkImageFileWriter.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteImage::WriteImage() :
AbstractFilter(),
m_RawImageDataArrayName("RawImageData"),
m_SelectedCellArrayName(""),
m_OutputFileName(""),
m_RawImageData(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteImage::~WriteImage()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteImage::setupFilterParameters()
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
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output File");
    option->setPropertyName("OutputFileName");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setValueType("string");
    option->setFileExtension("*.tif");
    option->setFileType("tif stack");
    options.push_back(option);
  }
  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteImage::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayName( reader->readValue( "SelectedCellArrayName", getSelectedCellArrayName() ) );
  setOutputFileName( reader->readValue( "OutputFileName", getOutputFileName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int WriteImage::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)

{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedCellArrayName", getSelectedCellArrayName() );
  writer->writeValue("OutputFileName", getOutputFileName() );
  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteImage::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
    GET_PREREQ_DATA(m, DREAM3D, CellData, RawImageData, ss, -300, ImageProcessing::DefaultPixelType, ImageProcessing::DefaultArrayType, voxels, 1)
  }

  if(m_OutputFileName.empty())
  {
    ss.str("");
    ss << "The output file name must be set before executing this filter.";
    setErrorCondition(-1);
    addErrorMessage(getHumanLabel(), ss.str(), -1);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteImage::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteImage::execute()
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
  if (getErrorCondition() < 0)
  {
    return;
  }

  //wrap m_RawImageData as itk::image
  ImageProcessing::DefaultImageType::Pointer inputImage=ITKUtilities::Dream3DtoITK(m, m_RawImageData);

  //create writer filter
  typedef itk::ImageFileWriter<ImageProcessing::DefaultImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( m_OutputFileName );
  writer->SetInput( inputImage );
  writer->Update();

  /* Let the GUI know we are done with this filter */
   notifyStatusMessage("Complete");
}
