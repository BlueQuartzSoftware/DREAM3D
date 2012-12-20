/* ============================================================================
 * Copyright (c)  2010 Dr.Sukbin Lee slee@andrew.cmu.edu
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
 * Neither the name of Dr.Sukbin Lee, Carnegie-Mellon University nor the names of
 * its contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ============================================================================
 * Copyright (c) 2009, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2009, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "M3CSliceBySlice.h"


// Include this FIRST because there is a needed define for some compiles
// to expose some of the constants needed below



// C Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-- C++ STL
#include <vector>
#include <map>
#include <sstream>
#include <queue>
#include <vector>
#include <map>


#include "MXA/Common/LogTime.h"
#include "MXA/Common/MXAEndian.h"
#include "MXA/Utilities/MXADir.h"
#include "MXA/Utilities/MXAFileInfo.h"
#include "MXA/Utilities/StringUtils.h"


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/PipelineMessage.h"


#define WRITE_BINARY_TEMP_FILES 1

namespace Detail
{

  static int triangleResizeCount = 0;
  static size_t triangleResize = 1000;

  const std::string NodesFile("Nodes.bin");
  const std::string TrianglesFile("Triangles.bin");

    int edgeTable_2d[20][8] =
  {
  { -1, -1, -1, -1, -1, -1, -1, -1 },
  { -1, -1, -1, -1, -1, -1, -1, -1 },
  { -1, -1, -1, -1, -1, -1, -1, -1 },
  { 0, 1, -1, -1, -1, -1, -1, -1 },
  { -1, -1, -1, -1, -1, -1, -1, -1 },
  { 0, 2, -1, -1, -1, -1, -1, -1 },
  { 1, 2, -1, -1, -1, -1, -1, -1 },
  { 0, 4, 2, 4, 1, 4, -1, -1 },
  { -1, -1, -1, -1, -1, -1, -1, -1 },
  { 3, 0, -1, -1, -1, -1, -1, -1 },
  { 3, 1, -1, -1, -1, -1, -1, -1 },
  { 3, 4, 0, 4, 1, 4, -1, -1 },
  { 2, 3, -1, -1, -1, -1, -1, -1 },
  { 3, 4, 0, 4, 2, 4, -1, -1 },
  { 3, 4, 1, 4, 2, 4, -1, -1 },
  { 3, 0, 1, 2, -1, -1, -1, -1 },
  { 0, 1, 2, 3, -1, -1, -1, -1 },
  { 0, 1, 2, 3, -1, -1, -1, -1 },
  { 3, 0, 1, 2, -1, -1, -1, -1 },
  { 3, 4, 1, 4, 0, 4, 2, 4 } };

  int nsTable_2d[20][8] =
  {
  { -1, -1, -1, -1, -1, -1, -1, -1 },
  { -1, -1, -1, -1, -1, -1, -1, -1 },
  { -1, -1, -1, -1, -1, -1, -1, -1 },
  { 1, 0, -1, -1, -1, -1, -1, -1 },
  { -1, -1, -1, -1, -1, -1, -1, -1 },
  { 1, 0, -1, -1, -1, -1, -1, -1, },
  { 2, 1, -1, -1, -1, -1, -1, -1 },
  { 1, 0, 3, 2, 2, 1, -1, -1 },
  { -1, -1, -1, -1, -1, -1, -1, -1 },
  { 0, 3, -1, -1, -1, -1, -1, -1 },
  { 0, 3, -1, -1, -1, -1, -1, -1 },
  { 0, 3, 1, 0, 2, 1, -1, -1 },
  { 3, 2, -1, -1, -1, -1, -1, -1 },
  { 0, 3, 1, 0, 3, 2, -1, -1 },
  { 0, 3, 2, 1, 3, 2, -1, -1 },
  { 0, 3, 2, 1, -1, -1, -1, -1 },
  { 1, 0, 3, 2, -1, -1, -1, -1 },
  { 1, 0, 3, 2, -1, -1, -1, -1 },
  { 0, 3, 2, 1, -1, -1, -1, -1 },
  { 0, 3, 2, 1, 1, 0, 3, 2 } };



}

/**
 * @brief This class is a wrapper for an absolute path to a temp file or any
 * file that you want to be automatically deleted when the variable goes out
 * of scope.
 * @author Mike Jackson for BlueQuartz Software
 */
class SMTempFile
{
  public:
    DREAM3D_SHARED_POINTERS(SMTempFile)
    DREAM3D_STATIC_NEW_MACRO(SMTempFile)
    virtual ~SMTempFile()
    {
      if (m_AutoDelete == true) {
      MXADir::remove(m_FilePath); }
    }

    DREAM3D_INSTANCE_STRING_PROPERTY(FilePath)
    DREAM3D_INSTANCE_PROPERTY(bool, AutoDelete)

  protected:
    SMTempFile()
    {}
  private:
    SMTempFile(const SMTempFile&); // Copy Constructor Not Implemented
    void operator=(const SMTempFile&); // Operator '=' Not Implemented
};



class GrainChecker
{
  public:
    DREAM3D_SHARED_POINTERS(GrainChecker)
    DREAM3D_STATIC_NEW_MACRO(GrainChecker)
    virtual ~GrainChecker(){}
    typedef std::map<int, int>  MapType;

