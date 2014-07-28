/*
 * Your License or Copyright Information can go here
 */
#if (_MSC_VER)
#define _SCL_SECURE_NO_WARNINGS
#endif

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
      bool operator!=( const Gamma& ) const
      {
        return false;
      }
      bool operator==( const Gamma& other ) const
      {
        return !(*this != other);
      }

      inline TPixel operator()(const TPixel& A, const TPixel& B) const
      {
        const double dA = static_cast< double >( A ) / std::numeric_limits<TPixel>::max();
        return static_cast< TPixel >( double(pow(dA, double(B))) * std::numeric_limits<TPixel>::max() );
      }
  };

  //custom functor to bring value within limits and round (without this functor itk add filter on an 8bit image 255+10->9)
  template< class TInput, class TOutput> class LimitsRound
  {
    public:
      LimitsRound() {};
      ~LimitsRound() {};
      bool operator!=( const LimitsRound& ) const
      {
        return false;
      }
      bool operator==( const LimitsRound& other ) const
      {
        return !(*this != other);
      }

      inline TOutput operator()(const TInput& A) const
      {
        const double dA = static_cast< double >( A );

        if(dA > std::numeric_limits<TOutput>::max())
        {
          return std::numeric_limits<TOutput>::max();
        }
        else if(dA < std::numeric_limits<TOutput>::min())
        {
          return std::numeric_limits<TOutput>::min();
        }

        //round if needed
        if(std::numeric_limits<TOutput>::is_integer && !std::numeric_limits<TInput>::is_integer)
        {
          if (dA >= floor(dA) + 0.5) { return static_cast< TOutput >(ceil(dA)); }
          else { return static_cast< TOutput >(floor(dA)); }
        }

        return static_cast< TOutput >( dA );
      }
  };
}

