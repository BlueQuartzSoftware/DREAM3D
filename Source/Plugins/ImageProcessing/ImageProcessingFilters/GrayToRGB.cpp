/*
 * Your License or Copyright Information can go here
 */
#if (_MSC_VER)
#define _SCL_SECURE_NO_WARNINGS
#endif

#include "GrayToRGB.h"

#include <string>

//thresholding filter
#include "itkComposeImageFilter.h"

// ImageProcessing Plugin
#include "ITKUtilities.h"


/**
 * @brief This is a private implementation for the filter that handles the actual algorithm implementation details
 * for us like figuring out if we can use this private implementation with the data array that is assigned.
 */
template<typename PixelType>
class GrayToRGBPrivate
{
  public:
    typedef DataArray<PixelType> DataArrayType;

    GrayToRGBPrivate() {}
    virtual ~GrayToRGBPrivate() {}

    // -----------------------------------------------------------------------------
    // Determine if this is the proper type of an array to downcast from the IDataArray
    // -----------------------------------------------------------------------------
    bool operator()(IDataArray::Pointer p)
    {
      return (boost::dynamic_pointer_cast<DataArrayType>(p).get() != NULL);
    }

    // -----------------------------------------------------------------------------
    // This is the actual templated algorithm
    // -----------------------------------------------------------------------------
    void static Execute(GrayToRGB* filter, IDataArray::Pointer redInputIDataArray, IDataArray::Pointer greenInputIDataArray, IDataArray::Pointer blueInputIDataArray, IDataArray::Pointer outputIDataArray, VolumeDataContainer* m, QString attrMatName)
    {
      typename DataArrayType::Pointer redInputDataPtr = boost::dynamic_pointer_cast<DataArrayType>(redInputIDataArray);
      typename DataArrayType::Pointer greenInputDataPtr = boost::dynamic_pointer_cast<DataArrayType>(greenInputIDataArray);
      typename DataArrayType::Pointer blueInputDataPtr = boost::dynamic_pointer_cast<DataArrayType>(blueInputIDataArray);
      typename DataArrayType::Pointer outputDataPtr = boost::dynamic_pointer_cast<DataArrayType>(outputIDataArray);

      //convert arrays to correct type
      PixelType* redData = static_cast<PixelType*>(redInputDataPtr->getPointer(0));
      PixelType* greenData = static_cast<PixelType*>(greenInputDataPtr->getPointer(0));
      PixelType* blueData = static_cast<PixelType*>(blueInputDataPtr->getPointer(0));
      PixelType* outputData = static_cast<PixelType*>(outputDataPtr->getPointer(0));

      size_t numVoxels = redInputDataPtr->getNumberOfTuples();

      typedef ITKUtilities<PixelType> ITKUtilitiesType;

      //wrap inputs as itk images
      typedef itk::Image<PixelType, ImageProcessing::ImageDimension> ImageType;
      typename ImageType::Pointer redImage = ITKUtilitiesType::Dream3DtoITK(m, attrMatName, redData);
      typename ImageType::Pointer greenImage = ITKUtilitiesType::Dream3DtoITK(m, attrMatName, greenData);
      typename ImageType::Pointer blueImage = ITKUtilitiesType::Dream3DtoITK(m, attrMatName, blueData);

      //define threshold filters
      typedef itk::ComposeImageFilter<typename ITKUtilitiesType::ScalarImageType, typename ITKUtilitiesType::RGBImageType> ComposeRGBType;

      //threshold
      typename ComposeRGBType::Pointer composeRGB = ComposeRGBType::New();
      composeRGB->SetInput(0, redImage);
      composeRGB->SetInput(1, greenImage);
      composeRGB->SetInput(2, blueImage);
      composeRGB->GetOutput()->GetPixelContainer()->SetImportPointer(reinterpret_cast<typename ITKUtilitiesType::RGBImageType::PixelType*>(outputData), numVoxels, false);
      composeRGB->Update();
    }
  private:
    GrayToRGBPrivate(const GrayToRGBPrivate&); // Copy Constructor Not Implemented
    void operator=(const GrayToRGBPrivate&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrayToRGB::GrayToRGB() :
  AbstractFilter(),
  m_RedArrayPath("", "", ""),
  m_BlueArrayPath("", "", ""),
  m_GreenArrayPath("", "", ""),
  m_NewCellArrayName(""),
  m_Red(NULL),
  m_Green(NULL),
  m_Blue(NULL),
  m_NewCellArray(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GrayToRGB::~GrayToRGB()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrayToRGB::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Red Channel", "RedArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getRedArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Green Channel", "GreenArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getGreenArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Blue Channel", "BlueArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getBlueArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Created Array Name", "NewCellArrayName", FilterParameterWidgetType::StringWidget, getNewCellArrayName(), false, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrayToRGB::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setRedArrayPath( reader->readDataArrayPath( "RedArrayPath", getRedArrayPath() ) );
  setGreenArrayPath( reader->readDataArrayPath( "GreenArrayPath", getGreenArrayPath() ) );
  setBlueArrayPath( reader->readDataArrayPath( "BlueArrayPath", getBlueArrayPath() ) );
  setNewCellArrayName( reader->readString( "NewCellArrayName", getNewCellArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int GrayToRGB::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(RedArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(GreenArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(BlueArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(NewCellArrayName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrayToRGB::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  //check for required arrays
  QVector<size_t> dims(1, 1);
  TEMPLATE_GET_PREREQ_ARRAY(Red, getRedArrayPath(), dims)
  TEMPLATE_GET_PREREQ_ARRAY(Green, getGreenArrayPath(), dims)
  TEMPLATE_GET_PREREQ_ARRAY(Blue, getBlueArrayPath(), dims)

  //configured created name / location
  tempPath.update(getRedArrayPath().getDataContainerName(), getRedArrayPath().getAttributeMatrixName(), getNewCellArrayName() );

  //get types of array
  QString redTypeName = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getRedArrayPath().getDataContainerName())->getAttributeMatrix(getRedArrayPath().getAttributeMatrixName())->getAttributeArray(getRedArrayPath().getDataArrayName())->getTypeAsString();;
  QString greenTypeName = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getGreenArrayPath().getDataContainerName())->getAttributeMatrix(getGreenArrayPath().getAttributeMatrixName())->getAttributeArray(getGreenArrayPath().getDataArrayName())->getTypeAsString();;
  QString blueTypeName = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getBlueArrayPath().getDataContainerName())->getAttributeMatrix(getBlueArrayPath().getAttributeMatrixName())->getAttributeArray(getBlueArrayPath().getDataArrayName())->getTypeAsString();;
  int redType = TemplateUtilities::getTypeFromTypeName(redTypeName);
  int greenType = TemplateUtilities::getTypeFromTypeName(greenTypeName);
  int blueType = TemplateUtilities::getTypeFromTypeName(blueTypeName);

  if(redType!=greenType)
  {
    QString message = QObject::tr("The red array '%1' and green array '%2' are of different types (%3 / %4)").arg(getRedArrayPath().getDataArrayName()).arg(getGreenArrayPath().getDataArrayName()).arg(redTypeName).arg(greenTypeName);
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), message,getErrorCondition());
    return;
  }
  if(redType!=blueType)
  {
    QString message = QObject::tr("The red array '%1' and blue array '%2' are of different types (%3 / %4)").arg(getRedArrayPath().getDataArrayName()).arg(getBlueArrayPath().getDataArrayName()).arg(redTypeName).arg(blueTypeName);
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), message,getErrorCondition());
    return;
  }

  //create new array of same type
  dims[0]=3;
  TEMPLATE_CREATE_NONPREREQ_ARRAY(NewCellArray, tempPath, dims, redType);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GrayToRGB::preflight()
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
void GrayToRGB::execute()
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

  //get volume container
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getRedArrayPath().getDataContainerName());
  QString attrMatName = getRedArrayPath().getAttributeMatrixName();

