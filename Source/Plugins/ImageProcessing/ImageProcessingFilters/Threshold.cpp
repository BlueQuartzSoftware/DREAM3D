/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "Threshold.h"

#include "ITKUtilities.h"

#include "itkImageToHistogramFilter.h"
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
    choices.push_back("Enyi");
    choices.push_back("Shanbhag");
    choices.push_back("Triangle");
    choices.push_back("Yen");
    parameter->setChoices(choices);
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
    CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, ProcessedImageData, ss, uint8_t, UInt8ArrayType, 0, totalPoints, 1)
  }
  if (getErrorCondition() < 0)
  {
    return;
  }

   //wrap m_RawImageData as itk::image define / create histogram filter
  typedef itk::Statistics::ImageToHistogramFilter<ImageProcessing::UInt8ImageType> HistogramGenerator;
  HistogramGenerator::Pointer histogramFilter = HistogramGenerator::New();
  ImageProcessing::UInt8ImageType::Pointer inputImage=ITKUtilities::Dream3DtoITK(m, m_RawImageData);

  //specify number of bins / bounds
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

    //define 2d histogram generator
    typedef itk::Statistics::ImageToHistogramFilter<ImageProcessing::UInt8SliceType> HistogramGenerator2D;
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

    //loop over slices
    for(int i=0; i<dims[2]; i++)
    {
      //wrap output buffer as image
      ImageProcessing::UInt8ImageType::Pointer outputImage=ITKUtilities::Dream3DtoITK(m, m_ProcessedImageData);

      //get slice
      ImageProcessing::UInt8SliceType::Pointer slice = ITKUtilities::ExtractSlice<ImageProcessing::UInt8PixelType>(inputImage, ImageProcessing::ZSlice, i);

      //find histogram
      histogramFilter2D->SetInput( slice );
      histogramFilter2D->Update();
      const HistogramGenerator::HistogramType * histogram = histogramFilter2D->GetOutput();

      //calculate threshold level
      calculator->SetInput(histogram);
      calculator->Update();
      const uint8_t thresholdValue = calculator->GetThreshold();

      //threshold
      typedef itk::BinaryThresholdImageFilter <ImageProcessing::UInt8SliceType, ImageProcessing::UInt8SliceType> BinaryThresholdImageFilterType;
      BinaryThresholdImageFilterType::Pointer thresholdFilter = BinaryThresholdImageFilterType::New();
      thresholdFilter->SetInput(slice);
      thresholdFilter->SetLowerThreshold(thresholdValue);
      thresholdFilter->SetUpperThreshold(255);
      thresholdFilter->SetInsideValue(255);
      thresholdFilter->SetOutsideValue(0);
      thresholdFilter->Update();

      //copy back into volume

      ITKUtilities::SetSlice<ImageProcessing::UInt8PixelType>(outputImage, thresholdFilter->GetOutput(), ImageProcessing::ZSlice, i);

    }
  }
  else
  {
    //find histogram
    histogramFilter->SetInput( inputImage );
    histogramFilter->Update();
    const HistogramGenerator::HistogramType * histogram = histogramFilter->GetOutput();

    //calculate threshold level
    calculator->SetInput(histogram);
    calculator->Update();
    const uint8_t thresholdValue = calculator->GetThreshold();

    //threshold
    typedef itk::BinaryThresholdImageFilter <ImageProcessing::UInt8ImageType, ImageProcessing::UInt8ImageType> BinaryThresholdImageFilterType;
    BinaryThresholdImageFilterType::Pointer thresholdFilter = BinaryThresholdImageFilterType::New();
    thresholdFilter->SetInput(inputImage);
    thresholdFilter->SetLowerThreshold(thresholdValue);
    thresholdFilter->SetUpperThreshold(255);
    thresholdFilter->SetInsideValue(255);
    thresholdFilter->SetOutsideValue(0);
    thresholdFilter->GetOutput()->GetPixelContainer()->SetImportPointer(m_ProcessedImageData, totalPoints, false);
    thresholdFilter->Update();
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
