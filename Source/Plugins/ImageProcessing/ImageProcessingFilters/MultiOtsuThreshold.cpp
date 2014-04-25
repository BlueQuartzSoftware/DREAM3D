/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "MultiOtsuThreshold.h"

#include "ITKUtilities.h"

#include "itkImageToHistogramFilter.h"
#include "itkOtsuMultipleThresholdsImageFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiOtsuThreshold::MultiOtsuThreshold() :
AbstractFilter(),
m_RawImageDataArrayName("RawImageData"),
m_ProcessedImageDataArrayName("ProcessedData"),
m_SelectedCellArrayName(""),
m_NewCellArrayName("ProcessedArray"),
m_OverwriteArray(true),
m_Slice(false),
m_Levels(2),
m_RawImageData(NULL),
m_ProcessedImageData(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MultiOtsuThreshold::~MultiOtsuThreshold()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiOtsuThreshold::setupFilterParameters()
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
    parameter->setHumanLabel("Number of Levels");
    parameter->setPropertyName("Levels");
    parameter->setWidgetType(FilterParameter::IntWidget);
    parameter->setValueType("int");
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
void MultiOtsuThreshold::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayName( reader->readValue( "SelectedCellArrayName", getSelectedCellArrayName() ) );
  setNewCellArrayName( reader->readValue( "NewCellArrayName", getNewCellArrayName() ) );
  setOverwriteArray( reader->readValue( "OverwriteArray", getOverwriteArray() ) );
  setSlice( reader->readValue( "Slice", getSlice() ) );
  setLevels( reader->readValue( "Levels", getLevels() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MultiOtsuThreshold::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)

{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedCellArrayName", getSelectedCellArrayName() );
  writer->writeValue("NewCellArrayName", getNewCellArrayName() );
  writer->writeValue("OverwriteArray", getOverwriteArray() );
  writer->writeValue("Slice", getSlice() );
  writer->writeValue("Levels", getLevels() );
  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiOtsuThreshold::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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

    if(m_Levels<1)
    {
      ss << "Must threshold into at least 2 levels";
      setErrorCondition(-1001);
      addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiOtsuThreshold::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MultiOtsuThreshold::execute()
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

  //get dims
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

    //wrap input as itk image
    ImageProcessing::DefaultImageType::Pointer inputImage=ITKUtilities::Dream3DtoITK(m, m_RawImageData);

    if(m_Slice)
    {
      //define 2d histogram generator
      typedef itk::OtsuMultipleThresholdsImageFilter< ImageProcessing::DefaultSliceType, ImageProcessing::DefaultSliceType > ThresholdType;
      ThresholdType::Pointer otsuThresholder = ThresholdType::New();

      //wrap output buffer as image
      ImageProcessing::DefaultImageType::Pointer outputImage=ITKUtilities::Dream3DtoITK(m, m_ProcessedImageData);

      //loop over slices
      for(int i=0; i<dims[2]; i++)
      {
        //get slice
        ImageProcessing::DefaultSliceType::Pointer slice = ITKUtilities::ExtractSlice<ImageProcessing::DefaultPixelType>(inputImage, ImageProcessing::ZSlice, i);

        //threshold
        otsuThresholder->SetInput(slice);
        otsuThresholder->SetNumberOfThresholds(m_Levels);
        otsuThresholder->SetLabelOffset(1);
        otsuThresholder->Update();

        //copy back into volume
        ITKUtilities::SetSlice<ImageProcessing::DefaultPixelType>(outputImage, otsuThresholder->GetOutput(), ImageProcessing::ZSlice, i);
      }
    }
    else
    {
      typedef itk::OtsuMultipleThresholdsImageFilter< ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType > ThresholdType;
      ThresholdType::Pointer otsuThresholder = ThresholdType::New();
      otsuThresholder->SetInput(inputImage);
      otsuThresholder->SetNumberOfThresholds(m_Levels);
      otsuThresholder->SetLabelOffset(1);

      ITKUtilities::SetITKOutput(otsuThresholder->GetOutput(), m_ProcessedImageData, totalPoints);
      otsuThresholder->Update();
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
