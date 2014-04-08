/* ============================================================================
 *
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "AlignSectionsPhaseCorrelation.h"

#include "ITKUtilities.h"
#include "itkConstantPadImageFilter.h"
#include "itkFFTShiftImageFilter.h"
#include "itkVnlForwardFFTImageFilter.h"
#include "itkComplexConjugateImageAdaptor.h"
#include "itkMultiplyImageFilter.h"
#include "itkComplexToModulusImageFilter.h"
#include "itkDivideImageFilter.h"
#include "itkVnlInverseFFTImageFilter.h"
#include "itkMinimumMaximumImageCalculator.h"

//#include "itkImageDuplicator.h

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsPhaseCorrelation::AlignSectionsPhaseCorrelation() :
  AlignSections(),
  m_RawImageDataArrayName("RawImageData"),
  m_SelectedCellArrayName(""),
  m_RawImageData(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AlignSectionsPhaseCorrelation::~AlignSectionsPhaseCorrelation()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsPhaseCorrelation::setupFilterParameters()
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
    option->setHumanLabel("Write Alignment Shift File");
    option->setPropertyName("WriteAlignmentShifts");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    options.push_back(option);
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
void AlignSectionsPhaseCorrelation::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayName( reader->readValue( "SelectedCellArrayName", getSelectedCellArrayName() ) );
  setAlignmentShiftFileName( reader->readValue( "AlignmentShiftFileName", getAlignmentShiftFileName() ) );
  setWriteAlignmentShifts( reader->readValue( "WriteAlignmentShifts", getWriteAlignmentShifts() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int AlignSectionsPhaseCorrelation::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SelectedCellArrayName", getSelectedCellArrayName() );
  writer->writeValue("AlignmentShiftFileName", getAlignmentShiftFileName());
  writer->writeValue("WriteAlignmentShifts", getWriteAlignmentShifts());
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsPhaseCorrelation::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();


  if(true == getWriteAlignmentShifts() && getAlignmentShiftFileName().empty() == true)
  {
    ss << "The Alignment Shift file name must be set before executing this filter.";
    setErrorCondition(-1);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }

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
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsPhaseCorrelation::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsPhaseCorrelation::execute()
{
  setErrorCondition(0);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  int64_t totalPoints = m->getTotalPoints();
  size_t numgrains = m->getNumFieldTuples();
  size_t numensembles = m->getNumEnsembleTuples();
  dataCheck(false, totalPoints, numgrains, numensembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  AlignSections::execute();

  // If there is an error set this to something negative and also set a message
  notifyStatusMessage("Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AlignSectionsPhaseCorrelation::find_shifts(std::vector<int> &xshifts, std::vector<int> &yshifts)
{
  VoxelDataContainer* m = getVoxelDataContainer();

  std::ofstream outFile;
  if (getWriteAlignmentShifts() == true) {
    outFile.open(getAlignmentShiftFileName().c_str());
  }

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

  //determine closest fft compatible size (needs to be a power of 2, 3, or 5 for itk)
  int imageSize[2] = {dims[0], dims[1]};
  for(int i=0; i<2; i++)
  {
      int logImageSize = log(imageSize[i]);
      int pow2Size = pow(2,ceil(logImageSize/log(2)));
      int pow3Size = pow(3,ceil(logImageSize/log(3)));
      int pow5Size = pow(5,ceil(logImageSize/log(5)));
      imageSize[i] = std::min(pow3Size, pow5Size);
      imageSize[i] = std::min(pow2Size, imageSize[i]);
  }

  //determine padding needed for slices
  ImageProcessing::FloatSliceType::SizeType lowerPadRegion;
  lowerPadRegion.Fill(0);
  ImageProcessing::FloatSliceType::SizeType upperPadRegion;
  upperPadRegion[0]=imageSize[0]-dims[0];
  upperPadRegion[1]=imageSize[1]-dims[1];
  ImageProcessing::FloatSliceType::PixelType constantPixel = 0;

  //wrap dream3d array as itk image + set up slices
  ImageProcessing::DefaultImageType::Pointer inputImage=ITKUtilities::Dream3DtoITK(m, m_RawImageData);
  ImageProcessing::DefaultSliceType::Pointer fixedImage;
  ImageProcessing::DefaultSliceType::Pointer movingImage;

  //define filter types needed (fft requires a float or double image
  typedef itk::ConstantPadImageFilter< ImageProcessing::DefaultSliceType, ImageProcessing::FloatSliceType> PadImageType;
  typedef itk::FFTShiftImageFilter< ImageProcessing::FloatSliceType, ImageProcessing::FloatSliceType > FFTShiftFilterType;
  typedef itk::VnlForwardFFTImageFilter< ImageProcessing::FloatSliceType >  FFTFilterType;
  typedef FFTFilterType::OutputImageType    SpectralImageType;
  typedef itk::ComplexConjugateImageAdaptor< SpectralImageType > ConjugateType;
  typedef itk::MultiplyImageFilter< SpectralImageType, ConjugateType, SpectralImageType >  MultiplyFilterType;
  typedef itk::ComplexToModulusImageFilter<SpectralImageType, ImageProcessing::FloatSliceType> ModulusFilterType;
  typedef itk::DivideImageFilter< SpectralImageType, ImageProcessing::FloatSliceType, SpectralImageType >  DivideFilterType;
  typedef itk::VnlInverseFFTImageFilter< SpectralImageType >  IFFTFilterType;
  typedef itk::MinimumMaximumImageCalculator <ImageProcessing::FloatSliceType>  ImageCalculatorFilterType;

  //since the phase correlation is being calculated pairwise through the stack saving from one iteration to the next save operations
  //typedef itk::ImageDuplicator< SpectralImageType > DuplicatorType;

  for (DimType iter = 1; iter < dims[2]; iter++)
  {
    std::stringstream ss;
    int progInt = ((float)iter/dims[2])*100.0f;
    ss << "Determining Shifts - " << progInt << "% Complete";
    notifyStatusMessage(ss.str());
    if (getCancel() == true)
    {
      return;
    }
    int slice = static_cast<int>( (dims[2] - 1) - iter );

    //get 2 slices
    fixedImage = ITKUtilities::ExtractSlice<ImageProcessing::DefaultPixelType>(inputImage, ImageProcessing::ZSlice, slice);
    movingImage = ITKUtilities::ExtractSlice<ImageProcessing::DefaultPixelType>(inputImage, ImageProcessing::ZSlice, slice+1);

    //pad to fft compatible size
    PadImageType::Pointer padFixed = PadImageType::New();
    padFixed->SetInput(fixedImage);
    padFixed->SetPadLowerBound(lowerPadRegion);
    padFixed->SetPadUpperBound(upperPadRegion);
    padFixed->SetConstant(constantPixel);

    PadImageType::Pointer padMoving = PadImageType::New();
    padMoving->SetInput(movingImage);
    padMoving->SetPadLowerBound(lowerPadRegion);
    padMoving->SetPadUpperBound(upperPadRegion);
    padMoving->SetConstant(constantPixel);

    //shift
    FFTShiftFilterType::Pointer fixedFFTShiftFilter = FFTShiftFilterType::New();
    fixedFFTShiftFilter->SetInput(padFixed->GetOutput());

    FFTShiftFilterType::Pointer movingFFTShiftFilter = FFTShiftFilterType::New();
    movingFFTShiftFilter->SetInput(padMoving->GetOutput());

    //FFT
    FFTFilterType::Pointer fixedFFTFilter = FFTFilterType::New();
    fixedFFTFilter->SetInput( fixedFFTShiftFilter->GetOutput() );

    FFTFilterType::Pointer movingFFTFilter = FFTFilterType::New();
    movingFFTFilter->SetInput( movingFFTShiftFilter->GetOutput() );

    //wrap moving image as complex conjugate and multiply with moving fft
    ConjugateType::Pointer conjugateFilter = ConjugateType::New();
    conjugateFilter->SetImage(movingFFTFilter->GetOutput());

    MultiplyFilterType::Pointer multiplyFilter = MultiplyFilterType::New();
    multiplyFilter->SetInput1( fixedFFTFilter->GetOutput() );
    multiplyFilter->SetInput2( conjugateFilter );

    //normalize
    ModulusFilterType::Pointer modulusFilter = ModulusFilterType::New();
    modulusFilter->SetInput(multiplyFilter->GetOutput());

    DivideFilterType::Pointer divideFilter = DivideFilterType::New();
    divideFilter->SetInput1( multiplyFilter->GetOutput() );
    divideFilter->SetInput2( modulusFilter->GetOutput() );

    //inverse fft and find location of maximum
    IFFTFilterType::Pointer fftInverseFilter = IFFTFilterType::New();
    fftInverseFilter->SetInput( divideFilter->GetOutput() );
    fftInverseFilter->Update();

    ImageCalculatorFilterType::Pointer imageCalculatorFilter = ImageCalculatorFilterType::New ();
    imageCalculatorFilter->SetImage(fftInverseFilter->GetOutput());
    imageCalculatorFilter->Compute();

    ImageProcessing::FloatSliceType::IndexType maximumLocation = imageCalculatorFilter->GetIndexOfMaximum();

    //check all 9 possibilities to find shifts which which actually have overlap
    std::vector< std::pair <int, int> > transformList;
    for(int i=-1; i<2; i++)
    {
       for(int j=-1; j<2; j++)
       {
          int xShift=maximumLocation[0]+i*imageSize[0];
          int yShift=maximumLocation[1]+j*imageSize[1];
          bool xOverlap = true;
          bool yOverlap = true;
          if(xShift+(int)dims[0]<0 || xShift>(int)dims[0]) xOverlap=false;
          if(yShift+(int)dims[1]<0 || yShift>(int)dims[1]) yOverlap=false;
          if(xOverlap && yOverlap) transformList.push_back(std::make_pair(xShift, yShift));
       }
    }

    //pick best match
    //for now I assume the shift with the most overlap is best but applying an image to image metric on each possibility is more robust
    int mostOverlap = 0;
    int newxshift=0;
    int newyshift=0;
    for(std::vector< std::pair <int, int> >::iterator it = transformList.begin(); it!=transformList.end(); it++)
    {
      int overlap = ( dims[0]-abs((*it).first) )*( dims[1]-abs((*it).second) );
      if(overlap>mostOverlap)
      {
        mostOverlap=overlap;
        newxshift=(*it).first;
        newyshift=(*it).second;
      }
    }

    xshifts[iter] = xshifts[iter-1] + newxshift;
    yshifts[iter] = yshifts[iter-1] + newyshift;
    if (getWriteAlignmentShifts() == true) {
      outFile << slice << "	" << slice+1 << "	" << newxshift << "	" << newyshift << "	" << xshifts[iter] << "	" << yshifts[iter] << std::endl;
    }
  }
  if (getWriteAlignmentShifts() == true) {
    outFile.close();
  }
}
