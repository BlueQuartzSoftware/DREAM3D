/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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

#include <sstream>

#include "MXA/Common/LogTime.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

VTKFileUtils::VTKFileUtils()
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
  return in.gcount();
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

int VTKFileUtils::readVtkFile(SurfaceMeshFunc* func, const std::string &file)
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
   9: FIELD FieldData 1
   10: grainid 1 258465900 int
   11:

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
   10:
   11: SCALARS GrainID int  1
   12: LOOKUP_TABLE default
   0
   */
  int err = 0;
  std::cout << logTime() << " Reading vtk ascii file " << file << std::endl;
  std::ifstream in(file.c_str());

  if (!in.is_open())
  {
    std::cout << logTime() << " vtk ascii file could not be opened: " << file << std::endl;
    return -1;
  }
  char buf[kBufferSize];
  in.getline(buf, kBufferSize); // Read Line 1
  in.getline(buf, kBufferSize); // Read Line 2
  ::memset(buf, 0, kBufferSize);
  in.getline(buf, kBufferSize); // Read Line 3
  std::string fileType(buf);

  in.getline(buf, kBufferSize); // Read Line 4
  // Start reading Line 5
  std::string dimension_label;
//  int xpoints = 0;
//  int ypoints = 0;
//  int zpoints = 0;
  in >> dimension_label >>  func->xDim >>  func->yDim >>  func->zDim; // Read Line 5
//  func->zDim = 3;
  std::cout << logTime() << " Volume Size: " << func->xDim << " " << func->yDim << " " << func->zDim << std::endl;
  func->NS = func->xDim * func->yDim * func->zDim;
  func->NSP = func->xDim * func->yDim;

  func->neigh = new Neighbor[2 * func->NSP + 1];
  func->point = new Voxel[func->NS + 1];
  func->cSquare = new Face[3 * 2 * func->NSP];
  func->cVertex = new Node[7 * 2 * func->NSP];
  func->pVertex = new Node[7 * 2 * func->NSP];

  in.getline(buf, kBufferSize); // Read Remainder of Line 5
  ::memset(buf, 0, kBufferSize);
  in.getline(buf, kBufferSize); // Read Line 6 which is the Origin values
  float origin[3];
  err = parseFloat3V(buf, origin, 0.0f);
  func->xOrigin = origin[0];
  func->yOrigin = origin[1];
  func->zOrigin = origin[2];

  ::memset(buf, 0, kBufferSize);
  in.getline(buf, kBufferSize); // Read Line 7 which is the Scaling values
  float scaling[3];
  err = parseFloat3V(buf, scaling, 1.0f);
  func->xRes = scaling[0];
  func->yRes = scaling[1];
  func->zRes = scaling[2];

  ::memset(buf, 0, kBufferSize);
  in.getline(buf, kBufferSize); // Read Line 8
  ::memset(buf, 0, kBufferSize);
  char text1[kBufferSize]; ::memset(text1, 0, kBufferSize);
  char text2[kBufferSize]; ::memset(text2, 0, kBufferSize);
  int fieldNum = 0;
  int nComponents = 0;
  int totalValues = 0;
  size_t i = 1;
  if (fileType.find("BINARY", 0) == 0)
  {

    readLine(in, buf, kBufferSize); // Read Line 9
    int n = sscanf(buf, "%s %s %d", text1, text2, &fieldNum);
    ::memset(text1, 0, kBufferSize);
    ::memset(text2, 0, kBufferSize);

    readLine(in, buf, kBufferSize); // Read Line 10
    n = sscanf(buf, "%s %d %d %s", text1, &nComponents, &totalValues, text2);
    size_t int_byte_size = parseByteSize(text2);


    int* buffer = new int[func->xDim];

    for (int z = 0; z < func->zDim; ++z)
    {
      for (int y = 0; y < func->yDim; ++y)
      {
        // Read all the xpoints in one shot into a buffer
        in.read(reinterpret_cast<char* > (buffer), (func->xDim * int_byte_size));
        if (in.gcount() != (func->xDim * int_byte_size))
        {
          std::cout << logTime() <<
              " ERROR READING BINARY FILE. Bytes read was not the same as func->xDim *. "
              << int_byte_size << "." << in.gcount() << " vs " << (func->xDim * int_byte_size)
              << std::endl;
          return -1;
        }
        for (int x = 0; x < func->xDim; ++x)
        {
          if (buffer[x] <= 0) buffer[x] = -3;
          func->point[i].grainname = buffer[x];
          if ((z == 0 || z == func->zDim - 1)
              || (y == 0 || y == func->yDim - 1)
              || (x == 0 || x == func->xDim - 1))
          {
            func->point[i].grainname = -3;
          }
          ++i; // increment i;
        }
      }
    }
    delete buffer;
    return 1;
  }
  else if (fileType.find("ASCII", 0) == 0)
  {
    readLine(in, buf, kBufferSize);
    readLine(in, buf, kBufferSize);

    int tmp = -1;
    for (int z = 0; z < func->zDim; ++z)
    {
      for (int y = 0; y < func->yDim; ++y)
      {
        for (int x = 0; x < func->xDim; ++x)
        {
          in >> tmp;
          if (tmp <= 0) tmp = -3;
          func->point[i].grainname = tmp;
          if ((z == 0 || z == func->zDim - 1)
              || (y == 0 || y == func->yDim - 1)
              || (x == 0 || x == func->xDim - 1))
          {
            func->point[i].grainname = -3;
          }
          ++i; // increment i;
        }
      }
    }
    return 1;
  }

  std::cout << logTime() << " The file type of the VTK Legacy file should be set to 'ASCII' or 'BINARY'" << " and should appear on line 3 of the file."
      << std::endl;
  return -1;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

void VTKFileUtils::writeVTKFile()
{

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

}

