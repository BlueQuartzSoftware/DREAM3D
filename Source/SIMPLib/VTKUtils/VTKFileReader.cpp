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
#include "VTKFileReader.h"

#include <string.h>

#include <limits>
#include <sstream>

#include <QtCore/QFile>
#include <QtCore/QByteArray>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#define kBufferSize 1024

// Include the MOC generated file for this class
#include "moc_VTKFileReader.cpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTKFileReader::VTKFileReader() :
  FileReader(),
  m_DataContainerName(DREAM3D::Defaults::DataContainerName),
  m_InputFile("")
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTKFileReader::~VTKFileReader()
{
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t VTKFileReader::parseByteSize(char text[256])
{

  char cunsigned_char [64] = "unsigned_char";
  char cchar [64] = "char";
  char cunsigned_short [64] = "unsigned_short";
  char cshort [64] = "short";
  char cunsigned_int [64] = "unsigned_int";
  char cint [64] = "int";
  char cunsigned_long [64] = " unsigned_long";
  char clong [64] = "long";
  char cfloat [64] = "float";
  char cdouble [64] = " double";

  if (strcmp(text, cunsigned_char) == 0 )
  {
    return 1;
  }
  if (strcmp(text, cchar) == 0 )
  {
    return 1;
  }
  if (strcmp(text, cunsigned_short) == 0 )
  {
    return 2;
  }
  if (strcmp(text, cshort) == 0 )
  {
    return 2;
  }
  if (strcmp(text, cunsigned_int) == 0 )
  {
    return 4;
  }
  if (strcmp(text, cint) == 0 )
  {
    return 4;
  }
  if (strcmp(text, cunsigned_long) == 0 )
  {
    return 8;
  }
  if (strcmp(text, clong) == 0 )
  {
    return 8;
  }
  if (strcmp(text, cfloat) == 0 )
  {
    return 4;
  }
  if (strcmp(text, cdouble) == 0 )
  {
    return  8;
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VTKFileReader::ignoreData(std::ifstream& in, int byteSize, char* text, int xDim, int yDim, int zDim)
{
  char cunsigned_char [64] = "unsigned_char";
  char cchar [64] = "char";
  char cunsigned_short [64] = "unsigned_short";
  char cshort [64] = "short";
  char cunsigned_int [64] = "unsigned_int";
  char cint [64] = "int";
  char cunsigned_long [64] = " unsigned_long";
  char clong [64] = "long";
  char cfloat [64] = "float";
  char cdouble [64] = " double";
  int err = 0;
  if (strcmp(text, cunsigned_char) == 0 )
  {
    err |= skipVolume<unsigned char>(in, byteSize, xDim, yDim, zDim);
  }
  if (strcmp(text, cchar) == 0 )
  {
    err |= skipVolume<char>(in, byteSize, xDim, yDim, zDim);
  }
  if (strcmp(text, cunsigned_short) == 0 )
  {
    err |= skipVolume<unsigned short>(in, byteSize, xDim, yDim, zDim);
  }
  if (strcmp(text, cshort) == 0 )
  {
    err |= skipVolume<short>(in, byteSize, xDim, yDim, zDim);
  }
  if (strcmp(text, cunsigned_int) == 0 )
  {
    err |= skipVolume<unsigned int>(in, byteSize, xDim, yDim, zDim);
  }
  if (strcmp(text, cint) == 0 )
  {
    err |= skipVolume<int>(in, byteSize, xDim, yDim, zDim);
  }
  if (strcmp(text, cunsigned_long) == 0 )
  {
    err |= skipVolume<unsigned long long int>(in, byteSize, xDim, yDim, zDim);
  }
  if (strcmp(text, clong) == 0 )
  {
    err |= skipVolume<long long int>(in, byteSize, xDim, yDim, zDim);
  }
  if (strcmp(text, cfloat) == 0 )
  {
    err |= skipVolume<float>(in, byteSize, xDim, yDim, zDim);
  }
  if (strcmp(text, cdouble) == 0 )
  {
    err |= skipVolume<double>(in, byteSize, xDim, yDim, zDim);
  }
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VTKFileReader::readHeader()
{

  int err = 0;
  if (getInputFile().isEmpty() == true)
  {
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), "FileName was not set and must be valid", -1);
    return -1;
  }

  if (NULL == getDataContainerArray()->getDataContainer(getDataContainerName()).get())
  {
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), "DataContainer Pointer was NULL and must be valid", -1);
    return -1;
  }

  QFile in(getInputFile());
  if (!in.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString msg = QObject::tr("VTF file could not be opened: %1").arg(getInputFile());
    setErrorCondition(-100);
    notifyErrorMessage(getHumanLabel(), msg, getErrorCondition());
    return -100;
  }

  QByteArray buf;
  buf = in.readLine(); // Read Line 1 - VTK Version Info

  buf = in.readLine(); // Read Line 2 - User Comment
  setComment(QString(buf.trimmed()));

  buf = in.readLine(); // Read Line 3 - BINARY or ASCII
  QString fileType(buf);
  if (fileType.startsWith("BINARY") == true)
  {
    setFileIsBinary(true);
  }
  else if (fileType.startsWith("ASCII") == true)
  {
    setFileIsBinary(false);
  }
  else
  {
    err = -1;
    qDebug()
        << "The file type of the VTK legacy file could not be determined. It should be ASCII' or 'BINARY' and should appear on line 3 of the file."
        ;
    return err;
  }


  QList<QByteArray> tokens;
  buf = in.readLine(); // Read Line 4 - Type of Dataset
  {
    tokens = buf.split(' ');
    setDatasetType(QString(tokens[1]));
  }


  buf = in.readLine(); // Read Line 5 which is the Dimension values
  bool ok = false;
  int64_t dims[3];
  tokens = buf.split(' ');
  dims[0] = tokens[1].toLongLong(&ok, 10);
  dims[1] = tokens[2].toLongLong(&ok, 10);
  dims[2] = tokens[3].toLongLong(&ok, 10);
#if   (CMP_SIZEOF_SSIZE_T==4)
  int64_t max = std::numeric_limits<size_t>::max();
#else
  int64_t max = std::numeric_limits<int64_t>::max();
#endif
  if (dims[0] * dims[1] * dims[2] > max )
  {
    err = -1;
    QString ss = QObject::tr("The total number of elements '%1' is greater than this program can hold. Try the 64 bit version.").arg(dims[0] * dims[1] * dims[2]);
    setErrorCondition(err);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return err;
  }

  if (dims[0] > max || dims[1] > max || dims[2] > max)
  {
    err = -1;
    QString ss = QObject::tr("One of the dimensions is greater than the max index for this sysem. Try the 64 bit version. dim[0]=%1  dim[1]=%2im[2]=%3")\
                 .arg(dims[0]).arg(dims[1]).arg(dims[2]);
    setErrorCondition(err);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return err;
  }

  size_t dcDims[3] = { static_cast<size_t>(dims[0]), static_cast<size_t>(dims[1]), static_cast<size_t>(dims[2]) };
  DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(getDataContainerName());
  if (dc.get() == NULL)
  {
    return -1;
  }
  ImageGeom::Pointer image = dc->getGeometryAs<ImageGeom>();
  if (image.get() == NULL)
  {
    return -1;
  }
  image->setDimensions(dcDims);

  buf = in.readLine(); // Read Line 6 which is the Origin values
  float origin[3];
  tokens = buf.split(' ');
  origin[0] = tokens[1].toFloat(&ok);
  origin[1] = tokens[2].toFloat(&ok);
  origin[2] = tokens[3].toFloat(&ok);
  image->setOrigin(origin);

  buf = in.readLine(); // Read Line 7 which is the Scaling values
  float resolution[3];
  tokens = buf.split(' ');
  resolution[0] = tokens[1].toFloat(&ok);
  resolution[1] = tokens[2].toFloat(&ok);
  resolution[2] = tokens[3].toFloat(&ok);
  image->setResolution(resolution);

  return err;

}

