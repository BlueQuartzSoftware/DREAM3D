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
#ifndef _SurfaceMeshFilter_H
#define _SurfaceMeshFilter_H

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

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Field.h"
#include "DREAM3DLib/Common/DREAM3DRandom.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/SurfaceMeshingFilters/SMTempFile.hpp"
#include "DREAM3DLib/SurfaceMeshingFilters/SMStlWriter.h"
#include "DREAM3DLib/SurfaceMeshingFilters/Meshing/Patch.h"
#include "DREAM3DLib/SurfaceMeshingFilters/Meshing/Face.h"
#include "DREAM3DLib/SurfaceMeshingFilters/Meshing/Neighbor.h"
#include "DREAM3DLib/SurfaceMeshingFilters/Meshing/Node.h"
#include "DREAM3DLib/SurfaceMeshingFilters/Meshing/Segment.h"
#include "DREAM3DLib/SurfaceMeshingFilters/Meshing/SharedEdge.h"




class DREAM3DLib_EXPORT SurfaceMeshFilter : public AbstractFilter
{

  public:
    DREAM3D_SHARED_POINTERS(SurfaceMeshFilter);
    DREAM3D_STATIC_NEW_MACRO(SurfaceMeshFilter);
    DREAM3D_TYPE_MACRO_SUPER(SurfaceMeshFilter, AbstractFilter);

    virtual ~SurfaceMeshFilter();

    DREAM3D_INSTANCE_STRING_PROPERTY(InputFile);
    DREAM3D_INSTANCE_PROPERTY(bool, DeleteTempFiles);
    DREAM3D_INSTANCE_PROPERTY(bool, WriteSTLFile);
    DREAM3D_INSTANCE_STRING_PROPERTY(OutputDirectory);
    DREAM3D_INSTANCE_STRING_PROPERTY(OutputFilePrefix);

    DREAM3D_INSTANCE_PROPERTY(SMTempFile::Pointer, NodesFile);
    DREAM3D_INSTANCE_PROPERTY(SMTempFile::Pointer, TrianglesFile);


    void execute();

  protected:
    SurfaceMeshFilter();

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
    void analyzeWinding();
    std::vector<int> findAdjacentTriangles(meshing::Patch::Pointer triangle, int label);

    int writeNodesFile(int zID, int cNodeID, const std::string &NodesRawFile);
    int writeTrianglesFile(int zID, int ctid, const std::string &trianglesFile, int nt);
    int writeSTLFiles(int nTriangle, std::map<int, meshing::SMStlWriter::Pointer> &gidToSTLWriter);

  private:
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

    meshing::Neighbor* neigh; // contains nearest neighbor information...

    int* voxels; // contains voxel information...

    meshing::Face* cSquare; // contains square information...

    meshing::Node* cVertex; // contains edges on square faces for open loops...

    std::vector<meshing::Segment::Pointer> cEdge; // contains edges on square faces for open loops...
    std::vector<meshing::Patch::Pointer> cTriangle;

    typedef std::map<uint64_t, meshing::SharedEdge::Pointer> EdgeMapType;
    typedef int Label;
    typedef std::map<Label, int> LabelTriangleMapType;

    EdgeMapType eMap;
    LabelTriangleMapType labelTriangleMap;

    SurfaceMeshFilter(const SurfaceMeshFilter&); // Copy Constructor Not Implemented
    void operator=(const SurfaceMeshFilter&); // Operator '=' Not Implemented
};

#endif
