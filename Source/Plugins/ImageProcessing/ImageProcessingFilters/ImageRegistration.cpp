/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ImageRegistration.h"

#include "ITKUtilities.h"
#include "itkImageRegistrationMethod.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkMeanSquaresImageToImageMetric.h"
#include "itkNormalizedCorrelationImageToImageMetric.h"
#include "itkMeanReciprocalSquareDifferenceImageToImageMetric.h" //Pattern Intensity

#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkResampleImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSpatialObjectToImageFilter.h"
#include "itkTranslationTransform.h"


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
m_MinStep(0.01f),
m_MaxStep(4.00f),
m_MaxIterations(200),
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
    choices.push_back("Pattern Intensity");
    parameter->setChoices(choices);
    options.push_back(parameter);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Minimum Step Size");
    option->setPropertyName("MinStep");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("pixels");
    options.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Maximum Step Size");
    option->setPropertyName("MaxStep");
    option->setWidgetType(FilterParameter::DoubleWidget);
    option->setValueType("float");
    option->setCastableValueType("double");
    option->setUnits("piexls");
    options.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Maximum Iterations");
    option->setPropertyName("MaxIterations");
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
  setMinStep( reader->readValue( "MinStep", getMinStep() ) );
  setMaxStep( reader->readValue( "MaxStep", getMaxStep() ) );
  setMaxIterations( reader->readValue( "MaxIterations", getMaxIterations() ) );
  setWriteShifts( reader->readValue( "WriteShifts", getWriteShifts() ) );
  setMetric( reader->readValue( "Metric", getMetric() ) );
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
  writer->writeValue("MinStep", getMinStep());
  writer->writeValue("MaxStep", getMaxStep());
  writer->writeValue("MaxIterations", getMaxIterations());
  writer->writeValue("WriteShifts", getWriteShifts());
  writer->writeValue("ApplyShifts", getApplyShifts());
  writer->writeValue("Metric", getMetric());
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
    GET_PREREQ_DATA(m, DREAM3D, CellData, RawImageData, ss, -300, uint8_t, UInt8ArrayType, voxels, 1)
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
  ImageProcessing::UInt8ImageType::Pointer inputImage=ITKUtilities::Dream3DtoITK(m, m_RawImageData);

  ///setup registration
   //  The transform that will map the fixed image into the moving image.
  typedef itk::TranslationTransform< double, ImageProcessing::SliceDimension > TransformType;

  //  An optimizer is required to explore the parameter space of the transform
  //  in search of optimal values of the metric.
  typedef itk::RegularStepGradientDescentOptimizer       OptimizerType;

  //  The metric will compare how well the two images match each other. Metric
  //  types are usually parameterized by the image types as it can be seen in
  //  the following type declaration.
  typedef itk::MeanSquaresImageToImageMetric<ImageProcessing::UInt8SliceType, ImageProcessing::UInt8SliceType >  MeanSquaresMetricType;
  typedef itk::NormalizedCorrelationImageToImageMetric<ImageProcessing::UInt8SliceType, ImageProcessing::UInt8SliceType >  NormalizedCorrelationMetricType;
  typedef itk::MeanReciprocalSquareDifferenceImageToImageMetric<ImageProcessing::UInt8SliceType, ImageProcessing::UInt8SliceType >  PatternIntensityMetricType;

 //  Finally, the type of the interpolator is declared. The interpolator will
  //  evaluate the intensities of the moving image at non-grid positions.
  typedef itk:: LinearInterpolateImageFunction<
      ImageProcessing::UInt8SliceType,
      double          >    InterpolatorType;

  //  The registration method type is instantiated using the types of the
  //  fixed and moving images. This class is responsible for interconnecting
  //  all the components that we have described so far.
  typedef itk::ImageRegistrationMethod<
      ImageProcessing::UInt8SliceType,
      ImageProcessing::UInt8SliceType >    RegistrationType;

  // Create components
  //MetricType::Pointer         metric        = MetricType::New();
  TransformType::Pointer      transform     = TransformType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();

  // Each component is now connected to the instance of the registration method.
  //registration->SetMetric(        metric        );
  switch(m_Metric)
  {
    case 0:
      {
        registration->SetMetric(        MeanSquaresMetricType::New()        );
      }
      break;

    case 1:
      {
        registration->SetMetric(        NormalizedCorrelationMetricType::New()        );
      }
      break;

    case 2:
      {
        registration->SetMetric(        PatternIntensityMetricType::New()        );
      }
      break;
  }
  registration->SetOptimizer(     optimizer     );
  registration->SetTransform(     transform     );
  registration->SetInterpolator(  interpolator  );

  for(int i=1; i<dims[2]; ++i)
  {
    //update gui
    ss.str("");
    ss << "Aligning Slice "<< i << "/" << dims[2];
    notifyStatusMessage(ss.str());

    //get and register 2 images
    ImageProcessing::UInt8SliceType::Pointer fixedImage = ITKUtilities::ExtractSlice<ImageProcessing::UInt8PixelType>(inputImage, ImageProcessing::ZSlice, i-1);
    ImageProcessing::UInt8SliceType::Pointer movingImage = ITKUtilities::ExtractSlice<ImageProcessing::UInt8PixelType>(inputImage, ImageProcessing::ZSlice, i);

    // Set the registration inputs
    registration->SetFixedImage(fixedImage);
    registration->SetMovingImage(movingImage);

    registration->SetFixedImageRegion(
    fixedImage->GetLargestPossibleRegion() );

    //  Initialize the transform
    typedef RegistrationType::ParametersType ParametersType;
    ParametersType initialParameters( transform->GetNumberOfParameters() );

    initialParameters[0] = 0.0;  // Initial offset along X
    initialParameters[1] = 0.0;  // Initial offset along Y

    registration->SetInitialTransformParameters( initialParameters );

    optimizer->SetMaximumStepLength( m_MaxStep );
    optimizer->SetMinimumStepLength( m_MinStep );

    // Set a stopping criterion
    optimizer->SetNumberOfIterations( m_MaxIterations );

    try
    {
      registration->Update();
    }
    catch( itk::ExceptionObject & err )
    {
      ss.str("");
      ss << "ExceptionObject caught!: "<< err;
      setErrorCondition(-800);
      notifyErrorMessage(ss.str(), -800);
      return;
    }

    //  The result of the registration process is an array of parameters that
    //  defines the spatial transformation in an unique way. This final result is
    //  obtained using the \code{GetLastTransformParameters()} method.

    ParametersType finalParameters = registration->GetLastTransformParameters();

    //  In the case of the \doxygen{TranslationTransform}, there is a
    //  straightforward interpretation of the parameters.  Each element of the
    //  array corresponds to a translation along one spatial dimension.

    const double TranslationAlongX = finalParameters[0];
    const double TranslationAlongY = finalParameters[1];
    if(m_WriteShifts) outFile << i << " " << TranslationAlongX << " " << TranslationAlongY << std::endl;

    //  A resampling filter is created and the moving image is connected as  its input.
     if(m_ApplyShifts)
    {
      typedef itk::ResampleImageFilter<ImageProcessing::UInt8SliceType, ImageProcessing::UInt8SliceType>    ResampleFilterType;
      ResampleFilterType::Pointer resampler = ResampleFilterType::New();
      resampler->SetInput( movingImage);
      resampler->SetTransform( registration->GetOutput()->Get() );
      resampler->SetSize( fixedImage->GetLargestPossibleRegion().GetSize() );
      resampler->SetOutputOrigin(  fixedImage->GetOrigin() );
      resampler->SetOutputSpacing( fixedImage->GetSpacing() );
      resampler->SetOutputDirection( fixedImage->GetDirection() );
      resampler->SetDefaultPixelValue( 0 );
      resampler->Update();
      ITKUtilities::SetSlice<ImageProcessing::UInt8PixelType>(inputImage, resampler->GetOutput(), ImageProcessing::ZSlice, i);
    }
  }

  if(m_WriteShifts) outFile.close();

  /* Let the GUI know we are done with this filter */
   notifyStatusMessage("Complete");
}
