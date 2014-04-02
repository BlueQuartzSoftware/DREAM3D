/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "SobelEdge.h"

#include "ITKUtilities.h"
#include "itkSobelEdgeDetectionImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SobelEdge::SobelEdge() :
AbstractFilter(),
m_RawImageDataArrayName("RawImageData"),
m_ProcessedImageDataArrayName("ProcessedData"),
m_SelectedCellArrayName(""),
m_NewCellArrayName("ProcessedArray"),
m_OverwriteArray(true),
m_Slice(false),
m_RawImageData(NULL),
m_ProcessedImageData(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SobelEdge::~SobelEdge()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SobelEdge::setupFilterParameters()
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
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Slice at a Time");
    parameter->setPropertyName("Slice");
    parameter->setWidgetType(FilterParameter::BooleanWidget);
    parameter->setValueType("bool");
    options.push_back(parameter);
  }
  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SobelEdge::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayName( reader->readValue( "SelectedCellArrayName", getSelectedCellArrayName() ) );
  setNewCellArrayName( reader->readValue( "NewCellArrayName", getNewCellArrayName() ) );
  setOverwriteArray( reader->readValue( "OverwriteArray", getOverwriteArray() ) );
  setSlice( reader->readValue( "Slice", getSlice() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SobelEdge::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)

{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedCellArrayName", getSelectedCellArrayName() );
  writer->writeValue("NewCellArrayName", getNewCellArrayName() );
  writer->writeValue("OverwriteArray", getOverwriteArray() );
  writer->writeValue("Slice", getSlice() );
  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SobelEdge::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
      CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, ProcessedImageData, ss, uint8_t, UInt8ArrayType, 0, voxels, 1)
      m->renameCellData(m_ProcessedImageDataArrayName, m_NewCellArrayName);
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SobelEdge::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SobelEdge::execute()
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
    CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, ProcessedImageData, ss, uint8_t, UInt8ArrayType, 0, totalPoints, 1)
  }
  if (getErrorCondition() < 0)
  {
    return;
  }

  //wrap m_RawImageData as itk::image
  ImageProcessing::UInt8ImageType::Pointer inputImage=ITKUtilities::Dream3DtoITK(m, m_RawImageData);

  if(m_Slice)
  {
    //wrap output array
    ImageProcessing::UInt8ImageType::Pointer outputImage=ITKUtilities::Dream3DtoITK(m, m_ProcessedImageData);

    //get dimensions
    size_t udims[3] = {0,0,0};
    m->getDimensions(udims);
  #if (CMP_SIZEOF_SIZE_T == 4)
    typedef int32_t DimType;
  #else
    typedef int64_t DimType;
  #endif
    DimType dims[3] = {
      static_cast<DimType>(udims[0]),
      static_cast<DimType>(udims[1]),
      static_cast<DimType>(udims[2]),
    };

    //create edge filter
    typedef itk::SobelEdgeDetectionImageFilter < ImageProcessing::UInt8SliceType,  ImageProcessing::FloatSliceType > SobelFilterType;
    SobelFilterType::Pointer sobelFilter = SobelFilterType::New();

    //convert result back to uint8
    typedef itk::RescaleIntensityImageFilter<ImageProcessing::FloatSliceType, ImageProcessing::UInt8SliceType> RescaleImageType;
    RescaleImageType::Pointer rescaleFilter = RescaleImageType::New();
    rescaleFilter->SetOutputMinimum(0);
    rescaleFilter->SetOutputMaximum(255);

    //loop over slices applying filters
    for(int i=0; i<dims[2]; ++i)
    {
      ss.str("");
      ss << "Finding Edges Slice "<< i+1 << "/" << dims[2];
      notifyStatusMessage(ss.str());

      //get slice
      ImageProcessing::UInt8SliceType::Pointer inputSlice = ITKUtilities::ExtractSlice<ImageProcessing::UInt8PixelType>(inputImage, ImageProcessing::ZSlice, i);

      //run filters
      sobelFilter->SetInput(inputSlice);
      rescaleFilter->SetInput(sobelFilter->GetOutput());
      sobelFilter->Update();
      rescaleFilter->Update();

      //copy into volume
      ITKUtilities::SetSlice<ImageProcessing::UInt8PixelType>(outputImage, rescaleFilter->GetOutput(), ImageProcessing::ZSlice, i);
    }
  }
  else
  {
    //create edge filter
    typedef itk::SobelEdgeDetectionImageFilter < ImageProcessing::UInt8ImageType,  ImageProcessing::FloatImageType > SobelFilterType;
    SobelFilterType::Pointer sobelFilter = SobelFilterType::New();
    sobelFilter->SetInput(inputImage);

    //convert result back to uint8
    typedef itk::RescaleIntensityImageFilter<ImageProcessing::FloatImageType, ImageProcessing::UInt8ImageType> RescaleImageType;
    RescaleImageType::Pointer rescaleFilter = RescaleImageType::New();
    rescaleFilter->SetInput(sobelFilter->GetOutput());
    rescaleFilter->SetOutputMinimum(0);
    rescaleFilter->SetOutputMaximum(255);

    //have filter write to dream3d array instead of creating its own buffer
    ITKUtilities::SetITKOutput(rescaleFilter->GetOutput(), m_ProcessedImageData, totalPoints);

    //execute filters
    sobelFilter->Update();
    rescaleFilter->Update();
  }

  //array name changing/cleanup
  if(m_OverwriteArray)
  {
    m->removeCellData(m_SelectedCellArrayName);
    bool check = m->renameCellData(m_ProcessedImageDataArrayName, m_SelectedCellArrayName);
  }

  /* Let the GUI know we are done with this filter */
   notifyStatusMessage("Complete");
}
