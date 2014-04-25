/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ImageMath.h"

#include "ITKUtilities.h"
#include "itkAddImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include "itkDivideImageFilter.h"
#include "itkMinimumImageFilter.h"
#include "itkMaximumImageFilter.h"
#include "itkLogImageFilter.h"
#include "itkExpImageFilter.h"
#include "itkSquareImageFilter.h"
#include "itkSqrtImageFilter.h"
#include "itkInvertIntensityImageFilter.h"
#include "itkUnaryFunctorImageFilter.h"
#include "itkBinaryFunctorImageFilter.h"

#include <limits>

namespace Functor
{
  //gamma functor (doesn't seem to be implemented in itk)
  template< class TPixel > class Gamma
  {
  public:
    Gamma() {};
    ~Gamma() {};
    bool operator!=( const Gamma & ) const
    {
      return false;
    }
    bool operator==( const Gamma & other ) const
    {
      return !(*this != other);
    }

    inline TPixel operator()(const TPixel & A, const TPixel & B) const
    {
      const double dA = static_cast< double >( A )/std::numeric_limits<TPixel>::max();
      return static_cast< TPixel >( pow(dA, B)*std::numeric_limits<TPixel>::max() );
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

      if(dA>std::numeric_limits<TInput>::max())
      {
        return std::numeric_limits<TInput>::max();
      }
      else if(dA<std::numeric_limits<TInput>::min())
      {
        return std::numeric_limits<TInput>::min();
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
ImageMath::ImageMath() :
AbstractFilter(),
m_RawImageDataArrayName("RawImageData"),
m_ProcessedImageDataArrayName("ProcessedData"),
m_SelectedCellArrayName(""),
m_NewCellArrayName("ProcessedArray"),
m_OverwriteArray(true),
m_Operator(0),
m_Value(0),
m_RawImageData(NULL),
m_ProcessedImageData(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageMath::~ImageMath()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageMath::setupFilterParameters()
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
    choices.push_back("Min");
    choices.push_back("Max");
    choices.push_back("Gamma");
    choices.push_back("Log");
    choices.push_back("Exp");
    choices.push_back("Square");
    choices.push_back("Square Root");
    choices.push_back("Invert");
    parameter->setChoices(choices);
    options.push_back(parameter);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Value");
    option->setPropertyName("Value");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    //option->setUnits("units");
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
void ImageMath::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayName( reader->readValue( "SelectedCellArrayName", getSelectedCellArrayName() ) );
  setOperator( reader->readValue( "Operator", getOperator() ) );
  setValue( reader->readValue( "Value", getValue() ) );
  setNewCellArrayName( reader->readValue( "NewCellArrayName", getNewCellArrayName() ) );
  setOverwriteArray( reader->readValue( "OverwriteArray", getOverwriteArray() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImageMath::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)

{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedCellArrayName", getSelectedCellArrayName() );
  writer->writeValue("Operator", getOperator() );
  writer->writeValue("Value", getValue() );
  writer->writeValue("NewCellArrayName", getNewCellArrayName() );
  writer->writeValue("OverwriteArray", getOverwriteArray() );
  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageMath::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
void ImageMath::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageMath::execute()
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

  //define filter types
  typedef itk::AddImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::FloatImageType, ImageProcessing::FloatImageType> AddType;
  typedef itk::SubtractImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::FloatImageType, ImageProcessing::FloatImageType> SubtractType;
  typedef itk::MultiplyImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::FloatImageType, ImageProcessing::FloatImageType> MultiplyType;
  typedef itk::DivideImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::FloatImageType, ImageProcessing::FloatImageType> DivideType;
  typedef itk::MinimumImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::FloatImageType, ImageProcessing::FloatImageType> MinType;
  typedef itk::MaximumImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::FloatImageType, ImageProcessing::FloatImageType> MaxType;
  typedef itk::BinaryFunctorImageFilter< ImageProcessing::DefaultImageType, ImageProcessing::FloatImageType, ImageProcessing::FloatImageType, Functor::Gamma<ImageProcessing::FloatPixelType> > GammaType;
  typedef itk::LogImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::FloatImageType> LogType;
  typedef itk::ExpImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::FloatImageType> ExpType;
  typedef itk::SquareImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::FloatImageType> SquareType;
  typedef itk::SqrtImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::FloatImageType> SqrtType;
  typedef itk::InvertIntensityImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType> InvertType;

  //set up filter to cap image range + round
  typedef itk::UnaryFunctorImageFilter< ImageProcessing::FloatImageType, ImageProcessing::DefaultImageType, Functor::LimitsRound<ImageProcessing::FloatPixelType, ImageProcessing::DefaultPixelType> > LimitsRoundType;
  LimitsRoundType::Pointer limitsRound = LimitsRoundType::New();

  //apply selected operation
  switch(m_Operator)
  {
    case 0://add
      {
        AddType::Pointer add = AddType::New();
        add->SetInput1(inputImage);
        add->SetConstant2(m_Value);
        limitsRound->SetInput(add->GetOutput());
        ITKUtilities::SetITKOutput(limitsRound->GetOutput(), m_ProcessedImageData, totalPoints);
        limitsRound->Update();
      }
      break;

    case 1://subtract
      {
        SubtractType::Pointer subtract = SubtractType::New();
        subtract->SetInput1(inputImage);
        subtract->SetConstant2(m_Value);
        limitsRound->SetInput(subtract->GetOutput());
        ITKUtilities::SetITKOutput(limitsRound->GetOutput(), m_ProcessedImageData, totalPoints);
        limitsRound->Update();
      }
      break;

    case 2://multiply
      {
        MultiplyType::Pointer multiply = MultiplyType::New();
        multiply->SetInput1(inputImage);
        multiply->SetConstant2(m_Value);
        limitsRound->SetInput(multiply->GetOutput());
        ITKUtilities::SetITKOutput(limitsRound->GetOutput(), m_ProcessedImageData, totalPoints);
        limitsRound->Update();
      }
      break;

    case 3://divide
      {
        DivideType::Pointer divide = DivideType::New();
        divide->SetInput1(inputImage);
        divide->SetConstant2(m_Value);
        limitsRound->SetInput(divide->GetOutput());
        ITKUtilities::SetITKOutput(limitsRound->GetOutput(), m_ProcessedImageData, totalPoints);
        limitsRound->Update();
      }
      break;

    case 4://min
      {
        MinType::Pointer minimum = MinType::New();
        minimum->SetInput1(inputImage);
        minimum->SetConstant2(m_Value);
        limitsRound->SetInput(minimum->GetOutput());
        ITKUtilities::SetITKOutput(limitsRound->GetOutput(), m_ProcessedImageData, totalPoints);
        limitsRound->Update();
      }
      break;

    case 5://max
      {
        MaxType::Pointer maximum = MaxType::New();
        maximum->SetInput1(inputImage);
        maximum->SetConstant2(m_Value);
        limitsRound->SetInput(maximum->GetOutput());
        ITKUtilities::SetITKOutput(limitsRound->GetOutput(), m_ProcessedImageData, totalPoints);
        limitsRound->Update();
      }
      break;

    case 6://gamma
      {
        GammaType::Pointer gamma = GammaType::New();
        gamma->SetInput1(inputImage);
        gamma->SetConstant2(m_Value);
        limitsRound->SetInput(gamma->GetOutput());
        ITKUtilities::SetITKOutput(limitsRound->GetOutput(), m_ProcessedImageData, totalPoints);
        limitsRound->Update();
      }
      break;

    case 7://log
      {
        LogType::Pointer logfilter = LogType::New();
        logfilter->SetInput(inputImage);
        limitsRound->SetInput(logfilter->GetOutput());
        ITKUtilities::SetITKOutput(limitsRound->GetOutput(), m_ProcessedImageData, totalPoints);
        limitsRound->Update();
      }
      break;

    case 8://exp
      {
        ExpType::Pointer expfilter = ExpType::New();
        expfilter->SetInput(inputImage);
        limitsRound->SetInput(expfilter->GetOutput());
        ITKUtilities::SetITKOutput(limitsRound->GetOutput(), m_ProcessedImageData, totalPoints);
        limitsRound->Update();
      }
      break;

    case 9://square
      {
        SquareType::Pointer square = SquareType::New();
        square->SetInput(inputImage);
        limitsRound->SetInput(square->GetOutput());
        ITKUtilities::SetITKOutput(limitsRound->GetOutput(), m_ProcessedImageData, totalPoints);
        limitsRound->Update();
      }
      break;

    case 10://squareroot
      {
        SqrtType::Pointer sqrtfilter = SqrtType::New();
        sqrtfilter->SetInput(inputImage);
        limitsRound->SetInput(sqrtfilter->GetOutput());
        ITKUtilities::SetITKOutput(limitsRound->GetOutput(), m_ProcessedImageData, totalPoints);
        limitsRound->Update();
      }
      break;

    case 11://invert
      {
        InvertType::Pointer invert = InvertType::New();
        invert->SetInput(inputImage);
        ITKUtilities::SetITKOutput(invert->GetOutput(), m_ProcessedImageData, totalPoints);
        invert->Update();
      }
      break;
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
