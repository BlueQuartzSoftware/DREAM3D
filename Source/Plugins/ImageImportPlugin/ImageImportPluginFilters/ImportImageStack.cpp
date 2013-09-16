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

#include "ImportImageStack.h"

#include <string.h>

#include <QtCore/QString>
#include <QtGui/QImage>
#include <QtGui/QImageReader>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportImageStack::ImportImageStack() :
  AbstractFilter(),
  m_ImageDataArrayName(DREAM3D::CellData::ImageData),
  m_ZStartIndex(0),
  m_ZEndIndex(0),
  m_RefFrameZDir(Ebsd::LowtoHigh),
  m_ImageData(NULL)
{

  m_Origin.x = 0.0;
  m_Origin.y = 0.0;
  m_Origin.z = 0.0;

  m_Resolution.x = 1.0;
  m_Resolution.y = 1.0;
  m_Resolution.z = 1.0;


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
  QVector<FilterParameter::Pointer> options;



  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportImageStack::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setImageDataArrayName( reader->readValue("ImageDataArrayName", getImageDataArrayName()) );
  setZStartIndex( reader->readValue("ZStartIndex", getZStartIndex()) );
  setZEndIndex( reader->readValue("ZEndIndex", getZEndIndex()) );
  setOrigin( reader->readValue("Origin", getOrigin()) );
  setResolution( reader->readValue("Resolution", getResolution()) );
  Ebsd::RefFrameZDir zdir = static_cast<Ebsd::RefFrameZDir>(reader->readValue("RefFrameZDir",getRefFrameZDir()));
  setRefFrameZDir(zdir);
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImportImageStack::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  writer->writeValue("ImageDataArrayName", getImageDataArrayName() );
  writer->writeValue("ZStartIndex", getZStartIndex() );
  writer->writeValue("ZEndIndex", getZEndIndex() );
  writer->writeValue("Origin", getOrigin() );
  writer->writeValue("Resolution", getResolution() );
  writer->writeValue("RefFrameZDir", getRefFrameZDir() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportImageStack::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getVolumeDataContainer();

  if (m_ImageFileList.size() == 0)
  {
    QString ss = QObject::tr("No files have been selected for import. Have you set the input directory?");
    setErrorCondition(-11);
    addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else
  {
    // This would be for a gray scale image
    CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, ImageData, uint8_t, UInt8ArrayType, 0, voxels, 1)
    // If we have RGB or RGBA Images then we are going to have to change things a bit.
    // We should read the file and see what we have? Of course Qt is going to read it up into
    // an RGB array by default
    int err = 0;
    QImageReader reader((m_ImageFileList[0]));
    QSize imageDims = reader.size();
    int64_t dims[3] = {imageDims.width(), imageDims.height(), m_ImageFileList.size()};
    /* Sanity check what we are trying to load to make sure it can fit in our address space.
     * Note that this does not guarantee the user has enough left, just that the
     * size of the volume can fit in the address space of the program
     */
#if   (CMP_SIZEOF_SSIZE_T==4)
    int64_t max = std::numeric_limits<size_t>::max();
#else
    int64_t max = std::numeric_limits<int64_t>::max();
#endif
  if(dims[0] * dims[1] * dims[2] > max)
  {
    err = -1;
    QString ss = QObject::tr("The total number of elements '%1' is greater than this program can hold. Try the 64 bit version.").arg((dims[0] * dims[1] * dims[2]));
    setErrorCondition(err);
    addErrorMessage(getHumanLabel(), ss, err);
  }

  if(dims[0] > max || dims[1] > max || dims[2] > max)
  {
    err = -1;
    QString ss = QObject::tr("One of the dimensions is greater than the max index for this sysem. Try the 64 bit version."
    " dim[0]=%1  dim[1]=%2  dim[2]=%3").arg(dims[0]).arg(dims[1]).arg(dims[2]);
    setErrorCondition(err);
    addErrorMessage(getHumanLabel(), ss, err);
  }
    /* ************ End Sanity Check *************************** */

    m->setDimensions(static_cast<size_t>(dims[0]), static_cast<size_t>(dims[1]), static_cast<size_t>(dims[2]));
    m->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
    m->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);

  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportImageStack::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportImageStack::execute()
{
  int err = 0;
  setErrorCondition(err);
  VolumeDataContainer* m = getVolumeDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);



  m->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  m->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);


  UInt8ArrayType::Pointer data = UInt8ArrayType::NullPointer();

  uint8_t* imagePtr = NULL;

  int pixelBytes = 0;
  int totalPixels = 0;
  int height = 0;
  int width = 0;
  //  int bytesPerLine = 0;

  int64_t z = m_ZStartIndex;
  int64_t zSpot;
  for (QVector<QString>::iterator filepath = m_ImageFileList.begin(); filepath != m_ImageFileList.end(); ++filepath)
  {
    QString imageFName = *filepath;
    QString ss = QObject::tr("Importing file %1").arg(imageFName);
    notifyStatusMessage(ss);

    QImage image((imageFName));
    if (image.isNull() == true)
    {
      setErrorCondition(-14000);
      notifyErrorMessage("Failed to load Image file", getErrorCondition());
    }
    height = image.height();
    width = image.width();
    totalPixels = width * height;
    //  bytesPerLine = image.bytesPerLine();
    // This is the first image so we need to create our block of data to store the data
    if (z == m_ZStartIndex)
    {
      m->setDimensions(width, height, m_ImageFileList.size());
      if (image.format() == QImage::Format_Indexed8)
      {
        pixelBytes = 1;
      }
      else if (image.format() == QImage::Format_RGB32 || image.format() == QImage::Format_ARGB32)
      {
        pixelBytes = 4;
      }
      data = UInt8ArrayType::CreateArray(totalPixels * m_ImageFileList.size(), pixelBytes, m_ImageDataArrayName);

    }

    // Get the current position in the array to copy the image into
    zSpot = (z-m_ZStartIndex);
    for(qint32 i = 0; i < height; ++i)
    {
      imagePtr = data->getPointer( (zSpot) * totalPixels * pixelBytes + i * (width * pixelBytes));
      uint8_t* source = image.scanLine(i);
      ::memcpy(imagePtr, source, width * pixelBytes);
    }

    ++z;
    if(getCancel() == true)
    {
      notifyStatusMessage("Conversion was Canceled");
      return;
    }
  }

  m->addCellData(data->GetName(), data);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
