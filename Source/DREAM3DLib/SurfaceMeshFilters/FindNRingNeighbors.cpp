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

#include "MXA/Common/MXAEndian.h"
#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/MXADir.h"

namespace Detail {
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

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNRingNeighbors::FindNRingNeighbors() :
m_SurfaceMeshDataContainer(NULL),
  m_TriangleId(-1),
  m_RegionId(0),
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
UniqueTriangleIds_t& FindNRingNeighbors::getNRingTriangles()
{
  return m_NRingTriangles;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNRingNeighbors::generate(NodeTrianglesMap_t &node2Triangle)
{
  assert(m_SurfaceMeshDataContainer != NULL);

  // Write the triangle indices into the vtk File
  StructArray<Triangle>::Pointer trianglesPtr = getSurfaceMeshDataContainer()->getTriangles();
  Triangle* triangles = trianglesPtr->GetPointer(0);

  // Clear out any previous triangles
  m_NRingTriangles.clear();
  Triangle& tri = triangles[m_TriangleId];
  // Add our seed triangle

#if 1
  if (tri.nSpin[0] != m_RegionId && tri.nSpin[1] != m_RegionId)
  {
    std::cout << "FindNRingNeighbors Seed triangle ID does not have a matching Region ID for " << m_RegionId << std::endl;
    std::cout << "Region Ids are: " << triangles[m_TriangleId].nSpin[0] << " & " << triangles[m_TriangleId].nSpin[1] << std::endl;
    return;
  }
#endif

  m_NRingTriangles.insert(m_TriangleId);

  for (int ring = 0; ring < m_Ring; ++ring)
  {
    // Make a copy of the 1 Ring Triangles that we just found so that we can use those triangles as the
    // seed triangles for the 2 Ring triangles
    UniqueTriangleIds_t lcvTriangles(m_NRingTriangles);

    // Now that we have the 1 ring triangles, get the 2 Ring neighbors from that list
    for(UniqueTriangleIds_t::iterator triIter = lcvTriangles.begin(); triIter != lcvTriangles.end(); ++triIter)
    {
      Triangle& t = triangles[*triIter];
      // For each node, get the triangle ids that the node belongs to
      for(int i = 0; i < 3; ++i)
      {
        // Get all the triangles for this Node id
        UniqueTriangleIds_t& tids = node2Triangle[t.node_id[i]];

        // Copy all the triangles into our "2Ring" set which will be the unique set of triangle ids
        for(UniqueTriangleIds_t::iterator iter = tids.begin(); iter != tids.end(); ++iter)
        {
          TriangleId_t triId = *iter;
          if (triangles[triId].nSpin[0] == m_RegionId || triangles[triId].nSpin[1] == m_RegionId)
          {
            m_NRingTriangles.insert(*iter);
          }
        }
      }
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNRingNeighbors::writeVTKFile(const std::string &outputVtkFile)
{

  SurfaceMeshDataContainer* m = getSurfaceMeshDataContainer();
  /* Place all your code to execute your filter here. */
  StructArray<Node>::Pointer nodesPtr = m->getNodes();
  StructArray<Node>& nodes = *(nodesPtr);
  int nNodes = nodes.GetNumberOfTuples();

  std::stringstream ss;


  FILE* vtkFile = NULL;
  vtkFile = fopen(outputVtkFile.c_str(), "wb");
  if (NULL == vtkFile)
  {
    ss.str("");
    ss << "Error creating file '" << outputVtkFile << "'";
    return;
  }
  Detail::ScopedFileMonitor vtkFileMonitor(vtkFile);

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
    Node& n = nodes[i]; // Get the current Node
    //  if (m_SurfaceMeshNodeType[i] > 0)
    {
      pos[0] = static_cast<float>(n.coord[0]);
      pos[1] = static_cast<float>(n.coord[1]);
      pos[2] = static_cast<float>(n.coord[2]);
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
  StructArray<Triangle>& triangles = *(m->getTriangles());

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
  for (std::set<TriangleId_t>::iterator iter = m_NRingTriangles.begin(); iter != m_NRingTriangles.end(); ++iter)
  {
    TriangleId_t tid = *iter;
    tData[1] = triangles[tid].node_id[0];
    tData[2] = triangles[tid].node_id[1];
    tData[3] = triangles[tid].node_id[2];
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


