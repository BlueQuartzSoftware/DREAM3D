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

#include "SurfaceMeshToNonconformalVtk.h"


#include "MXA/Common/MXAEndian.h"
#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/MXADir.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshToNonconformalVtk::SurfaceMeshToNonconformalVtk() :
  AbstractFilter(),
  m_WriteBinaryFile(false)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshToNonconformalVtk::~SurfaceMeshToNonconformalVtk()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNonconformalVtk::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> options;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Output Vtk File");
    option->setPropertyName("OutputVtkFile");
    option->setWidgetType(FilterParameter::OutputFileWidget);
    option->setValueType("string");
    options.push_back(option);
  }
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Write Binary Vtk File");
    option->setPropertyName("WriteBinaryFile");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    options.push_back(option);
  }
  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNonconformalVtk::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("OutputVtkFile", getOutputVtkFile() );
  writer->writeValue("WriteBinaryFile", getWriteBinaryFile() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNonconformalVtk::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;

  if (m_OutputVtkFile.empty() == true)
  {
    setErrorCondition(-1003);
    addErrorMessage(getHumanLabel(), "Vtk Output file is Not set correctly", -1003);
  }

  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if (NULL == sm)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", -383);
    setErrorCondition(-384);
  }
  else
  {
    if (sm->getFaces().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", -383);
      setErrorCondition(-384);
    }

    if (sm->getVertices().get() == NULL)
    {
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", -384);
      setErrorCondition(-384);
    }
    if (sm->getFaceData(DREAM3D::FaceData::SurfaceMeshTriangleLabels).get() == NULL)
    {
      setErrorCondition(-385);
      addErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangle Label array", getErrorCondition());
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNonconformalVtk::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
  * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

/**
 * @brief The ScopedFileMonitor class will automatically close an open FILE pointer
 * when the object goes out of scope.
 */
class ScopedFileMonitor
{
  public:
    ScopedFileMonitor(FILE* f) : m_File(f) {}
    virtual ~ScopedFileMonitor() { fclose(m_File);}
  private:
    FILE* m_File;
    ScopedFileMonitor(const ScopedFileMonitor&); // Copy Constructor Not Implemented
    void operator=(const ScopedFileMonitor&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToNonconformalVtk::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  dataCheck(false, 0, 0, 0);
  if(getErrorCondition() < 0)
  {
    return;
  }


  setErrorCondition(0);
  SurfaceMeshDataContainer* m = getSurfaceMeshDataContainer();

  StructArray<SurfaceMesh::DataStructures::Vert_t>::Pointer nodesPtr = m->getVertices();
  StructArray<SurfaceMesh::DataStructures::Vert_t>& nodes = *(nodesPtr);
  int nNodes = nodes.GetNumberOfTuples();

  // Make sure we have a node type array or create a default one.
  DataArray<int8_t>::Pointer nodeTypeSharedPtr = DataArray<int8_t>::NullPointer();
  DataArray<int8_t>* nodeTypePtr = nodeTypeSharedPtr.get();
  IDataArray::Pointer iNodeTypePtr = getSurfaceMeshDataContainer()->getPointData(DREAM3D::PointData::SurfaceMeshNodeType);

  if (NULL == iNodeTypePtr.get() )
  {
    // The node type array does not exist so create one with the default node type populated
    nodeTypeSharedPtr = DataArray<int8_t>::CreateArray(nodesPtr->GetNumberOfTuples(), DREAM3D::PointData::SurfaceMeshNodeType);
    nodeTypeSharedPtr->initializeWithValues(DREAM3D::SurfaceMesh::NodeType::Default);
    nodeTypePtr = nodeTypeSharedPtr.get();
  }
  else
  {
    // The node type array does exist so use that one.
    nodeTypePtr = DataArray<int8_t>::SafeObjectDownCast<IDataArray*, DataArray<int8_t>* >(iNodeTypePtr.get());
  }
  int8_t* m_SurfaceMeshNodeType = nodeTypePtr->GetPointer(0);

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  std::string parentPath = MXAFileInfo::parentPath(getOutputVtkFile());
  if(!MXADir::mkdir(parentPath, true))
  {
    ss.str("");
    ss << "Error creating parent path '" << parentPath << "'";
    notifyErrorMessage(ss.str(), -1);
    setErrorCondition(-1);
    return;
  }


  // Open the output VTK File for writing
  FILE* vtkFile = NULL;
  vtkFile = fopen(getOutputVtkFile().c_str(), "wb");
  if (NULL == vtkFile)
  {
    ss.str("");
    ss << "Error creating file '" << getOutputVtkFile() << "'";
    notifyErrorMessage(ss.str(), -18542);
    setErrorCondition(-18542);
    return;
  }
  ScopedFileMonitor vtkFileMonitor(vtkFile);

  notifyStatusMessage("Writing Vertex Data ....");


  fprintf(vtkFile, "# vtk DataFile Version 2.0\n");
  fprintf(vtkFile, "Data set from DREAM.3D Surface Meshing Module\n");
  if (m_WriteBinaryFile) {
    fprintf(vtkFile, "BINARY\n");
  }
  else {
    fprintf(vtkFile, "ASCII\n");
  }
  fprintf(vtkFile, "DATASET POLYDATA\n");

  int numberWrittenNodes = 0;
  for (int i = 0; i < nNodes; i++)
  {
    //  Node& n = nodes[i]; // Get the current Node
    if (m_SurfaceMeshNodeType[i] > 0) { ++numberWrittenNodes; }
    else { std::cout << "Node Type Invalid: " << i << "::" << (int)(m_SurfaceMeshNodeType[i]) << std::endl;}
  }

  fprintf(vtkFile, "POINTS %d float\n", numberWrittenNodes);

  float pos[3] = {0.0f, 0.0f, 0.0f};

  size_t totalWritten = 0;

  // Write the POINTS data (Vertex)
  for (int i = 0; i < nNodes; i++)
  {
    SurfaceMesh::DataStructures::Vert_t& n = nodes[i]; // Get the current Node
    if (m_SurfaceMeshNodeType[i] > 0)
    {
      pos[0] = static_cast<float>(n.pos[0]);
      pos[1] = static_cast<float>(n.pos[1]);
      pos[2] = static_cast<float>(n.pos[2]);

      if (m_WriteBinaryFile == true)
      {
        MXA::Endian::FromSystemToBig::convert<float>(pos[0]);
        MXA::Endian::FromSystemToBig::convert<float>(pos[1]);
        MXA::Endian::FromSystemToBig::convert<float>(pos[2]);
        totalWritten = fwrite(pos, sizeof(float), 3, vtkFile);
      }
      else {
        fprintf(vtkFile, "%f %f %f\n", pos[0], pos[1], pos[2]); // Write the positions to the output file
      }
    }
  }


  // Write the triangle indices into the vtk File
  notifyStatusMessage("Writing Faces ....");
  StructArray<SurfaceMesh::DataStructures::Face_t>& triangles = *(m->getFaces());

  int tData[4];

  int triangleCount = triangles.GetNumberOfTuples();

  IDataArray::Pointer flPtr = getSurfaceMeshDataContainer()->getFaceData(DREAM3D::FaceData::SurfaceMeshTriangleLabels);
  DataArray<int32_t>* faceLabelsPtr = DataArray<int32_t>::SafePointerDownCast(flPtr.get());
  int32_t* faceLabels = faceLabelsPtr->GetPointer(0);


  // Store all the unique Spins
  std::map<int32_t, int32_t> grainTriangleCount;
  for (int i = 0; i < triangleCount; i++)
  {
    if (grainTriangleCount.find(faceLabels[i*2]) == grainTriangleCount.end())
    {
      grainTriangleCount[faceLabels[i*2]] = 1;
    }
    else
    {
      grainTriangleCount[faceLabels[i*2]]++;
    }
    if (grainTriangleCount.find(faceLabels[i*2+1]) == grainTriangleCount.end())
    {
      grainTriangleCount[faceLabels[i*2+1]] = 1;
    }
    else
    {
      grainTriangleCount[faceLabels[i*2+1]]++;
    }
  }

  triangleCount = triangles.GetNumberOfTuples();
  // Write the POLYGONS
  fprintf(vtkFile, "\nPOLYGONS %d %d\n", triangleCount*2, (triangleCount*2*4));

  size_t totalCells = 0;
  // Loop over all the grains
  for(std::map<int32_t, int32_t>::iterator grainIter = grainTriangleCount.begin(); grainIter != grainTriangleCount.end(); ++grainIter)
  {
    totalCells += (*grainIter).second;
  }
  assert(totalCells == (size_t)(triangleCount * 2) );


  // Loop over all the grains
  for(std::map<int32_t, int32_t>::iterator grainIter = grainTriangleCount.begin(); grainIter != grainTriangleCount.end(); ++grainIter)
  {
    int32_t gid = (*grainIter).first; // The current Grain Id
    int32_t numTriToWrite = (*grainIter).second; // The number of triangles for this grain
    uint8_t doWrite = 0;

    // Loop over all the triangles looking for the current grain id
    // this is probably sub-optimal as if we have 1000 grains we are going to loop 1000 times but this will use the
    // least amount of memory. We could run a filter to group the triangles by grain but then we would need an
    // additional amount of memory equal to 3X the memory used for the triangle list because every triangle will be listed
    // twice. We could get some slightly better performance if we buffered 4K worth of data then wrote out that data
    // in one chunk versus what we are doing here.
    for (int j = 0; j < triangleCount; j++)
    {
      doWrite = 0;

      if (faceLabels[j*2] == gid ) { doWrite = 1; }
      else if (faceLabels[j*2+1] == gid) { doWrite = 2; } // We need to flip the winding of the triangle
      if (doWrite == 0) { continue; } // Labels in the triangle did match the current grain id.

      if (doWrite == 1)
      {
        tData[0] = 3; // Push on the total number of entries for this entry
        tData[1] = triangles[j].verts[0];
        tData[2] = triangles[j].verts[1];
        tData[3] = triangles[j].verts[2];
      }
      else
      {
        tData[0] = 3; // Push on the total number of entries for this entry
        tData[1] = triangles[j].verts[2];
        tData[2] = triangles[j].verts[1];
        tData[3] = triangles[j].verts[0];
      }
      if (m_WriteBinaryFile == true)
      {

        MXA::Endian::FromSystemToBig::convert<int>(tData[0]);
        MXA::Endian::FromSystemToBig::convert<int>(tData[1]); // Index of Vertex 0
        MXA::Endian::FromSystemToBig::convert<int>(tData[2]); // Index of Vertex 1
        MXA::Endian::FromSystemToBig::convert<int>(tData[3]); // Index of Vertex 2
        fwrite(tData, sizeof(int), 4, vtkFile);
      }
      else
      {
        fprintf(vtkFile, "3 %d %d %d\n", tData[1], tData[2], tData[3]);
      }
      numTriToWrite--;
    }
    if (numTriToWrite != 0)
    {
      std::cout << "Not enough triangles written: " << gid << "::" << numTriToWrite << " Total Triangles to Write " << (*grainIter).second << std::endl;
    }

  }


  // Write the POINT_DATA section
  err = writePointData(vtkFile);

  // Write the CELL_DATA section
  err = writeCellData(vtkFile, grainTriangleCount);


  fprintf(vtkFile, "\n");

  setErrorCondition(0);
  notifyStatusMessage("Complete");
}





// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename DataContainer, typename T>
void writePointScalarData(DataContainer* dc, const std::string &dataName, const std::string &dataType,
                          bool writeBinaryData, FILE* vtkFile, int nT)
{
  IDataArray::Pointer data = dc->getPointData(dataName);
  std::stringstream ss;
  if (NULL != data.get())
  {
    T* m = reinterpret_cast<T*>(data->GetVoidPointer(0));
    fprintf(vtkFile, "\n");
    fprintf(vtkFile, "SCALARS %s %s\n", dataName.c_str(), dataType.c_str());
    fprintf(vtkFile, "LOOKUP_TABLE default\n");
    for(int i = 0; i < nT; ++i)
    {
      T swapped = 0x00;
      if(writeBinaryData == true)
      {
        swapped = static_cast<T>(m[i]);
        MXA::Endian::FromSystemToBig::convert<T>(swapped);
        fwrite(&swapped, sizeof(T), 1, vtkFile);
      }
      else
      {
        ss.str("");
        ss << m[i] << " ";
        fprintf(vtkFile, "%s ", ss.str().c_str());
        //if (i%50 == 0)
        { fprintf(vtkFile, "\n"); }
      }

    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename DataContainer, typename T>
void writePointVectorData(DataContainer* dc, const std::string &dataName, const std::string &dataType,
                          bool writeBinaryData, const std::string &vtkAttributeType,
                          FILE* vtkFile, int nT)
{
  IDataArray::Pointer data = dc->getPointData(dataName);
  std::stringstream ss;
  if (NULL != data.get())
  {
    T* m = reinterpret_cast<T*>(data->GetVoidPointer(0));
    fprintf(vtkFile, "\n");
    fprintf(vtkFile, "%s %s %s\n", vtkAttributeType.c_str(), dataName.c_str(), dataType.c_str());
    for(int i = 0; i < nT; ++i)
    {
      T s0 = 0x00;
      T s1 = 0x00;
      T s2 = 0x00;
      if(writeBinaryData == true)
      {
        s0 = static_cast<T>(m[i*3+0]);
        s1 = static_cast<T>(m[i*3+1]);
        s2 = static_cast<T>(m[i*3+2]);
        MXA::Endian::FromSystemToBig::convert<T>(s0);
        MXA::Endian::FromSystemToBig::convert<T>(s1);
        MXA::Endian::FromSystemToBig::convert<T>(s2);
        fwrite(&s0, sizeof(T), 1, vtkFile);
        fwrite(&s1, sizeof(T), 1, vtkFile);
        fwrite(&s1, sizeof(T), 1, vtkFile);
      }
      else
      {
        ss.str("");
        ss << m[i*3+0] << " " << m[i*3+1] << " " << m[i*3+2] << " ";
        fprintf(vtkFile, "%s ", ss.str().c_str());
        //if (i%50 == 0)
        { fprintf(vtkFile, "\n"); }
      }

    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshToNonconformalVtk::writePointData(FILE* vtkFile)
{
  int err = 0;
  if (NULL == vtkFile)
  {
    return -1;
  }

  StructArray<SurfaceMesh::DataStructures::Vert_t>::Pointer nodesPtr = getSurfaceMeshDataContainer()->getVertices();

  // Make sure we have a node type array or create a default one.
  DataArray<int8_t>::Pointer nodeTypeSharedPtr = DataArray<int8_t>::NullPointer();
  DataArray<int8_t>* nodeTypePtr = nodeTypeSharedPtr.get();
  IDataArray::Pointer iNodeTypePtr = getSurfaceMeshDataContainer()->getPointData(DREAM3D::PointData::SurfaceMeshNodeType);

  if (NULL == iNodeTypePtr.get() )
  {
    // The node type array does not exist so create one with the default node type populated
    nodeTypeSharedPtr = DataArray<int8_t>::CreateArray(nodesPtr->GetNumberOfTuples(), DREAM3D::PointData::SurfaceMeshNodeType);
    nodeTypeSharedPtr->initializeWithValues(DREAM3D::SurfaceMesh::NodeType::Default);
    nodeTypePtr = nodeTypeSharedPtr.get();
  }
  else
  {
    // The node type array does exist so use that one.
    nodeTypePtr = DataArray<int8_t>::SafeObjectDownCast<IDataArray*, DataArray<int8_t>* >(iNodeTypePtr.get());
  }
  int8_t* m_SurfaceMeshNodeType = nodeTypePtr->GetPointer(0);


  //Get the Number of Vertex points in the mesh with a valid node type
  StructArray<SurfaceMesh::DataStructures::Vert_t>& nodes = *(getSurfaceMeshDataContainer()->getVertices());
  int numNodes = nodes.GetNumberOfTuples();
  int nNodes = 0;
  // int swapped;
  for (int i = 0; i < numNodes; i++)
  {
    if (m_SurfaceMeshNodeType[i] > 0) { ++nNodes; }
    else { std::cout << "Node Type Invalid: " << i << "::" << (int)(m_SurfaceMeshNodeType[i]) << std::endl;}
  }
  // This is the section header
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "POINT_DATA %d\n", nNodes);


  fprintf(vtkFile, "SCALARS Node_Type char 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");

  for(int i = 0; i < numNodes; ++i)
  {
    if(m_SurfaceMeshNodeType[i] > 0)
    {
      if(m_WriteBinaryFile == true)
      {
        // swapped = m_SurfaceMeshNodeType[i];
        // MXA::Endian::FromSystemToBig::convert<int>(swapped);
        fwrite(m_SurfaceMeshNodeType + i, sizeof(char), 1, vtkFile);
      }
      else
      {
        fprintf(vtkFile, "%d ", m_SurfaceMeshNodeType[i]);
      }
    }
  }

#if 1
  // This is from the Goldfeather Paper
  writePointVectorData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), "Principal_Direction_1",
                                                         "double", m_WriteBinaryFile, "VECTORS", vtkFile, numNodes);
  // This is from the Goldfeather Paper
  writePointVectorData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), "Principal_Direction_2",
                                                         "double", m_WriteBinaryFile, "VECTORS", vtkFile, numNodes);

  // This is from the Goldfeather Paper
  writePointScalarData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), "Principal_Curvature_1",
                                                         "double", m_WriteBinaryFile, vtkFile, numNodes);

  // This is from the Goldfeather Paper
  writePointScalarData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), "Principal_Curvature_2",
                                                         "double", m_WriteBinaryFile, vtkFile, numNodes);
#endif

  // This is from the Goldfeather Paper
  writePointVectorData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), DREAM3D::PointData::SurfaceMeshNodeNormals,
                                                         "double", m_WriteBinaryFile, "VECTORS", vtkFile, numNodes);


  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename SurfaceMeshDataContainer, typename T>
