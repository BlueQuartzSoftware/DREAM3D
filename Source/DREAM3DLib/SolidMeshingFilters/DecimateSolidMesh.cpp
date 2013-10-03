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
#include "DREAM3DLib/DataArrays/IDataArray.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DecimateSolidMesh::DecimateSolidMesh() :
  AbstractFilter(),
  m_GoalElementNumber(0)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DecimateSolidMesh::~DecimateSolidMesh()
{
  writer->closeFilterGroup();
  return index; // we want to return the next index that was just written to
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
  FilterParameterVector parameters;
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
void DecimateSolidMesh::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
////!!##
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DecimateSolidMesh::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
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
  StructArray<Tetrahedron>& tetrahedrons = *(sm->getTetrahedrons());
  int numNodes = nodes.GetNumberOfTuples();
  int numTets = tetrahedrons.GetNumberOfTuples();

  int gnum;
  size_t nodeId1, nodeId2, nodeId3, nodeId4;

  //Locate Boundary Nodes
  nodeGrainIds.resize(numNodes, -1);
  nodeEuclideanDistances.resize(numNodes, -1);
  for(int k = 0; k < numTets; k++)
  {
    gnum = tetrahedrons[k].nSpin;
    //  float x = nodes[k].coord[0];
    nodeId1 = tetrahedrons[k].node_id[0];
    nodeId2 = tetrahedrons[k].node_id[1];
    nodeId3 = tetrahedrons[k].node_id[2];
    nodeId4 = tetrahedrons[k].node_id[3];
    if(nodeGrainIds[nodeId1] == -1 || nodeGrainIds[nodeId1] == gnum) { nodeGrainIds[nodeId1] = gnum; }
    else
    {
      nodeGrainIds[nodeId1] = -1000;
      nodeEuclideanDistances[nodeId1] = 0;
    }
    if(nodeGrainIds[nodeId2] == -1 || nodeGrainIds[nodeId2] == gnum) { nodeGrainIds[nodeId2] = gnum; }
    else
    {
      nodeGrainIds[nodeId2] = -1000;
      nodeEuclideanDistances[nodeId2] = 0;
    }
    if(nodeGrainIds[nodeId3] == -1 || nodeGrainIds[nodeId3] == gnum) { nodeGrainIds[nodeId3] = gnum; }
    else
    {
      nodeGrainIds[nodeId3] = -1000;
      nodeEuclideanDistances[nodeId3] = 0;
    }
    if(nodeGrainIds[nodeId4] == -1 || nodeGrainIds[nodeId4] == gnum) { nodeGrainIds[nodeId4] = gnum; }
    else
    {
      nodeGrainIds[nodeId4] = -1000;
      nodeEuclideanDistances[nodeId4] = 0;
    }
  }
  nodeGrainIds.clear();

  //Determine Node Euclidean Distance Map
  tetDone.resize(numTets, false);
  int tetsLeft = 1;
  int ED1, ED2, ED3, ED4;
  int currentED = 0;
  int maxGlobalED = 0;
  while(tetsLeft > 0)
  {
    tetsLeft = 0;
    currentED++;
    for(int k = 0; k < numTets; k++)
    {
      if(tetDone[k] == false)
      {
        nodeId1 = tetrahedrons[k].node_id[0];
        nodeId2 = tetrahedrons[k].node_id[1];
        nodeId3 = tetrahedrons[k].node_id[2];
        nodeId4 = tetrahedrons[k].node_id[3];
        ED1 = nodeEuclideanDistances[nodeId1];
        ED2 = nodeEuclideanDistances[nodeId2];
        ED3 = nodeEuclideanDistances[nodeId3];
        ED4 = nodeEuclideanDistances[nodeId4];
        if(ED1 == -1 || ED2 == -1 || ED3 == -1 || ED4 == -1) { tetsLeft++; }
        if(ED1 == (currentED - 1) || ED2 == (currentED - 1) || ED3 == (currentED - 1) || ED4 == (currentED - 1))
        {
          if(ED1 == -1) { nodeEuclideanDistances[nodeId1] = currentED; }
          if(ED2 == -1) { nodeEuclideanDistances[nodeId2] = currentED; }
          if(ED3 == -1) { nodeEuclideanDistances[nodeId3] = currentED; }
          if(ED4 == -1) { nodeEuclideanDistances[nodeId4] = currentED; }
          tetDone[k] = true;
        }
      }
    }
    maxGlobalED = currentED;
  }
  tetDone.clear();

  //Decimate Mesh
// ED1, ED2, ED3, ED4;
  int removeED = maxGlobalED;
// int secondLargestED = 0;
  int point;
  int otherPoint;
  while(numTets > m_GoalElementNumber && removeED > 1)
  {
    for(int k = 0; k < numTets; k++)
    {
      nodeId1 = tetrahedrons[k].node_id[0];
      nodeId2 = tetrahedrons[k].node_id[1];
      nodeId3 = tetrahedrons[k].node_id[2];
      nodeId4 = tetrahedrons[k].node_id[3];
      ED1 = nodeEuclideanDistances[nodeId1];
      ED2 = nodeEuclideanDistances[nodeId2];
      ED3 = nodeEuclideanDistances[nodeId3];
      ED4 = nodeEuclideanDistances[nodeId4];
      if(ED1 >= removeED)
      {
        point = nodeId1;
//        secondLargestED = ED2;
        otherPoint = nodeId2;
//        if(ED3 > secondLargestED) secondLargestED = ED3, otherPoint = nodeId3;
//        if(ED4 > secondLargestED) secondLargestED = ED4, otherPoint = nodeId4;
        k = updateNodesandTets(k, point, otherPoint);
      }
      else if(ED2 >= removeED)
      {
        point = nodeId2;
//        secondLargestED = ED1;
        otherPoint = nodeId3;
//        if(ED3 > secondLargestED) secondLargestED = ED3, otherPoint = nodeId3;
//        if(ED4 > secondLargestED) secondLargestED = ED4, otherPoint = nodeId4;
        k = updateNodesandTets(k, point, otherPoint);
      }
      else if(ED3 >= removeED)
      {
        point = nodeId3;
//        secondLargestED = ED1;
        otherPoint = nodeId4;
//        if(ED2 > secondLargestED) secondLargestED = ED2, otherPoint = nodeId2;
//        if(ED4 > secondLargestED) secondLargestED = ED4, otherPoint = nodeId4;
        k = updateNodesandTets(k, point, otherPoint);
      }
      else if(ED4 >= removeED)
      {
        point = nodeId4;
//        secondLargestED = ED1;
        otherPoint = nodeId1;
//        if(ED2 > secondLargestED) secondLargestED = ED2, otherPoint = nodeId2;
//        if(ED3 > secondLargestED) secondLargestED = ED3, otherPoint = nodeId3;
        k = updateNodesandTets(k, point, otherPoint);
      }
      numTets = tetrahedrons.GetNumberOfTuples();
      StructArray<Node>& nodes = *(sm->getNodes());
      StructArray<Tetrahedron>& tetrahedrons = *(sm->getTetrahedrons());
      numNodes = nodes.GetNumberOfTuples();
      numTets = tetrahedrons.GetNumberOfTuples();
    }
    removeED = removeED - 1;
  }

  notifyStatusMessage("Complete");
}

