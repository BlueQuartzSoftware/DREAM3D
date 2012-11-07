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
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#ifndef _M3CSliceBySlice_H_
#define _M3CSliceBySlice_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

//#include <assert.h>
//#include <stdio.h>
//#include <time.h>
//#include <stdlib.h>

//#include <cstddef>
#include <vector>
#include <string>
//#include <iostream>
//#include <cmath>
//#include <fstream>
//#include <list>
//#include <algorithm>
//#include <numeric>
//#include <map>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"


//#include "SurfaceMeshing/IO/SMTempFile.hpp"
//#include "SurfaceMeshing/IO/SMStlWriter.h"
//#include "SurfaceMeshing/Meshing/Patch.h"
//#include "SurfaceMeshing/Meshing/Face.h"
//#include "SurfaceMeshing/Meshing/Neighbor.h"
//#include "SurfaceMeshing/Meshing/Node.h"
//#include "SurfaceMeshing/Meshing/Segment.h"
//#include "SurfaceMeshing/Meshing/SharedEdge.h"


class M3CSliceBySlice : public AbstractFilter
{

  public:
    DREAM3D_SHARED_POINTERS(M3CSliceBySlice)
    DREAM3D_STATIC_NEW_MACRO(M3CSliceBySlice)
    DREAM3D_TYPE_MACRO_SUPER(M3CSliceBySlice, AbstractFilter)

    virtual ~M3CSliceBySlice();

    //------ Required Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(SurfaceMeshNodeTypeArrayName)

    //DREAM3D_INSTANCE_STRING_PROPERTY(InputFile)
    DREAM3D_INSTANCE_PROPERTY(bool, DeleteTempFiles)
//    DREAM3D_INSTANCE_PROPERTY(bool, WriteSTLFile)
//    DREAM3D_INSTANCE_STRING_PROPERTY(StlOutputDirectory)
//    DREAM3D_INSTANCE_STRING_PROPERTY(StlFilePrefix)

//    DREAM3D_INSTANCE_STRING_PROPERTY(VtkOutputFile)
//    DREAM3D_INSTANCE_PROPERTY(bool, WriteBinaryVTKFiles)
//    DREAM3D_INSTANCE_PROPERTY(bool, WriteConformalMesh)

//    DREAM3D_INSTANCE_PROPERTY(SMTempFile::Pointer, NodesFile)
//    DREAM3D_INSTANCE_PROPERTY(SMTempFile::Pointer, TrianglesFile)

