
//
//  This code was written under United States Air Force Contract number
//                           FA8650-07-D-5800
//
/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _M3CEntireVolume_H_
#define _M3CEntireVolume_H_

#include <string>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/Common/SurfaceMeshStructs.h"

/**
 * @class M3CEntireVolume M3CEntireVolume.h DREAM3DLic/SurfaceMeshingFilters/M3CEntireVolume.h
 * @brief This filter was contributed by Dr. Sukbin Lee of Carnegi-Mellon University and uses a "MultiMaterial Marching
 * Cubes" algorithm originally proposed by Wu & Sullivan. This version of the code
 * considers the entire volume at once for the algorithm thus driving up the amount
 * of memory needed considerably. The trade off is speed. The algorithm is fairly
 * quick but at the expense of a large resident memory size during and after execution.
 * Multiple material marching cubes algorithm, Ziji Wu1, John M. Sullivan Jr2, International Journal for Numerical Methods in Engineering
 * Special Issue: Trends in Unstructured Mesh Generation, Volume 58, Issue 2, pages 189-207, 14 September 2003
 * @author
 * @date
 * @version 1.0
 */
class M3CEntireVolume : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    SIMPL_SHARED_POINTERS(M3CEntireVolume)
    SIMPL_STATIC_NEW_MACRO(M3CEntireVolume)
    SIMPL_TYPE_MACRO_SUPER(M3CEntireVolume, AbstractFilter)

    virtual ~M3CEntireVolume();

    //------ Created Surface Mesh Cell Data
    SIMPL_INSTANCE_STRING_PROPERTY(SurfaceMeshEdgesArrayName)
    SIMPL_INSTANCE_STRING_PROPERTY(SurfaceMeshInternalEdgesArrayName)

    //------ Required Cell Data

    //--------- Input Parameters
    SIMPL_INSTANCE_PROPERTY(bool, AddSurfaceLayer)

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const QString getGroupName() { return DREAM3D::FilterGroups::SurfaceMeshingFilters; }

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const QString getHumanLabel() { return "M3C Surface Meshing (Volume)"; }

    /**
    * @brief This method will instantiate all the end user settable options/parameters
    * for this filter
    */
    virtual void setupFilterParameters();

    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
    * @brief Reimplemented from @see AbstractFilter class
    */
    virtual void execute();

    /**
    * @brief This function runs some sanity checks on the DataContainer and inputs
    * in an attempt to ensure the filter can process the inputs.
    */
    virtual void preflight();

  protected:
    M3CEntireVolume();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param features The number of features
    * @param ensembles The number of ensembles
    */
    void dataCheck();

  private:
    DEFINE_PTR_WEAKPTR_DATAARRAY(int32_t, GrainIds)
    DEFINE_PTR_WEAKPTR_DATAARRAY(int8_t, SurfaceMeshNodeType)

    int createMesh();

    /**
     * @brief initialize_micro_from_grainIds
     * @param dims
     * @param res
     * @param fileDims
     * @param grainIds
     * @param points
     * @param voxelCoords
     * @return
     */
    int initialize_micro_from_grainIds(size_t dims[3], float res[3], size_t fileDims[3],
                                       int32_t* grainIds,
                                       DataArray<int32_t>::Pointer points,
                                       SurfaceMesh::M3C::VoxelCoord* point);

    /**
     * @brief get_neighbor_list
     * @param n
     * @param ns
     * @param nsp
     * @param xDim
     * @param yDim
     * @param zDim
     */
    void get_neighbor_list(SurfaceMesh::M3C::Node* n, int ns, int nsp, int xDim, int yDim, int zDim);

    void initialize_nodes (SurfaceMesh::M3C::VoxelCoord* p,
                           SurfaceMesh::M3C::Node* v,
                           int ns, float dx, float dy, float dz);
    void initialize_squares (SurfaceMesh::M3C::Neighbor* neighbors, SurfaceMesh::M3C::Face* sq, int ns, int nsp);
    int  get_number_fEdges (SurfaceMesh::M3C::Face* sq, DataArray<int32_t>::Pointer points, SurfaceMesh::M3C::Neighbor* n, int eT2d[20][8], int ns);
    void get_nodes_fEdges (SurfaceMesh::M3C::Face* sq,
                           DataArray<int32_t>::Pointer points,
                           SurfaceMesh::M3C::Neighbor* neighbors,
                           SurfaceMesh::M3C::Node* v,
                           SurfaceMesh::M3C::Segment* e,
                           int eT2d[20][8],
                           int nsT2d[20][8],
                           int ns,
                           int nsp,
                           int xDim);
    int  get_square_index (int tns[4]);
    int  treat_anomaly (int tnst[4], DataArray<int32_t>::Pointer points, SurfaceMesh::M3C::Neighbor* n1, int sqid);
    void get_nodes (int cst, int ord, int nidx[2], int* nid, int nsp1, int xDim1);
    void get_spins (DataArray<int32_t>::Pointer points, int cst, int ord, int pID[2], int* pSpin, int nsp1, int xDim1);
    int  get_number_triangles (DataArray<int32_t>::Pointer points,
                               SurfaceMesh::M3C::Face* sq,
                               SurfaceMesh::M3C::Node* v,
                               SurfaceMesh::M3C::Segment* e,
                               int ns,
                               int nsp,
                               int xDim);
    int  get_number_case0_triangles (int* afe, SurfaceMesh::M3C::Node* v, SurfaceMesh::M3C::Segment* e1, int nfedge);
    int  get_number_case2_triangles (int* afe, SurfaceMesh::M3C::Node* v1, SurfaceMesh::M3C::Segment* fedge, int nfedge, int* afc, int nfctr);
    int  get_number_caseM_triangles (int* afe, SurfaceMesh::M3C::Node* v, SurfaceMesh::M3C::Segment* e1, int nfedge, int* afc, int nfctr);
    int  get_triangles (SurfaceMesh::M3C::VoxelCoord* p,
                        SurfaceMesh::M3C::Triangle* t, int* mCubeID,
                        SurfaceMesh::M3C::Face* sq,
                        SurfaceMesh::M3C::Node* v,
                        SurfaceMesh::M3C::Segment* e,
                        SurfaceMesh::M3C::Neighbor* neighbors,
                        int ns,
                        int nsp,
                        int xDim);
    void get_case0_triangles (SurfaceMesh::M3C::Triangle* t1, int* mCubeID, int* afe, SurfaceMesh::M3C::Node* v1, SurfaceMesh::M3C::Segment* e1,
                              int nfedge, int tin, int* tout, double tcrd1[3], double tcrd2[3], int mcid);
    void get_case2_triangles (SurfaceMesh::M3C::Triangle* triangles1, int* mCubeID, int* afe, SurfaceMesh::M3C::Node* v1, SurfaceMesh::M3C::Segment* fedge,
                              int nfedge, int* afc, int nfctr, int tin, int* tout, double tcrd1[3], double tcrd2[3], int mcid);
    void get_caseM_triangles (SurfaceMesh::M3C::Triangle* triangles1, int* mCubeID, int* afe, SurfaceMesh::M3C::Node* v1, SurfaceMesh::M3C::Segment* fedge,
                              int nfedge, int* afc, int nfctr, int tin, int* tout, int ccn, double tcrd1[3], double tcrd2[3], int mcid);
    void find_edgePlace(double tvcrd1[3], double tvcrd2[3], double tvcrd3[3], int tw[3],
                        double xh, double xl, double yh, double yl, double zh, double zl);
    int get_number_unique_inner_edges(SurfaceMesh::M3C::Triangle* triangles, int* mCubeID, int nT);
    void get_unique_inner_edges(SurfaceMesh::M3C::Triangle* t, int* mCubeID, ISegment* ie, int nT, int nfedge);
    void update_triangle_sides_with_fedge(SurfaceMesh::M3C::Triangle* t, int* mCubeID,
                                          SurfaceMesh::M3C::Segment* e,
                                          SurfaceMesh::M3C::Face* sq,
                                          int nT,
                                          int xDim,
                                          int nsp);
    void arrange_spins (DataArray<int32_t>::Pointer points,
                        SurfaceMesh::M3C::VoxelCoord* pCoord,
                        SurfaceMesh::M3C::Triangle* triangles,
                        SurfaceMesh::M3C::Node* v,
                        int numT, int xDim, int nsp);
    void update_node_edge_kind(SurfaceMesh::M3C::Node* v, SurfaceMesh::M3C::Segment* fe, ISegment* ie, SurfaceMesh::M3C::Triangle* t, int nT, int nfedge);
    int assign_new_nodeID (SurfaceMesh::M3C::Node* v, DataArray<int32_t>::Pointer node_ids, int ns);
    void generate_update_nodes_edges_array( DataArray<int32_t>::Pointer new_ids_for_nodes,
                                            DataArray<int8_t>::Pointer nodeKindPtr,
                                            StructArray<SurfaceMesh::M3C::Node>::Pointer shortNodes,
                                            StructArray<SurfaceMesh::M3C::Node>::Pointer vertices,
                                            StructArray<SurfaceMesh::M3C::SurfaceMesh::M3C::Face_t>::Pointer triangles,
                                            StructArray<SurfaceMesh::M3C::Segment>::Pointer faceEdges,
                                            StructArray<ISegment>::Pointer internalEdges,
                                            int maxGrainId);

    //void get_output(SurfaceMesh::M3C::Node* v, SurfaceMesh::M3C::Segment* fedge, ISegment* iedge, SurfaceMesh::M3C::Triangle* triangles, int ns, int nN, int nfe, int nie, int nT, MMC_MeshParameters* mp);

    void cleanupUnusedNodesTriangles(SurfaceMesh::M3C::Node* nodes,
                                     SurfaceMesh::M3C::Triangle* triangles);

    M3CEntireVolume(const M3CEntireVolume&); // Copy Constructor Not Implemented
    void operator=(const M3CEntireVolume&); // Operator '=' Not Implemented
};

#endif /* _M3CEntireVolume_H_ */

