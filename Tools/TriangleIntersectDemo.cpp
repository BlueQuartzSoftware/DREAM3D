
#include "SIMPLib/Common/SIMPLArray.hpp"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"
#include "SIMPLib/Math/GeometryMath.h"

#include <iostream>

constexpr float k_Epsilon = 1e-8;
constexpr uint32_t width = 310;
constexpr uint32_t height = 150;

using Vec3f = FloatVec3Type;

inline float deg2rad(const float& deg)
{
  return deg * M_PI / 180;
}

inline float clamp(const float& lo, const float& hi, const float& v)
{
  return std::max(lo, std::min(hi, v));
}

/**
 * @brief This version of the rayTriangleIntersect algorithm uses a more traditional algorithm to
 * determine if a point is inside a triangle. This version should be more computationally
 * efficient than the other version
 * @param rayOrigin
 * @param rayDirection
 * @param v0 First Vertex of Triangle
 * @param v1  Second Vertex of Triangle
 * @param v2  Third Vertex of Triangle
 * @param t t part of Barycentric Coord
 * @param u u part of Barycentric Coord
 * @param v v part of Barycentric Coord
 * @return
 */
bool RayTriangleIntersect(const Vec3f& rayOrigin, const Vec3f& rayDirection, const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, Vec3f& barycentricCoord)
{
  // compute plane's normal
  Vec3f v0v1 = v1 - v0;
  Vec3f v0v2 = v2 - v0;
  // no need to normalize
  Vec3f N = v0v1.crossProduct(v0v2); // N
  float denom = N.dotProduct(N);

  // Step 1: finding P

  // check if ray and plane are parallel ?
  float NdotRayDirection = N.dotProduct(rayDirection);

  if(fabs(NdotRayDirection) < k_Epsilon) // almost 0
  {
    return false;
  } // they are parallel so they don't intersect !

  // compute d parameter using equation 2
  float d = -N.dotProduct(v0);

  // compute t (equation 3)
  barycentricCoord[2] = -(N.dotProduct(rayOrigin) + d) / NdotRayDirection;

  // check if the triangle is in behind the ray
  if(barycentricCoord[2] < 0)
  {
    return false;
  } // the triangle is behind

  // compute the intersection point using equation 1
  Vec3f P = rayOrigin + (barycentricCoord[2] * rayDirection);

  // Step 2: inside-outside test
  Vec3f C; // vector perpendicular to triangle's plane

  // edge 0
  Vec3f edge0 = v1 - v0;
  Vec3f vp0 = P - v0;
  C = edge0.crossProduct(vp0);
  if(N.dotProduct(C) < 0)
  {
    return false;
  } // P is on the right side

  // edge 1
  Vec3f edge1 = v2 - v1;
  Vec3f vp1 = P - v1;
  C = edge1.crossProduct(vp1);
  if((barycentricCoord[0] = N.dotProduct(C)) < 0)
  {
    return false;
  } // P is on the right side

  // edge 2
  Vec3f edge2 = v0 - v2;
  Vec3f vp2 = P - v2;
  C = edge2.crossProduct(vp2);
  if((barycentricCoord[1] = N.dotProduct(C)) < 0)
  {
    return false; // P is on the right side;
  }

  barycentricCoord[0] /= denom;
  barycentricCoord[1] /= denom;
  barycentricCoord[2] = 1 - barycentricCoord[0] - barycentricCoord[1];

  return true; // this ray hits the triangle
}

