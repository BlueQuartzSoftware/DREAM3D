/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "SurfaceWinding.h"

#include <stdio.h>

#include <map>
#include <vector>
#include <iostream>

#include <boost/shared_array.hpp>


#include "MXA/Common/MXASetGetMacros.h"




SurfaceWinding::SurfaceWinding()
{
  // TODO Auto-generated constructor stub

}

SurfaceWinding::~SurfaceWinding()
{
  // TODO Auto-generated destructor stub
}


typedef int wVertIdx;
typedef int wGrainId;
typedef int wFaceIdx;

class wFace {
  public:
    MXA_SHARED_POINTERS(wFace)
    MXA_STATIC_NEW_MACRO(wFace);
    virtual ~wFace(){};
    wFaceIdx  idx;
    wVertIdx verts[3];
    wGrainId grains[2];
  protected:
    wFace() {}

  private:
    wFace(const wFace&); // Copy Constructor Not Implemented
    void operator=(const wFace&); // Operator '=' Not Implemented

};



typedef std::map<uint64_t, std::vector<int> > EdgeMap;
typedef std::map<uint32_t, std::vector<int> > GrainMap;

#define SURF_WIND1(i0, i1)\
    {\
  *e0 = f0->verts[i1]; *e1 = f0->verts[i0];\
    std::vector<int>& eFaces = edgeMap[edgeId];\
    for (size_t eFaceIdx = 0; eFaceIdx < eFaces.size(); ++eFaceIdx)    {\
      int eFace = eFaces[eFaceIdx];\
      if (eFace != faceId &&\
          ( allTriangles[eFace]->grains[0] == gid || allTriangles[eFace]->grains[1] == gid ) ) { \
        std::cout << "    eFace: " << eFace << std::endl; \
        ++neighCount; \
      }\
    }\
    }

