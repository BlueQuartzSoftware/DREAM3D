/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ImageRegistration.h"

#include "ITKUtilities.h"
#include "itkImageRegistrationMethod.h"
#include "itkTranslationTransform.h"
#include "itkResampleImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"

//image metrics
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkNormalizedCorrelationImageToImageMetric.h"
#include "itkMeanReciprocalSquareDifferenceImageToImageMetric.h"
#include "itkMattesMutualInformationImageToImageMetric.h"

//search optimizers
#include "itkGradientDescentOptimizer.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkLBFGSOptimizer.h"
#include "itkAmoebaOptimizer.h"
#include "itkOnePlusOneEvolutionaryOptimizer.h"
  #include "itkNormalVariateGenerator.h"

//#include "itkRescaleIntensityImageFilter.h"
//#include "itkSpatialObjectToImageFilter.h"

#include "itkCommand.h"
#include "itkSingleValuedNonLinearOptimizer.h"
class CommandIterationUpdate : public itk::Command
{
    public:
    typedef CommandIterationUpdate Self;
    typedef itk::Command Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    itkNewMacro( Self );

  protected:
    CommandIterationUpdate()
    {
      m_IterationNumber=0;
    }

  public:
    typedef itk::SingleValuedNonLinearOptimizer OptimizerType;
    typedef const OptimizerType * OptimizerPointer;
    void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

    void Execute(const itk::Object * object, const itk::EventObject & event)
    {
      OptimizerPointer optimizer = dynamic_cast< OptimizerPointer >( object );
      if( ! itk::IterationEvent().CheckEvent( &event ) )
      {
        return;
      }
    }

