/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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

 //
 //  This code was written under United States Air Force Contract number
 //                           FA8650-07-D-5800
 //
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "M3CEntireVolume.h"

#include "MXA/Common/MXAMath.h"
#include "MXA/Utilities/MXADir.h"
#include "MXA/Utilities/MXAFileInfo.h"


#ifndef M_PI
#define M_PI 3.14159265
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
M3CEntireVolume::M3CEntireVolume() :
AbstractSurfaceMeshAlgorithm(),
m_SurfaceMeshEdgesArrayName(DREAM3D::CellData::SurfaceMeshEdges),
m_SurfaceMeshInternalEdgesArrayName(DREAM3D::CellData::SurfaceMeshInternalEdges),
m_SurfaceMeshNodeTypeArrayName(DREAM3D::CellData::SurfaceMeshNodeType),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_AddSurfaceLayer(true)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
M3CEntireVolume::~M3CEntireVolume()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CEntireVolume::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
   {
     FilterParameter::Pointer option = FilterParameter::New();
     option->setHumanLabel("Add Surface Layer");
     option->setPropertyName("AddSurfaceLayer");
     option->setWidgetType(FilterParameter::BooleanWidget);
     option->setValueType("bool");
     parameters.push_back(option);
   }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CEntireVolume::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("AddSurfaceLayer", getAddSurfaceLayer() );

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CEntireVolume::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  int32_t* m_GrainIds = NULL;
  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1);
  setGrainIds(m_GrainIds); // This pushes the GrainIds Pointer to the super class

  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if (NULL == sm)
  {
      addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", -383);
      setErrorCondition(-384);
  }
  else {
    StructArray<Node>::Pointer vertices = StructArray<Node>::CreateArray(1, DREAM3D::CellData::SurfaceMeshNodes);
    StructArray<Triangle>::Pointer triangles = StructArray<Triangle>::CreateArray(1, DREAM3D::CellData::SurfaceMeshTriangles);
    StructArray<Segment>::Pointer faceEdges = StructArray<Segment>::CreateArray(1, DREAM3D::CellData::SurfaceMeshEdges);
    StructArray<ISegment>::Pointer internalEdges = StructArray<ISegment>::CreateArray(1, DREAM3D::CellData::SurfaceMeshInternalEdges);

    int8_t* m_SurfaceMeshNodeType = NULL;
    CREATE_NON_PREREQ_DATA(sm, DREAM3D, CellData, SurfaceMeshNodeType, ss, int8_t, Int8ArrayType, 0, 1, 1)
    setSurfaceMeshNodeType(m_SurfaceMeshNodeType); // This pushes this pointer to the superclass


    sm->setNodes(vertices);
    sm->setTriangles(triangles);
    sm->addCellData(DREAM3D::CellData::SurfaceMeshEdges, faceEdges);
    sm->addCellData(DREAM3D::CellData::SurfaceMeshInternalEdges, internalEdges);

    addCreatedCellData(DREAM3D::CellData::SurfaceMeshEdges);
    addCreatedCellData(DREAM3D::CellData::SurfaceMeshInternalEdges);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CEntireVolume::preflight()
{
  /* Place code here that sanity checks input arrays and input values. Look at some
   * of the other DREAM3DLib/Filters/.cpp files for sample codes */
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CEntireVolume::execute()
{
  int err = 0;
  std::stringstream ss;
  setErrorCondition(err);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << " VoxelDataContainer was NULL";
    PipelineMessage em(getNameOfClass(), ss.str(), -1);
    addErrorMessage(em);
    return;
  }
  setErrorCondition(0);

  if(getSurfaceMeshDataContainer() == NULL)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << " SurfaceMeshDataContainer was NULL";
    PipelineMessage em(getNameOfClass(), ss.str(), -1);
    addErrorMessage(em);
    return;
  }

  setErrorCondition(0);
  int64_t totalPoints = m->getTotalPoints();
  size_t totalFields = m->getNumFieldTuples();
  size_t totalEnsembles = m->getNumEnsembleTuples();
  dataCheck(false, totalPoints, totalFields, totalEnsembles);
  if(getErrorCondition() < 0)
  {
    return;
  }

  err = createMesh();
  if(err < 0)
  {
    setErrorCondition(-1);
    std::stringstream ss;
    ss << "Error Creating the Surface Mesh";
    PipelineMessage em(getNameOfClass(), ss.str(), -1);
    addErrorMessage(em);
    return;
  }

  notifyStatusMessage("Complete");
}


