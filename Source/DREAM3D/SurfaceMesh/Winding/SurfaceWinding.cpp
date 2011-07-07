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

#include <string.h>
#include <stdio.h>

#include <map>
#include <vector>
#include <iostream>
#include <queue>


#include <boost/shared_array.hpp>



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceWinding::SurfaceWinding()
{
  // TODO Auto-generated constructor stub

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceWinding::~SurfaceWinding()
{
  // TODO Auto-generated destructor stub
}


//#define DEBUG1  if (tData[0] == 141350 || tData[0] == 141355 || tData[0] == 141351 || tData[0] == 141438)


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SurfaceWinding::debugPrintConnectivity(int nNodes, int nTriangle, const std::string &triangleFile)
{
  int err = 0;
  std::cout << "--------------------------------------------------------------" << std::endl;


  // Open the triangles file for reading
  FILE* triFile = fopen(triangleFile.c_str(), "rb");
  int tData[6];

//  uint64_t edgeId;
//  int32_t* e0 = (int32_t*)(&edgeId);
//  int32_t* e1 = e0+1;


  masterTriangleList.clear();
  masterTriangleList.resize(nTriangle);
  TriangleType::Pointer face;
  LabelTriangleMapType labelTriangleMap;
  EdgeMapPointer m_EdgeMap = EdgeMapPointer(new EdgeMapType);
  EdgeMapType& eMap = *(m_EdgeMap.get());

  std::cout << "Reading File and Building Face/Vertex/Edge Relationships...." << std::endl;


  for (int i = 0; i < nTriangle; i++)
  {
    // Read from the Input Triangles Temp File
    fread(tData, sizeof(int), 6, triFile);
    face = masterTriangleList[tData[0]]; // Get the Triangle Object
    if (NULL == face.get())
    {
      TriangleType::Pointer f = TriangleType::New(0,0,0);
      face.swap(f);
      masterTriangleList[tData[0]] = face;
    }
    face = masterTriangleList[tData[0]];
    face->tIndex = tData[0];
    face->verts[0] = tData[1];
    face->verts[1] = tData[2];
    face->verts[2] = tData[3];
    face->labels[0] = tData[4];
    face->labels[1] = tData[5];

    // Create an Entry for the Edge Map
    m3c::Edge::Pointer e0 = m3c::Edge::New(face->verts[0], face->verts[1]);
    m3c::Edge::Pointer e = eMap[e0->getId()];
    if (NULL == e.get())
    {
      eMap[e0->getId()] = e0;
    }else{
      e0 = e;
    }
    e0->triangles.insert(tData[0]);

    m3c::Edge::Pointer e1 = m3c::Edge::New(face->verts[1], face->verts[2]);
    e = eMap[e1->getId()];
    if (NULL == e.get())
    {
      eMap[e1->getId()] = e1;
    }else{
      e1 = e;
    }
    e1->triangles.insert(tData[0]);

    m3c::Edge::Pointer e2 = m3c::Edge::New(face->verts[2], face->verts[0]);
    e = eMap[e2->getId()];
    if (NULL == e.get())
    {
      eMap[e2->getId()] = e2;
    }else{
      e2 = e;
    }
    e2->triangles.insert(tData[0]);


    face->edges[0] = e0;
    face->edges[1] = e1;
    face->edges[2] = e2;

    // This provides a seed triangle index to start at for each grain
    labelTriangleMap[tData[4]] = tData[0];
    labelTriangleMap[tData[5]] = tData[0];
  }

  fclose(triFile);

  std::cout << "Done Reading Triangles File" << std::endl;
//  std::cout << " Edge Count: " << edgeMap.size() << std::endl;
  std::cout << " Triangle Count: " << nTriangle << std::endl;
  std::cout << " Node Count: " << nNodes << std::endl;
//  std::cout << " Grain Count: " << grainMap.size() << std::endl;

  std::cout << "labelTriangleMap.size(): " << labelTriangleMap.size() << std::endl;

  float total = (float)(labelTriangleMap.size());
 // bool firstLabel = true;
  // Keeps a list of all the triangles that have been visited.
  std::vector<bool> masterVisited(masterTriangleList.size(), false);

  // Start with first triangle in the master list:
  int masterTriangleIndex = 0;
  TriangleType::Pointer t = masterTriangleList[masterTriangleIndex];
//  std::deque<int> labelsToVisit;

  // Get the first 2 labels to visit from the first triangle. We use these 2 labels
  // because this triangle shares these 2 labels and guarantees these 2 labels are
  // connected to each other.

  float curPercent = 0.0;
  int progressIndex = 0;
  for (LabelTriangleMapType::iterator cLabel = labelTriangleMap.begin(); cLabel != labelTriangleMap.end(); ++cLabel )
  {
    int currentLabel = cLabel->first;
  //  if (currentLabel != 1) { continue; }
    masterTriangleIndex = cLabel->second;
    t = masterTriangleList[masterTriangleIndex];

    if ( (progressIndex/total * 100.0f) > (curPercent) )
    {
      std::cout << "Verifying Winding: " << curPercent << "% Complete" << std::endl;
      curPercent += 5.0f;
    }
    ++progressIndex;
  //  std::cout << "Current Label: " << currentLabel << std::endl;
  //  int seedTriIndex = masterTriangleIndex;

    if (NULL == t.get() )
    {
      std::cout << "Could not find a triangle with the winding set. This should NOT happen" << std::endl;
      assert(1 == 0);
    }

    std::set<int> localVisited; // Keep track of which triangles have been visited
    std::deque<int> triangleDeque;
    triangleDeque.push_back(t->tIndex);

    while (triangleDeque.empty() == false)
    {
      t = masterTriangleList[triangleDeque.front()];
  //    std::cout << "tIndex = " << t->tIndex << std::endl;
      localVisited.insert(t->tIndex);
#if 0
      bool debug = ( t->tIndex == 163674 || t->tIndex == 163673 || t->tIndex == 163675 || t->tIndex == 163700);
      if (debug == true)
      {
        std::cout << "Debugging" << std::endl;
      } debug = false;
#endif
      TriangleListPtrType adjTris = findAdjacentTriangles(t, currentLabel);
      for ( TriangleListType::iterator adjTri = adjTris->begin(); adjTri != adjTris->end(); ++adjTri )
      {
        //  std::cout << "  ^ AdjTri index: " << (*adjTri)->tIndex << std::endl;
        if (masterVisited[(*adjTri)->tIndex] == false)
        {
          //   std::cout << "   * Checking Winding: " << (*adjTri)->tIndex << std::endl;
          t->verifyWinding( *adjTri, currentLabel);
        }


        if (localVisited.find((*adjTri)->tIndex) == localVisited.end()
          && find(triangleDeque.begin(), triangleDeque.end(), (*adjTri)->tIndex) == triangleDeque.end())
        {
          // std::cout << "   # Adding to Deque: " << (*adjTri)->tIndex << std::endl;
          triangleDeque.push_back((*adjTri)->tIndex);
          localVisited.insert((*adjTri)->tIndex);
          masterVisited[(*adjTri)->tIndex] = true;
        }
      }

      triangleDeque.pop_front();
    }

  }

  std::cout << "--------------------------------------------------------------" << std::endl;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SurfaceWinding::TriangleContainerPointerType SurfaceWinding::findAdjacentTriangles(
                                                                          TriangleType::Pointer triangle,
                                                                          Label label)
{
  TriangleContainerPointerType adjacentTris(new TriangleType::ContainerT);
  typedef m3c::Edge::Pointer EdgeType;
  // Get the 3 edges from the triangle

  TriangleType::Pointer t;
  // Iterate over the 3 Edges of the triangle
  for (int i = 0; i < 3; ++i)
  {
    // Get the current edge
    EdgeType e = triangle->edges[i];
    // Get the list of indices of the triangles that belong to that edge
    std::set<int> tIndices = e->triangles;
    // Iterate over the indices to find triangles that match the label and are NOT the current triangle index
    for (std::set<int>::iterator iter = tIndices.begin(); iter != tIndices.end(); ++iter )
    {
      t = masterTriangleList.at(*iter);
      if ( (t->labels[0] == label || t->labels[1] == label)
          && (t->tIndex != triangle->tIndex) )
      {
     //   std::cout << "    Found Adjacent Triangle: " << t->tIndex << std::endl;
        adjacentTris->push_back(t);
      }
    }
  }
  if (triangle->tIndex == 163674)
  {
    for(int i = 0; i < adjacentTris->size(); ++i)
    {
      std::cout << " tIndex: " << adjacentTris->at(i)->tIndex << std::endl;
    }
  }
  return adjacentTris;

}

#if 0


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

#endif
