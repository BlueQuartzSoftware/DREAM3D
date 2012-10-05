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
#ifndef MMCSurfaceMeshingFilter_H_
#define MMCSurfaceMeshingFilter_H_

#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/VoxelDataContainer.h"
#include "MMCSurfaceMeshStructs.h"

/**
 * @class MMCSurfaceMeshingFilter MMCSurfaceMeshingFilter.h CMUSurfaceMeshing/Code/MMCSurfaceMeshingFilters/MMCSurfaceMeshingFilter.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class MMCSurfaceMeshingFilter : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(MMCSurfaceMeshingFilter);
    DREAM3D_STATIC_NEW_MACRO(MMCSurfaceMeshingFilter);
    DREAM3D_TYPE_MACRO_SUPER(MMCSurfaceMeshingFilter, AbstractFilter);

    virtual ~MMCSurfaceMeshingFilter();

    //------ Required Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(VoxelCoordsArrayName)

    //--------- Input Parameters


    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::SurfaceMeshingFilters; }

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const std::string getHumanLabel() { return "Multi-Material Marching Cubes"; }

    /**
    * @brief This method will instantiate all the end user settable options/parameters
    * for this filter
    */
    virtual void setupFilterParameters();

    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    */
    virtual void writeFilterParameters(AbstractFilterParametersWriter* writer);

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
    MMCSurfaceMeshingFilter();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param fields The number of fields
    * @param ensembles The number of ensembles
    */
    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

  private:
    int32_t*  m_GrainIds;
    float*    m_VoxelCoords;


    int createMesh();
    void get_neighbor_list(StructArray<Neighbor>::Pointer n, int ns, int nsp, int xDim, int yDim, int zDim);

    void initialize_nodes (int32_t* p, StructArray<Node>::Pointer v, int ns, double dx, double dy, double dz);
    void initialize_squares (StructArray<Neighbor>::Pointer neighbors, StructArray<Face>::Pointer sq, int ns, int nsp);
    int  get_number_fEdges (StructArray<Face>::Pointer sq, int32_t* p, StructArray<Neighbor>::Pointer n, int eT2d[20][8], int ns);
    void get_nodes_fEdges (StructArray<Face>::Pointer squares,
                                               int32_t* p,
                                               StructArray<Neighbor>::Pointer neighbors,
                                               StructArray<Node>::Pointer vertices,
                                               StructArray<Segment>::Pointer fedge,
                                               int eT2d[20][8],
                                               int nsT2d[20][8],
                                               int ns,
                                               int nsp,
                                               int xDim);
    int  get_square_index (int tns[4]);
    int  treat_anomaly (int tnst[4], int32_t* p1, StructArray<Neighbor>::Pointer neighbors, int sqid);
    void get_nodes (int cst, int ord, int nidx[2], int *nid, int nsp1, int xDim1);
    void get_spins (int32_t* p1, int cst, int ord, int pID[2], int *pSpin, int nsp1, int xDim1);
    int  get_number_triangles (int32_t* p,
                                StructArray<Face>::Pointer squares,
                                StructArray<Node>::Pointer vertices,
                                StructArray<Segment>::Pointer fedge,
                                int ns,
                                int nsp,
                                int xDim);
    int  get_number_case0_triangles (int *afe, StructArray<Node>::Pointer v1, StructArray<Segment>::Pointer fedge, int nfedge);
    int  get_number_case2_triangles (int *afe, StructArray<Node>::Pointer v1, StructArray<Segment>::Pointer fedge, int nfedge, int *afc, int nfctr);
    int  get_number_caseM_triangles (int *afe, StructArray<Node>::Pointer v1, StructArray<Segment>::Pointer fedge, int nfedge, int *afc, int nfctr);
    int  get_triangles (int32_t* p,
                         StructArray<Triangle>::Pointer triangles,
                         StructArray<Face>::Pointer squares,
                         StructArray<Node>::Pointer vertices,
                         StructArray<Segment>::Pointer fedge,
                         StructArray<Neighbor>::Pointer neighbors,
                         int ns,
                         int nsp,
                         int xDim);
    void get_case0_triangles (StructArray<Triangle>::Pointer triangles, int *afe, StructArray<Node>::Pointer v1, StructArray<Segment>::Pointer fedge,
            int nfedge, int tin, int *tout, double tcrd1[3], double tcrd2[3], int mcid);
    void get_case2_triangles (StructArray<Triangle>::Pointer triangles1, int *afe, StructArray<Node>::Pointer v1, StructArray<Segment>::Pointer fedge,
            int nfedge, int *afc, int nfctr, int tin, int *tout, double tcrd1[3], double tcrd2[3], int mcid);
    void get_caseM_triangles (StructArray<Triangle>::Pointer triangles1, int *afe, StructArray<Node>::Pointer v1, StructArray<Segment>::Pointer fedge,
            int nfedge, int *afc, int nfctr, int tin, int *tout, int ccn, double tcrd1[3], double tcrd2[3], int mcid);
    void find_edgePlace(double tvcrd1[3], double tvcrd2[3], double tvcrd3[3], int tw[3],
            double xh, double xl, double yh, double yl, double zh, double zl);
    int get_number_unique_inner_edges(StructArray<Triangle>::Pointer triangles, int nT);
    void get_unique_inner_edges(StructArray<Triangle>::Pointer triangles, StructArray<ISegment>::Pointer iedge, int nT, int nfedge);
    void update_triangle_sides_with_fedge(StructArray<Triangle>::Pointer triangles,
                                                               StructArray<Segment>::Pointer fedge,
                                                               StructArray<Face>::Pointer squares,
                                                               int nT,
                                                               int xDim,
                                                               int nsp);
    void arrange_spins (int32_t* p, StructArray<Triangle>::Pointer triangles, StructArray<Node>::Pointer v, int numT, int xDim, int nsp);
    void update_node_edge_kind(StructArray<Node>::Pointer v, StructArray<Segment>::Pointer fedge, StructArray<ISegment>::Pointer iedge, StructArray<Triangle>::Pointer triangles, int nT, int nfedge);
    int assign_new_nodeID (StructArray<Node>::Pointer v, int ns);
    //void get_output(StructArray<Node>::Pointer v, StructArray<Segment>::Pointer fedge, StructArray<ISegment>::Pointer iedge, StructArray<Triangle>::Pointer triangles, int ns, int nN, int nfe, int nie, int nT, MMC_MeshParameters* mp);

    void cleanupUnusedNodesTriangles(StructArray<Node>::Pointer nodes,
                                     StructArray<Triangle>::Pointer triangles);

    MMCSurfaceMeshingFilter(const MMCSurfaceMeshingFilter&); // Copy Constructor Not Implemented
    void operator=(const MMCSurfaceMeshingFilter&); // Operator '=' Not Implemented
};

#endif /* MMCSurfaceMeshingFilter_H_ */