// Functions...

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int M3CEntireVolume::createMesh()
{

  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();
  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();

  int NSP;
  int nFEdge; // number of edges on the square...
  int nTriangle; // number of triangles...
  int nNodes; // number of total nodes used...
  int tnIEdge; // number of endges inside marching square...

  float res[3];
  m->getResolution(res);
  int64_t totalPoints = m->getTotalPoints();
  size_t dims[3] =
  { 0, 0, 0 };
  m->getDimensions(dims);

  size_t fileDim[3] = {dims[0], dims[1], dims[2]};
  size_t posDim[3] = {fileDim[0] + 1,fileDim[1] + 1, fileDim[2] + 1};


  if(m_AddSurfaceLayer)
  {
    fileDim[0] = dims[0] + 2;
    fileDim[1] = dims[1] + 2;
    fileDim[2] = dims[2] + 2;

    posDim[0] = fileDim[0] + 1;
    posDim[1] = fileDim[1] + 1;
    posDim[2] = fileDim[2] + 1;
  }
  totalPoints = fileDim[0] * fileDim[1] * fileDim[2];

  //Copy the data from the m_GrainIds Array into the "Voxel" Struct
  DataArray<int32_t>::Pointer point = DataArray<int32_t>::CreateArray(totalPoints+1, DREAM3D::CellData::SurfaceMeshVoxels);
  point->initializeWithZeros();

  StructArray<VoxelCoord>::Pointer voxelCoords = StructArray<VoxelCoord>::CreateArray(totalPoints + 1, DREAM3D::CellData::SurfaceMeshVoxelCoords);
  voxelCoords->initializeWithZeros();
  VoxelCoord* voxCoords = voxelCoords.get()->GetPointer(0);

  // Get the Max Grain ID so that we can reset it when the meshing is complete
  int32_t* m_GrainIds = getGrainIds();
  int maxGrainId = initialize_micro_from_grainIds(dims, res, fileDim, m_GrainIds, point, voxCoords);


  int NS = fileDim[0] * fileDim[1] * fileDim[2];
  NSP = fileDim[0] * fileDim[1] ;

  StructArray<Neighbor>::Pointer neighbors = StructArray<Neighbor>::CreateArray(NS + 1, DREAM3D::CellData::SurfaceMeshNeighbors);
  neighbors->initializeWithZeros();
  StructArray<Face>::Pointer squares = StructArray<Face>::CreateArray(3*NS, DREAM3D::CellData::SurfaceMeshFaces);
  squares->initializeWithZeros();
  StructArray<Node>::Pointer nodesPtr = StructArray<Node>::CreateArray(7*NS, DREAM3D::CellData::SurfaceMeshNodes);
  nodesPtr->initializeWithZeros();

  DataArray<int8_t>::Pointer nodeKindPtr = DataArray<int8_t>::CreateArray(7*NS, DREAM3D::CellData::SurfaceMeshNodeType);
  nodeKindPtr->initializeWithValues(0);
  setSurfaceMeshNodeType(nodeKindPtr->GetPointer(0)); // Push the point to the superclass

  Neighbor* neigh = neighbors.get()->GetPointer(0);
  Face* square = squares.get()->GetPointer(0);
  Node* vertex = nodesPtr.get()->GetPointer(0);


  notifyStatusMessage("Finding neighbors for each site...");
  get_neighbor_list(neigh, NS, NSP, fileDim[0], fileDim[1], fileDim[2]);

  //printf("\nReading edge and neighbor spin tables...\n");
  //read_edge_neighspin_table(edgeTable_2d, nsTable_2d);

  notifyStatusMessage("\nInitializing all possible nodes...");
  initialize_nodes(voxCoords, vertex, NS, res[0], res[1], res[2]);

  notifyStatusMessage("\nInitializing all possible squares...");
  initialize_squares(neigh, square, NS, NSP);

  notifyStatusMessage("\nCounting number of total edges turned on...\n");
  nFEdge = get_number_fEdges(square, point, neigh, NS);
  ss.str(""); ss << "total number of face edges = " << nFEdge;
  notifyStatusMessage(ss.str());

  // memory allocation for face edges...
//  fedge = (segment *)malloc(nFEdge * sizeof(segment));
  StructArray<Segment>::Pointer faceEdges = StructArray<Segment>::CreateArray(nFEdge, DREAM3D::CellData::SurfaceMeshEdges);
  faceEdges->initializeWithZeros();
  Segment* fedge = faceEdges.get()->GetPointer(0);

  notifyStatusMessage("Finding nodes and edges on each square...");
  get_nodes_fEdges(square, point, neigh, vertex, fedge, NS, NSP, fileDim[0]);

  notifyStatusMessage("\nCounting number of triangles...");
  nTriangle = get_number_triangles(point, square, vertex, fedge, NS, NSP, fileDim[0]);
  ss.str("");
  ss << "\ttotal number of triangles = " << nTriangle;
  notifyStatusMessage(ss.str());


  // memory allocation for triangle...
//  triangle = (patch *)malloc(nTriangle * sizeof(patch));
  StructArray<Triangle>::Pointer triangles = StructArray<Triangle>::CreateArray(nTriangle, DREAM3D::CellData::SurfaceMeshTriangles);
  triangles->initializeWithZeros();
  sm->setTriangles(triangles);
  Triangle* triangle = triangles.get()->GetPointer(0);

  notifyStatusMessage("\nFinding triangles...");
  get_triangles(voxCoords, triangle, square, vertex, fedge, neigh, NS, NSP, fileDim[0]);

  notifyStatusMessage("\nupdating triagle sides as face edges...\n");
  update_triangle_sides_with_fedge(triangle, fedge, square, nTriangle, fileDim[0], NSP);

  notifyStatusMessage("\nCounting the number of inner edges including duplicates...\n");
  tnIEdge = get_number_unique_inner_edges(triangle, nTriangle);
  //printf("\ttotal number of unique inner edges = %d\n", tnIEdge);
  // memory allocation for inner edges...
//  iedge = (isegment *)malloc(tnIEdge * sizeof(isegment));
  StructArray<ISegment>::Pointer internalEdges = StructArray<ISegment>::CreateArray(tnIEdge, DREAM3D::CellData::SurfaceMeshInternalEdges);
  internalEdges->initializeWithZeros();
  ISegment* iedge = internalEdges.get()->GetPointer(0);

  notifyStatusMessage("\nFinidng unique inner edges and updating triagle sides as inner edges...\n");
  get_unique_inner_edges(triangle, iedge, nTriangle, nFEdge);

  notifyStatusMessage("\nupdating node and edge kinds...\n");
  update_node_edge_kind(vertex, fedge, iedge, triangle, nTriangle, nFEdge);

  notifyStatusMessage("\nArranging neighboring spins across the triangle patches...\n");
  arrange_spins(point, voxCoords, triangle, vertex, nTriangle, fileDim[0], NSP);

  notifyStatusMessage("\nAssigning new node IDs...\n");
  DataArray<int32_t>::Pointer new_ids_for_nodes = DataArray<int32_t>::CreateArray(7*NS, 1, "NewIds_For_Nodes");
  new_ids_for_nodes->initializeWithValues(-1);

  nNodes = assign_new_nodeID(vertex, new_ids_for_nodes, NS);
  ss.str("");
  ss << "number of nodes used = " << nNodes;
  notifyStatusMessage(ss.str());

  // Create new shortend arrays for the Triangles and the Nodes and NodeKind
  StructArray<Node>::Pointer nodes = StructArray<Node>::CreateArray(nNodes, DREAM3D::CellData::SurfaceMeshNodes);
  nodes->initializeWithZeros();
  DataArray<int8_t>::Pointer shortNodeKindPtr = DataArray<int8_t>::CreateArray(nNodes, DREAM3D::CellData::SurfaceMeshNodeType);


  generate_update_nodes_edges_array(new_ids_for_nodes, shortNodeKindPtr, nodes, nodesPtr, triangles, faceEdges, internalEdges, maxGrainId);

  // Set the updated Nodes & Triangles into the SurfaceMeshDataContainer
  sm->setTriangles(triangles);
  sm->setNodes(nodes);
  sm->addCellData(DREAM3D::CellData::SurfaceMeshEdges, faceEdges);
  sm->addCellData(DREAM3D::CellData::SurfaceMeshInternalEdges, internalEdges);
  sm->addCellData(DREAM3D::CellData::SurfaceMeshNodeType, shortNodeKindPtr);

//  notifyStatusMessage("\nOutputting nodes and triangles...\n");
//  get_output(vertex, fedge, iedge, triangle, NS, nNodes, nFEdge, tnIEdge, nTriangle, mp);


  return 0;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int M3CEntireVolume::initialize_micro_from_grainIds(size_t dims[3], float res[3], size_t fileDim[3],
                                                             int32_t* grainIds,
                                                             DataArray<int32_t>::Pointer points,
                                                             VoxelCoord* point)
{
  int maxGrainId = 0;
  int32_t* p = points->GetPointer(0);


  if(m_AddSurfaceLayer == false)
  {
    size_t totalPoints = dims[0] * dims[1] * dims[2];
    for (size_t i = 0; i < totalPoints; ++i)
    {
      p[i + 1] = grainIds[i];
      if (p[i+1] > maxGrainId) { maxGrainId = p[i+1]; }
    }
  }
  else
  {
    size_t index = 0;
    size_t gIdx = 0;

    // Add bottom wrapping slice of voxels
    for (size_t i = 0; i < (fileDim[0] * fileDim[1]); ++i)
    {
      p[++index] = -3;
    }
    // Copy the bulk of the volume over
    for (size_t z = 0; z < dims[2]; ++z)
    {
      // Add a leading surface Row for this plane if needed
      for (size_t i = 0; i < fileDim[0]; ++i)
      {
        p[++index] = -4;
      }

      for (size_t y = 0; y < dims[1]; ++y)
      {
        // write leading surface voxel for this row
        p[++index] = -5;

        // Write the actual voxel data
        for (size_t x = 0; x < dims[0]; ++x)
        {
          p[++index] = grainIds[gIdx++];
          if (p[index] > maxGrainId) { maxGrainId = p[index]; }
        }
        // write trailing surface voxel for this row
        p[++index] = -6;
      }
      // Add a trailing surface Row for this plane if needed
      for (size_t i = 0; i < fileDim[0]; ++i)
      {
        p[++index] = -7;
      }
    }

    for (size_t i = 0; i < (fileDim[0] * fileDim[1]); ++i)
    {
      p[++index] = -8;
    }
  }

  // Increment MaxGrainId by one so we have an independent grain
  maxGrainId = maxGrainId + 1;

  // Point 0 is a garbage...
  points->SetValue(0, 0);

  float tx, ty, tz;
  size_t id = 0;
  // Let's fill out the coordinate of each voxel. Remember that x coordinate goes fastest...
  for (size_t k = 0; k < fileDim[2]; k++)
  {
    for (size_t j = 0; j < fileDim[1]; j++)
    {
      for (size_t i = 0; i < fileDim[0]; i++)
      {
        id = (k * fileDim[0] * fileDim[1]) + (j * fileDim[0]) + (i + 1);
        tx = (float)(i) * res[0];
        ty = (float)(j) * res[1];
        tz = (float)(k) * res[2];
        //printf("%10d %6.3f %6.3f %6.3f\n", id, tx, ty, tz);
        point[id].coord[0] = tx;
        point[id].coord[1] = ty;
        point[id].coord[2] = tz;
        // Adjust all GrainIds=0 to maxGrainId;
        if (p[id] == 0) { p[id] = maxGrainId; }
      }
    }
  }
  return maxGrainId;
}