/* Adapted from https://github.com/erich666/jgt-code/blob/master/Volume_02/Number_1/Moller1997a/raytri.c */
/* code rewritten to do tests on the sign of the determinant */
/* the division is before the test of the sign of the det    */
/* and one CROSS has been moved out from the if-else if-else
* @param rayOrigin
* @param rayDirection
* @param v0 First Vertex of Triangle
* @param v1  Second Vertex of Triangle
* @param v2  Third Vertex of Triangle
* @param t t part of Barycentric Coord
* @param u u part of Barycentric Coord
* @param v v part of Barycentric Coord
* @return If the point is within the triangle
*/
bool RayTriangleIntersect2(const Vec3f& orig, const Vec3f& dir, const Vec3f& vert0, const Vec3f& vert1, const Vec3f& vert2, Vec3f& bcoord)
{
  Vec3f edge1, edge2, tvec, pvec, qvec;
  double det, inv_det;

  /* find vectors for two edges sharing vert0 */
  edge1 = vert1 - vert0;
  edge2 = vert2 - vert0;

  /* begin calculating determinant - also used to calculate U parameter */
  pvec = dir.crossProduct(edge2);

  /* if determinant is near zero, ray lies in plane of triangle */
  det = edge1.dotProduct(pvec);

  /* calculate distance from vert0 to ray origin */
  tvec = orig - vert0;

  inv_det = 1.0 / det;

  qvec = tvec.crossProduct(edge1);

  if(det > k_Epsilon)
  {
    bcoord[0] = tvec.dotProduct(pvec);
    if(bcoord[0] < 0.0 || bcoord[0] > det)
    {
      return false;
    }

    /* calculate V parameter and test bounds */
    // bcoord[1] = DOT(dir, qvec);
    bcoord[1] = dir.dotProduct(qvec);
    if(bcoord[1] < 0.0 || bcoord[0] + bcoord[1] > det)
    {
      return false;
    }
  }
  else if(det < -k_Epsilon)
  {
    /* calculate U parameter and test bounds */
    bcoord[0] = tvec.dotProduct(pvec);
    if(bcoord[0] > 0.0 || bcoord[0] < det)
    {
      return false;
    }

    /* calculate V parameter and test bounds */
    bcoord[1] = dir.dotProduct(qvec);
    if(bcoord[1] > 0.0 || bcoord[0] + bcoord[1] < det)
    {
      return false;
    }
  }
  else
  {
    return false;
  } /* ray is parallel to the plane of the triangle */

 // float t = edge2.dotProduct(qvec) * inv_det;
  float u = bcoord[0] * inv_det;
  float v = bcoord[1] * inv_det;

  bcoord[0] = 1 - u - v;
  bcoord[1] = u;
  bcoord[2] = v;

  return true;
}

//-----------------------------------------------------------------------------
TriangleGeom::Pointer CreateTestTriangleMesh()
{
  SharedVertexList::Pointer sharedVertexList = SharedVertexList::CreateArray(6, {3}, QString("SHARED_VERTEX_LIST"), true);
  sharedVertexList->setTuple(0, {0.0F, 0.0F, 0.0F});
  sharedVertexList->setTuple(1, {100.0F, 0.0F, 0.0F});
  sharedVertexList->setTuple(2, {200.0F, 0.0F, 0.0F});
  sharedVertexList->setTuple(3, {300.0F, 100.0F, 0.0F});
  sharedVertexList->setTuple(4, {150.0F, 100.0F, 0.0F});
  sharedVertexList->setTuple(5, {50.0F, 100.0F, 0.0F});
  TriangleGeom::Pointer triangleGeom = TriangleGeom::CreateGeometry(4, sharedVertexList, "SHARED_TRIANGLE_LIST", true);
  SharedTriList::Pointer triangles = triangleGeom->getTriangles();
  triangles->setTuple(0, {0, 1, 5});
  triangles->setTuple(1, {1, 4, 5});
  triangles->setTuple(2, {1, 2, 4});
  triangles->setTuple(3, {2, 3, 4});

  return triangleGeom;
}

SharedVertexList::Pointer CreateTestPointCloud()
{
  // In this example we ASSUME we are on a 2D XY Plane where all of the Z values of
  // each point is going to be set to a constant value -1.0. We are just going to generate
  // a point cloud. The real code would just get the SharedVertexList from the Vertex
  // geometry that is the XY positions from the DataBase.

  SharedVertexList::Pointer pointCloud = SharedVertexList::CreateArray(width * height, {3}, "DataBase Positions", true);
  size_t index = 0;
  for(uint32_t y = 0; y < height; ++y)
  {
    for(uint32_t x = 0; x < width; ++x)
    {
      index = (width * y) + x;
      Vec3f dir(static_cast<float>(x), static_cast<float>(y), 0.0F);
      pointCloud->setTuple(index, dir.data());
    }
  }
  return pointCloud;
}

