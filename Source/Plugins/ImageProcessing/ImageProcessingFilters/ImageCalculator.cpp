/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ImageCalculator.h"

#include "ITKUtilities.h"
#include "itkAddImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include "itkDivideImageFilter.h"
#include "itkAndImageFilter.h"
#include "itkOrImageFilter.h"
#include "itkXorImageFilter.h"
#include "itkMinimumImageFilter.h"
#include "itkMaximumImageFilter.h"
#include "itkBinaryFunctorImageFilter.h"
#include "itkAbsoluteValueDifferenceImageFilter.h"
#include "itkUnaryFunctorImageFilter.h"

#include <limits>

namespace Functor
{
  //mean functor (doesn't seem to be currently implemented in itk)
  template< class TPixel> class Mean
  {
  public:
    Mean() {}
    ~Mean() {}
    bool operator!=(const Mean &) const
    {
      return false;
    }
    bool operator==(const Mean & other) const
    {
      return !( *this != other );
    }

    inline TPixel operator()(const TPixel & A, const TPixel & B) const
    {
      const double dA = static_cast< double >( A );
      const double dB = static_cast< double >( B );
      const double sum = dA + dB;

      return static_cast< TPixel >( sum / 2 );
    }
  };

  //custom functor to bring value within limits and round (without this functor itk add filter on an 8bit image 255+10->9)
  template< class TInput, class TOutput> class LimitsRound
  {
  public:
    LimitsRound() {};
    ~LimitsRound() {};
    bool operator!=( const LimitsRound & ) const
    {
      return false;
    }
    bool operator==( const LimitsRound & other ) const
    {
      return !(*this != other);
    }

    inline TOutput operator()(const TInput & A) const
    {
      const double dA = static_cast< double >( A );

      if(dA>std::numeric_limits<TOutput>::max())
      {
        return std::numeric_limits<TOutput>::max();
      }
      else if(dA<std::numeric_limits<TOutput>::min())
      {
        return std::numeric_limits<TOutput>::min();
      }

      //round if needed
      if(std::numeric_limits<TOutput>::is_integer && !std::numeric_limits<TInput>::is_integer)
      {
        if (dA >= floor(dA)+0.5) return static_cast< TOutput >(ceil(dA));
        else return static_cast< TOutput >(floor(dA));
      }

      return static_cast< TOutput >( dA );
    }
  };

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageCalculator::ImageCalculator() :
AbstractFilter(),
m_RawImageData1ArrayName("RawImageData1"),
m_RawImageData2ArrayName("RawImageData2"),
m_ProcessedImageDataArrayName("ProcessedData"),
m_SelectedCellArrayName1(""),
m_SelectedCellArrayName2(""),
m_Operator(0),
m_NewCellArrayName("ProcessedArray"),
m_RawImageData1(NULL),
m_RawImageData2(NULL),
m_ProcessedImageData(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageCalculator::~ImageCalculator()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageCalculator::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> options;
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("First Array to Process");
    parameter->setPropertyName("SelectedCellArrayName1");
    parameter->setWidgetType(FilterParameter::VoxelCellArrayNameSelectionWidget);
    parameter->setValueType("string");
    parameter->setUnits("");
    options.push_back(parameter);
  }
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Operator");
    parameter->setPropertyName("Operator");
    parameter->setWidgetType(FilterParameter::ChoiceWidget);
    parameter->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back("Add");
    choices.push_back("Subtract");
    choices.push_back("Multiply");
    choices.push_back("Divide");
    choices.push_back("AND");
    choices.push_back("OR");
    choices.push_back("XOR");
    choices.push_back("Min");
    choices.push_back("Max");
    choices.push_back("Mean");
    choices.push_back("Difference");
    parameter->setChoices(choices);
    options.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Second Array to Process");
    parameter->setPropertyName("SelectedCellArrayName2");
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
void ImageCalculator::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayName1( reader->readValue( "SelectedCellArrayName1", getSelectedCellArrayName1() ) );
  setOperator( reader->readValue( "Operator", getOperator() ) );
  setSelectedCellArrayName2( reader->readValue( "SelectedCellArrayName2", getSelectedCellArrayName2() ) );
  setNewCellArrayName( reader->readValue( "NewCellArrayName", getNewCellArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImageCalculator::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)

{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedCellArrayName1", getSelectedCellArrayName1() );
  writer->writeValue("Operator", getOperator() );
  writer->writeValue("SelectedCellArrayName2", getSelectedCellArrayName2() );
  writer->writeValue("NewCellArrayName", getNewCellArrayName() );
  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageCalculator::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  if(m_SelectedCellArrayName1.empty() == true)
  {
    setErrorCondition(-11000);
    ss << "A first array from the Voxel Data Container must be selected.";
    addErrorMessage(getHumanLabel(),ss.str(),getErrorCondition());
  }
  else if(m_SelectedCellArrayName2.empty() == true)
  {
    setErrorCondition(-11000);
    ss << "A second array from the Voxel Data Container must be selected.";
    addErrorMessage(getHumanLabel(),ss.str(),getErrorCondition());
  }
  else
  {
    m_RawImageData1ArrayName=m_SelectedCellArrayName1;
    m_RawImageData2ArrayName=m_SelectedCellArrayName2;
    GET_PREREQ_DATA(m, DREAM3D, CellData, RawImageData1, ss, -300, ImageProcessing::DefaultPixelType, ImageProcessing::DefaultArrayType, voxels, 1)
    GET_PREREQ_DATA(m, DREAM3D, CellData, RawImageData2, ss, -300, ImageProcessing::DefaultPixelType, ImageProcessing::DefaultArrayType, voxels, 1)

    CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, ProcessedImageData, ss, ImageProcessing::DefaultPixelType, ImageProcessing::DefaultArrayType, 0, voxels, 1)
    m->renameCellData(m_ProcessedImageDataArrayName, m_NewCellArrayName);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageCalculator::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageCalculator::execute()
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
  ImageProcessing::DefaultImageType::Pointer inputImage1=ITKUtilities::Dream3DtoITK(m, m_RawImageData1);
  ImageProcessing::DefaultImageType::Pointer inputImage2=ITKUtilities::Dream3DtoITK(m, m_RawImageData2);

  //define filters
  typedef itk::AddImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType, ImageProcessing::FloatImageType> AddType;//
  typedef itk::SubtractImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType, ImageProcessing::FloatImageType> SubtractType;//
  typedef itk::MultiplyImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType, ImageProcessing::FloatImageType> MultiplyType;//
  typedef itk::DivideImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType, ImageProcessing::FloatImageType> DivideType;//
  typedef itk::AndImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType> AndType;
  typedef itk::OrImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType> OrType;
  typedef itk::XorImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType> XorType;
  typedef itk::MinimumImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType> MinType;
  typedef itk::MaximumImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType> MaxType;
  typedef itk::BinaryFunctorImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType, ImageProcessing::FloatImageType, Functor::Mean<ImageProcessing::DefaultPixelType>> MeanType;
  typedef itk::AbsoluteValueDifferenceImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType, ImageProcessing::FloatImageType> DifferenceType;

  //set up filters to cap image ranges
  typedef itk::UnaryFunctorImageFilter< ImageProcessing::FloatImageType, ImageProcessing::DefaultImageType, Functor::LimitsRound<ImageProcessing::FloatPixelType, ImageProcessing::DefaultPixelType> > LimitsRoundType;
  LimitsRoundType::Pointer limitsRound = LimitsRoundType::New();

  //set up and run selected filter
  switch(m_Operator)
  {
    case 0://add
      {
        AddType::Pointer add = AddType::New();
        add->SetInput1(inputImage1);
        add->SetInput2(inputImage2);
        limitsRound->SetInput(add->GetOutput());
        ITKUtilities::SetITKOutput(limitsRound->GetOutput(), m_ProcessedImageData, totalPoints);
        limitsRound->Update();
      }
      break;

    case 1://subtract
      {
        SubtractType::Pointer subtract = SubtractType::New();
        subtract->SetInput1(inputImage1);
        subtract->SetInput2(inputImage2);
        limitsRound->SetInput(subtract->GetOutput());
        ITKUtilities::SetITKOutput(limitsRound->GetOutput(), m_ProcessedImageData, totalPoints);
        limitsRound->Update();
      }
      break;

    case 2://multiply
      {
        MultiplyType::Pointer multiply = MultiplyType::New();
        multiply->SetInput1(inputImage1);
        multiply->SetInput2(inputImage2);
        limitsRound->SetInput(multiply->GetOutput());
        ITKUtilities::SetITKOutput(limitsRound->GetOutput(), m_ProcessedImageData, totalPoints);
        limitsRound->Update();
      }
      break;

    case 3://divide
      {
        DivideType::Pointer divide = DivideType::New();
        divide->SetInput1(inputImage1);
        divide->SetInput2(inputImage2);
        limitsRound->SetInput(divide->GetOutput());
        ITKUtilities::SetITKOutput(limitsRound->GetOutput(), m_ProcessedImageData, totalPoints);
        limitsRound->Update();
      }
      break;

    case 4://and
      {
        AndType::Pointer andfilter = AndType::New();
        andfilter->SetInput1(inputImage1);
        andfilter->SetInput2(inputImage2);
        ITKUtilities::SetITKOutput(andfilter->GetOutput(), m_ProcessedImageData, totalPoints);
        andfilter->Update();
      }
      break;

    case 5://or
      {
        OrType::Pointer orfilter = OrType::New();
        orfilter->SetInput1(inputImage1);
        orfilter->SetInput2(inputImage2);
        ITKUtilities::SetITKOutput(orfilter->GetOutput(), m_ProcessedImageData, totalPoints);
        orfilter->Update();
      }
      break;

    case 6://xor
      {
        XorType::Pointer xorfilter = XorType::New();
        xorfilter->SetInput1(inputImage1);
        xorfilter->SetInput2(inputImage2);
        ITKUtilities::SetITKOutput(xorfilter->GetOutput(), m_ProcessedImageData, totalPoints);
        xorfilter->Update();
      }
      break;

    case 7://min
      {
        MinType::Pointer minimum = MinType::New();
        minimum->SetInput1(inputImage1);
        minimum->SetInput2(inputImage2);
        ITKUtilities::SetITKOutput(minimum->GetOutput(), m_ProcessedImageData, totalPoints);
        minimum->Update();
      }
      break;

    case 8://max
      {
        MaxType::Pointer maximum = MaxType::New();
        maximum->SetInput1(inputImage1);
        maximum->SetInput2(inputImage2);
        ITKUtilities::SetITKOutput(maximum->GetOutput(), m_ProcessedImageData, totalPoints);
        maximum->Update();
      }
      break;

    case 9://mean
      {
        MeanType::Pointer mean = MeanType::New();
        mean->SetInput1(inputImage1);
        mean->SetInput2(inputImage2);
        limitsRound->SetInput(mean->GetOutput());
        ITKUtilities::SetITKOutput(limitsRound->GetOutput(), m_ProcessedImageData, totalPoints);
        limitsRound->Update();
      }
      break;

    case 10://difference
      {
        DifferenceType::Pointer difference = DifferenceType::New();
        difference->SetInput1(inputImage1);
        difference->SetInput2(inputImage2);
        limitsRound->SetInput(difference->GetOutput());
        ITKUtilities::SetITKOutput(limitsRound->GetOutput(), m_ProcessedImageData, totalPoints);
        limitsRound->Update();
      }
      break;
  }

  /* Let the GUI know we are done with this filter */
   notifyStatusMessage("Complete");
}
