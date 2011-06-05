/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "VTKFileReader.h"

#include <string.h>

#include <sstream>

#include <boost/shared_ptr.hpp>

#include "MXA/Common/LogTime.h"
#include "MXA/Common/MXAEndian.h"

#include "DREAM3D/DREAM3DConfiguration.h"

#define kBufferSize 1024

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTKFileReader::VTKFileReader()
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
int VTKFileReader::parseFloat3V(const char* input, float* output, float defaultValue)
{
  char text[256];
  int n = sscanf(input, "%s %f %f %f", text, &(output[0]), &(output[1]), &(output[2]) );
  if (n != 4)
  {
    output[0] = output[1] = output[2] = defaultValue;
    return -1;
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VTKFileReader::parseInt3V(const char* input, int* output, int defaultValue)
{
  char text[256];
  int n = sscanf(input, "%s %d %d %d", text, &(output[0]), &(output[1]), &(output[2]) );
  if (n != 4)
  {
    output[0] = output[1] = output[2] = defaultValue;
    return -1;
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VTKFileReader::readLine(std::istream &in, char* buf, int bufSize)
{

  bool readAnotherLine = true;
  while ( readAnotherLine == true && in.gcount() != 0) {
    // Zero out the buffer
    ::memset(buf, 0, bufSize);
    // Read a line up to a '\n' which will catch windows and unix line endings but
    // will leave a trailing '\r' at the end of the string
    in.getline(buf, bufSize, '\n');
    if (buf[in.gcount()-2] == '\r')
    {
      buf[in.gcount()-2] = 0;
    }
    if (strlen(buf) != 0)
    {
      readAnotherLine = false;
    }
  }
  return static_cast<int>(in.gcount());
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
int VTKFileReader::readHeader()
{

  int err = 0;
  if (m_InputFileName.empty() == true)
  {
    return -1;
  }

  std::ifstream instream;
  instream.open(m_InputFileName.c_str());
  if (!instream.is_open())
  {
    std::cout << logTime() << " vtk file could not be opened: " << m_InputFileName << std::endl;
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
  err = parseInt3V(buf, m_Dims, 0);

  ::memset(buf, 0, kBufferSize);
  instream.getline(buf, kBufferSize); // Read Line 6 which is the Origin values
  err = parseFloat3V(buf, m_Origin, 0.0f);

  ::memset(buf, 0, kBufferSize);
  instream.getline(buf, kBufferSize); // Read Line 7 which is the Scaling values
  err = parseFloat3V(buf, m_Scaling, 1.0f);

  ::memset(buf, 0, kBufferSize);


  instream.close();
  return err;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VTKFileReader::skipVolume(std::ifstream &inStream, int byteSize, int xDim, int yDim, int zDim)
{
  int err = 0;
  if (getFileIsBinary() == true)
  {
    int* buffer = new int[xDim];
    for (int z = 0; z < zDim; ++z)
    {
      for (int y = 0; y < yDim; ++y)
      {
        // Read all the xpoints in one shot into a buffer
        inStream.read(reinterpret_cast<char* > (buffer), (xDim * byteSize));
        if (inStream.gcount() != (xDim * byteSize))
        {
          std::cout << logTime() << " ERROR READING BINARY FILE. Bytes read was not the same as func->xDim *. " << byteSize << "." << inStream.gcount()
              << " vs " << (xDim * byteSize) << std::endl;
          return -1;
        }
      }
    }
    delete buffer;
  }
  else
  {
    int tmp;
    for (int z = 0; z < zDim; ++z)
    {
      for (int y = 0; y < yDim; ++y)
      {
        for (int x = 0; x < xDim; ++x)
        {
          inStream >> tmp;
        }
      }
    }
  }
  return err;
}
