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
#include "MatrixMath.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MatrixMath::MatrixMath()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MatrixMath::~MatrixMath()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatrixMath::CrossProduct(double a[3], double b[3], double c[3])
{
  c[0] = a[1] * b[2] - a[2] * b[1];
  c[1] = a[2] * b[0] - a[0] * b[2];
  c[2] = a[0] * b[1] - a[1] * b[0];
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatrixMath::Normalize3x1(double &i, double &j, double &k)
{
  double denom;
  denom = sqrt(((i*i)+(j*j)+(k*k)));
  if (denom == 0)
  {
    return;
  }
  i = i/denom;
  j = j/denom;
  k = k/denom;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatrixMath::Normalize3x1(float &i, float &j, float &k)
{
  float denom;
  denom = sqrtf(((i*i)+(j*j)+(k*k)));
  if (denom == 0)
  {
    return;
  }
  i = i/denom;
  j = j/denom;
  k = k/denom;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatrixMath::Multiply3x3with3x3(float g1[3][3], float g2[3][3], float outMat[3][3])
{
  outMat[0][0] = g1[0][0]*g2[0][0] + g1[0][1]*g2[1][0] + g1[0][2]*g2[2][0];
  outMat[0][1] = g1[0][0]*g2[0][1] + g1[0][1]*g2[1][1] + g1[0][2]*g2[2][1];
  outMat[0][2] = g1[0][0]*g2[0][2] + g1[0][1]*g2[1][2] + g1[0][2]*g2[2][2];
  outMat[1][0] = g1[1][0]*g2[0][0] + g1[1][1]*g2[1][0] + g1[1][2]*g2[2][0];
  outMat[1][1] = g1[1][0]*g2[0][1] + g1[1][1]*g2[1][1] + g1[1][2]*g2[2][1];
  outMat[1][2] = g1[1][0]*g2[0][2] + g1[1][1]*g2[1][2] + g1[1][2]*g2[2][2];
  outMat[2][0] = g1[2][0]*g2[0][0] + g1[2][1]*g2[1][0] + g1[2][2]*g2[2][0];
  outMat[2][1] = g1[2][0]*g2[0][1] + g1[2][1]*g2[1][1] + g1[2][2]*g2[2][1];
  outMat[2][2] = g1[2][0]*g2[0][2] + g1[2][1]*g2[1][2] + g1[2][2]*g2[2][2];
}

void MatrixMath::Multiply3x3with3x1(const float g1[3][3], float g2[3], float outMat[3])
{
  outMat[0] = g1[0][0]*g2[0] + g1[0][1]*g2[1] + g1[0][2]*g2[2];
  outMat[1] = g1[1][0]*g2[0] + g1[1][1]*g2[1] + g1[1][2]*g2[2];
  outMat[2] = g1[2][0]*g2[0] + g1[2][1]*g2[1] + g1[2][2]*g2[2];
}

void MatrixMath::Multiply3x3with3x1(double g1[3][3], double g2[3], double outMat[3])
{
  outMat[0] = g1[0][0]*g2[0] + g1[0][1]*g2[1] + g1[0][2]*g2[2];
  outMat[1] = g1[1][0]*g2[0] + g1[1][1]*g2[1] + g1[1][2]*g2[2];
  outMat[2] = g1[2][0]*g2[0] + g1[2][1]*g2[1] + g1[2][2]*g2[2];
}

void MatrixMath::Multiply3x1withConstant(float g[3], float constant)
{
  g[0] *= constant;
  g[1] *= constant;
  g[2] *= constant;
}

void MatrixMath::Transpose3x3(float g[3][3], float outMat[3][3])
{
  outMat[0][0] = g[0][0];
  outMat[0][1] = g[1][0];
  outMat[0][2] = g[2][0];
  outMat[1][0] = g[0][1];
  outMat[1][1] = g[1][1];
  outMat[1][2] = g[2][1];
  outMat[2][0] = g[0][2];
  outMat[2][1] = g[1][2];
  outMat[2][2] = g[2][2];
}

void MatrixMath::Copy3x3(float g[3][3], float outMat[3][3])
{
  outMat[0][0] = g[0][0];
  outMat[0][1] = g[0][1];
  outMat[0][2] = g[0][2];
  outMat[1][0] = g[1][0];
  outMat[1][1] = g[1][1];
  outMat[1][2] = g[1][2];
  outMat[2][0] = g[2][0];
  outMat[2][1] = g[2][1];
  outMat[2][2] = g[2][2];
}

void MatrixMath::Copy3x1(float g[3], float outMat[3])
{
  outMat[0] = g[0];
  outMat[1] = g[1];
  outMat[2] = g[2];
}

void MatrixMath::Identity3x3(float g[3][3])
{
  g[0][0] = 1.0f;
  g[0][1] = 0.0f;
  g[0][2] = 0.0f;
  g[1][0] = 0.0f;
  g[1][1] = 1.0f;
  g[1][2] = 0.0f;
  g[2][0] = 0.0f;
  g[2][1] = 0.0f;
  g[2][2] = 1.0f;
}

void MatrixMath::Normalize3x3(float g[3][3])
{
  float denom;
  denom = g[0][0]*g[0][0]+g[1][0]*g[1][0]+g[2][0]*g[2][0];
  denom = sqrt(denom);
  g[0][0] = g[0][0]/denom;
  if(g[0][0] > 1) g[0][0] = 1;
  g[1][0] = g[1][0]/denom;
  if(g[1][0] > 1) g[1][0] = 1;
  g[2][0] = g[2][0]/denom;
  if(g[2][0] > 1) g[2][0] = 1;
  denom = g[0][1]*g[0][1]+g[1][1]*g[1][1]+g[2][1]*g[2][1];
  denom = sqrt(denom);
  g[0][1] = g[0][1]/denom;
  if(g[0][1] > 1) g[0][1] = 1;
  g[1][1] = g[1][1]/denom;
  if(g[1][1] > 1) g[1][1] = 1;
  g[2][1] = g[2][1]/denom;
  if(g[2][1] > 1) g[2][1] = 1;
  denom = g[0][2]*g[0][2]+g[1][2]*g[1][2]+g[2][2]*g[2][2];
  denom = sqrt(denom);
  g[0][2] = g[0][2]/denom;
  if(g[0][2] > 1) g[0][2] = 1;
  g[1][2] = g[1][2]/denom;
  if(g[1][2] > 1) g[1][2] = 1;
  g[2][2] = g[2][2]/denom;
  if(g[2][2] > 1) g[2][2] = 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatrixMath::Normalize3x1(float g[3])
{
  float denom;
  denom = g[0]*g[0]+g[1]*g[1]+g[2]*g[2];
  denom = sqrt(denom);
  g[0] = g[0]/denom;
  if(g[0] > 1.0f) g[0] = 1.0f;
  g[1] = g[1]/denom;
  if(g[1] > 1.0f) g[1] = 1.0f;
  g[2] = g[2]/denom;
  if(g[2] > 1.0f) g[2] = 1.0f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatrixMath::Normalize3x1(double g[3])
{
  double denom;
  denom = g[0]*g[0]+g[1]*g[1]+g[2]*g[2];
  denom = sqrt(denom);
  g[0] = g[0]/denom;
  if(g[0] > 1.0) g[0] = 1.0;
  g[1] = g[1]/denom;
  if(g[1] > 1.0) g[1] = 1.0;
  g[2] = g[2]/denom;
  if(g[2] > 1.0) g[2] = 1.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float MatrixMath::DotProduct3x1(float a[3], float b[3])
{
  return (a[0]*b[0]+a[1]*b[1]+a[2]*b[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float MatrixMath::CosThetaBetweenVectors(float a[3], float b[3])
{
  float norm1 = sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
  float norm2 = sqrt(b[0]*b[0]+b[1]*b[1]+b[2]*b[2]);
  return (a[0]*b[0]+a[1]*b[1]+a[2]*b[2])/(norm1*norm2);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float MatrixMath::AngleBetweenVectors(float a[3], float b[3])
{
  float norm1 = sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
  float norm2 = sqrt(b[0]*b[0]+b[1]*b[1]+b[2]*b[2]);
  return acos((a[0]*b[0]+a[1]*b[1]+a[2]*b[2])/(norm1*norm2));
}
