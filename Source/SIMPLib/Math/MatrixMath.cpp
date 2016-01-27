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

#include "MatrixMath.h"

#include "SIMPLib/Math/SIMPLibMath.h"

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
void MatrixMath::CrossProduct(float a[3], float b[3], float c[3])
{
  c[0] = a[1] * b[2] - a[2] * b[1];
  c[1] = a[2] * b[0] - a[0] * b[2];
  c[2] = a[0] * b[1] - a[1] * b[0];
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatrixMath::Normalize3x1(double& i, double& j, double& k)
{
  double denom;
  denom = sqrt(((i * i) + (j * j) + (k * k)));
  if (denom == 0)
  {
    return;
  }
  i = i / denom;
  j = j / denom;
  k = k / denom;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatrixMath::Normalize3x1(float& i, float& j, float& k)
{
  float denom;
  denom = sqrtf(((i * i) + (j * j) + (k * k)));
  if (denom == 0)
  {
    return;
  }
  i = i / denom;
  j = j / denom;
  k = k / denom;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatrixMath::Multiply3x3with3x3(float g1[3][3], float g2[3][3], float outMat[3][3])
{
  outMat[0][0] = g1[0][0] * g2[0][0] + g1[0][1] * g2[1][0] + g1[0][2] * g2[2][0];
  outMat[0][1] = g1[0][0] * g2[0][1] + g1[0][1] * g2[1][1] + g1[0][2] * g2[2][1];
  outMat[0][2] = g1[0][0] * g2[0][2] + g1[0][1] * g2[1][2] + g1[0][2] * g2[2][2];
  outMat[1][0] = g1[1][0] * g2[0][0] + g1[1][1] * g2[1][0] + g1[1][2] * g2[2][0];
  outMat[1][1] = g1[1][0] * g2[0][1] + g1[1][1] * g2[1][1] + g1[1][2] * g2[2][1];
  outMat[1][2] = g1[1][0] * g2[0][2] + g1[1][1] * g2[1][2] + g1[1][2] * g2[2][2];
  outMat[2][0] = g1[2][0] * g2[0][0] + g1[2][1] * g2[1][0] + g1[2][2] * g2[2][0];
  outMat[2][1] = g1[2][0] * g2[0][1] + g1[2][1] * g2[1][1] + g1[2][2] * g2[2][1];
  outMat[2][2] = g1[2][0] * g2[0][2] + g1[2][1] * g2[1][2] + g1[2][2] * g2[2][2];
}

void MatrixMath::Multiply3x3with3x1(const float g1[3][3], float g2[3], float outMat[3])
{
  outMat[0] = g1[0][0] * g2[0] + g1[0][1] * g2[1] + g1[0][2] * g2[2];
  outMat[1] = g1[1][0] * g2[0] + g1[1][1] * g2[1] + g1[1][2] * g2[2];
  outMat[2] = g1[2][0] * g2[0] + g1[2][1] * g2[1] + g1[2][2] * g2[2];
}

void MatrixMath::Multiply3x3with3x1(double g1[3][3], double g2[3], double outMat[3])
{
  outMat[0] = g1[0][0] * g2[0] + g1[0][1] * g2[1] + g1[0][2] * g2[2];
  outMat[1] = g1[1][0] * g2[0] + g1[1][1] * g2[1] + g1[1][2] * g2[2];
  outMat[2] = g1[2][0] * g2[0] + g1[2][1] * g2[1] + g1[2][2] * g2[2];
}

void MatrixMath::Add3x1s(const float g1[3], float g2[3], float outMat[3])
{
  outMat[0] = g1[0] + g2[0];
  outMat[1] = g1[1] + g2[1];
  outMat[2] = g1[2] + g2[2];
}

void MatrixMath::Subtract3x1s(const float g1[3], float g2[3], float outMat[3])
{
  outMat[0] = g1[0] - g2[0];
  outMat[1] = g1[1] - g2[1];
  outMat[2] = g1[2] - g2[2];
}

void MatrixMath::Add3x3s(const float g1[3][3], float g2[3][3], float outMat[3][3])
{
  outMat[0][0] = g1[0][0] + g2[0][0];
  outMat[0][1] = g1[0][1] + g2[0][1];
  outMat[0][2] = g1[0][2] + g2[0][2];
  outMat[1][0] = g1[1][0] + g2[1][0];
  outMat[1][1] = g1[1][1] + g2[1][1];
  outMat[1][2] = g1[1][2] + g2[1][2];
  outMat[2][0] = g1[2][0] + g2[2][0];
  outMat[2][1] = g1[2][1] + g2[2][1];
  outMat[2][2] = g1[2][2] + g2[2][2];
}

void MatrixMath::Subtract3x3s(const float g1[3][3], float g2[3][3], float outMat[3][3])
{
  outMat[0][0] = g1[0][0] - g2[0][0];
  outMat[0][1] = g1[0][1] - g2[0][1];
  outMat[0][2] = g1[0][2] - g2[0][2];
  outMat[1][0] = g1[1][0] - g2[1][0];
  outMat[1][1] = g1[1][1] - g2[1][1];
  outMat[1][2] = g1[1][2] - g2[1][2];
  outMat[2][0] = g1[2][0] - g2[2][0];
  outMat[2][1] = g1[2][1] - g2[2][1];
  outMat[2][2] = g1[2][2] - g2[2][2];
}

void MatrixMath::Multiply3x1withConstant(float g[3], float constant)
{
  g[0] *= constant;
  g[1] *= constant;
  g[2] *= constant;
}

void MatrixMath::Multiply3x3withConstant(float g[3][3], float constant)
{
  g[0][0] *= constant;
  g[0][1] *= constant;
  g[0][2] *= constant;
  g[1][0] *= constant;
  g[1][1] *= constant;
  g[1][2] *= constant;
  g[2][0] *= constant;
  g[2][1] *= constant;
  g[2][2] *= constant;
}

void MatrixMath::Invert3x3(float g[3][3], float outMat[3][3])
{
  Adjoint3x3(g, outMat);
  float determinant = Determinant3x3(g);
  float oneOverDeterminant = 1.0 / determinant;
  Multiply3x3withConstant(outMat, oneOverDeterminant);
}

void MatrixMath::Adjoint3x3(float g[3][3], float outMat[3][3])
{
  float temp[3][3];
  Cofactor3x3(g, temp);
  Transpose3x3(temp, outMat);
}

void MatrixMath::Cofactor3x3(float g[3][3], float outMat[3][3])
{
  float temp[3][3];
  Minors3x3(g, temp);
  outMat[0][0] = temp[0][0];
  outMat[0][1] = -temp[1][0];
  outMat[0][2] = temp[2][0];
  outMat[1][0] = -temp[0][1];
  outMat[1][1] = temp[1][1];
  outMat[1][2] = -temp[2][1];
  outMat[2][0] = temp[0][2];
  outMat[2][1] = -temp[1][2];
  outMat[2][2] = temp[2][2];
}

void MatrixMath::Minors3x3(float g[3][3], float outMat[3][3])
{
  outMat[0][0] = g[1][1] * g[2][2] - g[2][1] * g[1][2];
  outMat[0][1] = g[1][0] * g[2][2] - g[2][0] * g[1][2];
  outMat[0][2] = g[1][0] * g[2][1] - g[2][0] * g[1][1];
  outMat[1][0] = g[0][1] * g[2][2] - g[2][1] * g[0][2];
  outMat[1][1] = g[0][0] * g[2][2] - g[2][0] * g[0][2];
  outMat[1][2] = g[0][0] * g[2][1] - g[2][0] * g[0][1];
  outMat[2][0] = g[0][1] * g[1][2] - g[1][1] * g[0][2];
  outMat[2][1] = g[0][0] * g[1][2] - g[1][0] * g[0][2];
  outMat[2][2] = g[0][0] * g[1][1] - g[1][0] * g[0][1];
}

float MatrixMath::Determinant3x3(float g[3][3])
{
  return (g[0][0] * (g[1][1] * g[2][2] - g[1][2] * g[2][1])) - (g[0][1] * (g[1][0] * g[2][2] - g[1][2] * g[2][0])) + (g[0][2] * (g[1][0] * g[2][1] - g[1][1] * g[2][0]));
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
  denom = g[0][0] * g[0][0] + g[1][0] * g[1][0] + g[2][0] * g[2][0];
  if(denom == 0.0)
  {
    return;
  }
  denom = sqrt(denom);
  g[0][0] = g[0][0] / denom;
  if(g[0][0] > 1)
  {
    g[0][0] = 1;
  }
  g[1][0] = g[1][0] / denom;
  if(g[1][0] > 1)
  {
    g[1][0] = 1;
  }
  g[2][0] = g[2][0] / denom;
  if(g[2][0] > 1)
  {
    g[2][0] = 1;
  }
  denom = g[0][1] * g[0][1] + g[1][1] * g[1][1] + g[2][1] * g[2][1];
  if(denom == 0.0)
  {
    return;
  }
  denom = sqrt(denom);
  g[0][1] = g[0][1] / denom;
  if(g[0][1] > 1)
  {
    g[0][1] = 1;
  }
  g[1][1] = g[1][1] / denom;
  if(g[1][1] > 1)
  {
    g[1][1] = 1;
  }
  g[2][1] = g[2][1] / denom;
  if(g[2][1] > 1)
  {
    g[2][1] = 1;
  }
  denom = g[0][2] * g[0][2] + g[1][2] * g[1][2] + g[2][2] * g[2][2];
  if(denom == 0.0)
  {
    return;
  }
  denom = sqrt(denom);
  g[0][2] = g[0][2] / denom;
  if(g[0][2] > 1)
  {
    g[0][2] = 1;
  }
  g[1][2] = g[1][2] / denom;
  if(g[1][2] > 1)
  {
    g[1][2] = 1;
  }
  g[2][2] = g[2][2] / denom;
  if(g[2][2] > 1)
  {
    g[2][2] = 1;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatrixMath::Normalize3x1(float g[3])
{
  float denom;
  denom = g[0] * g[0] + g[1] * g[1] + g[2] * g[2];
  if(denom == 0.0)
  {
    return;
  }
  denom = sqrt(denom);
  g[0] = g[0] / denom;
  if(g[0] > 1.0f)
  {
    g[0] = 1.0f;
  }
  g[1] = g[1] / denom;
  if(g[1] > 1.0f)
  {
    g[1] = 1.0f;
  }
  g[2] = g[2] / denom;
  if(g[2] > 1.0f)
  {
    g[2] = 1.0f;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MatrixMath::FindIndexOfMaxVal3x1(float g[3])
{
  float a = fabs(g[0]);
  float b = fabs(g[1]);
  float c = fabs(g[2]);
  if(a >= b && a >= c)
  {
    return 0;
  }
  else if(b >= a && b >= c)
  {
    return 1;
  }
  else
  {
    return 2;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatrixMath::Sort3x1Ascending(float g[3])
{
  float temp;

  if(g[0] >= g[1] && g[0] >= g[2])
  {
    if(g[1] >= g[2])
    {
      g[0] = g[0];
      g[1] = g[1];
      g[2] = g[2];
    }
    else
    {
      g[0] = g[0];
      temp = g[1];
      g[1] = g[2];
      g[2] = temp;
    }
  }
  else if(g[1] >= g[0] && g[1] >= g[2])
  {
    if(g[0] >= g[2])
    {
      temp = g[0];
      g[0] = g[1];
      g[1] = temp;
      g[2] = g[2];
    }
    else
    {
      temp = g[0];
      g[0] = g[1];
      g[1] = g[2];
      g[2] = temp;
    }
  }
  else if(g[2] >= g[0] && g[2] >= g[1])
  {
    if(g[0] >= g[1])
    {
      temp = g[0];
      g[0] = g[2];
      g[2] = g[1];
      g[1] = temp;
    }
    else
    {
      temp = g[0];
      g[0] = g[2];
      g[1] = g[1];
      g[2] = temp;
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MatrixMath::Normalize3x1(double g[3])
{
  double denom;
  denom = g[0] * g[0] + g[1] * g[1] + g[2] * g[2];
  denom = sqrt(denom);
  g[0] = g[0] / denom;
  if(g[0] > 1.0)
  {
    g[0] = 1.0;
  }
  g[1] = g[1] / denom;
  if(g[1] > 1.0)
  {
    g[1] = 1.0;
  }
  g[2] = g[2] / denom;
  if(g[2] > 1.0)
  {
    g[2] = 1.0;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double MatrixMath::Magnitude4x1(double a[4])
{
  return (sqrt((a[0] * a[0]) + (a[1] * a[1]) + (a[2] * a[2]) + (a[3] * a[3])));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float MatrixMath::Magnitude4x1(float a[4])
{
  return (sqrtf((a[0] * a[0]) + (a[1] * a[1]) + (a[2] * a[2]) + (a[3] * a[3])));
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float MatrixMath::DotProduct3x1(float a[3], float b[3])
{
  return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float MatrixMath::DotProduct3x1(double a[3], double b[3])
{
  return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}
