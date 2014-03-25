/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "Watershed.h"

#include "ITKUtilities.h"
#include "itkGradientMagnitudeImageFilter.h"
#include "itkWatershedImageFilter.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Watershed::Watershed() :
AbstractFilter(),
m_RawImageDataArrayName("RawImageData"),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_RawImageData(NULL),
m_GrainIds(NULL),
m_Threshold(0.005f),
m_Level(0.5f)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Watershed::~Watershed()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Watershed::setupFilterParameters()
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
    option->setHumanLabel("Watershed Threshold");
    option->setPropertyName("Threshold");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    //option->setUnits("units");
    options.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Watershed Level");
    option->setPropertyName("Level");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    //option->setUnits("units");
    options.push_back(option);
  }
  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Watershed::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayName( reader->readValue( "SelectedCellArrayName", getSelectedCellArrayName() ) );
  setThreshold( reader->readValue( "Threshold", getThreshold() ) );
  setLevel( reader->readValue( "Level", getLevel() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int Watershed::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)

{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedCellArrayName", getSelectedCellArrayName() );
  writer->writeValue("Threshold", getThreshold() );
  writer->writeValue("Level", getLevel() );
  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Watershed::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
  }

  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, int32_t, Int32ArrayType, 0, voxels, 1)

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Watershed::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Watershed::execute()
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
  ImageProcessing::UInt8ImageType::Pointer inputImage=ITKUtilities::Dream3DtoITK(m, m_RawImageData);

  //create gradient magnitude filter
  notifyStatusMessage("Calculating Gradient Magnitude");
  typedef itk::GradientMagnitudeImageFilter<ImageProcessing::UInt8ImageType, ImageProcessing::UInt8ImageType >  GradientMagnitudeImageFilterType;
  GradientMagnitudeImageFilterType::Pointer gradientMagnitudeImageFilter = GradientMagnitudeImageFilterType::New();
  gradientMagnitudeImageFilter->SetInput(inputImage);
  gradientMagnitudeImageFilter->Update();

  //watershed image
  notifyStatusMessage("Watershedding");
  typedef itk::WatershedImageFilter<ImageProcessing::UInt8ImageType> WatershedFilterType;
  WatershedFilterType::Pointer watershed = WatershedFilterType::New();
  watershed->SetThreshold(m_Threshold);
  watershed->SetLevel(m_Level);
  watershed->SetInput(gradientMagnitudeImageFilter->GetOutput());

  //execute filter
  watershed->Update();

  //get output and copy to grainids
  typedef itk::Image<unsigned long, ImageProcessing::ImageDimension>   WatershedImageType;
  WatershedImageType::Pointer output = watershed->GetOutput();
  WatershedImageType::RegionType filterRegion = output->GetLargestPossibleRegion();
  typedef itk::ImageRegionConstIterator<itk::Image<unsigned long, ImageProcessing::ImageDimension> > WatershedIteratorType;
  WatershedIteratorType it(output, filterRegion);
  it.GoToBegin();
  int index=0;
  while(!it.IsAtEnd())
  {
    m_GrainIds[index]=it.Get();
    ++it;
    ++index;
  }

  /* Let the GUI know we are done with this filter */
   notifyStatusMessage("Complete");
}
