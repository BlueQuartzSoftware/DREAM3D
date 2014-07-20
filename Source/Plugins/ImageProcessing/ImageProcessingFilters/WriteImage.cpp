/*
 * Your License or Copyright Information can go here
 */

#include "WriteImage.h"

#include <QtCore/QString>

#include "ITKUtilities.h"
#include "itkImageFileWriter.h"
#include "itkRGBPixel.h"
#include "itkRGBAPixel.h"
#include "itkVectorImage.h"

/**
 * @brief This is a private implementation for the filter that handles the actual algorithm implementation details
 * for us like figuring out if we can use this private implementation with the data array that is assigned.
 */
template<typename PixelType>
class WriteImagePrivate
{
  public:
    typedef DataArray<PixelType> DataArrayType;

    WriteImagePrivate() {}
    virtual ~WriteImagePrivate() {}

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
    void static Execute(WriteImage* filter, VolumeDataContainer* m, QString attrMatName, IDataArray::Pointer intputIDataArray, QString outputFile)
    {
      typename DataArrayType::Pointer inputDataPtr = boost::dynamic_pointer_cast<DataArrayType>(intputIDataArray);

      //convert arrays to correct type
      PixelType* inputData = static_cast<PixelType*>(inputDataPtr->getPointer(0));
      size_t numVoxels = inputDataPtr->getNumberOfTuples();

      //get utilities
      typedef ITKUtilities<PixelType> ITKUtilitiesType;

      //get dimensions of image and switch accordingly
      QVector<size_t> dims = inputDataPtr->getComponentDimensions();
      int numComp = dims[0];

      if(1 == numComp) //scalar image
      {
        //define types and wrap input image
        typedef typename ITKUtilitiesType::ScalarImageType ImageType;
        typedef itk::ImageFileWriter<ImageType> WriterType;
        typename ImageType::Pointer inputImage = ITKUtilitiesType::Dream3DtoITK(m, attrMatName, inputData);

        //create writer and execute
        typename WriterType::Pointer writer = WriterType::New();
        writer->SetFileName( outputFile.toLatin1().constData() );
        writer->SetInput( inputImage );
        writer->Update();
      }
      else if(3 == numComp)//rgb image
      {
        //define types and wrap input image
        typedef typename ITKUtilitiesType::RGBImageType ImageType;
        typedef typename itk::ImageFileWriter<ImageType> WriterType;
        typename ImageType::Pointer inputImage = ITKUtilitiesType::template Dream3DtoITKTemplate<ImageType>(m, attrMatName, inputData);

        //create writer and execute
        typename WriterType::Pointer writer = WriterType::New();
        writer->SetFileName( outputFile.toLatin1().constData() );
        writer->SetInput( inputImage );
        writer->Update();
      }
      else if(4 == numComp)//rgba image
      {
        //define types and wrap input image
        typedef typename ITKUtilitiesType::RGBAImageType ImageType;
        typedef typename itk::ImageFileWriter<ImageType> WriterType;
        typename ImageType::Pointer inputImage = ITKUtilitiesType::template Dream3DtoITKTemplate<ImageType>(m, attrMatName, inputData);

        //create writer and execute
        typename WriterType::Pointer writer = WriterType::New();
        writer->SetFileName( outputFile.toLatin1().constData() );
        writer->SetInput( inputImage );
        writer->Update();
      } /** else//vector image
      {
        //define types and wrap input image
        typedef itk::Image<itk::FixedArray<PixelType, numComp> >, ImageProcessing::ImageDimension> ImageType;
        typedef itk::ImageFileWriter<ImageType> WriterType;
        ImageType::Pointer inputImage = ITKUtilitiesType::Dream3DtoITKTemplate<ImageType>(m, attrMatName, inputData);

        //create writer and execute
        typename WriterType::Pointer writer = WriterType::New();
        writer->SetFileName( outputFile.toLatin1().constData() );
        writer->SetInput( inputImage );
        writer->Update();
      }*/
    }
  private:
    WriteImagePrivate(const WriteImagePrivate&); // Copy Constructor Not Implemented
    void operator=(const WriteImagePrivate&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteImage::WriteImage() :
  AbstractFilter(),
  m_SelectedCellArrayPath("", "", ""),
  m_OutputFileName(""),
  m_SelectedCellArrayArrayName(""),
  m_SelectedCellArray(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteImage::~WriteImage()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteImage::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Array to Write", "SelectedCellArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedCellArrayPath(), false, ""));
  parameters.push_back(FileSystemFilterParameter::New("Output File Name", "OutputFileName", FilterParameterWidgetType::OutputFileWidget, getOutputFileName(), false, "", "*.tif", "TIFF"));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteImage::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedCellArrayPath( reader->readDataArrayPath( "SelectedCellArrayPath", getSelectedCellArrayPath() ) );
  setOutputFileName( reader->readString( "OutputFileName", getOutputFileName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int WriteImage::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue( "SelectedCellArrayPath", getSelectedCellArrayPath() );
  writer->writeValue( "OutputFileName", getOutputFileName() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteImage::dataCheck()
{
  setErrorCondition(0);

  //pass empty dimensions to allow any size
  QVector<size_t> dims = QVector<size_t>::QVector();
  TEMPLATE_GET_PREREQ_ARRAY(SelectedCellArray, getSelectedCellArrayPath(), dims)

  //make sure dims of selected array are appropriate
  if(1 == dims.size())
  {
    if(1 == dims[0]) //scalar
    {

    }
    else if (3 == dims[0])//rgb
    {
      notifyWarningMessage(getHumanLabel(), "Warning: writing of rgb images is currenlty experimental (unstable behavoir may occur)", 0);
    }
    else if (4 == dims[0])//rgba
    {
      notifyWarningMessage(getHumanLabel(), "Warning: writing of rgba images is currenlty experimental (unstable behavoir may occur)", 0);
    }
    else  //vector
    {
      //notifyWarningMessage(getHumanLabel(), "Warning: writing of vector images is currenlty experimental (unstable behavoir may occur)", 0);
      setErrorCondition(-102);
      notifyErrorMessage(getHumanLabel(), "Error: writing of vector images is currently not supported", getErrorCondition());
    }
  }
  else
  {
    QString message = QObject::tr("The selected array '%1' has unsupported dimensionality (%2)").arg(m_SelectedCellArrayArrayName).arg(dims.size());
    setErrorCondition(-101);
  }

  if(m_OutputFileName.isEmpty())
  {
    QString ss = QObject::tr("Need to enter output file name");
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteImage::preflight()
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
void WriteImage::execute()
{
  //int err = 0;
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getSelectedCellArrayPath().getDataContainerName());
  QString attrMatName = getSelectedCellArrayPath().getAttributeMatrixName();

  //get input data
  IDataArray::Pointer inputData = m_SelectedCellArrayPtr.lock();

  if(WriteImagePrivate<int8_t>()(inputData))
  {
    WriteImagePrivate<int8_t>::Execute(this, m, attrMatName, inputData, m_OutputFileName);
  }
  else if(WriteImagePrivate<uint8_t>()(inputData) )
  {
    WriteImagePrivate<uint8_t>::Execute(this, m, attrMatName, inputData, m_OutputFileName);
  }
  else if(WriteImagePrivate<int16_t>()(inputData) )
  {
    WriteImagePrivate<int16_t>::Execute(this, m, attrMatName, inputData, m_OutputFileName);
  }
  else if(WriteImagePrivate<uint16_t>()(inputData) )
  {
    WriteImagePrivate<uint16_t>::Execute(this, m, attrMatName, inputData, m_OutputFileName);
  }
  else if(WriteImagePrivate<int32_t>()(inputData) )
  {
    WriteImagePrivate<int32_t>::Execute(this, m, attrMatName, inputData, m_OutputFileName);
  }
  else if(WriteImagePrivate<uint32_t>()(inputData) )
  {
    WriteImagePrivate<uint32_t>::Execute(this, m, attrMatName, inputData, m_OutputFileName);
  }
  else if(WriteImagePrivate<int64_t>()(inputData) )
  {
    WriteImagePrivate<int64_t>::Execute(this, m, attrMatName, inputData, m_OutputFileName);
  }
  else if(WriteImagePrivate<uint64_t>()(inputData) )
  {
    WriteImagePrivate<uint64_t>::Execute(this, m, attrMatName, inputData, m_OutputFileName);
  }
  else if(WriteImagePrivate<float>()(inputData) )
  {
    WriteImagePrivate<float>::Execute(this, m, attrMatName, inputData, m_OutputFileName);
  }
  else if(WriteImagePrivate<double>()(inputData) )
  {
    WriteImagePrivate<double>::Execute(this, m, attrMatName, inputData, m_OutputFileName);
  }
  else
  {
    setErrorCondition(-10001);
    QString ss = QObject::tr("A Supported DataArray type was not used for an input array.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }



  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer WriteImage::newFilterInstance(bool copyFilterParameters)
{
  WriteImage::Pointer filter = WriteImage::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteImage::getCompiledLibraryName()
{return ImageProcessing::ImageProcessingBaseName;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteImage::getGroupName()
{return "ImageProcessing";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteImage::getSubGroupName()
{return "Misc";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteImage::getHumanLabel()
{return "Write Image from Array";}

