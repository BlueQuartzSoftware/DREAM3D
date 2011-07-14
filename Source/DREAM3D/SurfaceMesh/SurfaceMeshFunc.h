/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _SurfaceMeshFunc_H
#define _SurfaceMeshFunc_H

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include <cstddef>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <fstream>
#include <list>
#include <algorithm>
#include <numeric>
#include <map>

#include "MXA/MXATypes.h"
#include "MXA/Common/MXASetGetMacros.h"

#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/Grain.h"
#include "DREAM3D/Common/AIMRandomNG.h"
#include "DREAM3D/ANGSupport/AngDataLoader.h"
#include "DREAM3D/SurfaceMesh/Meshing/Patch.h"
#include "DREAM3D/SurfaceMesh/Meshing/Face.h"
#include "DREAM3D/SurfaceMesh/Meshing/Neighbor.h"
#include "DREAM3D/SurfaceMesh/Meshing/Node.h"
#include "DREAM3D/SurfaceMesh/Meshing/Segment.h"
#include "DREAM3D/SurfaceMesh/Meshing/SharedEdge.h"

using namespace meshing;


class DREAM3DLib_EXPORT SurfaceMeshFunc
{

public:

  MXA_SHARED_POINTERS(SurfaceMeshFunc)
  MXA_STATIC_NEW_MACRO(SurfaceMeshFunc)

  virtual ~SurfaceMeshFunc();

  std::ifstream in;
  int err;
  int NS; // The number of sites(voxels) in the simulation box...
  int NSP;
  int numgrains;
  int xDim;
  int yDim;
  int zDim;
  float xRes;
  float yRes;
  float zRes;
  float xOrigin;
  float yOrigin;
  float zOrigin;

  Neighbor* neigh; // contains nearest neighbor information...

  int* voxels; // contains voxel information...

  Face* cSquare; // contains square information...

  Node* cVertex; // contains edges on square faces for open loops...

  std::vector<Segment::Pointer> cEdge; // contains edges on square faces for open loops...
  std::vector<Patch::Pointer> cTriangle;

  typedef std::map<uint64_t, meshing::SharedEdge::Pointer>    EdgeMapType;
  typedef int                                       Label;
  typedef std::map<Label, int >                     LabelTriangleMapType;

  EdgeMapType eMap;
  LabelTriangleMapType labelTriangleMap;

  void analyzeWinding();
  std::vector<int> findAdjacentTriangles(Patch::Pointer triangle, int label);


  void get_neighbor_list();
  void initialize_nodes(int zID);
  void initialize_squares(int zID);
  int get_nodes_Edges(int et2d[20][8], int NST2d[20][8], int zID);
  int get_square_index(int tns[4]);
  int treat_anomaly(int tnst[4], int zID1);
  void get_nodes(int cst, int ord, int nidx[2], int *nid);
  void get_grainnames(int CubeOrigin, int sqOrder, int pID[2], int *pgrainname);
  int get_triangles();
  void get_case0_triangles(int site, int *afe, int nfedge, int tin, int *tout);
  void get_case2_triangles(int site, int *afe, int nfedge, int *afc, int nfctr, int tin, int *tout);
  void get_caseM_triangles(int site, int *afe, int nfedge, int *afc, int nfctr, int tin, int *tout, int ccn);
  void arrange_grainnames(int numT, int zID);
  int assign_nodeID(int nN);
  void update_current_triangles(int nT);
  float find_xcoord(int);
  float find_ycoord(int);
  float find_zcoord(int);
  int writeNodesFile(int zID, int cNodeID, const std::string &NodesRawFile);
  int writeTrianglesFile(int zID, int ctid,
                         const std::string &trianglesFile,
                         int nt);


protected:
  SurfaceMeshFunc();

private:
	SurfaceMeshFunc(const SurfaceMeshFunc&);    // Copy Constructor Not Implemented
    void operator=(const SurfaceMeshFunc&);  // Operator '=' Not Implemented
};


#endif
