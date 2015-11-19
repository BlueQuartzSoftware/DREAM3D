/* ============================================================================
#include "SIMPLib/Geometry/TriangleGeom.h"
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


#include "FindNRingNeighbors.h"

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
void FindNRingNeighbors::setRegionIds(int32_t g, int32_t r)
{
  m_RegionId0 = g;
  m_RegionId1 = r;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindNRingNeighbors::UniqueFaceIds_t& FindNRingNeighbors::getNRingTriangles()
{
  return m_NRingTriangles;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t FindNRingNeighbors::generate(TriangleGeom::Pointer triangleGeom, int32_t* faceLabels)
{
  int64_t* triangles = triangleGeom->getTriPointer(0);
  int32_t err = 0;

  //Clear out all the previous triangles.
  m_NRingTriangles.clear();

  // Make sure we have the proper connectivity built
  ElementDynamicList::Pointer node2TrianglePtr = triangleGeom->getElementsContainingVert();
  if (node2TrianglePtr.get() == NULL)
  {
    err = triangleGeom->findElementsContainingVert();
    if (err < 0)
    {
      return err;
    }
    node2TrianglePtr = triangleGeom->getElementsContainingVert();
  }

  // Figure out these boolean values for a sanity check
  bool check0 = faceLabels[m_TriangleId * 2] == m_RegionId0 && faceLabels[m_TriangleId * 2 + 1] == m_RegionId1;
  bool check1 = faceLabels[m_TriangleId * 2 + 1] == m_RegionId0 && faceLabels[m_TriangleId * 2] == m_RegionId1;

#if 1
  if ( check0 == false && check1 == false)
  {
    qDebug() << "FindNRingNeighbors Seed triangle ID does not have a matching Region ID for " << m_RegionId0 << " & " << m_RegionId1 << "\n";
    qDebug() << "Region Ids are: " << faceLabels[m_TriangleId * 2] << " & " << faceLabels[m_TriangleId * 2 + 1] << "\n";
    return err;
  }
#endif

  // Add our seed triangle
  m_NRingTriangles.insert(m_TriangleId);

  for (int64_t ring = 0; ring < m_Ring; ++ring)
  {
    // Make a copy of the 1 Ring Triangles that we just found so that we can use those triangles as the
    // seed triangles for the 2 Ring triangles
    UniqueFaceIds_t lcvTriangles(m_NRingTriangles);

    // Now that we have the 1 ring triangles, get the 2 Ring neighbors from that list
    for (UniqueFaceIds_t::iterator triIter = lcvTriangles.begin(); triIter != lcvTriangles.end(); ++triIter)
    {
      int64_t triangleIdx = *triIter;
      // For each node, get the triangle ids that the node belongs to
      for(int32_t i = 0; i < 3; ++i)
      {
        // Get all the triangles for this Node id
        uint16_t tCount = node2TrianglePtr->getNumberOfElements(triangles[triangleIdx * 3 + i]);
        int64_t* data = node2TrianglePtr->getElementListPointer(triangles[triangleIdx * 3 + i]);

        // Copy all the triangles into our "2Ring" set which will be the unique set of triangle ids
        for (uint16_t t = 0; t < tCount; ++t)
        {
          int64_t tid = data[t];
          check0 = faceLabels[tid * 2] == m_RegionId0 && faceLabels[tid * 2 + 1] == m_RegionId1;
          check1 = faceLabels[tid * 2 + 1] == m_RegionId0 && faceLabels[tid * 2] == m_RegionId1;
          if (check0 == true || check1 == true)
          {
            m_NRingTriangles.insert(tid);
          }
        }
      }
    }
  }
  return err;
}
