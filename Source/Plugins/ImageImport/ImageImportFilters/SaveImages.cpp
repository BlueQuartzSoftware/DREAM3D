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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SaveImages::SaveImages() :
  AbstractFilter(),
  m_ColorsArrayPath("", "", ""),
  m_ImagePrefix(""),
  m_OutputPath(""),
  m_ImageFormat(0)
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
  /* Place all your option initialization code here */
  /* To Display a Combobox with a list of current Voxel Cell Arrays in it */
  parameters.push_back(FilterParameter::New(" Colors Array", "ColorsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget,"DataArrayPath", false));
  {
    ChoiceFilterParameter::Pointer parameter = ChoiceFilterParameter::New();
    parameter->setHumanLabel("Image Format");
    parameter->setPropertyName("ImageFormat");
    parameter->setWidgetType(FilterParameterWidgetType::ChoiceWidget);
    parameter->setValueType("unsigned int");
    QVector<QString> choices;
    choices.push_back("tif");
    choices.push_back("bmp");
    choices.push_back("png");
    parameter->setChoices(choices);
    parameters.push_back(parameter);
  }
  /* For String input use this code */
  parameters.push_back(FilterParameter::New(" Image Prefix", "ImagePrefix", FilterParameterWidgetType::StringWidget,"QString", false));
  /*   For an output path use this code*/
  parameters.push_back(FilterParameter::New("Output Path", "OutputPath", FilterParameterWidgetType::OutputPathWidget,"QString", false));


  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SaveImages::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setImagePrefix( reader->readString("ImagePrefix", getImagePrefix()) );
  setOutputPath( reader->readString("OutputPath", getOutputPath()) );
  setColorsArrayPath( reader->readDataArrayPath("ColorsArrayPath", getColorsArrayPath()) );
  setImageFormat( reader->readValue("ImageFormat", getImageFormat()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SaveImages::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("ImagePrefix", getImagePrefix() );
  writer->writeValue("OutputPath", getOutputPath() );
  writer->writeValue("ColorsArrayPath", getColorsArrayPath() );
  writer->writeValue("ImageFormat", getImageFormat() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SaveImages::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, m_ColorsArrayPath.getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, m_ColorsArrayPath.getAttributeMatrixName(), -301);
  if(getErrorCondition() < 0 || NULL == cellAttrMat.get() ) { return; }

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

  if(m_ColorsArrayPath.isEmpty() == true)
  {
    setErrorCondition(-1004);
    notifyErrorMessage(getHumanLabel(), "Input  Color Array name is empty", getErrorCondition());
  }
  else
  {
    QVector<size_t> dims(1, 3);
    m_ColorsPtr = cellAttrMat->getPrereqArray<DataArray<uint8_t>, AbstractFilter>(this, m_ColorsArrayPath.getDataArrayName(), -300, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if( NULL != m_ColorsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_Colors = m_ColorsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SaveImages::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
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

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(m_ColorsArrayPath.getDataContainerName());

  /* Place all your code to execute your filter here. */
  size_t dims[3];
  m->getDimensions(dims);

  size_t index = 0;
  uint8_t* slice = NULL;
  for(size_t z = 0; z < dims[2]; ++z)
  {
    index = z * (dims[0] * dims[1]) * 3;
    // Get the pointer to the data.
    slice = m_Colors + index;

    err = saveImage(slice, z, dims);
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SaveImages::saveImage(uint8_t* ipfColors, size_t slice, size_t* dims)
{
  int err = 0;

  QString path = (m_OutputPath) + QDir::separator() + (m_ImagePrefix) + QString::number(slice);
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
  else if (m_ImageFormat == JpgImageType)
  {
    path.append(".jpg");
  }

  path = QDir::toNativeSeparators(path);
  QFileInfo fi(path);
  QDir parent(fi.absolutePath());
  if (parent.exists() == false)
  {
    parent.mkpath(fi.absolutePath());
  }

  int index = 0;
  QImage image(dims[0], dims[1], QImage::Format_RGB32);
  for(size_t y = 0; y < dims[1]; ++y)
  {
    uint8_t* scanLine = image.scanLine(y);
    for(size_t x = 0; x < dims[0]; ++x)
    {
#if defined (CMP_WORDS_BIGENDIAN)
#error
#else

      scanLine[x * 4 + 3] = 0xFF;
      index = y * dims[0] * 3 + x * 3;
      scanLine[x * 4 + 2] = ipfColors[index + 0];
      scanLine[x * 4 + 1] = ipfColors[index + 1];
      scanLine[x * 4 + 0] = ipfColors[index + 2];

#endif
    }

  }
  image.setText("", "DREAM3D generated Image");
  bool success = image.save(path);
  if (success) { err = 0;}
  else { err = -1;}
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer SaveImages::newFilterInstance(bool copyFilterParameters)
{
  /*
  * ImagePrefix
  * OutputPath
  * ImageFormat
  * ColorsArrayName
  */
  SaveImages::Pointer filter = SaveImages::New();
  if(true == copyFilterParameters)
  {
    filter->setColorsArrayPath( getColorsArrayPath() );
    filter->setImageFormat( getImageFormat() );
    filter->setImagePrefix( getImagePrefix() );
    filter->setOutputPath( getOutputPath() );
  }
  return filter;
}
