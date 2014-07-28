/*
 * Your License or Copyright Information can go here
 */
#if (_MSC_VER)
#define _SCL_SECURE_NO_WARNINGS
#endif

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
      bool operator!=(const Mean&) const
      {
        return false;
      }
      bool operator==(const Mean& other) const
      {
        return !( *this != other );
      }

      inline TPixel operator()(const TPixel& A, const TPixel& B) const
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
ImageCalculator::ImageCalculator() :
  AbstractFilter(),
  m_SelectedCellArrayPath1("", "", ""),
  m_SelectedCellArrayPath2("", "", ""),
  m_NewCellArrayName(""),
  m_Operator(0),
  m_SelectedCellArray1ArrayName(""),
  m_SelectedCellArray1(NULL),
  m_SelectedCellArray2ArrayName(""),
  m_SelectedCellArray2(NULL),
  m_NewCellArray(NULL)
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
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("First Array to Process", "SelectedCellArrayPath1", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedCellArrayPath1(), false, ""));
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
    choices.push_back("AND");
    choices.push_back("OR");
    choices.push_back("XOR");
    choices.push_back("Min");
    choices.push_back("Max");
    choices.push_back("Mean");
    choices.push_back("Difference");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  parameters.push_back(FilterParameter::New("Second Array to Process", "SelectedCellArrayPath2", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedCellArrayPath2(), false, ""));
  parameters.push_back(FilterParameter::New("Created Array Name", "NewCellArrayName", FilterParameterWidgetType::StringWidget, getNewCellArrayName(), false, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageCalculator::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayPath1( reader->readDataArrayPath( "SelectedCellArrayPath1", getSelectedCellArrayPath1() ) );
  setOperator( reader->readValue( "Operator", getOperator() ) );
  setSelectedCellArrayPath2( reader->readDataArrayPath( "SelectedCellArrayPath2", getSelectedCellArrayPath2() ) );
  setNewCellArrayName( reader->readString( "NewCellArrayName", getNewCellArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImageCalculator::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedCellArrayPath1)
  DREAM3D_FILTER_WRITE_PARAMETER(Operator)
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedCellArrayPath2)
  DREAM3D_FILTER_WRITE_PARAMETER(NewCellArrayName)
  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageCalculator::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  QVector<size_t> dims(1, 1);
  m_SelectedCellArray1Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<ImageProcessing::DefaultPixelType>, AbstractFilter>(this, getSelectedCellArrayPath1(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SelectedCellArray1Ptr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SelectedCellArray1 = m_SelectedCellArray1Ptr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_SelectedCellArray2Ptr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<ImageProcessing::DefaultPixelType>, AbstractFilter>(this, getSelectedCellArrayPath2(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SelectedCellArray2Ptr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SelectedCellArray2 = m_SelectedCellArray2Ptr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getSelectedCellArrayPath1().getDataContainerName(), getSelectedCellArrayPath1().getAttributeMatrixName(), getNewCellArrayName() );
  m_NewCellArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<ImageProcessing::DefaultPixelType>, AbstractFilter, ImageProcessing::DefaultPixelType>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NewCellArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NewCellArray = m_NewCellArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageCalculator::preflight()
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
void ImageCalculator::execute()
{
  //int err = 0;
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getSelectedCellArrayPath1().getDataContainerName());
  QString attrMatName = getSelectedCellArrayPath1().getAttributeMatrixName();

  //wrap m_RawImageData as itk::image
  ImageProcessing::DefaultImageType::Pointer inputImage1 = ITKUtilitiesType::Dream3DtoITK(m, attrMatName, m_SelectedCellArray1);
  ImageProcessing::DefaultImageType::Pointer inputImage2 = ITKUtilitiesType::Dream3DtoITK(m, attrMatName, m_SelectedCellArray2);

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
  typedef itk::BinaryFunctorImageFilter<ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType, ImageProcessing::FloatImageType, Functor::Mean<ImageProcessing::DefaultPixelType> > MeanType;
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
      ITKUtilitiesType::SetITKFilterOutput(limitsRound->GetOutput(), m_NewCellArrayPtr.lock());
      limitsRound->Update();
    }
    break;

    case 1://subtract
    {
      SubtractType::Pointer subtract = SubtractType::New();
      subtract->SetInput1(inputImage1);
      subtract->SetInput2(inputImage2);
      limitsRound->SetInput(subtract->GetOutput());
      ITKUtilitiesType::SetITKFilterOutput(limitsRound->GetOutput(), m_NewCellArrayPtr.lock());
      limitsRound->Update();
    }
    break;

    case 2://multiply
    {
      MultiplyType::Pointer multiply = MultiplyType::New();
      multiply->SetInput1(inputImage1);
      multiply->SetInput2(inputImage2);
      limitsRound->SetInput(multiply->GetOutput());
      ITKUtilitiesType::SetITKFilterOutput(limitsRound->GetOutput(), m_NewCellArrayPtr.lock());
      limitsRound->Update();
    }
    break;

    case 3://divide
    {
      DivideType::Pointer divide = DivideType::New();
      divide->SetInput1(inputImage1);
      divide->SetInput2(inputImage2);
      limitsRound->SetInput(divide->GetOutput());
      ITKUtilitiesType::SetITKFilterOutput(limitsRound->GetOutput(), m_NewCellArrayPtr.lock());
      limitsRound->Update();
    }
    break;

    case 4://and
    {
      AndType::Pointer andfilter = AndType::New();
      andfilter->SetInput1(inputImage1);
      andfilter->SetInput2(inputImage2);
      ITKUtilitiesType::SetITKFilterOutput(andfilter->GetOutput(), m_NewCellArrayPtr.lock());
      andfilter->Update();
    }
    break;

    case 5://or
    {
      OrType::Pointer orfilter = OrType::New();
      orfilter->SetInput1(inputImage1);
      orfilter->SetInput2(inputImage2);
      ITKUtilitiesType::SetITKFilterOutput(orfilter->GetOutput(), m_NewCellArrayPtr.lock());
      orfilter->Update();
    }
    break;

    case 6://xor
    {
      XorType::Pointer xorfilter = XorType::New();
      xorfilter->SetInput1(inputImage1);
      xorfilter->SetInput2(inputImage2);
      ITKUtilitiesType::SetITKFilterOutput(xorfilter->GetOutput(), m_NewCellArrayPtr.lock());
      xorfilter->Update();
    }
    break;

    case 7://min
    {
      MinType::Pointer minimum = MinType::New();
      minimum->SetInput1(inputImage1);
      minimum->SetInput2(inputImage2);
      ITKUtilitiesType::SetITKFilterOutput(minimum->GetOutput(), m_NewCellArrayPtr.lock());
      minimum->Update();
    }
    break;

    case 8://max
    {
      MaxType::Pointer maximum = MaxType::New();
      maximum->SetInput1(inputImage1);
      maximum->SetInput2(inputImage2);
      ITKUtilitiesType::SetITKFilterOutput(maximum->GetOutput(), m_NewCellArrayPtr.lock());
      maximum->Update();
    }
    break;

    case 9://mean
    {
      MeanType::Pointer mean = MeanType::New();
      mean->SetInput1(inputImage1);
      mean->SetInput2(inputImage2);
      limitsRound->SetInput(mean->GetOutput());
      ITKUtilitiesType::SetITKFilterOutput(limitsRound->GetOutput(), m_NewCellArrayPtr.lock());
      limitsRound->Update();
    }
    break;

    case 10://difference
    {
      DifferenceType::Pointer difference = DifferenceType::New();
      difference->SetInput1(inputImage1);
      difference->SetInput2(inputImage2);
      limitsRound->SetInput(difference->GetOutput());
      ITKUtilitiesType::SetITKFilterOutput(limitsRound->GetOutput(), m_NewCellArrayPtr.lock());
      limitsRound->Update();
    }
    break;
  }


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ImageCalculator::newFilterInstance(bool copyFilterParameters)
{
  /*
  * write code to optionally copy the filter parameters from the current filter into the new instance
  */
  ImageCalculator::Pointer filter = ImageCalculator::New();
  if(true == copyFilterParameters)
  {
    /* If the filter uses all the standard Filter Parameter Widgets you can probabaly get
     * away with using this method to copy the filter parameters from the current instance
     * into the new instance
     */
    copyFilterParameterInstanceVariables(filter.get());
    /* If your filter is using a lot of custom FilterParameterWidgets @see ReadH5Ebsd then you
     * may need to copy each filter parameter explicitly plus any other instance variables that
     * are needed into the new instance. Here is some example code from ReadH5Ebsd
     */
    //    DREAM3D_COPY_INSTANCEVAR(OutputFile)
    //    DREAM3D_COPY_INSTANCEVAR(ZStartIndex)
    //    DREAM3D_COPY_INSTANCEVAR(ZEndIndex)
    //    DREAM3D_COPY_INSTANCEVAR(ZResolution)
  }
  return filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImageCalculator::getCompiledLibraryName()
{return ImageProcessing::ImageProcessingBaseName;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImageCalculator::getGroupName()
{return "ImageProcessing";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImageCalculator::getSubGroupName()
{return "Misc";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImageCalculator::getHumanLabel()
{return "Image Calculator";}

