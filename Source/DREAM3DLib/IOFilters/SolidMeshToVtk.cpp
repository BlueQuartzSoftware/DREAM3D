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

#include "SolidMeshToVtk.h"



#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include "DREAM3DLib/Common/DREAM3DEndian.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SolidMeshToVtk::SolidMeshToVtk() :
AbstractFilter(),
m_WriteBinaryFile(false)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SolidMeshToVtk::~SolidMeshToVtk()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SolidMeshToVtk::setupFilterParameters()
{
  QVector<FilterParameter::Pointer> options;
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
void SolidMeshToVtk::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setOutputVtkFile( reader->readValue( "OutputVtkFile", getOutputVtkFile() ) );
  setWriteBinaryFile( reader->readValue("WriteBinaryFile", getWriteBinaryFile()) );
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SolidMeshToVtk::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("OutputVtkFile", getOutputVtkFile() );
  writer->writeValue("WriteBinaryFile", getWriteBinaryFile() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SolidMeshToVtk::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  QString ss;

  if (m_OutputVtkFile.isEmpty() == true)
  {
    setErrorCondition(-1003);
    addErrorMessage(getHumanLabel(), "Vtk Output file is Not set correctly", -1003);
  }

  VolumeDataContainer* sm = getVolumeDataContainer();
  if (NULL == sm)
  {
      addErrorMessage(getHumanLabel(), "VolumeDataContainer is missing", -383);
      setErrorCondition(-384);
  }
  else
  {
    //if (sm->getTetrahedrons().get() == NULL)
    //{
    //    addErrorMessage(getHumanLabel(), "SolidMesh DataContainer missing Triangles", -383);
    //    setErrorCondition(-384);
    //}

    //if (sm->getVertices().get() == NULL)
    //{
    //    addErrorMessage(getHumanLabel(), "SolidMesh DataContainer missing Nodes", -384);
    //    setErrorCondition(-384);
    //}

  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SolidMeshToVtk::preflight()
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
void SolidMeshToVtk::execute()
{
<<<<<<< HEAD
  int err = 0;
  QString ss;
  setErrorCondition(err);
  dataCheck(false, 0, 0, 0);
  if(getErrorCondition() < 0)
  {
    return;
  }


  setErrorCondition(0);
  SolidMeshDataContainer* m = getSolidMeshDataContainer();
  /* Place all your code to execute your filter here. */
  DREAM3D::SurfaceMesh::VertListPointer_t nodesPtr = m->getVertices();
  DREAM3D::SurfaceMesh::VertList_t& nodes = *(nodesPtr);
  int nNodes = nodes.GetNumberOfTuples();

  // Make sure any directory path is also available as the user may have just typed
  // in a path without actually creating the full path
  QString parentPath = QFileInfo::parentPath(getOutputVtkFile());
    QDir dir;
  if(!dir.mkpath(parentPath))
  {
      ss.str("");
      QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
      notifyErrorMessage(ss, -1);
      setErrorCondition(-1);
      return;
  }


// Open the output VTK File for writing
  FILE* vtkFile = NULL;
  vtkFile = fopen(getOutputVtkFile().toLatin1().data(), "wb");
  if (NULL == vtkFile)
  {
      ss.str("");
      QString ss = QObject::tr("Error creating file '%1'").arg(getOutputVtkFile());
      notifyErrorMessage(ss, -18542);
      setErrorCondition(-18542);
      return;
  }
  ScopedFileMonitor vtkFileMonitor(vtkFile);

  fprintf(vtkFile, "# vtk DataFile Version 2.0\n");
  fprintf(vtkFile, "Data set from DREAM.3D Solid Meshing Module\n");
  if (m_WriteBinaryFile) {
    fprintf(vtkFile, "BINARY\n");
  }
  else {
    fprintf(vtkFile, "ASCII\n");
  }
  fprintf(vtkFile, "DATASET UNSTRUCTURED_GRID\n");

  fprintf(vtkFile, "POINTS %d float\n", nNodes);

  float pos[3] = {0.0f, 0.0f, 0.0f};

  size_t totalWritten = 0;
  // Write the POINTS data (Vertex)
  for (int i = 0; i < nNodes; i++)
  {
    DREAM3D::SurfaceMesh::Vert_t& n = nodes[i]; // Get the current Node
    pos[0] = static_cast<float>(n.pos[0]);
    pos[1] = static_cast<float>(n.pos[1]);
    pos[2] = static_cast<float>(n.pos[2]);
    if (m_WriteBinaryFile == true)
    {
        DREAM3D::Endian::FromSystemToBig::convert(tData[0]);
        DREAM3D::Endian::FromSystemToBig::convert(tData[1]);
        DREAM3D::Endian::FromSystemToBig::convert(tData[2]);
        totalWritten = fwrite(pos, sizeof(float), 3, vtkFile);
        if (totalWritten != sizeof(float) * 3)
        {

        }
    }
    else
  {
        fprintf(vtkFile, "%f %f %f\n", pos[0], pos[1], pos[2]); // Write the positions to the output file
    }
  }

  // Write the triangle indices into the vtk File
  StructArray<Tetrahedron>& tetrahedrons = *(m->getTetrahedrons());

  int tData[5];
  int nT = tetrahedrons.GetNumberOfTuples();
  int tetrahedronCount = nT;
//  int tn1, tn2, tn3;
  // Write the CELLS Data
  fprintf(vtkFile, "CELLS %d %d\n", tetrahedronCount, (tetrahedronCount * 5));
  for (int j = 0; j < nT; j++)
  {
  //  Triangle& t = triangles[j];
    tData[1] = tetrahedrons[j].node_id[0];
    tData[2] = tetrahedrons[j].node_id[1];
    tData[3] = tetrahedrons[j].node_id[2];
    tData[4] = tetrahedrons[j].node_id[3];
    if (m_WriteBinaryFile == true)
    {
      tData[0] = 4; // Push on the total number of entries for this entry
      DREAM3D::Endian::FromSystemToBig::convert(tData[0]);
      DREAM3D::Endian::FromSystemToBig::convert(tData[1]); // Index of Vertex 0
      DREAM3D::Endian::FromSystemToBig::convert(tData[2]); // Index of Vertex 1
      DREAM3D::Endian::FromSystemToBig::convert(tData[3]); // Index of Vertex 2
      MXA::Endian::FromSystemToBig::convert<int>(tData[4]); // Index of Vertex 3
      fwrite(tData, sizeof(int), 5, vtkFile);
    }
    else
    {
      fprintf(vtkFile, "4 %d %d %d %d\n", tData[1], tData[2], tData[3], tData[4]);
    }
  }

  // Write the CELL_TYPE Data
  int tData2[1];
  fprintf(vtkFile, "CELL_TYPES %d\n", tetrahedronCount);
  for (int j = 0; j < nT; j++)
  {
  //  Triangle& t = triangles[j];
    tData2[0] = 10;
    if (m_WriteBinaryFile == true)
    {
      tData2[0] = 10; // Push on the total number of entries for this entry
      MXA::Endian::FromSystemToBig::convert<int>(tData2[0]);
      fwrite(tData2, sizeof(int), 1, vtkFile);
    }
    else
    {
      fprintf(vtkFile, "10\n");
    }
  }

  // Write the CELL_DATA section
  err = writeCellData(vtkFile);
//  err = writePointData(vtkFile);

  fprintf(vtkFile, "\n");

  setErrorCondition(0);
  notifyStatusMessage("Complete");
=======
//  int err = 0;
//  QTextStream ss;
//  setErrorCondition(err);
//  dataCheck(false, 0, 0, 0);
//  if(getErrorCondition() < 0)
//  {
//    return;
//  }
//
//
//  setErrorCondition(0);
//  VertexDataContainer* m = getVertexDataContainer();
//  /* Place all your code to execute your filter here. */
//  DREAM3D::SurfaceMesh::VertListPointer_t nodesPtr = m->getVertices();
//  DREAM3D::SurfaceMesh::VertList_t& nodes = *(nodesPtr);
//  int nNodes = nodes.GetNumberOfTuples();
//
//  // Make sure any directory path is also available as the user may have just typed
//  // in a path without actually creating the full path
//  QString parentPath = MXAFileInfo::parentPath(getOutputVtkFile());
//  if(!MXADir::mkdir(parentPath, true))
//  {
//      ss.str("");
//      QString ss = QObject::tr("Error creating parent path '%1'").arg(parentPath);
//      notifyErrorMessage(ss.str(), -1);
//      setErrorCondition(-1);
//      return;
//  }
//
//
//// Open the output VTK File for writing
//  FILE* vtkFile = NULL;
//  vtkFile = fopen(getOutputVtkFile().toLatin1().data(), "wb");
//  if (NULL == vtkFile)
//  {
//      ss.str("");
//      QString ss = QObject::tr("Error creating file '%1'").arg(getOutputVtkFile());
//      notifyErrorMessage(ss.str(), -18542);
//      setErrorCondition(-18542);
//      return;
//  }
//  ScopedFileMonitor vtkFileMonitor(vtkFile);
//
//  fprintf(vtkFile, "# vtk DataFile Version 2.0\n");
//  fprintf(vtkFile, "Data set from DREAM.3D Solid Meshing Module\n");
//  if (m_WriteBinaryFile) {
//    fprintf(vtkFile, "BINARY\n");
//  }
//  else {
//    fprintf(vtkFile, "ASCII\n");
//  }
//  fprintf(vtkFile, "DATASET UNSTRUCTURED_GRID\n");
//
//  fprintf(vtkFile, "POINTS %d float\n", nNodes);
//
//  float pos[3] = {0.0f, 0.0f, 0.0f};
//
//  size_t totalWritten = 0;
//  // Write the POINTS data (Vertex)
//  for (int i = 0; i < nNodes; i++)
//  {
//    DREAM3D::SurfaceMesh::Vert_t& n = nodes[i]; // Get the current Node
//    pos[0] = static_cast<float>(n.pos[0]);
//    pos[1] = static_cast<float>(n.pos[1]);
//    pos[2] = static_cast<float>(n.pos[2]);
//    if (m_WriteBinaryFile == true)
//    {
//        DREAM3D::Endian::FromSystemToBig::convert(tData[0]);
//        DREAM3D::Endian::FromSystemToBig::convert(tData[1]);
//        DREAM3D::Endian::FromSystemToBig::convert(tData[2]);
//        totalWritten = fwrite(pos, sizeof(float), 3, vtkFile);
//        if (totalWritten != sizeof(float) * 3)
//        {
//
//        }
//    }
//    else
//  {
//        fprintf(vtkFile, "%f %f %f\n", pos[0], pos[1], pos[2]); // Write the positions to the output file
//    }
//  }
//
//  // Write the triangle indices into the vtk File
//  StructArray<Tetrahedron>& tetrahedrons = *(m->getTetrahedrons());
//
//  int tData[5];
//  int nT = tetrahedrons.GetNumberOfTuples();
//  int tetrahedronCount = nT;
////  int tn1, tn2, tn3;
//  // Write the CELLS Data
//  fprintf(vtkFile, "CELLS %d %d\n", tetrahedronCount, (tetrahedronCount * 5));
//  for (int j = 0; j < nT; j++)
//  {
//  //  Triangle& t = triangles[j];
//    tData[1] = tetrahedrons[j].node_id[0];
//    tData[2] = tetrahedrons[j].node_id[1];
//    tData[3] = tetrahedrons[j].node_id[2];
//    tData[4] = tetrahedrons[j].node_id[3];
//    if (m_WriteBinaryFile == true)
//    {
//      tData[0] = 4; // Push on the total number of entries for this entry
//      DREAM3D::Endian::FromSystemToBig::convert(tData[0]);
//      DREAM3D::Endian::FromSystemToBig::convert(tData[1]); // Index of Vertex 0
//      DREAM3D::Endian::FromSystemToBig::convert(tData[2]); // Index of Vertex 1
//      DREAM3D::Endian::FromSystemToBig::convert(tData[3]); // Index of Vertex 2
//      MXA::Endian::FromSystemToBig::convert<int>(tData[4]); // Index of Vertex 3
//      fwrite(tData, sizeof(int), 5, vtkFile);
//    }
//    else
//    {
//      fprintf(vtkFile, "4 %d %d %d %d\n", tData[1], tData[2], tData[3], tData[4]);
//    }
//  }
//
//  // Write the CELL_TYPE Data
//  int tData2[1];
//  fprintf(vtkFile, "CELL_TYPES %d\n", tetrahedronCount);
//  for (int j = 0; j < nT; j++)
//  {
//  //  Triangle& t = triangles[j];
//    tData2[0] = 10;
//    if (m_WriteBinaryFile == true)
//    {
//      tData2[0] = 10; // Push on the total number of entries for this entry
//      MXA::Endian::FromSystemToBig::convert<int>(tData2[0]);
//      fwrite(tData2, sizeof(int), 1, vtkFile);
//    }
//    else
//    {
//      fprintf(vtkFile, "10\n");
//    }
//  }
//
//  // Write the CELL_DATA section
//  err = writeCellData(vtkFile);
////  err = writePointData(vtkFile);
//
//  fprintf(vtkFile, "\n");
//
//  setErrorCondition(0);
//  notifyStatusMessage("Complete");
>>>>>>> develop
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SolidMeshToVtk::writePointData(FILE* vtkFile)
{
  int err = 0;
  if (NULL == vtkFile)
  {
    return -1;
  }

  // Write the triangle indices into the vtk File
//  StructArray<DREAM3D::SurfaceMesh::Face_t>& triangles = *(getVertexDataContainer()->getTriangles());

  DREAM3D::SurfaceMesh::VertList_t& nodes = *(getVertexDataContainer()->getVertices());
  int numNodes = nodes.GetNumberOfTuples();
  int nNodes = 0;

  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "POINT_DATA %d\n", nNodes);
  fprintf(vtkFile, "SCALARS Node_Type int 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");


  for(int i = 0; i < numNodes; ++i)
  {
      if(m_WriteBinaryFile == true)
      {

      }
      else
      {

      }
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SolidMeshToVtk::writeCellData(FILE* vtkFile)
{
  int err = 0;
  if (NULL == vtkFile)
  {
    return -1;
  }

  // Write the triangle region ids
  StructArray<Tetrahedron>& tetrahedrons = *(getVertexDataContainer()->getTetrahedrons());

  int nT = tetrahedrons.GetNumberOfTuples();
  int tetrahedronCount = nT;
  int swapped;

  // Write the GrainId Data to the file
  fprintf(vtkFile, "\n");
  fprintf(vtkFile, "CELL_DATA %d\n", tetrahedronCount);
  fprintf(vtkFile, "SCALARS GrainID int 1\n");
  fprintf(vtkFile, "LOOKUP_TABLE default\n");


  for(int i = 0; i < nT; ++i)
  {
    Tetrahedron& t = tetrahedrons[i]; // Get the current Node

    if(m_WriteBinaryFile == true)
    {
      swapped = t.nSpin;
      DREAM3D::Endian::FromSystemToBig::convert(swapped);
      fwrite(&swapped, sizeof(int), 1, vtkFile);
    }
    else
    {
      fprintf(vtkFile, "%d\n", t.nSpin);
    }

  }
  return err;
}
