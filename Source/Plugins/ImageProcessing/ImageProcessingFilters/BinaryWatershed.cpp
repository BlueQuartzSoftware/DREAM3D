/* ============================================================================
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "BinaryWatershed.h"

//thresholding filter
#include "itkSignedMaurerDistanceMapImageFilter.h"
#include "itkInvertIntensityImageFilter.h"
#include "itkMorphologicalWatershedFromMarkersImageFilter.h"
#include "itkMaskImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"

// ImageProcessing Plugin
#include "ItkBridge.h"
#include "ImageProcessing/ImageProcessingHelpers.hpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BinaryWatershed::BinaryWatershed() :
  AbstractFilter(),
  m_SelectedCellArrayPath("", "", ""),
  m_PeakTolerance(1.0),
  m_NewCellArrayName("BinaryWatershed"),
  m_SelectedCellArray(NULL),
  m_NewCellArray(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BinaryWatershed::~BinaryWatershed()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BinaryWatershed::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Input Array", "SelectedCellArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedCellArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Peak Noise Tolerance", "PeakTolerance", FilterParameterWidgetType::DoubleWidget, getPeakTolerance(), false, ""));
  parameters.push_back(FilterParameter::New("Created Array Name", "NewCellArrayName", FilterParameterWidgetType::StringWidget, getNewCellArrayName(), false, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BinaryWatershed::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayPath( reader->readDataArrayPath( "SelectedCellArrayPath", getSelectedCellArrayPath() ) );
  setPeakTolerance( reader->readValue( "PeakTolerance", getPeakTolerance() ) );
  setNewCellArrayName( reader->readString( "NewCellArrayName", getNewCellArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BinaryWatershed::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedCellArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(PeakTolerance)
  DREAM3D_FILTER_WRITE_PARAMETER(NewCellArrayName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BinaryWatershed::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  QVector<size_t> dims(1, 1);
  m_SelectedCellArrayPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<bool>, AbstractFilter>(this, getSelectedCellArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SelectedCellArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SelectedCellArray = m_SelectedCellArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  tempPath.update(getSelectedCellArrayPath().getDataContainerName(), getSelectedCellArrayPath().getAttributeMatrixName(), getNewCellArrayName() );
  m_NewCellArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<bool>, AbstractFilter, ImageProcessing::DefaultPixelType>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_NewCellArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_NewCellArray = m_NewCellArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BinaryWatershed::preflight()
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
void BinaryWatershed::execute()
{
  QString ss;
  dataCheck();
  if(getErrorCondition() < 0)
  {
    setErrorCondition(-10000);
    ss = QObject::tr("DataCheck did not pass during execute");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getSelectedCellArrayPath().getDataContainerName());
  QString attrMatName = getSelectedCellArrayPath().getAttributeMatrixName();

  //get utilities
  typedef ItkBridge<bool> BoolBridgeType;
  typedef ItkBridge<float> FloatBridgeType;

  //wrap input
  BoolBridgeType::ScalarImageType::Pointer inputImage = BoolBridgeType::CreateItkWrapperForDataPointer(m, attrMatName, m_SelectedCellArray);

  //compute distance map
  typedef itk::SignedMaurerDistanceMapImageFilter<BoolBridgeType::ScalarImageType, FloatBridgeType::ScalarImageType> DistanceMapType;
  DistanceMapType::Pointer distanceMap = DistanceMapType::New();
  distanceMap->SetInsideIsPositive(true);
  distanceMap->SetInput(inputImage);
  try
  {
    distanceMap->Update();
  }
  catch( itk::ExceptionObject & err )
  {
    setErrorCondition(-5);
    QString ss = QObject::tr("Failed to execute itk::KMeans filter. Error Message returned from ITK:\n   %1").arg(err.GetDescription());
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  //find maxima in distance map (ultimate points)
  std::vector<FloatBridgeType::ScalarImageType::IndexType> peakLocations = ImageProcessing::LocalMaxima<FloatBridgeType::ScalarImageType>::Find(distanceMap->GetOutput(), m_PeakTolerance, true);

  //create labeled image from peaks
  typedef itk::Image<uint32_t, FloatBridgeType::ScalarImageType::ImageDimension> LabelImageType;
  LabelImageType::Pointer seedLabels = LabelImageType::New();
  LabelImageType::RegionType region = inputImage->GetLargestPossibleRegion();
  seedLabels->SetRegions(region);
  seedLabels->Allocate();
  seedLabels->FillBuffer(0);
  for(size_t i=0; i<peakLocations.size(); i++)
  {
    seedLabels->SetPixel(peakLocations[i], i+1);
  }

  //invert distance map as gradient for watershed
  typedef itk::InvertIntensityImageFilter< FloatBridgeType::ScalarImageType, FloatBridgeType::ScalarImageType > InvertType;
  InvertType::Pointer invert = InvertType::New();
  invert->SetInput(distanceMap->GetOutput());
  invert->SetMaximum(0);

  //set up seeded watershed
  typedef itk::MorphologicalWatershedFromMarkersImageFilter< FloatBridgeType::ScalarImageType, LabelImageType > WatershedType;
  WatershedType::Pointer watershed = WatershedType::New();
  watershed->SetInput1(invert->GetOutput());
  watershed->SetInput2(seedLabels);

  //mask watershed output (we only want things that were originally in the)
  typedef itk::MaskImageFilter<LabelImageType, BoolBridgeType::ScalarImageType, LabelImageType> MaskType;
  MaskType::Pointer mask = MaskType::New();
  mask->SetInput(watershed->GetOutput());
  mask->SetMaskImage(inputImage);

  //threshold all labels into boolean array
  typedef itk::BinaryThresholdImageFilter< LabelImageType, BoolBridgeType::ScalarImageType > ThresholdType;
  ThresholdType::Pointer threshold = ThresholdType::New();
  threshold->SetInput(mask->GetOutput());
  threshold->SetLowerThreshold(1);
  threshold->SetInsideValue(true);
  threshold->SetOutsideValue(false);

  //wrap output
  BoolBridgeType::SetITKFilterOutput(threshold->GetOutput(), m_NewCellArrayPtr.lock());
  try
  {
    threshold->Update();
  }
  catch( itk::ExceptionObject & err )
  {
    setErrorCondition(-5);
    QString ss = QObject::tr("Failed to execute itk::KMeans filter. Error Message returned from ITK:\n   %1").arg(err.GetDescription());
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer BinaryWatershed::newFilterInstance(bool copyFilterParameters)
{
  BinaryWatershed::Pointer filter = BinaryWatershed::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString BinaryWatershed::getCompiledLibraryName()
{return ImageProcessing::ImageProcessingBaseName;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString BinaryWatershed::getGroupName()
{return "ImageProcessing";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString BinaryWatershed::getSubGroupName()
{return "Misc";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString BinaryWatershed::getHumanLabel()
{return "Binary Watershed";}

