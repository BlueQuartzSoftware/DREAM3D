/*
 * Your License or Copyright Information can go here
 */
#if (_MSC_VER)
#define _SCL_SECURE_NO_WARNINGS
#endif

#include "MedianKernel.h"

#include <QtCore/QString>

#include "ITKUtilities.h"
#include "itkMedianImageFilter.h"

//// Setup some typedef 's for the ITKUtilities class to shorten up our code
typedef ITKUtilities<ImageProcessing::DefaultPixelType>    ITKUtilitiesType;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MedianKernel::MedianKernel() :
  AbstractFilter(),
  m_SelectedCellArrayPath("", "", ""),
  m_NewCellArrayName(""),
  m_SaveAsNewArray(true),
  m_Slice(false),
  m_SelectedCellArrayArrayName(""),
  m_SelectedCellArray(NULL),
  m_NewCellArray(NULL)
{
  m_KernelSize.x = 1;
  m_KernelSize.y = 1;
  m_KernelSize.z = 1;
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MedianKernel::~MedianKernel()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MedianKernel::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Array to Process", "SelectedCellArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedCellArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Kernel Size", "KernelSize", FilterParameterWidgetType::IntVec3Widget, getKernelSize(), false, ""));
  QStringList linkedProps;
  linkedProps << "NewCellArrayName";
  parameters.push_back(FilterParameter::NewConditional("Save As New Array", "SaveAsNewArray", FilterParameterWidgetType::LinkedBooleanWidget, getSaveAsNewArray(), false, linkedProps));

  parameters.push_back(FilterParameter::New("Created Array Name", "NewCellArrayName", FilterParameterWidgetType::StringWidget, getNewCellArrayName(), false, ""));
  parameters.push_back(FilterParameter::New("Slice at a Time", "Slice", FilterParameterWidgetType::BooleanWidget, getSlice(), false));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MedianKernel::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayPath( reader->readDataArrayPath( "SelectedCellArrayPath", getSelectedCellArrayPath() ) );
  setNewCellArrayName( reader->readString( "NewCellArrayName", getNewCellArrayName() ) );
  setSaveAsNewArray( reader->readValue( "SaveAsNewArray", getSaveAsNewArray() ) );
  setSlice( reader->readValue( "Slice", getSlice() ) );
  reader->closeFilterGroup();
  setKernelSize( reader->readIntVec3( "KernelSize", getKernelSize() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MedianKernel::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedCellArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(NewCellArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SaveAsNewArray)
  DREAM3D_FILTER_WRITE_PARAMETER(Slice)
  DREAM3D_FILTER_WRITE_PARAMETER(KernelSize)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MedianKernel::dataCheck()
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
void MedianKernel::preflight()
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
void MedianKernel::execute()
{
  //int err = 0;
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  /* Place all your code to execute your filter here. */
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getSelectedCellArrayPath().getDataContainerName());
  QString attrMatName = getSelectedCellArrayPath().getAttributeMatrixName();

  ImageProcessing::DefaultImageType::Pointer inputImage = ITKUtilitiesType::Dream3DtoITK(m, attrMatName, m_SelectedCellArray);

  //create edge filter
  typedef itk::MedianImageFilter < ImageProcessing::DefaultImageType,  ImageProcessing::DefaultImageType > MedianFilterType;
  MedianFilterType::Pointer medianFilter = MedianFilterType::New();
  medianFilter->SetInput(inputImage);

  //set kernel size
  MedianFilterType::InputSizeType radius;
  radius[0] = m_KernelSize.x;
  radius[1] = m_KernelSize.y;
  radius[2] = m_KernelSize.z;
  medianFilter->SetRadius(radius);

  //have filter write to dream3d array instead of creating its own buffer
  ITKUtilitiesType::SetITKFilterOutput(medianFilter->GetOutput(), m_NewCellArrayPtr.lock());

  //execute filter
  medianFilter->Update();

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
AbstractFilter::Pointer MedianKernel::newFilterInstance(bool copyFilterParameters)
{
  MedianKernel::Pointer filter = MedianKernel::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MedianKernel::getCompiledLibraryName()
{return ImageProcessing::ImageProcessingBaseName;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MedianKernel::getGroupName()
{return "ImageProcessing";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MedianKernel::getSubGroupName()
{return "Misc";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString MedianKernel::getHumanLabel()
{return "Median (Kernel)";}

