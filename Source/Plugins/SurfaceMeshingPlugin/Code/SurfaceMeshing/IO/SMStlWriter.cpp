/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011, Dr. Michael A. Groeber (US Air Force Research Laboratories
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
#include "SMStlWriter.h"

#include <string.h>

#include "DREAM3DLib/Common/DREAM3DMath.h"


using namespace meshing;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SMStlWriter::SMStlWriter() :
m_FileName(""),
m_TriangleCount(0),
m_File(NULL)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SMStlWriter::~SMStlWriter()
{
  // Close the file if it is non null. Note that we assume that if the m_File
  // variable is NON NULL then the file is still open. We could conceivably have
  // the variable be NON NULL but the file was closed. This may cause errors. Out
  // API should be able to take care of this?
  if (m_File != NULL)
  {
    fclose(m_File);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SMStlWriter::openFile(const char* mode)
{
  if (m_FileName.empty() == true)
  {
    return -1;
  }
  m_File = fopen(m_FileName.c_str(), mode);
  if (NULL == m_File)
  {
    return -2;
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SMStlWriter::closeFile()
{
  if (m_File != NULL)
  {
    fclose(m_File);
    m_File = NULL;
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SMStlWriter::writeHeader(const std::string &header)
{
  if (NULL == m_File)
  {
    return -1;
  }
  char h[80];
  size_t headlength = 80;
  if(header.length() < 80) headlength = header.length();
  ::memset(h, 0, 80);
  ::memcpy(h, header.data(), headlength);
  // Return the number of bytes written - which should be 80
  fwrite(h, 1, 80, m_File);
  fwrite(h, 1, 4, m_File);// Write junk into the 4 bytes that hold the triangle count,
                          //  which will get updated as we write the file
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SMStlWriter::resetTriangleCount()
{
  m_TriangleCount = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SMStlWriter::writeTriangleBlock(int numTriangles, const std::vector<Patch::Pointer>& cTriangle, Node* cVertex)
{

 // std::cout << "Writing " << numTriangles << " Triangles to STL file" << std::endl;
  int err = 0;
  int n1, n2, n3;
  m_TriangleCount += numTriangles;

  unsigned char data[50];
  float* normal = (float*)data;
  float* vert1 = (float*)(data + 12);
  float* vert2 = (float*)(data + 24);
  float* vert3 = (float*)(data + 36);
  uint16_t* attrByteCount = (uint16_t*)(data + 48);
  *attrByteCount = 0;

  size_t totalWritten = 0;
  float u[3], w[3];
  float length;

  // first Open the file

  err = openFile("ab");
  if (err < 0)
  {
    std::cout << "SMStlWriter: Could not open file: '" << m_FileName << "' to write the data into." << std::endl;
    return -1;
  }
  //size_t curPos = 0;

  for (int i = 0; i < numTriangles; i++)
  {
    ::memset(data, 0, 50);
    n1 = cTriangle[i]->node_id[0];
    n2 = cTriangle[i]->node_id[1];
    n3 = cTriangle[i]->node_id[2];

    vert1[0] = cVertex[n1].coord[0];
    vert1[1] = cVertex[n1].coord[1];
    vert1[2] = cVertex[n1].coord[2];

    vert2[0] = cVertex[n2].coord[0];
    vert2[1] = cVertex[n2].coord[1];
    vert2[2] = cVertex[n2].coord[2];

    vert3[0] = cVertex[n3].coord[0];
    vert3[1] = cVertex[n3].coord[1];
    vert3[2] = cVertex[n3].coord[2];

    // Compute the normal
    u[0] = vert2[0] - vert1[0];
    u[1] = vert2[1] - vert1[1];
    u[2] = vert2[2] - vert1[2];
    w[0] = vert3[0] - vert1[0];
    w[1] = vert3[1] - vert1[1];
    w[2] = vert3[2] - vert1[2];
    normal[0] = u[1] * w[2] - u[2] * w[1];
    normal[1] = u[2] * w[0] - u[0] * w[2];
    normal[2] = u[0] * w[1] - u[1] * w[0];
    length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
    normal[0] = normal[0] / length;
    normal[1] = normal[1] / length;
    normal[2] = normal[2] / length;

    totalWritten = fwrite(data, 1, 50, m_File);
    if (totalWritten != 50)
    {
      std::cout << "Error Writing STL File. Not enough elements written. Wrote " << totalWritten << " of 50." << std::endl;
      return -1;
    }
    //    fprintf(f, "%d %d %d %d %d %d\n", newID, n1, n2, n3, s1, s2);
    data[0] = data[0] + 1;
  }
  // Close the file so we do not have too many files open
  this->closeFile();

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SMStlWriter::writeNumTrianglesToFile()
{

  // We need to update the number of triangles in the file which means we need
  // to use C++ because with "C" one can only append to a file and NOT write
  // at any point in the file. Yes this will slow us down even more
  int err =0;

  FILE* out = fopen(m_FileName.c_str(), "r+b");
  fseek(out, 80L, SEEK_SET);
  fwrite( (char*)(&m_TriangleCount), 1, 4, out);
  fclose(out);

  return err;
}



