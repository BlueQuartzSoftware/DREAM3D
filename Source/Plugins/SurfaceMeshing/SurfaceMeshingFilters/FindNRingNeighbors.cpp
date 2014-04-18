/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson,
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "FindNRingNeighbors.h"

#include <stdio.h>
#include <sstream>

#include "DREAM3DLib/Common/ScopedFileMonitor.hpp"
#include "DREAM3DLib/Utilities/DREAM3DEndian.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNRingNeighbors::FindNRingNeighbors() :
  m_TriangleId(-1),
  m_RegionId0(0),
  m_RegionId1(0),
  m_Ring(2),
  m_WriteBinaryFile(false),
  m_WriteConformalMesh(true)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNRingNeighbors::~FindNRingNeighbors()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNRingNeighbors::setRegionIds(int g, int r)
{
  m_RegionId0 = g;
  m_RegionId1 = r;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FaceArray::UniqueFaceIds_t& FindNRingNeighbors::getNRingTriangles()
{
  return m_NRingTriangles;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindNRingNeighbors::generate(FaceArray::Pointer trianglesPtr, int32_t* faceLabels)
{
  FaceArray::Face_t* triangles = trianglesPtr->getPointer(0);

  // Make sure we have the proper connectivity built
  Int32DynamicListArray::Pointer node2TrianglePtr = trianglesPtr->getFacesContainingVert();
  if (node2TrianglePtr.get() == NULL)
  {
    trianglesPtr->findFacesContainingVert();
    node2TrianglePtr = trianglesPtr->getFacesContainingVert();
  }

  // Figure out these boolean values for a sanity check
  bool check0 = faceLabels[m_TriangleId * 2] == m_RegionId0 && faceLabels[m_TriangleId * 2 + 1] == m_RegionId1;
  bool check1 = faceLabels[m_TriangleId * 2 + 1] == m_RegionId0 && faceLabels[m_TriangleId * 2] == m_RegionId1;

#if 1
  if ( check0 == false && check1 == false)
  {
    qDebug() << "FindNRingNeighbors Seed triangle ID does not have a matching Region ID for " << m_RegionId0 << " & " << m_RegionId1 << "\n";
    qDebug() << "Region Ids are: " << faceLabels[m_TriangleId * 2] << " & " << faceLabels[m_TriangleId * 2 + 1] << "\n";
    return;
  }
#endif


  // Add our seed triangle
  m_NRingTriangles.insert(m_TriangleId);

  for (int ring = 0; ring < m_Ring; ++ring)
  {
    // Make a copy of the 1 Ring Triangles that we just found so that we can use those triangles as the
    // seed triangles for the 2 Ring triangles
    FaceArray::UniqueFaceIds_t lcvTriangles(m_NRingTriangles);

    // Now that we have the 1 ring triangles, get the 2 Ring neighbors from that list
    for(FaceArray::UniqueFaceIds_t::iterator triIter = lcvTriangles.begin(); triIter != lcvTriangles.end(); ++triIter)
    {
      FaceArray::Face_t& face = triangles[*triIter];
      // For each node, get the triangle ids that the node belongs to
      for(int i = 0; i < 3; ++i)
      {
        // Get all the triangles for this Node id
        uint16_t tCount = node2TrianglePtr->getNumberOfElements(face.verts[i]);
        int32_t* data = node2TrianglePtr->getElementListPointer(face.verts[i]);

        // Copy all the triangles into our "2Ring" set which will be the unique set of triangle ids
        for(uint16_t t = 0; t < tCount; ++t)
        {
          int tid = data[t];
          check0 = faceLabels[tid * 2] == m_RegionId0 && faceLabels[tid * 2 + 1] == m_RegionId1;
          check1 = faceLabels[tid * 2 + 1] == m_RegionId0 && faceLabels[tid * 2] == m_RegionId1;
          if (check0 == true || check1 == true)
          {
            m_NRingTriangles.insert(static_cast<int>(tid) );
          }
        }
      }
    }
  }

}