#define DEBUG1  if (tData[0] == 141350 || tData[0] == 141355 || tData[0] == 141351 || tData[0] == 14138)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceWinding::analyzeWinding(int nNodes, int nTriangle, const std::string &triangleFile)
{
  // Open the triangles file for reading
  FILE* triFile = fopen(triangleFile.c_str(), "rb");
  int tData[6];

  EdgeMap edgeMap;
  GrainMap grainMap;

  uint64_t edgeId;
  int32_t* e0 = (int32_t*)(&edgeId);
  int32_t* e1 = e0+1;

  std::vector<wFace::Pointer> allTriangles(nTriangle);
  wFace::Pointer face = wFace::NullPointer();

  std::cout << "Reading File and Building Face/Vertex/Edge Relationships...." << std::endl;


  for (int i = 0; i < nTriangle; i++)
  {
    // Read from the Input Triangles Temp File
    fread(tData, sizeof(int), 6, triFile);
    face = allTriangles[tData[0]]; // Get the Triangle Object
    if (NULL == face.get())
    {
      wFace::Pointer f = wFace::New();
      face.swap(f);
      allTriangles[tData[0]] = face;
    }
    face = allTriangles[tData[0]];
    face->idx = tData[0];
    face->verts[0] = tData[1];
    face->verts[1] = tData[2];
    face->verts[2] = tData[3];
    face->grains[0] = tData[4];
    face->grains[1] = tData[5];

    // Create an Entry for the Edge Map
    DEBUG1
    {
      std::cout << "-- Face ID: " << tData[0] << std::endl;
    }
    // Update the Edge ID -> Face list Map
    *e0 = face->verts[0]; *e1 = face->verts[1];
    {
      DEBUG1
      {
        std::cout << "  " << *e0 << "\t" << *e1 << "\t" << edgeId << std::endl;
      }
      std::vector<int>& v = edgeMap[edgeId];
      v.push_back(tData[0]);
    }

    *e0 = face->verts[1]; *e1 = face->verts[2];
    {
      DEBUG1
      {
        std::cout << "  " << *e0 << "\t" << *e1 << "\t" << edgeId << std::endl;
      }
      std::vector<int>& v = edgeMap[edgeId];
      v.push_back(tData[0]);
    }

    *e0 = face->verts[2]; *e1 = face->verts[0];
    {
      DEBUG1
      {
        std::cout << "  " << *e0 << "\t" << *e1 << "\t" << edgeId << std::endl;
      }
      std::vector<int>& v = edgeMap[edgeId];
      v.push_back(tData[0]);
    }


    // Update the Grain ID -> Face List Map
    {
      std::vector<int>& v = grainMap[tData[4]];
      v.push_back(tData[0]);
    }
    {
      std::vector<int>& v = grainMap[tData[5]];
      v.push_back(tData[0]);
    }
  }

  std::cout << "Done Reading Triangles File" << std::endl;
  std::cout << " Edge Count: " << edgeMap.size() << std::endl;
  std::cout << " Triangle Count: " << nTriangle << std::endl;
  std::cout << " Node Count: " << nNodes << std::endl;
  std::cout << " Grain Count: " << grainMap.size() << std::endl;

  boost::shared_array<bool> visited;
  wFace::Pointer f0;
  wFace::Pointer f1;
  wFace::Pointer f2;
  wFace::Pointer f3;
  for (GrainMap::iterator grain = grainMap.begin(); grain != grainMap.end(); ++grain )
  {
    wGrainId gid = (*grain).first;
    std::vector<int>& gFaces = (*grain).second;
    size_t gFacesSize = gFaces.size();
    std::cout << "Analyzing Grain " << gid << std::endl;
    std::cout << " Face Count: " << gFacesSize << std::endl;
    // Create a self cleaning visited list
    visited.reset( new bool[gFacesSize] );
    ::memset(visited.get(), 0, gFacesSize);
    int grainCheckIdx = 1;
    // Loop on all the triangles
    for(size_t f = 0; f < gFacesSize; ++f)
    {
      int faceId = gFaces[f];
      if (visited[f] == true) { continue; }
      std::cout << " faceId: " << faceId << std::endl;
      f0 = allTriangles[faceId];
      grainCheckIdx = 1;
      if (gid == f0->grains[1])
      {
        grainCheckIdx = 0;
      }
      visited[f] = true;
      int neighCount = 0;

//      SURF_WIND1(0, 1)
//      SURF_WIND1(1, 2)
//      SURF_WIND1(2, 0)


#if 1
      size_t i0 = 0;
      size_t i1 = 1;
      *e0 = f0->verts[i0]; *e1 = f0->verts[i1];
      std::vector<int>& eFaces = edgeMap[edgeId];
      for (size_t eFaceIdx = 0; eFaceIdx < eFaces.size(); ++eFaceIdx)
      {
        int eFace = eFaces[eFaceIdx];
        f1 = allTriangles[eFace];
        if (eFace != faceId && allTriangles[eFace]->grains[grainCheckIdx] == gid )
        {
          std::cout << "    eFace: " << eFace << std::endl;
          ++neighCount;
        }
      }

      i0 = 1;
      i1 = 2;
      *e0 = f0->verts[i0]; *e1 = f0->verts[i1];
      eFaces = edgeMap[edgeId];
      for (size_t eFaceIdx = 0; eFaceIdx < eFaces.size(); ++eFaceIdx)
      {
        int eFace = eFaces[eFaceIdx];
        f2 = allTriangles[eFace];
        if (eFace != faceId && allTriangles[eFace]->grains[grainCheckIdx] == gid )
        {
          std::cout << "    eFace: " << eFace << std::endl;
          ++neighCount;
        }
      }

      i0 = 2;
      i1 = 0;
      *e0 = f0->verts[i0]; *e1 = f0->verts[i1];
      eFaces = edgeMap[edgeId];
      for (size_t eFaceIdx = 0; eFaceIdx < eFaces.size(); ++eFaceIdx)
      {
        int eFace = eFaces[eFaceIdx];
        f3 = allTriangles[eFace];
        if (eFace != faceId && allTriangles[eFace]->grains[grainCheckIdx] == gid )
        {
          std::cout << "    eFace: " << eFace << std::endl;
          ++neighCount;
        }
      }

#endif
      if(neighCount != 3)
      {
        std::cout << " ! neighCount != 3: " << neighCount << std::endl;
      }
    }

  }

}
