#include "TriangleOps.h"

#include "DREAM3DLib/Common/MatrixMath.h"
#include "DREAM3DLib/Common/ManagedPointerArray.hpp"
#include "DREAM3DLib/Common/SurfaceMeshDataContainer.h"

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
std::vector<int32_t> TriangleOps::findAdjacentTriangles(SurfaceMeshDataContainer* m,
                                                        StructArray<Triangle>::Pointer trianglesPtr,
                                                        int32_t triangleIndex,
                                                        int32_t label)
{
  IDataArray::Pointer edgeTrianglePtr = m->getCellData(DREAM3D::CellData::SurfaceMeshEdgeTriangles);
  ManagedPointerArray<int>* edgeTriangles = ManagedPointerArray<int>::SafePointerDownCast(edgeTrianglePtr.get());


  std::vector<int32_t> adjacentTris;

  StructArray<Triangle>& triangles = *(trianglesPtr.get());
  Triangle& triangle = triangles[triangleIndex];
  // Iterate over the 3 Edges of the triangle
  for (int i = 0; i < 3; ++i)
  {
    // Get the index of the edge data from the triangle
    int32_t edge_id = triangle.e_id[i];
    // Now lookup the number of triangles for that particular edge
    ManagedPointerArray<int32_t>::Data_t* edgeTriangleData = edgeTriangles->GetPointer(edge_id);
    for(size_t t = 0; t < edgeTriangleData->count; ++t)
    {
      Triangle& tri = triangles[edgeTriangleData->data[t]];
      if ( (tri.nSpin[0] == label || tri.nSpin[1] == label)  && (edgeTriangleData->data[t] != triangleIndex) )
      {
        //  std::cout << "    Found Adjacent Triangle: " << t->tIndex << std::endl;
        adjacentTris.push_back(edgeTriangleData->data[t]);
      }
    }



  }

  return adjacentTris;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleOps::getWindingIndices4(Triangle& triangle, int ids[4], int32_t label)
{
  int idx = TriangleOps::getLabelIndex(triangle, label);

  if (idx == 1)
  {
    ids[0] = triangle.node_id[2];
    ids[1] = triangle.node_id[1];
    ids[2] = triangle.node_id[0];
    ids[3] = triangle.node_id[2];
  }
  else
  {
    ids[0] = triangle.node_id[0];
    ids[1] = triangle.node_id[1];
    ids[2] = triangle.node_id[2];
    ids[3] = triangle.node_id[0];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool TriangleOps::verifyWinding(Triangle& source, Triangle& tri, int32_t label)
{
  int ids[4];
  int nids[4];
  bool flipped = false;
  TriangleOps::getWindingIndices4(source, ids, label);
  TriangleOps::getWindingIndices4(tri, nids, label);
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
        //    std::cout << ">>>>>> Winding OK "<< tIndex << " <-> "<< tri->tIndex << std::endl;
        done = true;
        break;
      }
      else if (i0 == nids[j] && i1 == nids[j + 1])
      {
        //std::cout << "!!!!!! Winding Bad " << std::endl;
        done = true;
        TriangleOps::flipWinding(tri);
        flipped = true;
        break;
      }
    }
    if (done) break;
  }
  return flipped;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TriangleOps::getLabelIndex(Triangle& t, int label)
{
  if (label == t.nSpin[0]) return 0;
  if (label == t.nSpin[1]) return 1;
  return 2; // Error condition. Valid values are 0 or 1 since there are only 2 elements to the array.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<int> TriangleOps::getNodeIndices(Triangle& t, int label)
{
  std::vector<int > tNodes(3);
  int idx = TriangleOps::getLabelIndex(t, label);
  if (idx == 1)
  {
    tNodes[0] = t.node_id[2];
    tNodes[1] = t.node_id[1];
    tNodes[2] = t.node_id[0];
  }
  else
  {
    tNodes[0] = t.node_id[0];
    tNodes[1] = t.node_id[1];
    tNodes[2] = t.node_id[2];
  }
  return tNodes;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TriangleOps::flipWinding(Triangle& t)
{
  int tmp = t.node_id[0];
  t.node_id[0] = t.node_id[2];
  t.node_id[2] = tmp;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VectorType TriangleOps::computeNormal(Node& n0, Node& n1, Node& n2)
{
  double vert0[3];
  double vert1[3];
  double vert2[3];
  double u[3];
  double w[3];
  double normal[3];

  vert0[0] = static_cast<float>(n0.coord[0]);
  vert0[1] = static_cast<float>(n0.coord[1]);
  vert0[2] = static_cast<float>(n0.coord[2]);

  vert1[0] = static_cast<float>(n1.coord[0]);
  vert1[1] = static_cast<float>(n1.coord[1]);
  vert1[2] = static_cast<float>(n1.coord[2]);

  vert2[0] = static_cast<float>(n2.coord[0]);
  vert2[1] = static_cast<float>(n2.coord[1]);
  vert2[2] = static_cast<float>(n2.coord[2]);

  //
  // Compute the normal
  u[0] = vert1[0] - vert0[0];
  u[1] = vert1[1] - vert0[1];
  u[2] = vert1[2] - vert0[2];

  w[0] = vert2[0] - vert0[0];
  w[1] = vert2[1] - vert0[1];
  w[2] = vert2[2] - vert0[2];

  MatrixMath::crossProduct(u, w, normal);
  MatrixMath::normalizeVector(normal);

  return VectorType(normal[0], normal[1], normal[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::set<int32_t> TriangleOps::generateUniqueLabels(StructArray<Triangle>::Pointer trianglesPtr)
{
  std::set<int32_t> uniqueLabels;
  Triangle* tris = trianglesPtr->GetPointer(0);

  for (size_t i =0; i < trianglesPtr->GetNumberOfTuples(); ++i)
  {
    uniqueLabels.insert(tris[i].nSpin[0]);
    uniqueLabels.insert(tris[i].nSpin[1]);
  }
  return uniqueLabels;
}