  //get input and output data
  IDataArray::Pointer redData = m_RedPtr.lock();
  IDataArray::Pointer greenData = m_GreenPtr.lock();
  IDataArray::Pointer blueData = m_BluePtr.lock();
  IDataArray::Pointer outputData = m_NewCellArrayPtr.lock();

  //execute type dependant portion using a Private Implementation that takes care of figuring out if
  // we can work on the correct type and actually handling the algorithm execution. We pass in "this" so
  // that the private implementation can get access to the current object to pass up status notifications,
  // progress or handle "cancel" if needed.
  if(GrayToRGBPrivate<int8_t>()(redData))
  {
    GrayToRGBPrivate<int8_t>::Execute(this, redData, greenData, blueData, outputData, m, attrMatName);
  }
  else if(GrayToRGBPrivate<uint8_t>()(redData) )
  {
    GrayToRGBPrivate<uint8_t>::Execute(this, redData, greenData, blueData, outputData, m, attrMatName);
  }
  else if(GrayToRGBPrivate<int16_t>()(redData) )
  {
    GrayToRGBPrivate<int16_t>::Execute(this, redData, greenData, blueData, outputData, m, attrMatName);
  }
  else if(GrayToRGBPrivate<uint16_t>()(redData) )
  {
    GrayToRGBPrivate<uint16_t>::Execute(this, redData, greenData, blueData, outputData, m, attrMatName);
  }
  else if(GrayToRGBPrivate<int32_t>()(redData) )
  {
    GrayToRGBPrivate<int32_t>::Execute(this, redData, greenData, blueData, outputData, m, attrMatName);
  }
  else if(GrayToRGBPrivate<uint32_t>()(redData) )
  {
    GrayToRGBPrivate<uint32_t>::Execute(this, redData, greenData, blueData, outputData, m, attrMatName);
  }
  else if(GrayToRGBPrivate<int64_t>()(redData) )
  {
    GrayToRGBPrivate<int64_t>::Execute(this, redData, greenData, blueData, outputData, m, attrMatName);
  }
  else if(GrayToRGBPrivate<uint64_t>()(redData) )
  {
    GrayToRGBPrivate<uint64_t>::Execute(this, redData, greenData, blueData, outputData, m, attrMatName);
  }
  else if(GrayToRGBPrivate<float>()(redData) )
  {
    GrayToRGBPrivate<float>::Execute(this, redData, greenData, blueData, outputData, m, attrMatName);
  }
  else if(GrayToRGBPrivate<double>()(redData) )
  {
    GrayToRGBPrivate<double>::Execute(this, redData, greenData, blueData, outputData, m, attrMatName);
  }
  else
  {
    setErrorCondition(-10001);
    ss = QObject::tr("A Supported DataArray type was not used for an input array.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  //array name changing/cleanup
  AttributeMatrix::Pointer attrMat = m->getAttributeMatrix(m_RedArrayPath.getAttributeMatrixName());
  attrMat->addAttributeArray(getNewCellArrayName(), outputData);


  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer GrayToRGB::newFilterInstance(bool copyFilterParameters)
{
  GrayToRGB::Pointer filter = GrayToRGB::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GrayToRGB::getCompiledLibraryName()
{return ImageProcessing::ImageProcessingBaseName;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GrayToRGB::getGroupName()
{return "ImageProcessing";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GrayToRGB::getSubGroupName()
{return "Misc";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString GrayToRGB::getHumanLabel()
{return "Gray to RGB (Merge Channels)";}

