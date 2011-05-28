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
#include "SMVtkFileIO.h"

#include <string.h>

#include <sstream>

#include "MXA/Common/LogTime.h"
#include "MXA/Common/MXAEndian.h"

#include "AIM/Common/SurfaceMeshFunc.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

SMVtkFileIO::SMVtkFileIO() :
m_fileIsBinary(false),
m_HeaderComplete(false),
//m_CurrentSlice(-1),
m_IntByteSize(1)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

SMVtkFileIO::~SMVtkFileIO()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SMVtkFileIO::readZSlice(int xDim, int yDim, int zDim, int* voxels)
{
  if (m_HeaderComplete == false)
  {
    return -1;
  }
  size_t size = xDim * yDim;
  if (m_fileIsBinary == true)
  {
    if (m_IntByteSize != 4)
    {
      std::cout << "ERROR Reading Binary VTK File. This program only accepts a type of Int" << std::endl;
      return -1;
    }
    m_InputFile.read(reinterpret_cast<char*>(voxels), (size * m_IntByteSize));
    // Binary VTK File are written in Big Endian Format and need to be swapped to the native
    // format of the host system
    for(size_t i = 0; i < size; ++i)
    {
      MXA::Endian::FromBigToSystem::convert<int>(voxels[i]);
	  if(voxels[i] > 1000 || voxels[i] < -5)
	  {
		int stop = 0;
	  }
    }
  }
  else
  {
    int tmp = -1;
    for(size_t i = 0; i < size; ++i)
    {
      m_InputFile >> tmp;
      voxels[i] = tmp;
	}
  }
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SMVtkFileIO::primeFileToScalarDataLocation(SurfaceMeshFunc* m, const std::string &file, const std::string &scalarName)
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
   10: SCALARS GrainID int 1
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
   10: SCALARS GrainID int 1
   11: LOOKUP_TABLE default
   12:
   0
   */
  int err = 0;
  //std::cout << logTime() << " Reading vtk file " << file << std::endl;
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
  int xpoints = 0;
  int ypoints = 0;
  int zpoints = 0;
  m_InputFile >> dimension_label >>  xpoints >>  ypoints >>  zpoints; // Read Line 5

  // Add a layer of padding around the volume which are going to be our boundary voxels
  m->xDim = xpoints + 2;
  m->yDim = ypoints + 2;
  m->zDim = zpoints + 2;

  m->NS = m->xDim * m->yDim * m->zDim;
  m->NSP = m->xDim * m->yDim;

  m->neigh = new Neighbor[2 * m->NSP + 1];
  m->voxels = new int[2 * m->NSP + 1];
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

  bool keepReading = true;
  while (keepReading)
  {
    m_InputFile.getline(buf, kBufferSize); // Read Line 9
    ::memset(buf, 0, kBufferSize);

    readLine(m_InputFile, buf, kBufferSize); // Read Line 10
    int n = sscanf(buf, "%s %s %s %d", text1, text2, text3, &fieldNum);
    if (n != 4)
    {
      std::cout << "Error reading SCALARS header section of VTK file." << std::endl;
      return -1;
    }
    m_ScalarName = std::string(text2);
    m_IntByteSize = parseByteSize(text3);
    if (m_IntByteSize == 0)
    {
      return -1;
    }
    ::memset(text1, 0, kBufferSize);
    ::memset(text2, 0, kBufferSize);
    ::memset(text3, 0, kBufferSize);
    readLine(m_InputFile, buf, kBufferSize); // Read Line 11

    // Check to make sure we are reading the correct set of scalars and if we are
    // NOT then read all this particular Scalar Data and try again
    if (m_ScalarName.compare(scalarName) != 0)
    {
      for (int z = 0; z < zpoints - 1; ++z)
      {
        skipVolume(m_InputFile, m_IntByteSize, xpoints, ypoints, zpoints);
      }
    }
    else
    {
      keepReading = false;
    }
  }
  // We are now queued up to read the data
  m_HeaderComplete = true;
  err = 1;

  return m->zDim;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

int SMVtkFileIO::writeVTKFile(SurfaceMeshFunc* m,
                             int nNodes, int nTriangles,
                             const std::string &VisualizationFile,
                             const std::string &NodesFile,
                             const std::string &TrianglesFile,
                             bool binaryFile,
                             bool conformalMesh)
{
  // Open the Nodes file for reading
  FILE* nodesFile = fopen(NodesFile.c_str(), "rb");
  // Open the triangles file for reading
  FILE* triFile = fopen(TrianglesFile.c_str(), "rb");

  // Open the output VTK File for writing
  FILE* vtkFile = NULL;
  int err = 0;

  vtkFile = fopen(VisualizationFile.c_str(), "wb");
  if (NULL == vtkFile)
  {
    std::cout << "Error Creating VTK Visualization File '" << VisualizationFile << "'" << std::endl;
    return -1;
  }
  fprintf(vtkFile, "# vtk DataFile Version 2.0\n");
  fprintf(vtkFile, "Data set from DREAM.3D Surface Meshing Module\n");
  if (binaryFile) {
    fprintf(vtkFile, "BINARY\n");
  }
  else {
    fprintf(vtkFile, "ASCII\n");
  }
  fprintf(vtkFile, "DATASET POLYDATA\n");
  fprintf(vtkFile, "POINTS %d float\n", nNodes);
  unsigned char nodeData[32];
  float* vec3d = (float*)(&nodeData[8]);
//  int* nodeId = (int*)(&nodeData[0]);
//  int* nodeKind = (int*)(&nodeData[4]);
  float vec3f[3];
  size_t totalWritten = 0;

  // Write the POINTS data (Vertex)
  for (int i = 0; i < nNodes; i++)
  {
    fread(nodeData, 32, 1, nodesFile); // Read one set of positions from the nodes file
    if (binaryFile == true) {
      vec3f[0] = vec3d[0]; vec3f[1] = vec3d[1]; vec3f[2] = vec3d[2];
      MXA::Endian::FromSystemToBig::convert<float>(vec3f[0]);
      MXA::Endian::FromSystemToBig::convert<float>(vec3f[1]);
      MXA::Endian::FromSystemToBig::convert<float>(vec3f[2]);
      totalWritten = fwrite(vec3f, sizeof(float), 3, vtkFile);
    }
    else {
      fprintf(vtkFile, "%f %f %f\n", vec3d[0], vec3d[1], vec3d[2]); // Write the positions to the output file
    }
  }
  fclose(nodesFile);

  // Write the triangle indices into the vtk File
  int tData[6];
  int triangleCount = nTriangles;
  if (false == conformalMesh)
  {
    triangleCount = nTriangles * 2;
  }
  // Write the CELLS Data
  fprintf(vtkFile, "POLYGONS %d %d\n", triangleCount, (triangleCount * 4));
  for (int i = 0; i < nTriangles; i++)
  {
    // Read from the Input Triangles Temp File
    fread(tData, sizeof(int), 6, triFile);
    if (binaryFile == true)
    {
      tData[0] = 3; // Push on the total number of entries for this entry
      MXA::Endian::FromSystemToBig::convert<int>(tData[0]);
      MXA::Endian::FromSystemToBig::convert<int>(tData[1]); // Index of Vertex 0
      MXA::Endian::FromSystemToBig::convert<int>(tData[2]); // Index of Vertex 1
      MXA::Endian::FromSystemToBig::convert<int>(tData[3]); // Index of Vertex 2
      fwrite(tData, sizeof(int), 4, vtkFile);
      if (false == conformalMesh)
      {
        tData[0] = tData[1];
        tData[1] = tData[3];
        tData[3] = tData[0];
        tData[0] = 3;
        MXA::Endian::FromSystemToBig::convert<int>(tData[0]);
        fwrite(tData, sizeof(int), 4, vtkFile);
      }
    }
    else
    {
      fprintf(vtkFile, "3 %d %d %d\n", tData[1], tData[2], tData[3]);
      if (false == conformalMesh)
      {
        fprintf(vtkFile, "3 %d %d %d\n", tData[3], tData[2], tData[1]);
      }
    }
  }
  fclose(triFile);
#if 0
  // Write the CELL_TYPES into the file
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "CELL_TYPES %d\n", triangleCount);
  if (binaryFile == true)
  {
    std::vector<int> cellTypes(triangleCount, 5);
    fwrite( &(cellTypes.front()), sizeof(int), triangleCount, vtkFile);
  }
  else
  {
    char sBuf[4];
    if (conformalMesh == true)
    {
      sBuf[0] = '5'; sBuf[1] =0; sBuf[2] = 0; sBuf[3] = 0;
    }
    else {
      sBuf[0] = '5'; sBuf[1] =0; sBuf[2] = '5'; sBuf[3] = 0;
    }
    for (int i = 0; i < triangleCount; i++)
    {
      fprintf(vtkFile, "%s ", sBuf);
      if (triangleCount % 80 == 0) { fprintf(vtkFile, "\n"); }
    }
  }
#endif

  // Write the CELL_DATA section
  if (binaryFile == true)
  {
    err = writeBinaryCellData(TrianglesFile, vtkFile, nTriangles, conformalMesh);
  }
  else
  {
    err = writeASCIICellData(TrianglesFile, vtkFile, nTriangles, conformalMesh);
  }


  // Write the POINT_DATA section
  if (binaryFile == true)
  {
    err = writeBinaryPointData(NodesFile, vtkFile, nNodes, conformalMesh);
  }
  else
  {
    err = writeASCIIPointData(NodesFile, vtkFile, nNodes, conformalMesh);
  }



  fprintf(vtkFile, "\n");
  // Free the memory
  // Close the input and output files
  fclose(vtkFile);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SMVtkFileIO::writeBinaryPointData(const std::string &NodesFile, FILE* vtkFile, int nNodes, bool conformalMesh)
{
  int err = 0;
  unsigned char nodeData[32];
  int* nodeKind = (int*)(&nodeData[4]);
  int swapped;
  FILE* nodesFile = fopen(NodesFile.c_str(), "rb");
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "POINT_DATA %d\n", nNodes);
  fprintf(vtkFile, "SCALARS Node_Type int 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");

  std::vector<int> data (nNodes, 0);
  for (int i = 0; i < nNodes; i++)
  {
    fread(nodeData, 32, 1, nodesFile); // Read one set of Node Kind from the nodes file
    swapped = *nodeKind;
    MXA::Endian::FromSystemToBig::convert<int>( swapped );
    data[i] = swapped;
  }
  int totalWritten = fwrite( &(data.front()), sizeof(int), nNodes, vtkFile);
  fclose(nodesFile);
  if (totalWritten != nNodes)
  {
    return -1;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SMVtkFileIO::writeASCIIPointData(const std::string &NodesFile, FILE* vtkFile, int nNodes, bool conformalMesh)
{
  int err = 0;
  unsigned char nodeData[32];
 // double* vec3d = (double*)(&nodeData[8]);
 // int* nodeId = (int*)(&nodeData[0]);
  int* nodeKind = (int*)(&nodeData[4]);
 // float vec3f[3];

  FILE* nodesFile = fopen(NodesFile.c_str(), "rb");
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "POINT_DATA %d\n", nNodes);
  fprintf(vtkFile, "SCALARS Node_Type int 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");
  for (int i = 0; i < nNodes; i++)
  {
    fread(nodeData, 32, 1, nodesFile); // Read one set of Node Kind from the nodes file
    fprintf(vtkFile, "%d\n", *nodeKind); // Write the Node Kind to the output file
  }

// Close the input files
  fclose(nodesFile);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SMVtkFileIO::writeBinaryCellData(const std::string &TrianglesFile, FILE* vtkFile, int nTriangles, bool conformalMesh)
{
  int err = 0;
  size_t offset = 1;
  // Open the triangles file for reading
  FILE* triFile = fopen(TrianglesFile.c_str(), "rb");
  int triangleCount = nTriangles;
  if (false == conformalMesh)
  {
    triangleCount = nTriangles * 2;
    offset = 2;
  }
  // Write the GrainId Data to the file
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "CELL_DATA %d\n", triangleCount);
  fprintf(vtkFile, "SCALARS GrainID int 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");
  int tData[6];

  std::vector<int> cell_data(triangleCount);
  for (int i = 0; i < nTriangles; i++)
  {
    fread(tData, sizeof(int), 6, triFile);
    MXA::Endian::FromSystemToBig::convert<int>(tData[4]);
    cell_data[i*offset] = tData[4];
    if (false == conformalMesh)
    {
      MXA::Endian::FromSystemToBig::convert<int>(tData[5]);
      cell_data[i*offset + 1] = tData[5];
    }
  }

  int totalWritten = fwrite( &(cell_data.front()), sizeof(int), triangleCount, vtkFile);
  fclose(triFile);
  if (totalWritten != triangleCount)
  {
    return -1;
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SMVtkFileIO::writeASCIICellData(const std::string &TrianglesFile, FILE* vtkFile, int nTriangles, bool conformalMesh)
{
  // Open the triangles file for reading
  FILE* triFile = fopen(TrianglesFile.c_str(), "rb");
  // Write the GrainId Data to the file
  int triangleCount = nTriangles;
  if (false == conformalMesh)
  {
    triangleCount = nTriangles * 2;
  }
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "CELL_DATA %d\n", triangleCount);
  fprintf(vtkFile, "SCALARS GrainID int 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");

  int tData[6];
  for (int i = 0; i < nTriangles; i++)
  {
    fread(tData, sizeof(int), 6, triFile);
    fprintf(vtkFile, "%d\n", tData[4]);
    if (false == conformalMesh)
    {
      fprintf(vtkFile, "%d\n", tData[5]);
    }
  }
  fclose(triFile);
  return 0;
}
