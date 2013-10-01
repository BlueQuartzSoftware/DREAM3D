/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

#include "VTKFileReader.h"

#include <string.h>

#include <limits>
#include <sstream>

#include <boost/shared_ptr.hpp>

#include "MXA/Common/LogTime.h"
#include "MXA/Common/MXAEndian.h"

#include "DREAM3DLib/DREAM3DLib.h"

#define kBufferSize 1024

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTKFileReader::VTKFileReader() :
FileReader(),
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

  if (strcmp(text, cunsigned_char) == 0 ) { return 1;}
  if (strcmp(text, cchar) == 0 ) { return 1;}
  if (strcmp(text, cunsigned_short) == 0 ) { return 2;}
  if (strcmp(text, cshort) == 0 ) { return 2;}
  if (strcmp(text, cunsigned_int) == 0 ) { return 4;}
  if (strcmp(text, cint) == 0 ) { return 4;}
  if (strcmp(text, cunsigned_long) == 0 ) { return 8;}
  if (strcmp(text, clong) == 0 ) { return 8;}
  if (strcmp(text, cfloat) == 0 ) { return 4;}
  if (strcmp(text, cdouble) == 0 ) { return  8;}
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VTKFileReader::ignoreData(std::ifstream &in, int byteSize, char* text, int xDim, int yDim, int zDim)
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
  if (strcmp(text, cunsigned_char) == 0 ) {
    err |= skipVolume<unsigned char>(in, byteSize, xDim, yDim, zDim);
  }
  if (strcmp(text, cchar) == 0 ) { err |= skipVolume<char>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, cunsigned_short) == 0 ) { err |= skipVolume<unsigned short>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, cshort) == 0 ) {err |= skipVolume<short>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, cunsigned_int) == 0 ) { err |= skipVolume<unsigned int>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, cint) == 0 ) { err |= skipVolume<int>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, cunsigned_long) == 0 ) { err |= skipVolume<unsigned long long int>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, clong) == 0 ) { err |= skipVolume<long long int>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, cfloat) == 0 ) { err |= skipVolume<float>(in, byteSize, xDim, yDim, zDim);}
  if (strcmp(text, cdouble) == 0 ) { err |= skipVolume<double>(in, byteSize, xDim, yDim, zDim);}
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VTKFileReader::readHeader()
{

  int err = 0;
  if (getInputFile().empty() == true)
  {
    setErrorCondition(-1);
    PipelineMessage em (getHumanLabel(), "FileName was not set and must be valid", -1);
    addErrorMessage(em);
    return -1;
  }

  if (NULL == getVolumeDataContainer())
  {
    setErrorCondition(-1);
    PipelineMessage em (getHumanLabel(), "DataContainer Pointer was NULL and must be valid", -1);
    addErrorMessage(em);
    return -1;
  }

  std::ifstream instream;
  instream.open(getInputFile().c_str(), std::ios_base::binary);
  if (!instream.is_open())
  {
    std::cout << logTime() << " vtk file could not be opened: " << getInputFile() << std::endl;
    return -1;
  }
  char buf[kBufferSize];
  instream.getline(buf, kBufferSize); // Read Line 1 - VTK Version Info
  ::memset(buf, 0, kBufferSize);
  instream.getline(buf, kBufferSize); // Read Line 2 - User Comment
  setComment(std::string(buf));
  ::memset(buf, 0, kBufferSize);
  instream.getline(buf, kBufferSize); // Read Line 3 - BINARY or ASCII
  std::string fileType(buf);
  if (fileType.find("BINARY", 0) == 0)
  {
    setFileIsBinary(true);
  }
  else if (fileType.find("ASCII", 0) == 0)
  {
    setFileIsBinary(false);
  }
  else
  {
    err = -1;
    std::cout << logTime()
        << "The file type of the VTK legacy file could not be determined. It should be ASCII' or 'BINARY' and should appear on line 3 of the file."
        << std::endl;
    return err;
  }
  ::memset(buf, 0, kBufferSize);
  instream.getline(buf, kBufferSize); // Read Line 4 - Type of Dataset
  {
    char text[256];
    int n = sscanf(buf, "%s %s", text, &(text[16]) );
    if (n < 2)
    {
      std::cout << "Error Reading the type of data set. Was expecting 2 fields but got " << n << std::endl;
      return -1;
    }
    std::string dataset(&(text[16]));
    setDatasetType(dataset);
  }

  ::memset(buf, 0, kBufferSize);
  instream.getline(buf, kBufferSize); // Read Line 5 which is the Dimension values
  int64_t dims[3];
  err = parse64_3V(buf, dims, 0);
#if   (CMP_SIZEOF_SSIZE_T==4)
    int64_t max = std::numeric_limits<size_t>::max();
#else
    int64_t max = std::numeric_limits<int64_t>::max();
#endif
  if (dims[0] * dims[1] * dims[2] > max )
  {
    err = -1;
    std::stringstream s;
    s << "The total number of elements '" << (dims[0] * dims[1] * dims[2])
                << "' is greater than this program can hold. Try the 64 bit version.";
    setErrorCondition(err);
    addErrorMessage(getHumanLabel(), s.str(), err);
    return err;
  }

  if (dims[0] > max || dims[1] > max || dims[2] > max)
  {
    err = -1;
    std::stringstream s;
    s << "One of the dimensions is greater than the max index for this sysem. Try the 64 bit version.";
    s << " dim[0]="<< dims[0] << "  dim[1]="<<dims[1] << "  dim[2]=" << dims[2];
    setErrorCondition(err);
    addErrorMessage(getHumanLabel(), s.str(), -1);
    return err;
  }

  size_t dcDims[3] = {dims[0], dims[1], dims[2]};
  getVolumeDataContainer()->setDimensions(dcDims);


  ::memset(buf, 0, kBufferSize);
  instream.getline(buf, kBufferSize); // Read Line 6 which is the Origin values
  float origin[3];
  err = parseFloat3V(buf, origin, 0.0f);
  getVolumeDataContainer()->setOrigin(origin);

  ::memset(buf, 0, kBufferSize);
  instream.getline(buf, kBufferSize); // Read Line 7 which is the Scaling values
  float resolution[3];
  err = parseFloat3V(buf, resolution, 1.0f);
  getVolumeDataContainer()->setResolution(resolution);

  ::memset(buf, 0, kBufferSize);

  instream.close();
  return err;

}