int DecimateSolidMesh::updateNodesandTets(int currentTet, int killedNode, int newNode)
{
  SolidMeshDataContainer* sm = getSolidMeshDataContainer();
  StructArray<Node>& nodes = *(sm->getNodes());
  StructArray<Tetrahedron>& tetrahedrons = *(sm->getTetrahedrons());
  int numNodes = nodes.GetNumberOfTuples();
  int numTets = tetrahedrons.GetNumberOfTuples();

  int nodeId1, nodeId2, nodeId3, nodeId4;
  int counter = 0;
  int nodeCount = 0;
  newNodeIds.resize(numNodes, -1);
  for(int i = 0; i < numNodes; i++)
  {
    if(i != killedNode)
    {
      nodes.CopyTuple(i, nodeCount);
      newNodeIds[i] = nodeCount;
      nodeEuclideanDistances[nodeCount] = nodeEuclideanDistances[i];
      nodeCount++;
    }
  }
  nodes.Resize(nodeCount);
  nodeEuclideanDistances.resize(nodeCount);

  int tetCount = 0;
  for(int i = 0; i < numTets; i++)
  {
    nodeId1 = tetrahedrons[i].node_id[0];
    nodeId2 = tetrahedrons[i].node_id[1];
    nodeId3 = tetrahedrons[i].node_id[2];
    nodeId4 = tetrahedrons[i].node_id[3];
    if((nodeId1 != killedNode && nodeId2 != killedNode && nodeId3 != killedNode && nodeId4 != killedNode) || (nodeId1 != newNode && nodeId2 != newNode && nodeId3 != newNode && nodeId4 != newNode))
    {
      if(nodeId1 == killedNode) { tetrahedrons[i].node_id[0] = newNodeIds[newNode]; }
      else { tetrahedrons[i].node_id[0] = newNodeIds[nodeId1]; }
      if(nodeId2 == killedNode) { tetrahedrons[i].node_id[1] = newNodeIds[newNode]; }
      else { tetrahedrons[i].node_id[1] = newNodeIds[nodeId2]; }
      if(nodeId3 == killedNode) { tetrahedrons[i].node_id[2] = newNodeIds[newNode]; }
      else { tetrahedrons[i].node_id[2] = newNodeIds[nodeId3]; }
      if(nodeId4 == killedNode) { tetrahedrons[i].node_id[3] = newNodeIds[newNode]; }
      else { tetrahedrons[i].node_id[3] = newNodeIds[nodeId4]; }
      tetrahedrons.CopyTuple(i, tetCount);
      tetCount++;
    }
    else
    {
      if(i <= currentTet) { counter++; }
    }
  }
  tetrahedrons.Resize(tetCount);

  return (currentTet - counter);
}


