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

#include "VTKRectilinearGridFileReader.h"

#include <map>

#include "MXA/Common/LogTime.h"

#include "DREAM3D/Common/Constants.h"


#define kBufferSize 1024

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTKRectilinearGridFileReader::VTKRectilinearGridFileReader() :
m_GrainIdScalarName(DREAM3D::VTK::GrainIdScalarName)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTKRectilinearGridFileReader::~VTKRectilinearGridFileReader()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VTKRectilinearGridFileReader::readHeader()
{

  int err = 0;
  if (getInputFileName().empty() == true)
  {
    return -1;
  }

  std::ifstream instream;
  instream.open(getInputFileName().c_str());
  if (!instream.is_open())
  {
    std::cout << logTime() << " vtk file could not be opened: " << getInputFileName() << std::endl;
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
    char text[7];
    char text1[64];
    int n = sscanf(buf, "%s %s", text, text1 );
    if (n < 2)
    {
      std::cout << "Error Reading the type of data set. Was expecting 2 fields but got " << n << std::endl;
      return -1;
    }
    std::string dataset(text1);
    setDatasetType(dataset);
  }

  ::memset(buf, 0, kBufferSize);
  instream.getline(buf, kBufferSize); // Read Line 5 which is the Dimension values
  int dims[3];
  err = parseInt3V(buf, dims, 0);
  setDims(dims);

  setScaling(0.0f, 0.0f, 0.0f);
  setOrigin(0.0f, 0.0f, 0.0f);


  instream.close();
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VTKRectilinearGridFileReader::parseCoordinateLine(const char* input, int &value)
{
  char text[256];
  char text1[256];
  int i = 0;
  int n = sscanf(input, "%s %d %s", text, &i, text1);
  if (n != 3)
  {
    value = -1;
    return -1;
  }
  value = i;
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VTKRectilinearGridFileReader::readFile()
{
  int err = 0;

  err = readHeader();

  std::string filename = getInputFileName();
  std::ifstream instream;
  instream.open(filename.c_str());
  if (!instream.is_open())
  {
    std::cout << logTime() << " vtk file could not be opened: " << filename << std::endl;
    return -1;
  }
  char buf[kBufferSize];
  for (int i = 0; i < 5; ++i)
  {
    instream.getline(buf, kBufferSize);
  }

  int dims[3];
  getDims(dims);


  int dim = 0;
  // Now parse the X, coordinates.
  ::memset(buf, 0, kBufferSize);
  instream.getline(buf, kBufferSize); // Read Line 5 which is the Dimension values
  err = parseCoordinateLine(buf, dim);
  if (err || dim != dims[0])
  {
    return -1;
  }
  err = skipVolume(instream, 4, dim, 1, 1);

  // Now parse the Y coordinates.
  ::memset(buf, 0, kBufferSize);
  instream.getline(buf, kBufferSize); // Read Line 5 which is the Dimension values
  err = parseCoordinateLine(buf, dim);
  if (err || dim != dims[0])
  {
    return -1;
  }
  err = skipVolume(instream, 4, 1, dim, 1);

  // Now parse the Z coordinates.
  ::memset(buf, 0, kBufferSize);
  instream.getline(buf, kBufferSize); // Read Line 5 which is the Dimension values
  err = parseCoordinateLine(buf, dim);
  if (err || dim != dims[0])
  {
    return -1;
  }
  err = skipVolume(instream, 4, 1, 1, dim);



  // Now we need to search for the 'GrainID' and
  char text1[kBufferSize];
  ::memset(text1, 0, kBufferSize);
  char text2[kBufferSize];
  ::memset(text2, 0, kBufferSize);
  char text3[kBufferSize];
  ::memset(text3, 0, kBufferSize);
  int fieldNum = 0;
  bool needGrainIds = true;

  std::string scalarName;
  int typeByteSize = 0;

  size_t index = 0;
  //Cell Data is one less in each direction
  setDims(dims[0] -1, dims[1] -1, dims[2] -1);
  getDims(dims);
  size_t totalVoxels = dims[0] * dims[1] * dims[2];
  m_GrainIds = AIMArray<int>::CreateArray(totalVoxels);



  int i = 0;
  while (needGrainIds == true)
  {
    instream.getline(buf, kBufferSize);
    ::memset(buf, 0, kBufferSize);

    readLine(instream, buf, kBufferSize);
    int n = sscanf(buf, "%s %s %s %d", text1, text2, text3, &fieldNum);
    if (n != 4)
    {
      std::cout << "Error reading SCALARS header section of VTK file." << std::endl;
      return -1;
    }
    scalarName = std::string(text2);
    typeByteSize = parseByteSize(text3);
    if (typeByteSize == 0)
    {
      return -1;
    }
    ::memset(text1, 0, kBufferSize);
    ::memset(text2, 0, kBufferSize);
    ::memset(text3, 0, kBufferSize);
    readLine(instream, buf, kBufferSize); // Read Line 11

    // Check to make sure we are reading the correct set of scalars and if we are
    // NOT then read all this particular Scalar Data and try again

    if (m_GrainIdScalarName.compare(scalarName) == 0)
    {
      std::map<int, int> grainIdMap;
      if (getFileIsBinary() == true)
      {
        instream.read(reinterpret_cast<char*> (m_GrainIds->GetPointer(0)), totalVoxels);
      }
      else // ASCII VTK File
      {
        int grain_index = -1;
        for (size_t i = 0; i < totalVoxels; ++i)
        {
          instream >> grain_index;
          m_GrainIds->SetValue(i, grain_index);
          grainIdMap[grain_index]++;
        }
      }
      needGrainIds = false;
    }
    else
    {
        skipVolume(instream, typeByteSize, dims[0], dims[1], dims[2]);
    }

  }

  instream.close();
  return err;
}