void writeCellScalarData(SurfaceMeshDataContainer* dc, const std::string &dataName, const std::string &dataType,
                         bool writeBinaryData, FILE* vtkFile, std::map<int32_t, int32_t> &grainIds)
{
  StructArray<SurfaceMesh::DataStructures::Face_t>& triangles = *(dc->getFaces());

  IDataArray::Pointer flPtr = dc->getFaceData(DREAM3D::FaceData::SurfaceMeshTriangleLabels);
  DataArray<int32_t>* faceLabelsPtr = DataArray<int32_t>::SafePointerDownCast(flPtr.get());
  int32_t* faceLabels = faceLabelsPtr->GetPointer(0);


  int triangleCount = triangles.GetNumberOfTuples();
  IDataArray::Pointer data = dc->getFaceData(dataName);
  std::stringstream ss;
  if (NULL != data.get())
  {
    int32_t totalCellsWritten = 0;

    T* m = reinterpret_cast<T*>(data->GetVoidPointer(0));
    fprintf(vtkFile, "\n");
    fprintf(vtkFile, "SCALARS %s %s 1\n", dataName.c_str(), dataType.c_str());
    fprintf(vtkFile, "LOOKUP_TABLE default\n");
    // Loop over all the grains
    for(std::map<int32_t, int32_t>::iterator grainIter = grainIds.begin(); grainIter != grainIds.end(); ++grainIter)
    {
      int32_t gid = (*grainIter).first; // The current Grain Id
      size_t size = (*grainIter).second; // The number of triangles for this grain id
      std::vector<T> buffer(size, 0);
      totalCellsWritten += size;
      size_t index = 0;

      for (int j = 0; j < triangleCount; j++)
      {
        if (faceLabels[j*2] != gid && faceLabels[j*2+1] != gid) { continue; }
        // Get the data
        T s0 = static_cast<T>(m[j]);


        // Write the values to the buffer after an Endian swap.
        if(writeBinaryData == true)
        {
          MXA::Endian::FromSystemToBig::convert<T>(s0);
          buffer[index]=s0; ++index;
        }
        else
        {
          ss.str("");
          ss << s0;
          fprintf(vtkFile, "%s\n", ss.str().c_str());
        }
      }

      // Write the Buffer
      if(writeBinaryData == true)
      {
        fwrite(&(buffer.front()), sizeof(T), size, vtkFile);
      }
    }
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename DataContainer, typename T>
void writeCellNormalData(DataContainer* dc, const std::string &dataName, const std::string &dataType,
                         bool writeBinaryData, FILE* vtkFile, std::map<int32_t, int32_t> &grainIds)
{

  StructArray<SurfaceMesh::DataStructures::Face_t>& triangles = *(dc->getFaces());
  IDataArray::Pointer flPtr = dc->getFaceData(DREAM3D::FaceData::SurfaceMeshTriangleLabels);
  DataArray<int32_t>* faceLabelsPtr = DataArray<int32_t>::SafePointerDownCast(flPtr.get());
  int32_t* faceLabels = faceLabelsPtr->GetPointer(0);


  int triangleCount = triangles.GetNumberOfTuples();
  IDataArray::Pointer data = dc->getFaceData(dataName);
  std::stringstream ss;
  if (NULL != data.get())
  {
    int32_t totalCellsWritten = 0;

    T* m = reinterpret_cast<T*>(data->GetVoidPointer(0));
    fprintf(vtkFile, "\n");
    fprintf(vtkFile, "NORMALS %s %s\n", dataName.c_str(), dataType.c_str());
    // Loop over all the grains
    for(std::map<int32_t, int32_t>::iterator grainIter = grainIds.begin(); grainIter != grainIds.end(); ++grainIter)
    {
      int32_t gid = (*grainIter).first; // The current Grain Id
      size_t size = (*grainIter).second; // The number of triangles for this grain id
      std::vector<T> buffer(size* 3, 0);
      totalCellsWritten += size*3;
      size_t index = 0;

      for (int j = 0; j < triangleCount; j++)
      {
        if (faceLabels[j*2] != gid && faceLabels[j*2+1] != gid) { continue; }
        // Get the data
        T s0 = static_cast<T>(m[j*3+0]);
        T s1 = static_cast<T>(m[j*3+1]);
        T s2 = static_cast<T>(m[j*3+2]);
        // Flip the normal if needed because the current grain id is assigned to the triangle.labels[1]
        if (faceLabels[j*2+1] == gid )
        {
          s0 *= -1.0;
          s1 *= -1.0;
          s2 *= -1.0;
        }
        // Write the values to the buffer after an Endian swap.
        if(writeBinaryData == true)
        {
          MXA::Endian::FromSystemToBig::convert<T>(s0);
          buffer[index]=s0; ++index;

          MXA::Endian::FromSystemToBig::convert<T>(s1);
          buffer[index]=s1; ++index;

          MXA::Endian::FromSystemToBig::convert<T>(s2);
          buffer[index]=s2; ++index;
        }
        else
        {
          ss.str("");
          ss << s0 << " " << s1 << " " << s2;
          fprintf(vtkFile, "%s\n", ss.str().c_str());
        }
      }

      // Write the Buffer
      if(writeBinaryData == true)
      {
        fwrite(&(buffer.front()), sizeof(T), size*3, vtkFile);
      }
    }
  }
}




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename DataContainer, typename T>
void writeCellVectorData(DataContainer* dc, const std::string &dataName, const std::string &dataType,
                         bool writeBinaryData, const std::string &vtkAttributeType,
                         FILE* vtkFile, std::map<int32_t, int32_t> &grainIds)
{
  StructArray<SurfaceMesh::DataStructures::Face_t>& triangles = *(dc->getFaces());

  int triangleCount = triangles.GetNumberOfTuples();

  IDataArray::Pointer data = dc->getFaceData(dataName);
  std::stringstream ss;
  if (NULL != data.get())
  {
    T* m = reinterpret_cast<T*>(data->GetVoidPointer(0));
    fprintf(vtkFile, "\n");
    fprintf(vtkFile, "%s %s %s\n", vtkAttributeType.c_str(), dataName.c_str(), dataType.c_str());
    for(int i = 0; i < triangleCount; ++i)
    {
      T s0 = 0x00;
      T s1 = 0x00;
      T s2 = 0x00;
      if(writeBinaryData == true)
      {
        s0 = static_cast<T>(m[i*3+0]);
        s1 = static_cast<T>(m[i*3+1]);
        s2 = static_cast<T>(m[i*3+2]);
        MXA::Endian::FromSystemToBig::convert<T>(s0);
        MXA::Endian::FromSystemToBig::convert<T>(s1);
        MXA::Endian::FromSystemToBig::convert<T>(s2);
        fwrite(&s0, sizeof(T), 1, vtkFile);
        fwrite(&s1, sizeof(T), 1, vtkFile);
        fwrite(&s2, sizeof(T), 1, vtkFile);

      }
      else
      {
        ss.str("");
        ss << m[i*3+0] << " " << m[i*3+1] << " " << m[i*3+2] << " ";

        fprintf(vtkFile, "%s ", ss.str().c_str());
        if (i%25 == 0) { fprintf(vtkFile, "\n"); }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshToNonconformalVtk::writeCellData(FILE* vtkFile, std::map<int32_t, int32_t> &grainIds)
{
  int err = 0;
  if (NULL == vtkFile)
  {
    return -1;
  }
  // Write the triangle region ids
  StructArray<SurfaceMesh::DataStructures::Face_t>& triangles = *(getSurfaceMeshDataContainer()->getFaces());
  IDataArray::Pointer flPtr = getSurfaceMeshDataContainer()->getFaceData(DREAM3D::FaceData::SurfaceMeshTriangleLabels);
  DataArray<int32_t>* faceLabelsPtr = DataArray<int32_t>::SafePointerDownCast(flPtr.get());
  int32_t* faceLabels = faceLabelsPtr->GetPointer(0);

  int triangleCount = triangles.GetNumberOfTuples();
  int swapped;

  // This is like a "section header"
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "CELL_DATA %d\n", triangleCount * 2);

  int32_t totalCellsWritten = 0;

  // Write the GrainId Data to the file
  fprintf(vtkFile, "SCALARS GrainID int 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");

  // Loop over all the grains
  for(std::map<int32_t, int32_t>::iterator grainIter = grainIds.begin(); grainIter != grainIds.end(); ++grainIter)
  {
    int32_t gid = (*grainIter).first; // The current Grain Id
    size_t size = (*grainIter).second; // The number of triangles for this grain id
    std::vector<int32_t> buffer(size, 0);
    totalCellsWritten += size;

    // Endian Swap our current grain Id since we are going to write it a bunch of times.
    swapped = gid;
    MXA::Endian::FromSystemToBig::convert<int>(swapped);
    size_t index = 0;

    // Loop over all the triangles looking for the current grain id
    // this is probably sub-optimal as if we have 1000 grains we are going to loop 1000 times but this will use the
    // least amount of memory. We could run a filter to group the triangles by grain but then we would need an
    // additional amount of memory equal to 3X the memory used for the triangle list because every triangle will be listed
    // twice. We could get some slightly better performance if we buffered 4K worth of data then wrote out that data
    // in one chunk versus what we are doing here.
    for (int j = 0; j < triangleCount; j++)
    {
      if (faceLabels[j*2] == gid || faceLabels[j*2+1] == gid)
      {
        if(m_WriteBinaryFile == true)
        {
          buffer[index]=swapped;
          ++index;
        }
        else
        {
          fprintf(vtkFile, "%d\n", gid);
        }
      }
    }
    // Write the Buffer
    if(m_WriteBinaryFile == true)
    {
      fwrite(&(buffer.front()), sizeof(int32_t), size, vtkFile);
    }
  }

#if 0
  // Write the Original Triangle ID Data to the file
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "SCALARS TriangleID int 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");
  for(int i = 0; i < triangleCount * 2; ++i)
  {
    //Triangle& t = triangles[i]; // Get the current Node

    if(m_WriteBinaryFile == true)
    {
      swapped = i;
      MXA::Endian::FromSystemToBig::convert<int>(swapped);
      fwrite(&swapped, sizeof(int), 1, vtkFile);
      fwrite(&swapped, sizeof(int), 1, vtkFile);
    }
    else
    {
      fprintf(vtkFile, "%d\n", i);
      fprintf(vtkFile, "%d\n", i);
    }
  }
#endif
  notifyStatusMessage("Writing Face Normals...");
  writeCellNormalData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), DREAM3D::FaceData::SurfaceMeshTriangleNormals,
                                                        "double", m_WriteBinaryFile, vtkFile, grainIds);

  notifyStatusMessage("Writing Principal Curvature 1");
  writeCellScalarData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), DREAM3D::FaceData::SurfaceMeshPrincipalCurvature1,
                                                        "double", m_WriteBinaryFile, vtkFile, grainIds);
  notifyStatusMessage("Writing Principal Curvature 2");
  writeCellScalarData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), DREAM3D::FaceData::SurfaceMeshPrincipalCurvature2,
                                                        "double", m_WriteBinaryFile, vtkFile, grainIds);

  notifyStatusMessage("Writing Grain Face Id");
  writeCellScalarData<SurfaceMeshDataContainer, int32_t>(getSurfaceMeshDataContainer(), DREAM3D::FaceData::SurfaceMeshGrainFaceId,
                                                         "int", m_WriteBinaryFile, vtkFile, grainIds);

  notifyStatusMessage("Writing Gaussian Curvature");
  writeCellScalarData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), DREAM3D::FaceData::SurfaceMeshGaussianCurvatures,
                                                        "double", m_WriteBinaryFile, vtkFile, grainIds);

  notifyStatusMessage("Writing Mean Curvature");
  writeCellScalarData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), DREAM3D::FaceData::SurfaceMeshMeanCurvatures,
                                                        "double", m_WriteBinaryFile, vtkFile, grainIds);
#if 0
  writeCellVectorData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), DREAM3D::CellData::SurfaceMeshPrincipalDirection1,
                                                        "double", m_WriteBinaryFile, "VECTORS", vtkFile, nT);

  writeCellVectorData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), DREAM3D::CellData::SurfaceMeshPrincipalDirection2,
                                                        "double", m_WriteBinaryFile, "VECTORS", vtkFile, nT);

  writeCellNormalData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), "Goldfeather_Triangle_Normals",
                                                        "double", m_WriteBinaryFile, vtkFile, nT);
#endif

  return err;
}
