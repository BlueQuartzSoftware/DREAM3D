/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ReadImage.h"

#include "ITKUtilities.h"
#include "itkImageIOBase.h"
#include "itkImageFileReader.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadImage::ReadImage() :
AbstractFilter(),
m_ProcessedImageDataArrayName("ProcessedData"),
m_NewCellArrayName("ImageData"),
m_InputFileName(""),
m_ProcessedImageData(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadImage::~ReadImage()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadImage::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> options;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Input File");
    option->setPropertyName("InputFileName");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setValueType("string");
    option->setFileExtension("*.tif");
    option->setFileType("tif stack");
    options.push_back(option);
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
void ReadImage::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setNewCellArrayName( reader->readValue( "NewCellArrayName", getNewCellArrayName() ) );
  setInputFileName( reader->readValue( "InputFileName", getInputFileName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ReadImage::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)

{
  writer->openFilterGroup(this, index);
  writer->writeValue("NewCellArrayName", getNewCellArrayName() );
  writer->writeValue("InputFileName", getInputFileName() );
  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadImage::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  if(m_InputFileName.empty())
  {
    ss.str("");
    ss << "The input file name must be set before executing this filter.";
    setErrorCondition(-1);
    addErrorMessage(getHumanLabel(), ss.str(), -1);
  }
  else
  {
    CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, ProcessedImageData, ss, ImageProcessing::DefaultPixelType, ImageProcessing::DefaultArrayType, 0, voxels, 1)
    m->renameCellData(m_ProcessedImageDataArrayName, m_NewCellArrayName);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadImage::preflight()
{
  dataCheck(true, 1, 1, 1);

  VoxelDataContainer* m = getVoxelDataContainer();
  setErrorCondition(0);
  std::stringstream ss;
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  if(m_InputFileName.empty())
  {
    setErrorCondition(-1);
    notifyErrorMessage("The input file name must be set before executing this filter.", -1);
    return;
  }

  //read header information of image and make sure the dimensions are correct
  itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(m_InputFileName.c_str(), itk::ImageIOFactory::ReadMode);
  imageIO->SetFileName(m_InputFileName);
  imageIO->ReadImageInformation();
  const size_t numDimensions = imageIO->GetNumberOfDimensions();
  if(3!=numDimensions)
  {
    ss.str("");
    ss << "The selected image's dimensionality of  " << numDimensions << " does not equal 3";
    addErrorMessage(getHumanLabel(), ss.str(), -5554);
    setErrorCondition(-5554);
  }
  else
  {
    int xdim = imageIO->GetDimensions(0);
    int ydim = imageIO->GetDimensions(1);
    int zdim = imageIO->GetDimensions(2);

    if (xdim != static_cast<int>(m->getXPoints()))
    {
      ss.str("");
      ss << "The selected image's x size of " << xdim << " does not equal the dataset x size of " << static_cast<int64_t>(m->getXPoints());
      addErrorMessage(getHumanLabel(), ss.str(), -5555);
      setErrorCondition(-5555);
    }
    if (ydim != static_cast<int>(m->getYPoints()))
    {
      ss.str("");
      ss << "The selected image's y size of " << ydim << " does not equal the dataset y size of " << static_cast<int64_t>(m->getYPoints());
      addErrorMessage(getHumanLabel(), ss.str(), -5556);
      setErrorCondition(-5556);
    }
    if (zdim != static_cast<int>(m->getZPoints()))
    {
      ss.str("");
      ss << "The selected image's z size of " << zdim << " does not equal the dataset z size of " << static_cast<int64_t>(m->getZPoints());
      addErrorMessage(getHumanLabel(), ss.str(), -5557);
      setErrorCondition(-5557);
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadImage::execute()
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

  //create reader filter (already know image is the correct size from preflight)
  typedef itk::ImageFileReader<ImageProcessing::DefaultImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( m_InputFileName );

  //read image into new array
  ITKUtilities::SetITKOutput(reader->GetOutput(), m_ProcessedImageData, totalPoints);
  reader->Update();

  /* Let the GUI know we are done with this filter */
   notifyStatusMessage("Complete");
}
