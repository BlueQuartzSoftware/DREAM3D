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

#include "SurfaceMeshToVtk.h"


#include "MXA/Common/MXAEndian.h"
#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/MXADir.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshToVtk::SurfaceMeshToVtk() :
AbstractFilter(),
//m_SurfaceMeshNodeTypeArrayName(DREAM3D::VertexData::SurfaceMeshNodeType),
m_WriteBinaryFile(false),
m_WriteConformalMesh(true),
m_SurfaceMeshNodeType(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceMeshToVtk::~SurfaceMeshToVtk()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToVtk::setupFilterParameters()
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
//   {
//     FilterParameter::Pointer option = FilterParameter::New();
//     option->setHumanLabel("Write Conformal Mesh");
//     option->setPropertyName("WriteConformalMesh");
//     option->setWidgetType(FilterParameter::BooleanWidget);
//     option->setValueType("bool");
//     options.push_back(option);
//   }

  setFilterParameters(options);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToVtk::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("OutputVtkFile", getOutputVtkFile() );
  writer->writeValue("WriteBinaryFile", getWriteBinaryFile() );
  writer->writeValue("WriteConformalMesh", getWriteConformalMesh() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToVtk::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
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
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceMeshToVtk::preflight()
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
void SurfaceMeshToVtk::execute()
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
  /* Place all your code to execute your filter here. */
  DREAM3D::SurfaceMesh::VertListPointer_t nodesPtr = m->getVertices();
  DREAM3D::SurfaceMesh::VertList_t& nodes = *(nodesPtr);
  int nNodes = nodes.GetNumberOfTuples();

  // Make sure we have a node type array or create a default one.
  DataArray<int8_t>::Pointer nodeTypeSharedPtr = DataArray<int8_t>::NullPointer();
  DataArray<int8_t>* nodeTypePtr = nodeTypeSharedPtr.get();
  IDataArray::Pointer iNodeTypePtr = getSurfaceMeshDataContainer()->getVertexData(DREAM3D::VertexData::SurfaceMeshNodeType);

  if (NULL == iNodeTypePtr.get() )
  {
    // The node type array does not exist so create one with the default node type populated
    nodeTypeSharedPtr = DataArray<int8_t>::CreateArray(nodesPtr->GetNumberOfTuples(), DREAM3D::VertexData::SurfaceMeshNodeType);
    nodeTypeSharedPtr->initializeWithValues(DREAM3D::SurfaceMesh::NodeType::Default);
    nodeTypePtr = nodeTypeSharedPtr.get();
  }
  else
  {
    // The node type array does exist so use that one.
    nodeTypePtr = DataArray<int8_t>::SafeObjectDownCast<IDataArray*, DataArray<int8_t>* >(iNodeTypePtr.get());
  }
  m_SurfaceMeshNodeType = nodeTypePtr->GetPointer(0);

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
  }


  fprintf(vtkFile, "POINTS %d float\n", numberWrittenNodes);

  float pos[3] = {0.0f, 0.0f, 0.0f};

  size_t totalWritten = 0;

  // Write the POINTS data (Vertex)
  for (int i = 0; i < nNodes; i++)
  {
    DREAM3D::SurfaceMesh::Vert_t& n = nodes[i]; // Get the current Node
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
        if (totalWritten != sizeof(float) * 3)
        {

        }
      }
      else {
        fprintf(vtkFile, "%f %f %f\n", pos[0], pos[1], pos[2]); // Write the positions to the output file
      }
    }
  }

  // Write the triangle indices into the vtk File
  StructArray<DREAM3D::SurfaceMesh::Face_t>& triangles = *(m->getFaces());

  int tData[4];
  int nT = triangles.GetNumberOfTuples();
  int triangleCount = nT;
