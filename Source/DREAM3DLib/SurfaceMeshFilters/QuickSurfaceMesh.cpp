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
void QuickSurfaceMesh::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
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

  std::vector<int> m_NodeIds;
  std::vector<int> m_EdgeIds;
  std::vector<int> m_TriangleIds;

  size_t possibleNumNodes = (xP+1)*(yP+1)*(zP+1);
  size_t possibleNumEdges = (xP*(yP+1)*(zP+1)) + ((xP+1)*yP*(zP+1)) + ((xP+1)*(yP+1)*zP) + ((xP+1)*yP*zP) + (xP*(yP+1)*zP) + (xP*yP*(zP+1));
  size_t possibleNumTriangles = ((xP+1)*yP*zP*2) + (xP*(yP+1)*zP*2) + (xP*yP*(zP+1)*2);
  m_NodeIds.resize(possibleNumNodes, -1);
  m_EdgeIds.resize(possibleNumEdges, -1);
  m_TriangleIds.resize(possibleNumTriangles, -1);

  int nodeCount = 0;
  int edgeCount = 0;
  int triangleCount = 0;

  size_t point, neigh1, neigh2, neigh3;

  size_t nodeId1, nodeId2, nodeId3, nodeId4;
  size_t edgeId1, edgeId2, edgeId3;
  size_t triangleId1, triangleId2;

  for(size_t k = 0; k < (zP-1); k++)
  {
	  for(size_t j = 0; j < (yP-1); j++)
	  {
		  for(size_t i = 0; i < (xP-1); i++)
		  {
			point = (k*xP*yP)+(j*xP)+i;
			neigh1 = point + 1;
			neigh2 = point + xP;
			neigh3 = point + (xP*yP);

			if(m_GrainIds[point] != m_GrainIds[neigh1])
			{
				nodeId1 = (k*(xP+1)*(yP+1)) + (j*(xP+1)) + i + 1;
				if(m_NodeIds[nodeId1] == -1)
				{
					m_NodeIds[nodeId1] = nodeCount;
					nodeCount++;
				}
				nodeId2 = (k*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + i + 1;
				if(m_NodeIds[nodeId2] == -1)
				{
					m_NodeIds[nodeId2] = nodeCount;
					nodeCount++;
				}
				nodeId3 = ((k+1)*(xP+1)*(yP+1)) + (j*(xP+1)) + i + 1;
				if(m_NodeIds[nodeId3] == -1)
				{
					m_NodeIds[nodeId3] = nodeCount;
					nodeCount++;
				}
				nodeId4 = ((k+1)*(xP+1)*(yP+1)) + ((j+1)*(xP+1)) + i + 1;
				if(m_NodeIds[nodeId4] == -1)
				{
					m_NodeIds[nodeId4] = nodeCount;
					nodeCount++;
				}
				edgeId1 = (k*(((xP*6+3)*yP)+(xP*3)+1)) + (j*(xP*6+3)) + ((i+1)*6) + 0;
				if(m_EdgeIds[edgeId1] == -1)
				{
					m_EdgeIds[edgeId1] = edgeCount;
					edgeCount++;
				}
				edgeId2 = (k*(((xP*6+3)*yP)+(xP*3)+1)) + (j*(xP*6+3)) + ((i+1)*6) + 1;
				if(m_EdgeIds[edgeId2] == -1)
				{
					m_EdgeIds[edgeId2] = edgeCount;
					edgeCount++;
				}
				edgeId3 = (k*(((xP*6+3)*yP)+(xP*3)+1)) + (j*(xP*6+3)) + ((i+1)*6) + 2;
				if(m_EdgeIds[edgeId3] == -1)
				{
					m_EdgeIds[edgeId3] = edgeCount;
					edgeCount++;
				}
				triangleId1 = (k*(((xP*6+2)*yP)+(xP*2))) + (j*(xP*6+2)) + ((i+1)*6) + 0;
				if(m_TriangleIds[triangleId1] == -1)
				{
					m_TriangleIds[triangleId1] = triangleCount;
					triangleCount++;
				}
				triangleId2 = (k*(((xP*6+2)*yP)+(xP*2))) + (j*(xP*6+2)) + ((i+1)*6) + 1;
				if(m_TriangleIds[triangleId2] == -1)
				{
					m_TriangleIds[triangleId2] = triangleCount;
					triangleCount++;
				}
			}
			if(m_GrainIds[point] != m_GrainIds[neigh2])
			{

			}
			if(m_GrainIds[point] != m_GrainIds[neigh3])
			{

			}
		  }
	  }
  }

  notifyStatusMessage("Complete");
}


