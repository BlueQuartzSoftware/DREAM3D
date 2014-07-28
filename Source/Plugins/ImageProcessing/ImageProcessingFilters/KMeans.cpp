/*
 * Your License or Copyright Information can go here
 */
#if (_MSC_VER)
#define _SCL_SECURE_NO_WARNINGS
#endif

#include "KMeans.h"

#include "DREAM3DLib/Common/Constants.h"

#include "ITKUtilities.h"

#include "itkScalarImageKmeansImageFilter.h"
#include "itkMinimumMaximumImageCalculator.h"

//// Setup some typedef 's for the ITKUtilities class to shorten up our code
typedef ITKUtilities<ImageProcessing::DefaultPixelType>    ITKUtilitiesType;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
KMeans::KMeans() :
  AbstractFilter(),
  m_SelectedCellArrayPath("", "", ""),
  m_NewCellArrayName(""),
  m_SaveAsNewArray(true),
  m_Slice(false),
  m_Classes(2),
  m_SelectedCellArrayArrayName(""),
  m_SelectedCellArray(NULL),
  m_NewCellArray(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
KMeans::~KMeans()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void KMeans::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Array to Process", "SelectedCellArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedCellArrayPath(), false, ""));
  QStringList linkedProps;
  linkedProps << "NewCellArrayName";
  parameters.push_back(FilterParameter::NewConditional("Save As New Array", "SaveAsNewArray", FilterParameterWidgetType::LinkedBooleanWidget, getSaveAsNewArray(), false, linkedProps));
  parameters.push_back(FilterParameter::New("Created Array Name", "NewCellArrayName", FilterParameterWidgetType::StringWidget, getNewCellArrayName(), false, ""));
  parameters.push_back(FilterParameter::New("Number of Classes", "Classes", FilterParameterWidgetType::IntWidget, getClasses(), false, ""));
  parameters.push_back(FilterParameter::New("Slice at a Time", "Slice", FilterParameterWidgetType::BooleanWidget, getSlice(), false));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void KMeans::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayPath( reader->readDataArrayPath( "SelectedCellArrayPath", getSelectedCellArrayPath() ) );
  setNewCellArrayName( reader->readString( "NewCellArrayName", getNewCellArrayName() ) );
  setSaveAsNewArray( reader->readValue( "SaveAsNewArray", getSaveAsNewArray() ) );
  setSlice( reader->readValue( "Slice", getSlice() ) );
  setClasses( reader->readValue( "Classes", getClasses() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int KMeans::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedCellArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(NewCellArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(SaveAsNewArray)
  DREAM3D_FILTER_WRITE_PARAMETER(Slice)
  DREAM3D_FILTER_WRITE_PARAMETER(Classes)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void KMeans::dataCheck()
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

  if(m_Classes < 2)
  {
    QString ss = QObject::tr("Must threshold into at least 2 classes");
    setErrorCondition(-1000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void KMeans::preflight()
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
void KMeans::execute()
{
  //int err = 0;
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getSelectedCellArrayPath().getDataContainerName());
  QString attrMatName = getSelectedCellArrayPath().getAttributeMatrixName();

  /* Place all your code to execute your filter here. */
  //get dims
  size_t udims[3] = {0, 0, 0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  //wrap input as itk image
  ImageProcessing::DefaultImageType::Pointer inputImage = ITKUtilitiesType::Dream3DtoITK(m, attrMatName, m_SelectedCellArray);

  if(m_Slice)
  {
    //define filters
    typedef itk::MinimumMaximumImageCalculator< ImageProcessing::DefaultSliceType > CalculatorType;
    typedef itk::ScalarImageKmeansImageFilter< ImageProcessing::DefaultSliceType, ImageProcessing::DefaultSliceType > KMeansType;

    //wrap output buffer as image
    ImageProcessing::DefaultImageType::Pointer outputImage = ITKUtilitiesType::Dream3DtoITK(m, attrMatName, m_NewCellArray);

    //loop over slices
    for(int i = 0; i < dims[2]; i++)
    {
      //get slice
      ImageProcessing::DefaultSliceType::Pointer slice = ITKUtilitiesType::ExtractSlice(inputImage, ImageProcessing::ZSlice, i);

      //find max/min
      CalculatorType::Pointer minMaxFilter = CalculatorType::New ();
      minMaxFilter->SetImage(slice);
      minMaxFilter->Compute();
      ImageProcessing::DefaultPixelType range = minMaxFilter->GetMaximum() - minMaxFilter->GetMinimum();

      //set up kmeans filter
      KMeansType::Pointer kMeans = KMeansType::New();
      kMeans->SetInput(slice);
      ImageProcessing::DefaultPixelType meanIncrement = range / m_Classes;
      ImageProcessing::DefaultPixelType mean = range / (2 * m_Classes);
      for(int j = 0; j < m_Classes; j++)
      {
        kMeans->AddClassWithInitialMean(mean);
        mean = mean + meanIncrement;
      }

      //execute
      kMeans->Update();

      //copy back into volume
      ITKUtilitiesType::SetSlice(outputImage, kMeans->GetOutput(), ImageProcessing::ZSlice, i);
    }
  }
  else
  {
    //find min+max of image
    typedef itk::MinimumMaximumImageCalculator< ImageProcessing::DefaultImageType > CalculatorType;
    CalculatorType::Pointer minMaxFilter = CalculatorType::New ();
    minMaxFilter->SetImage(inputImage);
    minMaxFilter->Compute();
    ImageProcessing::DefaultPixelType range = minMaxFilter->GetMaximum() - minMaxFilter->GetMinimum();

    //set up kmeans filter
    typedef itk::ScalarImageKmeansImageFilter< ImageProcessing::DefaultImageType, ImageProcessing::DefaultImageType > KMeansType;
    KMeansType::Pointer kMeans = KMeansType::New();
    kMeans->SetInput(inputImage);

    //start with evenly spaced class means
    ImageProcessing::DefaultPixelType meanIncrement = range / m_Classes;
    ImageProcessing::DefaultPixelType mean = range / (2 * m_Classes);
    for(int i = 0; i < m_Classes; i++)
    {
      kMeans->AddClassWithInitialMean(mean);
      mean = mean + meanIncrement;
    }

    ITKUtilitiesType::SetITKFilterOutput(kMeans->GetOutput(), m_NewCellArrayPtr.lock());
    kMeans->Update();
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
AbstractFilter::Pointer KMeans::newFilterInstance(bool copyFilterParameters)
{
  /*
  * write code to optionally copy the filter parameters from the current filter into the new instance
  */
  KMeans::Pointer filter = KMeans::New();
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
const QString KMeans::getCompiledLibraryName()
{return ImageProcessing::ImageProcessingBaseName;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString KMeans::getGroupName()
{return "ImageProcessing";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString KMeans::getSubGroupName()
{return "Misc";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString KMeans::getHumanLabel()
{return "K Means Filter";}

