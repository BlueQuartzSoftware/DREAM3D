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

#include "QuickSolidMesh.h"

#include <sstream>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/IDataArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuickSolidMesh::QuickSolidMesh() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_GrainIds(NULL)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuickSolidMesh::~QuickSolidMesh()
{
}
// -----------------------------------------------------------------------------
void QuickSolidMesh::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QuickSolidMesh::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->closeFilterGroup();
  return index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuickSolidMesh::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{

  setErrorCondition(0);
  QString ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, -300, int32_t, Int32ArrayType, voxels, 1)

  SolidMeshDataContainer* sm = getSolidMeshDataContainer();
  if (NULL == sm)
  {
      addErrorMessage(getHumanLabel(), "SolidMeshDataContainer is missing", -383);
      setErrorCondition(-384);
  }
  else 
  {
    StructArray<Node>::Pointer vertices = StructArray<Node>::CreateArray(1, DREAM3D::CellData::SolidMeshNodes);
    StructArray<Tetrahedron>::Pointer tetrahedrons = StructArray<Tetrahedron>::CreateArray(1, DREAM3D::CellData::SolidMeshTetrahedrons);

    sm->setNodes(vertices);
    sm->setTetrahedrons(tetrahedrons);
  }
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuickSolidMesh::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QuickSolidMesh::execute()
{
  setErrorCondition(0);
  VoxelDataContainer* m = getVoxelDataContainer();
  SolidMeshDataContainer* sm = getSolidMeshDataContainer();
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

  int numNodes = (xP+1)*(yP+1)*(zP+1);
  int numTetrahedrons = 6*(xP*yP*zP);

  size_t point;
  size_t nodeId1, nodeId2, nodeId3, nodeId4, nodeId5, nodeId6, nodeId7, nodeId8;

  StructArray<Node>::Pointer vertices = StructArray<Node>::CreateArray(numNodes, DREAM3D::CellData::SolidMeshNodes);
  StructArray<Tetrahedron>::Pointer tetrahedrons = StructArray<Tetrahedron>::CreateArray(numTetrahedrons, DREAM3D::CellData::SolidMeshTetrahedrons);
  Node* vertex = vertices.get()->GetPointer(0);
  Tetrahedron* tetrahedron = tetrahedrons.get()->GetPointer(0);

  for(size_t k = 0; k < zP; k++)
  {
	  for(size_t j = 0; j < yP; j++)
	  {
		  for(size_t i = 0; i < xP; i++)
		  {
			point = (k*xP*yP)+(j*xP)+i;

			nodeId1 = (k*(xP+1)*(yP+1)) + (j*(xP+1)) + i;
			vertex[nodeId1].coord[0] = (i*xRes) - (xRes/2.0);
			vertex[nodeId1].coord[1] = (j*yRes) - (yRes/2.0);
			vertex[nodeId1].coord[2] = (k*zRes) - (zRes/2.0);

			nodeId2 = (k*(xP+1)*(yP+1)) + (j*(xP+1)) + (i+1);
			vertex[nodeId2].coord[0] = ((i+1)*xRes) - (xRes/2.0);
			vertex[nodeId2].coord[1] = (j*yRes) - (yRes/2.0);
			vertex[nodeId2].coord[2] = (k*zRes) - (zRes/2.0);

			nodeId3 = (k*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + i;
			vertex[nodeId3].coord[0] = (i*xRes) - (xRes/2.0);
			vertex[nodeId3].coord[1] = ((j+1)*yRes) - (yRes/2.0);
			vertex[nodeId3].coord[2] = (k*zRes) - (zRes/2.0);

			nodeId4 = (k*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
			vertex[nodeId4].coord[0] = ((i+1)*xRes) - (xRes/2.0);
			vertex[nodeId4].coord[1] = ((j+1)*yRes) - (yRes/2.0);
			vertex[nodeId4].coord[2] = (k*zRes) - (zRes/2.0);

			nodeId5 = ((k+1)*(xP+1)*(yP+1)) + (j*(xP+1)) + i;
			vertex[nodeId5].coord[0] = (i*xRes) - (xRes/2.0);
			vertex[nodeId5].coord[1] = (j*yRes) - (yRes/2.0);
			vertex[nodeId5].coord[2] = ((k+1)*zRes) - (zRes/2.0);

			nodeId6 = ((k+1)*(xP+1)*(yP+1)) + (j*(xP+1)) + (i+1);
			vertex[nodeId6].coord[0] = ((i+1)*xRes) - (xRes/2.0);
			vertex[nodeId6].coord[1] = (j*yRes) - (yRes/2.0);
			vertex[nodeId6].coord[2] = ((k+1)*zRes) - (zRes/2.0);

			nodeId7 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + i;
			vertex[nodeId7].coord[0] = (i*xRes) - (xRes/2.0);
			vertex[nodeId7].coord[1] = ((j+1)*yRes) - (yRes/2.0);
			vertex[nodeId7].coord[2] = ((k+1)*zRes) - (zRes/2.0);

			nodeId8 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + (i+1);
			vertex[nodeId8].coord[0] = ((i+1)*xRes) - (xRes/2.0);
			vertex[nodeId8].coord[1] = ((j+1)*yRes) - (yRes/2.0);
			vertex[nodeId8].coord[2] = ((k+1)*zRes) - (zRes/2.0);

			tetrahedron[6*point].node_id[0] = nodeId1;
			tetrahedron[6*point].node_id[1] = nodeId2;
			tetrahedron[6*point].node_id[2] = nodeId3;
			tetrahedron[6*point].node_id[3] = nodeId7;
			tetrahedron[6*point].nSpin = m_GrainIds[point];

			tetrahedron[6*point+1].node_id[0] = nodeId2;
			tetrahedron[6*point+1].node_id[1] = nodeId4;
			tetrahedron[6*point+1].node_id[2] = nodeId3;
			tetrahedron[6*point+1].node_id[3] = nodeId7;
			tetrahedron[6*point+1].nSpin = m_GrainIds[point];

			tetrahedron[6*point+2].node_id[0] = nodeId1;
			tetrahedron[6*point+2].node_id[1] = nodeId2;
			tetrahedron[6*point+2].node_id[2] = nodeId7;
			tetrahedron[6*point+2].node_id[3] = nodeId5;
			tetrahedron[6*point+2].nSpin = m_GrainIds[point];

			tetrahedron[6*point+3].node_id[0] = nodeId2;
			tetrahedron[6*point+3].node_id[1] = nodeId4;
			tetrahedron[6*point+3].node_id[2] = nodeId7;
			tetrahedron[6*point+3].node_id[3] = nodeId8;
			tetrahedron[6*point+3].nSpin = m_GrainIds[point];

			tetrahedron[6*point+4].node_id[0] = nodeId2;
			tetrahedron[6*point+4].node_id[1] = nodeId5;
			tetrahedron[6*point+4].node_id[2] = nodeId6;
			tetrahedron[6*point+4].node_id[3] = nodeId7;
			tetrahedron[6*point+4].nSpin = m_GrainIds[point];

			tetrahedron[6*point+5].node_id[0] = nodeId6;
			tetrahedron[6*point+5].node_id[1] = nodeId8;
			tetrahedron[6*point+5].node_id[2] = nodeId2;
			tetrahedron[6*point+5].node_id[3] = nodeId7;
			tetrahedron[6*point+5].nSpin = m_GrainIds[point];
		  }

	  }
  }

  sm->setTetrahedrons(tetrahedrons);
  sm->setNodes(vertices);

  notifyStatusMessage("Complete");
}