//  int tn1, tn2, tn3;
  if (false == m_WriteConformalMesh)
  {
    triangleCount = nT * 2;
  }
  // Write the POLYGONS
  fprintf(vtkFile, "\nPOLYGONS %d %d\n", triangleCount, (triangleCount * 4));
  for (int j = 0; j < nT; j++)
  {
  //  Triangle& t = triangles[j];
    tData[1] = triangles[j].verts[0];
    tData[2] = triangles[j].verts[1];
    tData[3] = triangles[j].verts[2];

    if (m_WriteBinaryFile == true)
    {
      tData[0] = 3; // Push on the total number of entries for this entry
      MXA::Endian::FromSystemToBig::convert<int>(tData[0]);
      MXA::Endian::FromSystemToBig::convert<int>(tData[1]); // Index of Vertex 0
      MXA::Endian::FromSystemToBig::convert<int>(tData[2]); // Index of Vertex 1
      MXA::Endian::FromSystemToBig::convert<int>(tData[3]); // Index of Vertex 2
      fwrite(tData, sizeof(int), 4, vtkFile);
      if (false == m_WriteConformalMesh)
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
      if (false == m_WriteConformalMesh)
      {
        fprintf(vtkFile, "3 %d %d %d\n", tData[3], tData[2], tData[1]);
      }
    }
  }


  // Write the POINT_DATA section
  err = writePointData(vtkFile);
  // Write the CELL_DATA section
  err = writeCellData(vtkFile);


  fprintf(vtkFile, "\n");

  setErrorCondition(0);
  notifyStatusMessage("Complete");
}





// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename DataContainer, typename T>
void writePointScalarData(DataContainer* dc, const std::string &dataName, const std::string &dataType,
                                bool writeBinaryData, bool writeConformalMesh, FILE* vtkFile, int nT)
{
  IDataArray::Pointer data = dc->getVertexData(dataName);
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
                                bool writeBinaryData, bool writeConformalMesh, const std::string &vtkAttributeType,
                                FILE* vtkFile, int nT)
{
  IDataArray::Pointer data = dc->getVertexData(dataName);
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
int SurfaceMeshToVtk::writePointData(FILE* vtkFile)
{
  int err = 0;
  if (NULL == vtkFile)
  {
    return -1;
  }


  // Write the Node Type Data to the file
  DREAM3D::SurfaceMesh::VertList_t& nodes = *(getSurfaceMeshDataContainer()->getVertices());
  int numNodes = nodes.GetNumberOfTuples();
  int nNodes = 0;
 // int swapped;
  for (int i = 0; i < numNodes; i++)
  {
    if (m_SurfaceMeshNodeType[i] > 0) { ++nNodes; }
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
                                                          "double", m_WriteBinaryFile, m_WriteConformalMesh, "VECTORS", vtkFile, numNodes);
  // This is from the Goldfeather Paper
  writePointVectorData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), "Principal_Direction_2",
                                                          "double", m_WriteBinaryFile, m_WriteConformalMesh, "VECTORS", vtkFile, numNodes);

                                                          // This is from the Goldfeather Paper
  writePointScalarData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), "Principal_Curvature_1",
                                                          "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, numNodes);

// This is from the Goldfeather Paper
  writePointScalarData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), "Principal_Curvature_2",
                                                         "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, numNodes);
#endif

// This is from the Goldfeather Paper
  writePointVectorData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), DREAM3D::VertexData::SurfaceMeshNodeNormals,
                                                          "double", m_WriteBinaryFile, m_WriteConformalMesh, "VECTORS", vtkFile, numNodes);


  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename DataContainer, typename T>
