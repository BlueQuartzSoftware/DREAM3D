/*
 * Your License or Copyright Information can go here
 */
#if (_MSC_VER)
#define _SCL_SECURE_NO_WARNINGS
#endif

#include "WriteImage.h"

#include <QtCore/QString>

#include "ItkBridge.h"
#include "itkImageFileWriter.h"
#include "itkRGBPixel.h"
#include "itkRGBAPixel.h"
#include "itkVectorImage.h"

/**
 * @brief This is a private implementation for the filter that handles the actual algorithm implementation details
 * for us like figuring out if we can use this private implementation with the data array that is assigned.
 */
template<typename TInputType>
class WriteImagePrivate
{
  public:
    typedef DataArray<TInputType> DataArrayType;

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
    void static Execute(WriteImage* filter, VolumeDataContainer* m, QString attrMatName, IDataArray::Pointer inputDataArray, QString outputFile)
    {
      typename DataArrayType::Pointer inputDataPtr = boost::dynamic_pointer_cast<DataArrayType>(inputDataArray);

      // Get a Raw Pointer to the data
      TInputType* inputData = inputDataPtr->getPointer(0);
      //size_t numVoxels = inputDataPtr->getNumberOfTuples();

      //get utilities


      //get the total number of components of the data and switch accordingly
      int numComp = inputDataPtr->getNumberOfComponents();

      itk::ProcessObject::Pointer writerObject;

      if(1 == numComp) //scalar image
      {
        typedef ItkBridge<TInputType> ItkBridgeType;
        //define types and wrap input image
        typedef typename ItkBridgeType::ScalarImageType ImageType;
        typename ImageType::Pointer inputImage = ItkBridgeType::CreateItkWrapperForDataPointer(m, attrMatName, inputData);

        //create writer and execute
        typedef itk::ImageFileWriter<ImageType> WriterType;
        typename WriterType::Pointer writer = WriterType::New();
        writer->SetFileName( outputFile.toLatin1().constData() );
        writer->SetInput( inputImage );
        writerObject = writer;
      }
      else if(3 == numComp)//rgb image
      {
        //define types and wrap input image
        {
#if 0
          typedef ItkBridge2<TInputType, TInputType>                            ItkBridge2Type;
          typedef typename ItkBridge2Type::RGBImageType                         ImageType;
          typedef typename itk::ImageFileWriter<ImageType>                      WriterType;
          typename ImageType::Pointer inputImage;

          typedef itk::RGBPixel<TInputType>                                     ItkRgbPixelType;
          typedef ItkBridge2<TInputType, ItkRgbPixelType>                       ItkBridge2Type;
          typedef typename ItkBridge2Type::ItkImportImageFilterPointerType      ItkImportImageFilterPointerType;
          ItkImportImageFilterPointerType itkImportFilter = ItkBridge2Type::Dream3DtoITKImportFilter(m, attrMatName, inputData);
          inputImage = itkImportFilter->GetOutput();
#endif
          typedef CreateItkWrapperForDataPointer<TInputType> WrapperType;
          typedef typename WrapperType::ScalarImageType                             WrapperScalarImageType;
          typedef typename WrapperType::ScalarImagePointerType                      WrapperScalarImagePointerType;

          WrapperScalarImagePointerType inputImage = CreateItkWrapperForDataPointer<TInputType>()(m, attrMatName, inputData);

          typedef typename itk::ImageFileWriter<WrapperScalarImageType>                      WriterType;

          typename WriterType::Pointer writer = WriterType::New();
          writer->SetFileName( outputFile.toLatin1().constData() );
          writer->SetInput( inputImage );
          writerObject = writer;


        }
#if 0
        {
          typedef Dream3DToItkImageConversion<TInputType, TInputType, itk::RGBPixel<TInputType> > Dream3DToItkImageConversionType;

          Dream3DToItkImageConversionType importFunctor;
          typename Dream3DToItkImageConversionType::ItkImportImageFilterPointerType itkImportFilter = importFunctor(m, attrMatName, inputData);
          typename Dream3DToItkImageConversionType::ItkImageOutPointerType inputImage = itkImportFilter->GetOutput();


          //create writer and execute
          typename WriterType::Pointer writer = WriterType::New();
          writer->SetFileName( outputFile.toLatin1().constData() );
          writer->SetInput( inputImage );
          writerObject = writer;


        }

#endif


      }
      else if(4 == numComp)//rgba image
      {
        typedef ItkBridge<TInputType>                            ItkBridgeType;

        //define types and wrap input image
        typedef typename ItkBridgeType::RGBAImageType ImageType;
        typedef typename itk::ImageFileWriter<ImageType> WriterType;
        //     typename ImageType::Pointer inputImage = ItkBridgeType::template Dream3DtoITKTemplate<ImageType>(m, attrMatName, inputData);
        typename ImageType::Pointer inputImage = ItkBridgeType::template Dream3DtoITKImportFilter<typename ImageType::PixelType>(m, attrMatName, inputData)->GetOutput();

        //create writer and execute
        typename WriterType::Pointer writer = WriterType::New();
        writer->SetFileName( outputFile.toLatin1().constData() );
        writer->SetInput( inputImage );
        writerObject = writer;
      } /** else//vector image
      {
        //define types and wrap input image
        typedef itk::Image<itk::FixedArray<TInputType, numComp> >, ImageProcessing::ImageDimension> ImageType;
        typedef itk::ImageFileWriter<ImageType> WriterType;
        ImageType::Pointer inputImage = ItkBridgeType::Dream3DtoITKTemplate<ImageType>(m, attrMatName, inputData);

        //create writer and execute
        typename WriterType::Pointer writer = WriterType::New();
        writer->SetFileName( outputFile.toLatin1().constData() );
        writer->SetInput( inputImage );
        writerObject = writer;
      }*/

      try
      {
        writerObject->Update();
      }
      catch( itk::ExceptionObject & err )
      {
        filter->setErrorCondition(-5);
        QString ss = QObject::tr("Failed to write image. Error Message returned from ITK:\n   %1").arg(err.GetDescription());
        filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
      }
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
  QVector<size_t> compDims;
  m_SelectedCellArrayPtr = TemplateHelper::GetPrereqArrayFromPath<AbstractFilter, VolumeDataContainer>()(this, getSelectedCellArrayPath(), compDims);
  if(NULL != m_SelectedCellArrayPtr.lock().get())
  {
    m_SelectedCellArray = m_SelectedCellArrayPtr.lock().get();
  }

  //make sure dims of selected array are appropriate
  if(1 == compDims.size())
  {
    if(1 == compDims[0]) //scalar
    {

    }
    else if (3 == compDims[0])//rgb
    {
      notifyWarningMessage(getHumanLabel(), "Warning: writing of rgb images is currenlty experimental (unstable behavoir may occur)", 0);
    }
    else if (4 == compDims[0])//rgba
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
    QString message = QObject::tr("The selected array '%1' has unsupported dimensionality (%2)").arg(m_SelectedCellArrayArrayName).arg(compDims.size());
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

