/* ============================================================================
* Copyright (c) 2012 Dr. Sukbin Lee (Carnegie-Mellon University)
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
* Neither the name of Anthony Rollet, Sukbin Lee, Jason Gruber, Stephen Sintay
* nor the names of its contributors may be used to endorse
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
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef MESHSTRUCTS_H_
#define MESHSTRUCTS_H_

#include <vector>
#include <string>
#include <set>
#include <map>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/DataArrays/StructArray.hpp"



/**
 * @brief These are the basic data structures to use for our triangle based surface meshes.
 */
namespace DREAM3D
{
  namespace Mesh
  {
    typedef float Float_t;
    typedef double Double_t;

    typedef struct
    {
      Float_t pos[3];
    } Vert_t;

    typedef struct
    {
      Double_t pos[3];
    } VertD_t;

    typedef struct
    {
      int verts[2];
    } Edge_t;

    const int32_t k_VertexNumElements = 3;

    typedef struct
    {
      int verts[3];
    } Face_t;

    typedef struct
    {
      std::vector<int64_t> verts;
    } Cell_t;

    // This constant values needs to be the number of total integers that are encoded into the Face Structure
    const int32_t k_FaceNumElements = 3;

    typedef StructArray<Vert_t> VertList_t;
    typedef StructArray<Edge_t> EdgeList_t;
    typedef StructArray<Face_t> FaceList_t;
    typedef StructArray<Cell_t> CellList_t;

    typedef VertList_t::Pointer VertListPointer_t;
    typedef EdgeList_t::Pointer EdgeListPointer_t;
    typedef FaceList_t::Pointer FaceListPointer_t;
    typedef CellList_t::Pointer CellListPointer_t;

    typedef std::set<int32_t> UniqueFaceIds_t;
    typedef std::map<int32_t, UniqueFaceIds_t > VertexFaceMap_t;

  }
}
// -----------------------------------------------------------------------------
// These structures are specific to the MultiMaterial Marching Cubes "M3C" algorithms
// -----------------------------------------------------------------------------
#define num_neigh 26

namespace SurfaceMesh
{
  namespace M3C
  {
    typedef struct
    {
      DREAM3D::Mesh::Float_t coord[3];
    } VoxelCoord;

    typedef struct
    {
      int neigh_id[num_neigh + 1];
    } Neighbor;

    typedef struct
    {
      int site_id[4];    // stores 4 sites at the corners of each square...
      int edge_id[4];    // stores edge id turned on...others will have dummy -1...
      int nEdge;         // number of edges on the square...
      int FCnode;        // face center node...if not, it's -1...
      int effect;        // 0 if the square is useless; 1 is good...
    } Face;

    typedef struct
    {
      int node_id[2];    // the segment heads from node_id[0] to node_id[1]...
      int edgeKind;      // initially marked as 2; for face edges it's always 2...
      int nSpin[2];      // 0 is to the left of the arrow; 1 is at right...
    } Segment;

    /* Used for "inner edge" spin calculations */
    typedef struct
    {
      int node_id[2];    // the segment heads from node_id[0] to node_id[1]...
      int edgeKind;      // initially marked with 2...
      int nSpin[4];
    } ISegment;

    typedef struct
    {
      int node_id[3];    // stores three new node id for vertices of the triangles...
      uint64_t e_id[3];       // stores three new edge id for sides of the triangles...
      int nSpin[2];      // two spins...
      int edgePlace[3];
    } Triangle;

    typedef Triangle Patch; // This is here for compatibility
  }
}


// -----------------------------------------------------------------------------
// These structures are used to write the binary temp files during some of the
// meshing algorithms.
// -----------------------------------------------------------------------------
namespace SurfaceMesh
{
  namespace NodesFile
  {
    typedef struct
    {
      int nodeId;
      int nodeKind;
      DREAM3D::Mesh::Float_t x;
      DREAM3D::Mesh::Float_t y;
      DREAM3D::Mesh::Float_t z;
    } NodesFileRecord_t;
    const int ByteCount = sizeof(NodesFileRecord_t);
  }

  namespace TrianglesFile
  {
    typedef struct
    {
      int triId;
      int nodeId_0;
      int nodeId_1;
      int nodeId_2;
      int label_0;
      int label_1;
    } TrianglesFileRecord_t;
    const int ByteCount = sizeof(TrianglesFileRecord_t);
  }
}

#endif /* MESHSTRUCTS_H_ */
