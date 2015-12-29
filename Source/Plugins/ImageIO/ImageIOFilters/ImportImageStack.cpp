/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ImportImageStack.h"

#include <QtGui/QImageReader>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/SIMPLibVersion.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersWriter.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/FileListInfoFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedChoicesFilterParameter.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Utilities/FilePathGenerator.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/RectGridGeom.h"

#include "ImageIO/ImageIOConstants.h"

// Include the MOC generated file for this class
#include "moc_ImportImageStack.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportImageStack::ImportImageStack() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::Defaults::ImageDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_BoundsFile(""),
  m_GeometryType(0),
  m_ImageDataArrayName(DREAM3D::CellData::ImageData)
{
  m_Origin.x = 0.0f;
  m_Origin.y = 0.0f;
  m_Origin.z = 0.0f;

  m_Resolution.x = 1.0f;
  m_Resolution.y = 1.0f;
  m_Resolution.z = 1.0f;

  m_InputFileListInfo.FileExtension = QString("tif");
  m_InputFileListInfo.StartIndex = 0;
  m_InputFileListInfo.EndIndex = 0;
  m_InputFileListInfo.PaddingDigits = 0;

  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportImageStack::~ImportImageStack()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportImageStack::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
  parameters.push_back(FileListInfoFilterParameter::New("Input File List", "InputFileListInfo", getInputFileListInfo(), FilterParameter::Parameter));
  {
    LinkedChoicesFilterParameter::Pointer parameter = LinkedChoicesFilterParameter::New();
    parameter->setHumanLabel("Geometry Type");
    parameter->setPropertyName("GeometryType");

    QVector<QString> choices;
    choices.push_back("Image");
    choices.push_back("Rectilinear Grid");
    parameter->setChoices(choices);
    QStringList linkedProps;
    linkedProps << "Origin" << "Resolution" << "BoundsFile";
    parameter->setLinkedProperties(linkedProps);
    parameter->setEditable(false);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  parameters.push_back(FloatVec3FilterParameter::New("Origin", "Origin", getOrigin(), FilterParameter::Parameter, 0));
  parameters.push_back(FloatVec3FilterParameter::New("Resolution", "Resolution", getResolution(), FilterParameter::Parameter, 0));
  parameters.push_back(InputFileFilterParameter::New("Bounds File", "BoundsFile", getBoundsFile(), FilterParameter::Parameter, "*.txt", "", 1));
  parameters.push_back(StringFilterParameter::New("Data Container", "DataContainerName", getDataContainerName(), FilterParameter::CreatedArray));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Cell Attribute Matrix", "CellAttributeMatrixName", getCellAttributeMatrixName(), FilterParameter::CreatedArray));
  parameters.push_back(StringFilterParameter::New("Image Data", "ImageDataArrayName", getImageDataArrayName(), FilterParameter::CreatedArray));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportImageStack::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName() ) );
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName() ) );
  setImageDataArrayName(reader->readString("ImageDataArrayName", getImageDataArrayName() ) );
  setInputFileListInfo( reader->readFileListInfo("InputFileListInfo", getInputFileListInfo() ) );
  setOrigin( reader->readFloatVec3("Origin", getOrigin()) );
  setResolution( reader->readFloatVec3("Resolution", getResolution()) );
  setGeometryType(reader->readValue("GeometryType", getGeometryType()));
  setBoundsFile(reader->readString("BoundsFile", getBoundsFile()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImportImageStack::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  SIMPL_FILTER_WRITE_PARAMETER(FilterVersion)
  SIMPL_FILTER_WRITE_PARAMETER(DataContainerName)
  SIMPL_FILTER_WRITE_PARAMETER(CellAttributeMatrixName)
  SIMPL_FILTER_WRITE_PARAMETER(ImageDataArrayName)
  SIMPL_FILTER_WRITE_PARAMETER(InputFileListInfo)
  SIMPL_FILTER_WRITE_PARAMETER(Origin)
  SIMPL_FILTER_WRITE_PARAMETER(Resolution)
  SIMPL_FILTER_WRITE_PARAMETER(GeometryType)
  SIMPL_FILTER_WRITE_PARAMETER(BoundsFile)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportImageStack::dataCheck()
{
  setErrorCondition(0);

  DataArrayPath tempPath;
  QString ss;

  if (m_InputFileListInfo.InputPath.isEmpty() == true)
  {
    ss = QObject::tr("The input directory must be set");
    setErrorCondition(-13);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0 || NULL == m.get()) { return; }

  if (m_GeometryType == 0)
  {
    ImageGeom::Pointer image = ImageGeom::CreateGeometry(DREAM3D::Geometry::ImageGeometry);
    m->setGeometry(image);
  }
  else if (m_GeometryType == 1)
  {
    RectGridGeom::Pointer rectGrid = RectGridGeom::CreateGeometry(DREAM3D::Geometry::RectGridGeometry);
    m->setGeometry(rectGrid);

    if (m_BoundsFile.isEmpty() == true)
    {
      ss = QObject::tr("The Bounds file must be set");
      setErrorCondition(-14);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
  }

  bool hasMissingFiles = false;
  bool orderAscending = false;

  if (m_InputFileListInfo.Ordering == 0) { orderAscending = true; }
  else if (m_InputFileListInfo.Ordering == 1) { orderAscending = false; }

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList = FilePathGenerator::GenerateFileList(m_InputFileListInfo.StartIndex,
                              m_InputFileListInfo.EndIndex, hasMissingFiles, orderAscending,
                              m_InputFileListInfo.InputPath, m_InputFileListInfo.FilePrefix,
                              m_InputFileListInfo.FileSuffix, m_InputFileListInfo.FileExtension,
                              m_InputFileListInfo.PaddingDigits);
  if (fileList.size() == 0)
  {
    ss.clear();
    QTextStream out(&ss);
    out <<" No files have been selected for import. Have you set the input directory and other values so that input files will be generated?\n";
    out << "InputPath: " << m_InputFileListInfo.InputPath << "\n";
    out << "FilePrefix: " << m_InputFileListInfo.FilePrefix << "\n";
    out << "FileSuffix: " << m_InputFileListInfo.FileSuffix << "\n";
    out << "FileExtension: " << m_InputFileListInfo.FileExtension << "\n";
    out << "PaddingDigits: " << m_InputFileListInfo.PaddingDigits << "\n";
    out << "StartIndex: " << m_InputFileListInfo.StartIndex << "\n";
    out << "EndIndex: " << m_InputFileListInfo.EndIndex << "\n";

    setErrorCondition(-11);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else
  {
    // If we have RGB or RGBA Images then we are going to have to change things a bit.
    // We should read the file and see what we have? Of course Qt is going to read it up into
    // an RGB array by default
    int32_t err = 0;
    QImageReader reader((fileList[0]));
    QSize imageDims = reader.size();
    int64_t dims[3] = { imageDims.width(), imageDims.height(), fileList.size() };
    /* Sanity check what we are trying to load to make sure it can fit in our address space.
     * Note that this does not guarantee the user has enough left, just that the
     * size of the volume can fit in the address space of the program
     */
#if   (CMP_SIZEOF_SSIZE_T==4)
    int64_t max = std::numeric_limits<size_t>::max();
#else
    int64_t max = std::numeric_limits<int64_t>::max();
#endif
    if (dims[0] * dims[1] * dims[2] > max)
    {
      err = -1;
      QString ss = QObject::tr("The total number of elements '%1' is greater than this program can hold").arg((dims[0] * dims[1] * dims[2]));
      setErrorCondition(err);
      notifyErrorMessage(getHumanLabel(), ss, err);
    }

    if (dims[0] > max || dims[1] > max || dims[2] > max)
    {
      err = -1;
      QString ss = QObject::tr("One of the dimensions is greater than the max index for this sysem"
                               " dim[0]=%1  dim[1]=%2  dim[2]=%3").arg(dims[0]).arg(dims[1]).arg(dims[2]);
      setErrorCondition(err);
      notifyErrorMessage(getHumanLabel(), ss, err);
    }
    /* ************ End Sanity Check *************************** */

    if (m_GeometryType == 0)
    {
      m->getGeometryAs<ImageGeom>()->setDimensions(static_cast<size_t>(dims[0]), static_cast<size_t>(dims[1]), static_cast<size_t>(dims[2]));
      m->getGeometryAs<ImageGeom>()->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
      m->getGeometryAs<ImageGeom>()->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);
    }
    else if (m_GeometryType == 1)
    {
      m->getGeometryAs<RectGridGeom>()->setDimensions(static_cast<size_t>(dims[0]), static_cast<size_t>(dims[1]), static_cast<size_t>(dims[2]));
      if (!m_BoundsFile.isEmpty()) { err = readBounds(); }
      if (err < 0) { setErrorCondition(err); }
    }

    QVector<size_t> tDims(3, 0);
    for (int32_t i = 0; i < 3; i++)
    {
      tDims[i] = dims[i];
    }
    m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);

    size_t pixelBytes = 0;

    QImage::Format format = reader.imageFormat();
    switch(format)
    {
       case QImage::Format_Indexed8:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
      case QImage::Format_Grayscale8:
#endif
        pixelBytes = 1;
        break;
      case QImage::Format_RGB32:
      case QImage::Format_ARGB32:
        pixelBytes = 4;
        break;
      default:
         pixelBytes = 0;
    }

    if(pixelBytes == 0)
    {
      ss = QObject::tr("Image format is of unsupported type (QImage::Format=%1). Imported images must be either grayscale, RGB, or ARGB").arg(format);
      setErrorCondition(-4400);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    QVector<size_t> cDims(1, pixelBytes);
    // This would be for a gray scale image
    tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), getImageDataArrayName() );
    m_ImageDataPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter, uint8_t>(this, tempPath, 0, cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_ImageDataPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_ImageData = m_ImageDataPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportImageStack::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportImageStack::execute()
{
  setErrorCondition(0);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());

  if (m_GeometryType == 0)
  {
    m->getGeometryAs<ImageGeom>()->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
    m->getGeometryAs<ImageGeom>()->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);
  }
  else if (m_GeometryType == 1)
  {
    int err = readBounds();
    if (err < 0) return;
  }
  UInt8ArrayType::Pointer data = UInt8ArrayType::NullPointer();

  uint8_t* imagePtr = NULL;

  int64_t z = m_InputFileListInfo.StartIndex;
  int64_t zSpot = 0;
  size_t pixelBytes = 0; // MUST BE Defined & Initialized out here.
  bool hasMissingFiles = false;
  bool orderAscending = false;

  if (m_InputFileListInfo.Ordering == 0) { orderAscending = true; }
  else if (m_InputFileListInfo.Ordering == 1) { orderAscending = false; }

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList = FilePathGenerator::GenerateFileList(m_InputFileListInfo.StartIndex,
                              m_InputFileListInfo.EndIndex, hasMissingFiles, orderAscending,
                              m_InputFileListInfo.InputPath, m_InputFileListInfo.FilePrefix,
                              m_InputFileListInfo.FileSuffix, m_InputFileListInfo.FileExtension,
                              m_InputFileListInfo.PaddingDigits);
  if (fileList.size() == 0)
  {
    QString ss = QObject::tr("No files have been selected for import");
    setErrorCondition(-11);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  for (QVector<QString>::iterator filepath = fileList.begin(); filepath != fileList.end(); ++filepath)
  {
    QString imageFName = *filepath;
    QString ss = QObject::tr("Importing file %1").arg(imageFName);
    notifyStatusMessage(getMessagePrefix(), getHumanLabel(), ss);
    QImageReader reader(imageFName);
    QImage image(imageFName);
    if (image.isNull() == true)
    {
      setErrorCondition(-14000);
      notifyErrorMessage(getHumanLabel(), "Failed to load image file", getErrorCondition());
      return;
    }
#if defined (CMP_WORDS_BIGENDIAN)
#error
#else
    // We need to convert from Little Endian based ARGB to a physical RGB layout
    image = image.rgbSwapped();
#endif
    size_t height = static_cast<size_t>(image.height());
    size_t width = static_cast<size_t>(image.width());
    size_t totalPixels = width * height;



    // This is the first image so we need to create our block of data to store the data
    if (z == m_InputFileListInfo.StartIndex)
    {
      if (m_GeometryType == 0) { m->getGeometryAs<ImageGeom>()->setDimensions(width, height, fileList.size()); }
      else if (m_GeometryType == 1) { m->getGeometryAs<RectGridGeom>()->setDimensions(width, height, fileList.size()); }

      const QImage::Format format = reader.imageFormat();
      switch(format)
      {
        case QImage::Format_Indexed8:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
      case QImage::Format_Grayscale8:
#endif
          pixelBytes = 1;
          break;
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32:
          pixelBytes = 4;
          break;
        default:
           pixelBytes = 0;
      }

      if(pixelBytes == 0)
      {
        ss = QObject::tr("Image format is of unsupported type (QImage::Format=%1). Imported images must be either grayscale, RGB, or ARGB").arg(format);
        setErrorCondition(-4400);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return;
      }

      QVector<size_t> cDims(1, pixelBytes);

      data = UInt8ArrayType::CreateArray(size_t(fileList.size()) * height * width, cDims, m_ImageDataArrayName);
      data->initializeWithValue(128);
    }

    // Get the current position in the array to copy the image into
    zSpot = (z - m_InputFileListInfo.StartIndex);
    for (size_t i = 0; i < height; ++i)
    {
      imagePtr = data->getPointer( (zSpot) * totalPixels * pixelBytes + i * (width * pixelBytes));
      uint8_t* source = image.scanLine(i);
      ::memcpy(imagePtr, source, width * pixelBytes);
    }

    ++z;
    if (getCancel() == true) { return; }
  }

  m->getAttributeMatrix(getCellAttributeMatrixName())->addAttributeArray(data->getName(), data);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImportImageStack::readBounds()
{
  size_t dims[3];
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  m->getGeometryAs<RectGridGeom>()->getDimensions(dims);

  FloatArrayType::Pointer xbounds = FloatArrayType::CreateArray(dims[0] + 1, DREAM3D::Geometry::xBoundsList);
  FloatArrayType::Pointer ybounds = FloatArrayType::CreateArray(dims[1] + 1, DREAM3D::Geometry::yBoundsList);
  FloatArrayType::Pointer zbounds = FloatArrayType::CreateArray(dims[2] + 1, DREAM3D::Geometry::zBoundsList);
  float* xbnds = xbounds->getPointer(0);
  float* ybnds = ybounds->getPointer(0);
  float* zbnds = zbounds->getPointer(0);

  QFile inFile;
  inFile.setFileName(m_BoundsFile.toLatin1().data());
  if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString ss = QObject::tr("Bounds Input file could not be opened: %1").arg(getBoundsFile());
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }

  size_t count = 0;

  QByteArray buf;
  QList<QByteArray> tokens; /* vector to store the split data */
  bool ok = false;
  QString word;

  //Read header until we get to the line that should have the format: XPOINTS number
  bool header = true;
  while (header == true)
  {
    buf = inFile.readLine();
    if (buf.startsWith("#") == false) { header = false; }
  }
  buf = buf.trimmed();
  buf = buf.simplified();
  tokens = buf.split(' ');
  word = tokens.at(0);
  if (word.compare("X_COORDINATES") != 0)
  {
    QString ss = QObject::tr("X_COORDINATES keyword not found in proper location in: %1.").arg(m_BoundsFile);
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }
  if (tokens.size() != 2)
  {
    QString ss = QObject::tr("X_COORDINATES keyword line not properly formatted in: %1.").arg(m_BoundsFile);
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }
  size_t numXBounds = tokens[1].toInt(&ok, 10);
  if (numXBounds != xbounds->getNumberOfTuples())
  {
    QString ss = QObject::tr("The number of X bounds in %1 does not match the x dimension of the geometry.").arg(m_BoundsFile);
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }
  count = 0;
  while (count < numXBounds)
  {
    if (inFile.atEnd())
    {
      QString ss = QObject::tr(" %1..").arg(m_BoundsFile);
      setErrorCondition(-100);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return -1;
    }
    buf = inFile.readLine();
    buf = buf.trimmed();
    buf = buf.simplified();
    tokens = buf.split(' ');
    for (size_t iter2 = 0; iter2 < tokens.size(); iter2++)
    {
      xbnds[count] = tokens[iter2].toFloat(&ok);
      if (!ok)
      {
        QString ss = QObject::tr("Could not read X coordinte number %1. Could not interpret as float value.").arg(count+1);
        setErrorCondition(-100);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return -1;
      }
      count++;
    }
  }

  //Read the line that should have the format: YPOINTS number
  buf = inFile.readLine();
  buf = buf.trimmed();
  buf = buf.simplified();
  tokens = buf.split(' ');
  word = tokens.at(0);
  if (word.compare("Y_COORDINATES") != 0)
  {
    QString ss = QObject::tr("Y_COORDINATES keyword not found in proper location in: %1.").arg(m_BoundsFile);
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }
  if (tokens.size() != 2)
  {
    QString ss = QObject::tr("Y_COORDINATES keyword line not properly formatted in: %1.").arg(m_BoundsFile);
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }
  size_t numYBounds = tokens[1].toInt(&ok, 10);
  if (numYBounds != ybounds->getNumberOfTuples())
  {
    QString ss = QObject::tr("The number of Y bounds in %1 does not match the y dimension of the geometry.").arg(m_BoundsFile);
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }
  count = 0;
  while (count < numYBounds)
  {
    buf = inFile.readLine();
    buf = buf.trimmed();
    buf = buf.simplified();
    tokens = buf.split(' ');
    for (size_t iter2 = 0; iter2 < tokens.size(); iter2++)
    {
      ybnds[count] = tokens[iter2].toFloat(&ok);
      if (!ok)
      {
        QString ss = QObject::tr("Could not read Y coordinte number %1. Could not interpret as float value.").arg(count + 1);
        setErrorCondition(-100);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return -1;
      }
      count++;
    }
  }

  //Read the line that should have the format: ZPOINTS number
  buf = inFile.readLine();
  buf = buf.trimmed();
  buf = buf.simplified();
  tokens = buf.split(' ');
  word = tokens.at(0);
  if (word.compare("Z_COORDINATES") != 0)
  {
    QString ss = QObject::tr("Z_COORDINATES keyword not found in proper location in: %1.").arg(m_BoundsFile);
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }
  if (tokens.size() != 2)
  {
    QString ss = QObject::tr("Z_COORDINATES keyword line not properly formatted in: %1.").arg(m_BoundsFile);
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }
  size_t numZBounds = tokens[1].toInt(&ok, 10);
  if (numZBounds != zbounds->getNumberOfTuples())
  {
    QString ss = QObject::tr("The number of Z bounds in %1 does not match the z dimension of the geometry.").arg(m_BoundsFile);
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return -1;
  }
  count = 0;
  while (count < numZBounds)
  {
    buf = inFile.readLine();
    buf = buf.trimmed();
    buf = buf.simplified();
    tokens = buf.split(' ');
    for (size_t iter2 = 0; iter2 < tokens.size(); iter2++)
    {
      zbnds[count] = tokens[iter2].toFloat(&ok);
      if (!ok)
      {
        QString ss = QObject::tr("Could not read Z coordinte number %1. Could not interpret as float value.").arg(count + 1);
        setErrorCondition(-100);
        notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
        return -1;
      }
      count++;
    }
  }

  inFile.close();

  m->getGeometryAs<RectGridGeom>()->setXBounds(xbounds);
  m->getGeometryAs<RectGridGeom>()->setYBounds(ybounds);
  m->getGeometryAs<RectGridGeom>()->setZBounds(zbounds);

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ImportImageStack::newFilterInstance(bool copyFilterParameters)
{
  ImportImageStack::Pointer filter = ImportImageStack::New();
  if(true == copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters() );
    // We are going to hand copy all of the parameters because the other way of copying the parameters are going to
    // miss some of them because we are not enumerating all of them.
    SIMPL_COPY_INSTANCEVAR(DataContainerName)
    SIMPL_COPY_INSTANCEVAR(CellAttributeMatrixName)
    SIMPL_COPY_INSTANCEVAR(GeometryType)
    SIMPL_COPY_INSTANCEVAR(Resolution)
    SIMPL_COPY_INSTANCEVAR(Origin)
    SIMPL_COPY_INSTANCEVAR(BoundsFile)
#if 0
    SIMPL_COPY_INSTANCEVAR(ZStartIndex)
    SIMPL_COPY_INSTANCEVAR(ZEndIndex)
    SIMPL_COPY_INSTANCEVAR(InputPath)
    SIMPL_COPY_INSTANCEVAR(FilePrefix)
    SIMPL_COPY_INSTANCEVAR(FileSuffix)
    SIMPL_COPY_INSTANCEVAR(FileExtension)
    SIMPL_COPY_INSTANCEVAR(PaddingDigits)
    SIMPL_COPY_INSTANCEVAR(RefFrameZDir)
#endif
    SIMPL_COPY_INSTANCEVAR(InputFileListInfo)
    SIMPL_COPY_INSTANCEVAR(ImageStack)
    SIMPL_COPY_INSTANCEVAR(ImageDataArrayName)
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportImageStack::getCompiledLibraryName()
{
  return ImageIOConstants::ImageIOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportImageStack::getBrandingString()
{
  return "ImageIO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportImageStack::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  SIMPLib::Version::Major() << "." << SIMPLib::Version::Minor() << "." << SIMPLib::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportImageStack::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportImageStack::getSubGroupName()
{ return DREAM3D::FilterSubGroups::InputFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ImportImageStack::getHumanLabel()
{ return "Import Images (3D Stack)"; }