    virtual void preflight();

    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::SurfaceMeshingFilters; }
    virtual const std::string getHumanLabel() { return "M3C Surface Mesh (Slice at a time)"; }

    virtual void setupFilterParameters();
    virtual void writeFilterParameters(AbstractFilterParametersWriter* writer);


    void execute();

  protected:
    M3CSliceBySlice();

    /**
     * @brief get_neighbor_list
     * @param NSP
     * @param NS
     * @param wrappedDims
     * @param neighborsPtr
     * @param neighborCSiteIdPtr
     */
    void get_neighbor_list(int NSP, int NS, int wrappedDims[],
                            StructArray<Neighbor>::Pointer neighborsPtr,
                            DataArray<int32_t>::Pointer neighborCSiteIdPtr);

    /**
     * @brief initialize_nodes
     * @param NSP
     * @param zID
     * @param wrappedDims
     * @param res
     * @param cVertexPtr
     * @param voxelsPtr
     * @param cVertexNodeIdPtr
     * @param cVertexNodeKindPtr
     */
    void initialize_nodes(int NSP, int zID, int *wrappedDims, float* res,
                          StructArray<Node>::Pointer cVertexPtr,
                          DataArray<int32_t>::Pointer voxelsPtr,
                          DataArray<int32_t>::Pointer cVertexNodeIdPtr,
                          DataArray<int8_t>::Pointer cVertexNodeKindPtr );

    /**
     * @brief initialize_squares
     * @param zID
     * @param NSP
     * @param cSquarePtr
     * @param neighborsPtr
     */
    void initialize_squares(int zID, int NSP,
                                          StructArray<Face>::Pointer cSquarePtr,
                                          StructArray<Neighbor>::Pointer neighborsPtr);


    /**
     * @brief get_nodes_Edges
     * @param NSP
     * @param zID
     * @param wrappedDims
     * @param cSquarePtr
     * @param voxelsPtr
     * @param cEdgePtr
     * @param cVertexNodeKindPtr
     * @param neighborsPtr
     * @return
     */
    size_t get_nodes_Edges(int NSP, int zID, int *wrappedDims,
                           StructArray<Face>::Pointer cSquarePtr,
                           DataArray<int32_t>::Pointer voxelsPtr,
                           StructArray<Segment>::Pointer cEdgePtr,
                           DataArray<int8_t>::Pointer cVertexNodeKindPtr,
                           StructArray<Neighbor>::Pointer neighborsPtr);
    /**
     * @brief get_triangles
     * @param NSP
     * @param wrappedDims
     * @param cSquarePtr
     * @param voxelsPtr
     * @param cVertexNodeKindPtr
     * @param cEdgePtr
     * @param cTrianglePtr
     * @return
     */
    int get_triangles(int NSP, int* wrappedDims,
                      StructArray<Face>::Pointer cSquarePtr,
                      DataArray<int32_t>::Pointer voxelsPtr,
                      DataArray<int8_t>::Pointer cVertexNodeKindPtr,
                      StructArray<Segment>::Pointer cEdgePtr,
                      StructArray<Patch>::Pointer cTrianglePtr);

    /**
     * @brief arrange_grainnames
     * @param numT
     * @param zID
     * @param NSP
     * @param wrappedDims
     * @param cTrianglePtr
     * @param cVertexPtr
     * @param voxelsPtr
     * @param cVertexNodeIdPtr
     * @param neighborsPtr
     */
    void arrange_grainnames(int numT, int zID, int NSP, int* wrappedDims, float *res,
                            StructArray<Patch>::Pointer cTrianglePtr,
                            StructArray<Node>::Pointer cVertexPtr,
                            DataArray<int32_t>::Pointer voxelsPtr,
                            StructArray<Neighbor>::Pointer neighborsPtr);

    /**
     * @brief find_xcoord
     * @param xDim
     * @param xRes
     * @return
     */
    float find_xcoord(int index, int xDim, float xRes);

    /**
     * @brief find_ycoord
     * @param index
     * @param xDim
     * @param yDim
     * @param yRes
     * @return
     */
    float find_ycoord(int index, int xDim, int yDim, float yRes);

    /**
     * @brief find_zcoord
     * @param xDim
     * @param yDim
     * @param zRes
     * @return
     */
    float find_zcoord(int index, int xDim, int yDim, float zRes);

    /**
     * @brief treat_anomaly
     * @param tNSt
     * @param zID1
     * @param voxelsPtr
     * @param neighborsPtr
     * @return
     */
    int treat_anomaly(int tNSt[4], int zID1,
                                    DataArray<int32_t>::Pointer voxelsPtr,
                                    StructArray<Neighbor>::Pointer neighborsPtr);

    /**
     * @brief get_nodes
     * @param cst
     * @param ord
     * @param nidx
     * @param nid
     * @param NSP
     * @param wrappedDims
     */
    void get_nodes(int cst, int ord, int nidx[2], int *nid, int NSP, int *wrappedDims);

    /**
     * @brief get_grainnames
     * @param cst
     * @param ord
     * @param pID
     * @param pgrainname
     * @param NSP
     * @param wrappedDims
     * @param voxelsPtr
     */
    void get_grainnames(int cst, int ord, int pID[2], int* pgrainname, int NSP,
                        int* wrappedDims,
                        DataArray<int32_t>::Pointer voxelsPtr);



    void get_case0_triangles(int site, int *ae, int nedge,
                                          int tin, int* tout,
                                          StructArray<Segment>::Pointer cEdgePtr,
                                          StructArray<Patch>::Pointer cTrianglePtr);
    void get_case2_triangles(int site, int *ae, int nedge, int *afc,
                                          int nfctr, int tin, int *tout,
                                          StructArray<Segment>::Pointer cEdgePtr,
                                          StructArray<Patch>::Pointer cTrianglePtr);
    void get_caseM_triangles(int site, int *afe, int nfedge, int *afc, int nfctr,
                             int tin, int *tout, int ccn,
                             StructArray<Segment>::Pointer cEdgePtr,
                             StructArray<Patch>::Pointer cTrianglePtr);

    /**
     * @brief assign_nodeID
     * @param nN
     * @param NSP
     * @param cVertexNodeIdPtr
     * @param cVertexNodeKindPtr
     * @return
     */
    int assign_nodeID(int nN, int NSP,
                      DataArray<int32_t>::Pointer cVertexNodeIdPtr,
                      DataArray<int8_t>::Pointer cVertexNodeKindPtr);

    int get_square_index(int tns[4]);


    void update_current_triangles(int nT);

    // The next two methods are for analyzing the winding
    // void analyzeWinding();
    // std::vector<int> findAdjacentTriangles(Triangle *triangle, int label);

    /**
     * @brief writeNodesFile
     * @param zID
     * @param cNodeID
     * @param NSP
     * @param nodesFile
     * @param cVertexPtr
     * @param cVertexNodeIdPtr
     * @param cVertexNodeKindPtr
     * @return
     */
    int writeNodesFile(int zID, int cNodeID, int NSP,
                                    const std::string &nodesFile,
                                    StructArray<Node>::Pointer cVertexPtr,
                                    DataArray<int32_t>::Pointer cVertexNodeIdPtr,
                                    DataArray<int8_t>::Pointer cVertexNodeKindPtr);

    /**
     * @brief writeTrianglesFile
     * @param zID
     * @param ctid
     * @param trianglesFile
     * @param nt
     * @param cTrianglePtr
     * @param cVertexNodeIdPtr
     * @return
     */
    int writeTrianglesFile(int zID, int ctid,
                           const std::string &trianglesFile, int nt,
                           StructArray<Patch>::Pointer cTrianglePtr,
                           DataArray<int32_t>::Pointer cVertexNodeIdPtr);
  //  int writeSTLFiles(int nTriangle, std::map<int, meshing::SMStlWriter::Pointer> &gidToSTLWriter);

    void readNodesTriangles(const std::string &nodesFile, const std::string &trianglesFile);

  private:
    int32_t* m_GrainIds;
    int8_t* m_SurfaceMeshNodeType;
    int numgrains;


//    typedef std::map<uint64_t, meshing::SharedEdge::Pointer> EdgeMapType;
//    typedef int Label;
//    typedef std::map<Label, int> LabelTriangleMapType;

//    EdgeMapType eMap;
//    LabelTriangleMapType labelTriangleMap;

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    M3CSliceBySlice(const M3CSliceBySlice&); // Copy Constructor Not Implemented
    void operator=(const M3CSliceBySlice&); // Operator '=' Not Implemented
};

#endif
