/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Michael A. Groeber (US Air Force Research Laboratory)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "GeometryMath.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

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
float GeometryMath::CosThetaBetweenVectors(float a[3], float b[3])
{
  float norm1 = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
  float norm2 = sqrt(b[0] * b[0] + b[1] * b[1] + b[2] * b[2]);
  return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]) / (norm1 * norm2);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float GeometryMath::AngleBetweenVectors(float a[3], float b[3])
{
  float norm1 = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
  float norm2 = sqrt(b[0] * b[0] + b[1] * b[1] + b[2] * b[2]);
  return acos((a[0] * b[0] + a[1] * b[1] + a[2] * b[2]) / (norm1 * norm2));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GeometryMath::PointInBox(float p[3], float ll[3], float ur[3])
{
  if((ll[0] <= p[0]) && (p[0] <= ur[0]) && (ll[1] <= p[1]) && (p[1] <= ur[1]) && (ll[2] <= p[2]) && (p[2] <= ur[2])) return true;
  else return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GeometryMath::RayIntersectsBox(float p[3], float q[3], float ll[3], float ur[3])
{
  for(int i=0;i<3;i++)
  {
    if((ll[i] > p[i]) && (ll[i] > q[i])) return false;
    if((ur[i] < p[i]) && (ur[i] < q[i])) return false;
  }
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::GenerateRandomRay(float length, float ray[3])
{
  float w, t;

  unsigned long long int m_Seed = QDateTime::currentMSecsSinceEpoch();
  DREAM3D_RANDOMNG_NEW_SEEDED(m_Seed);

  ray[2] = (2.0 * rg.genrand_real1())-1.0;
  t = (DREAM3D::Constants::k_2Pi * rg.genrand_real1());
  w = sqrt(1.0 - (ray[2]*ray[2]));
  ray[0] = w * cos(t);
  ray[1] = w * sin(t);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindBoundingBoxOfVertices(VertexArray::Pointer verts, float ll[3], float ur[3])
{
  ll[0] = 100000000.0;
  ll[1] = 100000000.0;
  ll[2] = 100000000.0;
  ur[0] = 0.0;
  ur[1] = 0.0;
  ur[2] = 0.0;

  VertexArray::Vert_t* v = verts->getPointer(0);
  for(int i=0;i<verts->count();i++)
  {
    if(v[i].pos[0] < ll[0]) ll[0] = v[i].pos[0];
    if(v[i].pos[0] > ur[0]) ur[0] = v[i].pos[0];
    if(v[i].pos[1] < ll[1]) ll[1] = v[i].pos[1];
    if(v[i].pos[1] > ur[1]) ur[1] = v[i].pos[1];
    if(v[i].pos[2] < ll[2]) ll[2] = v[i].pos[2];
    if(v[i].pos[2] > ur[2]) ur[2] = v[i].pos[2];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindPlaneNormalVector(float a[3], float b[3], float c[3], float n[3])
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
void GeometryMath::FindPlaneCoefficients(float a[3], float b[3], float c[3], float n[3], float& d)
{
  FindPlaneNormalVector(a, b, c, n);

  d = MatrixMath::DotProduct3x1(a, n);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindTriangleArea(float a[3], float b[3], float c[3], float& area)
{
  area = ((b[0]-a[0])*(c[1]-a[1])) - ((c[0]-a[0])*(b[1]-a[1]));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GeometryMath::FindTetrahedronVolume(float a[3], float b[3], float c[3], float d[3], float& volume)
{
  float axdx, aydy, azdz, bxdx, bydy, bzdz, cxdx, cydy, czdz;

  axdx = a[0]-d[0];
  aydy = a[1]-d[1];
  azdz = a[2]-d[2];
  bxdx = b[0]-d[0];
  bydy = b[1]-d[1];
  bzdz = b[2]-d[2];
  cxdx = c[0]-d[0];
  cydy = c[1]-d[1];
  czdz = c[2]-d[2];

  volume = (azdz*((bxdx*cydy)-(bydy*cxdx))) + (aydy*((bzdz*cxdx)-(bxdx*czdz))) + (axdx*((bydy*czdz)-(bzdz*cydy)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char GeometryMath::RayIntersectsTriangle(float a[3], float b[3], float c[3], float q[3], float r[3], float p[3])
{
  int code = -1;
  int m = -1;

  code = RayIntersectsPlane(a, b, c, q, r, p, m);

  if(code == '0') return '0';
  if(code == 'q') return PointInTriangle3D(a, b, c, m, q);
  if(code == 'r') return PointInTriangle3D(a, b, c, m, r);
  if(code == 'p') return 'p';
  if(code == '1') return RayCrossesTriangle(a, b, c, q, r);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char GeometryMath::RayIntersectsPlane(float a[3], float b[3], float c[3], float q[3], float r[3], float p[3], int& m)
{
  float n[3];
  float rq[3];
  float d, num, denom, t;

  FindPlaneCoefficients(a, b, c, n, d);
  num = d - MatrixMath::DotProduct3x1(q, n);
  MatrixMath::Subtract3x1s(r, q, rq);
  denom = MatrixMath::DotProduct3x1(rq, n);
  m = MatrixMath::FindIndexOfMaxVal3x1(n);

  if(denom == 0.0)
  {
    if(num == 0.0) return 'p';
    else return '0';
  }
  else
  {
    t = num/denom;
    for(int i=0;i<3;i++)
    {
      p[i] = q[i] + (t*(r[i]-q[i]));
    }
    if(t > 0.0 && t < 1.0) return '1';
    else if (num == 0.0) return 'q';
    else if (num == denom) return 'r';
    else return '0';
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char GeometryMath::PointInTriangle3D(float a[3], float b[3], float c[3], int m, float p[3])
{
  float pp[3], aP[3], bP[3], cP[3];

  int j = 0;
  for(int i=0;i<3;i++)
  {
    if(i != m)
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
char GeometryMath::PointInTriangle2D(float a[3], float b[3], float c[3], float p[3])
{
  float area0, area1, area2;

  FindTriangleArea(p, a, b, area0);
  FindTriangleArea(p, b, c, area1);
  FindTriangleArea(p, c, a, area2);

  if((area0 == 0 && area1 > 0 && area2 > 0) || (area1 == 0 && area0 > 0 && area2 > 0) || (area2 == 0 && area0 > 0 && area1 > 0)) return 'E';
  if((area0 == 0 && area1 < 0 && area2 < 0) || (area1 == 0 && area0 < 0 && area2 < 0) || (area2 == 0 && area0 < 0 && area1 < 0)) return 'E';
  if((area0 > 0 && area1 > 0 && area2 > 0) || (area0 < 0 && area1 < 0 && area2 < 0)) return 'F';
  if((area0 == 0 && area1 == 0 && area2 == 0)) return '?';
  if((area0 == 0 && area1 == 0) || (area0 == 0 && area2 == 0) || (area1 == 0 && area2 == 0)) return 'V';
  else return '0';
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char GeometryMath::RayCrossesTriangle(float a[3], float b[3], float c[3], float q[3], float r[3])
{
  float vol0, vol1, vol2;

  FindTetrahedronVolume(q, a, b, r, vol0);
  FindTetrahedronVolume(q, b, c, r, vol1);
  FindTetrahedronVolume(q, c, a, r, vol2);

  if((vol0 > 0 && vol1 > 0 && vol2 > 0) || (vol0 < 0 && vol1 < 0 && vol2 < 0)) return 'f';
  if((vol0 > 0 || vol1 > 0 || vol2 > 0) && (vol0 < 0 || vol1 < 0 || vol2 < 0)) return '0';
  if((vol0 == 0 && vol1 == 0 && vol2 == 0)) return '?';
  if((vol0 == 0 && vol1 == 0) || (vol0 == 0 && vol2 == 0) || (vol1 == 0 && vol2 == 0)) return 'v';
  if(vol0 == 0 || vol1 == 0 || vol2 == 0) return 'e';
  else return '?';
}

char GeometryMath::PointInPolyhedron(FaceArray::Pointer faces, float q[3], float ll[3], float ur[3], float radius)
{
   float ray[3];  /* Ray */
   float r[3];  /* Ray endpoint. */
   float p[3];  /* Intersection point; not used. */
   int f, k = 0, crossings = 0;
   char code = '?';
   VertexArray::Vert_t a, b, c;
 
   /* If query point is outside bounding box, finished. */
   if ( PointInBox(q, ll, ur) == false)
   {
      return 'o';
   }
  
   int numFaces = faces->count();
   FaceArray::Face_t* face = faces->getPointer(0);

   LOOP:
   while( k++ < numFaces )
   {
      crossings = 0;
  
      GenerateRandomRay(radius, ray); 
      MatrixMath::Add3x1s(q, ray, r); 
  
      for ( f = 0; f < numFaces; f++ )
      {  /* Begin check each face */
         if( RayIntersectsBox(q, p, ll, ur) == false )
         {
           code = '0';
         }
         else
         {
           faces->getVertObjects(f, a, b, c);
           code = RayIntersectsTriangle(a, b, c, q, r, p);
         }

         /* If ray is degenerate, then goto outer while to generate another. */
         if ( code == 'p' || code == 'v' || code == 'e' )
         {
            goto LOOP;
         }
   
         /* If ray hits face at interior point, increment crossings. */
         else if ( code == 'f' )
         {
            crossings++;
         }

         /* If query endpoint q sits on a V/E/F, return that code. */
         else if ( code == 'V' || code == 'E' || code == 'F' ) return(code);

         /* If ray misses triangle, do nothing. */
         else if ( code == '0' )
         {

         }

         else exit(1);

      } /* End check each face */

      /* No degeneracies encountered: ray is generic, so finished. */
      break;

   } /* End while loop */
 
   /* q strictly interior to polyhedron if an odd number of crossings. */
   if( ( crossings % 2 ) == 1 )return 'i';
   else return 'o';
}