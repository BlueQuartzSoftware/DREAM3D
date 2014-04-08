/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "Threshold.h"

#include "ITKUtilities.h"

//histogram calculation
#include "itkImageToHistogramFilter.h"

//histogram based selectors
#include "itkHistogramThresholdCalculator.h"
#include "itkHuangThresholdCalculator.h"
#include "itkIntermodesThresholdCalculator.h"
#include "itkIsoDataThresholdCalculator.h"
#include "itkKittlerIllingworthThresholdCalculator.h"
#include "itkLiThresholdCalculator.h"
#include "itkMaximumEntropyThresholdCalculator.h"
#include "itkMomentsThresholdCalculator.h"
#include "itkOtsuThresholdCalculator.h"
#include "itkRenyiEntropyThresholdCalculator.h"
#include "itkShanbhagThresholdCalculator.h"
#include "itkTriangleThresholdCalculator.h"
#include "itkYenThresholdCalculator.h"

//robust automatic selection
//#include "itkGradientMagnitudeImageFilter.h"
//#include "itkRobustAutomaticThresholdCalculator.h"

//thresholding filter
#include "itkBinaryThresholdImageFilter.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Threshold::Threshold() :
AbstractFilter(),
m_RawImageDataArrayName("RawImageData"),
m_ProcessedImageDataArrayName("ProcessedData"),
m_SelectedCellArrayName(""),
m_NewCellArrayName("ProcessedArray"),
m_OverwriteArray(true),
m_Slice(false),
m_Method(7),
m_ManualParameter(128),
m_RawImageData(NULL),
m_ProcessedImageData(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Threshold::~Threshold()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Threshold::setupFilterParameters()
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
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Threshold Method");
    parameter->setPropertyName("Method");
    parameter->setWidgetType(FilterParameter::ChoiceWidget);
    parameter->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back("Huang");
    choices.push_back("Intermodes");
    choices.push_back("IsoData");
    choices.push_back("Kittler Illingworth");
    choices.push_back("Li");
    choices.push_back("Maximum Entropy");
    choices.push_back("Moments");
    choices.push_back("Otsu");
    choices.push_back("Renyi Entropy");
    choices.push_back("Shanbhag");
    choices.push_back("Triangle");
    choices.push_back("Yen");
    choices.push_back("Manual Selection");
    //choices.push_back("Robust Automatic");
    parameter->setChoices(choices);
    options.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Manual Parameter");
    parameter->setPropertyName("ManualParameter");
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
void Threshold::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayName( reader->readValue( "SelectedCellArrayName", getSelectedCellArrayName() ) );
  setNewCellArrayName( reader->readValue( "NewCellArrayName", getNewCellArrayName() ) );
  setOverwriteArray( reader->readValue( "OverwriteArray", getOverwriteArray() ) );
  setSlice( reader->readValue( "Slice", getSlice() ) );
  setManualParameter( reader->readValue( "ManualParameter", getManualParameter() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int Threshold::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)

{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedCellArrayName", getSelectedCellArrayName() );
  writer->writeValue("NewCellArrayName", getNewCellArrayName() );
  writer->writeValue("OverwriteArray", getOverwriteArray() );
  writer->writeValue("Slice", getSlice() );
  writer->writeValue("ManualParameter", getManualParameter() );
  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Threshold::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
void Threshold::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Threshold::execute()
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

    //define threshold filters
    typedef itk::BinaryThresholdImageFilter <ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType> BinaryThresholdImageFilterType;
    typedef itk::BinaryThresholdImageFilter <ImageProcessing::DefaultSliceType, ImageProcessing::DefaultSliceType> BinaryThresholdImageFilterType2D;

  if(m_Method>=0 && m_Method<=11)
  {
     //define hitogram generator (will make the same kind of histogram for 2 and 3d images
    typedef itk::Statistics::ImageToHistogramFilter<ImageProcessing::DefaultImageType> HistogramGenerator;

    //find threshold value w/ histogram
    itk::HistogramThresholdCalculator< HistogramGenerator::HistogramType, uint8_t >::Pointer calculator;

    typedef itk::HuangThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > HuangCalculatorType;
    typedef itk::IntermodesThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > IntermodesCalculatorType;
    typedef itk::IsoDataThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > IsoDataCalculatorType;
    typedef itk::KittlerIllingworthThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > KittlerIllingowrthCalculatorType;
    typedef itk::LiThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > LiCalculatorType;
    typedef itk::MaximumEntropyThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > MaximumEntropyCalculatorType;
    typedef itk::MomentsThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > MomentsCalculatorType;
    typedef itk::OtsuThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > OtsuCalculatorType;
    typedef itk::RenyiEntropyThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > RenyiEntropyCalculatorType;
    typedef itk::ShanbhagThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > ShanbhagCalculatorType;
    typedef itk::TriangleThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > TriangleCalculatorType;
    typedef itk::YenThresholdCalculator< HistogramGenerator::HistogramType, uint8_t > YenCalculatorType;

    switch(m_Method)
    {
      case 0:
        {
          calculator = HuangCalculatorType::New();
        }
        break;

      case 1:
        {
          calculator = IntermodesCalculatorType::New();
        }
        break;

      case 2:
        {
          calculator = IsoDataCalculatorType::New();
        }
        break;

      case 3:
        {
          calculator = KittlerIllingowrthCalculatorType::New();
        }
        break;

      case 4:
        {
          calculator = LiCalculatorType::New();
        }
        break;

      case 5:
        {
          calculator = MaximumEntropyCalculatorType::New();
        }
        break;

      case 6:
        {
          calculator = MomentsCalculatorType::New();
        }
        break;

      case 7:
        {
          calculator = OtsuCalculatorType::New();
        }
        break;

      case 8:
        {
          calculator = RenyiEntropyCalculatorType::New();
        }
        break;

      case 9:
        {
          calculator = ShanbhagCalculatorType::New();
        }
        break;

      case 10:
        {
          calculator = TriangleCalculatorType::New();
        }
        break;

      case 11:
        {
          calculator = YenCalculatorType::New();
        }
        break;
    }

    if(m_Slice)
    {
      //define 2d histogram generator
      typedef itk::Statistics::ImageToHistogramFilter<ImageProcessing::DefaultSliceType> HistogramGenerator2D;
      HistogramGenerator2D::Pointer histogramFilter2D = HistogramGenerator2D::New();

      //specify number of bins / bounds
      typedef HistogramGenerator2D::HistogramSizeType SizeType;
      SizeType size( 1 );
      size[0] = 255;
      histogramFilter2D->SetHistogramSize( size );
      histogramFilter2D->SetMarginalScale( 10.0 );
      HistogramGenerator2D::HistogramMeasurementVectorType lowerBound( 1 );
      HistogramGenerator2D::HistogramMeasurementVectorType upperBound( 1 );
      lowerBound[0] = 0;
      upperBound[0] = 256;
      histogramFilter2D->SetHistogramBinMinimum( lowerBound );
      histogramFilter2D->SetHistogramBinMaximum( upperBound );

      //wrap output buffer as image
      ImageProcessing::DefaultImageType::Pointer outputImage=ITKUtilities::Dream3DtoITK(m, m_ProcessedImageData);

      //loop over slices
      for(int i=0; i<dims[2]; i++)
      {
        //get slice
        ImageProcessing::DefaultSliceType::Pointer slice = ITKUtilities::ExtractSlice<ImageProcessing::DefaultPixelType>(inputImage, ImageProcessing::ZSlice, i);

        //find histogram
        histogramFilter2D->SetInput( slice );
        histogramFilter2D->Update();
        const HistogramGenerator::HistogramType * histogram = histogramFilter2D->GetOutput();

        //calculate threshold level
        calculator->SetInput(histogram);
        calculator->Update();
        const uint8_t thresholdValue = calculator->GetThreshold();

        //threshold
        BinaryThresholdImageFilterType2D::Pointer thresholdFilter = BinaryThresholdImageFilterType2D::New();
        thresholdFilter->SetInput(slice);
        thresholdFilter->SetLowerThreshold(thresholdValue);
        thresholdFilter->SetUpperThreshold(255);
        thresholdFilter->SetInsideValue(255);
        thresholdFilter->SetOutsideValue(0);
        thresholdFilter->Update();

        //copy back into volume
        ITKUtilities::SetSlice<ImageProcessing::DefaultPixelType>(outputImage, thresholdFilter->GetOutput(), ImageProcessing::ZSlice, i);
      }
    }
    else
    {
      //specify number of bins / bounds
      HistogramGenerator::Pointer histogramFilter = HistogramGenerator::New();
      typedef HistogramGenerator::HistogramSizeType SizeType;
      SizeType size( 1 );
      size[0] = 255;
      histogramFilter->SetHistogramSize( size );
      histogramFilter->SetMarginalScale( 10.0 );
      HistogramGenerator::HistogramMeasurementVectorType lowerBound( 1 );
      HistogramGenerator::HistogramMeasurementVectorType upperBound( 1 );
      lowerBound[0] = 0;
      upperBound[0] = 256;
      histogramFilter->SetHistogramBinMinimum( lowerBound );
      histogramFilter->SetHistogramBinMaximum( upperBound );

      //find histogram
      histogramFilter->SetInput( inputImage );
      histogramFilter->Update();
      const HistogramGenerator::HistogramType * histogram = histogramFilter->GetOutput();

      //calculate threshold level
      calculator->SetInput(histogram);
      calculator->Update();
      const uint8_t thresholdValue = calculator->GetThreshold();

      //threshold
      BinaryThresholdImageFilterType::Pointer thresholdFilter = BinaryThresholdImageFilterType::New();
      thresholdFilter->SetInput(inputImage);
      thresholdFilter->SetLowerThreshold(thresholdValue);
      thresholdFilter->SetUpperThreshold(255);
      thresholdFilter->SetInsideValue(255);
      thresholdFilter->SetOutsideValue(0);
      thresholdFilter->GetOutput()->GetPixelContainer()->SetImportPointer(m_ProcessedImageData, totalPoints, false);
      thresholdFilter->Update();
    }
  }
  else if(m_Method==12)//manual
  {
    //threshold
    BinaryThresholdImageFilterType::Pointer thresholdFilter = BinaryThresholdImageFilterType::New();
    thresholdFilter->SetInput(inputImage);
    thresholdFilter->SetLowerThreshold(m_ManualParameter);
    thresholdFilter->SetUpperThreshold(255);
    thresholdFilter->SetInsideValue(255);
    thresholdFilter->SetOutsideValue(0);
    thresholdFilter->GetOutput()->GetPixelContainer()->SetImportPointer(m_ProcessedImageData, totalPoints, false);
    thresholdFilter->Update();
  }
  /*
  else if(m_Method==13)//robust automatic
  {
    typedef itk::GradientMagnitudeImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType>  GradientMagnitudeType;
    typedef itk::GradientMagnitudeImageFilter<ImageProcessing::DefaultSliceType, ImageProcessing::DefaultSliceType>  GradientMagnitudeType2D;

    typedef itk::itkRobustAutomaticThresholdCalculator<ImageProcessing::DefaultImageType,ImageProcessing::DefaultImageType> SelectionType;
    typedef itk::itkRobustAutomaticThresholdCalculator<ImageProcessing::DefaultSliceType,ImageProcessing::DefaultSliceType> SelectionType2D;

    if(m_slice)
    {
      //wrap output buffer as image
      ImageProcessing::DefaultImageType::Pointer outputImage=ITKUtilities::Dream3DtoITK(m, m_ProcessedImageData);

      //loop over slices
      for(int i=0; i<dims[2]; i++)
      {
        //get slice
        ImageProcessing::DefaultSliceType::Pointer slice = ITKUtilities::ExtractSlice<ImageProcessing::DefaultPixelType>(inputImage, ImageProcessing::ZSlice, i);

        //find gradient
        GradientMagnitudeType2D::Pointer gradientFilter = GradientMagnitudeType2D::New();
        gradientFilter->setInput(slice);
        gradientFilter->Update();

        //calculate thershold
        SelectionType2D::Pointer calculatorFilter = SelectionType2D::New();
        calculatorFilter->SetInput(slice);
        calculatorFilter->SetGradientImage(gradientFilter->GetOutput());
        calculatorFilter->SetPow(m_ManualParameter);
        calculatorFilter->Update();
        const uint8_t thresholdValue = calculatorFilter->GetOutput();

        //threshold
        BinaryThresholdImageFilterType::Pointer thresholdFilter = BinaryThresholdImageFilterType::New();
        thresholdFilter->SetInput(inputImage);
        thresholdFilter->SetLowerThreshold(thresholdValue);
        thresholdFilter->SetUpperThreshold(255);
        thresholdFilter->SetInsideValue(255);
        thresholdFilter->SetOutsideValue(0);
        thresholdFilter->Update();

        //copy back into volume
        ITKUtilities::SetSlice<ImageProcessing::DefaultPixelType>(outputImage, thresholdFilter->GetOutput(), ImageProcessing::ZSlice, i);
      }
    }
    else
    {
      //find gradient
      GradientMagnitudeType::Pointer gradientFilter = GradientMagnitudeType::New();
      gradientFilter->setInput(inputImage);
      gradientFilter->Update();

      //calculate threshold
      SelectionType::Pointer calculatorFilter = SelectionType::New();
      calculatorFilter->SetInput(inputImage);
      calculatorFilter->SetGradientImage(gradientFilter->GetOutput());
      calculatorFilter->SetPow(m_ManualParameter);
      calculatorFilter->Update();
      const uint8_t thresholdValue = calculatorFilter->GetOutput();

      //threshold
      BinaryThresholdImageFilterType::Pointer thresholdFilter = BinaryThresholdImageFilterType::New();
      thresholdFilter->SetInput(inputImage);
      thresholdFilter->SetLowerThreshold(thresholdValue);
      thresholdFilter->SetUpperThreshold(255);
      thresholdFilter->SetInsideValue(255);
      thresholdFilter->SetOutsideValue(0);
      thresholdFilter->GetOutput()->GetPixelContainer()->SetImportPointer(m_ProcessedImageData, totalPoints, false);
      thresholdFilter->Update();
    }
  }
  */


  //array name changing/cleanup
  if(m_OverwriteArray)
  {
    m->removeCellData(m_SelectedCellArrayName);
    bool check = m->renameCellData(m_ProcessedImageDataArrayName, m_SelectedCellArrayName);
  }

  /* Let the GUI know we are done with this filter */
   notifyStatusMessage("Complete");
}
