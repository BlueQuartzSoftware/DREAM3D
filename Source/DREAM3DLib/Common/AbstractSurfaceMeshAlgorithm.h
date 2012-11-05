#ifndef _AbstractSurfaceMeshAlgorithm_H_
#define _AbstractSurfaceMeshAlgorithm_H_


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/SurfaceMeshStructs.h"
#include "DREAM3DLib/Common/StructArray.hpp"

class DREAM3DLib_EXPORT AbstractSurfaceMeshAlgorithm : public AbstractFilter
{

  public:
    DREAM3D_SHARED_POINTERS(AbstractSurfaceMeshAlgorithm)
    DREAM3D_STATIC_NEW_MACRO(AbstractSurfaceMeshAlgorithm)
    DREAM3D_TYPE_MACRO_SUPER(AbstractSurfaceMeshAlgorithm, AbstractFilter)

    virtual ~AbstractSurfaceMeshAlgorithm();



    DREAM3D_INSTANCE_PROPERTY(int32_t*, GrainIds)
    DREAM3D_INSTANCE_PROPERTY(int8_t*, SurfaceMeshNodeType)


  protected:
    AbstractSurfaceMeshAlgorithm();

    virtual int createMesh();


    /**
     * @brief get_neighbor_list
     * @param n
     * @param ns
     * @param nsp
     * @param xDim
     * @param yDim
     * @param zDim
     */
    virtual void get_neighbor_list(Neighbor* n, int ns, int nsp, int xDim, int yDim, int zDim);

    virtual void initialize_nodes (VoxelCoord* p,
                           Node* v,
                           int ns, float dx, float dy, float dz);
    virtual void initialize_squares (Neighbor* neighbors, Face* sq, int ns, int nsp);
    virtual int  get_number_fEdges (Face* sq, DataArray<int32_t>::Pointer points, Neighbor* n, int ns);
    virtual void get_nodes_fEdges (Face* sq,
                           DataArray<int32_t>::Pointer points,
                           Neighbor* neighbors,
                           Node* v,
                           Segment* e,
                           int ns,
                           int nsp,
                           int xDim);
    virtual int  get_square_index (int tns[4]);
    virtual int  treat_anomaly (int tnst[4], DataArray<int32_t>::Pointer points, Neighbor* n1, int sqid);
    virtual void get_nodes (int cst, int ord, int nidx[2], int *nid, int nsp1, int xDim1);
    virtual void get_spins (DataArray<int32_t>::Pointer points, int cst, int ord, int pID[2], int *pSpin, int nsp1, int xDim1);
    virtual int  get_number_triangles (DataArray<int32_t>::Pointer points,
                               Face* sq,
                               Node* v,
                               Segment* e,
                               int ns,
                               int nsp,
                               int xDim);
    virtual int  get_number_case0_triangles (int *afe, Node* v, Segment* e1, int nfedge);
    virtual int  get_number_case2_triangles (int *afe, Node* v1, Segment* fedge, int nfedge, int *afc, int nfctr);
    virtual int  get_number_caseM_triangles (int *afe, Node* v, Segment* e1, int nfedge, int *afc, int nfctr);
    virtual int  get_triangles (VoxelCoord* p,
                        Triangle* t,
                        Face* sq,
                        Node* v,
                        Segment* e,
                        Neighbor* neighbors,
                        int ns,
                        int nsp,
                        int xDim);
    virtual void get_case0_triangles (Triangle* t1, int *afe, Node* v1, Segment* e1,
                              int nfedge, int tin, int *tout, double tcrd1[3], double tcrd2[3], int mcid);
    virtual void get_case2_triangles (Triangle* triangles1, int *afe, Node* v1, Segment* fedge,
                              int nfedge, int *afc, int nfctr, int tin, int *tout, double tcrd1[3], double tcrd2[3], int mcid);
    virtual void get_caseM_triangles (Triangle* triangles1, int *afe, Node* v1, Segment* fedge,
                              int nfedge, int *afc, int nfctr, int tin, int *tout, int ccn, double tcrd1[3], double tcrd2[3], int mcid);
    virtual void find_edgePlace(double tvcrd1[3], double tvcrd2[3], double tvcrd3[3], int tw[3],
                        double xh, double xl, double yh, double yl, double zh, double zl);
    virtual int get_number_unique_inner_edges(Triangle* triangles, int nT);
    virtual void get_unique_inner_edges(Triangle* t, ISegment* ie, int nT, int nfedge);
    virtual void update_triangle_sides_with_fedge(Triangle* t,
                                          Segment* e,
                                          Face* sq,
                                          int nT,
                                          int xDim,
                                          int nsp);
    virtual void arrange_spins (DataArray<int32_t>::Pointer points,
                        VoxelCoord* pCoord,
                        Triangle* triangles,
                        Node* v,
                        int numT, int xDim, int nsp);
    virtual void update_node_edge_kind(Node* v, Segment* fe, ISegment* ie, Triangle* t, int nT, int nfedge);
    virtual int assign_new_nodeID (Node* v, DataArray<int32_t>::Pointer node_ids, int ns);
    virtual void generate_update_nodes_edges_array( DataArray<int32_t>::Pointer new_ids_for_nodes,
                                            DataArray<int8_t>::Pointer nodeKindPtr,
                                            StructArray<Node>::Pointer shortNodes,
                                            StructArray<Node>::Pointer vertices,
                                            StructArray<Triangle>::Pointer triangles,
                                            StructArray<Segment>::Pointer faceEdges,
                                            StructArray<ISegment>::Pointer internalEdges,
                                            int maxGrainId);

//    virtual void cleanupUnusedNodesTriangles(Node* nodes,
//                                     Triangle* triangles);



  private:

    int EdgeTable2D[20][8];
    int NSTable2D[20][8];


    AbstractSurfaceMeshAlgorithm(const AbstractSurfaceMeshAlgorithm&); // Copy Constructor Not Implemented
    void operator=(const AbstractSurfaceMeshAlgorithm&); // Operator '=' Not Implemented
};


#endif /* _AbstractSurfaceMeshAlgorithm_H_ */