  private:
    unsigned long m_IterationNumber;
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageRegistration::ImageRegistration() :
AbstractFilter(),
m_RawImageDataArrayName("RawImageData"),
m_SelectedCellArrayName(""),
m_AlignmentShiftFileName(""),
m_WriteShifts(false),
m_ApplyShifts(true),
m_Metric(0),
m_Optimizer(1),
m_OptimizerParameter1(0.0f),
m_OptimizerParameter2(0.0f),
m_Iterations(200),
m_RawImageData(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageRegistration::~ImageRegistration()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageRegistration::setupFilterParameters()
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
    parameter->setHumanLabel("Similarity Metric");
    parameter->setPropertyName("Metric");
    parameter->setWidgetType(FilterParameter::ChoiceWidget);
    parameter->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back("Mean Squares");
    choices.push_back("Normalized Correlation");
    choices.push_back("Mean Reciprocal Square Difference");
    //choices.push_back("Mattes Mutual Information");
    parameter->setChoices(choices);
    options.push_back(parameter);
  }
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Optimizer");
    parameter->setPropertyName("Optimizer");
    parameter->setWidgetType(FilterParameter::ChoiceWidget);
    parameter->setValueType("unsigned int");
    std::vector<std::string> choices;
    choices.push_back("Gradient Descent");
    choices.push_back("Regular Step Gradient Descent");
    choices.push_back("LBFGS");
    choices.push_back("Amoeba");
    choices.push_back("One Plus One Evolutionary");
    parameter->setChoices(choices);
    options.push_back(parameter);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Optimizer Parameter 1");
    option->setPropertyName("OptimizerParameter1");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    options.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Optimizer Parameter 2");
    option->setPropertyName("OptimizerParameter2");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    options.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Maximum Iterations");
    option->setPropertyName("Iterations");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
    options.push_back(option);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Apply Shifts");
    parameter->setPropertyName("ApplyShifts");
    parameter->setWidgetType(FilterParameter::BooleanWidget);
    parameter->setValueType("bool");
    options.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Write Alignment Shift File");
    parameter->setPropertyName("WriteShifts");
    parameter->setWidgetType(FilterParameter::BooleanWidget);
    parameter->setValueType("bool");
    options.push_back(parameter);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Alignment File");
    option->setPropertyName("AlignmentShiftFileName");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setValueType("string");
    options.push_back(option);
  }
  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageRegistration::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayName( reader->readValue( "SelectedCellArrayName", getSelectedCellArrayName() ) );
  setAlignmentShiftFileName( reader->readValue( "AlignmentShiftFileName", getAlignmentShiftFileName() ) );
  setOptimizerParameter1( reader->readValue( "OptimizerParameter1", getOptimizerParameter1() ) );
  setOptimizerParameter2( reader->readValue( "OptimizerParameter2", getOptimizerParameter2() ) );
  setIterations( reader->readValue( "Iterations", getIterations() ) );
  setWriteShifts( reader->readValue( "WriteShifts", getWriteShifts() ) );
  setMetric( reader->readValue( "Metric", getMetric() ) );
  setOptimizer( reader->readValue( "Optimizer", getOptimizer() ) );
  setApplyShifts( reader->readValue( "ApplyShifts", getApplyShifts() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImageRegistration::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)

{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedCellArrayName", getSelectedCellArrayName() );
  writer->writeValue("AlignmentShiftFileName", getAlignmentShiftFileName());
  writer->writeValue("OptimizerParameter1", getOptimizerParameter1());
  writer->writeValue("OptimizerParameter2", getOptimizerParameter2());
  writer->writeValue("Iterations", getIterations());
  writer->writeValue("WriteShifts", getWriteShifts());
  writer->writeValue("ApplyShifts", getApplyShifts());
  writer->writeValue("Metric", getMetric());
  writer->writeValue("Optimizer", getOptimizer());
  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageRegistration::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
    //dont want subpixel shifts if there is information other than image data
    if(m_ApplyShifts)
    {
      if(m->getNumCellArrays()>1)
      {
        ss.str("");
        ss << "Shifts cannot be applied if other cell arrays exist (" << m->getNumCellArrays()-1 << " other array(s) present).";
        setErrorCondition(-1);
        addErrorMessage(getHumanLabel(), ss.str(), -1);
      }
    }
  }

  if(m_WriteShifts && true == m_AlignmentShiftFileName.empty())
  {
    ss.str("");
    ss << "The Alignment Shift file name must be set before executing this filter.";
    setErrorCondition(-1);
    addErrorMessage(getHumanLabel(), ss.str(), -1);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageRegistration::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageRegistration::execute()
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

  //open file to write alignments
  std::ofstream outFile;
  if(m_WriteShifts) outFile.open(getAlignmentShiftFileName().c_str());

  //wrap data as itk image
  ImageProcessing::DefaultImageType::Pointer inputImage=ITKUtilities::Dream3DtoITK(m, m_RawImageData);

  //translation transform
  typedef itk::TranslationTransform< double, ImageProcessing::SliceDimension > TransformType;
  TransformType::Pointer transform = TransformType::New();

  //linearly interpolate pixels
  typedef itk:: LinearInterpolateImageFunction<ImageProcessing::DefaultSliceType, double > InterpolatorType;
  InterpolatorType::Pointer interpolator = InterpolatorType::New();

  //set image comparison metric
  itk::ImageToImageMetric<ImageProcessing::DefaultSliceType,ImageProcessing::DefaultSliceType>::Pointer metric;
  switch(m_Metric)
  {
    case 0: {
        metric = itk::MeanSquaresImageToImageMetric<ImageProcessing::DefaultSliceType, ImageProcessing::DefaultSliceType >::New();
      } break;

    case 1: {
        metric = itk::NormalizedCorrelationImageToImageMetric<ImageProcessing::DefaultSliceType, ImageProcessing::DefaultSliceType >::New();
      } break;

    case 2: {
        metric = itk::MeanReciprocalSquareDifferenceImageToImageMetric<ImageProcessing::DefaultSliceType, ImageProcessing::DefaultSliceType >::New();
      } break;

    case 3: {
        typedef itk::MattesMutualInformationImageToImageMetric<ImageProcessing::DefaultSliceType, ImageProcessing::DefaultSliceType > MattesMetricType;
        MattesMetricType::Pointer mattesMetric = MattesMetricType::New();
        mattesMetric->SetNumberOfHistogramBins( 20 ); // 20-24
        mattesMetric->SetNumberOfSpatialSamples( 10000 );
        //mattesMetric->SetUseExplicitPDFDerivatives( false );
        metric = mattesMetric;
      } break;
  }

  //set up optimization
  itk::SingleValuedNonLinearOptimizer::Pointer optimizer;
  switch(m_Metric)
  {
    case 0: {
        itk::GradientDescentOptimizer::Pointer gradientDescentOptimizer = itk::GradientDescentOptimizer::New();
        gradientDescentOptimizer->SetLearningRate( m_OptimizerParameter1 ); // 0.001-5.0
        gradientDescentOptimizer->SetNumberOfIterations( m_Iterations ); // 200
        CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
        gradientDescentOptimizer->AddObserver( itk::IterationEvent(), observer );
        //if(3==m_Metric)gradientDescentOptimizer->MinimizeOn();
        optimizer = gradientDescentOptimizer;
      } break;

    case 1: {
        itk::RegularStepGradientDescentOptimizer::Pointer regularStepOptimizer = itk::RegularStepGradientDescentOptimizer::New();
        regularStepOptimizer->SetMinimumStepLength( m_OptimizerParameter1 ); // 0.0001-0.01
        regularStepOptimizer->SetMaximumStepLength( m_OptimizerParameter2 ); // 0.1-4.0
        regularStepOptimizer->SetNumberOfIterations( m_Iterations ); // 200-500
        //if(3==m_Metric)regularStepOptimizer->MinimizeOn();
        optimizer = regularStepOptimizer;
      } break;

    case 2: {
        itk::LBFGSOptimizer::Pointer lbfgsOptimizer = itk::LBFGSOptimizer::New();
        lbfgsOptimizer->SetGradientConvergenceTolerance( 0.1 ); //0.01-0.25 (pixels)
        lbfgsOptimizer->SetLineSearchAccuracy( m_OptimizerParameter1 ); // 0.9
        lbfgsOptimizer->SetDefaultStepLength( m_OptimizerParameter2 ); // 1.5 (pixels)
        lbfgsOptimizer->SetMaximumNumberOfFunctionEvaluations( m_Iterations ); // 1000
        lbfgsOptimizer->TraceOn();
        //if(3==m_Metric)lbfgsOptimizer->MinimizeOn();
        optimizer = lbfgsOptimizer;
      } break;

    case 3: {
        itk::AmoebaOptimizer::Pointer amoebaOptimizer = itk::AmoebaOptimizer::New();
        amoebaOptimizer->SetParametersConvergenceTolerance( m_OptimizerParameter1 ); // 0.1-0.25 (pixels)
        amoebaOptimizer->SetFunctionConvergenceTolerance( m_OptimizerParameter2 ); // 0.001 (0.1%)
        amoebaOptimizer->SetMaximumNumberOfIterations( m_Iterations ); // 200
        CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
        amoebaOptimizer->AddObserver( itk::IterationEvent(), observer );
        metric->ComputeGradientOff();
        //if(3==m_Metric)amoebaOptimizer->MinimizeOn();
        optimizer = amoebaOptimizer;
      } break;

    case 4: {
        typedef itk::Statistics::NormalVariateGenerator GeneratorType;
        GeneratorType::Pointer generator = GeneratorType::New();
        generator->Initialize(12345);///need to replace seed with random number (time)

        itk::OnePlusOneEvolutionaryOptimizer::Pointer evolutionaryOptimizer = itk::OnePlusOneEvolutionaryOptimizer::New();
        evolutionaryOptimizer->SetNormalVariateGenerator( generator );
        itk::OnePlusOneEvolutionaryOptimizer::ScalesType optimizerScales ( transform->GetNumberOfParameters() );
        ImageProcessing::DefaultImageType::SpacingType spacing = inputImage->GetSpacing();
        optimizerScales[0] = 1.0 / ( 0.1 * dims[0] * spacing[0] );
        optimizerScales[1] = 1.0 / ( 0.1 * dims[1] * spacing[1] );
        optimizer->SetScales( optimizerScales );
        evolutionaryOptimizer->Initialize( m_OptimizerParameter1 ); // 0.01-0.05
        evolutionaryOptimizer->SetEpsilon( m_OptimizerParameter2 ); // 0.001
        evolutionaryOptimizer->SetMaximumIteration( m_Iterations ); // 2000-4000
        CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
        evolutionaryOptimizer->AddObserver( itk::IterationEvent(), observer );
        //if(3==m_Metric)evolutionaryOptimizer->MinimizeOn();
        optimizer = evolutionaryOptimizer;
      } break;
  }

  //define and create registration
  typedef itk::ImageRegistrationMethod<ImageProcessing::DefaultSliceType, ImageProcessing::DefaultSliceType > RegistrationType;
  RegistrationType::Pointer   registration  = RegistrationType::New();
  registration->SetTransform( transform );
  registration->SetInterpolator( interpolator );
  registration->SetMetric( metric );
  registration->SetOptimizer( optimizer );

  //loop over slices applying registration
  for(int i=1; i<dims[2]; ++i)
  {
    //update gui
    ss.str("");
    ss << "Aligning Slice "<< i << "/" << dims[2];
    notifyStatusMessage(ss.str());

    //get and register 2 images
    ImageProcessing::DefaultSliceType::Pointer fixedImage = ITKUtilities::ExtractSlice<ImageProcessing::DefaultPixelType>(inputImage, ImageProcessing::ZSlice, i-1);
    ImageProcessing::DefaultSliceType::Pointer movingImage = ITKUtilities::ExtractSlice<ImageProcessing::DefaultPixelType>(inputImage, ImageProcessing::ZSlice, i);

    // Set the registration inputs and range
    registration->SetFixedImage(fixedImage);
    registration->SetMovingImage(movingImage);
    registration->SetFixedImageRegion( fixedImage->GetLargestPossibleRegion() );

    //  Initialize the transform
    typedef RegistrationType::ParametersType ParametersType;
    ParametersType initialParameters( transform->GetNumberOfParameters() );
    initialParameters[0] = 0.0;  // Initial offset along X
    initialParameters[1] = 0.0;  // Initial offset along Y
    registration->SetInitialTransformParameters( initialParameters );

    try
    {
      registration->Update();
    }
    catch( itk::ExceptionObject & err )
    {
      ss.str("");
      ss << "ITK Exception: "<< err;
      setErrorCondition(-800);
      notifyErrorMessage(ss.str(), -800);
      return;
    }

    //get final registration parameters
    ParametersType finalParameters = registration->GetLastTransformParameters();
    const double TranslationAlongX = finalParameters[0];
    const double TranslationAlongY = finalParameters[1];

    //write shifts to file
    if(m_WriteShifts) outFile << i << " " << TranslationAlongX << " " << TranslationAlongY << std::endl;

    //apply shifts
    if(m_ApplyShifts)
    {
      typedef itk::ResampleImageFilter<ImageProcessing::DefaultSliceType, ImageProcessing::DefaultSliceType>    ResampleFilterType;
      ResampleFilterType::Pointer resampler = ResampleFilterType::New();
      resampler->SetInput( movingImage);
      resampler->SetTransform( registration->GetOutput()->Get() );
      resampler->SetSize( fixedImage->GetLargestPossibleRegion().GetSize() );
      resampler->SetOutputOrigin(  fixedImage->GetOrigin() );
      resampler->SetOutputSpacing( fixedImage->GetSpacing() );
      resampler->SetOutputDirection( fixedImage->GetDirection() );
      resampler->SetDefaultPixelValue( 0 );
      resampler->Update();
      ITKUtilities::SetSlice<ImageProcessing::DefaultPixelType>(inputImage, resampler->GetOutput(), ImageProcessing::ZSlice, i);
    }
  }

  if(m_WriteShifts) outFile.close();

  /* Let the GUI know we are done with this filter */
   notifyStatusMessage("Complete");
}
