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
void GeometryMath::GenerateRandomRay(float p[3], float length, float ray[3])
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