//// Setup some typedef 's for the ITKUtilities class to shorten up our code
typedef ITKUtilities<ImageProcessing::DefaultPixelType>    ITKUtilitiesType;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageMath::ImageMath() :
  AbstractFilter(),
  m_SelectedCellArrayPath("", "", ""),
  m_NewCellArrayName(""),
  m_SaveAsNewArray(true),
  m_Operator(0),
  m_Value(1),
  m_SelectedCellArrayArrayName(""),
  m_SelectedCellArray(NULL),
  m_NewCellArray(NULL)
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
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Array to Operate On", "SelectedCellArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedCellArrayPath(), false, ""));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Operator");
    parameter->setPropertyName("Operator");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    QVector<QString> choices;
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
    parameters.push_back(parameter);
  }
  parameters.push_back(FilterParameter::New("Value", "Value", FilterParameterWidgetType::DoubleWidget, getValue(), false, ""));
  QStringList linkedProps;
  linkedProps << "NewCellArrayName";
  parameters.push_back(FilterParameter::NewConditional("Save As New Array", "SaveAsNewArray", FilterParameterWidgetType::LinkedBooleanWidget, getSaveAsNewArray(), false, linkedProps));
  parameters.push_back(FilterParameter::New("Created Array Name", "NewCellArrayName", FilterParameterWidgetType::StringWidget, getNewCellArrayName(), false, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageMath::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayPath( reader->readDataArrayPath( "SelectedCellArrayPath", getSelectedCellArrayPath() ) );
  setOperator( reader->readValue( "Operator", getOperator() ) );
  setValue( reader->readValue( "Value", getValue() ) );
  setSaveAsNewArray( reader->readValue( "SaveAsNewArray", getSaveAsNewArray() ) );
  setNewCellArrayName( reader->readString( "NewCellArrayName", getNewCellArrayName() ) );
  reader->closeFilterGroup();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImageMath::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedCellArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(Operator)
  DREAM3D_FILTER_WRITE_PARAMETER(Value)
  DREAM3D_FILTER_WRITE_PARAMETER(SaveAsNewArray)
  DREAM3D_FILTER_WRITE_PARAMETER(NewCellArrayName)
  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageMath::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  QVector<size_t> dims(1, 1);
  m_SelectedCellArrayPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<ImageProcessing::DefaultPixelType>, AbstractFilter>(this, getSelectedCellArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SelectedCellArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SelectedCellArray = m_SelectedCellArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_SaveAsNewArray == false) { m_NewCellArrayName = "thisIsATempName"; }
  tempPath.update(getSelectedCellArrayPath().getDataContainerName(), getSelectedCellArrayPath().getAttributeMatrixName(), getNewCellArrayName() );
  m_NewCellArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<ImageProcessing::DefaultPixelType>, AbstractFilter, ImageProcessing::DefaultPixelType>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NewCellArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NewCellArray = m_NewCellArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageMath::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageMath::execute()
{
  //int err = 0;
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getSelectedCellArrayPath().getDataContainerName());
  QString attrMatName = getSelectedCellArrayPath().getAttributeMatrixName();

  //wrap m_RawImageData as itk::image
  ImageProcessing::DefaultImageType::Pointer inputImage = ITKUtilitiesType::Dream3DtoITK(m, attrMatName, m_SelectedCellArray);

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
      ITKUtilitiesType::SetITKFilterOutput(limitsRound->GetOutput(), m_NewCellArrayPtr.lock());
      limitsRound->Update();
    }
    break;

    case 1://subtract
    {
      SubtractType::Pointer subtract = SubtractType::New();
      subtract->SetInput1(inputImage);
      subtract->SetConstant2(m_Value);
      limitsRound->SetInput(subtract->GetOutput());
      ITKUtilitiesType::SetITKFilterOutput(limitsRound->GetOutput(), m_NewCellArrayPtr.lock());
      limitsRound->Update();
    }
    break;

    case 2://multiply
    {
      MultiplyType::Pointer multiply = MultiplyType::New();
      multiply->SetInput1(inputImage);
      multiply->SetConstant2(m_Value);
      limitsRound->SetInput(multiply->GetOutput());
      ITKUtilitiesType::SetITKFilterOutput(limitsRound->GetOutput(), m_NewCellArrayPtr.lock());
      limitsRound->Update();
    }
    break;

    case 3://divide
    {
      DivideType::Pointer divide = DivideType::New();
      divide->SetInput1(inputImage);
      divide->SetConstant2(m_Value);
      limitsRound->SetInput(divide->GetOutput());
      ITKUtilitiesType::SetITKFilterOutput(limitsRound->GetOutput(), m_NewCellArrayPtr.lock());
      limitsRound->Update();
    }
    break;

    case 4://min
    {
      MinType::Pointer minimum = MinType::New();
      minimum->SetInput1(inputImage);
      minimum->SetConstant2(m_Value);
      limitsRound->SetInput(minimum->GetOutput());
      ITKUtilitiesType::SetITKFilterOutput(limitsRound->GetOutput(), m_NewCellArrayPtr.lock());
      limitsRound->Update();
    }
    break;

    case 5://max
    {
      MaxType::Pointer maximum = MaxType::New();
      maximum->SetInput1(inputImage);
      maximum->SetConstant2(m_Value);
      limitsRound->SetInput(maximum->GetOutput());
      ITKUtilitiesType::SetITKFilterOutput(limitsRound->GetOutput(), m_NewCellArrayPtr.lock());
      limitsRound->Update();
    }
    break;

    case 6://gamma
    {
      GammaType::Pointer gamma = GammaType::New();
      gamma->SetInput1(inputImage);
      gamma->SetConstant2(m_Value);
      limitsRound->SetInput(gamma->GetOutput());
      ITKUtilitiesType::SetITKFilterOutput(limitsRound->GetOutput(), m_NewCellArrayPtr.lock());
      limitsRound->Update();
    }
    break;

    case 7://log
    {
      LogType::Pointer logfilter = LogType::New();
      logfilter->SetInput(inputImage);
      limitsRound->SetInput(logfilter->GetOutput());
      ITKUtilitiesType::SetITKFilterOutput(limitsRound->GetOutput(), m_NewCellArrayPtr.lock());
      limitsRound->Update();
    }
    break;

    case 8://exp
    {
      ExpType::Pointer expfilter = ExpType::New();
      expfilter->SetInput(inputImage);
      limitsRound->SetInput(expfilter->GetOutput());
      ITKUtilitiesType::SetITKFilterOutput(limitsRound->GetOutput(), m_NewCellArrayPtr.lock());
      limitsRound->Update();
    }
    break;

    case 9://square
    {
      SquareType::Pointer square = SquareType::New();
      square->SetInput(inputImage);
      limitsRound->SetInput(square->GetOutput());
      ITKUtilitiesType::SetITKFilterOutput(limitsRound->GetOutput(), m_NewCellArrayPtr.lock());
      limitsRound->Update();
    }
    break;

    case 10://squareroot
    {
      SqrtType::Pointer sqrtfilter = SqrtType::New();
      sqrtfilter->SetInput(inputImage);
      limitsRound->SetInput(sqrtfilter->GetOutput());
      ITKUtilitiesType::SetITKFilterOutput(limitsRound->GetOutput(), m_NewCellArrayPtr.lock());
      limitsRound->Update();
    }
    break;

    case 11://invert
    {
      InvertType::Pointer invert = InvertType::New();
      invert->SetInput(inputImage);
      ITKUtilitiesType::SetITKFilterOutput(invert->GetOutput(), m_NewCellArrayPtr.lock());
      invert->Update();
    }
    break;
  }

  //array name changing/cleanup
  if(m_SaveAsNewArray == false)
  {
    AttributeMatrix::Pointer attrMat = m->getAttributeMatrix(m_SelectedCellArrayPath.getAttributeMatrixName());
    attrMat->removeAttributeArray(m_SelectedCellArrayPath.getDataArrayName());
    attrMat->renameAttributeArray(m_NewCellArrayName, m_SelectedCellArrayPath.getDataArrayName());
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ImageMath::newFilterInstance(bool copyFilterParameters)
{
  ImageMath::Pointer filter = ImageMath::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImageMath::getCompiledLibraryName()
{return ImageProcessing::ImageProcessingBaseName;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImageMath::getGroupName()
{return "ImageProcessing";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImageMath::getSubGroupName()
{return "Misc";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImageMath::getHumanLabel()
{return "Image Math";}

