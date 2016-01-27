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

#include "GeometryMath.h"

#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Math/MatrixMath.h"
#include "SIMPLib/Utilities/SIMPLibRandom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GeometryMath::GeometryMath()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GeometryMath::~GeometryMath()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float GeometryMath::CosThetaBetweenVectors(const float a[3], const float b[3])
{
  float norm1 = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
  float norm2 = sqrt(b[0] * b[0] + b[1] * b[1] + b[2] * b[2]);
  if (norm1 == 0 || norm2 == 0)
  {
    return 1.0;
  }
  return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]) / (norm1 * norm2);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float GeometryMath::AngleBetweenVectors(const float a[3], const float b[3])
{
  float norm1 = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
  float norm2 = sqrt(b[0] * b[0] + b[1] * b[1] + b[2] * b[2]);
  float cosAng = (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]) / (norm1 * norm2);
  SIMPLibMath::boundF(cosAng, -1, 1);
  return acos(cosAng);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GeometryMath::PointInBox(const float p[3], const float ll[3], const float ur[3])
{
  if ((ll[0] <= p[0]) && (p[0] <= ur[0]) && (ll[1] <= p[1]) && (p[1] <= ur[1]) && (ll[2] <= p[2]) && (p[2] <= ur[2]))
  {
    return true;
  }
  else
  {
    return false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GeometryMath::RayIntersectsBox(const float* p,
                                    const float* q,
                                    const float* ll,
                                    const float* ur)
{
  if ((ll[0] > p[0]) && (ll[0] > q[0]))
  {
    return false;
  }
  else if ((ur[0] < p[0]) && (ur[0] < q[0]))
  {
    return false;
  }
  else if ((ll[1] > p[1]) && (ll[1] > q[1]))
  {
    return false;
  }
  else if ((ur[1] < p[1]) && (ur[1] < q[1]))
  {
    return false;
  }
  else if ((ll[2] > p[2]) && (ll[2] > q[2]))
  {
    return false;
  }
  else if ((ur[2] < p[2]) && (ur[2] < q[2]))
  {
    return false;
  }
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float GeometryMath::LengthOfRayInBox(const float* p,
                                     const float* q,
                                     const float* ll,
                                     const float* ur)
{
  float length = 0.0;
  float frac = 0.0;

  float x1 = p[0];
  float y1 = p[1];
  float z1 = p[2];
  float x2 = q[0];
  float y2 = q[1];
  float z2 = q[2];

  float delX = x2 - x1;
  float delY = y2 - y1;
  float delZ = z2 - z1;

  float c1x = ll[0];
  float c1y = ll[1];
  float c1z = ll[2];
  float c2x = ur[0];
  float c2y = ur[1];
  float c2z = ur[2];

  //clip ray by min x face of box
  if (x1 < c1x && x2 > c1x)
  {
    frac = ((c1x - x1) / delX);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if (x1 > c1x && x2 < c1x)
  {
    frac = ((c1x - x2) / delX);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if (x1 < c1x && x2 < c1x)
  {
    return 0.0;
  }
  //clip ray by min y face of box
  if (y1 < c1y && y2 > c1y)
  {
    frac = ((c1y - y1) / delY);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if (y1 > c1y && y2 < c1y)
  {
    frac = ((c1y - y2) / delY);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if (y1 < c1y && y2 < c1y)
  {
    return 0.0;
  }
  //clip ray by min z face of box
  if (z1 < c1z && z2 > c1z)
  {
    frac = ((c1z - z1) / delZ);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if (z1 > c1z && z2 < c1z)
  {
    frac = ((c1z - z2) / delZ);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if (z1 < c1z && z2 < c1z)
  {
    return 0.0;
  }
  //clip ray by max x face of box
  if (x1 > c2x && x2 < c2x)
  {
    frac = ((c2x - x1) / delX);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if (x1 < c2x && x2 > c2x)
  {
    frac = ((c2x - x2) / delX);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if (x1 > c2x && x2 > c2x)
  {
    return 0.0;
  }
  //clip ray by max y face of box
  if (y1 > c2y && y2 < c2y)
  {
    frac = ((c2y - y1) / delY);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if (y1 < c2y && y2 > c2y)
  {
    frac = ((c2y - y2) / delY);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if (y1 > c2y && y2 > c2y)
  {
    return 0.0;
  }
  //clip ray by max z face of box
  if (z1 > c2z && z2 < c2z)
  {
    frac = ((c2z - z1) / delZ);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if (z1 < c2z && z2 > c2z)
  {
    frac = ((c2z - z2) / delZ);
    x1 = x1 + (frac * delX);
    y1 = y1 + (frac * delY);
    z1 = z1 + (frac * delZ);
  }
  else if (z1 > c2z && z2 > c2z)
  {
    return 0.0;
  }

  length = ((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)) + ((z2 - z1) * (z2 - z1));
  length = sqrt(length);

  return length;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::GenerateRandomRay(float length, float ray[3])
{
  float w, t;

  SIMPL_RANDOMNG_NEW();

  ray[2] = (2.0 * rg.genrand_real1()) - 1.0;
  t = (SIMPLib::Constants::k_2Pi * rg.genrand_real1());
  w = sqrt(1.0 - (ray[2] * ray[2]));
  ray[0] = w * cos(t);
  ray[1] = w * sin(t);
  ray[0] *= length;
  ray[1] *= length;
  ray[2] *= length;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindBoundingBoxOfVertices(VertexGeom::Pointer verts, float* ll, float* ur)
{
  ll[0] = 100000000.0;
  ll[1] = 100000000.0;
  ll[2] = 100000000.0;
  ur[0] = 0.0;
  ur[1] = 0.0;
  ur[2] = 0.0;

  float* v = verts->getVertexPointer(0);
  int64_t numVerts = verts->getNumberOfVertices();
  for (int i = 0; i < numVerts; i++)
  {
    if (v[i] < ll[0])
    {
      ll[0] = v[i];
    }
    if (v[i] > ur[0])
    {
      ur[0] = v[i];
    }
    if (v[i + 1] < ll[1])
    {
      ll[1] = v[i + 1];
    }
    if (v[i + 1] > ur[1])
    {
      ur[1] = v[i + 1];
    }
    if (v[i + 2] < ll[2])
    {
      ll[2] = v[i + 2];
    }
    if (v[i + 2] > ur[2])
    {
      ur[2] = v[i + 2];
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindBoundingBoxOfFaces(TriangleGeom::Pointer faces, Int32Int32DynamicListArray::ElementList faceIds, float* ll, float* ur)
{
  ll[0] = 100000000.0;
  ll[1] = 100000000.0;
  ll[2] = 100000000.0;
  ur[0] = 0.0;
  ur[1] = 0.0;
  ur[2] = 0.0;

  float facell[3], faceur[3];

  int numFaces = faceIds.ncells;
  int32_t* faceId = faceIds.cells;
  for (int i = 0; i < numFaces; i++)
  {
    FindBoundingBoxOfFace(faces, faceId[i], facell, faceur);
    if (facell[0] < ll[0])
    {
      ll[0] = facell[0];
    }
    if (facell[1] < ll[1])
    {
      ll[1] = facell[1];
    }
    if (facell[2] < ll[2])
    {
      ll[2] = facell[2];
    }
    if (faceur[0] > ur[0])
    {
      ur[0] = faceur[0];
    }
    if (faceur[1] > ur[1])
    {
      ur[1] = faceur[1];
    }
    if (faceur[2] > ur[2])
    {
      ur[2] = faceur[2];
    }
  }

  if (faceIds.ncells == 0)
  {
    ll[0] = 0.0;
    ll[1] = 0.0;
    ll[2] = 0.0;
    ur[0] = 0.0;
    ur[1] = 0.0;
    ur[2] = 0.0;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindBoundingBoxOfRotatedFaces(TriangleGeom::Pointer faces, Int32Int32DynamicListArray::ElementList faceIds, float g[3][3], float* ll, float* ur)
{
  ll[0] = 100000000.0;
  ll[1] = 100000000.0;
  ll[2] = 100000000.0;
  ur[0] = 0.0;
  ur[1] = 0.0;
  ur[2] = 0.0;

  float facell[3], faceur[3];

  int numFaces = faceIds.ncells;
  int32_t* faceId = faceIds.cells;
  for (int i = 0; i < numFaces; i++)
  {
    FindBoundingBoxOfRotatedFace(faces, faceId[i], g, facell, faceur);
    if (facell[0] < ll[0])
    {
      ll[0] = facell[0];
    }
    if (facell[1] < ll[1])
    {
      ll[1] = facell[1];
    }
    if (facell[2] < ll[2])
    {
      ll[2] = facell[2];
    }
    if (faceur[0] > ur[0])
    {
      ur[0] = faceur[0];
    }
    if (faceur[1] > ur[1])
    {
      ur[1] = faceur[1];
    }
    if (faceur[2] > ur[2])
    {
      ur[2] = faceur[2];
    }
  }

  if (numFaces == 0)
  {
    ll[0] = 0.0;
    ll[1] = 0.0;
    ll[2] = 0.0;
    ur[0] = 0.0;
    ur[1] = 0.0;
    ur[2] = 0.0;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindBoundingBoxOfFace(TriangleGeom::Pointer faces, int faceId, float* ll, float* ur)
{
  float a[3], b[3], c[3];

  faces->getVertCoordsAtTri(faceId, a, b, c);
  ll[0] = a[0];
  ur[0] = a[0];
  ll[1] = a[1];
  ur[1] = a[1];
  ll[2] = a[2];
  ur[2] = a[2];
  if (b[0] < ll[0])
  {
    ll[0] = b[0];
  }
  if (b[0] > ur[0])
  {
    ur[0] = b[0];
  }
  if (b[1] < ll[1])
  {
    ll[1] = b[1];
  }
  if (b[1] > ur[1])
  {
    ur[1] = b[1];
  }
  if (b[2] < ll[2])
  {
    ll[2] = b[2];
  }
  if (b[2] > ur[2])
  {
    ur[2] = b[2];
  }
  if (c[0] < ll[0])
  {
    ll[0] = c[0];
  }
  if (c[0] > ur[0])
  {
    ur[0] = c[0];
  }
  if (c[1] < ll[1])
  {
    ll[1] = c[1];
  }
  if (c[1] > ur[1])
  {
    ur[1] = c[1];
  }
  if (c[2] < ll[2])
  {
    ll[2] = c[2];
  }
  if (c[2] > ur[2])
  {
    ur[2] = c[2];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindBoundingBoxOfRotatedFace(TriangleGeom::Pointer faces, int faceId, float g[3][3], float* ll, float* ur)
{
  float a[3], b[3], c[3];
  float p1[3], p2[3], p3[3];
  float p1r[3], p2r[3], p3r[3];

  faces->getVertCoordsAtTri(faceId, a, b, c);
  p1[0] = a[0];
  p1[1] = a[1];
  p1[2] = a[2];
  p2[0] = b[0];
  p2[1] = b[1];
  p2[2] = b[2];
  p3[0] = c[0];
  p3[1] = c[1];
  p3[2] = c[2];
  MatrixMath::Multiply3x3with3x1(g, p1, p1r);
  MatrixMath::Multiply3x3with3x1(g, p2, p2r);
  MatrixMath::Multiply3x3with3x1(g, p3, p3r);
  ll[0] = p1r[0];
  ur[0] = p1r[0];
  ll[1] = p1r[1];
  ur[1] = p1r[1];
  ll[2] = p1r[2];
  ur[2] = p1r[2];
  if (p2r[0] < ll[0])
  {
    ll[0] = p2r[0];
  }
  if (p2r[0] > ur[0])
  {
    ur[0] = p2r[0];
  }
  if (p2r[1] < ll[1])
  {
    ll[1] = p2r[1];
  }
  if (p2r[1] > ur[1])
  {
    ur[1] = p2r[1];
  }
  if (p2r[2] < ll[2])
  {
    ll[2] = p2r[2];
  }
  if (p2r[2] > ur[2])
  {
    ur[2] = p2r[2];
  }
  if (p3r[0] < ll[0])
  {
    ll[0] = p3r[0];
  }
  if (p3r[0] > ur[0])
  {
    ur[0] = p3r[0];
  }
  if (p3r[1] < ll[1])
  {
    ll[1] = p3r[1];
  }
  if (p3r[1] > ur[1])
  {
    ur[1] = p3r[1];
  }
  if (p3r[2] < ll[2])
  {
    ll[2] = p3r[2];
  }
  if (p3r[2] > ur[2])
  {
    ur[2] = p3r[2];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindPlaneNormalVector(const float a[3], const float b[3], const float c[3], float n[3])
{
  float ab[3], ac[3];

  ab[0] = b[0] - a[0];
  ab[1] = b[1] - a[1];
  ab[2] = b[2] - a[2];

  ac[0] = c[0] - a[0];
  ac[1] = c[1] - a[1];
  ac[2] = c[2] - a[2];

  MatrixMath::CrossProduct(ab, ac, n);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindPlaneNormalVector(const double a[3], const double b[3], const double c[3], double n[3])
{
  double ab[3], ac[3];

  ab[0] = b[0] - a[0];
  ab[1] = b[1] - a[1];
  ab[2] = b[2] - a[2];

  ac[0] = c[0] - a[0];
  ac[1] = c[1] - a[1];
  ac[2] = c[2] - a[2];

  MatrixMath::CrossProduct(ab, ac, n);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindPlaneCoefficients(const float a[3], const float b[3], const float c[3], float n[3], float& d)
{
  FindPlaneNormalVector(a, b, c, n);

  d = (a[0] * n[0]) + (a[1] * n[1]) + (a[2] * n[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindDistanceToTriangleCentroid(const float* a, const float* b, const float* c, const float* q, float& distance)
{
  float centroid[3] = { 0.0f, 0.0f, 0.0f };
  centroid[0] = (a[0] + b[0] + c[0]) / 3.0f;
  centroid[1] = (a[1] + b[1] + c[1]) / 3.0f;
  centroid[2] = (a[2] + b[2] + c[2]) / 3.0f;

  FindDistanceBetweenPoints(centroid, q, distance);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindDistanceFromPlane(const float* q, float n[3], float d, float& distance)
{
  distance = (q[0] * n[0]) + (q[1] * n[1]) + (q[2] * n[2]) - d;
  distance /= sqrtf((n[0] * n[0]) + (n[1] * n[1]) + (n[2] * n[2]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindDistanceBetweenPoints(const float a[3], const float b[3], float& distance)
{
  float dx = b[0] - a[0];
  float dy = b[1] - a[1];
  float dz = b[2] - a[2];
  distance = sqrtf((dx * dx) + (dy * dy) + (dz * dz));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindTriangleArea(const float a[3], const float b[3], const float c[3], float& area)
{
  area = ((b[0] - a[0]) * (c[1] - a[1])) - ((c[0] - a[0]) * (b[1] - a[1]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindTetrahedronVolume(const float a[3], const float b[3], const float c[3], const float d[3], float& volume)
{
  float axdx, aydy, azdz, bxdx, bydy, bzdz, cxdx, cydy, czdz;

  axdx = a[0] - d[0];
  aydy = a[1] - d[1];
  azdz = a[2] - d[2];
  bxdx = b[0] - d[0];
  bydy = b[1] - d[1];
  bzdz = b[2] - d[2];
  cxdx = c[0] - d[0];
  cydy = c[1] - d[1];
  czdz = c[2] - d[2];

  volume = (azdz * ((bxdx * cydy) - (bydy * cxdx))) + (aydy * ((bzdz * cxdx) - (bxdx * czdz))) + (axdx * ((bydy * czdz) - (bzdz * cydy)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char GeometryMath::RayIntersectsTriangle(const float* a, const float* b, const float* c, const float* q, const float* r, float* p)
{
  char code = '?';
  int m = -1;

  code = RayIntersectsPlane(a, b, c, q, r, p, m);

  if (code == '0')
  {
    return '0';
  }
  else if (code == 'q')
  {
    return PointInTriangle3D(a, b, c, m, q);
  }
  else if (code == 'r')
  {
    return PointInTriangle3D(a, b, c, m, r);
  }
  else if (code == 'p')
  {
    return 'p';
  }
  else if (code == '1')
  {
    return RayCrossesTriangle(a, b, c, q, r);
  }
  else
  {
    return code;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char GeometryMath::RayIntersectsPlane(const float* a, const float* b, const float* c, const float* q, const float* r, float* p, int& m)
{
  float n[3];
  float rq[3];
  float d, num, denom, t;

  FindPlaneCoefficients(a, b, c, n, d);

  num = d - ((q[0] * n[0]) + (q[1] * n[1]) + (q[2] * n[2]));
  rq[0] = r[0] - q[0];
  rq[1] = r[1] - q[1];
  rq[2] = r[2] - q[2];
  denom = (rq[0] * n[0]) + (rq[1] * n[1]) + (rq[2] * n[2]);
  m = MatrixMath::FindIndexOfMaxVal3x1(n);

  if (denom == 0.0)
  {
    if (num == 0.0)
    {
      return 'p';
    }
    else
    {
      return '0';
    }
  }
  else
  {
    t = num / denom;
    for (int i = 0; i < 3; i++)
    {
      p[i] = q[i] + (t * (r[i] - q[i]));
    }
    if (t > 0.0 && t < 1.0)
    {
      return '1';
    }
    else if (num == 0.0)
    {
      return 'q';
    }
    else if (num == denom)
    {
      return 'r';
    }
    else
    {
      return '0';
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char GeometryMath::PointInTriangle3D(const float* a, const float* b, const float* c, int m, const float* p)
{
  float pp[3], aP[3], bP[3], cP[3];

  int j = 0;
  for (int i = 0; i < 3; i++)
  {
    if (i != m)
    {
      pp[j] = p[i];
      aP[j] = a[i];
      bP[j] = b[i];
      cP[j] = c[i];
      j++;
    }
  }
  return PointInTriangle2D(aP, bP, cP, pp);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char GeometryMath::PointInTriangle2D(const float* a, const float* b, const float* c, const float* p)
{
  float area0, area1, area2;

  FindTriangleArea(p, a, b, area0);
  FindTriangleArea(p, b, c, area1);
  FindTriangleArea(p, c, a, area2);

  if ((area0 == 0 && area1 > 0 && area2 > 0) || (area1 == 0 && area0 > 0 && area2 > 0) || (area2 == 0 && area0 > 0 && area1 > 0))
  {
    return 'E';
  }
  else if ((area0 == 0 && area1 < 0 && area2 < 0) || (area1 == 0 && area0 < 0 && area2 < 0) || (area2 == 0 && area0 < 0 && area1 < 0))
  {
    return 'E';
  }
  else if ((area0 > 0 && area1 > 0 && area2 > 0) || (area0 < 0 && area1 < 0 && area2 < 0))
  {
    return 'F';
  }
  else if ((area0 == 0 && area1 == 0 && area2 == 0))
  {
    return '?';
  }
  else if ((area0 == 0 && area1 == 0) || (area0 == 0 && area2 == 0) || (area1 == 0 && area2 == 0))
  {
    return 'V';
  }
  else
  {
    return '0';
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char GeometryMath::RayCrossesTriangle(const float* a, const float* b, const float* c, const float* q, const float* r)
{
  float vol0, vol1, vol2;

  FindTetrahedronVolume(q, a, b, r, vol0);
  FindTetrahedronVolume(q, b, c, r, vol1);
  FindTetrahedronVolume(q, c, a, r, vol2);

  if ((vol0 > 0 && vol1 > 0 && vol2 > 0) || (vol0 < 0 && vol1 < 0 && vol2 < 0))
  {
    return 'f';
  }
  else if ((vol0 > 0 || vol1 > 0 || vol2 > 0) && (vol0 < 0 || vol1 < 0 || vol2 < 0))
  {
    return '0';
  }
  else if ((vol0 == 0 && vol1 == 0 && vol2 == 0))
  {
    return '?';
  }
  else if ((vol0 == 0 && vol1 == 0) || (vol0 == 0 && vol2 == 0) || (vol1 == 0 && vol2 == 0))
  {
    return 'v';
  }
  else if (vol0 == 0 || vol1 == 0 || vol2 == 0)
  {
    return 'e';
  }
  else
  {
    return '?';
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char GeometryMath::PointInPolyhedron(const TriangleGeom::Pointer faces,
                                     const Int32Int32DynamicListArray::ElementList& faceIds,
                                     const VertexGeom::Pointer faceBBs,
                                     const float* q,
                                     const float* ll,
                                     const float* ur,
                                     float radius)
{
  float ray[3];  /* Ray */
  float r[3];  /* Ray endpoint. */
  float p[3];  /* Intersection point; not used. */
  int f, k = 0, crossings = 0;
  char code = '?';
  float a[3];
  float b[3];
  float c[3];

  //* If query point is outside bounding box, finished. */
  if (PointInBox(q, ll, ur) == false)
  {
    return 'o';
  }

  int numFaces = faceIds.ncells;
  int32_t* faceId = faceIds.cells;

  p[0] = 0;
  p[1] = 0;
  p[2] = 0;

LOOP:
  while (k++ < numFaces)
  {
    crossings = 0;

    //Generate and add ray to point to find other end
    GenerateRandomRay(radius, ray);
    r[0] = q[0] + ray[0];
    r[1] = q[1] + ray[1];
    r[2] = q[2] + ray[2];

    for (f = 0; f < numFaces; f++)
    {
      /* Begin check each face */
      int32_t idx = 2 * faceId[f];
      float* v0 = faceBBs->getVertexPointer(idx);
      float* v1 = faceBBs->getVertexPointer(idx + 1);
      if (RayIntersectsBox(q, r, v0, v1) == false)
      {
        code = '0';
      }
      else
      {
        faces->getVertCoordsAtTri(faceId[f], a, b, c);
        code = RayIntersectsTriangle(a, b, c, q, r, p);
      }

      /* If ray is degenerate, then goto outer while to generate another. */
      if (code == 'p' || code == 'v' || code == 'e' || code == '?')
      {
        goto LOOP;
      }

      /* If ray hits face at interior point, increment crossings. */
      else if (code == 'f')
      {
        crossings++;
      }

      /* If query endpoint q sits on a V/E/F, return that code. */
      else if (code == 'V' || code == 'E' || code == 'F')
      {
        return(code);
      }

    } /* End check each face */

    /* No degeneracies encountered: ray is generic, so finished. */
    break;

  } /* End while loop */

  /* q strictly interior to polyhedron if an odd number of crossings. */
  if ((crossings % 2) == 1)
  {
    return 'i';
  }
  else
  {
    return 'o';
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char GeometryMath::PointInPolyhedron(const TriangleGeom::Pointer faces,
                                     const Int32Int32DynamicListArray::ElementList& faceIds,
                                     const VertexGeom::Pointer faceBBs,
                                     const float* q,
                                     const float* ll,
                                     const float* ur,
                                     float radius,
                                     float& distToBoundary)
{
  float ray[3] = { 0.0f, 0.0f, 0.0f };  /* Ray */
  float r[3] = { 0.0f, 0.0f, 0.0f };  /* Ray endpoint. */
  float p[3] = { 0.0f, 0.0f, 0.0f };  /* Intersection point; not used. */
  int f, k = 0, crossings = 0;
  char code = '?';
  float a[3] = { 0.0f, 0.0f, 0.0f };
  float b[3] = { 0.0f, 0.0f, 0.0f };
  float c[3] = { 0.0f, 0.0f, 0.0f };
  float n[3] = { 0.0f, 0.0f, 0.0f };
  float d = 0.0f;
  float closestTriangleDistance = std::numeric_limits<float>::max();
  float distance = 0.0f;

  //* If query point is outside bounding box, finished. */
  if (PointInBox(q, ll, ur) == false)
  {
    return 'o';
  }

  int numFaces = faceIds.ncells;
  int32_t* faceId = faceIds.cells;

  p[0] = 0;
  p[1] = 0;
  p[2] = 0;

LOOP:
  while (k++ < numFaces)
  {
    crossings = 0;

    //Generate and add ray to point to find other end
    GenerateRandomRay(radius, ray);
    r[0] = q[0] + ray[0];
    r[1] = q[1] + ray[1];
    r[2] = q[2] + ray[2];

    for (f = 0; f < numFaces; f++)
    {
      /* Begin check each face */
      int32_t idx = 2 * faceId[f];
      float* v0 = faceBBs->getVertexPointer(idx);
      float* v1 = faceBBs->getVertexPointer(idx + 1);
      faces->getVertCoordsAtTri(faceId[f], a, b, c);
      FindDistanceToTriangleCentroid(a, b, c, q, distance);
      if (distance < closestTriangleDistance)
      {
        closestTriangleDistance = distance;
        FindPlaneCoefficients(a, b, c, n, d);
        FindDistanceFromPlane(q, n, d, distance);
        distToBoundary = fabs(distance);
      }
      if (RayIntersectsBox(q, r, v0, v1) == false)
      {
        code = '0';
      }
      else
      {
        code = RayIntersectsTriangle(a, b, c, q, r, p);
      }

      /* If ray is degenerate, then goto outer while to generate another. */
      if (code == 'p' || code == 'v' || code == 'e' || code == '?')
      {
        goto LOOP;
      }

      /* If ray hits face at interior point, increment crossings. */
      else if (code == 'f')
      {
        crossings++;
      }

      /* If query endpoint q sits on a V/E/F, return that code. */
      else if (code == 'V' || code == 'E' || code == 'F')
      {
        return(code);
      }

    } /* End check each face */

    /* No degeneracies encountered: ray is generic, so finished. */
    break;

  } /* End while loop */

  /* q strictly interior to polyhedron if an odd number of crossings. */
  if ((crossings % 2) == 1)
  {
    return 'i';
  }
  else
  {
    return 'o';
  }
}

