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
#ifndef SURFACEMESHSTRUCTS_H_
#define SURFACEMESHSTRUCTS_H_


#include <string>

#define num_neigh 26

#if 0
typedef struct _voxel{
  int spin;
  //float coord[3];
} voxel;
#endif

typedef struct _neighbor{
  int neigh_id[num_neigh+1];
} neighbor;

typedef struct _face{
  int site_id[4];    // stores 4 sites at the corners of each square...
  int edge_id[4];    // stores edge id turned on...others will have dummy -1...
  int nEdge;         // number of edges on the square...
  int FCnode;        // face center node...if not, it's -1...
  int effect;        // 0 if the square is useless; 1 is good...
} Face;

typedef struct _node{
  int nodeKind;      // 2 for binary, 3 for triple, and so on...
  float coord[3];
  int newID;         // newID for used nodes; if not used, it's -1...
} Node;

typedef struct _segment{
  int neigh_spin[2]; // 0 is to the left of the arrow; 1 is at right...
  int node_id[2];    // the segment heads from node_id[0] to node_id[1]...
  int edgeKind;      // initially marked as 2; for face edges it's always 2...
} Segment;

/* Used for "inner edge" spin calculations */
typedef struct _isegment{
  int node_id[2];
  int edgeKind;      // initially marked with 2...
  int nSpin[4];
} isegment;

typedef struct _patch{
  int node_id[3];       // stores three new node id for vertices of the triangles...
  int e_id[3];       // stores three new edge id for sides of the triangles...
  int nSpin[2];      // neighboring two spins...
  int edgePlace[3];
  int mCubeID;       // marching cube id = site id at corner 0 of the marching cube..
} Patch;

#if 0
typedef struct _meshParameters {
    int32_t* point;    // This is the voxel volume
    int    xnum;
    int    ynum;
    int    znum;
    float xstep;
    float ystep;
    float zstep;
 //   std::string outputBaseName;
//    std::string nodes_file;
//    std::string edges_file;
//    std::string triangles_file;
    bool   verbose;
} MeshParameters;
#endif

#endif /* SURFACEMESHSTRUCTS_H_ */
