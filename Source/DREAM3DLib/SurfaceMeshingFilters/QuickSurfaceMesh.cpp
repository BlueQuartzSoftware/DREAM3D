/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "QuickSurfaceMesh.h"

#include <sstream>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/IDataArray.h"

#define QSM_GETCOORD(index, res, coord, origin)\
  coord = float((float(index)*float(res)) + float(origin));\

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuickSurfaceMesh::QuickSurfaceMesh() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_GrainIds(NULL)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuickSurfaceMesh::~QuickSurfaceMesh()
{
}
// -----------------------------------------------------------------------------
void QuickSurfaceMesh::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
/* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QuickSurfaceMesh::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuickSurfaceMesh::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{

  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1)

      SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if (NULL == sm)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", -383);
    setErrorCondition(-384);
  }
  else
  {
    DREAM3D::SurfaceMesh::VertListPointer_t vertices = DREAM3D::SurfaceMesh::VertList_t::CreateArray(1, DREAM3D::VertexData::SurfaceMeshNodes);
    DREAM3D::SurfaceMesh::FaceListPointer_t triangles = DREAM3D::SurfaceMesh::FaceList_t::CreateArray(1, DREAM3D::FaceData::SurfaceMeshFaces);
    DataArray<int32_t>::Pointer faceLabelPtr = DataArray<int32_t>::CreateArray(1, 2, DREAM3D::FaceData::SurfaceMeshFaceLabels);
    DataArray<int8_t>::Pointer nodeTypePtr = DataArray<int8_t>::CreateArray(1, 1, DREAM3D::VertexData::SurfaceMeshNodeType);

    sm->setVertices(vertices);
    sm->setFaces(triangles);
    sm->addFaceData(faceLabelPtr->GetName(), faceLabelPtr);
    sm->addVertexData(nodeTypePtr->GetName(), nodeTypePtr);
  }
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuickSurfaceMesh::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuickSurfaceMesh::execute()
{
  setErrorCondition(0);
  VoxelDataContainer* m = getVoxelDataContainer();
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  int64_t totalPoints = m->getTotalPoints();
  size_t totalFields = m->getNumFieldTuples();
  size_t totalEnsembles = m->getNumEnsembleTuples();
  dataCheck(false, totalPoints, totalFields, totalEnsembles);

  float m_OriginX, m_OriginY, m_OriginZ;
  m->getOrigin(m_OriginX, m_OriginY, m_OriginZ);

  if (getErrorCondition() < 0)
  {
    return;
  }

  size_t udims[3] = {0,0,0};
  m->getDimensions(udims);
#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] = {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  size_t xP = dims[0];
  size_t yP = dims[1];
  size_t zP = dims[2];
  float xRes = m->getXRes();
  float yRes = m->getYRes();
  float zRes = m->getZRes();

  std::vector<int> m_NodeIds;
  std::vector<std::set<int> > ownerLists;

  size_t possibleNumNodes = (xP+1)*(yP+1)*(zP+1);
  m_NodeIds.resize(possibleNumNodes, -1);

  int nodeCount = 0;
  int triangleCount = 0;

  size_t point, neigh1, neigh2, neigh3;

  size_t nodeId1, nodeId2, nodeId3, nodeId4;

  //first determining which nodes are actually boundary nodes and
  //count number of nodes and triangles that will be created
  for(size_t k = 0; k < zP; k++)
  {
    for(size_t j = 0; j < yP; j++)
    {
      for(size_t i = 0; i < xP; i++)
      {
      point = (k*xP*yP)+(j*xP)+i;
      neigh1 = point + 1;
      neigh2 = point + xP;
      neigh3 = point + (xP*yP);

      if(i == 0)
      {
        nodeId1 = (k*(xP+1)*(yP+1)) + (j*(xP+1)) + i;
        if(m_NodeIds[nodeId1] == -1)
        {
          m_NodeIds[nodeId1] = nodeCount;
          nodeCount++;
        }
        nodeId2 = (k*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + i;
        if(m_NodeIds[nodeId2] == -1)
        {
          m_NodeIds[nodeId2] = nodeCount;
          nodeCount++;
        }
        nodeId3 = ((k+1)*(xP+1)*(yP+1)) + (j*(xP+1)) + i;
        if(m_NodeIds[nodeId3] == -1)
        {
          m_NodeIds[nodeId3] = nodeCount;
          nodeCount++;
        }
        nodeId4 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + i;
        if(m_NodeIds[nodeId4] == -1)
        {
          m_NodeIds[nodeId4] = nodeCount;
          nodeCount++;
        }
        triangleCount++;
        triangleCount++;
      }
      if(j == 0)
      {
        nodeId1 = (k*(xP+1)*(yP+1)) + (j*(xP+1)) + i;
        if(m_NodeIds[nodeId1] == -1)
        {
          m_NodeIds[nodeId1] = nodeCount;
          nodeCount++;
        }
        nodeId2 = (k*(xP+1)*(yP+1)) + (j*(xP+1)) + (i+1);
        if(m_NodeIds[nodeId2] == -1)
        {
          m_NodeIds[nodeId2] = nodeCount;
          nodeCount++;
        }
        nodeId3 = ((k+1)*(xP+1)*(yP+1)) + (j*(xP+1)) + i;
        if(m_NodeIds[nodeId3] == -1)
        {
          m_NodeIds[nodeId3] = nodeCount;
          nodeCount++;
        }
        nodeId4 = ((k+1)*(xP+1)*(yP+1)) + (j*(xP+1)) + (i+1);
        if(m_NodeIds[nodeId4] == -1)
        {
          m_NodeIds[nodeId4] = nodeCount;
          nodeCount++;
        }
        triangleCount++;
        triangleCount++;
      }
      if(k == 0)
      {
        nodeId1 = (k*(xP+1)*(yP+1)) + (j*(xP+1)) + i;
        if(m_NodeIds[nodeId1] == -1)
        {
          m_NodeIds[nodeId1] = nodeCount;
          nodeCount++;
        }
        nodeId2 = (k*(xP+1)*(yP+1)) + (j*(xP+1)) + (i+1);
        if(m_NodeIds[nodeId2] == -1)
        {
          m_NodeIds[nodeId2] = nodeCount;
          nodeCount++;
        }
        nodeId3 = (k*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + i;
        if(m_NodeIds[nodeId3] == -1)
        {
          m_NodeIds[nodeId3] = nodeCount;
          nodeCount++;
        }
        nodeId4 = (k*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
        if(m_NodeIds[nodeId4] == -1)
        {
          m_NodeIds[nodeId4] = nodeCount;
          nodeCount++;
        }
        triangleCount++;
        triangleCount++;
      }
      if(i == (xP-1))
      {
          nodeId1 = (k*(xP+1)*(yP+1)) + (j*(xP+1)) + (i+1);
          if(m_NodeIds[nodeId1] == -1)
          {
            m_NodeIds[nodeId1] = nodeCount;
            nodeCount++;
          }
          nodeId2 = (k*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
          if(m_NodeIds[nodeId2] == -1)
          {
            m_NodeIds[nodeId2] = nodeCount;
            nodeCount++;
          }
          nodeId3 = ((k+1)*(xP+1)*(yP+1)) + (j*(xP+1)) + (i+1);
          if(m_NodeIds[nodeId3] == -1)
          {
            m_NodeIds[nodeId3] = nodeCount;
            nodeCount++;
          }
          nodeId4 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
          if(m_NodeIds[nodeId4] == -1)
          {
            m_NodeIds[nodeId4] = nodeCount;
            nodeCount++;
          }
          triangleCount++;
          triangleCount++;
      }
    else if(m_GrainIds[point] != m_GrainIds[neigh1])
      {
          nodeId1 = (k*(xP+1)*(yP+1)) + (j*(xP+1)) + (i+1);
          if(m_NodeIds[nodeId1] == -1)
          {
            m_NodeIds[nodeId1] = nodeCount;
            nodeCount++;
          }
          nodeId2 = (k*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
          if(m_NodeIds[nodeId2] == -1)
          {
            m_NodeIds[nodeId2] = nodeCount;
            nodeCount++;
          }
          nodeId3 = ((k+1)*(xP+1)*(yP+1)) + (j*(xP+1)) + (i+1);
          if(m_NodeIds[nodeId3] == -1)
          {
            m_NodeIds[nodeId3] = nodeCount;
            nodeCount++;
          }
          nodeId4 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
          if(m_NodeIds[nodeId4] == -1)
          {
            m_NodeIds[nodeId4] = nodeCount;
            nodeCount++;
          }
          triangleCount++;
          triangleCount++;
      }
      if(j == (yP-1))
      {
          nodeId1 = (k*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
          if(m_NodeIds[nodeId1] == -1)
          {
            m_NodeIds[nodeId1] = nodeCount;
            nodeCount++;
          }
          nodeId2 = (k*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + i;
          if(m_NodeIds[nodeId2] == -1)
          {
            m_NodeIds[nodeId2] = nodeCount;
            nodeCount++;
          }
          nodeId3 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
          if(m_NodeIds[nodeId3] == -1)
          {
            m_NodeIds[nodeId3] = nodeCount;
            nodeCount++;
          }
          nodeId4 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + i;
          if(m_NodeIds[nodeId4] == -1)
          {
            m_NodeIds[nodeId4] = nodeCount;
            nodeCount++;
          }
          triangleCount++;
          triangleCount++;
      }
    else if(m_GrainIds[point] != m_GrainIds[neigh2])
      {
          nodeId1 = (k*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
          if(m_NodeIds[nodeId1] == -1)
          {
            m_NodeIds[nodeId1] = nodeCount;
            nodeCount++;
          }
          nodeId2 = (k*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + i;
          if(m_NodeIds[nodeId2] == -1)
          {
            m_NodeIds[nodeId2] = nodeCount;
            nodeCount++;
          }
          nodeId3 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
          if(m_NodeIds[nodeId3] == -1)
          {
            m_NodeIds[nodeId3] = nodeCount;
            nodeCount++;
          }
          nodeId4 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + i;
          if(m_NodeIds[nodeId4] == -1)
          {
            m_NodeIds[nodeId4] = nodeCount;
            nodeCount++;
          }
          triangleCount++;
          triangleCount++;
      }
      if(k == (zP-1))
      {
          nodeId1 = ((k+1)*(xP+1)*(yP+1)) + (j*(xP+1)) + (i+1);
          if(m_NodeIds[nodeId1] == -1)
          {
            m_NodeIds[nodeId1] = nodeCount;
            nodeCount++;
          }
          nodeId2 = ((k+1)*(xP+1)*(yP+1)) + (j*(xP+1)) + i;
          if(m_NodeIds[nodeId2] == -1)
          {
            m_NodeIds[nodeId2] = nodeCount;
            nodeCount++;
          }
          nodeId3 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
          if(m_NodeIds[nodeId3] == -1)
          {
            m_NodeIds[nodeId3] = nodeCount;
            nodeCount++;
          }
          nodeId4 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + i;
          if(m_NodeIds[nodeId4] == -1)
          {
            m_NodeIds[nodeId4] = nodeCount;
            nodeCount++;
          }
          triangleCount++;
          triangleCount++;
      }
    else if(k < zP-1 && m_GrainIds[point] != m_GrainIds[neigh3])
      {
          nodeId1 = ((k+1)*(xP+1)*(yP+1)) + (j*(xP+1)) + (i+1);
          if(m_NodeIds[nodeId1] == -1)
          {
            m_NodeIds[nodeId1] = nodeCount;
            nodeCount++;
          }
          nodeId2 = ((k+1)*(xP+1)*(yP+1)) + (j*(xP+1)) + i;
          if(m_NodeIds[nodeId2] == -1)
          {
            m_NodeIds[nodeId2] = nodeCount;
            nodeCount++;
          }
          nodeId3 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
          if(m_NodeIds[nodeId3] == -1)
          {
            m_NodeIds[nodeId3] = nodeCount;
            nodeCount++;
          }
          nodeId4 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + i;
          if(m_NodeIds[nodeId4] == -1)
          {
            m_NodeIds[nodeId4] = nodeCount;
            nodeCount++;
          }
          triangleCount++;
          triangleCount++;
      }
      }
    }
  }

  //now create node and triangle arrays knowing the number that will be needed
  DREAM3D::SurfaceMesh::VertListPointer_t vertices = DREAM3D::SurfaceMesh::VertList_t::CreateArray(nodeCount, DREAM3D::VertexData::SurfaceMeshNodes);
  DREAM3D::SurfaceMesh::FaceListPointer_t triangles = DREAM3D::SurfaceMesh::FaceList_t::CreateArray(triangleCount, DREAM3D::FaceData::SurfaceMeshFaces);
  DataArray<int32_t>::Pointer faceLabelPtr = DataArray<int32_t>::CreateArray(triangleCount, 2, DREAM3D::FaceData::SurfaceMeshFaceLabels);
  DataArray<int8_t>::Pointer nodeTypePtr = DataArray<int8_t>::CreateArray(nodeCount, 1, DREAM3D::VertexData::SurfaceMeshNodeType);
  DREAM3D::SurfaceMesh::Vert_t* vertex = vertices.get()->GetPointer(0);
  DREAM3D::SurfaceMesh::Face_t* triangle = triangles.get()->GetPointer(0);
  int32_t* faceLabels = faceLabelPtr->GetPointer(0);
  int8_t* nodeTypes = nodeTypePtr->GetPointer(0);

  ownerLists.resize(nodeCount);

  //Cycle through again assigning coordinates to each node and assigning node numbers and grain labels to each triangle
  triangleCount = 0;
  //const DREAM3D::SurfaceMesh::Float_t k_Two = static_cast<DREAM3D::SurfaceMesh::Float_t>(2.0);
  for(size_t k = 0; k < zP; k++)
  {
    for(size_t j = 0; j < yP; j++)
    {
      for(size_t i = 0; i < xP; i++)
      {
      point = (k*xP*yP)+(j*xP)+i;
      neigh1 = point + 1;
      neigh2 = point + xP;
      neigh3 = point + (xP*yP);

      if(i == 0)
      {
        nodeId1 = (k*(xP+1)*(yP+1)) + (j*(xP+1)) + i;
        QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId1]].pos[0], m_OriginX);
        QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId1]].pos[1], m_OriginY);
        QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId1]].pos[2], m_OriginZ);

        nodeId2 = (k*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + i;
        QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId2]].pos[0], m_OriginX);
        QSM_GETCOORD((j+1), yRes, vertex[m_NodeIds[nodeId2]].pos[1], m_OriginY);
        QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId2]].pos[2], m_OriginZ);

        nodeId3 = ((k+1)*(xP+1)*(yP+1)) + (j*(xP+1)) + i;
        QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId3]].pos[0], m_OriginX);
        QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId3]].pos[1], m_OriginY);
        QSM_GETCOORD((k+1), zRes, vertex[m_NodeIds[nodeId3]].pos[2], m_OriginZ);

        nodeId4 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + i;
        QSM_GETCOORD((i+1), xRes, vertex[m_NodeIds[nodeId4]].pos[0], m_OriginX);
        QSM_GETCOORD((j+1), yRes, vertex[m_NodeIds[nodeId4]].pos[1], m_OriginY);
        QSM_GETCOORD((k+1), zRes, vertex[m_NodeIds[nodeId4]].pos[2], m_OriginZ);

        triangle[triangleCount].verts[0] = m_NodeIds[nodeId1];
        triangle[triangleCount].verts[1] = m_NodeIds[nodeId2];
        triangle[triangleCount].verts[2] = m_NodeIds[nodeId3];
        faceLabels[triangleCount*2] = m_GrainIds[point];
        faceLabels[triangleCount*2+1] = -1;
        triangleCount++;

        triangle[triangleCount].verts[0] = m_NodeIds[nodeId2];
        triangle[triangleCount].verts[1] = m_NodeIds[nodeId4];
        triangle[triangleCount].verts[2] = m_NodeIds[nodeId3];
        faceLabels[triangleCount*2] = m_GrainIds[point];
        faceLabels[triangleCount*2+1] = -1;
        triangleCount++;

        ownerLists[m_NodeIds[nodeId1]].insert(m_GrainIds[point]);
        ownerLists[m_NodeIds[nodeId1]].insert(-1);
        ownerLists[m_NodeIds[nodeId2]].insert(m_GrainIds[point]);
        ownerLists[m_NodeIds[nodeId2]].insert(-1);
        ownerLists[m_NodeIds[nodeId3]].insert(m_GrainIds[point]);
        ownerLists[m_NodeIds[nodeId3]].insert(-1);
        ownerLists[m_NodeIds[nodeId4]].insert(m_GrainIds[point]);
        ownerLists[m_NodeIds[nodeId4]].insert(-1);
      }
      if(j == 0)
      {
        nodeId1 = (k*(xP+1)*(yP+1)) + (j*(xP+1)) + i;
        QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId1]].pos[0], m_OriginX);
        QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId1]].pos[1], m_OriginY);
        QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId1]].pos[2], m_OriginZ);

        nodeId2 = (k*(xP+1)*(yP+1)) + (j*(xP+1)) + (i+1);
        QSM_GETCOORD((i+1), xRes, vertex[m_NodeIds[nodeId2]].pos[0], m_OriginX);
        QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId2]].pos[1], m_OriginY);
        QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId2]].pos[2], m_OriginZ);

        nodeId3 = ((k+1)*(xP+1)*(yP+1)) + (j*(xP+1)) + i;
        QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId3]].pos[0], m_OriginX);
        QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId3]].pos[1], m_OriginY);
        QSM_GETCOORD((k+1), zRes, vertex[m_NodeIds[nodeId3]].pos[2], m_OriginZ);

        nodeId4 = ((k+1)*(xP+1)*(yP+1)) + (j*(xP+1)) + (i+1);
        QSM_GETCOORD((i+1), xRes, vertex[m_NodeIds[nodeId4]].pos[0], m_OriginX);
        QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId4]].pos[1], m_OriginY);
        QSM_GETCOORD((k+1), zRes, vertex[m_NodeIds[nodeId4]].pos[2], m_OriginZ);

        triangle[triangleCount].verts[0] = m_NodeIds[nodeId1];
        triangle[triangleCount].verts[1] = m_NodeIds[nodeId3];
        triangle[triangleCount].verts[2] = m_NodeIds[nodeId2];
        faceLabels[triangleCount*2] = m_GrainIds[point];
        faceLabels[triangleCount*2+1] = -1;
        triangleCount++;

        triangle[triangleCount].verts[0] = m_NodeIds[nodeId2];
        triangle[triangleCount].verts[1] = m_NodeIds[nodeId3];
        triangle[triangleCount].verts[2] = m_NodeIds[nodeId4];
        faceLabels[triangleCount*2] = m_GrainIds[point];
        faceLabels[triangleCount*2+1] = -1;
        triangleCount++;

        ownerLists[m_NodeIds[nodeId1]].insert(m_GrainIds[point]);
        ownerLists[m_NodeIds[nodeId1]].insert(-1);
        ownerLists[m_NodeIds[nodeId2]].insert(m_GrainIds[point]);
        ownerLists[m_NodeIds[nodeId2]].insert(-1);
        ownerLists[m_NodeIds[nodeId3]].insert(m_GrainIds[point]);
        ownerLists[m_NodeIds[nodeId3]].insert(-1);
        ownerLists[m_NodeIds[nodeId4]].insert(m_GrainIds[point]);
        ownerLists[m_NodeIds[nodeId4]].insert(-1);
      }
      if(k == 0)
      {
        nodeId1 = (k*(xP+1)*(yP+1)) + (j*(xP+1)) + i;
        QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId1]].pos[0], m_OriginX);
        QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId1]].pos[1], m_OriginY);
        QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId1]].pos[2], m_OriginZ);

        nodeId2 = (k*(xP+1)*(yP+1)) + (j*(xP+1)) + (i+1);
        QSM_GETCOORD((i+1), xRes, vertex[m_NodeIds[nodeId2]].pos[0], m_OriginX);
        QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId2]].pos[1], m_OriginY);
        QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId2]].pos[2], m_OriginZ);

        nodeId3 = (k*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + i;
        QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId3]].pos[0], m_OriginX);
        QSM_GETCOORD((j+1), yRes, vertex[m_NodeIds[nodeId3]].pos[1], m_OriginY);
        QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId3]].pos[2], m_OriginZ);

        nodeId4 = (k*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
        QSM_GETCOORD((i+1), xRes, vertex[m_NodeIds[nodeId4]].pos[0], m_OriginX);
        QSM_GETCOORD((j+1), yRes, vertex[m_NodeIds[nodeId4]].pos[1], m_OriginY);
        QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId4]].pos[2], m_OriginZ);

        triangle[triangleCount].verts[0] = m_NodeIds[nodeId1];
        triangle[triangleCount].verts[1] = m_NodeIds[nodeId2];
        triangle[triangleCount].verts[2] = m_NodeIds[nodeId3];
        faceLabels[triangleCount*2] = m_GrainIds[point];
        faceLabels[triangleCount*2+1] = -1;
        triangleCount++;

        triangle[triangleCount].verts[0] = m_NodeIds[nodeId2];
        triangle[triangleCount].verts[1] = m_NodeIds[nodeId4];
        triangle[triangleCount].verts[2] = m_NodeIds[nodeId3];
        faceLabels[triangleCount*2] = m_GrainIds[point];
        faceLabels[triangleCount*2+1] = -1;
        triangleCount++;

        ownerLists[m_NodeIds[nodeId1]].insert(m_GrainIds[point]);
        ownerLists[m_NodeIds[nodeId1]].insert(-1);
        ownerLists[m_NodeIds[nodeId2]].insert(m_GrainIds[point]);
        ownerLists[m_NodeIds[nodeId2]].insert(-1);
        ownerLists[m_NodeIds[nodeId3]].insert(m_GrainIds[point]);
        ownerLists[m_NodeIds[nodeId3]].insert(-1);
        ownerLists[m_NodeIds[nodeId4]].insert(m_GrainIds[point]);
        ownerLists[m_NodeIds[nodeId4]].insert(-1);
      }
      if(i == (xP-1))
      {
      nodeId1 = (k*(xP+1)*(yP+1)) + (j*(xP+1)) + (i+1);
      QSM_GETCOORD((i+1), xRes, vertex[m_NodeIds[nodeId1]].pos[0], m_OriginX);
      QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId1]].pos[1], m_OriginY);
      QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId1]].pos[2], m_OriginZ);

      nodeId2 = (k*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
      QSM_GETCOORD((i+1), xRes, vertex[m_NodeIds[nodeId2]].pos[0], m_OriginX);
      QSM_GETCOORD((j+1), yRes, vertex[m_NodeIds[nodeId2]].pos[1], m_OriginY);
      QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId2]].pos[2], m_OriginZ);

      nodeId3 = ((k+1)*(xP+1)*(yP+1)) + (j*(xP+1)) + (i+1);
      QSM_GETCOORD((i+1), xRes, vertex[m_NodeIds[nodeId3]].pos[0], m_OriginX);
      QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId3]].pos[1], m_OriginY);
      QSM_GETCOORD((k+1), zRes, vertex[m_NodeIds[nodeId3]].pos[2], m_OriginZ);

      nodeId4 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
      QSM_GETCOORD((i+1), xRes, vertex[m_NodeIds[nodeId4]].pos[0], m_OriginX);
      QSM_GETCOORD((j+1), yRes, vertex[m_NodeIds[nodeId4]].pos[1], m_OriginY);
      QSM_GETCOORD((k+1), zRes, vertex[m_NodeIds[nodeId4]].pos[2], m_OriginZ);

      triangle[triangleCount].verts[0] = m_NodeIds[nodeId3];
      triangle[triangleCount].verts[1] = m_NodeIds[nodeId2];
      triangle[triangleCount].verts[2] = m_NodeIds[nodeId1];
      faceLabels[triangleCount*2] = m_GrainIds[point];
      faceLabels[triangleCount*2+1] = -1;
      triangleCount++;

      triangle[triangleCount].verts[0] = m_NodeIds[nodeId3];
      triangle[triangleCount].verts[1] = m_NodeIds[nodeId4];
      triangle[triangleCount].verts[2] = m_NodeIds[nodeId2];
      faceLabels[triangleCount*2] = m_GrainIds[point];
      faceLabels[triangleCount*2+1] = -1;
      triangleCount++;

      ownerLists[m_NodeIds[nodeId1]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId1]].insert(-1);
      ownerLists[m_NodeIds[nodeId2]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId2]].insert(-1);
      ownerLists[m_NodeIds[nodeId3]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId3]].insert(-1);
      ownerLists[m_NodeIds[nodeId4]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId4]].insert(-1);
      }
      else if(m_GrainIds[point] != m_GrainIds[neigh1])
      {
      nodeId1 = (k*(xP+1)*(yP+1)) + (j*(xP+1)) + (i+1);
      QSM_GETCOORD((i+1), xRes, vertex[m_NodeIds[nodeId1]].pos[0], m_OriginX);
      QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId1]].pos[1], m_OriginY);
      QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId1]].pos[2], m_OriginZ);

      nodeId2 = (k*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
      QSM_GETCOORD((i+1), xRes, vertex[m_NodeIds[nodeId2]].pos[0], m_OriginX);
      QSM_GETCOORD((j+1), yRes, vertex[m_NodeIds[nodeId2]].pos[1], m_OriginY);
      QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId2]].pos[2], m_OriginZ);

      nodeId3 = ((k+1)*(xP+1)*(yP+1)) + (j*(xP+1)) + (i+1);
      QSM_GETCOORD((i+1), xRes, vertex[m_NodeIds[nodeId3]].pos[0], m_OriginX);
      QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId3]].pos[1], m_OriginY);
      QSM_GETCOORD((k+1), zRes, vertex[m_NodeIds[nodeId3]].pos[2], m_OriginZ);

      nodeId4 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
      QSM_GETCOORD((i+1), xRes, vertex[m_NodeIds[nodeId4]].pos[0], m_OriginX);
      QSM_GETCOORD((j+1), yRes, vertex[m_NodeIds[nodeId4]].pos[1], m_OriginY);
      QSM_GETCOORD((k+1), zRes, vertex[m_NodeIds[nodeId4]].pos[2], m_OriginZ);

      triangle[triangleCount].verts[0] = m_NodeIds[nodeId1];
      triangle[triangleCount].verts[1] = m_NodeIds[nodeId2];
      triangle[triangleCount].verts[2] = m_NodeIds[nodeId3];
      faceLabels[triangleCount*2] = m_GrainIds[neigh1];
      faceLabels[triangleCount*2+1] = m_GrainIds[point];
      triangleCount++;

      triangle[triangleCount].verts[0] = m_NodeIds[nodeId2];
      triangle[triangleCount].verts[1] = m_NodeIds[nodeId4];
      triangle[triangleCount].verts[2] = m_NodeIds[nodeId3];
      faceLabels[triangleCount*2] = m_GrainIds[neigh1];
      faceLabels[triangleCount*2+1] = m_GrainIds[point];
      triangleCount++;

      ownerLists[m_NodeIds[nodeId1]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId1]].insert(m_GrainIds[neigh1]);
      ownerLists[m_NodeIds[nodeId2]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId2]].insert(m_GrainIds[neigh1]);
      ownerLists[m_NodeIds[nodeId3]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId3]].insert(m_GrainIds[neigh1]);
      ownerLists[m_NodeIds[nodeId4]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId4]].insert(m_GrainIds[neigh1]);
      }
      if(j == (yP-1))
      {
      nodeId1 = (k*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
      QSM_GETCOORD((i+1), xRes, vertex[m_NodeIds[nodeId1]].pos[0], m_OriginX);
      QSM_GETCOORD((j+1), yRes, vertex[m_NodeIds[nodeId1]].pos[1], m_OriginY);
      QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId1]].pos[2], m_OriginZ);

      nodeId2 = (k*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + i;
      QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId2]].pos[0], m_OriginX);
      QSM_GETCOORD((j+1), yRes, vertex[m_NodeIds[nodeId2]].pos[1], m_OriginY);
      QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId2]].pos[2], m_OriginZ);

      nodeId3 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
      QSM_GETCOORD((i+1), xRes, vertex[m_NodeIds[nodeId3]].pos[0], m_OriginX);
      QSM_GETCOORD((j+1), yRes, vertex[m_NodeIds[nodeId3]].pos[1], m_OriginY);
      QSM_GETCOORD((k+1), zRes, vertex[m_NodeIds[nodeId3]].pos[2], m_OriginZ);

      nodeId4 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + i;
      QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId4]].pos[0], m_OriginX);
      QSM_GETCOORD((j+1), yRes, vertex[m_NodeIds[nodeId4]].pos[1], m_OriginY);
      QSM_GETCOORD((k+1), zRes, vertex[m_NodeIds[nodeId4]].pos[2], m_OriginZ);

      triangle[triangleCount].verts[0] = m_NodeIds[nodeId3];
      triangle[triangleCount].verts[1] = m_NodeIds[nodeId2];
      triangle[triangleCount].verts[2] = m_NodeIds[nodeId1];
      faceLabels[triangleCount*2] = m_GrainIds[point];
      faceLabels[triangleCount*2+1] = -1;
      triangleCount++;

      triangle[triangleCount].verts[0] = m_NodeIds[nodeId3];
      triangle[triangleCount].verts[1] = m_NodeIds[nodeId4];
      triangle[triangleCount].verts[2] = m_NodeIds[nodeId2];
      faceLabels[triangleCount*2] = m_GrainIds[point];
      faceLabels[triangleCount*2+1] = -1;
      triangleCount++;

      ownerLists[m_NodeIds[nodeId1]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId1]].insert(-1);
      ownerLists[m_NodeIds[nodeId2]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId2]].insert(-1);
      ownerLists[m_NodeIds[nodeId3]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId3]].insert(-1);
      ownerLists[m_NodeIds[nodeId4]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId4]].insert(-1);
      }
      else if(m_GrainIds[point] != m_GrainIds[neigh2])
      {
      nodeId1 = (k*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
      QSM_GETCOORD((i+1), xRes, vertex[m_NodeIds[nodeId1]].pos[0], m_OriginX);
      QSM_GETCOORD((j+1), yRes, vertex[m_NodeIds[nodeId1]].pos[1], m_OriginY);
      QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId1]].pos[2], m_OriginZ);

      nodeId2 = (k*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + i;
      QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId2]].pos[0], m_OriginX);
      QSM_GETCOORD((j+1), yRes, vertex[m_NodeIds[nodeId2]].pos[1], m_OriginY);
      QSM_GETCOORD(k, zRes, vertex[m_NodeIds[nodeId2]].pos[2], m_OriginZ);

      nodeId3 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
      QSM_GETCOORD((i+1), xRes, vertex[m_NodeIds[nodeId3]].pos[0], m_OriginX);
      QSM_GETCOORD((j+1), yRes, vertex[m_NodeIds[nodeId3]].pos[1], m_OriginY);
      QSM_GETCOORD((k+1), zRes, vertex[m_NodeIds[nodeId3]].pos[2], m_OriginZ);

      nodeId4 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + i;
      QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId4]].pos[0], m_OriginX);
      QSM_GETCOORD((j+1), yRes, vertex[m_NodeIds[nodeId4]].pos[1], m_OriginY);
      QSM_GETCOORD((k+1), zRes, vertex[m_NodeIds[nodeId4]].pos[2], m_OriginZ);

      triangle[triangleCount].verts[0] = m_NodeIds[nodeId1];
      triangle[triangleCount].verts[1] = m_NodeIds[nodeId2];
      triangle[triangleCount].verts[2] = m_NodeIds[nodeId3];
      faceLabels[triangleCount*2] = m_GrainIds[neigh2];
      faceLabels[triangleCount*2+1] = m_GrainIds[point];
      triangleCount++;

      triangle[triangleCount].verts[0] = m_NodeIds[nodeId2];
      triangle[triangleCount].verts[1] = m_NodeIds[nodeId4];
      triangle[triangleCount].verts[2] = m_NodeIds[nodeId3];
      faceLabels[triangleCount*2] = m_GrainIds[neigh2];
      faceLabels[triangleCount*2+1] = m_GrainIds[point];
      triangleCount++;

      ownerLists[m_NodeIds[nodeId1]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId1]].insert(m_GrainIds[neigh2]);
      ownerLists[m_NodeIds[nodeId2]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId2]].insert(m_GrainIds[neigh2]);
      ownerLists[m_NodeIds[nodeId3]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId3]].insert(m_GrainIds[neigh2]);
      ownerLists[m_NodeIds[nodeId4]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId4]].insert(m_GrainIds[neigh2]);
      }
      if(k == (zP-1))
      {
      nodeId1 = ((k+1)*(xP+1)*(yP+1)) + (j*(xP+1)) + (i+1);
      QSM_GETCOORD((i+1), xRes, vertex[m_NodeIds[nodeId1]].pos[0], m_OriginX);
      QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId1]].pos[1], m_OriginY);
      QSM_GETCOORD((k+1), zRes, vertex[m_NodeIds[nodeId1]].pos[2], m_OriginZ);

      nodeId2 = ((k+1)*(xP+1)*(yP+1)) + (j*(xP+1)) + i;
      QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId2]].pos[0], m_OriginX);
      QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId2]].pos[1], m_OriginY);
      QSM_GETCOORD((k+1), zRes, vertex[m_NodeIds[nodeId2]].pos[2], m_OriginZ);

      nodeId3 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
      QSM_GETCOORD((i+1), xRes, vertex[m_NodeIds[nodeId3]].pos[0], m_OriginX);
      QSM_GETCOORD((j+1), yRes, vertex[m_NodeIds[nodeId3]].pos[1], m_OriginY);
      QSM_GETCOORD((k+1), zRes, vertex[m_NodeIds[nodeId3]].pos[2], m_OriginZ);

      nodeId4 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + i;
      QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId4]].pos[0], m_OriginX);
      QSM_GETCOORD((j+1), yRes, vertex[m_NodeIds[nodeId4]].pos[1], m_OriginY);
      QSM_GETCOORD((k+1), zRes, vertex[m_NodeIds[nodeId4]].pos[2], m_OriginZ);

      triangle[triangleCount].verts[0] = m_NodeIds[nodeId2];
      triangle[triangleCount].verts[1] = m_NodeIds[nodeId3];
      triangle[triangleCount].verts[2] = m_NodeIds[nodeId1];
      faceLabels[triangleCount*2] = m_GrainIds[point];
      faceLabels[triangleCount*2+1] = -1;
      triangleCount++;

      triangle[triangleCount].verts[0] = m_NodeIds[nodeId4];
      triangle[triangleCount].verts[1] = m_NodeIds[nodeId3];
      triangle[triangleCount].verts[2] = m_NodeIds[nodeId2];
      faceLabels[triangleCount*2] = m_GrainIds[point];
      faceLabels[triangleCount*2+1] = -1;
      triangleCount++;

      ownerLists[m_NodeIds[nodeId1]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId1]].insert(-1);
      ownerLists[m_NodeIds[nodeId2]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId2]].insert(-1);
      ownerLists[m_NodeIds[nodeId3]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId3]].insert(-1);
      ownerLists[m_NodeIds[nodeId4]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId4]].insert(-1);
      }
      else if(m_GrainIds[point] != m_GrainIds[neigh3])
      {
      nodeId1 = ((k+1)*(xP+1)*(yP+1)) + (j*(xP+1)) + (i+1);
      QSM_GETCOORD((i+1), xRes, vertex[m_NodeIds[nodeId1]].pos[0], m_OriginX);
      QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId1]].pos[1], m_OriginY);
      QSM_GETCOORD((k+1), zRes, vertex[m_NodeIds[nodeId1]].pos[2], m_OriginZ);

      nodeId2 = ((k+1)*(xP+1)*(yP+1)) + (j*(xP+1)) + i;
      QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId2]].pos[0], m_OriginX);
      QSM_GETCOORD(j, yRes, vertex[m_NodeIds[nodeId2]].pos[1], m_OriginY);
      QSM_GETCOORD((k+1), zRes, vertex[m_NodeIds[nodeId2]].pos[2], m_OriginZ);

      nodeId3 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
      QSM_GETCOORD((i+1), xRes, vertex[m_NodeIds[nodeId3]].pos[0], m_OriginX);
      QSM_GETCOORD((j+1), yRes, vertex[m_NodeIds[nodeId3]].pos[1], m_OriginY);
      QSM_GETCOORD((k+1), zRes, vertex[m_NodeIds[nodeId3]].pos[2], m_OriginZ);

      nodeId4 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + i;
      QSM_GETCOORD(i, xRes, vertex[m_NodeIds[nodeId4]].pos[0], m_OriginX);
      QSM_GETCOORD((j+1), yRes, vertex[m_NodeIds[nodeId4]].pos[1], m_OriginY);
      QSM_GETCOORD((k+1), zRes, vertex[m_NodeIds[nodeId4]].pos[2], m_OriginZ);

      triangle[triangleCount].verts[0] = m_NodeIds[nodeId1];
      triangle[triangleCount].verts[1] = m_NodeIds[nodeId3];
      triangle[triangleCount].verts[2] = m_NodeIds[nodeId2];
      faceLabels[triangleCount*2] = m_GrainIds[neigh3];
      faceLabels[triangleCount*2+1] = m_GrainIds[point];
      triangleCount++;

      triangle[triangleCount].verts[0] = m_NodeIds[nodeId2];
      triangle[triangleCount].verts[1] = m_NodeIds[nodeId3];
      triangle[triangleCount].verts[2] = m_NodeIds[nodeId4];
      faceLabels[triangleCount*2] = m_GrainIds[neigh3];
      faceLabels[triangleCount*2+1] = m_GrainIds[point];
      triangleCount++;

      ownerLists[m_NodeIds[nodeId1]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId1]].insert(m_GrainIds[neigh3]);
      ownerLists[m_NodeIds[nodeId2]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId2]].insert(m_GrainIds[neigh3]);
      ownerLists[m_NodeIds[nodeId3]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId3]].insert(m_GrainIds[neigh3]);
      ownerLists[m_NodeIds[nodeId4]].insert(m_GrainIds[point]);
      ownerLists[m_NodeIds[nodeId4]].insert(m_GrainIds[neigh3]);
      }
      }
    }
  }

  for (int i = 0; i < nodeCount; i++)
  {
    nodeTypes[i] = ownerLists[i].size();
    if(nodeTypes[i] > 4) nodeTypes[i] = 4;
    if(ownerLists[i].find(-1) != ownerLists[i].end()) nodeTypes[i] = nodeTypes[i]+10;
  }

  sm->setFaces(triangles);
  sm->addFaceData(faceLabelPtr->GetName(), faceLabelPtr);
  sm->setVertices(vertices);
  sm->addVertexData(nodeTypePtr->GetName(), nodeTypePtr);

  notifyStatusMessage("Complete");
}


