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
  }

  if(true == m_AlignmentShiftFileName.empty())
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
  outFile.open(getAlignmentShiftFileName().c_str());

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
  typedef itk::MeanSquaresImageToImageMetric<
      ImageProcessing::UInt8SliceType,
      ImageProcessing::UInt8SliceType >    MetricType;

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
  MetricType::Pointer         metric        = MetricType::New();
  TransformType::Pointer      transform     = TransformType::New();
  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
  RegistrationType::Pointer   registration  = RegistrationType::New();

  // Each component is now connected to the instance of the registration method.
  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetTransform(     transform     );
  registration->SetInterpolator(  interpolator  );

  for(int i=dims[2]-1; i>0; i--)
  {
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

    optimizer->SetMaximumStepLength( 4.00 );
    optimizer->SetMinimumStepLength( 0.01 );

    // Set a stopping criterion
    optimizer->SetNumberOfIterations( 200 );

    try
    {
      registration->Update();
    }
    catch( itk::ExceptionObject & err )
    {
      std::stringstream ss;
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

    int xShift=0;
    int yShift=0;

    if(TranslationAlongX>0)
    {
      xShift=floor(TranslationAlongX);
    } else
    {
      xShift=ceil(TranslationAlongX);
    }
    if((fabsf(TranslationAlongX)-abs(xShift))>0.5)
    {
      if(xShift>0)xShift++;
      if(xShift<0)xShift--;
    }
    if(TranslationAlongY>0)
    {
      yShift=floor(TranslationAlongY);
    } else
    {
      yShift=ceil(TranslationAlongY);
    }
    if((fabsf(TranslationAlongY)-abs(xShift))>0.5)
    {
      if(yShift>0)yShift++;
      if(yShift<0)yShift--;
    }

    outFile << i << " " << -xShift << " " << -yShift << std::endl;
  }

  outFile.close();

  //array name changing/cleanup

  /* Let the GUI know we are done with this filter */
   notifyStatusMessage("Complete");
}
