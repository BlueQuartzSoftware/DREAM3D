/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2014 Dr. Joseph C. Tucker (UES, Inc.)
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *                   FA8650-07-D-5800 and FA8650-13-M-5048
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "ImportR3DStack.h"

#include <string.h>

#include <QtCore/QString>
#include <QtGui/QImage>
#include <QtCore/QFile>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportR3DStack::ImportR3DStack() :
  AbstractFilter(),
  m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
  m_ZStartIndex(0),
  m_ZEndIndex(0),
  m_GrainIds(NULL)
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
ImportR3DStack::~ImportR3DStack()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportR3DStack::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> options;



  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportR3DStack::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setZStartIndex( reader->readValue("ZStartIndex", getZStartIndex()) );
  setZEndIndex( reader->readValue("ZEndIndex", getZEndIndex()) );
  setOrigin( reader->readValue("Origin", getOrigin()) );
  setResolution( reader->readValue("Resolution", getResolution()) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImportR3DStack::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
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
void ImportR3DStack::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  if (m_R3DFileList.size() == 0)
  {
    ss.str("");
    ss << "No files have been selected for import. Have you set the input directory?";
    setErrorCondition(-11);
    addErrorMessage(getHumanLabel(), ss.str(), getErrorCondition());
  }
  else
  {
  CREATE_NON_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, int32_t, Int32ArrayType, 0, voxels, 1)
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportR3DStack::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportR3DStack::execute()
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

  uint8_t* currentPositionPtr = NULL;

  float total = static_cast<float>( m_ZEndIndex - m_ZStartIndex );
  int progress = 0;
  int pixelBytes = 0;
  int totalPixels = 0;
  int height = 0;
  int width = 0;
//  int bytesPerLine = 0;

  int64_t z = m_ZStartIndex;
  for (std::vector<std::string>::iterator filepath = m_R3DFileList.begin(); filepath != m_R3DFileList.end(); ++filepath)
  {
    QString R3DFName = QString::fromStdString(*filepath);
//    progress = static_cast<int>( z - m_ZStartIndex );
//    progress = (int)(100.0f * (float)(progress) / total);

    ss.str("");
	ss << "Importing file " << R3DFName.toStdString();
    notifyStatusMessage(ss.str());

    QByteArray buf;
	QFile in(R3DFName);

	if (!in.open(QIODevice::ReadOnly | QIODevice::Text))
	{
	  QString msg = QString("R3D file could not be opened: ") + R3DFName;
      setErrorCondition(-14000);
      notifyErrorMessage(msg.toStdString(), getErrorCondition());
	}

	buf = in.readLine();

	QList<QByteArray> tokens = buf.split(',');
//    QString word(tokens.at(0));

//    QImage image(QString::fromStdString(R3DFName));
//    if (image.isNull() == true)
//    {
//      setErrorCondition(-14000);
//      notifyErrorMessage("Failed to load Image file", getErrorCondition());
//    }
//    height = image.height();
//    width = image.width();

  //  bytesPerLine = image.bytesPerLine();
    // This is the first slice so we need to create our block of data to store the data

    if (z == m_ZStartIndex)
    {
	  width = tokens.at(0).toInt();
	  height = tokens.at(1).toInt();
      totalPixels = width * height;
      m->setDimensions(width, height, m_R3DFileList.size());
//      if (image.format() == QImage::Format_Indexed8)
//      {
//        pixelBytes = 1;
//      }
//      else if (image.format() == QImage::Format_RGB32 || image.format() == QImage::Format_ARGB32)
//      {
        pixelBytes = 4;
//      }
      data = UInt8ArrayType::CreateArray(totalPixels * m_R3DFileList.size(), pixelBytes, m_GrainIdsArrayName);

    }

    // Get the current position in the array to copy the image into
    currentPositionPtr = data->GetPointer( (z-m_ZStartIndex) * totalPixels * pixelBytes);

	Int32ArrayType::Pointer m_GrainIdData = Int32ArrayType::CreateArray(totalPixels, m_GrainIdsArrayName);
    m_GrainIdData->initializeWithValues(-1);
    int32_t* grainIds = m_GrainIdData->GetPointer(0);
	int index = 0;
	int32_t value = 0;

    for(qint32 i = 0; i < height; ++i)
    {
	  buf = in.readLine();
	  tokens = buf.split(',');
//	  for(qint32 j = 0; j < width-1; ++j)
//	  {
//		index = totalPixels*z + i*width + j;
//		value = tokens.at(j+1).toInt();
//		grainIds[index] = value;
//	  }
      currentPositionPtr = data->GetPointer( (z-m_ZStartIndex) * totalPixels * pixelBytes + i * (width * pixelBytes));
//      uint8_t* source = image.scanLine(i);
      ::memcpy(currentPositionPtr, &tokens, width * pixelBytes);
	  if (in.atEnd() == true)
	  {
		break;
	  }
    }

    ++z;
    if(getCancel() == true)
    {
      notifyStatusMessage("Conversion was Canceled");
      return;
    }
  }




  getVoxelDataContainer()->addCellData(DREAM3D::CellData::GrainIds, data);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage("Complete");
}
