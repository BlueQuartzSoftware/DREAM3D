/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "MedianKernel.h"

#include "ITKUtilities.h"
#include "itkMedianImageFilter.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MedianKernel::MedianKernel() :
AbstractFilter(),
m_RawImageDataArrayName("RawImageData"),
m_ProcessedImageDataArrayName("ProcessedData"),
m_SelectedCellArrayName(""),
m_NewCellArrayName("ProcessedArray"),
m_OverwriteArray(true),
m_RawImageData(NULL),
m_ProcessedImageData(NULL)
{
  m_KernelSize.x = 2;
  m_KernelSize.y = 2;
  m_KernelSize.z = 2;
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MedianKernel::~MedianKernel()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MedianKernel::setupFilterParameters()
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
    option->setHumanLabel("Kernel Size");
    option->setPropertyName("KernelSize");
    option->setWidgetType(FilterParameter::IntVec3Widget);
    option->setValueType("IntVec3Widget_t");
    option->setUnits("voxels");
    options.push_back(option);
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
void MedianKernel::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayName( reader->readValue( "SelectedCellArrayName", getSelectedCellArrayName() ) );
  setNewCellArrayName( reader->readValue( "NewCellArrayName", getNewCellArrayName() ) );
  setOverwriteArray( reader->readValue( "OverwriteArray", getOverwriteArray() ) );
  setKernelSize( reader->readValue( "KernelSize", getKernelSize() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MedianKernel::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)

{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedCellArrayName", getSelectedCellArrayName() );
  writer->writeValue("NewCellArrayName", getNewCellArrayName() );
  writer->writeValue("OverwriteArray", getOverwriteArray() );
  writer->writeValue("KernelSize", getKernelSize() );
  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MedianKernel::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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

    if(m_OverwriteArray)
    {

    }
    else
    {
      CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, ProcessedImageData, ss, ImageProcessing::DefaultPixelType, ImageProcessing::DefaultArrayType, 0, voxels, 1)
      m->renameCellData(m_ProcessedImageDataArrayName, m_NewCellArrayName);
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MedianKernel::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MedianKernel::execute()
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
    CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, ProcessedImageData, ss, ImageProcessing::DefaultPixelType, ImageProcessing::DefaultArrayType, 0, totalPoints, 1)
  }
  if (getErrorCondition() < 0)
  {
    return;
  }

  //wrap m_RawImageData as itk::image
  ImageProcessing::DefaultImageType::Pointer inputImage=ITKUtilities::Dream3DtoITK(m, m_RawImageData);

  //create edge filter
  typedef itk::MedianImageFilter < ImageProcessing::DefaultImageType,  ImageProcessing::DefaultImageType > MedianFilterType;
  MedianFilterType::Pointer medianFilter = MedianFilterType::New();
  medianFilter->SetInput(inputImage);

  //set kernel size
  MedianFilterType::InputSizeType radius;
  radius[0]=m_KernelSize.x;
  radius[1]=m_KernelSize.y;
  radius[2]=m_KernelSize.z;
  medianFilter->SetRadius(radius);

  //have filter write to dream3d array instead of creating its own buffer
  ITKUtilities::SetITKOutput(medianFilter->GetOutput(), m_ProcessedImageData, totalPoints);

  //execute filter
  medianFilter->Update();

  //array name changing/cleanup
  if(m_OverwriteArray)
  {
    m->removeCellData(m_SelectedCellArrayName);
    bool check = m->renameCellData(m_ProcessedImageDataArrayName, m_SelectedCellArrayName);
  }

  /* Let the GUI know we are done with this filter */
   notifyStatusMessage("Complete");
}
