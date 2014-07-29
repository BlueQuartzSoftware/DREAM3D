/*
 * Your License or Copyright Information can go here
 */
#if (_MSC_VER)
#define _SCL_SECURE_NO_WARNINGS
#endif

#include "ReadImage.h"

#include <string>

//image reading
#include "itkImageIOBase.h"
#include "itkImageIOFactory.h"
#include "itkImageFileReader.h"
#include "itkRGBPixel.h"
#include "itkRGBAPixel.h"
#include "itkVectorImage.h"

// ImageProcessing Plugin
#include "ITKUtilities.h"

/**
 * @brief This is a private implementation for the filter that handles the actual algorithm implementation details
 * for us like figuring out if we can use this private implementation with the data array that is assigned.
 */
template<typename PixelType>
class ReadImagePrivate
{
  public:
    typedef DataArray<PixelType> DataArrayType;

    ReadImagePrivate() {}
    virtual ~ReadImagePrivate() {}

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
    void static Execute(ReadImage* filter, QString inputFile, IDataArray::Pointer outputIDataArray, VolumeDataContainer* m, QString attrMatName)
    {
      typename DataArrayType::Pointer outputDataPtr = boost::dynamic_pointer_cast<DataArrayType>(outputIDataArray);

      //convert arrays to correct type
      PixelType* outputData = static_cast<PixelType*>(outputDataPtr->getPointer(0));
      size_t numVoxels = outputDataPtr->getNumberOfTuples();

      //read image meta data and get pixel type
      itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(inputFile.toLocal8Bit().constData(), itk::ImageIOFactory::ReadMode);
      imageIO->SetFileName(inputFile.toLocal8Bit().data());
      imageIO->ReadImageInformation();
      itk::ImageIOBase::IOPixelType pixelType = imageIO->GetPixelType();

      itk::ProcessObject::Pointer readerObject;

      //read based on pixel type
      switch(pixelType)
      {
        case itk::ImageIOBase::SCALAR:
        {
          typedef itk::Image<PixelType, ImageProcessing::ImageDimension> ImageType;
          typedef itk::ImageFileReader<ImageType> ReaderType;
          typename ReaderType::Pointer reader = ReaderType::New();
          reader->SetFileName(inputFile.toLocal8Bit().constData());
          reader->GetOutput()->GetPixelContainer()->SetImportPointer(outputData, numVoxels, false);
          readerObject=reader;
        }
        break;

        case itk::ImageIOBase::RGB:
        {
          typedef itk::Image<itk::RGBPixel<PixelType>, ImageProcessing::ImageDimension> ImageType;
          typedef itk::ImageFileReader<ImageType> ReaderType;
          typename ReaderType::Pointer reader = ReaderType::New();
          reader->SetFileName(inputFile.toLocal8Bit().constData());
          reader->GetOutput()->GetPixelContainer()->SetImportPointer(reinterpret_cast<itk::RGBPixel<PixelType>*>(outputData), numVoxels, false);
          readerObject=reader;
        }
        break;

        case itk::ImageIOBase::RGBA:
        {
          typedef itk::Image<itk::RGBAPixel<PixelType>, ImageProcessing::ImageDimension> ImageType;
          typedef itk::ImageFileReader<ImageType> ReaderType;
          typename ReaderType::Pointer reader = ReaderType::New();
          reader->SetFileName(inputFile.toLocal8Bit().constData());
          reader->GetOutput()->GetPixelContainer()->SetImportPointer(reinterpret_cast<itk::RGBAPixel<PixelType>*>(outputData), numVoxels, false);
          readerObject=reader;
        }
        break;
        /**
        case itk::ImageIOBase::FIXEDARRAY:
          {
            typedef itk::VectorImage<PixelType>, ImageProcessing::ImageDimension> ImageType;
            typedef itk::ImageFileReader<ImageType> ReaderType;
            typename ReaderType::Pointer reader = ReaderType::New();
            reader->SetFileName(inputFile.toLocal8Bit().constData());
            reader->GetOutput()->GetPixelContainer()->SetImportPointer(outputData, numVoxels, false);
            readerObject=reader;
          }break;
          */
        case itk::ImageIOBase::UNKNOWNPIXELTYPE:
        case itk::ImageIOBase::OFFSET:
        case itk::ImageIOBase::VECTOR:
        case itk::ImageIOBase::POINT:
        case itk::ImageIOBase::COVARIANTVECTOR:
        case itk::ImageIOBase::SYMMETRICSECONDRANKTENSOR:
        case itk::ImageIOBase::DIFFUSIONTENSOR3D:
        case itk::ImageIOBase::COMPLEX:
        case itk::ImageIOBase::FIXEDARRAY:
        case itk::ImageIOBase::MATRIX:
          break;
        default:
          filter->setErrorCondition(-2);
          QString message = QObject::tr("Unable to read image '%1'").arg(filter->getInputFileName());
          filter->notifyErrorMessage(filter->getHumanLabel(), message, filter->getErrorCondition());
          outputIDataArray->resize(0);
      }

      try
      {
        readerObject->Update();
      }
      catch( itk::ExceptionObject & err )
      {
        filter->setErrorCondition(-5);
        QString ss = QObject::tr("Failed to read image '%1': %2").arg(inputFile).arg(err.GetDescription());
        filter->notifyErrorMessage(filter->getHumanLabel(), ss, filter->getErrorCondition());
      }
    }
  private:
    ReadImagePrivate(const ReadImagePrivate&); // Copy Constructor Not Implemented
    void operator=(const ReadImagePrivate&); // Operator '=' Not Implemented
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadImage::ReadImage() :
  AbstractFilter(),
  m_InputFileName(""),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_ImageDataArrayName(DREAM3D::CellData::ImageData),
  m_ImageData(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadImage::~ReadImage()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadImage::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FileSystemFilterParameter::New("Input File", "InputFileName", FilterParameterWidgetType::InputFileWidget, getInputFileName(), false, "", "*.tif *.jpeg *.png *.bmp", "Image"));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Data Container Name", "DataContainerName", FilterParameterWidgetType::StringWidget, getDataContainerName(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Attribute Matrix Name", "CellAttributeMatrixName", FilterParameterWidgetType::StringWidget, getCellAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("ImageData", "ImageDataArrayName", FilterParameterWidgetType::StringWidget, getImageDataArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadImage::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setInputFileName( reader->readString( "InputFileName", getInputFileName() ) );
  setDataContainerName( reader->readString( "DataContainerName", getDataContainerName() ) );
  setCellAttributeMatrixName( reader->readString( "CellAttributeMatrixName", getCellAttributeMatrixName() ) );
  setImageDataArrayName( reader->readString( "ImageDataArrayName", getImageDataArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ReadImage::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(InputFileName)
  DREAM3D_FILTER_WRITE_PARAMETER(DataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(ImageDataArrayName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadImage::dataCheck()
{
  setErrorCondition(0);

  //check file name exists
  if(getInputFileName().isEmpty())
  {
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), "The input file name must be set before executing this filter.", getErrorCondition());
    return;
  }

  //read image metadata
  itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(getInputFileName().toLocal8Bit().constData(), itk::ImageIOFactory::ReadMode);
  if(NULL == imageIO)
  {
    setErrorCondition(-2);
    QString message = QObject::tr("Unable to read image '%1'").arg(getInputFileName());
    notifyErrorMessage(getHumanLabel(), message, getErrorCondition());
    return;
  }
  imageIO->SetFileName(getInputFileName().toLocal8Bit().data());
  imageIO->ReadImageInformation();

  //get size of image
  const size_t numDimensions = imageIO->GetNumberOfDimensions();
  int xdim = imageIO->GetDimensions(0);
  int ydim = imageIO->GetDimensions(1);
  int zdim = 1;
  if(3 != numDimensions)
  {
    if(2 == numDimensions)
    {
      //allow 2 dimensional images (as 3d image withs size 1 in the z direction)
    }
    else
    {
      QString message = QObject::tr("3 dimensional image required (slected image dimensions: %1)").arg(numDimensions);
      setErrorCondition(-2);
      notifyErrorMessage(getHumanLabel(), message, getErrorCondition());
      return;
    }
  }
  else
  {
    zdim = imageIO->GetDimensions(2);
  }

  //determine if container/attribute matrix already exist. if so check size compatibility
  DataArrayPath createdPath;
  VolumeDataContainer* m;
  AttributeMatrix::Pointer cellAttrMat;
  createdPath.update(getDataContainerName(), getCellAttributeMatrixName(), getImageDataArrayName() );

  m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(getErrorCondition() < 0) { return; }
  bool createAttributeMatrix = false;

  if( NULL == m) //datacontainer doesn't exist->create
  {
    m = getDataContainerArray()->createNonPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName());
    m->setDimensions(xdim, ydim, zdim);
    double zRes = 1;
    double zOrigin = 0;
    if(3 == numDimensions)
    {
      zRes = imageIO->GetSpacing(2);
      zOrigin = imageIO->GetOrigin(2);
    }
    m->setResolution(imageIO->GetSpacing(0), imageIO->GetSpacing(0), zRes);
    m->setOrigin(imageIO->GetOrigin(0), imageIO->GetOrigin(1), zOrigin);
    createAttributeMatrix = true;
    if(getErrorCondition() < 0) { return; }
  }
  else   //datacontainer exists, check if attribute matrix exists
  {
    if(m->doesAttributeMatrixExist(getCellAttributeMatrixName()))//attribute matrix exists, check compatibility
    {
      //get matrix
      cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), false);
      if(getErrorCondition() < 0) { return; }

      //check dimension compatibility
      QVector<size_t> tDims = cellAttrMat->getTupleDimensions();
      if(tDims[0] != xdim)
      {
        QString message = QObject::tr("The x size of '%1' (%2) does not match the x size of '%3' (%4)").arg(getInputFileName()).arg(xdim).arg(getDataContainerName() + "/" + getCellAttributeMatrixName()).arg(tDims[0]);
        setErrorCondition(-3);
        notifyErrorMessage(getHumanLabel(), message, getErrorCondition());
        return;
      }
      if(tDims[1] != ydim)
      {
        QString message = QObject::tr("The y size of '%1' (%2) does not match the x size of '%3' (%4)").arg(getInputFileName()).arg(ydim).arg(getDataContainerName() + "/" + getCellAttributeMatrixName()).arg(tDims[1]);
        setErrorCondition(-4);
        notifyErrorMessage(getHumanLabel(), message, getErrorCondition());
        return;
      }
      if(3 == numDimensions)
      {
        if(tDims[2] != zdim)
        {
          QString message = QObject::tr("The z size of '%1' (%2) does not match the x size of '%3' (%4)").arg(getInputFileName()).arg(zdim).arg(getDataContainerName() + "/" + getCellAttributeMatrixName()).arg(tDims[2]);
          setErrorCondition(-5);
          notifyErrorMessage(getHumanLabel(), message, getErrorCondition());
          return;
        }
      }
      else
      {
        if(tDims[2] != 1)
        {
          QString message = QObject::tr("The z size of '%1' (%2) does not match the x size of '%3' (1)").arg(getInputFileName()).arg(zdim).arg(getDataContainerName() + "/" + getCellAttributeMatrixName());
          setErrorCondition(-5);
          notifyErrorMessage(getHumanLabel(), message, getErrorCondition());
          return;
        }
      }
    }
    else//attribute matrix doesn't exist, create
    {
      createAttributeMatrix = true;
    }
  }

  //image/attribute matrix dimensions
  QVector<size_t> tDims(3, 0);
  tDims[0] = xdim;
  tDims[1] = ydim;
  tDims[2] = zdim;

  //create attribute matrix if needed
  if(createAttributeMatrix)
  {
    //create attribute matrix
    cellAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
    if(getErrorCondition() < 0) { return; }
  }

  //check pixel type (scalar, vector, etc) for support
  QVector<size_t> componentDims(1, 0);
  itk::ImageIOBase::IOPixelType pixelType = imageIO->GetPixelType();
  if(itk::ImageIOBase::SCALAR == pixelType)
  {
    componentDims[0] = 1;
  }
  else if(itk::ImageIOBase::RGB == pixelType)
  {
    componentDims[0] = 3;
    notifyWarningMessage(getHumanLabel(), "Warning: reading of rgb images is currenlty experimental (unstable behavoir may occur)", 0);
  }
  else if(itk::ImageIOBase::RGBA == pixelType)
  {
    componentDims[0] = 4;
    notifyWarningMessage(getHumanLabel(), "Warning: reading of rgba images is currenlty experimental (unstable behavoir may occur)", 0);
  }/**
  else if(itk::ImageIOBase::IOPixelType::FIXEDARRAY==pixelType)
  {
    componentDims[0]=imageIO->GetNumberOfComponents();
    notifyWarningMessage(getHumanLabel(), "Warning: reading of vector images is currenlty experimental (unstable behavoir may occur)", 0);
  }*/
  else
  {
    std::string pixelTypeName = itk::ImageIOBase::GetPixelTypeAsString(pixelType);
    QString message = QObject::tr("The pixel type of '%1' (%2) is unsupported").arg(getInputFileName()).arg(QString::fromStdString(pixelTypeName));
    setErrorCondition(-6);
    notifyErrorMessage(getHumanLabel(), message, getErrorCondition());
    return;
  }

  //get component type
  int type;
  itk::ImageIOBase::IOComponentType componentType = imageIO->GetComponentType();
  if(itk::ImageIOBase::CHAR == componentType)
  {
    type = TemplateConstants::Int8;
  }
  else if(itk::ImageIOBase::UCHAR == componentType)
  {
    type = TemplateConstants::UInt8;
  }
  else if(itk::ImageIOBase::SHORT == componentType)
  {
    type = TemplateConstants::Int16;
  }
  else if(itk::ImageIOBase::USHORT == componentType)
  {
    type = TemplateConstants::UInt16;
  }
  else if(itk::ImageIOBase::INT == componentType)
  {
    type = TemplateConstants::Int32;
  }
  else if(itk::ImageIOBase::UINT == componentType)
  {
    type = TemplateConstants::UInt32;
  }
  else if(itk::ImageIOBase::LONG == componentType)
  {
    type = TemplateConstants::Int64;
  }
  else if(itk::ImageIOBase::ULONG == componentType)
  {
    type = TemplateConstants::UInt64;
  }
  else if(itk::ImageIOBase::FLOAT == componentType)
  {
    type = TemplateConstants::Float;
  }
  else if(itk::ImageIOBase::DOUBLE == componentType)
  {
    type = TemplateConstants::Double;
  }
  else
  {
    std::string componentTypeName = itk::ImageIOBase::GetComponentTypeAsString(componentType);
    QString message = QObject::tr("The component type type of '%1' (%2) is unsupported").arg(getInputFileName()).arg(QString::fromStdString(componentTypeName));
    setErrorCondition(-6);
    notifyErrorMessage(getHumanLabel(), message, getErrorCondition());
    return;
  }

  if(getErrorCondition() < 0) { return; }

  //create array
  TEMPLATE_CREATE_NONPREREQ_ARRAY(ImageData, createdPath, componentDims, type);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadImage::preflight()
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
void ReadImage::execute()
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
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  //get input and output data
  IDataArray::Pointer imageData = m_ImageDataPtr.lock();
  std::string fileNameString = getInputFileName().toLocal8Bit().constData();
  const char* fileNameCStr = fileNameString.c_str();


  //execute type dependant portion using a Private Implementation that takes care of figuring out if
  // we can work on the correct type and actually handling the algorithm execution. We pass in "this" so
  // that the private implementation can get access to the current object to pass up status notifications,
  // progress or handle "cancel" if needed.
  if(ReadImagePrivate<int8_t>()(imageData))
  {
    ReadImagePrivate<int8_t>::Execute(this, getInputFileName(), imageData, m, getCellAttributeMatrixName());
  }
  else if(ReadImagePrivate<uint8_t>()(imageData) )
  {
    ReadImagePrivate<uint8_t>::Execute(this, getInputFileName(), imageData, m, getCellAttributeMatrixName());
  }
  else if(ReadImagePrivate<int16_t>()(imageData) )
  {
    ReadImagePrivate<int16_t>::Execute(this, getInputFileName(), imageData, m, getCellAttributeMatrixName());
  }
  else if(ReadImagePrivate<uint16_t>()(imageData) )
  {
    ReadImagePrivate<uint16_t>::Execute(this, getInputFileName(), imageData, m, getCellAttributeMatrixName());
  }
  else if(ReadImagePrivate<int32_t>()(imageData) )
  {
    ReadImagePrivate<int32_t>::Execute(this, getInputFileName(), imageData, m, getCellAttributeMatrixName());
  }
  else if(ReadImagePrivate<uint32_t>()(imageData) )
  {
    ReadImagePrivate<uint32_t>::Execute(this, getInputFileName(), imageData, m, getCellAttributeMatrixName());
  }
  else if(ReadImagePrivate<int64_t>()(imageData) )
  {
    ReadImagePrivate<int64_t>::Execute(this, getInputFileName(), imageData, m, getCellAttributeMatrixName());
  }
  else if(ReadImagePrivate<uint64_t>()(imageData) )
  {
    ReadImagePrivate<uint64_t>::Execute(this, getInputFileName(), imageData, m, getCellAttributeMatrixName());
  }
  else if(ReadImagePrivate<float>()(imageData) )
  {
    ReadImagePrivate<float>::Execute(this, getInputFileName(), imageData, m, getCellAttributeMatrixName());
  }
  else if(ReadImagePrivate<double>()(imageData) )
  {
    ReadImagePrivate<double>::Execute(this, getInputFileName(), imageData, m, getCellAttributeMatrixName());
  }
  else
  {
    setErrorCondition(-10001);
    ss = QObject::tr("A Supported DataArray type was not used for an input array.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  AttributeMatrix::Pointer attrMat = m->getAttributeMatrix(getCellAttributeMatrixName());
  attrMat->addAttributeArray(getImageDataArrayName(), imageData);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ReadImage::newFilterInstance(bool copyFilterParameters)
{
  ReadImage::Pointer filter = ReadImage::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadImage::getCompiledLibraryName()
{return ImageProcessing::ImageProcessingBaseName;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadImage::getGroupName()
{return "ImageProcessing";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadImage::getSubGroupName()
{return "IO";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadImage::getHumanLabel()
{return "Read Image";}