void writeCellScalarData(DataContainer* dc, const std::string &dataName, const std::string &dataType,
                                bool writeBinaryData, bool writeConformalMesh, FILE* vtkFile, int nT)
{
  // Write the Grain Face ID Data to the file
  IDataArray::Pointer data = dc->getFaceData(dataName);
  std::stringstream ss;
  if (NULL != data.get())
  {
    T* m = reinterpret_cast<T*>(data->GetVoidPointer(0));
    fprintf(vtkFile, "\n");
    fprintf(vtkFile, "SCALARS %s %s 1\n", dataName.c_str(), dataType.c_str());
    fprintf(vtkFile, "LOOKUP_TABLE default\n");
    for(int i = 0; i < nT; ++i)
    {
      T swapped = 0x00;
      if(writeBinaryData == true)
      {
        swapped = static_cast<T>(m[i]);
        MXA::Endian::FromSystemToBig::convert<T>(swapped);
        fwrite(&swapped, sizeof(T), 1, vtkFile);
        if(false == writeConformalMesh)
        {
          fwrite(&swapped, sizeof(T), 1, vtkFile);
        }
      }
      else
      {
        ss.str("");
        ss << m[i] << " ";
        if(false == writeConformalMesh)
        {
          ss << m[i] << " ";
        }
        fprintf(vtkFile, "%s ", ss.str().c_str());
        if (i%50 == 0) { fprintf(vtkFile, "\n"); }
      }
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename DataContainer, typename T>
void writeCellVectorData(DataContainer* dc, const std::string &dataName, const std::string &dataType,
                                bool writeBinaryData, bool writeConformalMesh, const std::string &vtkAttributeType,
                                FILE* vtkFile, int nT)
{
  IDataArray::Pointer data = dc->getFaceData(dataName);
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
        fwrite(&s2, sizeof(T), 1, vtkFile);
        if(false == writeConformalMesh)
        {
          fwrite(&s0, sizeof(T), 1, vtkFile);
          fwrite(&s1, sizeof(T), 1, vtkFile);
          fwrite(&s2, sizeof(T), 1, vtkFile);
        }
      }
      else
      {
        ss.str("");
        ss << m[i*3+0] << " " << m[i*3+1] << " " << m[i*3+2] << " ";
        if(false == writeConformalMesh)
        {
          ss << m[i*3+0] << " " << m[i*3+1] << " " << m[i*3+2] << " ";
        }
        fprintf(vtkFile, "%s ", ss.str().c_str());
        if (i%25 == 0) { fprintf(vtkFile, "\n"); }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename DataContainer, typename T>
void writeCellNormalData(DataContainer* dc, const std::string &dataName, const std::string &dataType,
                                bool writeBinaryData, bool writeConformalMesh,
                                FILE* vtkFile, int nT)
{
  IDataArray::Pointer data = dc->getFaceData(dataName);
  std::stringstream ss;
  if (NULL != data.get())
  {
    T* m = reinterpret_cast<T*>(data->GetVoidPointer(0));
    fprintf(vtkFile, "\n");
    fprintf(vtkFile, "NORMALS %s %s\n", dataName.c_str(), dataType.c_str());
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
        fwrite(&s2, sizeof(T), 1, vtkFile);
        if(false == writeConformalMesh)
        {
          s0 = static_cast<T>(m[i*3+0]) * -1.0;
          s1 = static_cast<T>(m[i*3+1]) * -1.0;
          s2 = static_cast<T>(m[i*3+2]) * -1.0;
          MXA::Endian::FromSystemToBig::convert<T>(s0);
          MXA::Endian::FromSystemToBig::convert<T>(s1);
          MXA::Endian::FromSystemToBig::convert<T>(s2);
          fwrite(&s0, sizeof(T), 1, vtkFile);
          fwrite(&s1, sizeof(T), 1, vtkFile);
          fwrite(&s2, sizeof(T), 1, vtkFile);
        }
      }
      else
      {
        ss.str("");
        ss << m[i*3+0] << " " << m[i*3+1] << " " << m[i*3+2] << " ";
        if(false == writeConformalMesh)
        {
          ss << -1.0*m[i*3+0] << " " << -1.0*m[i*3+1] << " " << -1.0*m[i*3+2] << " ";
        }
        fprintf(vtkFile, "%s ", ss.str().c_str());
        if (i%50 == 0) { fprintf(vtkFile, "\n"); }
      }
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SurfaceMeshToVtk::writeCellData(FILE* vtkFile)
{
  int err = 0;
  if (NULL == vtkFile)
  {
    return -1;
  }


  // Write the triangle region ids
  StructArray<DREAM3D::SurfaceMesh::Face_t>& triangles = *(getSurfaceMeshDataContainer()->getFaces());
  IDataArray::Pointer flPtr = getSurfaceMeshDataContainer()->getFaceData(DREAM3D::FaceData::SurfaceMeshFaceLabels);
  DataArray<int32_t>* faceLabelsPtr = DataArray<int32_t>::SafePointerDownCast(flPtr.get());
  int32_t* faceLabels = faceLabelsPtr->GetPointer(0);


  int nT = triangles.GetNumberOfTuples();
  int triangleCount = nT;
  int swapped;
  if (false == m_WriteConformalMesh)
  {
    triangleCount = nT * 2;
  }


    // This is like a "section header"
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "CELL_DATA %d\n", triangleCount);

  // Write the GrainId Data to the file
  fprintf(vtkFile, "SCALARS GrainID int 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");
  for(int i = 0; i < nT; ++i)
  {
    //DREAM3D::SurfaceMesh::Face_t& t = triangles[i]; // Get the current Node

    if(m_WriteBinaryFile == true)
    {
      swapped = faceLabels[i*2];
      MXA::Endian::FromSystemToBig::convert<int>(swapped);
      fwrite(&swapped, sizeof(int), 1, vtkFile);
      if(false == m_WriteConformalMesh)
      {
        swapped = faceLabels[i*2+1];
        MXA::Endian::FromSystemToBig::convert<int>(swapped);
        fwrite(&swapped, sizeof(int), 1, vtkFile);
      }
    }
    else
    {
      fprintf(vtkFile, "%d\n", faceLabels[i*2]);
      if(false == m_WriteConformalMesh)
      {
        fprintf(vtkFile, "%d\n", faceLabels[i*2+1]);
      }
    }

  }

#if 0
  // Write the Original Triangle ID Data to the file
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "SCALARS TriangleID int 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");
  for(int i = 0; i < nT; ++i)
  {
    //Triangle& t = triangles[i]; // Get the current Node

    if(m_WriteBinaryFile == true)
    {
      swapped = i;
      MXA::Endian::FromSystemToBig::convert<int>(swapped);
      fwrite(&swapped, sizeof(int), 1, vtkFile);
      if(false == m_WriteConformalMesh)
      {
        fwrite(&swapped, sizeof(int), 1, vtkFile);
      }
    }
    else
    {
      fprintf(vtkFile, "%d\n", i);
      if(false == m_WriteConformalMesh)
      {
        fprintf(vtkFile, "%d\n", i);
      }
    }
  }
#endif

  writeCellScalarData<SurfaceMeshDataContainer, int32_t>(getSurfaceMeshDataContainer(), DREAM3D::FaceData::SurfaceMeshGrainFaceId,
                                                                             "int", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, nT);

  writeCellScalarData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), DREAM3D::FaceData::SurfaceMeshPrincipalCurvature1,
                                                                             "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, nT);

  writeCellScalarData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), DREAM3D::FaceData::SurfaceMeshPrincipalCurvature2,
                                                                             "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, nT);

  writeCellVectorData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), DREAM3D::FaceData::SurfaceMeshPrincipalDirection1,
                                                                             "double", m_WriteBinaryFile, m_WriteConformalMesh, "VECTORS", vtkFile, nT);

  writeCellVectorData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), DREAM3D::FaceData::SurfaceMeshPrincipalDirection2,
                                                                             "double", m_WriteBinaryFile, m_WriteConformalMesh, "VECTORS", vtkFile, nT);

  writeCellScalarData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), DREAM3D::FaceData::SurfaceMeshGaussianCurvatures,
                                                                             "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, nT);

  writeCellScalarData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), DREAM3D::FaceData::SurfaceMeshMeanCurvatures,
                                                                             "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, nT);

  writeCellNormalData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), DREAM3D::FaceData::SurfaceMeshFaceNormals,
                                                                             "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, nT);

  writeCellNormalData<SurfaceMeshDataContainer, double>(getSurfaceMeshDataContainer(), "Goldfeather_Triangle_Normals",
                                                                             "double", m_WriteBinaryFile, m_WriteConformalMesh, vtkFile, nT);

  return err;
}
