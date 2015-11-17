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

#include "TriangleOps.h"

#include "SIMPLib/Math/MatrixMath.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/Geometry/TriangleGeom.h"


namespace SM = DREAM3D::SurfaceMesh;
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TriangleOps::TriangleOps()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------4
TriangleOps::~TriangleOps()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int64_t> TriangleOps::findAdjacentTriangles(TriangleGeom::Pointer triangles,
                                                    int64_t triangleIndex,
                                                    DataArray<int32_t>::Pointer faceLabelsPtr,
                                                    int32_t label)
{
  QVector<int64_t> adjacentTris;

  int32_t* faceLabels = faceLabelsPtr->getPointer(0);

  // Get the Triangle Neighbor Structure
  ElementDynamicList::Pointer triNeighbors = triangles->getElementNeighbors();
  if(NULL == triNeighbors.get())
  {
    return adjacentTris;
  }

  // For the specific triangle that was passed, get its neighbor list
  uint16_t count = triNeighbors->getNumberOfElements(triangleIndex);
  int64_t* nList = triNeighbors->getElementListPointer(triangleIndex);

  if (count < 3)
  {
    qDebug() << "Triangle Neighbor List had only " << count << " neighbors. Must be at least 3." << "\n";
    BOOST_ASSERT(false);
  }
  else if (count == 3) // This triangle only has 3 neighbors so we are assuming all three have the same label set.
  {
    for (uint16_t n = 0; n < count; ++n)
    {
      adjacentTris.push_back(nList[n]);
    }
  }
  else
  {
    // Iterate over the indices to find triangles that match the label and are NOT the current triangle index
    for (uint16_t n = 0; n < count; ++n)
    {
      int32_t fl_0 = faceLabels[nList[n] * 2];
      int32_t fl_1 = faceLabels[nList[n] * 2 + 1];
      if ( (fl_0 == label || fl_1 == label)  && (nList[n] != triangleIndex) )
      {
        //  qDebug() << "    Found Adjacent Triangle: " << t->tIndex << "\n";
        adjacentTris.push_back(nList[n]);
        // ++index;
      }
    }
  }
  return adjacentTris;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleOps::getWindingIndices4(int64_t triangle[3],
                                     int32_t* faceLabel,
                                     int ids[4], int32_t label)
{
  int64_t idx = TriangleOps::getLabelIndex(faceLabel, label);

  if (idx == 1)
  {
    ids[0] = triangle[2];
    ids[1] = triangle[1];
    ids[2] = triangle[0];
    ids[3] = triangle[2];
  }
  else
  {
    ids[0] = triangle[0];
    ids[1] = triangle[1];
    ids[2] = triangle[2];
    ids[3] = triangle[0];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool TriangleOps::verifyWinding(int64_t source[3],
                                int64_t tri[3],
                                int32_t* faceLabelSource,
                                int32_t* faceLabelTri,
                                int32_t label)
{
  int ids[4];
  int nids[4];
  bool flipped = false;
  TriangleOps::getWindingIndices4(source, faceLabelSource, ids, label);
  TriangleOps::getWindingIndices4(tri, faceLabelTri, nids, label);
  //  int idx = 0;
  // There are 2 indices that are shared between the two triangles
  // Find them
  int i0, i1;
  //  bool flip = false;
  bool done = false;
  for (int i = 0; i < 3; ++i)
  {
    i0 = ids[i];
    i1 = ids[i + 1];
    for (int j = 0; j < 3; ++j)
    {
      if (i0 == nids[j + 1] && i1 == nids[j])
      {
        //    qDebug() << ">>>>>> Winding OK "<< tIndex << " <-> "<< tri->tIndex << "\n";
        done = true;
        break;
      }
      else if (i0 == nids[j] && i1 == nids[j + 1])
      {
        //   qDebug() << "!!!!!! Winding Bad " << "\n";
        done = true;
        TriangleOps::flipWinding(tri);
        flipped = true;
        break;
      }
    }
    if (done) { break; }
  }
  return flipped;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleOps::getLabelIndex(int32_t* triLabels, int label)
{
  if (label == triLabels[0]) { return 0; }
  if (label == triLabels[1]) { return 1; }
  return 2; // Error condition. Valid values are 0 or 1 since there are only 2 elements to the array.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<int64_t> TriangleOps::getNodeIndices(int64_t t[3], int32_t* faceLabel, int32_t label)
{
  QVector<int64_t> tNodes(3);
  int64_t idx = TriangleOps::getLabelIndex(faceLabel, label);
  if (idx == 1)
  {
    tNodes[0] = t[2];
    tNodes[1] = t[1];
    tNodes[2] = t[0];
  }
  else
  {
    tNodes[0] = t[0];
    tNodes[1] = t[1];
    tNodes[2] = t[2];
  }
  return tNodes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleOps::flipWinding(int64_t t[3])
{
  int tmp = t[0];
  t[0] = t[2];
  t[2] = tmp;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VectorType TriangleOps::computeNormal(float n0[3], float n1[3], float n2[3])
{
  double vert0[3];
  double vert1[3];
  double vert2[3];
  double u[3];
  double w[3];
  double normal[3];

  vert0[0] = static_cast<float>(n0[0]);
  vert0[1] = static_cast<float>(n0[1]);
  vert0[2] = static_cast<float>(n0[2]);

  vert1[0] = static_cast<float>(n1[0]);
  vert1[1] = static_cast<float>(n1[1]);
  vert1[2] = static_cast<float>(n1[2]);

  vert2[0] = static_cast<float>(n2[0]);
  vert2[1] = static_cast<float>(n2[1]);
  vert2[2] = static_cast<float>(n2[2]);

  //
  // Compute the normal
  u[0] = vert1[0] - vert0[0];
  u[1] = vert1[1] - vert0[1];
  u[2] = vert1[2] - vert0[2];

  w[0] = vert2[0] - vert0[0];
  w[1] = vert2[1] - vert0[1];
  w[2] = vert2[2] - vert0[2];

  MatrixMath::CrossProduct(u, w, normal);
  MatrixMath::Normalize3x1(normal);

  return VectorType(normal[0], normal[1], normal[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSet<int32_t> TriangleOps::generateUniqueLabels(DataArray<int32_t>::Pointer faceLabelsPtr)
{
  QSet<int32_t> uniqueLabels;
  int32_t* faceLabels = faceLabelsPtr->getPointer(0);

  size_t count = faceLabelsPtr->getNumberOfTuples();
  for (size_t i = 0; i < count; ++i)
  {
    uniqueLabels.insert(faceLabels[i * 2]);
    uniqueLabels.insert(faceLabels[i * 2 + 1]);
  }
  return uniqueLabels;
}

