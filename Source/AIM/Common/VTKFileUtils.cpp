/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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
#include "VTKFileUtils.h"

#include <string.h>

#include <sstream>

#include "MXA/Common/LogTime.h"

#include "SurfaceMeshFunc.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

VTKFileUtils::VTKFileUtils() :
m_fileIsBinary(false),
m_HeaderComplete(false),
//m_CurrentSlice(-1),
m_IntByteSize(1)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

VTKFileUtils::~VTKFileUtils()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

int VTKFileUtils::parseFloat3V(const char* input, float* output, float defaultValue)
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
int VTKFileUtils::readLine(std::istream &in, char* buf, int bufSize)
{

  bool readAnotherLine = true;
  while ( readAnotherLine == true) {
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
size_t parseByteSize(char text[256])
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
int VTKFileUtils::readZSlice(SurfaceMeshFunc* m, int zID)
{
  if (m_HeaderComplete == false)
  {
    return -1;
  }
  int start = m->NSP + 1;
  int zEnd = 1;
  if(zID == 0) { // This is the first read so read a pair of slices
    start = 1;
    zEnd = 2;
  }
  // If we are on slice > 0, then copy the Voxels from the top half of the
  // points array to the bottom half
  if(zID > 0)
  {
    for (int i = 1; i <= m->NSP; i++)
    {
      m->point[i].deepCopy(&(m->point[i+m->NSP]));
    }
  }
  int index = start;

  if (m_fileIsBinary == true)
  {
    int* buffer = new int[m->xDim];
   // int i = 1;
    for (int z = 0; z < zEnd; ++z)
    {
      for (int y = 0; y < m->yDim; ++y)
      {
        // Read all the xpoints in one shot into a buffer
        m_InputFile.read(reinterpret_cast<char* > (buffer), (m->xDim * m_IntByteSize));
        if (m_InputFile.gcount() != (m->xDim * m_IntByteSize))
        {
          std::cout << logTime() << " ERROR READING BINARY FILE. Bytes read was not the same as func->xDim *. " << m_IntByteSize << "." << m_InputFile.gcount()
              << " vs " << (m->xDim * m_IntByteSize) << std::endl;
          return -1;
        }
        for (int x = 0; x < m->xDim; ++x)
        {
          if (buffer[x] <= 0) {
            buffer[x] = -3;
          }
          m->point[index].grainname = buffer[x];
          if ((zID == 0 || zID == m->zDim - 2)
              || (y == 0 || y == m->yDim - 1)
              || (x == 0 || x == m->xDim - 1))
          {
            m->point[index].grainname = -3;
          }
          ++index; // increment i;
        }
      }
    }
    delete buffer;
  }
  else
  {
    int tmp = -1;
    for (int z = 0; z < zEnd; ++z)
    {
      for (int y = 0; y < m->yDim; ++y)
      {
        for (int x = 0; x < m->xDim; ++x)
        {
          m_InputFile >> tmp;
          if (tmp <= 0)
            { tmp = -3; }
          m->point[index].grainname = tmp;
          if ((zID == 0 || zID == m->zDim - 2)
              || (y == 0 || y == m->yDim - 1)
              || (x == 0 || x == m->xDim - 1))
          {
            m->point[index].grainname = -3;
          }
          ++index; // increment i;
        }
      }
    }
  }
  return m->zDim;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#if 0
int VTKFileUtils::readNextZSlice(SurfaceMeshFunc* m, int zID)
{
  int error = 0;
  if (m_HeaderComplete == false)
  {
    return -1;
  }

  size_t i = m->NSP + 1;
  if (m_fileIsBinary == true)
  {
    int* buffer = new int[m->xDim];

    //for (int z = 0; z < m->zDim; ++z)

    {
      for (int y = 0; y < m->yDim; ++y)
      {
        // Read all the xpoints in one shot into a buffer
        m_InputFile.read(reinterpret_cast<char* > (buffer), (m->xDim * m_IntByteSize));
        if (m_InputFile.gcount() != (m->xDim * m_IntByteSize))
        {
          std::cout << logTime() << " ERROR READING BINARY FILE. Bytes read was not the same as func->xDim *. " << m_IntByteSize << "." << m_InputFile.gcount()
          << " vs " << (m->xDim * m_IntByteSize) << std::endl;
          return -1;
        }
        for (int x = 0; x < m->xDim; ++x)
        {
          if (buffer[x] <= 0)
          { buffer[x] = -3;}
          m->point[i].grainname = buffer[x];
          if ((zID == 0 || zID == m->zDim - 1)
              || (y == 0 || y == m->yDim - 1)
              || (x == 0 || x == m->xDim - 1))
          {
            m->point[i].grainname = -3;
          }
          ++i; // increment i;
        }
      }
    }
    delete buffer;
  }
  else
  {
    int tmp = -1;

    // for (int z = 0; z < m->zDim; ++z)

    {
      for (int y = 0; y < m->yDim; ++y)
      {
        for (int x = 0; x < m->xDim; ++x)
        {
          m_InputFile >> tmp;
          if (tmp <= 0) tmp = -3;
          Voxel& v = m->point[i];
          v.grainname = tmp;
          if ((zID == 0 || zID == m->zDim - 1) || (y == 0 || y == m->yDim - 1) || (x == 0 || x == m->xDim - 1))
          {
            v.grainname = -3;
          }
          ++i; // increment i;
        }
      }
    }
  }
  return error;
}
#endif


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VTKFileUtils::readHeader(SurfaceMeshFunc* m, const std::string &file)
{
  /*
   1: # vtk DataFile Version 2.0
   2: NRL TiBeta Stack
   3: BINARY
   4: DATASET STRUCTURED_POINTS
   5: DIMENSIONS 1670 770 201
   6: ORIGIN 0.00 0.00 0.00
   7: SPACING 0.665 0.665 1.48
   8: POINT_DATA 258465900
   9:
   10: SCALARS GrainID int  1
   11: LOOKUP_TABLE default

   OR

   1: # vtk DataFile Version 2.0
   2: data set from FFT2dx_GB
   3: ASCII
   4: DATASET STRUCTURED_POINTS
   5: DIMENSIONS 189 201 100
   6: ORIGIN 0.0 0.0 0.0
   7: SPACING 0.25 0.25 0.25
   8: POINT_DATA 3798900
   9:
   10: SCALARS GrainID int  1
   11: LOOKUP_TABLE default
   12:
   0
   */
  int err = 0;
  std::cout << logTime() << " Reading vtk file " << file << std::endl;
  //std::ifstream m_InputFile(file.c_str());

  m_InputFile.open(file.c_str());
  if (!m_InputFile.is_open())
  {
    std::cout << logTime() << " vtk file could not be opened: " << file << std::endl;
    return -1;
  }
  char buf[kBufferSize];
  m_InputFile.getline(buf, kBufferSize); // Read Line 1
  m_InputFile.getline(buf, kBufferSize); // Read Line 2
  ::memset(buf, 0, kBufferSize);
  m_InputFile.getline(buf, kBufferSize); // Read Line 3
  std::string fileType(buf);
  if (fileType.find("BINARY", 0) == 0)
  {
    m_fileIsBinary = true;
  }
  else if (fileType.find("ASCII", 0) == 0)
  {
    m_fileIsBinary = false;
  }
  else
  {
    err = -1;
    std::cout << logTime() << "The file type of the VTK legacy file could not be determined. It should be ASCII' or 'BINARY' and should appear on line 3 of the file."
        << std::endl;
    return err;
  }
  m_InputFile.getline(buf, kBufferSize); // Read Line 4
  // Start reading Line 5
  std::string dimension_label;
//  int xpoints = 0;
//  int ypoints = 0;
//  int zpoints = 0;
  m_InputFile >> dimension_label >>  m->xDim >>  m->yDim >>  m->zDim; // Read Line 5
//  m->zDim = 5;
  std::cout << logTime() << " Volume Size: " << m->xDim << " " << m->yDim << " " << m->zDim << std::endl;
  m->NS = m->xDim * m->yDim * m->zDim;
  m->NSP = m->xDim * m->yDim;

  m->neigh = new Neighbor[2 * m->NSP + 1];
  m->point = new Voxel[2 * m->NSP + 1];
  m->cSquare = new Face[3 * 2 * m->NSP];
  m->cVertex = new Node[2 * 7 * m->NSP];

  m_InputFile.getline(buf, kBufferSize); // Read Remainder of Line 5
  ::memset(buf, 0, kBufferSize);
  m_InputFile.getline(buf, kBufferSize); // Read Line 6 which is the Origin values
  float origin[3];
  err = parseFloat3V(buf, origin, 0.0f);
  m->xOrigin = origin[0];
  m->yOrigin = origin[1];
  m->zOrigin = origin[2];

  ::memset(buf, 0, kBufferSize);
  m_InputFile.getline(buf, kBufferSize); // Read Line 7 which is the Scaling values
  float scaling[3];
  err = parseFloat3V(buf, scaling, 1.0f);
  m->xRes = scaling[0];
  m->yRes = scaling[1];
  m->zRes = scaling[2];
  ::memset(buf, 0, kBufferSize);

  m_InputFile.getline(buf, kBufferSize); // Read Line 8
  ::memset(buf, 0, kBufferSize);

  char text1[kBufferSize]; ::memset(text1, 0, kBufferSize);
  char text2[kBufferSize]; ::memset(text2, 0, kBufferSize);
  char text3[kBufferSize]; ::memset(text3, 0, kBufferSize);
  int fieldNum = 0;
//  int nComponents = 0;
//  int totalValues = 0;
  m_InputFile.getline(buf, kBufferSize); // Read Line 9
  ::memset(buf, 0, kBufferSize);

  readLine(m_InputFile, buf, kBufferSize); // Read Line 10
  int n = sscanf(buf, "%s %s %s %d", text1, text2, text3, &fieldNum);
  m_IntByteSize = parseByteSize(text3);
  if (m_IntByteSize == 0)
  {
    return -1;
  }
  ::memset(text1, 0, kBufferSize);
  ::memset(text2, 0, kBufferSize);
  ::memset(text3, 0, kBufferSize);
  readLine(m_InputFile, buf, kBufferSize); // Read Line 11

  m_HeaderComplete = true;
  err = 1;

  return m->zDim;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

int VTKFileUtils::writeVTKFile(SurfaceMeshFunc* m,
                                int nNodes, int nTriangles,
                                const std::string &VisualizationFile,
                                const std::string &NodesFile,
                                const std::string &TrianglesFile,
                                bool binaryFile)
{

int err = -1;
#if 0
  bool conformalMesh = inputs->conformalMesh;
  bool labelPointData = inputs->labelPointData;
  Label boundingBoxLabel = inputs->boundBox;

  std::cout << logTime() << " Writing VTK File: " << vtkFileName << std::endl;
  NodeVector_Ptr nodes = mesh->nodes();
  std::cout << logTime() << "  Number of Nodes: " << nodes->size() << std::endl;
  std::stringstream ss;
  ss << "M3C-";
  if (inputs->lockTriplePoints)
  { ss << "LockedTJ-";}
  if (inputs->lockQuadPoints)
  { ss << "LockedQuad-";}
  if (inputs->boundBox)
  { ss << "BoundBox-";}

  std::ofstream vtkFile(vtkFileName.c_str(), std::ios::binary);
  vtkFile << "# vtk DataFile Version 2.0" << std::endl;
  vtkFile << ss.str() << std::endl;
  vtkFile << "ASCII" << std::endl;
  vtkFile << "DATASET POLYDATA" << std::endl;
  vtkFile << "POINTS " << nodes->size() << " float" << std::endl;

  for (typename NodeVector::const_iterator iter = nodes->begin(); iter != nodes->end(); ++iter)
  {
    NodeType node = *iter;
    vtkFile << node.position.x << " " << node.position.y << " " << node.position.z << std::endl;
  }

  // Write out the triangles
  TriangleListPtrType triangles = mesh->triangles();

  Label label0 = 0;
  Label label1 = 0;
  char dir0 = 0;
  char dir1 = 0;
  std::map<Label, char > winding;

  std::cout << logTime() << "  Number of triangles: " << triangles->size() << std::endl;
  size_t nTris = mesh->triangles()->size();
  size_t triIndex = 0;
  if (conformalMesh)
  {
    vtkFile << "POLYGONS " << (nTris) << " " << (nTris * 4) << std::endl;
  }
  else
  {
    vtkFile << "POLYGONS " << (nTris * 2) << " " << (nTris * 4 * 2) << std::endl;
  }
  for (typename TriangleListType::const_iterator tri = triangles->begin(); tri != triangles->end(); ++tri)
  {
#if 0
    dir0 = 0;
    dir1 = 0;
    label0 = regionVect->at(triIndex * 2);
    label1 = regionVect->at(triIndex * 2 + 1);
    if (winding.find(label0) == winding.end())
    {
      dir0 = 0;
    }
    else
    {
      dir0 = winding[label0];
    }
    if (winding.find(label1) == winding.end())
    {
      dir1 = 0;
    }
    else
    {
      dir1 = winding[label1];
    }

    // No winding direction has been specified for both labels
    if (dir0 == 0 && dir1 == 0)
    {
      winding[label0] = 1;
      dir0 = 1;
      winding[label1] = 2;
      dir1 = 2;
    }
    else
    {
      if (dir0 != 0 && dir1 == 0) // Direction0 has a preferred winding

      {
        dir1 = dir0 ^ 3; // Makes dir1 either 1 or 2 based on the value of dir0
        winding[label1] = dir1;
      }
      else if (dir1 != 0 && dir0 == 0)
      {
        dir0 = dir1 ^ 3;
        winding[label0] = dir0;
      }
    }
    if (dir0 == 1)
    {
      vtkFile << "3 " << (*tri)->indices[0] << " " << (*tri)->indices[1] << " " << (*tri)->indices[2] << std::endl;
      if (conformalMesh == false)
      {
        vtkFile << "3 " << (*tri)->indices[2] << " " << (*tri)->indices[1] << " " << (*tri)->indices[0] << std::endl;
      }
    }
    else
    {
      vtkFile << "3 " << (*tri)->indices[2] << " " << (*tri)->indices[1] << " " << (*tri)->indices[0] << std::endl;
      if (conformalMesh == false)
      {
        vtkFile << "3 " << (*tri)->indices[0] << " " << (*tri)->indices[1] << " " << (*tri)->indices[2] << std::endl;
      }
    }
#endif
    vtkFile << "3 " << (*tri)->indices[0] << " " << (*tri)->indices[1] << " " << (*tri)->indices[2] << std::endl;
    if (conformalMesh == false)
    {
      vtkFile << "3 " << (*tri)->indices[2] << " " << (*tri)->indices[1] << " " << (*tri)->indices[0] << std::endl;
    }
    ++triIndex;
  }

  if (true == labelPointData)
  {
    vtkFile << "POINT_DATA " << nodes->size() << std::endl;
    vtkFile << "SCALARS Node_Type int 1" << std::endl;
    vtkFile << "LOOKUP_TABLE default" << std::endl;
    for (typename NodeVector::const_iterator iter = nodes->begin(); iter != nodes->end(); ++iter)
    {
      NodeType node = *iter;
      if (node.labels.find(boundingBoxLabel) == node.labels.end() )
      {
        vtkFile << node.labels.size() << std::endl;
      }
      else
      {
        vtkFile << "1" << std::endl;
      }
    }
  }
  if (false == conformalMesh)
  {
    vtkFile << "CELL_DATA " << (regionVect->size()) << std::endl;
    vtkFile << "SCALARS Region_ID int 1" << std::endl;
    vtkFile << "LOOKUP_TABLE default" << std::endl;
    for (typename TriangleListType::const_iterator tri = triangles->begin(); tri != triangles->end(); ++tri)
    {
      vtkFile << (*tri)->labels[0] << std::endl;
      vtkFile << (*tri)->labels[1] << std::endl;
    }
  }
  else
  {
    vtkFile << "CELL_DATA " << (regionVect->size() / 2) << std::endl;
    vtkFile << "SCALARS Region_ID int 2" << std::endl;
    vtkFile << "LOOKUP_TABLE default" << std::endl;
    for (typename TriangleListType::const_iterator tri = triangles->begin(); tri != triangles->end(); ++tri)
    {
      vtkFile << (*tri)->labels[0] << " " << (*tri)->labels[1] << std::endl;
    }
  }
#endif
  return err;
}

