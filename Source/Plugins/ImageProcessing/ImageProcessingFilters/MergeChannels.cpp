/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "MergeChannels.h"

#include "ITKUtilities.h"
#include "itkComposeImageFilter.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MergeChannels::MergeChannels() :
AbstractFilter(),
m_RedImageDataArrayName(DREAM3D::CellData::RedImageData),
m_GreenImageDataArrayName(DREAM3D::CellData::GreenImageData),
m_BlueImageDataArrayName(DREAM3D::CellData::BlueImageData),
m_ProcessedImageDataArrayName("ProcessedData"),
m_RedArrayName(""),
m_GreenArrayName(""),
m_BlueArrayName(""),
m_NewCellArrayName("ProcessedArray"),
m_RedImageData(NULL),
m_GreenImageData(NULL),
m_BlueImageData(NULL),
m_ProcessedImageData(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MergeChannels::~MergeChannels()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeChannels::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> options;
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Red");
    parameter->setPropertyName("RedArrayName");
    parameter->setWidgetType(FilterParameter::VoxelCellArrayNameSelectionWidget);
    parameter->setValueType("string");
    parameter->setUnits("");
    options.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Green");
    parameter->setPropertyName("GreenArrayName");
    parameter->setWidgetType(FilterParameter::VoxelCellArrayNameSelectionWidget);
    parameter->setValueType("string");
    parameter->setUnits("");
    options.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Blue");
    parameter->setPropertyName("BlueArrayName");
    parameter->setWidgetType(FilterParameter::VoxelCellArrayNameSelectionWidget);
    parameter->setValueType("string");
    parameter->setUnits("");
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
void MergeChannels::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setRedArrayName( reader->readValue( "RedArrayName", getRedArrayName() ) );
  setGreenArrayName( reader->readValue( "GreenArrayName", getGreenArrayName() ) );
  setBlueArrayName( reader->readValue( "BlueArrayName", getBlueArrayName() ) );
  setNewCellArrayName( reader->readValue( "NewCellArrayName", getNewCellArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MergeChannels::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)

{
  writer->openFilterGroup(this, index);
  writer->writeValue("RedArrayName", getRedArrayName() );
  writer->writeValue("GreenArrayName", getGreenArrayName() );
  writer->writeValue("BlueArrayName", getBlueArrayName() );
  writer->writeValue("NewCellArrayName", getNewCellArrayName() );
  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeChannels::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  if(m_RedArrayName.empty() == true)
  {
    setErrorCondition(-11000);
    ss << "A red array from the Voxel Data Container must be selected.";
    addErrorMessage(getHumanLabel(),ss.str(),getErrorCondition());
  } else if (m_GreenArrayName.empty() == true)
  {
    setErrorCondition(-11000);
    ss << "A green array from the Voxel Data Container must be selected.";
    addErrorMessage(getHumanLabel(),ss.str(),getErrorCondition());
  } else if (m_BlueArrayName.empty() == true)
  {
    setErrorCondition(-11000);
    ss << "A blue array from the Voxel Data Container must be selected.";
    addErrorMessage(getHumanLabel(),ss.str(),getErrorCondition());
  }
  else
  {
    m_RedImageDataArrayName=m_RedArrayName;
    m_GreenImageDataArrayName=m_GreenArrayName;
    m_BlueImageDataArrayName=m_BlueArrayName;
    GET_PREREQ_DATA(m, DREAM3D, CellData, RedImageData, ss, -300, ImageProcessing::DefaultPixelType, ImageProcessing::DefaultArrayType, voxels, 1)
    GET_PREREQ_DATA(m, DREAM3D, CellData, GreenImageData, ss, -300, ImageProcessing::DefaultPixelType, ImageProcessing::DefaultArrayType, voxels, 1)
    GET_PREREQ_DATA(m, DREAM3D, CellData, BlueImageData, ss, -300, ImageProcessing::DefaultPixelType, ImageProcessing::DefaultArrayType, voxels, 1)

    CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, ProcessedImageData, ss, ImageProcessing::DefaultPixelType, ImageProcessing::DefaultArrayType, 0, voxels, 3)
    m->renameCellData(m_ProcessedImageDataArrayName, m_NewCellArrayName);

  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeChannels::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MergeChannels::execute()
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

  //wrap input as itk::image
  ImageProcessing::DefaultImageType::Pointer redImage=ITKUtilities::Dream3DtoITK(m, m_RedImageData);
  ImageProcessing::DefaultImageType::Pointer greenImage=ITKUtilities::Dream3DtoITK(m, m_GreenImageData);
  ImageProcessing::DefaultImageType::Pointer blueImage=ITKUtilities::Dream3DtoITK(m, m_BlueImageData);

  //set up filter to compose image
  typedef itk::ComposeImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::RGBDefaultImageType> ComposeRGBType;
  ComposeRGBType::Pointer composeRGB = ComposeRGBType::New();
  composeRGB->SetInput(0, redImage);
  composeRGB->SetInput(1, greenImage);
  composeRGB->SetInput(2, blueImage);

  //set output and execute
  ITKUtilities::SetITKOutput(composeRGB->GetOutput(), reinterpret_cast<ImageProcessing::RGBDefaultPixelType*>(m_ProcessedImageData), totalPoints);
  composeRGB->Update();

  /* Let the GUI know we are done with this filter */
   notifyStatusMessage("Complete");
}
