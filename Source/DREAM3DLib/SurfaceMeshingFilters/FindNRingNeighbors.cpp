/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson,
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FindNRingNeighbors.h"

#include <stdio.h>
#include <sstream>


#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include "DREAM3DLib/Common/ManagedArrayOfArrays.hpp"
#include "DREAM3DLib/Common/ScopedFileMonitor.hpp"
#include "DREAM3DLib/SurfaceMeshingFilters/MeshVertLinks.hpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNRingNeighbors::FindNRingNeighbors() :
m_SurfaceMeshDataContainer(NULL),
  m_TriangleId(-1),
  m_RegionId0(0),
  m_RegionId1(0),
  m_Ring(2),
  m_WriteBinaryFile(false),
  m_WriteConformalMesh(true)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNRingNeighbors::~FindNRingNeighbors()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNRingNeighbors::setRegionIds(int g, int r)
{
  m_RegionId0 = g;
  m_RegionId1 = r;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3D::SurfaceMesh::UniqueFaceIds_t &FindNRingNeighbors::getNRingTriangles()
{
  return m_NRingTriangles;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNRingNeighbors::generate()
{
  BOOST_ASSERT(m_SurfaceMeshDataContainer != NULL);
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();

  // Clear out any previous triangles
  m_NRingTriangles.clear();

  // Get the Triangle List from the Data Container
  DREAM3D::SurfaceMesh::FaceListPointer_t trianglesPtr = getSurfaceMeshDataContainer()->getFaces();
  DREAM3D::SurfaceMesh::Face_t* triangles = trianglesPtr->GetPointer(0);

  // Make sure we have the proper connectivity built
  MeshVertLinks::Pointer node2TrianglePtr =sm->getMeshVertLinks();
  if (node2TrianglePtr.get() == NULL)
  {
    sm->buildMeshVertLinks();
    node2TrianglePtr =sm->getMeshVertLinks();
  }

  IDataArray::Pointer flPtr = getSurfaceMeshDataContainer()->getFaceData(DREAM3D::FaceData::SurfaceMeshFaceLabels);
  DataArray<int32_t>* faceLabelsPtr = DataArray<int32_t>::SafePointerDownCast(flPtr.get());
  int32_t* faceLabels = faceLabelsPtr->GetPointer(0);


  // Figure out these boolean values for a sanity check
  bool check0 = faceLabels[m_TriangleId*2] == m_RegionId0 && faceLabels[m_TriangleId*2+1] == m_RegionId1;
  bool check1 = faceLabels[m_TriangleId*2+1] == m_RegionId0 && faceLabels[m_TriangleId*2] == m_RegionId1;

#if 1
  if ( check0 == false && check1 == false)
  {
    qDebug() << "FindNRingNeighbors Seed triangle ID does not have a matching Region ID for " << m_RegionId0 << " & " << m_RegionId1 ;
    qDebug() << "Region Ids are: " << faceLabels[m_TriangleId*2] << " & " << faceLabels[m_TriangleId*2+1] ;
    return;
  }
#endif


  // Add our seed triangle
  m_NRingTriangles.insert(m_TriangleId);

  for (int ring = 0; ring < m_Ring; ++ring)
  {
    // Make a copy of the 1 Ring Triangles that we just found so that we can use those triangles as the
    // seed triangles for the 2 Ring triangles
    DREAM3D::SurfaceMesh::UniqueFaceIds_t lcvTriangles(m_NRingTriangles);

    // Now that we have the 1 ring triangles, get the 2 Ring neighbors from that list
    for(DREAM3D::SurfaceMesh::UniqueFaceIds_t::iterator triIter = lcvTriangles.begin(); triIter != lcvTriangles.end(); ++triIter)
    {
      DREAM3D::SurfaceMesh::Face_t& face = triangles[*triIter];
      // For each node, get the triangle ids that the node belongs to
      for(int i = 0; i < 3; ++i)
      {
        // Get all the triangles for this Node id
        uint16_t tCount = node2TrianglePtr->getNumberOfFaces(face.verts[i]);
        int32_t* data = node2TrianglePtr->getFaceListPointer(face.verts[i]);

        // Copy all the triangles into our "2Ring" set which will be the unique set of triangle ids
        for(uint16_t t = 0; t < tCount; ++t)
        {
          int tid = data[t];
          check0 = faceLabels[tid*2] == m_RegionId0 && faceLabels[tid*2+1] == m_RegionId1;
          check1 = faceLabels[tid*2+1] == m_RegionId0 && faceLabels[tid*2] == m_RegionId1;
          if (check0 == true || check1 == true)
          {
            m_NRingTriangles.insert(static_cast<int>(tid) );
          }
        }
      }
    }
  }

//  if (m_TriangleId == 1000)
//  {
//    QString ss;
//    ss << "/tmp/" << m_Ring << "_RingNeighborhood.vtk";
//    writeVTKFile(ss.str());
//  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNRingNeighbors::writeVTKFile(const QString &outputVtkFile)
{

  SurfaceMeshDataContainer* m = getSurfaceMeshDataContainer();
  /* Place all your code to execute your filter here. */
  DREAM3D::SurfaceMesh::VertListPointer_t nodesPtr = m->getVertices();
  DREAM3D::SurfaceMesh::VertList_t& nodes = *(nodesPtr);
  int nNodes = nodes.GetNumberOfTuples();

  QString ss;


  FILE* vtkFile = NULL;
  vtkFile = fopen(outputVtkFile.c_str(), "wb");
  if (NULL == vtkFile)
  {
    ss.str("");
    ss << "Error creating file '" << outputVtkFile << "'";
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


  fprintf(vtkFile, "POINTS %d float\n", nNodes);
  float pos[3] = {0.0f, 0.0f, 0.0f};

  size_t totalWritten = 0;
  // Write the POINTS data (Vertex)
  for (int i = 0; i < nNodes; i++)
  {
    DREAM3D::SurfaceMesh::Vert_t& n = nodes[i]; // Get the current Node
    //  if (m_SurfaceMeshNodeType[i] > 0)
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
  int nT = m_NRingTriangles.size();
  int triangleCount = nT;
  //  int tn1, tn2, tn3;
  if (false == m_WriteConformalMesh)
  {
    triangleCount = nT * 2;
  }
  // Write the CELLS Data
  fprintf(vtkFile, "POLYGONS %d %d\n", triangleCount, (triangleCount * 4));
  for (QSet<int32_t>::iterator iter = m_NRingTriangles.begin(); iter != m_NRingTriangles.end(); ++iter)
  {
    int32_t tid = *iter;
    tData[1] = triangles[tid].verts[0];
    tData[2] = triangles[tid].verts[1];
    tData[3] = triangles[tid].verts[2];
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



  fprintf(vtkFile, "\n");
}


