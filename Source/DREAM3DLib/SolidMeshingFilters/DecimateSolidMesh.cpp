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

#include "DecimateSolidMesh.h"

#include <sstream>

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/IDataArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DecimateSolidMesh::DecimateSolidMesh() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_GoalElementNumber(0),
m_GrainIds(NULL)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DecimateSolidMesh::~DecimateSolidMesh()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DecimateSolidMesh::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{

  setErrorCondition(0);
  std::stringstream ss;

  SolidMeshDataContainer* sm = getSolidMeshDataContainer();
  if (NULL == sm)
  {
      addErrorMessage(getHumanLabel(), "SolidMeshDataContainer is missing", -383);
      setErrorCondition(-384);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DecimateSolidMesh::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Goal Number of Elements");
    option->setPropertyName("GoalElementNumber");
    option->setWidgetType(FilterParameter::IntWidget);
    option->setValueType("int");
	option->setUnits("");
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}


// -----------------------------------------------------------------------------
void DecimateSolidMesh::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("GoalElementNumber", getGoalElementNumber() );
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DecimateSolidMesh::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DecimateSolidMesh::execute()
{
  setErrorCondition(0);
  VoxelDataContainer* m = getVoxelDataContainer();
  SolidMeshDataContainer* sm = getSolidMeshDataContainer();

  int64_t totalPoints = m->getTotalPoints();
  size_t totalFields = m->getNumFieldTuples();
  size_t totalEnsembles = m->getNumEnsembleTuples();
  dataCheck(false, totalPoints, totalFields, totalEnsembles);
  if (getErrorCondition() < 0)
  {
    return;
  }


   StructArray<Node>& nodes = *(sm->getNodes());
   int numNodes = nodes.GetNumberOfTuples();

   StructArray<Tetrahedron>& tetrahedrons = *(sm->getTetrahedrons());
   int numTets = tetrahedrons.GetNumberOfTuples();

  int gnum;
  size_t nodeId1, nodeId2, nodeId3, nodeId4;

  //Locate Boundary Nodes
  std::vector<int> nodeGrainIds(numNodes,-1);
  std::vector<int> nodeEuclideanDistances(numNodes,-1);
  for(size_t k = 0; k < numTets; k++)
  {
	  gnum = tetrahedrons[k].nSpin;
	  nodeId1 = tetrahedrons[k].node_id[0];
	  nodeId2 = tetrahedrons[k].node_id[1];
	  nodeId3 = tetrahedrons[k].node_id[2];
	  nodeId4 = tetrahedrons[k].node_id[3];
	  if(nodeGrainIds[nodeId1] == -1 || nodeGrainIds[nodeId1] == gnum) nodeGrainIds[nodeId1] = gnum;
	  else 
	  {
		  nodeGrainIds[nodeId1] = -1000;
		  nodeEuclideanDistances[nodeId1] = 0;
	  }
	  if(nodeGrainIds[nodeId2] == -1 || nodeGrainIds[nodeId2] == gnum) nodeGrainIds[nodeId2] = gnum;
	  else 
	  {
		  nodeGrainIds[nodeId2] = -1000;
		  nodeEuclideanDistances[nodeId2] = 0;
	  }
	  if(nodeGrainIds[nodeId3] == -1 || nodeGrainIds[nodeId3] == gnum) nodeGrainIds[nodeId3] = gnum;
	  else 
	  {
		  nodeGrainIds[nodeId3] = -1000;
		  nodeEuclideanDistances[nodeId3] = 0;
	  }
	  if(nodeGrainIds[nodeId4] == -1 || nodeGrainIds[nodeId4] == gnum) nodeGrainIds[nodeId4] = gnum;
	  else 
	  {
		  nodeGrainIds[nodeId4] = -1000;
		  nodeEuclideanDistances[nodeId4] = 0;
	  }
  }
  nodeGrainIds.clear();

  //Determine Node Euclidean Distance Map
  std::vector<bool> tetDone(numTets,false);
  int tetsLeft = 1;
  int ED1, ED2, ED3, ED4;
  int minED = 1000000;
  int maxGlobalED = 0;
  while(tetsLeft > 0)
  {
	  tetsLeft = 0;
	  for(size_t k = 0; k < numTets; k++)
	  {
		  if(tetDone[k] == false)
		  {
			  minED = -1;
			  nodeId1 = tetrahedrons[k].node_id[0];
			  nodeId2 = tetrahedrons[k].node_id[1];
			  nodeId3 = tetrahedrons[k].node_id[2];
			  nodeId4 = tetrahedrons[k].node_id[3];
			  ED1 = nodeEuclideanDistances[nodeId1];
			  ED2 = nodeEuclideanDistances[nodeId2];
			  ED3 = nodeEuclideanDistances[nodeId3];
			  ED4 = nodeEuclideanDistances[nodeId4];
			  if(ED1 == -1 || ED2 == -1 || ED3 == -1 || ED4 == -1) tetsLeft++;
			  if(ED1 != -1) minED = ED1;
			  if(ED2 != -1 && ED2 < minED) minED = ED2;
			  if(ED3 != -1 && ED3 < minED) minED = ED3;
			  if(ED4 != -1 && ED4 < minED) minED = ED4;
			  if (minED != -1)
			  {
				  if(ED1 == -1 || ED1 > minED) nodeEuclideanDistances[nodeId1] = minED+1;
				  if(ED2 == -1 || ED2 > minED) nodeEuclideanDistances[nodeId2] = minED+1;
				  if(ED3 == -1 || ED3 > minED) nodeEuclideanDistances[nodeId3] = minED+1;
				  if(ED4 == -1 || ED4 > minED) nodeEuclideanDistances[nodeId4] = minED+1;
				  tetDone[k] = true;
				  if((minED+1) > maxGlobalED) maxGlobalED = minED+1;
			  }
		  }
	  }
  }
  tetDone.clear();

  //Decimate Mesh
  std::vector<bool> tetKilled(numTets,false);
  std::vector<bool> nodeKilled(numNodes,false);
  std::vector<int> newNodeIds(numNodes,-1);
  tetsLeft = numTets;
  ED1, ED2, ED3, ED4;
  int removeED = maxGlobalED;
  int secondLargestED = 0;
  int point;
  int otherPoint;
  while(tetsLeft > m_GoalElementNumber && removeED > 1)
  {
	  tetsLeft = 0;
	  for(size_t k = 0; k < numTets; k++)
	  {
		  if(tetKilled[k] == false)
		  {
			  nodeId1 = tetrahedrons[k].node_id[0];
			  nodeId2 = tetrahedrons[k].node_id[1];
			  nodeId3 = tetrahedrons[k].node_id[2];
			  nodeId4 = tetrahedrons[k].node_id[3];
			  if(nodeKilled[nodeId1] == true)
			  {
				  if(newNodeIds[nodeId1] == nodeId2 || newNodeIds[nodeId1] == nodeId3 || newNodeIds[nodeId1] == nodeId4) tetKilled[k] = true;
				  else 
				  {
					while(newNodeIds[tetrahedrons[k].node_id[0]] != -1)
					{
						tetrahedrons[k].node_id[0] = newNodeIds[tetrahedrons[k].node_id[0]];
					}
				  }
			  }
			  if(nodeKilled[nodeId2] == true)
			  {
				  if(newNodeIds[nodeId3] == nodeId1 || newNodeIds[nodeId2] == nodeId3 || newNodeIds[nodeId2] == nodeId4) tetKilled[k] = true;
				  else 
				  {
					while(newNodeIds[tetrahedrons[k].node_id[1]] != -1)
					{
						tetrahedrons[k].node_id[1] = newNodeIds[tetrahedrons[k].node_id[1]];
					}
				  }
			  }
			  if(nodeKilled[nodeId3] == true)
			  {
				  if(newNodeIds[nodeId3] == nodeId1 || newNodeIds[nodeId3] == nodeId2 || newNodeIds[nodeId3] == nodeId4) tetKilled[k] = true;
				  else 
				  {
					while(newNodeIds[tetrahedrons[k].node_id[2]] != -1)
					{
						tetrahedrons[k].node_id[2] = newNodeIds[tetrahedrons[k].node_id[2]];
					}
				  }
			  }
			  if(nodeKilled[nodeId4] == true)
			  {
				  if(newNodeIds[nodeId4] == nodeId1 || newNodeIds[nodeId4] == nodeId2 || newNodeIds[nodeId4] == nodeId3) tetKilled[k] = true;
				  else 
				  {
					while(newNodeIds[tetrahedrons[k].node_id[3]] != -1)
					{
						tetrahedrons[k].node_id[3] = newNodeIds[tetrahedrons[k].node_id[3]];
					}
				  }
			  }
			  if(tetKilled[k] == false)
			  {
				  ED1 = nodeEuclideanDistances[nodeId1];
				  ED2 = nodeEuclideanDistances[nodeId2];
				  ED3 = nodeEuclideanDistances[nodeId3];
				  ED4 = nodeEuclideanDistances[nodeId4];
				  if(ED1 >= removeED)
				  {
					  point = nodeId1;
					  secondLargestED = ED2;
					  otherPoint = nodeId2;
					  if(ED3 > secondLargestED) secondLargestED = ED3, otherPoint = nodeId3;
					  if(ED4 > secondLargestED) secondLargestED = ED4, otherPoint = nodeId4;
				  }
				  else if(ED2 >= removeED)
				  {
					  point = nodeId2;
					  secondLargestED = ED1;
					  otherPoint = nodeId1;
					  if(ED3 > secondLargestED) secondLargestED = ED3, otherPoint = nodeId3;
					  if(ED4 > secondLargestED) secondLargestED = ED4, otherPoint = nodeId4;
				  }
				  else if(ED3 >= removeED)
				  {
					  point = nodeId3;
					  secondLargestED = ED1;
					  otherPoint = nodeId1;
					  if(ED2 > secondLargestED) secondLargestED = ED2, otherPoint = nodeId2;
					  if(ED4 > secondLargestED) secondLargestED = ED4, otherPoint = nodeId4;
				  }
				  else if(ED4 >= removeED)
				  {
					  point = nodeId4;
					  secondLargestED = ED1;
					  otherPoint = nodeId1;
					  if(ED2 > secondLargestED) secondLargestED = ED2, otherPoint = nodeId2;
					  if(ED3 > secondLargestED) secondLargestED = ED3, otherPoint = nodeId3;
				  }
				  nodeKilled[point] = true;
				  newNodeIds[point] = otherPoint;
				  tetKilled[k] = true;
			  }
		  }
		  if(tetKilled[k] == false) tetsLeft++;
	  }
	  removeED = removeED-1;
  }
  tetKilled.clear();

  int nodeCount = 0;
  for(size_t i=0;i<numNodes;i++)
  {
	if(nodeKilled[i] == false)
	{
		nodes.CopyTuple(i, nodeCount);
		nodeCount++;
	}
  }
  nodes.Resize(nodeCount);

  int tetCount = 0;
  bool good = false;
  for(size_t i=0;i<numTets;i++)
  {
	if(tetKilled[i] == false)
	{
		tetrahedrons.CopyTuple(i, tetCount);
	    nodeId1 = tetrahedrons[tetCount].node_id[0];
		good = false;
		while(good == false)
		{

		}
	    nodeId2 = tetrahedrons[tetCount].node_id[1];
	    nodeId3 = tetrahedrons[tetCount].node_id[2];
	    nodeId4 = tetrahedrons[tetCount].node_id[3];
		tetCount++;
	}
  }
  tetrahedrons.Resize(tetCount);

  notifyStatusMessage("Complete");
}


