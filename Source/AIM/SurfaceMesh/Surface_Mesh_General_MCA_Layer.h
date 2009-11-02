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

#ifndef SURFACE_MESH_GENERAL_MCA_LAYER_H_
#define SURFACE_MESH_GENERAL_MCA_LAYER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define PI 3.14159265
#define num_neigh 26

// constants for procedure ran3
#define MBIG 1000000000
#define MSEED 161803398
#define MZ 0
#define FAC (1.0/MBIG)


#define NODES_RAW_FILE "nodes_raw.txt"
#define EDGETABLE_2D_FILE  "edgeTable_2d.txt";
#define NEIGHSPIN_TABLE_FILE  "neighspinTable_2d.txt";
#define MESH_STAT_FILE  "mesh_stat.txt";


  struct voxel
  {
    int spin;
    double coord[3];
    int atSurf; // if it's 0, it's in the bulk; if 1, it's on the surface of the domain...
  };

  struct neighbor
  {
    int csiteid;
    int neigh_id[num_neigh + 1];
  };

  struct face
  {
    int site_id[4]; // stores 4 sites at the corners of each square...
    int edge_id[4]; // stores edge id turned on...others will have dummy -1...
    int nEdge; // number of edges on the square...
    int turnFC; // if 1, face center is on..., else it's 0
    int FCnode; // face center node...if not, it's -1...
    int effect; // 0 if the square is useless; 1 is good...
  };

  struct node
  {
    int nodeKind; // 2 for binary, 3 for triple, and so on...
    double coord[3];
    int newNodeID; // newID for used nodes; if not used, it's -1...
  };

  struct segment
  {
    int neigh_spin[2]; // 0 is to the left of the arrow; 1 is at right...
    int node_id[2]; // the segment heads from node_id[0] to node_id[1]...
    int new_n_id[2];
    int segKind; // 2 for binary line, 3 for triple line, and so on...
    int where; // 0 for upper squares, 1 for side squares and 2 for bottom squares...
  };

  struct isegment
  {
    int n_id[2];
    int new_n_id[2];
    int edgeKind;
    int nSpin[4];
    int burnt; // if not burnt it's -1...
    int eff; // flag for output, if it's 1 it will be used for output...
  };

  struct patch
  {
    int v_id[3]; // stores three new node id for vertices of the triangles...
    int new_v_id[3];
    int nSpin[2]; // neighboring two spins...
    int edgePlace[3]; // if it's 0, face edges; if 1, inner edges...
    double normal[3];
    double area;
  };


  int SurfaceMesh_MCALayer( int xnum, int ynum, int znum,
                   const char* outputDir,
                   const char* dxFile );

  // function prototypes...
  int initialize_micro(struct voxel *p, int ns, int xDim, int yDim, int zDim, const char* dxFile );
  int read_edge_neighspin_table(int eT2d[20][8], int nsT2d[20][8], const char* edgeTable,
                                 const char* neighspinTable);

  void get_neighbor_list(struct neighbor *n, int ns, int nsp, int xDim, int yDim, int zDim, int zID);
  void initialize_nodes(struct voxel *p, struct node *v, int nsp, int zID);
  void initialize_squares(struct neighbor *n, struct face *sq, int ns, int nsp, int zID);

  int get_number_fEdges(struct face *sq, struct voxel *p, struct neighbor *n, int nsp, int zID);
  void get_nodes_fEdges(struct face *sq,
                        struct voxel *p,
                        struct neighbor *n,
                        struct node *v,
                        struct segment *e,
                        int eT2d[20][8],
                        int nsT2d[20][8],
                        int ns,
                        int nsp,
                        int xDim,
                        int zID);
  int get_square_index(int tns[4]);
  int treat_anomaly(int tnst[4], struct voxel *p1, struct neighbor *n1, int nsp1, int zID1);
  void get_nodes(int cst, int ord, int nidx[2], int *nid, int nsp1, int xDim1);
  void get_spins(int nSpn[4], int pID[2], int *pSpin);

  int get_number_triangles(struct voxel *p, struct face *sq, struct node *v, struct segment *e, int ns, int nsp, int xDim);
  int get_number_case0_triangles(int *afe, struct node *v1, struct segment *e1, int nfedge);
  int get_number_case2_triangles(int *afe, struct node *v1, struct segment *e1, int nfedge, int *afc, int nfctr);
  int get_number_caseM_triangles(int *afe, struct node *v1, struct segment *e1, int nfedge, int *afc, int nfctr);

  int get_triangles(struct patch *t, struct face *sq, struct node *v, struct segment *e, int ns, int nsp, int xDim);
  void get_case0_triangles(struct patch *t1, int *afe, struct node *v1, struct segment *e1, int nfedge, int tin, int *tout);
  void get_case2_triangles(struct patch *t1, int *afe, struct node *v1, struct segment *e1, int nfedge, int *afc, int nfctr, int tin, int *tout);
  void get_caseM_triangles(struct patch *t1, int *afe, struct node *v1, struct segment *e1, int nfedge, int *afc, int nfctr, int tin, int *tout, int ccn);
  void arrange_spins(struct voxel *p, struct patch *t, struct node *v, struct neighbor *n, int numT, int xDim, int nsp, int zID);

  int get_inner_edges(struct node *v, struct segment *fe, struct isegment *ie, struct patch *t, int nfe, int nT);

  void find_unique_inner_edges(struct isegment *ie, int nie, int *nEff);

  void copy_previous_nodes(struct node *cv, struct node *pv, int nsp);
  int assign_new_nodeID(struct node *v, int ns, int nN);

  void update_face_edges(struct node *v, struct segment *fe, int nfe);
  void update_inner_edges(struct node *v, struct isegment *ie, int tnie, int nie);
  void update_current_triangles(struct node *v, struct patch *t, int nT);

  int get_output_nodes(struct node *v, int nsp, int zID, int zDim, char* nodesFile);
  int get_output_edges(struct segment *fe, struct isegment *ie, int nfe, int tnie, int zID, int zDim, int ceid, int *feid, char* edgeFile);
  int get_output_triangles(struct patch *t, int nt, int zID, int ctid, char* trianglesFile);

  void copy_cNodes_2_pNodes(struct node *cv, struct node *pv, int nsp);

#ifdef __cplusplus
}
#endif



#endif /* SURFACE_MESH_GENERAL_MCA_LAYER_H_ */
