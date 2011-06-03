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
#include "STLWriter.h"


#include "DREAM3D/SurfaceMesh/SurfaceMeshFunc.h"
#include "DREAM3D/SurfaceMesh/Patch.h"
#include "DREAM3D/SurfaceMesh/Node.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
STLWriter::STLWriter() :
m_TriangleCount(0),
m_File(NULL)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
STLWriter::~STLWriter()
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
int STLWriter::openFile()
{
  if (m_Filename.empty() == true)
  {
    return -1;
  }
  m_File = fopen(m_Filename.c_str(), "wb");
  if (NULL != m_File)
  {
    return 0;
  }
  return -2;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void STLWriter::closeFile()
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
int STLWriter::writeHeader(const std::string &header)
{
  if (NULL == m_File)
  {
    return -1;
  }
  char h[80];
  int headlength = 80;
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
void STLWriter::resetTriangleCount()
{
  m_TriangleCount = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int STLWriter::writeTriangleBlock(int numTriangles, Patch* cTriangle, Node* cVertex)
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

  for (int i = 0; i < numTriangles; i++)
  {
    ::memset(data, 0, 50);
    n1 = cTriangle[i].node_id[0];
    n2 = cTriangle[i].node_id[1];
    n3 = cTriangle[i].node_id[2];

    vert1[0] = cVertex[n1].xc;
    vert1[1] = cVertex[n1].yc;
    vert1[2] = cVertex[n1].zc;

    vert2[0] = cVertex[n2].xc;
    vert2[1] = cVertex[n2].yc;
    vert2[2] = cVertex[n2].zc;

    vert3[0] = cVertex[n3].xc;
    vert3[1] = cVertex[n3].yc;
    vert3[2] = cVertex[n3].zc;

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

  // Get the current position
  size_t curPos = ftell(m_File);
//  std::cout << "-- curPos:" << curPos << std::endl;

  //Seek to the 80th byte so we can update the number of triangles
  fseek(m_File, 80, SEEK_SET);
 // std::cout << "  Updating TriangleCount in STL File: " << m_TriangleCount << std::endl;
  fwrite(&m_TriangleCount, 4, 1, m_File);

  // Seek back to the last position, which should be the end of the file at this point
  fseek(m_File, curPos, SEEK_SET);

  return err;
}