//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  // Create a triangle mesh, yours would come from the CT5 data
  TriangleGeom::Pointer ct5Geom = CreateTestTriangleMesh();
  SharedTriList::Pointer ct5Triangles = ct5Geom->getTriangles();

  // Create some displacement values that would have been read from the CT5 file and
  // fill with some test data
  FloatArrayType::Pointer dxValues = FloatArrayType::CreateArray(6, QString("dX Values"), true);
  FloatArrayType::Pointer dyValues = FloatArrayType::CreateArray(6, QString("dY Values"), true);
  for(size_t i = 0; i < 6; i++)
  {
    dxValues->setValue(i, static_cast<float>(i * 25.0F));
    dyValues->setValue(i, static_cast<float>(i * 50.0F));
  }

  // Get the XY Positions from the DataBase
  SharedVertexList::Pointer scanHeadPositionPtr = CreateTestPointCloud();

  SharedVertexList& scanHeadPosition = *scanHeadPositionPtr;
  // Loop over all the XY Data Base positions (Scan Head values)
  // This loop should be the loop that is parallelized
  for(size_t scanHeadVertIndex = 0; scanHeadVertIndex < scanHeadPositionPtr->getNumberOfTuples(); scanHeadVertIndex++)
  {
    // Create the origin of the ray that will be used to see if we intercept the triangle by
    // just grabbing the current scanHeadPoint and setting its Z value to ZERO. This MAY have
    // to change based on what values are coming in from the ScanHead.
    Vec3f rayOrigin(scanHeadPosition[scanHeadVertIndex * 3], scanHeadPosition[scanHeadVertIndex * 3 + 1], 1.0F);
    Vec3f rayDirection(0.0F, 0.0F, -1.0F);
    // rayDirection.normalize();

    Vec3f barycentricCoord(0.0F, 0.0F, 0.0F);

    // Loop over all the CT5 Triangles
    size_t numCt5Triangles = ct5Geom->getNumberOfTris();
    // Create these reusable variables to save the reallocation each time through the loop
    Vec3f v0;
    Vec3f v1;
    Vec3f v2;
    for(size_t triIndex = 0; triIndex < numCt5Triangles; triIndex++)
    {
      barycentricCoord = {0.0F, 0.0F, 0.0F};
      std::array<size_t, 3> triVertIndices;
      // Get the Vertex Coordinates for each of the 3 vertices
      ct5Geom->getVertCoordsAtTri(triIndex, v0.data(), v1.data(), v2.data());
      // Get the vertex Indices from the triangle
      ct5Geom->getVertsAtTri(triIndex, triVertIndices.data());
      bool inTriangle = RayTriangleIntersect2(rayOrigin, rayDirection, v0, v1, v2, barycentricCoord);
      if(inTriangle)
      {
        // Linear Interpolate dx and dy values using the barycentric coordinates
        float dx = (barycentricCoord[0] * dxValues->getValue(triVertIndices[0])) + (barycentricCoord[1] * dxValues->getValue(triVertIndices[1])) +
                   (barycentricCoord[2] * dxValues->getValue(triVertIndices[2]));
        float dy = (barycentricCoord[0] * dyValues->getValue(triVertIndices[0])) + (barycentricCoord[1] * dyValues->getValue(triVertIndices[1])) +
                   (barycentricCoord[2] * dyValues->getValue(triVertIndices[2]));
#if 0
// This is just a huge debugging print statement...
        std::cout << triIndex << ": "
                  << "(" << v0[0] << "," << v0[1] << "," << v0[2] << ")"
                  << ", "
                  << "(" << v1[0] << "," << v1[1] << "," << v2[2] << ")"
                  << ", "
                  << "(" << v2[0] << "," << v2[1] << "," << v2[2] << ")"
                  << "    Vertex Weights: (" << dxValues->getValue(triVertIndices[0]) << "," << dxValues->getValue(triVertIndices[1]) << "," << dxValues->getValue(triVertIndices[2]) << ")"

                  << "    ScanHead Pos: (" << scanHeadPosition[scanHeadVertIndex * 3] << ", " << scanHeadPosition[scanHeadVertIndex * 3 + 1] << ")"
                  << "    Barycentric: (" << barycentricCoord[0] << ", " << barycentricCoord[1] << ", " << barycentricCoord[2] << ")"
                  << "    dx dy: " << dx << ", " << dy << std::endl;
#endif
#if 0
// Nearest Neighbor, just look for the largest component of the barycentric coordinates
        if(barycentricCoord[0] >= barycentricCoord[1] && barycentricCoord[0] >= barycentricCoord[2])
        {
          dx = dxValues->getValue(0);
          dy = dyValues->getValue(0);
        }
        else if(barycentricCoord[1] >= barycentricCoord[0] && barycentricCoord[1] >= barycentricCoord[2])
        {
          dx = dxValues->getValue(1);
          dy = dyValues->getValue(1);
        }
        else if(barycentricCoord[2] >= barycentricCoord[0] && barycentricCoord[2] >= barycentricCoord[1])
        {
          dx = dxValues->getValue(2);
          dy = dyValues->getValue(2);
        }
#endif
        // APPLY The interpolated Transform to the DataBase vertex
        scanHeadPosition[scanHeadVertIndex * 3] += dx;
        scanHeadPosition[scanHeadVertIndex * 3 + 1] += dy;

        break; // We found the triangle so break out of the inner loop
      }
    }
  }

  return 0;
}
