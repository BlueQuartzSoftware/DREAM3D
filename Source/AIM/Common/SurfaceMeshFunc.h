///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////
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

#include <MXA/MXATypes.h>
#include <MXA/Common/MXASetGetMacros.h>

#include <AIM/Common/Grain.h>
#include <AIM/Common/Voxel.h>
#include <AIM/Common/Patch.h>
#include <AIM/Common/Face.h>
#include <AIM/Common/Neighbor.h>
#include <AIM/Common/Node.h>
#include <AIM/Common/Segment.h>
#include <AIM/Common/Bin.h>
#include <AIM/Common/AIMRandomNG.h>
#include <AIM/ANG/AngFileHelper.h>


using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

// constants for procedure ran3
#define MBIG 1000000000
#define MSEED 161803398
#define MZ 0
#define FAC (1.0/MBIG)

#define num_neigh 26


#define EDGES_RAW_POSTFIX     "_edges_raw.txt"
#define TRIANGLES_RAW_POSTFIX "_triangles_raw.txt"

// Output Files
#define EDGES_FILE         "edges.txt"
#define TRIANGLES_FILE     "triangles.txt"
#define NODES_FILE         "nodes.txt"

// Input Files
#define NODES_RAW_FILE     "nodes_raw.txt"
#define MESH_STAT_FILE     "mesh_stat.txt"



class SurfaceMeshFunc
{

public:

  MXA_SHARED_POINTERS(SurfaceMeshFunc)
  MXA_STATIC_NEW_MACRO(SurfaceMeshFunc)

  virtual ~SurfaceMeshFunc();

  std::ifstream in;
  int err;
  int NS; // The number of sites(voxels) in the simulation box...
  int NSP;
  int xDim;
  int yDim;
  int zDim;
  double xRes;
  double yRes;
  double zRes;
  double xOrigin;
  double yOrigin;
  double zOrigin;
  Neighbor* neigh; // contains nearest neighbor information...
  Voxel* point; // contains voxel information...
  Face* cSquare; // contains square information...
 // Face* pSquare;
  Node* cVertex; // contains edges on square faces for open loops...
  Segment* cEdge; // contains edges on square faces for open loops...
  Patch* cTriangle;
  // Edge edge and neighboring grainname table...



  int initialize_micro(string, int);
  void get_neighbor_list(int zID);
  void initialize_nodes(int zID);
  void initialize_squares(int zID);
  int get_number_Edges(int zID);
  void get_nodes_Edges(int et2d[20][8], int NST2d[20][8], int zID, int nFEdge);
  int get_square_index(int tns[4]);
  int treat_anomaly(int tnst[4], int zID1);
  void get_nodes(int cst, int ord, int nidx[2], int *nid);
  void get_grainnames(int nSpn[4], int pID[2], int *pgrainname);
  int get_number_triangles();
  int get_number_case0_triangles(int *afe, int nfedge);
  int get_number_case2_triangles(int *afe, int nfedge, int *afc, int nfctr);
  int get_number_caseM_triangles(int *afe, int nfedge, int *afc, int nfctr);
  int get_triangles(int nTriangle);
  void get_case0_triangles(int *afe, int nfedge, int tin, int *tout);
  void get_case2_triangles(int *afe, int nfedge, int *afc, int nfctr, int tin, int *tout);
  void get_caseM_triangles(int *afe, int nfedge, int *afc, int nfctr, int tin, int *tout, int ccn);
  void arrange_grainnames(int numT, int zID);
  int assign_nodeID(int nN, int zID);
  void update_current_triangles(int nT);

  double find_xcoord(long);
  double find_ycoord(long);
  double find_zcoord(long);


  void writeNodesFile(int zID, int cNodeID, const std::string &NodesRawFile);
  void writeTrianglesFile(int nt, const std::string &TrianglesFileIndex, int zID, int ctid);
  void writeVTKOutputFile (int nNodes, int nTriangles,
                           const std::string &VisualizationFile,
                           const std::string &NodesFile,
                           const std::string &TrianglesFile,
                           bool binaryFile,
                           bool conformalMesh);


protected:
  SurfaceMeshFunc();

private:
	SurfaceMeshFunc(const SurfaceMeshFunc&);    // Copy Constructor Not Implemented
    void operator=(const SurfaceMeshFunc&);  // Operator '=' Not Implemented
};


#endif
