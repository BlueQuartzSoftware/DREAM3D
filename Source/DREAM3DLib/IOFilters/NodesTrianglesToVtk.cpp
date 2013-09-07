/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#include "NodesTrianglesToVtk.h"



#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QtEndian>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NodesTrianglesToVtk::NodesTrianglesToVtk() :
  AbstractFilter(),
  m_NodeTypeArrayName(DREAM3D::VertexData::SurfaceMeshNodeType),
  m_WriteBinaryFile(false),
  m_WriteConformalMesh(true),
  m_NodeKind(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
NodesTrianglesToVtk::~NodesTrianglesToVtk()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NodesTrianglesToVtk::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Nodes File");
    option->setPropertyName("NodesFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Triangles File");
    option->setPropertyName("TrianglesFile");
    option->setWidgetType(FilterParameter::InputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output Vtk File");
    option->setPropertyName("OutputVtkFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setValueType("string");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Binary Vtk File");
    option->setPropertyName("WriteBinaryFile");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Conformal Mesh");
    option->setPropertyName("WriteConformalMesh");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NodesTrianglesToVtk::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setNodesFile( reader->readValue( "NodesFile", getNodesFile() ) );
  setTrianglesFile( reader->readValue( "TrianglesFile", getTrianglesFile() ) );
  setOutputVtkFile( reader->readValue( "OutputVtkFile", getOutputVtkFile() ) );
  setWriteBinaryFile( reader->readValue("WriteBinaryFile", getWriteBinaryFile()) );
  setWriteConformalMesh( reader->readValue("WriteConformalMesh", getWriteConformalMesh()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int NodesTrianglesToVtk::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("NodesFile", getNodesFile() );
  writer->writeValue("TrianglesFile", getTrianglesFile() );
  writer->writeValue("OutputVtkFile", getOutputVtkFile() );
  writer->writeValue("WriteBinaryFile", getWriteBinaryFile() );
  writer->writeValue("WriteConformalMesh", getWriteConformalMesh() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NodesTrianglesToVtk::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);

  QFileInfo fi(m_TrianglesFile);
  if (m_TrianglesFile.isEmpty() == true)
  {
    setErrorCondition(-1001);
    addErrorMessage(getHumanLabel(), "Triangles file is not set correctly", -1001);
  }
  else if (fi.exists() == false)
  {

    if (preflight == true)
      addWarningMessage(getHumanLabel(), "Triangles file does not exist currently.\nYou must have another filter that creates these files before this filter in your pipeline", -1004);
    else {
      setErrorCondition(-1001);
      addErrorMessage(getHumanLabel(), "Triangles file does not exist currently.\nYou must have another filter that creates these files before this filter in your pipeline", -1004);
    }
  }

  QFileInfo fii(m_NodesFile);
  if (m_NodesFile.isEmpty() == true)
  {
    setErrorCondition(-1002);
    addErrorMessage(getHumanLabel(), "Nodes file path or name is emtpy", -1002);
  }
  else if (fii.exists() == false)
  {

    if (preflight == true)
      addWarningMessage(getHumanLabel(), "Nodes file does not exist currently. You must have another filter that creates these files before this filter in your pipeline", -1005);
    else {
      setErrorCondition(-1002);
      addErrorMessage(getHumanLabel(), "Nodes file does not exist currently. You must have another filter that creates these files before this filter in your pipeline", -1005);
    }
  }

  if (m_OutputVtkFile.isEmpty() == true)
  {
    setErrorCondition(-1003);
    addErrorMessage(getHumanLabel(), "Vtk Output file is Not set correctly", -1003);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NodesTrianglesToVtk::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void NodesTrianglesToVtk::execute()
{
  int err = 0;
  QString ss;

  dataCheck(false, 1, 1, 1);
  if (getErrorCondition() < 0)
  {
    return;
  }

  // Open the Nodes file for reading
  FILE* nodesFile = fopen(m_NodesFile.toLatin1().data(), "rb+");
  if (nodesFile == NULL)
  {

    QString ss = QObject::tr("Error opening nodes file '%1'").arg(m_NodesFile);
    setErrorCondition(-1);
    PipelineMessage em(getHumanLabel(), ss, -666);
    addErrorMessage(em);
    return;
  }

  //  how many nodes are in the file
  int nNodes = 0;
  fscanf(nodesFile, "%d", &nNodes);
  {
    QString ss = QObject::tr("Node Count from %1 File: %2").arg(getNodesFile()).arg(nNodes);
    notifyStatusMessage(ss);
  }

  // Open the triangles file for reading
  FILE* triFile = fopen(m_TrianglesFile.toLatin1().data(), "rb+");
  if (triFile == NULL)
  {

    QString ss = QObject::tr(": Error opening Triangles file '%1'").arg(m_TrianglesFile);
    setErrorCondition(-1);
    PipelineMessage em(getHumanLabel(), ss, -666);
    addErrorMessage(em);
    return;
  }
  // how many triangles are in the file
  int nTriangles = 0;
  fscanf(triFile, "%d", &nTriangles);

  {
    QString ss = QObject::tr("Triangle Count from %1 File: %2").arg(getTrianglesFile()).arg(nTriangles);
    notifyStatusMessage(ss);
  }
  // Open the output VTK File for writing
  FILE* vtkFile = NULL;
  vtkFile = fopen(getOutputVtkFile().toLatin1().data(), "wb");
  if (NULL == vtkFile)
  {

    QString ss = QObject::tr(": Error creating Triangles VTK Visualization '%1'").arg(getOutputVtkFile());
    setErrorCondition(-1);
    PipelineMessage em(getHumanLabel(), ss, -666);
    addErrorMessage(em);
    return;
  }
  fprintf(vtkFile, "# vtk DataFile Version 2.0\n");
  fprintf(vtkFile, "Data set from DREAM.3D Surface Meshing Module\n");
  if (m_WriteBinaryFile) {
    fprintf(vtkFile, "BINARY\n");
  }
  else {
    fprintf(vtkFile, "ASCII\n");
  }
  fprintf(vtkFile, "DATASET POLYDATA\n");
  fprintf(vtkFile, "POINTS %d float\n", nNodes);

  int nodeId = 0;
  int nodeKind = 0;
  float pos[3] = {0.0f, 0.0f, 0.0f};


  size_t totalWritten = 0;
  size_t nread = 0;
  // Write the POINTS data (Vertex)
  for (int i = 0; i < nNodes; i++)
  {
    nread = fscanf(nodesFile, "%d %d %f %f %f", &nodeId, &nodeKind, pos, pos+1, pos+2); // Read one set of positions from the nodes file
    if (nread != 5)
    {
      break;
    }
    if (m_WriteBinaryFile == true) {
      pos[0] = qToBigEndian(pos[0]);
      pos[1] = qToBigEndian(pos[1]);
      pos[2] = qToBigEndian(pos[2]);
      totalWritten = fwrite(pos, sizeof(float), 3, vtkFile);
      if (totalWritten != sizeof(float) * 3)
      {

      }
    }
    else {
      fprintf(vtkFile, "%f %f %f\n", pos[0], pos[1], pos[2]); // Write the positions to the output file
    }
  }
  fclose(nodesFile);

  // Write the triangle indices into the vtk File
  // column 1 = triangle id, starts from zero
  // column 2 to 4 = node1, node2 and node3 of individual triangles
  // column 5 to 7 = edge1 (from node1 and node2), edge2 (from node2 and node3) and edge3 (from node3 and node1) of individual triangle
  // column 8 and 9 = neighboring spins of individual triangles, column 8 = spins on the left side when following winding order using right hand.
  int tData[9];
  int triangleCount = nTriangles;
  if (false == m_WriteConformalMesh)
  {
    triangleCount = nTriangles * 2;
  }
  // Write the CELLS Data
  fprintf(vtkFile, "POLYGONS %d %d\n", triangleCount, (triangleCount * 4));
  for (int i = 0; i < nTriangles; i++)
  {
    // Read from the Input Triangles Temp File
    nread = fscanf(triFile, "%d %d %d %d %d %d %d %d %d", tData, tData+1,tData+2,tData+3,tData+4,tData+5,tData+6,tData+7,tData+8 );
    if (m_WriteBinaryFile == true)
    {
      tData[0] = 3; // Push on the total number of entries for this entry
      tData[0] = qToBigEndian(tData[0]);
      tData[1] = qToBigEndian(tData[1]);
      tData[2] = qToBigEndian(tData[2]);
      tData[3] = qToBigEndian(tData[3]);
      fwrite(tData, sizeof(int), 4, vtkFile);
      if (false == m_WriteConformalMesh)
      {
        tData[0] = tData[1];
        tData[1] = tData[3];
        tData[3] = tData[0];
        tData[0] = 3;
        tData[0] = qToBigEndian(tData[0]);
        fwrite(tData, sizeof(int), 4, vtkFile);
      }
    }
    else
    {
      fprintf(vtkFile, "3 %d %d %d\n", tData[1], tData[2], tData[3]);
      if (false == m_WriteConformalMesh)
      {
        fprintf(vtkFile, "3 %d %d %d\n", tData[3], tData[2], tData[1]);
      }
    }
  }
  fclose(triFile);


  // Write the CELL_DATA section
  if (m_WriteBinaryFile == true)
  {
    err = writeBinaryCellData(m_TrianglesFile, vtkFile, nTriangles, m_WriteConformalMesh);
  }
  else
  {
    err = writeASCIICellData(m_TrianglesFile, vtkFile, nTriangles, m_WriteConformalMesh);
  }


  // Write the POINT_DATA section
  if (m_WriteBinaryFile == true)
  {
    err = writeBinaryPointData(m_NodesFile, vtkFile, nNodes, m_WriteConformalMesh);
  }
  else
  {
    err = writeASCIIPointData(m_NodesFile, vtkFile, nNodes, m_WriteConformalMesh);
  }



  fprintf(vtkFile, "\n");
  // Free the memory
  // Close the input and output files
  fclose(vtkFile);

  setErrorCondition(0);
  notifyStatusMessage("Complete");

  return;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int NodesTrianglesToVtk::writeBinaryPointData(const QString &NodesFile, FILE* vtkFile, int nNodes, bool conformalMesh)
{
  //# first line = number of nodes
  //# column 1 = node id, starts from zero.
  //# column 2 = node type, 2 = on the grain boundary, 3 = on the triple line, 4 = quadruple point or on quadruple line
  //#                       if 10 is added, eg 12, 13, and 14, then they are on the surface of microstructure.
  //#                       For the nodes on surfaces of microstructure, negative wrapper spins are treated as neighboring grains.
  //#                       12 = on the grain boundary trace area at the surface of microstructure
  //#                       13 = on the grain boundary trace line
  //#                 14 = on the triple point, and so on‚àö√¢
  //# column 3 to 5 = coordinates of nodes, x, y, and z

  int err = 0;
  int nodeId = 0;
  int nodeKind = 0;
  float pos[3] = {0.0f, 0.0f, 0.0f};
  int swapped;
  int nread = 0;
  FILE* nodesFile = fopen(NodesFile.toLatin1().data(), "rb");
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "POINT_DATA %d\n", nNodes);
  fprintf(vtkFile, "SCALARS Node_Type int 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");
  fscanf(nodesFile, "%d", &nodeId); // Read the number of nodes
  std::vector<int> data (nNodes, 0);
  for (int i = 0; i < nNodes; i++)
  {
    nread = fscanf(nodesFile, "%d %d %f %f %f", &nodeId, &nodeKind, pos, pos+1, pos+2); // Read one set of positions from the nodes file
    if (nread != 5)
    {
      break;
    }
    swapped = nodeKind;
    swapped = qToBigEndian(swapped);

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
int NodesTrianglesToVtk::writeASCIIPointData(const QString &NodesFile, FILE* vtkFile, int nNodes, bool conformalMesh)
{
  int err = 0;
  int nodeId = 0;
  int nodeKind = 0;
  float pos[3] = {0.0f, 0.0f, 0.0f};
  int nread = 0;


  FILE* nodesFile = fopen(NodesFile.toLatin1().data(), "rb");
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "POINT_DATA %d\n", nNodes);
  fprintf(vtkFile, "SCALARS Node_Type int 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");
  fscanf(nodesFile, "%d", &nodeId); // Read the number of nodes
  for (int i = 0; i < nNodes; i++)
  {
    nread = fscanf(nodesFile, "%d %d %f %f %f", &nodeId, &nodeKind, pos, pos+1, pos+2); // Read one set of positions from the nodes file
    if (nread != 5)
    {
      break;
    }
    fprintf(vtkFile, "%d\n", nodeKind); // Write the Node Kind to the output file
  }

  // Close the input files
  fclose(nodesFile);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int NodesTrianglesToVtk::writeBinaryCellData(const QString &TrianglesFile, FILE* vtkFile, int nTriangles, bool conformalMesh)
{
  //# first line = number of triangles
  //# column 1 = triangle id, starts from zero
  //# column 2 to 4 = node1, node2 and node3 of individual triangles
  //# column 5 to 7 = edge1 (from node1 and node2), edge2 (from node2 and node3) and edge3 (from node3 and node1) of individual triangle
  //# column 8 and 9 = neighboring spins of individual triangles, column 8 = spins on the left side when following winding order using right hand.
  //
  int err = 0;
  size_t offset = 1;
  int nread = 0;
  // Open the triangles file for reading
  FILE* triFile = fopen(TrianglesFile.toLatin1().data(), "rb");
  fscanf(triFile, "%d", &nread); // Read the number of triangles and throw it away
  int tData[9];

  int triangleCount = nTriangles;
  if (false == conformalMesh)
  {
    triangleCount = nTriangles * 2;
    offset = 2;
  }
  std::vector<int> tri_ids(triangleCount);
  // Write the GrainId Data to the file
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "CELL_DATA %d\n", triangleCount);
  fprintf(vtkFile, "SCALARS GrainID int 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");


  std::vector<int> cell_data(triangleCount);
  for (int i = 0; i < nTriangles; i++)
  {
    nread = fscanf(triFile, "%d %d %d %d %d %d %d %d %d", tData, tData+1,tData+2,tData+3,tData+4,tData+5,tData+6,tData+7,tData+8 );
    if (nread != 9) {
      return -1;
    }
    tData[0] = qToBigEndian(tData[0]);
    tri_ids[i*offset] = tData[0];
    tData[7] = qToBigEndian(tData[7]);
    cell_data[i*offset] = tData[7];
    if (false == conformalMesh)
    {
      tData[8] = qToBigEndian(tData[8]);
      cell_data[i*offset + 1] = tData[8];
      tri_ids[i*offset + 1] = tData[0];
    }
  }

  int totalWritten = fwrite( &(cell_data.front()), sizeof(int), triangleCount, vtkFile);
  if (totalWritten != triangleCount)
  {
    return -1;
  }

  // Now write the original Triangle Ids for Debugging in ParaView
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "SCALARS TriangleID int 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");

  totalWritten = fwrite( &(tri_ids.front()), sizeof(int), triangleCount, vtkFile);
  if (totalWritten != triangleCount)
  {
    return -1;
  }

  fclose(triFile);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int NodesTrianglesToVtk::writeASCIICellData(const QString &TrianglesFile, FILE* vtkFile, int nTriangles, bool conformalMesh)
{
  int nread = 0;
  // Open the triangles file for reading
  FILE* triFile = fopen(TrianglesFile.toLatin1().data(), "rb");
  fscanf(triFile, "%d", &nread); // Read the number of triangles and throw it away

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

  int tData[9];
  for (int i = 0; i < nTriangles; i++)
  {
    nread = fscanf(triFile, "%d %d %d %d %d %d %d %d %d", tData, tData+1,tData+2,tData+3,tData+4,tData+5,tData+6,tData+7,tData+8 );
    if (nread != 9) {
      return -1;
    }
    fprintf(vtkFile, "%d\n", tData[7]);
    if (false == conformalMesh)
    {
      fprintf(vtkFile, "%d\n", tData[8]);
    }
  }
  fclose(triFile);
  return 0;
}

