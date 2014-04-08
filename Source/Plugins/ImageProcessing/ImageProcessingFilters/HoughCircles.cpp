/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "HoughCircles.h"

#include "ITKUtilities.h"
#include "itkHoughTransform2DCirclesImageFilter.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HoughCircles::HoughCircles() :
AbstractFilter(),
m_RawImageDataArrayName("RawImageData"),
m_ProcessedImageDataArrayName("ProcessedData"),
m_SelectedCellArrayName(""),
m_NewCellArrayName("ProcessedArray"),
m_OverwriteArray(true),
m_RawImageData(NULL),
m_MinRadius(0),
m_MaxRadius(0),
m_NumberCircles(0),
m_ProcessedImageData(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HoughCircles::~HoughCircles()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HoughCircles::setupFilterParameters()
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
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Minimum Radius");
    option->setPropertyName("MinRadius");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("pixels");
    options.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Maximum Radius");
    option->setPropertyName("MaxRadius");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("pixels");
    options.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Number of Circles");
    option->setPropertyName("NumberCircles");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    options.push_back(option);
  }
  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HoughCircles::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayName( reader->readValue( "SelectedCellArrayName", getSelectedCellArrayName() ) );
  setNewCellArrayName( reader->readValue( "NewCellArrayName", getNewCellArrayName() ) );
  setOverwriteArray( reader->readValue( "OverwriteArray", getOverwriteArray() ) );
  setMinRadius( reader->readValue( "MinRadius", getMinRadius() ) );
  setMaxRadius( reader->readValue( "MaxRadius", getMaxRadius() ) );
  setNumberCircles( reader->readValue( "NumberCircles", getNumberCircles() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int HoughCircles::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)

{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedCellArrayName", getSelectedCellArrayName() );
  writer->writeValue("NewCellArrayName", getNewCellArrayName() );
  writer->writeValue("OverwriteArray", getOverwriteArray() );
  writer->writeValue("MinRadius", getMinRadius() );
  writer->writeValue("MaxRadius", getMaxRadius() );
  writer->writeValue("NumberCircles", getNumberCircles() );
  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HoughCircles::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
void HoughCircles::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HoughCircles::execute()
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

  for(int i=0; i<totalPoints; ++i)
  {
    m_ProcessedImageData[i]=m_RawImageData[i];
  }

  //wrap raw and processed image data as itk::images
  ImageProcessing::DefaultImageType::Pointer inputImage=ITKUtilities::Dream3DtoITK(m, m_RawImageData);
  ImageProcessing::DefaultImageType::Pointer outputImage=ITKUtilities::Dream3DtoITK(m, m_ProcessedImageData);

  ImageProcessing::DefaultSliceType::IndexType localIndex;
  typedef itk::HoughTransform2DCirclesImageFilter<ImageProcessing::DefaultPixelType, ImageProcessing::FloatPixelType> HoughTransformFilterType;
  HoughTransformFilterType::Pointer houghFilter = HoughTransformFilterType::New();
  houghFilter->SetNumberOfCircles( m_NumberCircles );
  houghFilter->SetMinimumRadius( m_MinRadius );
  houghFilter->SetMaximumRadius( m_MaxRadius );
  /*optional parameters, these are the default values
  houghFilter->SetSweepAngle( 0 );
  houghFilter->SetSigmaGradient( 1 );
  houghFilter->SetVariance( 5 );
  houghFilter->SetDiscRadiusRatio( 10 );
  */

  //loop over slices
  for(int i=0; i<dims[2]; ++i)
  {
    //extract slice and transform
    ss.str("");
    ss << "Hough Transforming Slice "<< i+1 << "/" << dims[2];
    notifyStatusMessage(ss.str());
    ImageProcessing::DefaultSliceType::Pointer inputSlice = ITKUtilities::ExtractSlice<ImageProcessing::DefaultPixelType>(inputImage, ImageProcessing::ZSlice, i);
    houghFilter->SetInput( inputSlice );
    houghFilter->Update();
    ImageProcessing::FloatSliceType::Pointer localAccumulator = houghFilter->GetOutput();

    //find circles
    ss.str("");
    ss << "Finding Circles Slice "<< i+1 << "/" << dims[2];
    notifyStatusMessage(ss.str());
    HoughTransformFilterType::CirclesListType circles = houghFilter->GetCircles( m_NumberCircles );

    //create blank slice of same dimensions
    ImageProcessing::DefaultSliceType::Pointer outputSlice = ImageProcessing::DefaultSliceType::New();
    ImageProcessing::DefaultSliceType::RegionType region;
    region.SetSize(inputSlice->GetLargestPossibleRegion().GetSize());
    region.SetIndex(inputSlice->GetLargestPossibleRegion().GetIndex());
    outputSlice->SetRegions( region );
    outputSlice->SetOrigin(inputSlice->GetOrigin());
    outputSlice->SetSpacing(inputSlice->GetSpacing());
    outputSlice->Allocate();
    outputSlice->FillBuffer(0);

    //loop over circles drawing on slice
    HoughTransformFilterType::CirclesListType::const_iterator itCircles = circles.begin();
    while( itCircles != circles.end() )
    {
      //std::cout << "Center: ";
      //std::cout << (*itCircles)->GetObjectToParentTransform()->GetOffset() << std::endl;
      //std::cout << "Radius: " << (*itCircles)->GetRadius()[0] << std::endl;

      for(double angle = 0;angle <= 2*vnl_math::pi; angle += vnl_math::pi/60.0 )
      {
        localIndex[0] = (long int)((*itCircles)->GetObjectToParentTransform()->GetOffset()[0]
        + (*itCircles)->GetRadius()[0]*vcl_cos(angle));
        localIndex[1] = (long int)((*itCircles)->GetObjectToParentTransform()->GetOffset()[1]
        + (*itCircles)->GetRadius()[0]*vcl_sin(angle));
        ImageProcessing::DefaultSliceType::RegionType outputRegion = outputSlice->GetLargestPossibleRegion();
        if( outputRegion.IsInside( localIndex ) )
        {
          outputSlice->SetPixel( localIndex, 255 );
        }
      }
      itCircles++;
    }

    //copy slice into output
    ITKUtilities::SetSlice<ImageProcessing::DefaultPixelType>(outputImage, outputSlice, ImageProcessing::ZSlice, i);
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
