/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include "WriteImages.h"

#include <QtCore/QDir>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/OutputPathFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/FilterParameters/DataArraySelectionFilterParameter.h"
#include "SIMPLib/FilterParameters/ChoiceFilterParameter.h"
#include "SIMPLib/FilterParameters/LinkedBooleanFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "ImageIO/ImageIOConstants.h"
#include "ImageIO/ImageIOVersion.h"

// Include the MOC generated file for this class
#include "moc_WriteImages.cpp"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteImages::WriteImages() :
  AbstractFilter(),
  m_FilePrefix(false),
  m_ImagePrefix(""),
  m_OutputPath(""),
  m_ImageFormat(0),
  m_Plane(0),
  m_ColorsArrayPath("", "", ""),
  m_Colors(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
WriteImages::~WriteImages()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteImages::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Image Format");
    parameter->setPropertyName("ImageFormat");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(WriteImages, this, ImageFormat));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(WriteImages, this, ImageFormat));

    QVector<QString> choices;
    choices.push_back("tif");
    choices.push_back("bmp");
    choices.push_back("png");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Plane");
    parameter->setPropertyName("Plane");
    parameter->setSetterCallback(SIMPL_BIND_SETTER(WriteImages, this, Plane));
    parameter->setGetterCallback(SIMPL_BIND_GETTER(WriteImages, this, Plane));

    QVector<QString> choices;
    choices.push_back("XY");
    choices.push_back("XZ");
    choices.push_back("YZ");
    parameter->setChoices(choices);
    parameter->setCategory(FilterParameter::Parameter);
    parameters.push_back(parameter);
  }

  parameters.push_back(SIMPL_NEW_OUTPUT_PATH_FP("Output Directory Path", OutputPath, FilterParameter::Parameter, WriteImages));
  {
    QStringList linkedProps;
    linkedProps << "ImagePrefix";
    parameters.push_back(SIMPL_NEW_LINKED_BOOL_FP("File Prefix", FilePrefix, FilterParameter::Parameter, WriteImages, linkedProps));
  }
  parameters.push_back(SIMPL_NEW_STRING_FP("Image File Prefix", ImagePrefix, FilterParameter::Parameter, WriteImages));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::RequiredArray));
  {
    DataArraySelectionFilterParameter::RequirementType req = DataArraySelectionFilterParameter::CreateRequirement(SIMPL::TypeNames::UInt8, SIMPL::Defaults::AnyComponentSize, SIMPL::AttributeMatrixType::Cell, SIMPL::GeometryType::ImageGeometry);
    parameters.push_back(SIMPL_NEW_DA_SELECTION_FP("Color Data", ColorsArrayPath, FilterParameter::RequiredArray, WriteImages, req));
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteImages::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setImagePrefix( reader->readString("ImagePrefix", getImagePrefix()) );
  setFilePrefix( reader->readValue("FilePrefix", getFilePrefix()) );
  setOutputPath( reader->readString("OutputPath", getOutputPath()) );
  setColorsArrayPath( reader->readDataArrayPath("ColorsArrayPath", getColorsArrayPath()) );
  setImageFormat( reader->readValue("ImageFormat", getImageFormat()) );
  setPlane(reader->readValue("Plane", getPlane()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteImages::initialize()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteImages::dataCheck()
{
  setErrorCondition(0);

  QDir dir(getOutputPath());

  if (getOutputPath().isEmpty() == true)
  {
    setErrorCondition(-1003);
    notifyErrorMessage(getHumanLabel(), "The output directory must be set", getErrorCondition());
  }
  else if (dir.exists() == false)
  {
    QString ss = QObject::tr("The output directory path does not exist. DREAM.3D will attempt to create this path during execution");
    notifyWarningMessage(getHumanLabel(), ss, -1);
  }

  ImageGeom::Pointer image = getDataContainerArray()->getPrereqGeometryFromDataContainer<ImageGeom, AbstractFilter>(this, getColorsArrayPath().getDataContainerName());
  if(getErrorCondition() < 0) { return; }

  size_t dims[3] = { 0, 0, 0 };
  image->getDimensions(dims);
  if (0 == m_Plane) // XY plane
  {
    size_t total = dims[0] * dims[1] * 4;
    if(total > std::numeric_limits<int32_t>::max())
    {
      QString ss = QObject::tr("The image will have more than 2GB worth of pixels. Try cropping the data so that the total pixels on a single plane is less than 2GB.");
      setErrorCondition(-1012);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }
  else if (1 == m_Plane) // XZ plane
  {
    size_t total = dims[0] * dims[2] * 4;
    if(total > std::numeric_limits<int32_t>::max())
    {
      QString ss = QObject::tr("The image will have more than 2GB worth of pixels. Try cropping the data so that the total pixels on a single plane is less than 2GB.");
      setErrorCondition(-1012);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }
  else if (2 == m_Plane) // YZ plane
  {
    size_t total = dims[1] * dims[2] * 4;
    if(total > std::numeric_limits<int32_t>::max())
    {
      QString ss = QObject::tr("The image will have more than 2GB worth of pixels. Try cropping the data so that the total pixels on a single plane is less than 2GB.");
      setErrorCondition(-1012);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }
  IDataArray::Pointer iDa = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getColorsArrayPath());
  if (getErrorCondition() < 0) { return; }

  QVector<size_t> cDims = iDa->getComponentDimensions();

  if (cDims[0] == 1)
  {
    m_ColorsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(this, getColorsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if (NULL != m_ColorsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_Colors = m_ColorsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  else if (cDims[0] == 3)
  {
    m_ColorsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(this, getColorsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if (NULL != m_ColorsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_Colors = m_ColorsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  else if (cDims[0] == 4)
  {
    m_ColorsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(this, getColorsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if (NULL != m_ColorsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_Colors = m_ColorsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  else
  {
    setErrorCondition(-1006);
    notifyErrorMessage(getHumanLabel(), "Number of components must be 1 (grayscale), 3 (RGB) or 4 (ARGB) arrays", getErrorCondition());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void WriteImages::preflight()
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
void WriteImages::execute()
{
  int32_t err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_ColorsArrayPath.getDataContainerName());
  size_t dims[3] = { 0, 0, 0 };
  m->getGeometryAs<ImageGeom>()->getDimensions(dims);

  int32_t nComp = m_ColorsPtr.lock()->getNumberOfComponents();

  if (0 == m_Plane) // XY plane
  {
    for (size_t z = 0; z < dims[2]; ++z)
    {
      if (nComp == 1)
      {
        err = writeGrayscaleImage(z, dims[0], dims[1], dims);
      }
      else
      {
        err = writeRGBImage(z, dims[0], dims[1], dims);
      }

      if (-1 == err)
      {
        return;
      }
    }
  }
  else if (1 == m_Plane) // XZ plane
  {
    for (size_t y = 0; y < dims[1]; ++y)
    {
      if (nComp == 1)
      {
        err = writeGrayscaleImage(y, dims[0], dims[2], dims);
      }
      else
      {
        err = writeRGBImage(y, dims[0], dims[2], dims);
      }

      if (-1 == err)
      {
        return;
      }
    }
  }
  else if (2 == m_Plane) // YZ plane
  {
    for (size_t x = 0; x < dims[0]; ++x)
    {
      if (nComp == 1)
      {
        err = writeGrayscaleImage(x, dims[1], dims[2], dims);
      }
      else
      {
        err = writeRGBImage(x, dims[1], dims[2], dims);
      }

      if (-1 == err)
      {
        return;
      }
    }
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t WriteImages::writeRGBImage(size_t slice, size_t dB, size_t dA, size_t* dims)
{
  int32_t err = 0;
  // Sets up for Gray Scale or RGB or RGBA arrays
  int32_t nComp = m_ColorsPtr.lock()->getNumberOfComponents();

  QString path = (m_OutputPath) + QDir::separator() + (m_ImagePrefix) + QString::number(slice);

  if (!m_FilePrefix)
  {
    path = (m_OutputPath) + QDir::separator() + QString::number(slice);
  }
  if (m_ImageFormat == TifImageType)
  {
    path.append(".tif");
  }
  else if (m_ImageFormat == BmpImageType)
  {
    path.append(".bmp");
  }
  else if (m_ImageFormat == PngImageType)
  {
    path.append(".png");
  }

  path = QDir::toNativeSeparators(path);
  QFileInfo fi(path);
  QDir parent(fi.absolutePath());
  if (parent.exists() == false)
  {
    parent.mkpath(fi.absolutePath());
  }

  int32_t index = 0;
  size_t total = dB * dA * 4; // The '4' is there because QImage will convert it to RGBA image.
  if(total > std::numeric_limits<int32_t>::max())
  {
    QString ss = QObject::tr("The image will have more than 2GB worth of pixels. Try cropping the data so that the total pixels on a single plane is less than 2GB.");
    setErrorCondition(-1012);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return getErrorCondition();
  }


  QImage image(dB, dA, QImage::Format_RGB32);
  if(image.isNull())
  {
    QString ss = QObject::tr("The memory for the image could not be allocated using a QImage. The total number of bytes would be greater than 2GB");
    setErrorCondition(-1014);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return getErrorCondition();
  }

  for (size_t axisA = 0; axisA < dA; ++axisA)
  {
    uint8_t* scanLine = image.scanLine(axisA);
    for (size_t axisB = 0; axisB < dB; ++axisB)
    {
#if defined (CMP_WORDS_BIGENDIAN)
#error
#else
      scanLine[axisB * 4 + 3] = 0xFF;
      if (0 == m_Plane)  // XY plane
      {
        index = (nComp * dims[0] * dims[1] * slice) + (nComp * dims[0] * axisA) + (nComp * axisB);
      }
      if (1 == m_Plane)  // XZ plane
      {
        index = (nComp * dims[0] * dims[1] * axisA) + (nComp * dims[0] * slice) + (nComp * axisB);
      }
      if (2 == m_Plane)  // YZ plane
      {
        index = (nComp * dims[0] * dims[1] * axisA) + (nComp * dims[0] * axisB) + (nComp * slice);
      }
      scanLine[axisB * 4 + 2] = m_Colors[index + 0];
      scanLine[axisB * 4 + 1] = m_Colors[index + 1];
      scanLine[axisB * 4 + 0] = m_Colors[index + 2];
#endif
    }
  }
  image.setText("Description", ImageIO::Version::PackageComplete());
  bool success = image.save(path);
  if (success)
  {
    err = 0;
  }
  else
  {
    err = -1;
    QString ss = QObject::tr("The image '%1' was not successfully saved").arg(path);
    setErrorCondition(-1007);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t WriteImages::writeGrayscaleImage(size_t slice, size_t dB, size_t dA, size_t* dims)
{
  int32_t err = 0;

  QString path = (m_OutputPath) + QDir::separator() + (m_ImagePrefix) + QString::number(slice);

  if (!m_FilePrefix)
  {
    path = (m_OutputPath) + QDir::separator() + QString::number(slice);
  }
  if (m_ImageFormat == TifImageType)
  {
    path.append(".tif");
  }
  else if (m_ImageFormat == BmpImageType)
  {
    path.append(".bmp");
  }
  else if (m_ImageFormat == PngImageType)
  {
    path.append(".png");
  }

  path = QDir::toNativeSeparators(path);
  QFileInfo fi(path);
  QDir parent(fi.absolutePath());
  if (parent.exists() == false)
  {
    parent.mkpath(fi.absolutePath());
  }

  int32_t index = 0;
  size_t total = dB * dA;
  if(total > std::numeric_limits<int32_t>::max())
  {
    QString ss = QObject::tr("The image will have more than 2GB worth of pixels. Try cropping the data so that the total pixels on a single plane is less than 2GB.");
    setErrorCondition(-1012);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return getErrorCondition();
  }


  QImage image(dB, dA, QImage::Format_Grayscale8);
  if(image.isNull())
  {
    QString ss = QObject::tr("The memory for the image could not be allocated using a QImage. The total number of bytes would be greater than 2GB");
    setErrorCondition(-1014);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return getErrorCondition();
  }

  for (size_t axisA = 0; axisA < dA; ++axisA)
  {
    uint8_t* scanLine = image.scanLine(axisA);
    for (size_t axisB = 0; axisB < dB; ++axisB)
    {
#if defined (CMP_WORDS_BIGENDIAN)
#error
#else
      if (0 == m_Plane)  // XY plane
      {
        index = (dims[0] * dims[1] * slice) + (dims[0] * axisA) + axisB;
      }
      if (1 == m_Plane)  // XZ plane
      {
        index = (dims[0] * dims[1] * axisA) + (dims[0] * slice) + axisB;
      }
      if (2 == m_Plane)  // YZ plane
      {
        index = (dims[0] * dims[1] * axisA) + (dims[0] * axisB) + slice;
      }
      scanLine[axisB] = m_Colors[index];
#endif
    }
  }
  image.setText("Description", ImageIO::Version::PackageComplete());
  bool success = image.save(path);
  if (success)
  {
    err = 0;
  }
  else
  {
    err = -1;
    QString ss = QObject::tr("The image '%1' was not successfully saved").arg(path);
    setErrorCondition(-1007);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer WriteImages::newFilterInstance(bool copyFilterParameters)
{
  WriteImages::Pointer filter = WriteImages::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteImages::getCompiledLibraryName()
{
  return ImageIOConstants::ImageIOBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteImages::getBrandingString()
{
  return "ImageIO";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteImages::getFilterVersion()
{
  QString version;
  QTextStream vStream(&version);
  vStream <<  ImageIO::Version::Major() << "." << ImageIO::Version::Minor() << "." << ImageIO::Version::Patch();
  return version;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteImages::getGroupName()
{ return SIMPL::FilterGroups::IOFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteImages::getSubGroupName()
{ return SIMPL::FilterSubGroups::OutputFilters; }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString WriteImages::getHumanLabel()
{ return "Write Images"; }
