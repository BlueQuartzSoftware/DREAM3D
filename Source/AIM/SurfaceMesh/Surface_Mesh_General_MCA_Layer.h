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

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif
#define num_neigh 26

// constants for procedure ran3
#define MBIG 1000000000
#define MSEED 161803398
#define MZ 0
#define FAC (1.0/MBIG)



#define EDGETABLE_2D_FILE  "edgeTable_2d.txt";
#define NEIGHSPIN_TABLE_FILE  "neighspinTable_2d.txt";
#define MESH_STAT_FILE "mesh_stat.txt"
#define NODES_RAW_FILE "nodes_raw.txt"




  int SurfaceMesh_MCALayer( int xnum, int ynum, int znum,
                   const char* outputDir,
                   const char* dxFile,
                   const char* meshStatFile,
                   const char* nodesRawFile );


#ifdef __cplusplus
}
#endif



#endif /* SURFACE_MESH_GENERAL_MCA_LAYER_H_ */
