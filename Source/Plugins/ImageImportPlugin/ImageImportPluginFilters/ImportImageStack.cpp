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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportImageStack::ImportImageStack() :
  AbstractFilter(),
  m_ImageDataArrayName(DREAM3D::CellData::ImageData),
  m_ZStartIndex(0),
  m_ZEndIndex(0),
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
  std::vector<FilterParameter::Pointer> options;



  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportImageStack::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImportImageStack::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  /* Place code that will write the inputs values into a file. reference the
  * AbstractFilterParametersWriter class for the proper API to use.
  */
  writer->writeValue("ImageDataArrayName", getImageDataArrayName() );
  writer->writeValue("ZStartIndex", getZStartIndex() );
  writer->writeValue("ZEndIndex", getZEndIndex() );
  writer->writeValue("Origin", getOrigin() );
  writer->writeValue("Resolution", getResolution() );

  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportImageStack::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  if (m_ImageFileList.size() == 0)
  {
    ss.str("");
    ss << "No files have been selected for import. Have you set the input directory?";
    setErrorCondition(-11);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }
  else
  {
    // This would be for a gray scale image
    CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, ImageData, ss, uint8_t, UInt8ArrayType, 0, voxels, 1)
    // If we have RGB or RGBA Images then we are going to have to change things a bit.
    // We should read the file and see what we have? Of course Qt is going to read it up into
    // an RGB array by default
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
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  std::stringstream ss;


  m->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  m->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);


  UInt8ArrayType::Pointer data = UInt8ArrayType::NullPointer();

  uint8_t* imagePtr = NULL;

  float total = static_cast<float>( m_ZEndIndex - m_ZStartIndex );
  int progress = 0;
  int pixelBytes = 0;
  int totalPixels = 0;
  int height = 0;
  int width = 0;
//  int bytesPerLine = 0;

  int64_t z = m_ZStartIndex;
  for (std::vector<std::string>::iterator filepath = m_ImageFileList.begin(); filepath != m_ImageFileList.end(); ++filepath)
  {
    std::string imageFName = *filepath;
    progress = static_cast<int>( z - m_ZStartIndex );
    progress = (int)(100.0f * (float)(progress) / total);

    ss.str("");
    ss << "Importing file " << imageFName;
    notifyStatusMessage(ss.str());

    QImage image(QString::fromStdString(imageFName));
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
    imagePtr = data->GetPointer( (z-m_ZStartIndex) * totalPixels * pixelBytes);
    for(qint32 i = 0; i < height; ++i)
    {
      imagePtr = data->GetPointer( (z-m_ZStartIndex) * totalPixels * pixelBytes + i * (width * pixelBytes));
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
