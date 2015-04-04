/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "SaveImages.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtGui/QImage>
#include <QtGui/QColor>

#include "DREAM3DLib/FilterParameters/AbstractFilterParametersReader.h"
#include "DREAM3DLib/FilterParameters/AbstractFilterParametersWriter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SaveImages::SaveImages() :
  AbstractFilter(),
  m_ImagePrefix(""),
  m_OutputPath(""),
  m_ImageFormat(0),
  m_Plane(0),
  m_FilePrefix(false),
  m_ColorsArrayPath(DREAM3D::Defaults::DataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::IPFColor),
  m_Colors(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SaveImages::~SaveImages()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SaveImages::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", false));
  parameters.push_back(FilterParameter::New("Select Color Data", "ColorsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getColorsArrayPath(), false, ""));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Image Format");
    parameter->setPropertyName("ImageFormat");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    QVector<QString> choices;
    choices.push_back("tif");
    choices.push_back("bmp");
    choices.push_back("png");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Plane");
    parameter->setPropertyName("Plane");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    QVector<QString> choices;
    choices.push_back("XY");
    choices.push_back("XZ");
    choices.push_back("YZ");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  {
    QStringList linkedProps;
    linkedProps << "ImagePrefix";
    parameters.push_back(LinkedBooleanFilterParameter::New("File Prefix", "FilePrefix", getFilePrefix(), linkedProps, false));
  }
  parameters.push_back(FilterParameter::New("Image File Prefix", "ImagePrefix", FilterParameterWidgetType::StringWidget, getImagePrefix(), false));
  parameters.push_back(FileSystemFilterParameter::New("Output Path", "OutputPath", FilterParameterWidgetType::OutputPathWidget, getOutputPath(), false));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SaveImages::readFilterParameters(AbstractFilterParametersReader* reader, int index)
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
int SaveImages::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(FilterVersion)
  DREAM3D_FILTER_WRITE_PARAMETER(ImagePrefix)
  DREAM3D_FILTER_WRITE_PARAMETER(FilePrefix)
  DREAM3D_FILTER_WRITE_PARAMETER(OutputPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ColorsArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(ImageFormat)
  DREAM3D_FILTER_WRITE_PARAMETER(Plane)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SaveImages::dataCheck()
{
  setErrorCondition(0);

  QDir dir(getOutputPath());

  if (getOutputPath().isEmpty() == true)
  {
    setErrorCondition(-1003);
    notifyErrorMessage(getHumanLabel(), "Output Directory is Not set correctly", getErrorCondition());
  }
  else if (dir.exists() == false)
  {
    QString ss = QObject::tr("The directory path for the output file does not exist. DREAM3D will attempt to create this path during execution of the filter.");
    notifyWarningMessage(getHumanLabel(), ss, -1);
  }

  IDataArray::Pointer iDa = getDataContainerArray()->getPrereqIDataArrayFromPath<IDataArray, AbstractFilter>(this, getColorsArrayPath());
  if (getErrorCondition() < 0)
  {
    return;
  }

  QVector<size_t> cDims = iDa->getComponentDimensions();

  if (cDims[0] == 1)
  {
    m_ColorsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(this, getColorsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if (NULL != m_ColorsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    {
      m_Colors = m_ColorsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  else if (cDims[0] == 3)
  {
    m_ColorsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(this, getColorsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if (NULL != m_ColorsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    {
      m_Colors = m_ColorsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  else if (cDims[0] == 4)
  {
    m_ColorsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(this, getColorsArrayPath(), cDims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if (NULL != m_ColorsPtr.lock().get()) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    {
      m_Colors = m_ColorsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
  else
  {
    setErrorCondition(-1006);
    notifyErrorMessage(getHumanLabel(), "Number of Components must be 1, 3 or 4 for Gray Scale or RGB or RGBA arrays", getErrorCondition());
    return;
  }

  if(getErrorCondition() < 0) { return; }

  ImageGeom::Pointer image = getDataContainerArray()->getDataContainer(getColorsArrayPath().getDataContainerName())->getPrereqGeometry<ImageGeom, AbstractFilter>(this);
  if(getErrorCondition() < 0 || NULL == image.get()) { return; }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SaveImages::preflight()
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
void SaveImages::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(m_ColorsArrayPath.getDataContainerName());
  size_t dims[3];
  m->getGeometryAs<ImageGeom>()->getDimensions(dims);

  if (0 == m_Plane) // XY plane
  {
    for (size_t z = 0; z < dims[2]; ++z)
    {
      err = saveImage(z, dims[0], dims[1], dims);
      if (-1 == err) { return; }
    }
  }
  else if (1 == m_Plane) // XZ plane
  {
    for (size_t y = 0; y < dims[1]; ++y)
    {
      err = saveImage(y, dims[0], dims[2], dims);
      if (-1 == err) { return; }
    }
  }
  else if (2 == m_Plane) // YZ plane
  {
    for (size_t x = 0; x < dims[0]; ++x)
    {
      err = saveImage(x, dims[1], dims[2], dims);
      if (-1 == err) { return; }
    }
  }
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SaveImages::saveImage(size_t slice, size_t dB, size_t dA, size_t* dims)
{
  int err = 0;
  // Sets up for Gray Scale or RGB or RGBA arrays
  int nComp = m_ColorsPtr.lock()->getNumberOfComponents();

  QString path = (m_OutputPath) + QDir::separator() + (m_ImagePrefix) + QString::number(slice);

  if (!m_FilePrefix)
  {
      path = (m_OutputPath)+QDir::separator() + QString::number(slice);
  }
  if(m_ImageFormat == TifImageType)
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

  int index = 0;
  QImage image(dB, dA, QImage::Format_RGB32);
  for (size_t axisA = 0; axisA < dA; ++axisA)
  {
    uint8_t* scanLine = image.scanLine(axisA);
    for (size_t axisB = 0; axisB < dB; ++axisB)
    {
#if defined (CMP_WORDS_BIGENDIAN)
#error
#else
      scanLine[axisB * 4 + 3] = 0xFF;
      if (0 == m_Plane){ // XY plane
        index = (nComp * dims[0] * dims[1] * slice) + (nComp * dims[0] * axisA) + (nComp * axisB);
      }
      if (1 == m_Plane){ // XZ plane
        index = (nComp * dims[0] * dims[1] * axisA) + (nComp * dims[0] * slice) + (nComp * axisB);
      }
      if (2 == m_Plane){ // YZ plane
        index = (nComp * dims[0] * dims[1] * axisA) + (nComp * dims[0] * axisB) + (nComp * slice);
      }
      scanLine[axisB * 4 + 2] = m_Colors[index + 0];
      scanLine[axisB * 4 + 1] = m_Colors[index + 1];
      scanLine[axisB * 4 + 0] = m_Colors[index + 2];
#endif
    }
  }
  image.setText("Description", DREAM3DLib::Version::PackageComplete());
  bool success = image.save(path);
  if (success) {
    err = 0;
  }
  else {
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
AbstractFilter::Pointer SaveImages::newFilterInstance(bool copyFilterParameters)
{
  SaveImages::Pointer filter = SaveImages::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SaveImages::getCompiledLibraryName()
{ return ImageImport::ImageImportBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SaveImages::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SaveImages::getSubGroupName()
{ return DREAM3D::FilterSubGroups::OutputFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString SaveImages::getHumanLabel()
{ return "Export Slice Images"; }

