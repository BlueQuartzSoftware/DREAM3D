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
#ifndef UPDATE_EDGE_TRI_H_
#define UPDATE_EDGE_TRI_H_

#ifdef __cplusplus
extern "C"
{
#endif

int Update_Edge_Tri(const char* meshStatFile,
                    const char* outputDir,
                    const char* edgesFile,
                    const char* trianglesFile,
                    const char* nodesFile,
                    const char* nodesRawFile);


#ifdef __cplusplus
}
#endif


#endif /* UPDATE_EDGE_TRI_H_ */
