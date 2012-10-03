#error DO NOT INCLUDE THIS FILE ANY MORE
/*
 * SurfaceMeshGeneralMCABoundedMulti.h
 *
 *  Created on: Jun 14, 2012
 *      Author: mjackson
 */

#ifndef SURFACEMESHGENERALMCABOUNDEDMULTI_H_
#define SURFACEMESHGENERALMCABOUNDEDMULTI_H_

#include "SurfaceMeshStructs.h"

#include <string>



namespace CMUSurfaceMeshing
{

// function prototypes...
int execute();

#if 0
int32_t* initialize_micro_from_dx_file ( size_t &xDim, size_t &yDim, size_t &zDim,
                        double dx, double dy, double dz,
                        const std::string ifn);
#endif
void get_neighbor_list (neighbor *n, int ns, int nsp, int xDim, int yDim, int zDim);

#if 0
void read_edge_neighspin_table (int eT2d[20][8], int nsT2d[20][8]);
#endif

void initialize_nodes (int32_t* p, Node *v, int ns, double dx, double dy, double dz);
void initialize_squares (neighbor *n, Face *sq, int ns, int nsp);
int  get_number_fEdges (Face *sq, int32_t* p, neighbor *n, int eT2d[20][8], int ns);
void get_nodes_fEdges (Face *sq, int32_t* p, neighbor *n, Node *v, Segment *e,
          int eT2d[20][8], int nsT2d[20][8], int ns, int nsp, int xDim);
int  get_square_index (int tns[4]);
int  treat_anomaly (int tnst[4], int32_t* p1, neighbor *n1, int sqid);
void get_nodes (int cst, int ord, int nidx[2], int *nid, int nsp1, int xDim1);
void get_spins (int32_t* p1, int cst, int ord, int pID[2], int *pSpin, int nsp1, int xDim1);
int  get_number_triangles (int32_t* p, Face *sq, Node *v, Segment *e,
         int ns, int nsp, int xDim);
int  get_number_case0_triangles (int *afe, Node *v1, Segment *e1, int nfedge);
int  get_number_case2_triangles (int *afe, Node *v1, Segment *e1, int nfedge, int *afc, int nfctr);
int  get_number_caseM_triangles (int *afe, Node *v1, Segment *e1, int nfedge, int *afc, int nfctr);
int  get_triangles (int32_t* p, Patch *t, Face *sq, Node *v, Segment *e,
        neighbor *n, int ns, int nsp, int xDim);
void get_case0_triangles (Patch *t1, int *afe, Node *v1, Segment *e1,
        int nfedge, int tin, int *tout, double tcrd1[3], double tcrd2[3], int mcid);
void get_case2_triangles (Patch *t1, int *afe, Node *v1, Segment *e1,
        int nfedge, int *afc, int nfctr, int tin, int *tout, double tcrd1[3], double tcrd2[3], int mcid);
void get_caseM_triangles (Patch *t1, int *afe, Node *v1, Segment *e1,
        int nfedge, int *afc, int nfctr, int tin, int *tout, int ccn, double tcrd1[3], double tcrd2[3], int mcid);
void find_edgePlace(double tvcrd1[3], double tvcrd2[3], double tvcrd3[3], int tw[3],
        double xh, double xl, double yh, double yl, double zh, double zl);
int get_number_unique_inner_edges(Patch *t, int nT);
void get_unique_inner_edges(Patch *t, isegment *ie, int nT, int nfedge);
void update_triangle_sides_with_fedge(Patch *t, Segment *e, Face *sq, int nT, int xDim, int nsp);
void arrange_spins (int32_t* p, Patch *t, Node *v, int numT, int xDim, int nsp);
void update_node_edge_kind(Node *v, Segment *fe, isegment *ie, Patch *t, int nT, int nfedge);
int assign_new_nodeID (Node *v, int ns);

#if 0
/**
 *
 * @param v
 * @param fe
 * @param ie
 * @param t
 * @param ns
 * @param nN
 * @param nfe
 * @param nie
 * @param nT
 * @param outBaseName
 */
void get_output (Node *v, Segment *fe, isegment *ie, Patch *t, int ns, int nN, int nfe, int nie, int nT,
                 MeshParameters* mp);
#endif
}

#endif /* SURFACEMESHGENERALMCABOUNDEDMULTI_H_ */