    template<typename T>
    static void TripletSort(T a, T b, T c, T* sorted)
    {
      if ( a > b && a > c)
      {
        sorted[2] = a;
        if (b > c) { sorted[1] = b; sorted[0] = c; }
        else { sorted[1] = c; sorted[0] = b; }
      }
      else if ( b > a && b > c)
      {
        sorted[2] = b;
        if (a > c) { sorted[1] = a; sorted[0] = c; }
        else { sorted[1] = c; sorted[0] = a; }
      }
      else if ( a > b )
      {
        sorted[1] = a; sorted[0] = b; sorted[2] = c;
      }
      else if (a >= c && b >=c)
      {
        sorted[0] = c; sorted[1] = a; sorted[2] = b;
      }
      else
      { sorted[0] = a; sorted[1] = b; sorted[2] = c;}
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
    void addData(int numTriangles, int ctid, StructArray<Triangle>::Pointer cTriangle, Node* cVertex)
    {
#if 0
      int n1, n2, n3;
      int gid;
      int size;
      for (int i = 0; i < numTriangles; ++i)
      {
        gid = cTriangle[i]->nSpin[0];
        size = static_cast<int>(grainMaps.size());
        if (gid >= size)
        {
          grainMaps.resize(gid + 1);
          tCounts.resize(gid + 1);
        }
        if (gid > 0)
        {
          // Get a reference to the map for this grain
          MapType& vertMap = grainMaps[gid];
          MapType& triMap = tCounts[gid];

          n1 = cTriangle[i].node_id[0];
          n2 = cTriangle[i].node_id[1];
          n3 = cTriangle[i].node_id[2];
          n1 = cVertex[n1].NodeID;
          n2 = cVertex[n2].NodeID;
          n3 = cVertex[n3].NodeID;

          vertMap[n1] = 0;
          vertMap[n2] = 0;
          vertMap[n3] = 0;
          triMap[ctid] = 0;
        }

        gid = cTriangle[i]->nSpin[1];
        size = static_cast<int>(grainMaps.size());
        if (gid >= size)
        {
          grainMaps.resize(gid + 1);
          tCounts.resize(gid + 1);
        }
        if (gid > 0)
        { // Get a reference to the map for this grain
          MapType& vertMap = grainMaps[gid];
          MapType& triMap = tCounts[gid];

          n1 = cTriangle[i].node_id[0];
          n2 = cTriangle[i].node_id[1];
          n3 = cTriangle[i].node_id[2];
          n1 = cVertex[n1].NodeID;
          n2 = cVertex[n2].NodeID;
          n3 = cVertex[n3].NodeID;

          vertMap[n1] = 0;
          vertMap[n2] = 0;
          vertMap[n3] = 0;
          triMap[ctid] = 0;
        }
        //
        ++ctid;
      }
#endif
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
    void analyzeGrains()
    {
      size_t size = tCounts.size();
      size_t vCount = 0;
      size_t tCount = 0;
      for (size_t i = 1; i < size; ++i)
      {
        MapType& t = tCounts[i];
        tCount = t.size();
        MapType& m = grainMaps[i];
        vCount = m.size();
        if (tCount + 4 != vCount * 2)
        {
          std::cout << "Grain ID: " << i << " Does not satisfy equation T=2V-4    " << "  tCount: " << tCount << "   " << "  vCount: " << vCount << std::endl;
        }
      }
    }

  protected:
    GrainChecker() {}

  private:
    std::vector<MapType> grainMaps;
    std::vector<MapType> tCounts;

    GrainChecker(const GrainChecker&); // Copy Constructor Not Implemented
    void operator=(const GrainChecker&); // Operator '=' Not Implemented
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
M3CSliceBySlice::M3CSliceBySlice() :
AbstractFilter(),
m_GrainIdsArrayName(DREAM3D::CellData::GrainIds),
m_SurfaceMeshNodeTypeArrayName(DREAM3D::CellData::SurfaceMeshNodeType),
m_DeleteTempFiles(true),
m_GrainIds(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
M3CSliceBySlice::~M3CSliceBySlice()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CSliceBySlice::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;
  {
    FilterParameter::Pointer option = FilterParameter::New();
    option->setHumanLabel("Delete Temp Files");
    option->setPropertyName("DeleteTempFiles");
    option->setWidgetType(FilterParameter::BooleanWidget);
    option->setValueType("bool");
    parameters.push_back(option);
  }

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CSliceBySlice::writeFilterParameters(AbstractFilterParametersWriter* writer)
{
  writer->writeValue("DeleteTempFiles", getDeleteTempFiles() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CSliceBySlice::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, GrainIds, ss, -300, int32_t, Int32ArrayType, voxels, 1);

  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  if (NULL == sm)
  {
    addErrorMessage(getHumanLabel(), "SurfaceMeshDataContainer is missing", -383);
    setErrorCondition(-384);
  }
  else {
    StructArray<Node>::Pointer vertices = StructArray<Node>::CreateArray(1, DREAM3D::CellData::SurfaceMeshNodes);
    StructArray<Triangle>::Pointer triangles = StructArray<Triangle>::CreateArray(1, DREAM3D::CellData::SurfaceMeshTriangles);


    int8_t* m_SurfaceMeshNodeType;
    CREATE_NON_PREREQ_DATA(sm, DREAM3D, CellData, SurfaceMeshNodeType, ss, int8_t, Int8ArrayType, 0, 1, 1)

    sm->setNodes(vertices);
    sm->setTriangles(triangles);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CSliceBySlice::preflight()
{
  dataCheck(true, 1, 1, 1);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CSliceBySlice::execute()
{
  setErrorCondition(0);
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }

  int64_t totalPoints = m->getTotalPoints();
  dataCheck(false, totalPoints, m->getNumFieldTuples(), m->getNumEnsembleTuples());
  if (getErrorCondition() < 0)
  {
    return;
  }

  int err = 0;
  std::stringstream ss;


  std::string nodesFile = MXADir::tempPath() + Detail::NodesFile;
  SMTempFile::Pointer nodesTempFile = SMTempFile::New();
  nodesTempFile->setFilePath(nodesFile);
  nodesTempFile->setAutoDelete(this->m_DeleteTempFiles);

  std::string trianglesFile = MXADir::tempPath() + Detail::TrianglesFile;
  SMTempFile::Pointer trianglesTempFile = SMTempFile::New();
  trianglesTempFile->setFilePath(trianglesFile);
  trianglesTempFile->setAutoDelete(this->m_DeleteTempFiles);


  int cNodeID = 0;
  int cTriID = 0;
  int cEdgeID = 0;
  int nTriangle = 0; // number of triangles...
  int nEdge = 0; // number of edges...
  int nNodes = 0; // number of total Nodes used...


  GrainChecker::Pointer m_GrainChecker = GrainChecker::New();

  size_t dims[3];
  float res[3];
  float origin[3];

  m->getDimensions(dims);
  m->getResolution(res);
  m->getOrigin(origin);

  int wrappedDims[3] = { dims[0], dims[1], dims[2]};

  // Check to see if there is already a layer of bounding negative grain ids around the volume
  bool isWrapped = volumeHasGhostLayer();
  // If the volume is NOT wrapped by a ghost layer of negative voxels then we need to wrap
  // volume in a ghost layer of voxels.
  if (isWrapped == false)
  {
    wrappedDims[0] += 2;
    wrappedDims[1] += 2;
    wrappedDims[2] += 2;
  }


  int NS = wrappedDims[0] * wrappedDims[1] * wrappedDims[2];
  int NSP = wrappedDims[0] * wrappedDims[1];

  DataArray<int32_t>::Pointer voxelsPtr = DataArray<int32_t>::CreateArray( 2 * NSP + 1, 1, "M3CSliceBySlice_Working_Voxels");
  voxelsPtr->initializeWithValues(-3);
  int32_t* voxels = voxelsPtr->GetPointer(0);

  StructArray<Neighbor>::Pointer neighborsPtr = StructArray<Neighbor>::CreateArray(2*NSP+1, "M3CSliceBySlice_Neighbor_Array");
  neighborsPtr->initializeWithZeros();

  DataArray<int32_t>::Pointer neighCSiteIdPtr = DataArray<int32_t>::CreateArray(2*NSP+1, "M3CSliceBySlice_Neighbor_CSiteId_Array");
  neighCSiteIdPtr->initializeWithZeros();

  StructArray<Face>::Pointer cSquarePtr = StructArray<Face>::CreateArray(3*2*NSP, "M3CSliceBySlice_Face_Array");
  cSquarePtr->initializeWithZeros();

  StructArray<Node>::Pointer cVertexPtr = StructArray<Node>::CreateArray(2*7*NSP, "M3CSliceBySlice_Node_Array");
  cVertexPtr->initializeWithZeros();

  DataArray<int32_t>::Pointer cVertexNodeIdPtr = DataArray<int32_t>::CreateArray(2*7*NSP, "M3CSliceBySlice_Node_NodeId_Array");
  cVertexNodeIdPtr->initializeWithZeros();

  DataArray<int8_t>::Pointer cVertexNodeTypePtr = DataArray<int8_t>::CreateArray(2*7*NSP, "M3CSliceBySlice_Node_NodeKind_Array");
  cVertexNodeTypePtr->initializeWithZeros();

  StructArray<Patch>::Pointer  cTrianglePtr = StructArray<Patch>::CreateArray(0, "M3CSliceBySlice_Triangle_Array");
  cTrianglePtr->initializeWithZeros();

  StructArray<Segment>::Pointer cEdgePtr = StructArray<Segment>::CreateArray(0, "M3CSliceBySlice_Segment_Array");
  cEdgePtr->initializeWithZeros();

  // Prime the working voxels (2 layers worth) with -3 values indicating border voxels if the
  // volume does NOT have a ghost layer
  if(isWrapped == false)
  {
    int size = 2 * NSP + 1;
    for (int i = 0; i < size; ++i)
    {
      voxels[i] = -3;
    }
  }
//  std::map<int, SMStlWriter::Pointer> gidToSTLWriter;

  // Loop over all the Z Slices. An Optimization for memory would be to loop over
  // a different plane say the XZ in case that plane is smaller in dimensions than
  // the XY plane, ie, the volume is rectangular
  size_t sliceCount = dims[2];
  if (isWrapped == false)
  {
    sliceCount = dims[2] + 1;
  }
  for (size_t i = 0; i < sliceCount; i++)
  {
    ss.str("");
    ss << " Layers " << i << " and " << i + 1 << " of " << sliceCount;
    notifyProgressValue((i * 90 / sliceCount));
    notifyStatusMessage(ss.str());

    if (getCancel() == true)
    {
        ss.str("");
        ss << "Cancelling filter";
        notifyWarningMessage(ss.str(), -1);
        setErrorCondition(-1);
        break;
    }

    // Copy the Voxels from layer 2 to Layer 1;
    ::memcpy(&(voxels[1]), &(voxels[1 + NSP]), NSP * sizeof(int));

    // Either interleave the voxels of just a straight copy depending if a ghost
    // layer was already present
    if (isWrapped == true)
    {
      // Get a pointer into the GrainIds Array at the appropriate offset
      int32_t* fileVoxelLayer = m_GrainIds + (i * dims[0] * dims[1]);
      // Copy the grain id values into the 2nd slice layer of the working voxels.
      ::memcpy( &(voxels[1 + NSP]), fileVoxelLayer, NSP * sizeof(int));
      for(int ii = 0; ii < 2 * NSP + 1; ++ii) { if (voxels[ii] < 0) { voxels[ii] = -3;} } // Ensure all ghost cells are -3
    }
    else
    {
      copyBulkSliceIntoWorkingArray(i, wrappedDims, dims, voxels);
    }

    // If we are on the last slice then we need both layers to be ghost cells with
    // negative grain ids but ONLY if the voxel volume was NOT originally wrapped in
    // ghost cells
    if (i == dims[2] && isWrapped == false)
    {
        for (int i = NSP; i < 2 * NSP + 1; ++i)
        {
          voxels[i] = -3;
        }
    }

    // This starts the actual M3C Algorithm codes
    get_neighbor_list(NSP, NS, wrappedDims, neighborsPtr, neighCSiteIdPtr);
    initialize_nodes(NSP, i, wrappedDims, res, cVertexPtr, voxelsPtr, cVertexNodeIdPtr, cVertexNodeTypePtr);
    initialize_squares(i, NSP, cSquarePtr, neighborsPtr);

    // find Face edges of each square of marching cubes in each layer...
    nEdge = get_nodes_Edges(NSP, i, wrappedDims, cSquarePtr, voxelsPtr, cEdgePtr, cVertexNodeTypePtr, neighborsPtr);

    // find triangles and arrange the spins across each triangle...
    nTriangle = get_triangles(NSP, wrappedDims, cSquarePtr, voxelsPtr, cVertexNodeTypePtr, cEdgePtr, cTrianglePtr);
    arrange_grainnames(nTriangle, i, NSP, wrappedDims, res, cTrianglePtr, cVertexPtr, voxelsPtr, neighborsPtr);

    // assign new, cumulative Node id...
    nNodes = assign_nodeID(cNodeID, NSP, cVertexNodeIdPtr, cVertexNodeTypePtr);
    update_node_edge_kind(nTriangle,cTrianglePtr, cVertexNodeTypePtr, cEdgePtr);

    //std::cout << "M3CSliceBySlice nNodes: " << nNodes << std::endl;

//    analyzeWinding();
//    eMap.clear();
//    labelTriangleMap.clear();

    // std::cout << "nNodes: " << nNodes << std::endl;
    // Output Nodes and triangles...
    err = writeNodesFile(i, cNodeID, NSP, nodesFile, cVertexPtr, cVertexNodeIdPtr, cVertexNodeTypePtr);
    if (err < 0)
    {
        ss.str("");
        ss << "Error writing Nodes file '" << nodesFile << "'";
        notifyErrorMessage(ss.str(), -1);
        setErrorCondition(-1);
        return;
    }

    err = writeTrianglesFile(i, cTriID, trianglesFile, nTriangle,cTrianglePtr,cVertexNodeIdPtr);
    if (err < 0)
    {
        ss.str("");
        ss << "Error writing triangles file '" << trianglesFile << "'";
        notifyErrorMessage(ss.str(), -1);
        setErrorCondition(-1);
        return;
    }

//    if (m_WriteSTLFile == true)
//    {
//      m_GrainChecker->addData(nTriangle, cTriID, cTriangle, cVertex);
//      writeSTLFiles(nTriangle, gidToSTLWriter);
//    }
    cNodeID = nNodes;
    cTriID = cTriID + nTriangle;
    cEdgeID = cEdgeID + nEdge;
    if (nTriangle > 0)
    {
      cTrianglePtr->Resize(0);
    }
  }

  // Clear out all the memory that we have used:
  voxelsPtr = DataArray<int32_t>::NullPointer();
  neighborsPtr = StructArray<Neighbor>::NullPointer();
  neighCSiteIdPtr = DataArray<int32_t>::NullPointer();
  cSquarePtr = StructArray<Face>::NullPointer();
  cVertexPtr = StructArray<Node>::NullPointer();
  cVertexNodeIdPtr = DataArray<int32_t>::NullPointer();
  cVertexNodeTypePtr = DataArray<int8_t>::NullPointer();
  cTrianglePtr = StructArray<Patch>::NullPointer();
  cEdgePtr = StructArray<Segment>::NullPointer();

  readNodesTriangles(nodesFile, trianglesFile);

  // This will possibly delete the triangles and Nodes file depending on the
  // DeleteTempFiles setting
  trianglesTempFile = SMTempFile::NullPointer();
  nodesTempFile = SMTempFile::NullPointer();


  setErrorCondition(0);
  notifyStatusMessage("Surface Meshing Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool M3CSliceBySlice::volumeHasGhostLayer()
{
  size_t fileDim[3];
  getVoxelDataContainer()->getDimensions(fileDim);
  size_t index = 0;
  int32_t* p = m_GrainIds;
  bool p_value = false;

  for (size_t z = 0; z < fileDim[2]; ++z)
  {
    for (size_t y = 0; y < fileDim[1]; ++y)
    {
      for (size_t x = 0; x < fileDim[0]; ++x)
      {
        index = (fileDim[0] * fileDim[1] * z) + (fileDim[0] * y) + x;
        p_value = p[index] >= 0;
             if (z == 0 && p_value) { return false;}
        else if (z == fileDim[2]-1 && p_value) { return false;}
        else if (y == 0 && p_value) { return false;}
        else if (y == fileDim[1]-1 && p_value) { return false;}
        else if (x == 0 && p_value) { return false;}
        else if (x == fileDim[0]-1 && p_value) { return false;}
      }
    }
  }
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CSliceBySlice::copyBulkSliceIntoWorkingArray(int i, int* wrappedDims,
                                                    size_t* dims, int32_t* voxels)
{
  std::stringstream ss;
  int NSP = wrappedDims[0] * wrappedDims[1];
  size_t offset = 0;

  // Get a pointer into the GrainIds Array at the appropriate offset
  int32_t* fileVoxelLayer = m_GrainIds + (i * dims[0] * dims[1]);



  // now splice the data into the 2nd z layer for our marching cubes remembering
  // that we have a layer of border voxels.
  int* vxPtr = voxels;
  vxPtr = voxels + 1; // Pointer to start of layer 2 - Should be 4 bytes farther in memory because of junk voxel at voxel[0]
  int* fVxPtr = fileVoxelLayer; // Pointer to actual Grain Ids
  for (size_t y = 0; y < dims[1]; ++y)
  {
    // Get the offset into the data just read from the file
    fVxPtr = fileVoxelLayer + (y * dims[0]);
    // Get the offset into the second layer remembering the border voxel and
    // the fact that we do not use voxel[0] for anything.
    offset = ((y + 1) * wrappedDims[0]) + 1 + (NSP + 1);
    // Use a straight memory copy to move the values from the temp array into the
    // array used for the meshing
    vxPtr = voxels + offset;

    ::memcpy((void*)vxPtr, (void*)fVxPtr, dims[0] * sizeof(int));
  }
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
void M3CSliceBySlice::readNodesTriangles(const std::string &nodesFileName, const std::string &trianglesFileName)
{
  //int err = 0;
  std::stringstream s;
  // Open the Nodes file for reading
  FILE* nodesFile = fopen(nodesFileName.c_str(), "rb+");
  if(nodesFile == NULL)
  {
    s.str("");
    s << "Error opening nodes file '" << nodesFileName << "'";
    setErrorCondition(-1);
    PipelineMessage em (getHumanLabel(), s.str(), -1);
    addErrorMessage(em);
    notifyMessage(em);
    return;
  }
  ScopedFileMonitor nodesMonitor(nodesFile);

  // Calculate how many nodes are in the file based on the file size
  fseek(nodesFile, 0, SEEK_END);
  size_t fLength = ftell(nodesFile);
  size_t nNodes = fLength / SurfaceMesh::NodesFile::ByteCount;
  fseek(nodesFile, 0, SEEK_SET);
  fLength = ftell(nodesFile);
  if(0 != fLength)
  {
    s.str("");
    s << getNameOfClass() << ": Error Could not rewind to beginning of file after nodes count.'" << nodesFileName << "'";
    setErrorCondition(-1);
    PipelineMessage em (getHumanLabel(), s.str(), -1);
    addErrorMessage(em);
    notifyMessage(em);
    return;
  }
  s.str("");
  s << "Calc Node Count from Nodes.bin File: " << nNodes;
  notifyStatusMessage(s.str());

  // Open the triangles file for reading
  FILE* triFile = fopen(trianglesFileName.c_str(), "rb+");
  if(triFile == NULL)
  {
    s.str("");
    s << getNameOfClass() << ": Error opening Triangles file '" << trianglesFileName << "'";
    setErrorCondition(-1);
    PipelineMessage em (getHumanLabel(), s.str(), -1);
    addErrorMessage(em);
    notifyMessage(em);
    return;
  }

  ScopedFileMonitor trianglesMonitor(triFile);
  // Calculate how many Triangles are in the file based in the file size
  fseek(triFile, 0, SEEK_END);
  fLength = ftell(triFile);
  size_t nTriangles = fLength / SurfaceMesh::TrianglesFile::ByteCount;
  fseek(triFile, 0, SEEK_SET);
  fLength = ftell(triFile);
  if(0 != fLength)
  {
    s.str("");
    s << getNameOfClass() << ": Error Could not rewind to beginning of file after triangles count.'" << trianglesFileName << "'";
    setErrorCondition(-1);
    PipelineMessage em (getHumanLabel(), s.str(), -1);
    addErrorMessage(em);
    notifyMessage(em);
    return;
  }
  s.str("");
  s << "Calc Triangle Count from Triangles.bin File: " << nTriangles;
  notifyStatusMessage(s.str());

  StructArray<Node>::Pointer nodesPtr = StructArray<Node>::CreateArray(nNodes, DREAM3D::CellData::SurfaceMeshNodes);
  nodesPtr->initializeWithZeros();
  Node* nodes = nodesPtr->GetPointer(0);
  DataArray<int8_t>::Pointer shortNodeKindPtr = DataArray<int8_t>::CreateArray(nNodes, DREAM3D::CellData::SurfaceMeshNodeType);
  int8_t* nodeKind = shortNodeKindPtr->GetPointer(0);

  s.str("");
  s << "Reading Nodes file into Memory";
  notifyStatusMessage(s.str());
  size_t nread = 0;
  SurfaceMesh::NodesFile::NodesFileRecord_t nRecord;
  for (size_t i = 0; i < nNodes; i++)
  {
    nread = fread(&nRecord, SurfaceMesh::NodesFile::ByteCount, 1, nodesFile); // Read one set of positions from the nodes file
    if(nread != 1)
    {
      break;
    }
    Node& node = nodes[i];
    node.coord[0] = nRecord.x;
    node.coord[1] = nRecord.y;
    node.coord[2] = nRecord.z;
    nodeKind[i] = nRecord.nodeKind;
  }

  SurfaceMeshDataContainer* sm = getSurfaceMeshDataContainer();
  sm->setNodes(nodesPtr);
  sm->addCellData(DREAM3D::CellData::SurfaceMeshNodeType, shortNodeKindPtr);


  StructArray<Triangle>::Pointer trianglesPtr = StructArray<Triangle>::CreateArray(nTriangles, DREAM3D::CellData::SurfaceMeshTriangles);
  trianglesPtr->initializeWithZeros();
  Triangle* triangles = trianglesPtr->GetPointer(0);

  s.str("");
  s << "Reading Triangles file into Memory";
  notifyStatusMessage(s.str());

  SurfaceMesh::TrianglesFile::TrianglesFileRecord_t tRecord;
  for (size_t i = 0; i < nTriangles; i++)
  {
    // Read from the Input Triangles Temp File
    nread = fread(&tRecord, SurfaceMesh::TrianglesFile::ByteCount, 1, triFile);
    if(nread != 1)
    {
      break;
    }
    Triangle& t = triangles[i];
    t.node_id[0] = tRecord.nodeId_0;
    t.node_id[1] = tRecord.nodeId_1;
    t.node_id[2] = tRecord.nodeId_2;
    t.nSpin[0] = tRecord.label_0;
    t.nSpin[1] = tRecord.label_1;
    t.e_id[0] = t.e_id[1] = t.e_id[2] = -1;
    t.edgePlace[0] = t.edgePlace[1] = t.edgePlace[2] = -1;
    t.mCubeID = -1;
    t.tIndex = -1;
  }

  sm->setTriangles(trianglesPtr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CSliceBySlice::get_neighbor_list(int NSP, int NS, int wrappedDims[],
                                        StructArray<Neighbor>::Pointer neighborsPtr,
                                        DataArray<int32_t>::Pointer neighborCSiteIdPtr)
{
  // NSP = number of sites in a plane of xDim by yDim...
  // neigh[][] = 2 dimeNSional array storing its site number and Neighbors...
  // site_id = id number for each site...starting from 1 to xDim*yDim*zDim....
  //
  // I assumed the square lattice...so the order of Neighbors as follows...
  //
  //    4   3   2         13  12  11          22  21  20
  //    5 site  1         14   9  10          23  18  19
  //    6   7   8         15  16  17          24  25  26
  //
  //    in-plane          upper plane         lower plane
  int ii; // loop index...
  int i, j, k, r; // position indices...
  int site_id; // id number for each site...

  Neighbor* neigh = neighborsPtr->GetPointer(0);
  int32_t* csiteid = neighborCSiteIdPtr->GetPointer(0);
  int xDim = wrappedDims[0];


  for (ii = 1; ii <= 2 * NSP; ii++)
  {
    site_id = ii;
    csiteid[ii] = site_id;
    k = (site_id - 1) / NSP;
    k = k * NSP;
    r = (site_id - 1) % NSP;
    j = r / xDim;
    j = j * xDim;
    i = r % xDim + 1;
    //same plane...
    neigh[ii].neigh_id[1] = k + j + i % xDim + 1;
    neigh[ii].neigh_id[2] = k + (j - xDim + NSP) % NSP + i % xDim + 1;
    neigh[ii].neigh_id[3] = k + (j - xDim + NSP) % NSP + i;
    neigh[ii].neigh_id[4] = k + (j - xDim + NSP) % NSP + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[5] = k + j + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[6] = k + (j + xDim) % NSP + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[7] = k + (j + xDim) % NSP + i;
    neigh[ii].neigh_id[8] = k + (j + xDim) % NSP + i % xDim + 1;
    //upper plane...
    //move the plane up and use the same scheme...
    neigh[ii].neigh_id[9] = (k - NSP + NS) % NS + j + i;
    neigh[ii].neigh_id[10] = (k - NSP + NS) % NS + j + i % xDim + 1;
    neigh[ii].neigh_id[11] = (k - NSP + NS) % NS + (j - xDim + NSP) % NSP + i % xDim + 1;
    neigh[ii].neigh_id[12] = (k - NSP + NS) % NS + (j - xDim + NSP) % NSP + i;
    neigh[ii].neigh_id[13] = (k - NSP + NS) % NS + (j - xDim + NSP) % NSP + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[14] = (k - NSP + NS) % NS + j + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[15] = (k - NSP + NS) % NS + (j + xDim) % NSP + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[16] = (k - NSP + NS) % NS + (j + xDim) % NSP + i;
    neigh[ii].neigh_id[17] = (k - NSP + NS) % NS + (j + xDim) % NSP + i % xDim + 1;
    //lower plane...
    neigh[ii].neigh_id[18] = (k + NSP) % NS + j + i;
    neigh[ii].neigh_id[19] = (k + NSP) % NS + j + i % xDim + 1;
    neigh[ii].neigh_id[20] = (k + NSP) % NS + (j - xDim + NSP) % NSP + i % xDim + 1;
    neigh[ii].neigh_id[21] = (k + NSP) % NS + (j - xDim + NSP) % NSP + i;
    neigh[ii].neigh_id[22] = (k + NSP) % NS + (j - xDim + NSP) % NSP + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[23] = (k + NSP) % NS + j + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[24] = (k + NSP) % NS + (j + xDim) % NSP + (i - 2 + xDim) % xDim + 1;
    neigh[ii].neigh_id[25] = (k + NSP) % NS + (j + xDim) % NSP + i;
    neigh[ii].neigh_id[26] = (k + NSP) % NS + (j + xDim) % NSP + i % xDim + 1;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float M3CSliceBySlice::find_xcoord(int index, int xDim, float xRes)
{
  index = index - 1;
  float x = xRes * float(index % xDim);
  return x;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float M3CSliceBySlice::find_ycoord(int index, int xDim, int yDim, float yRes)
{
  index = index - 1;
  float y = yRes * float((index / xDim) % yDim);
  return y;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float M3CSliceBySlice::find_zcoord(int index, int xDim, int yDim, float zRes)
{
  index = index - 1;
  float z = zRes * float(index / (xDim * yDim));
  return z;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CSliceBySlice::initialize_nodes(int NSP, int zID, int* wrappedDims, float* res,
                                      StructArray<Node>::Pointer cVertexPtr,
                                      DataArray<int32_t>::Pointer voxelsPtr,
                                      DataArray<int32_t>::Pointer cVertexNodeIdPtr,
                                      DataArray<int8_t>::Pointer cVertexNodeTypePtr  )
{

  // Finds the coordinates of Nodes...
  int i, j;
  int id, oid;
  int tsite, locale;
  float x, y, z;
  int start = NSP + 1;
  if (zID == 0)
  {
   start = 1;
   numgrains = 0;
   }

  float xRes = res[0];
  float yRes = res[1];
  float zRes = res[2];

  Node* cVertex = cVertexPtr->GetPointer(0);
  int32_t* voxels = voxelsPtr->GetPointer(0);
  int8_t* nodeKind = cVertexNodeTypePtr->GetPointer(0);
  int32_t* nodeID = cVertexNodeIdPtr->GetPointer(0);

  // Node id starts with 0....
  if (zID > 0)
  {
    for (i = 1; i <= NSP; i++)
    {
      id = 7 * (i - 1);
      oid = 7 * ((i + NSP) - 1);
      for (j = 0; j < 7; j++)
      {
        cVertex[id + j] = cVertex[oid + j];
        nodeKind[id + j] = nodeKind[oid + j];
        nodeID[id + j] = nodeID[oid + j];
      }
    }
  }
  int shift = (zID * NSP);
  for (i = start; i <= 2 * NSP; i++)
  {
    id = 7 * (i - 1);
    tsite = i;
    locale = tsite + shift;
    x = find_xcoord(locale, wrappedDims[0], xRes);
    y = find_ycoord(locale, wrappedDims[0], wrappedDims[1], yRes);
    z = find_zcoord(locale, wrappedDims[0], wrappedDims[1], zRes);
    int grainid = voxels[tsite];
    if (grainid > numgrains) numgrains = grainid;
    cVertex[id].coord[0] = x + (0.5f * xRes);
    cVertex[id].coord[1] = y;
    cVertex[id].coord[2] = z;
    nodeKind[id] = 0;
    nodeID[id] = -1;
    cVertex[id + 1].coord[0] = x;
    cVertex[id + 1].coord[1] = y + (0.5f * yRes);
    cVertex[id + 1].coord[2] = z;
    nodeKind[id + 1] = 0;
    nodeID[id + 1] = -1;
    cVertex[id + 2].coord[0] = x;
    cVertex[id + 2].coord[1] = y;
    cVertex[id + 2].coord[2] = z + (0.5f * zRes);
    nodeKind[id + 2] = 0;
    nodeID[id + 2] = -1;
    cVertex[id + 3].coord[0] = x + (0.5f * xRes);
    cVertex[id + 3].coord[1] = y + (0.5f * yRes);
    cVertex[id + 3].coord[2] = z;
    nodeKind[id + 3] = 0;
    nodeID[id + 3] = -1;
    cVertex[id + 4].coord[0] = x + (0.5f * xRes);
    cVertex[id + 4].coord[1] = y;
    cVertex[id + 4].coord[2] = z + (0.5f * zRes);
    nodeKind[id + 4] = 0;
    nodeID[id + 4] = -1;
    cVertex[id + 5].coord[0] = x;
    cVertex[id + 5].coord[1] = y + (0.5f * yRes);
    cVertex[id + 5].coord[2] = z + (0.5f * zRes);
    nodeKind[id + 5] = 0;
    nodeID[id + 5] = -1;
    cVertex[id + 6].coord[0] = x + (0.5f * xRes);
    cVertex[id + 6].coord[1] = y + (0.5f * yRes);
    cVertex[id + 6].coord[2] = z + (0.5f * zRes);
    nodeKind[id + 6] = 0;
    nodeID[id + 6] = -1;
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CSliceBySlice::initialize_squares(int zID, int NSP,
                                          StructArray<Face>::Pointer cSquarePtr,
                                          StructArray<Neighbor>::Pointer neighborsPtr)
{

  // Gather initial information on each square...
  int id;
  int i, j;
  int csite;

  Face* cSquare = cSquarePtr->GetPointer(0);
  Neighbor* neigh = neighborsPtr->GetPointer(0);

  // square id starts with 0....
  // notice that point at the surface will have the wrong values of Node at the other end...
  // since it includes periodic boundary condition...
  // but, since the structure surrounded by ghost layer of grainname -3, it's OK...
  for (i = 1; i <= 2 * NSP; i++)
  {
    id = 3 * (i - 1);
    csite = i;
    // store 4 corners of square...
    cSquare[id].site_id[0] = csite;
    cSquare[id].site_id[1] = neigh[i].neigh_id[1];
    cSquare[id].site_id[2] = neigh[i].neigh_id[8];
    cSquare[id].site_id[3] = neigh[i].neigh_id[7];
    cSquare[id + 1].site_id[0] = csite;
    cSquare[id + 1].site_id[1] = neigh[i].neigh_id[1];
    cSquare[id + 1].site_id[2] = neigh[i].neigh_id[19];
    cSquare[id + 1].site_id[3] = neigh[i].neigh_id[18];
    cSquare[id + 2].site_id[0] = neigh[i].neigh_id[7];
    cSquare[id + 2].site_id[1] = csite;
    cSquare[id + 2].site_id[2] = neigh[i].neigh_id[18];
    cSquare[id + 2].site_id[3] = neigh[i].neigh_id[25];
    // initialize Node, edge...-1 is dummy initial value...
    for (j = 0; j < 4; j++)
    {
      cSquare[id].edge_id[j] = -1;
      cSquare[id + 1].edge_id[j] = -1;
      cSquare[id + 2].edge_id[j] = -1;
    }
    for (int j = 0; j < 3; j++)
    {
      cSquare[id + j].nEdge = 0;
      cSquare[id + j].FCnode = -1;
      cSquare[id + j].effect = 0;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t M3CSliceBySlice::get_nodes_Edges(int NSP, int zID, int* wrappedDims,
                                        StructArray<Face>::Pointer cSquarePtr,
                                        DataArray<int32_t>::Pointer voxelsPtr,
                                        StructArray<Segment>::Pointer cEdgePtr,
                                        DataArray<int8_t>::Pointer cVertexNodeTypePtr,
                                        StructArray<Neighbor>::Pointer neighborsPtr)
{
  int j, k, m, ii;
  int tsite;
  int cubeOrigin; // stores the site id of 0th corner of each cube...
  int sqOrder; // stores the 0th, 1st and 2nd square...
  int tNSite[4];
  int tnSpin[4];
  int atBulk; // if it's 0, the square is at Bulk...
  int quot, rmd, rmd1;
  int sqIndex;
  int anFlag;
  int NodeIndex[2];
  int tnode;
  int tn1, tn2;
  int tnk;
  int pixIndex[2];
  size_t eid = 0; // edge id...
//  int tnode1, tnode2;
  int edgeCount; // number of edges for each square...
  int NodeID[2];
  int pixgrainname[2];

  size_t edgeResize = 1000;
  int    edgeResizeCount = 0;


  Face* cSquare = cSquarePtr->GetPointer(0);
  int32_t* voxels = voxelsPtr->GetPointer(0);
  int8_t* nodeType = cVertexNodeTypePtr->GetPointer(0);

  for (k = 0; k < 3 * 2 * NSP; k++)
  { // for each square...
    quot = k / (3 * NSP);
    rmd = k % (3 * NSP);
    rmd1 = rmd % 3;
    if (quot == 0 || (quot == 1 && rmd1 == 0))
    {
      cubeOrigin = k / 3 + 1;
      sqOrder = k % 3;
   //   csite = cSquare[k].site_id[0];
  //    cgrainname = voxels[csite];
      tNSite[0] = cSquare[k].site_id[0];
      tNSite[1] = cSquare[k].site_id[1];
      tNSite[2] = cSquare[k].site_id[2];
      tNSite[3] = cSquare[k].site_id[3];
      atBulk = 0;
      for (m = 0; m < 4; m++)
      {
        tsite = tNSite[m];
        tnSpin[m] = voxels[tsite];
        if (tnSpin[m] < 0) atBulk++;
      }
      edgeCount = 0;
      // Let's find the edges...
      if (atBulk != 4)
      { // consider the square inside the bulk only...
        cSquare[k].effect = 1;
        sqIndex = get_square_index(tnSpin);
        if (sqIndex == 15)
        {
          anFlag = treat_anomaly(tNSite, zID, voxelsPtr, neighborsPtr);
          sqIndex = sqIndex + anFlag;
        }
        if (sqIndex != 0)
        {
          for (j = 0; j < 8; j = j + 2)
          {
            if (Detail::edgeTable_2d[sqIndex][j] != -1)
            {
              NodeIndex[0] = Detail::edgeTable_2d[sqIndex][j];
              NodeIndex[1] = Detail::edgeTable_2d[sqIndex][j + 1];
              pixIndex[0] = Detail::nsTable_2d[sqIndex][j];
              pixIndex[1] = Detail::nsTable_2d[sqIndex][j + 1];
              get_nodes(cubeOrigin, sqOrder, NodeIndex, NodeID, NSP, wrappedDims);
              get_grainnames(cubeOrigin, sqOrder, pixIndex, pixgrainname, NSP, wrappedDims, voxelsPtr);
              if (pixgrainname[0] > 0 || pixgrainname[1] > 0)
              {
                size_t currentEdgeArraySize = cEdgePtr->GetNumberOfTuples();
                if (currentEdgeArraySize < eid+1)
                {
                  edgeResizeCount++;
                  if (edgeResizeCount == 10) {
                    edgeResizeCount = 0;
                    edgeResize *= 10; // Increment the resize factor by an order of magnitude
                  }
                  cEdgePtr->Resize(currentEdgeArraySize + edgeResize); // Allocate Edges in 100,000 packs
                  currentEdgeArraySize = cEdgePtr->GetNumberOfTuples();
                  //std::cout << "cEdgePtr->Resize(" <<currentEdgeArraySize << ")" << std::endl;
                }

                Segment* cEdge = cEdgePtr->GetPointer(0);
                cEdge[eid].node_id[0] = NodeID[0]; // actual Node ids for each edge...
                cEdge[eid].node_id[1] = NodeID[1];
                cEdge[eid].nSpin[0] = pixgrainname[0];
                cEdge[eid].nSpin[1] = pixgrainname[1];
                cEdge[eid].edgeKind = 2; // edges of the open loops are always binary...
                // triple lines only occurs inside the marching cubes...
                cSquare[k].edge_id[edgeCount] = eid;
                edgeCount++;
                eid++;
              }
              else
              {
                tn1 = NodeID[0];
                tn2 = NodeID[1];
                nodeType[tn1] = DREAM3D::SurfaceMesh::NodeType::Unused; // extra Nodes from meshing the surface of the box...
                nodeType[tn2] = DREAM3D::SurfaceMesh::NodeType::Unused; // we don't need them...
              }
              // Categorize the Node...if it's triple junction or not...
              for (ii = 0; ii < 2; ii++)
              {
                if (NodeIndex[ii] == 4)
                {
                  if (sqIndex == 7 || sqIndex == 11 || sqIndex == 13 || sqIndex == 14)
                  {
                    tnode = NodeID[ii];
                    nodeType[tnode] = DREAM3D::SurfaceMesh::NodeType::TriplePoint;
                    cSquare[k].FCnode = tnode;
                  }
                  else if (sqIndex == 19)
                  {
                    tnode = NodeID[ii];
                    nodeType[tnode] = DREAM3D::SurfaceMesh::NodeType::QuadPoint;
                    cSquare[k].FCnode = tnode;
                  }
                }
                else
                {
                  tnode = NodeID[ii];
                  tnk = nodeType[tnode];
                  if (tnk != -1)
                  {
                    nodeType[tnode] = DREAM3D::SurfaceMesh::NodeType::Default;
                  }
                }
              }
            }
          }
        }
      }
      cSquare[k].nEdge = edgeCount;
    }
  }
  cEdgePtr->Resize(eid);
  return eid;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int M3CSliceBySlice::get_square_index(int tNS[4])
{
  // identify each square configuration using binary bit...
  // returNS the unique decimal integer for each configuration...
  int aBit[6];
  int tempIndex;
  int subIndex;
  if (tNS[0] == tNS[1]) aBit[0] = 0;
  else aBit[0] = 1;
  if (tNS[1] == tNS[2]) aBit[1] = 0;
  else aBit[1] = 1;
  if (tNS[2] == tNS[3]) aBit[2] = 0;
  else aBit[2] = 1;
  if (tNS[3] == tNS[0]) aBit[3] = 0;
  else aBit[3] = 1;
  if (tNS[0] == tNS[2]) aBit[4] = 0;
  else aBit[4] = 1;
  if (tNS[1] == tNS[3]) aBit[5] = 0;
  else aBit[5] = 1;
  tempIndex = 8 * aBit[3] + 4 * aBit[2] + 2 * aBit[1] + 1 * aBit[0];
  if (tempIndex == 15)
  {
    subIndex = 2 * aBit[4] + 1 * aBit[5];
    if (subIndex != 0)
    {
      tempIndex = tempIndex + subIndex + 1;
    }
  }
  return tempIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int M3CSliceBySlice::treat_anomaly(int tNSt[4], int zID1,
                                    DataArray<int32_t>::Pointer voxelsPtr,
                                    StructArray<Neighbor>::Pointer neighborsPtr)
{
  int i, j, k, ii;
  int csite, cgrainname;
  int NSite, nSpin;
  int temp;
  int tempFlag;
  int min, minid;
  int numNeigh[4];
  min = 1000;
  minid = -1;

  int32_t* voxels = voxelsPtr->GetPointer(0);
  Neighbor* neigh = neighborsPtr->GetPointer(0);

  for (k = 0; k < 4; k++)
  {
    numNeigh[k] = 0;
  }
  for (i = 0; i < 4; i++)
  {
    csite = tNSt[i];
    cgrainname = voxels[csite];
//    for (j = 1; j <= num_neigh; j++)
    for (j = 1; j <= 8; j++)
    {
      NSite = neigh[csite].neigh_id[j];
      nSpin = voxels[NSite];
      if (cgrainname == nSpin)
      {
        numNeigh[i] = numNeigh[i] + 1;
      }
    }
  }
  for (ii = 0; ii < 4; ii++)
  {
    temp = numNeigh[ii];
    if (temp < min)
    {
      min = temp;
      minid = ii;
    }
  }
  if (minid == 1 || minid == 3)
  { // we can reverse this...let's try this later...
    tempFlag = 0;
  }
  else
  {
    tempFlag = 1;
  }
  return tempFlag;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CSliceBySlice::get_nodes(int cst, int ord, int nidx[2], int* nid, int NSP, int* wrappedDims)
{
  int ii;
  int tempIndex;
  for (ii = 0; ii < 2; ii++)
  {
    tempIndex = nidx[ii];
    if (ord == 0)
    { // if the square index is 0 for corner site...
      switch(tempIndex)
      {
        case 0:
          nid[ii] = 7 * (cst - 1);
          break;
        case 1:
          nid[ii] = 7 * cst + 1;
          break;
        case 2:
          nid[ii] = 7 * (cst + wrappedDims[0] - 1);
          break;
        case 3:
          nid[ii] = 7 * (cst - 1) + 1;
          break;
        case 4:
          nid[ii] = 7 * (cst - 1) + 3;
          break;
      }
    }
    else if (ord == 1)
    { // if the square index is 1...
      switch(tempIndex)
      {
        case 0:
          nid[ii] = 7 * (cst - 1);
          break;
        case 1:
          nid[ii] = 7 * cst + 2;
          break;
        case 2:
          nid[ii] = 7 * (cst + NSP - 1);
          break;
        case 3:
          nid[ii] = 7 * (cst - 1) + 2;
          break;
        case 4:
          nid[ii] = 7 * (cst - 1) + 4;
          break;
      }
    }
    else
    { // if the square index is 2...
      switch(tempIndex)
      {
        case 0:
          nid[ii] = 7 * (cst - 1) + 1;
          break;
        case 1:
          nid[ii] = 7 * (cst - 1) + 2;
          break;
        case 2:
          nid[ii] = 7 * (cst + NSP - 1) + 1;
          break;
        case 3:
          nid[ii] = 7 * (cst + wrappedDims[0] - 1) + 2;
          break;
        case 4:
          nid[ii] = 7 * (cst - 1) + 5;
          break;
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CSliceBySlice::get_grainnames(int cst, int ord, int pID[2], int* pgrainname,
                                      int NSP, int* wrappedDims,
                                      DataArray<int32_t>::Pointer voxelsPtr)
{
  int i;
  int pixTemp, tempgrainname;
  int32_t* voxels = voxelsPtr->GetPointer(0);
  for (i = 0; i < 2; i++)
  {
    pixTemp = pID[i];
    if (ord == 0)
    {
      switch(pixTemp)
      {
        case 0:
          tempgrainname = voxels[cst];
          pgrainname[i] = tempgrainname;
          break;
        case 1:
          tempgrainname = voxels[cst + 1];
          pgrainname[i] = tempgrainname;
          break;
        case 2:
          tempgrainname = voxels[cst + wrappedDims[0] + 1];
          pgrainname[i] = tempgrainname;
          break;
        case 3:
          tempgrainname = voxels[cst + wrappedDims[0]];
          pgrainname[i] = tempgrainname;
          break;
      }
    }
    else if (ord == 1)
    {
      switch(pixTemp)
      {
        case 0:
          tempgrainname = voxels[cst];
          pgrainname[i] = tempgrainname;
          break;
        case 1:
          tempgrainname = voxels[cst + 1];
          pgrainname[i] = tempgrainname;
          break;
        case 2:
          tempgrainname = voxels[cst + NSP + 1];
          pgrainname[i] = tempgrainname;
          break;
        case 3:
          tempgrainname = voxels[cst + NSP];
          pgrainname[i] = tempgrainname;
          break;
      }
    }
    else if (ord == 2)
    {
      switch(pixTemp)
      {
        case 0:
          tempgrainname = voxels[cst + wrappedDims[0]];
          pgrainname[i] = tempgrainname;
          break;
        case 1:
          tempgrainname = voxels[cst];
          pgrainname[i] = tempgrainname;
          break;
        case 2:
          tempgrainname = voxels[cst + NSP];
          pgrainname[i] = tempgrainname;
          break;
        case 3:
          tempgrainname = voxels[cst + NSP + wrappedDims[0]];
          pgrainname[i] = tempgrainname;
          break;
      }
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int M3CSliceBySlice::get_triangles(int NSP, int* wrappedDims,
                                    StructArray<Face>::Pointer cSquarePtr,
                                    DataArray<int32_t>::Pointer voxelsPtr,
                                    DataArray<int8_t>::Pointer cVertexNodeTypePtr,
                                    StructArray<Segment>::Pointer cEdgePtr,
                                    StructArray<Patch>::Pointer  cTrianglePtr)
{
  int i, ii, i1, i2;
  int sqID[6];
  int tsq; // current sq id...
  int tnE; // temp number of edges...
  int nFC; // number of FC turned on...
  int nE; // number of Face edges...
  int eff; // all the squares effective?...
  int cubeFlag; // if 1, we can do marching cube; if 0, useless...
  int BCnode; // cube center Node...
  int nkFlag;
  int tsqid1, tsqid2;
  int *arrayE;
  int fcid;
  int tindex;
  int nds, nburnt;
  int tsite1, tsite2;
  int tgrainname1, tgrainname2;
  int cgrainname, nSpin;
  int arraygrainname[8];
  int tidIn, tidOut;
  int arrayFC[6];

  Face* cSquare = cSquarePtr->GetPointer(0);
  int32_t* voxels = voxelsPtr->GetPointer(0);
  int8_t* nodeType = cVertexNodeTypePtr->GetPointer(0);
  // Reset these variables each time into this loop.
  Detail::triangleResize = 1000;
  Detail::triangleResizeCount = 0;

  tidIn = 0;
  for (i = 1; i <= NSP; i++)
  {
    cubeFlag = 0;
    // for each marching cube, collect Faces...
    sqID[0] = 3 * (i - 1);
    sqID[1] = 3 * (i - 1) + 1;
    sqID[2] = 3 * (i - 1) + 2;
    sqID[3] = 3 * i + 2;
    sqID[4] = 3 * (i + wrappedDims[0] - 1) + 1;
    sqID[5] = 3 * (i + NSP - 1);
    BCnode = 7 * (i - 1) + 6;
    nFC = 0;
    nE = 0;
    eff = 0;
    //initialize Face center array...
    for (ii = 0; ii < 6; ii++)
    {
      arrayFC[ii] = -1;
    }
    // Count the number of Face center turned on and Face edges...
    fcid = 0;
    ii = 0;
    for (ii = 0; ii < 6; ii++)
    {
      tsq = sqID[ii];
      if (cSquare[tsq].FCnode != -1)
      {
        arrayFC[fcid] = cSquare[tsq].FCnode;
        fcid++;
      }
//      nFC = nFC + 1;
      nE = nE + cSquare[tsq].nEdge;
      eff = eff + cSquare[tsq].effect;
    }
    nFC = fcid;
    if (eff > 0) cubeFlag = 1;
    if (nFC >= 3)
    {
      // If number of Face centers turned on is more than 2...
      // let's update the NodeKind of body center Node...
      tsqid1 = sqID[0];
      tsqid2 = sqID[5];
      nkFlag = 0;
      // get spin numbers for 8 corners for each marching cube...
      for (int j = 0; j < 4; j++)
      {
        tsite1 = cSquare[tsqid1].site_id[j];
        tsite2 = cSquare[tsqid2].site_id[j];
        tgrainname1 = voxels[tsite1];
        tgrainname2 = voxels[tsite2];
        arraygrainname[j] = tgrainname1;
        arraygrainname[j + 4] = tgrainname2;
        if (tgrainname1 < 0 || tgrainname2 < 0) nkFlag++;
      }
      nds = 0; // number of different spins in each marching cube...
      nburnt = 0; // so nds = NodeKind of body center position...
      for (int k = 0; k < 8; k++)
      {
        // arraySpin contains no -1 before any of it is burnt...
        cgrainname = arraygrainname[k];
        if (cgrainname != -1)
        {
          nds++;
          arraygrainname[k] = -1; // burn...
          nburnt++;
          for (int kk = 0; kk < 8; kk++)
          {
            nSpin = arraygrainname[kk];
            if (cgrainname == nSpin)
            {
              arraygrainname[kk] = -1; //burn...
              nburnt++;
            }
          }
        }
      }
      // update NodeKind of body center Node in the current marching cube...
      if (nkFlag > 0)
      {
        nodeType[BCnode] = nds;
      }
      else
      {
        nodeType[BCnode] = nds;
      }
    }
    // Checking the number of edges for loops in the cube...
    // if the current marching cube is a collection of 6 effective squares...and
    // the number of Face edges at least 3...
    // when nE==2, it doen't happen
    // when nE==1, the edge will contribute for the Neighboring marching cube...
    // when nE==0, it meaNS the cube is inside a grain...
    if (cubeFlag == 1 && nE > 2)
    {
      // Make edge array for each marching cube...
      arrayE = new int[nE];
      boost::shared_array<int> arrayEPtr(arrayE);
      tindex = 0;
      for (i1 = 0; i1 < 6; i1++)
      {
        tsq = sqID[i1];
        tnE = cSquare[tsq].nEdge;
        if (tnE != 0)
        {
          for (i2 = 0; i2 < tnE; i2++)
          {
            arrayE[tindex] = cSquare[tsq].edge_id[i2];
            tindex++;
          }
        }
      }
      // Consider each case as Z. Wu's paper...
      if (nFC == 0)
      { // when there's no Face center
        get_case0_triangles(i, arrayE, nE, tidIn, &tidOut, cEdgePtr, cTrianglePtr);
        tidIn = tidOut;
      }
      else if (nFC == 2)
      {
        get_case2_triangles(i, arrayE, nE, arrayFC, nFC, tidIn, &tidOut, cEdgePtr, cTrianglePtr);
        tidIn = tidOut;
      }
      else if (nFC > 2 && nFC <= 6)
      {
        get_caseM_triangles(i, arrayE, nE, arrayFC, nFC, tidIn, &tidOut, BCnode, cEdgePtr, cTrianglePtr);
        tidIn = tidOut;
      }
    }
  }

//  std::cout << "cTrianglePtr->Resize(" << tidIn << ")" << std::endl;
  cTrianglePtr->Resize(tidIn);

//  std::cout << "cEdgePtr numTuples: " << cEdgePtr->GetNumberOfTuples() << " - Clearing Array" << std::endl;
  cEdgePtr->Resize(0);

  return cTrianglePtr->GetNumberOfTuples();
}

#define ADD_TRIANGLE(cTrianglePtr, ctid, n0, n1, n2, label0, label1)\
{\
  size_t current_##cTrianglePtr##_size = cTrianglePtr->GetNumberOfTuples();\
  if (current_##cTrianglePtr##_size < static_cast<size_t>(ctid + 1) ) {\
    Detail::triangleResizeCount++; \
    if (Detail::triangleResizeCount == 10) { \
      Detail::triangleResizeCount = 0;\
      Detail::triangleResize *= 10;\
    }\
    cTrianglePtr->Resize(current_##cTrianglePtr##_size + Detail::triangleResize);\
  }\
}\
StructArray<Triangle>& cTriangle = *(cTrianglePtr.get());\
cTriangle[ctid].node_id[0] = n0;\
cTriangle[ctid].node_id[1] = n1;\
cTriangle[ctid].node_id[2] = n2;\
cTriangle[ctid].nSpin[0] = label0;\
cTriangle[ctid].nSpin[1] = label1;\
cTriangle[ctid].tIndex = ctid;

  #if 0
  SharedEdge::Pointer e0 = SharedEdge::New(cTriangle[ctid].node_id[0], cTriangle[ctid].node_id[1]);\
  SharedEdge::Pointer e = eMap[e0->getId()];\
  if (NULL == e.get()) { eMap[e0->getId()] = e0; }else{ e0 = e; }\
  e0->triangles.insert(ctid);\
  SharedEdge::Pointer e1 = SharedEdge::New(cTriangle[ctid].node_id[1], cTriangle[ctid].node_id[2]);\
  e = eMap[e1->getId()];\
  if (NULL == e.get()) { eMap[e1->getId()] = e1; }else{ e1 = e;}\
  e1->triangles.insert(ctid);\
  SharedEdge::Pointer e2 = SharedEdge::New(cTriangle[ctid].node_id[2], cTriangle[ctid].node_id[0]);\
  e = eMap[e2->getId()]; \
  if (NULL == e.get()){ eMap[e2->getId()] = e2; }else{ e2 = e; }\
  e2->triangles.insert(ctid);\
  cTriangle[ctid]->edges[0] = e0;  cTriangle[ctid]->edges[1] = e1;  cTriangle[ctid]->edges[2] = e2;\
  labelTriangleMap[label0] = ctid;\
  labelTriangleMap[label1] = ctid;
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CSliceBySlice::get_case0_triangles(int site, int *ae, int nedge,
                                          int tin, int* tout,
                                          StructArray<Segment>::Pointer cEdgePtr,
                                          StructArray<Patch>::Pointer cTrianglePtr)
{
  int ii, i, j, jj, k, kk, k1, mm;
  int loopID;
  int tail, head, coin;
  int ce, ne;
  int cgrainname1, cgrainname2, cnode1, cnode2;
  int nSpin1, nSpin2, nnode1, nnode2;
  //int nucleus;
  int chaser;
  int grainnameFlag, NodeFlag, flip;
  int bflag, nbflag;
  int* burnt = NULL;
  int* burnt_list = NULL;
  int* count = NULL;
  int numN, sumN;
  int from;
  int* loop = NULL;
  size_t ctid;
  int front, back;
  int te0, te1, te2, tv0, tcVertex, tv2;
  int numT, cnumT, new_node0;

  typedef boost::shared_array<int> SharedIntArray_t;

  SharedIntArray_t burntPtr(new int[nedge]);
  burnt = burntPtr.get();

  SharedIntArray_t burnt_list_ptr(new int[nedge]);
  burnt_list = burnt_list_ptr.get();

  Segment* cEdge = cEdgePtr->GetPointer(0);

  // initialize burn flags for Face edges...
  for (ii = 0; ii < nedge; ii++)
  {
    burnt[ii] = 0;
    burnt_list[ii] = -1;
  }
  loopID = 1;
  tail = 0;
  head = 0;
  for (i = 0; i < nedge; i++)
  {
    ce = ae[i];
    bflag = burnt[i];
    if (bflag == 0)
    {
    //  nucleus = ce;
      burnt[i] = loopID;
      burnt_list[tail] = ce;
      coin = 1;
      while (coin)
      {
        chaser = burnt_list[tail];
        cgrainname1 = cEdge[chaser].nSpin[0];
        cgrainname2 = cEdge[chaser].nSpin[1];
        cnode1 = cEdge[chaser].node_id[0];
        cnode2 = cEdge[chaser].node_id[1];
        for (j = 0; j < nedge; j++)
        {
          ne = ae[j];
          nbflag = burnt[j];
          if (nbflag == 0)
          {
            nSpin1 = cEdge[ne].nSpin[0];
            nSpin2 = cEdge[ne].nSpin[1];
            nnode1 = cEdge[ne].node_id[0];
            nnode2 = cEdge[ne].node_id[1];
            // checking if Neighbor edge has same Neighboring grainnames...
            if (((cgrainname1 == nSpin1) && (cgrainname2 == nSpin2)) || ((cgrainname1 == nSpin2) && (cgrainname2 == nSpin1)))
            {
              grainnameFlag = 1;
            }
            else
            {
              grainnameFlag = 0;
            }
            // checking if neighor egde is contiguous...
            if ((cnode2 == nnode1) && (cnode1 != nnode2))
            {
              NodeFlag = 1;
              flip = 0;
            }
            else if ((cnode2 == nnode2) && (cnode1 != nnode1))
            {
              NodeFlag = 1;
              flip = 1;
            }
            else
            {
              NodeFlag = 0;
              flip = 0;
            }
            if (grainnameFlag == 1 && NodeFlag == 1)
            {
              head = head + 1;
              burnt_list[head] = ne;
              burnt[j] = loopID;
              if (flip == 1)
              {
                cEdge[ne].nSpin[0] = nSpin2;
                cEdge[ne].nSpin[1] = nSpin1;
                cEdge[ne].node_id[0] = nnode2;
                cEdge[ne].node_id[1] = nnode1;
              }
            }
          }
        }
        if (tail == head)
        {
          coin = 0;
          tail = tail + 1;
          head = tail;
          loopID++;
        }
        else
        {
          tail = tail + 1;
          coin = 1;
        }
      }
    }
  }
  SharedIntArray_t countPtr(new int[loopID]);
  count = countPtr.get();
  for (k1 = 1; k1 < loopID; k1++)
  {
    count[k1] = 0;
  }
  for (k = 1; k < loopID; k++)
  {
    for (kk = 0; kk < nedge; kk++)
    {
      if (k == burnt[kk])
      {
        count[k] = count[k] + 1;
      }
    }
  }
  // Let's do triangulation...
  sumN = 0;
  ctid = tin;
  for (jj = 1; jj < loopID; jj++)
  {
    numN = count[jj];
    sumN = sumN + numN;
    from = sumN - numN;
    SharedIntArray_t loopPtr(new int[numN]);
    loop = loopPtr.get();
    for (mm = 0; mm < numN; mm++)
    {
      loop[mm] = burnt_list[from + mm];
    }
    if (numN == 3)
    {
      te0 = loop[0];
      te1 = loop[1];
      te2 = loop[2];
      ADD_TRIANGLE(cTrianglePtr, ctid, cEdge[te0].node_id[0], cEdge[te1].node_id[0], cEdge[te2].node_id[0], cEdge[te0].nSpin[0], cEdge[te0].nSpin[1] )
      ctid++;
    }
    else if (numN > 3)
    {
      numT = numN - 2;
      cnumT = 0;
      front = 0;
      back = numN - 1;
      te0 = loop[front];
      te1 = loop[back];
      tv0 = cEdge[te0].node_id[0];
      tcVertex = cEdge[te0].node_id[1];
      tv2 = cEdge[te1].node_id[0];
      ADD_TRIANGLE(cTrianglePtr, ctid, tv0, tcVertex, tv2, cEdge[te0].nSpin[0], cEdge[te0].nSpin[1] )

      new_node0 = tv2;
    //  new_node1 = tcVertex;
      cnumT++;
      ctid++;
      while (cnumT < numT)
      {
        if ((cnumT % 2) != 0)
        {
          front = front + 1;
          ce = loop[front];
          tv0 = cEdge[ce].node_id[0];
          tcVertex = cEdge[ce].node_id[1];
          tv2 = new_node0;
          ADD_TRIANGLE(cTrianglePtr, ctid, tv0, tcVertex, tv2, cEdge[ce].nSpin[0], cEdge[ce].nSpin[1] )

          new_node0 = tcVertex;
          cnumT++;
          ctid++;
        }
        else
        {
          back = back - 1;
          ce = loop[back];
          tv0 = cEdge[ce].node_id[0];
          tcVertex = cEdge[ce].node_id[1];
          tv2 = new_node0;
          ADD_TRIANGLE(cTrianglePtr, ctid, tv0, tcVertex, tv2, cEdge[ce].nSpin[0], cEdge[ce].nSpin[1] )

          new_node0 = tv0;
          cnumT++;
          ctid++;
        }
      }
    }
  }
  *tout = ctid;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CSliceBySlice::get_case_triangles_helper(int nedge, int* burnt, int* burnt_list, int &loopID,
                                                int* ae, StructArray<Segment>::Pointer cEdgePtr,
                                                std::vector<int> &countVec)
{
  int nSpin1, nSpin2, nnode1, nnode2;
  int bflag, nbflag;
  int cgrainname1, cgrainname2;
  int grainnameFlag, NodeFlag;
  int nucleus;
  int ne;
  int chaser;
  int cnode1, cnode2;
  int ce;
  Segment* cEdge = cEdgePtr->GetPointer(0);

  // initialize burn flags for Face edges...
  for (int ii = 0; ii < nedge; ii++)
  {
    burnt[ii] = 0;
    burnt_list[ii] = -1;
  }
  loopID = 1;
  int tail = 0;
  int head = 0;
  int coin;
  for (int i = 0; i < nedge; i++)
  {
    ce = ae[i];
    bflag = burnt[i];
    if (bflag == 0)
    {
      nucleus = ce;
      burnt[i] = loopID;
      burnt_list[tail] = ce;
      coin = 1;
      while (coin)
      {
        chaser = burnt_list[tail];
        cgrainname1 = cEdge[chaser].nSpin[0];
        cgrainname2 = cEdge[chaser].nSpin[1];
        cnode1 = cEdge[chaser].node_id[0];
        cnode2 = cEdge[chaser].node_id[1];
        for (int j = 0; j < nedge; j++)
        {
          ne = ae[j];
          nbflag = burnt[j];
          if (nbflag == 0)
          {
            nSpin1 = cEdge[ne].nSpin[0];
            nSpin2 = cEdge[ne].nSpin[1];
            nnode1 = cEdge[ne].node_id[0];
            nnode2 = cEdge[ne].node_id[1];
            // checking if Neighbor edge has same Neighboring grainnames...
            if (((cgrainname1 == nSpin1) && (cgrainname2 == nSpin2)) || ((cgrainname1 == nSpin2) && (cgrainname2 == nSpin1)))
            {
              grainnameFlag = 1;
            }
            else
            {
              grainnameFlag = 0;
            }
            // checking if neighor egde is contiguous...
            if ((cnode1 == nnode1) && (cnode2 != nnode2))
            {
              NodeFlag = 1;
            }
            else if ((cnode1 == nnode2) && (cnode2 != nnode1))
            {
              NodeFlag = 1;
            }
            else if ((cnode2 == nnode1) && (cnode1 != nnode2))
            {
              NodeFlag = 1;
            }
            else if ((cnode2 == nnode2) && (cnode1 != nnode1))
            {
              NodeFlag = 1;
            }
            else
            {
              NodeFlag = 0;
            }
            if (grainnameFlag == 1 && NodeFlag == 1)
            {
              head = head + 1;
              burnt_list[head] = ne;
              burnt[j] = loopID;
            }
          }
        }
        if (tail == head)
        {
          coin = 0;
          tail = tail + 1;
          head = tail;
          loopID++;
        }
        else
        {
          tail = tail + 1;
          coin = 1;
        }
      }
    }
  }
  countVec.resize(loopID);
  int* count = &(countVec.front());
  for (int k1 = 1; k1 < loopID; k1++)
  {
    count[k1] = 0;
  }
  for (int k = 1; k < loopID; k++)
  {
    for (int kk = 0; kk < nedge; kk++)
    {
      if (k == burnt[kk])
      {
        count[k] = count[k] + 1;
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CSliceBySlice::get_case_triangles_helper_2(int* burnt_loop, int* burnt_list,
                                                  int from, int to, int numN, int &ctid,
                                                  StructArray<Segment>::Pointer cEdgePtr,
                                                  StructArray<Patch>::Pointer cTrianglePtr)
{
  int front, back;
  int te0, te1, te2, tv0, tcVertex, tv2;
  int numT, cnumT, new_node0;
  int ce;
  int cnode1, cnode2;
  int tnode;
  int tgrainname;

  Segment* cEdge = cEdgePtr->GetPointer(0);

  int startEdge = burnt_list[from];
  burnt_loop[0] = startEdge;
  int index = 1;
  int endNode = cEdge[startEdge].node_id[1];
  int chaser = startEdge;
  while (index < numN)
  {
    for (int n = from; n < to; n++)
    {
      ce = burnt_list[n];
      cnode1 = cEdge[ce].node_id[0];
      cnode2 = cEdge[ce].node_id[1];
      if ((ce != chaser) && (endNode == cnode1))
      {
        burnt_loop[index] = ce;
        index++;
      }
      else if ((ce != chaser) && (endNode == cnode2))
      {
        burnt_loop[index] = ce;
        index++;
        // flip...
        tnode = cEdge[ce].node_id[0];
        tgrainname = cEdge[ce].nSpin[0];
        cEdge[ce].node_id[0] = cEdge[ce].node_id[1];
        cEdge[ce].node_id[1] = tnode;
        cEdge[ce].nSpin[0] = cEdge[ce].nSpin[1];
        cEdge[ce].nSpin[1] = tgrainname;
      }
    }
    chaser = burnt_loop[index - 1];
    endNode = cEdge[chaser].node_id[1];
  }
  if (numN == 3)
  {
    te0 = burnt_loop[0];
    te1 = burnt_loop[1];
    te2 = burnt_loop[2];
    ADD_TRIANGLE(cTrianglePtr, ctid, cEdge[te0].node_id[0], cEdge[te1].node_id[0], cEdge[te2].node_id[0], cEdge[te0].nSpin[0], cEdge[te0].nSpin[1] )

        ctid++;
  }
  else if (numN > 3)
  {
    numT = numN - 2;
    cnumT = 0;
    front = 0;
    back = numN - 1;
    te0 = burnt_loop[front];
    te1 = burnt_loop[back];
    tv0 = cEdge[te0].node_id[0];
    tcVertex = cEdge[te0].node_id[1];
    tv2 = cEdge[te1].node_id[0];
    ADD_TRIANGLE(cTrianglePtr, ctid, tv0, tcVertex, tv2, cEdge[te0].nSpin[0], cEdge[te0].nSpin[1] )

        new_node0 = tv2;
    //  new_node1 = tcVertex;
    cnumT++;
    ctid++;
    while (cnumT < numT)
    {
      if ((cnumT % 2) != 0)
      {
        front = front + 1;
        ce = burnt_loop[front];
        tv0 = cEdge[ce].node_id[0];
        tcVertex = cEdge[ce].node_id[1];
        tv2 = new_node0;
        ADD_TRIANGLE(cTrianglePtr, ctid, tv0, tcVertex, tv2, cEdge[ce].nSpin[0], cEdge[ce].nSpin[1] )

            new_node0 = tcVertex;
        cnumT++;
        ctid++;
      }
      else
      {
        back = back - 1;
        ce = burnt_loop[back];
        tv0 = cEdge[ce].node_id[0];
        tcVertex = cEdge[ce].node_id[1];
        tv2 = new_node0;
        ADD_TRIANGLE(cTrianglePtr, ctid, tv0, tcVertex, tv2, cEdge[ce].nSpin[0], cEdge[ce].nSpin[1] )

            new_node0 = tv0;
        cnumT++;
        ctid++;
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CSliceBySlice::get_case2_triangles(int site, int* ae, int nedge, int* afc,
                                          int nfctr, int tin, int* tout,
                                          StructArray<Segment>::Pointer cEdgePtr,
                                          StructArray<Patch>::Pointer cTrianglePtr)
{

  int n, i1;
  int loopID;

  int cnode1, cnode2;
  int tgrainname, tnode;
  int chaser;
  int start;
  int end;
  int from, to;
  int flip;
  int* burnt = NULL;
  int* burnt_list = NULL;
  int* count = NULL;
  std::vector<int> countVec;
  int numN;
  int* burnt_loop = NULL;
  int openL; // if a loop is an open loop, it's 1; if closed, it's 0...

  int index;
  int ctid;
  int front, back;
  int te0, te1, tv0, tcVertex, tv2;
  int numT, cnumT, new_node0;

  typedef boost::shared_array<int> SharedIntArray_t;

  SharedIntArray_t burntPtr(new int[nedge]);
  burnt = burntPtr.get();

  SharedIntArray_t burnt_list_ptr(new int[nedge]);
  burnt_list = burnt_list_ptr.get();

  Segment* cEdge = cEdgePtr->GetPointer(0);

  get_case_triangles_helper(nedge, burnt, burnt_list, loopID, ae, cEdgePtr, countVec);
  count = &(countVec.front());

  // Let's make complete loops...
  // numTri = 0;
  start = afc[0];
  end = afc[1];
  to = 0;
  from = 0;
  // Let's do triangulation...
  ctid = tin;
  for (int j1 = 1; j1 < loopID; j1++)
  {
    int startEdge;
    int endNode;
    int ce;
    openL = 0; // current loop is closed...
    numN = count[j1];
    to = to + numN;
    from = to - numN;
    SharedIntArray_t burnt_loop_ptr(new int[numN]);
    burnt_loop = burnt_loop_ptr.get();
    for (i1 = from; i1 < to; i1++)
    {
      ce = burnt_list[i1];
      cnode1 = cEdge[ce].node_id[0];
      cnode2 = cEdge[ce].node_id[1];
      if (start == cnode1)
      {
        openL = 1; // loop is open...
        startEdge = ce;
        flip = 0;
      }
      else if (start == cnode2)
      {
        openL = 1;
        startEdge = ce;
        flip = 1;
      }
    }
    if (openL == 1)
    { // if current loop is an open loop...
      if (flip == 1)
      {
        tnode = cEdge[startEdge].node_id[0];
        tgrainname = cEdge[startEdge].nSpin[0];
        cEdge[startEdge].node_id[0] = cEdge[startEdge].node_id[1];
        cEdge[startEdge].node_id[1] = tnode;
        cEdge[startEdge].nSpin[0] = cEdge[startEdge].nSpin[1];
        cEdge[startEdge].nSpin[1] = tgrainname;
      }
      burnt_loop[0] = startEdge;
      index = 1;
      endNode = cEdge[startEdge].node_id[1];
      chaser = startEdge;
      while (index < numN)
      {
        for (n = from; n < to; n++)
        {
          ce = burnt_list[n];
          cnode1 = cEdge[ce].node_id[0];
          cnode2 = cEdge[ce].node_id[1];
          if ((ce != chaser) && (endNode == cnode1))
          {
            burnt_loop[index] = ce;
            index++;
          }
          else if ((ce != chaser) && (endNode == cnode2))
          {
            burnt_loop[index] = ce;
            index++;
            // flip...
            tnode = cEdge[ce].node_id[0];
            tgrainname = cEdge[ce].nSpin[0];
            cEdge[ce].node_id[0] = cEdge[ce].node_id[1];
            cEdge[ce].node_id[1] = tnode;
            cEdge[ce].nSpin[0] = cEdge[ce].nSpin[1];
            cEdge[ce].nSpin[1] = tgrainname;
          }
        }
        chaser = burnt_loop[index - 1];
        endNode = cEdge[chaser].node_id[1];
      }
      if (numN == 2)
      {
        te0 = burnt_loop[0];
        te1 = burnt_loop[1];
        int ccn = cEdge[te0].node_id[0];
        int tn0 = cEdge[te1].node_id[0];
        int tn1 = cEdge[te1].node_id[1];
        int ts0 = cEdge[te0].nSpin[0];
        int ts1 = cEdge[te0].nSpin[1];
        ADD_TRIANGLE(cTrianglePtr, ctid, ccn, tn0, tn1, ts0, ts1 )
        ctid++;
      }
      else if (numN > 2)
      {
        numT = numN - 1; // Note that it's open loop...
        cnumT = 0;
        front = 0;
        back = numN;
        te0 = burnt_loop[front];
        te1 = burnt_loop[back - 1];
        tv0 = cEdge[te0].node_id[0];
        tcVertex = cEdge[te0].node_id[1];
        tv2 = cEdge[te1].node_id[1];
        ADD_TRIANGLE(cTrianglePtr, ctid, tv0, tcVertex, tv2, cEdge[te0].nSpin[0], cEdge[te0].nSpin[1] )

            new_node0 = tv2;
        //    new_node1 = tcVertex;
        cnumT++;
        ctid++;
        while (cnumT < numT)
        {
          if ((cnumT % 2) != 0)
          {
            front = front + 1;
            ce = burnt_loop[front];
            tv0 = cEdge[ce].node_id[0];
            tcVertex = cEdge[ce].node_id[1];
            tv2 = new_node0;
            ADD_TRIANGLE(cTrianglePtr, ctid, tv0, tcVertex, tv2, cEdge[ce].nSpin[0], cEdge[ce].nSpin[1] )

                new_node0 = tcVertex;
            cnumT++;
            ctid++;
          }
          else
          {
            back = back - 1;
            ce = burnt_loop[back];
            tv0 = cEdge[ce].node_id[0];
            tcVertex = cEdge[ce].node_id[1];
            tv2 = new_node0;
            ADD_TRIANGLE(cTrianglePtr, ctid, tv0, tcVertex, tv2, cEdge[ce].nSpin[0], cEdge[ce].nSpin[1] )

                new_node0 = tv0;
            cnumT++;
            ctid++;
          }
        }
      }
    }
    else
    {
      // if current loop is a closed one....i.e., openL = 0...
      get_case_triangles_helper_2(burnt_loop, burnt_list, from, to, numN, ctid, cEdgePtr, cTrianglePtr);
    }
  }
  *tout = ctid;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CSliceBySlice::get_caseM_triangles(int site, int *ae, int nedge, int *afc,
                                          int nfctr, int tin, int *tout, int ccn,
                                          StructArray<Segment>::Pointer cEdgePtr,
                                          StructArray<Patch>::Pointer cTrianglePtr)
{
  int n, i1;
  int n1, iii;
  int loopID;
  int ce;
  int cnode1, cnode2;
  int tgrainname, tnode;
  int chaser;
  int start;
  int from, to;
  int flip;
  int* burnt;
  int* burnt_list;
  int* count;
  std::vector<int> countVec;
  int numN, numTri;
  int tn0, tn1;
  int* burnt_loop;
  int openL; // if a loop is an open loop, it's 1; if closed, it's 0...
  int startEdge;
  int endNode;
  int index;
  int ctid;
  int ts0, ts1;

  typedef boost::shared_array<int> SharedIntArray_t;

  SharedIntArray_t burntPtr(new int[nedge]);
  burnt = burntPtr.get();

  SharedIntArray_t burnt_list_ptr(new int[nedge]);
  burnt_list = burnt_list_ptr.get();

  Segment* cEdge = cEdgePtr->GetPointer(0);

  get_case_triangles_helper(nedge, burnt, burnt_list, loopID, ae, cEdgePtr, countVec);
  count = &(countVec.front());

  // Let's make complete loops...
  numTri = 0;
  to = 0;
  from = 0;
  // Let's do triangulation...
  ctid = tin;
  for (int j1 = 1; j1 < loopID; j1++)
  {
    openL = 0; // current loop is closed...
    numN = count[j1];
    to = to + numN;
    from = to - numN;
    SharedIntArray_t burnt_loop_ptr(new int[numN]);
    burnt_loop = burnt_loop_ptr.get();
    for (i1 = from; i1 < to; i1++)
    {
      ce = burnt_list[i1];
      cnode1 = cEdge[ce].node_id[0];
      cnode2 = cEdge[ce].node_id[1];
      for (n1 = 0; n1 < nfctr; n1++)
      {
        start = afc[n1];
        if (start == cnode1)
        {
          openL = 1; // loop is open...
          startEdge = ce;
          flip = 0;
        }
        else if (start == cnode2)
        {
          openL = 1;
          startEdge = ce;
          flip = 1;
        }
      }
    }
    if (openL == 1)
    { // if current loop is an open loop...
      if (flip == 1)
      {
        tnode = cEdge[startEdge].node_id[0];
        tgrainname = cEdge[startEdge].nSpin[0];
        cEdge[startEdge].node_id[0] = cEdge[startEdge].node_id[1];
        cEdge[startEdge].node_id[1] = tnode;
        cEdge[startEdge].nSpin[0] = cEdge[startEdge].nSpin[1];
        cEdge[startEdge].nSpin[1] = tgrainname;
      }
      burnt_loop[0] = startEdge;
      index = 1;
      endNode = cEdge[startEdge].node_id[1];
      chaser = startEdge;
      while (index < numN)
      {
        for (n = from; n < to; n++)
        {
          ce = burnt_list[n];
          cnode1 = cEdge[ce].node_id[0];
          cnode2 = cEdge[ce].node_id[1];
          if ((ce != chaser) && (endNode == cnode1))
          {
            burnt_loop[index] = ce;
            index++;
          }
          else if ((ce != chaser) && (endNode == cnode2))
          {
            burnt_loop[index] = ce;
            index++;
            // flip...
            tnode = cEdge[ce].node_id[0];
            tgrainname = cEdge[ce].nSpin[0];
            cEdge[ce].node_id[0] = cEdge[ce].node_id[1];
            cEdge[ce].node_id[1] = tnode;
            cEdge[ce].nSpin[0] = cEdge[ce].nSpin[1];
            cEdge[ce].nSpin[1] = tgrainname;
          }
        }
        chaser = burnt_loop[index - 1];
        endNode = cEdge[chaser].node_id[1];
      }
      //triangulation...
      for (iii = 0; iii < numN; iii++)
      {
        ce = burnt_loop[iii];
        tn0 = cEdge[ce].node_id[0];
        tn1 = cEdge[ce].node_id[1];
        ts0 = cEdge[ce].nSpin[0];
        ts1 = cEdge[ce].nSpin[1];
        ADD_TRIANGLE(cTrianglePtr, ctid, ccn, tn0, tn1, ts0, ts1)
        ctid++;
      }
    }
    else
    {
    // if current loop is a closed one....i.e., openL = 0...
      get_case_triangles_helper_2(burnt_loop, burnt_list, from, to, numN, ctid, cEdgePtr, cTrianglePtr);
    }
  }
  *tout = ctid;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CSliceBySlice::arrange_grainnames(int numT, int zID, int NSP, int* wrappedDims,float* res,
                                        StructArray<Patch>::Pointer cTrianglePtr,
                                        StructArray<Node>::Pointer cVertexPtr,
                                        DataArray<int32_t>::Pointer voxelsPtr,
                                        StructArray<Neighbor>::Pointer neighborsPtr)
{
  //int i, j;
  int cnode;
  int csite, kind;
  int tsite1[3] = { -1, -1, -1};
  int tsite2[3] = { -1, -1, -1};
  //  int nSpin1, nSpin2;
  int tgrainname1[3], tgrainname2[3];
  float cx, cy, cz;
  float xSum, ySum, zSum;
  float vcoord[3][3];
  float u[3], w[3];
  float x, y, z;
  float a, b, c, d, length;
  float sidecheck;
  int shift = (zID * NSP);
  int locale;

  Patch* cTriangle = cTrianglePtr->GetPointer(0);
  Node* cVertex = cVertexPtr->GetPointer(0);
  int32_t* voxels = voxelsPtr->GetPointer(0);
  Neighbor* neigh = neighborsPtr->GetPointer(0);

  for (int i = 0; i < numT; i++)
  { // for each triangle...
    xSum = 0.0;
    ySum = 0.0;
    zSum = 0.0;
    //  nSpin1 = cTriangle[i]->nSpin[0];
    //  nSpin2 = cTriangle[i]->nSpin[1];
    cTriangle[i].nSpin[0] = -1;
    cTriangle[i].nSpin[1] = -1;
    for (int j = 0; j < 3; j++)
    { // for each Node inside the triangle...
      tsite1[j] = -1;
      tsite2[j] = -1;
      tgrainname1[j] = -1;
      tgrainname2[j] = -1;
      cnode = cTriangle[i].node_id[j];
      csite = cnode / 7 + 1;
      kind = cnode % 7;
      xSum = xSum + cVertex[cnode].coord[0];
      ySum = ySum + cVertex[cnode].coord[1];
      zSum = zSum + cVertex[cnode].coord[2];
      vcoord[j][0] = cVertex[cnode].coord[0];
      vcoord[j][1] = cVertex[cnode].coord[1];
      vcoord[j][2] = cVertex[cnode].coord[2];
      if (kind == 0)
      {
        tsite1[j] = csite;
        tsite2[j] = neigh[csite].neigh_id[1];
        tgrainname1[j] = voxels[tsite1[j]];
        tgrainname2[j] = voxels[tsite2[j]];
      }
      else if (kind == 1)
      {
        tsite1[j] = csite;
        tsite2[j] = neigh[csite].neigh_id[7];
        tgrainname1[j] = voxels[tsite1[j]];
        tgrainname2[j] = voxels[tsite2[j]];
      }
      else if (kind == 2)
      {
        tsite1[j] = csite;
        tsite2[j] = neigh[csite].neigh_id[18];
        tgrainname1[j] = voxels[tsite1[j]];
        tgrainname2[j] = voxels[tsite2[j]];
      }
    }
    // Getting the center of triangle...
    cx = xSum / 3.0f;
    cy = ySum / 3.0f;
    cz = zSum / 3.0f;
    // Getting normal vector of the triangle...(Right-hand Rule!!!)
    // Getting 2 edge vectors of triangle originating from vertex 0...
    u[0] = vcoord[1][0] - vcoord[0][0];
    u[1] = vcoord[1][1] - vcoord[0][1];
    u[2] = vcoord[1][2] - vcoord[0][2];
    w[0] = vcoord[2][0] - vcoord[0][0];
    w[1] = vcoord[2][1] - vcoord[0][1];
    w[2] = vcoord[2][2] - vcoord[0][2];
    // Cross product, UxW = aX + bY + cZ...
    // Note that UxW follows the same direction of triangle loop...
    a = u[1] * w[2] - u[2] * w[1];
    b = u[2] * w[0] - u[0] * w[2];
    c = u[0] * w[1] - u[1] * w[0];
    length = sqrt(a * a + b * b + c * c);
    a = a / length;
    b = b / length;
    c = c / length;
    if (fabs(a) < 0.00001) a = 0.0;
    if (fabs(b) < 0.00001) b = 0.0;
    if (fabs(c) < 0.00001) c = 0.0;
    // update Patch info...
//    cTriangle[i]->normal[0] = a;
//    cTriangle[i]->normal[1] = b;
//    cTriangle[i]->normal[2] = c;
//    cTriangle[i]->area = 0.5 * length;
    // determine which way normal should point...
    d = -(a * cx + b * cy + c * cz);
    for (int j = 0; j < 3; j++)
    {
      if (tsite1[j] != -1)
      {
        locale = tsite1[j] + shift;
        x = find_xcoord(locale,wrappedDims[0], res[0]);
        y = find_ycoord(locale, wrappedDims[0],wrappedDims[1], res[1]);
        z = find_zcoord(locale, wrappedDims[0], wrappedDims[1], res[2]);
        sidecheck = (a * x + b * y + c * z + d);
        if (sidecheck < -0.000001)
        {
          cTriangle[i].nSpin[0] = tgrainname2[j];
          cTriangle[i].nSpin[1] = tgrainname1[j];
        }
        else if (sidecheck > 0.000001)
        {
          cTriangle[i].nSpin[0] = tgrainname1[j];
          cTriangle[i].nSpin[1] = tgrainname2[j];
        }
      }
    }
    int k = 0;
    int index = 0;
    int testtsite = 0;
    //int sidechecked = 1;
    int xDim = wrappedDims[0];
    int yDim = wrappedDims[1];

    while (cTriangle[i].nSpin[0] == -1 && k < 6)
    {
      while (tsite1[k] == -1)
      {
        k++;
      }
      if (k == 3) a = -a, b = -b, c = -c;
      if (k < 3) index = k;
      if (k >= 3) index = k - 3;
      if (a != 0 && (tsite1[index] % xDim + a) >= 0 && (tsite1[index] % xDim + a) < xDim) testtsite = tsite1[index] + (a * 1);
      if (b != 0 && ((tsite1[index] / xDim) % yDim + b) >= 0 && ((tsite1[index] / xDim) % yDim + b) < yDim) testtsite = tsite1[index] + (b * xDim);
      if (c != 0 && (tsite1[index] / (xDim * yDim) + c) >= 0 && (tsite1[index] / (xDim * yDim) + c) < 2) testtsite = tsite1[index] + (c * NSP);
      int gname = voxels[testtsite];
      if (gname == tgrainname1[index])
      {
        if (k < 3) cTriangle[i].nSpin[0] = tgrainname1[index], cTriangle[i].nSpin[1] = tgrainname2[index];
        if (k >= 3) cTriangle[i].nSpin[0] = tgrainname2[index], cTriangle[i].nSpin[1] = tgrainname1[index];
      }
      if (gname == tgrainname2[index])
      {
        if (k < 3) cTriangle[i].nSpin[0] = tgrainname2[index], cTriangle[i].nSpin[1] = tgrainname1[index];
        if (k >= 3) cTriangle[i].nSpin[0] = tgrainname1[index], cTriangle[i].nSpin[1] = tgrainname2[index];
      }
      if (gname != tgrainname1[index] && gname != tgrainname2[index]) k++;
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int M3CSliceBySlice::assign_nodeID(int nN, int NSP,
                                   DataArray<int32_t>::Pointer cVertexNodeIdPtr,
                                   DataArray<int8_t>::Pointer cVertexNodeTypePtr)
{
  int nid = 0;
  int nkind = 0;
  int cnid = 0;

  int8_t* nodeType = cVertexNodeTypePtr->GetPointer(0);
  int32_t* nodeID = cVertexNodeIdPtr->GetPointer(0);
  nid = nN;
  for (int i = 0; i < (7 * 2 * NSP); i++)
  {
    nkind = nodeType[i];
    cnid = nodeID[i];
  //  plane = i % 7;
    if (nkind != 0 && cnid == -1)
    {
      nodeID[i] = nid;
      nid++;
    }
  }
  return nid;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CSliceBySlice::update_node_edge_kind(int nT,
                                            StructArray<Patch>::Pointer cTrianglePtr,
                                            DataArray<int8_t>::Pointer cVertexNodeTypePtr,
                                            StructArray<Segment>::Pointer cEdgePtr)
{
  int tn = 0;
 // int te;
  int triangleNodeType;
//  int tekind;
  int tspin1, tspin2;

  Triangle* t = cTrianglePtr->GetPointer(0);
  int8_t* nodeType = cVertexNodeTypePtr->GetPointer(0);
//  Segment* fe = cEdgePtr->GetPointer(0);
//  int nfedge = cEdgePtr->GetNumberOfTuples();
  for (int j = 0; j < nT; j++)
  {
    tspin1 = t[j].nSpin[0];
    tspin2 = t[j].nSpin[1];
    if(tspin1 * tspin2 < 0)
    { // if the triangle is the surface of whole microstructure...
      // increase edge and node kind by 10...
      for (int i = 0; i < 3; i++)
      {
        // nodeKind...
        tn = t[j].node_id[i];
        triangleNodeType = nodeType[tn];
        if(triangleNodeType < 10)
        {
          nodeType[tn] = triangleNodeType + 10;
        }
        // edgeKind...
//        te = t[j].e_id[i];
//        if(te < nfedge)
//        {
//          tekind = fe[te].edgeKind;
//          if(tekind < 10)
//          {
//            fe[te].edgeKind = tekind + 10;
//          }
//        }
//        else
//        {
//          te = te - nfedge;
//          tekind = ie[te].edgeKind;
//          if(tekind < 10)
//          {
//            ie[te].edgeKind = tekind + 10;
//          }
//        }
      }
    }

  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int M3CSliceBySlice::writeNodesFile(int zID, int cNodeID, int NSP,
                                    const std::string &nodesFile,
                                    StructArray<Node>::Pointer cVertexPtr,
                                    DataArray<int32_t>::Pointer cVertexNodeIdPtr,
                                    DataArray<int8_t>::Pointer cVertexNodeTypePtr )
{

  #if 0
  static const size_t BYTE_COUNT = 20;
  unsigned char data[BYTE_COUNT];
  int* tId = (int*)(&data[0]);
  int* nk = (int*)(&data[4]);
  float* vec3f = (float*)(&data[8]);
  #endif
  const size_t BYTE_COUNT = sizeof(SurfaceMesh::NodesFile::NodesFileRecord_t);
  SurfaceMesh::NodesFile::NodesFileRecord_t record;

  size_t totalWritten = 0;
  FILE* f = NULL;

 // std::cout << "M3CSliceBySlice writing Nodes file " << cNodeID << std::endl;

  // Create a new file if this is our first slice
  if (zID == 0)
  {
    f = fopen(nodesFile.c_str(), "wb");
    if (NULL == f)
    {
      return -1;
    }
  }
  // Append to existing file if we are on z>0 slice
  if (zID > 0)
  {
    f = fopen(nodesFile.c_str(), "ab");
    if (NULL == f)
    {
      return -1;
    }
  }
  int total = (7 * 2 * NSP);
  int32_t* nodeID = cVertexNodeIdPtr->GetPointer(0);
  int8_t* nodeKind = cVertexNodeTypePtr->GetPointer(0);
  Node* cVertex = cVertexPtr->GetPointer(0);

  for (int k = 0; k < total; k++)
  {
    //*tId = nodeID[k];
    record.nodeId = nodeID[k];
    if (record.nodeId > cNodeID - 1)
    {
      //*nk = nodeKind[k];
      record.nodeKind = nodeKind[k];
      record.x = cVertex[k].coord[0];
      record.y = cVertex[k].coord[1];
      record.z = cVertex[k].coord[2];

//      vec3f[0] = cVertex[k].coord[0];
//      vec3f[1] = cVertex[k].coord[1];
//      vec3f[2] = cVertex[k].coord[2];
      totalWritten = fwrite(&record, BYTE_COUNT, 1, f);
      if (totalWritten != 1)
      {
        std::cout << "Not enough data written to the Nodes file." << std::endl;
        return -1;
      }
    }
  }
  fclose(f);
  return 0;
}

// -----------------------------------------------------------------------------
//  Write a BINARY file which is only TEMP during the surface meshing
// -----------------------------------------------------------------------------
int M3CSliceBySlice::writeTrianglesFile(int zID, int ctid,
                                    const std::string &trianglesFile, int nt,
                                    StructArray<Patch>::Pointer cTrianglePtr,
                                    DataArray<int32_t>::Pointer cVertexNodeIdPtr)
{

  #if 0
  static const size_t DATA_COUNT = 9;
  int data[DATA_COUNT];
  data[0] = ctid;
  #endif

  const size_t BYTE_COUNT = SurfaceMesh::TrianglesFile::ByteCount;
  SurfaceMesh::TrianglesFile::TrianglesFileRecord_t record;
  record.triId = ctid;
//  std::cout << "Writing Triangles Starting at " << record.triId << std::endl;
 // int tag;
  int end = nt;
  int n1, n2, n3;

  FILE* f = NULL;

  int32_t* nodeID = cVertexNodeIdPtr->GetPointer(0);
  Patch* cTriangle = cTrianglePtr->GetPointer(0);

  // Create a new file if this is our first slice
  if (zID == 0)
  {
    f = fopen(trianglesFile.c_str(), "wb");
    if (NULL == f)
    {
      return -1;
    }
#if (WRITE_BINARY_TEMP_FILES == 0)
      fprintf(f, "XXXXXXXXXXXX\n"); // This should get over written at some point with the actual number of triangles
#endif
  }
  // Append to existing file if we are on z>0 slice
  if (zID > 0)
  {
    f = fopen(trianglesFile.c_str(), "ab");
    if (NULL == f)
    {
      return -1;
    }
  }
  size_t totalWritten = 0;
  //  outFile << nt <<endl;
  for (int i = 0; i < end; i++)
  {

    n1 = cTriangle[i].node_id[0];
    n2 = cTriangle[i].node_id[1];
    n3 = cTriangle[i].node_id[2];
#if (WRITE_BINARY_TEMP_FILES == 1)
    record.nodeId_0 = nodeID[n1];
    record.nodeId_1 = nodeID[n2];
    record.nodeId_2 = nodeID[n3];
//    data[4] = cTriangle[i].edges[0]->getId();
//    data[5] = cTriangle[i]->edges[1]->getId();
//    data[6] = cTriangle[i]->edges[2]->getId();
    record.label_0 = cTriangle[i].nSpin[0];
    record.label_1 = cTriangle[i].nSpin[1];

    totalWritten = fwrite(&record, BYTE_COUNT, 1, f);
    if (totalWritten != 1)
    {
      std::cout << "Error Writing Triangles Temp File. Not enough elements written. Wrote " << totalWritten << " of 6." << std::endl;
      return -1;
    }
#else

    fprintf(f, "%d    %d    %d    %d    %llu    %llu    %llu    %d    %d\n", data[0], cVertex[n1].NodeID,cVertex[n2].NodeID,cVertex[n3].NodeID,
            cTriangle[i]->edges[0]->getId(), cTriangle[i]->edges[1]->getId(), cTriangle[i]->edges[2]->getId(),
            cTriangle[i]->nSpin[0], cTriangle[i]->nSpin[1]);
#endif
//    fprintf(f, "%d %d %d %d %d %d\n", newID, n1, n2, n3, s1, s2);
    record.triId = record.triId + 1;
  }
  // Go to beginning of file and update the number of triangles
  //fseek(f, 0, SEEK_SET);
  fclose(f);

#if (WRITE_BINARY_TEMP_FILES == 0)
  f = fopen(trianglesFile.c_str(), "rb+");
  std::cout << "data[0]: " << data[0] << std::endl;
  fprintf(f, "%012d\n", data[0]);
  fclose(f);
#endif

  return 0;
}


#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void M3CSliceBySlice::analyzeWinding()
{
//  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
//  std::cout << " Edge Count: " << eMap.size() << std::endl;
//  std::cout << " Triangle Count: " << cTriangle.size() << std::endl;
//  std::cout << "labelTriangleMap.size(): " << labelTriangleMap.size() << std::endl;

  float total = (float)(labelTriangleMap.size());
  // Keeps a list of all the triangles that have been visited.
  std::vector<bool> masterVisited(cTriangle.size(), false);

  // Start with first triangle in the master list:
  int masterTriangleIndex = 0;

  // Get the first 2 labels to visit from the first triangle. We use these 2 labels
  // because this triangle shares these 2 labels and guarantees these 2 labels are
  // connected to each other.
  float curPercent = 0.0;
  int progressIndex = 0;
  for (LabelTriangleMapType::iterator cLabel = labelTriangleMap.begin(); cLabel != labelTriangleMap.end(); ++cLabel )
  {

    int currentLabel = cLabel->first;
  //  if (currentLabel != 1) { continue; }
    masterTriangleIndex = cLabel->second;
    Patch::Pointer t = cTriangle[masterTriangleIndex];

    if ( (progressIndex/total * 100.0f) > (curPercent) )
    {
   //   std::cout << "Verifying Winding: " << curPercent << "% Complete" << std::endl;
      curPercent += 5.0f;
    }
    ++progressIndex;
  //  std::cout << "Current Label: " << currentLabel << std::endl;
  //  int seedTriIndex = masterTriangleIndex;

    if (NULL == t.get() )
    {
      std::cout << "Could not find a triangle with the winding set. This should NOT happen" << std::endl;
      assert(1 == 0);
    }

    std::set<int> localVisited; // Keep track of which triangles have been visited
    std::deque<int> triangleDeque;
    triangleDeque.push_back(t->tIndex);

    while (triangleDeque.empty() == false)
    {
      Patch::Pointer currentTri = cTriangle[triangleDeque.front()];
  //    std::cout << "tIndex = " << t->tIndex << std::endl;
      localVisited.insert(currentTri->tIndex);
      std::vector<int> adjTris = findAdjacentTriangles(currentTri, currentLabel);
      for ( std::vector<int>::iterator adjTri = adjTris.begin(); adjTri != adjTris.end(); ++adjTri )
      {
        //  std::cout << "  ^ AdjTri index: " << (*adjTri)->tIndex << std::endl;
        if (masterVisited[*adjTri] == false)
        {
          //   std::cout << "   * Checking Winding: " << (*adjTri)->tIndex << std::endl;
          Patch::Pointer triToVerify = cTriangle[*adjTri];
          currentTri->verifyWinding( triToVerify.get(), currentLabel);
        }

        if (localVisited.find(*adjTri) == localVisited.end()
          && find(triangleDeque.begin(), triangleDeque.end(), *adjTri) == triangleDeque.end())
        {
          // std::cout << "   # Adding to Deque: " << (*adjTri)->tIndex << std::endl;
          triangleDeque.push_back(*adjTri);
          localVisited.insert(*adjTri);
          masterVisited[*adjTri] = true;
        }
      }

      triangleDeque.pop_front();
    }

  }

 // std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<int> M3CSliceBySlice::findAdjacentTriangles(Triangle* triangle, int label)
{
  std::vector<int> adjacentTris;
  typedef SharedEdge::Pointer EdgeType;
  // Get the 3 edges from the triangle


  // Iterate over the 3 Edges of the triangle
  for (int i = 0; i < 3; ++i)
  {
    // Get the current edge
    EdgeType e = triangle->edges[i];
    // Get the list of indices of the triangles that belong to that edge
    std::set<int> tIndices = e->triangles;
    // Iterate over the indices to find triangles that match the label and are NOT the current triangle index
    for (std::set<int>::iterator iter = tIndices.begin(); iter != tIndices.end(); ++iter )
    {
      Patch::Pointer t = cTriangle.at(*iter);
      if ( (t->nSpin[0] == label || t->nSpin[1] == label)
          && (t->tIndex != triangle->tIndex) )
      {
     //   std::cout << "    Found Adjacent Triangle: " << t->tIndex << std::endl;
        adjacentTris.push_back(t->tIndex);
      }
    }
  }

  return adjacentTris;

}


#endif
