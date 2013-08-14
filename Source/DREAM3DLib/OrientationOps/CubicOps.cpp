/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "CubicOps.h"
// Include this FIRST because there is a needed define for some compiles
// to expose some of the constants needed below
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/Common/ModifiedLambertProjection.h"
#include "DREAM3DLib/Utilities/ImageUtilities.h"

namespace Detail
{
  static const float CubicDim1InitValue = powf((0.75f*((DREAM3D::Constants::k_Pi/4.0f)-sinf((DREAM3D::Constants::k_Pi/4.0f)))),(1.0f/3.0f));
  static const float CubicDim2InitValue = powf((0.75f*((DREAM3D::Constants::k_Pi/4.0f)-sinf((DREAM3D::Constants::k_Pi/4.0f)))),(1.0f/3.0f));
  static const float CubicDim3InitValue = powf((0.75f*((DREAM3D::Constants::k_Pi/4.0f)-sinf((DREAM3D::Constants::k_Pi/4.0f)))),(1.0f/3.0f));
  static const float CubicDim1StepValue = CubicDim1InitValue/9.0f;
  static const float CubicDim2StepValue = CubicDim2InitValue/9.0f;
  static const float CubicDim3StepValue = CubicDim3InitValue/9.0f;
}

static const QuatF CubicQuatSym[24] = {
  QuaternionMathF::New(0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f),
  QuaternionMathF::New(1.000000000f, 0.000000000f, 0.000000000f, 0.000000000f),
  QuaternionMathF::New(0.000000000f, 1.000000000f, 0.000000000f, 0.000000000f),
  QuaternionMathF::New(0.000000000f, 0.000000000f, 1.000000000f, 0.000000000f),
  QuaternionMathF::New(DREAM3D::Constants::k_1OverRoot2, 0.000000000f, 0.000000000f, DREAM3D::Constants::k_1OverRoot2),
  QuaternionMathF::New(0.000000000f, DREAM3D::Constants::k_1OverRoot2, 0.000000000f, DREAM3D::Constants::k_1OverRoot2),
  QuaternionMathF::New(0.000000000f, 0.000000000f, DREAM3D::Constants::k_1OverRoot2, DREAM3D::Constants::k_1OverRoot2),
  QuaternionMathF::New(-DREAM3D::Constants::k_1OverRoot2, 0.000000000f, 0.000000000f, DREAM3D::Constants::k_1OverRoot2),
  QuaternionMathF::New(0.000000000f, -DREAM3D::Constants::k_1OverRoot2, 0.000000000f, DREAM3D::Constants::k_1OverRoot2),
  QuaternionMathF::New(0.000000000f, 0.000000000f, -DREAM3D::Constants::k_1OverRoot2, DREAM3D::Constants::k_1OverRoot2),
  QuaternionMathF::New(DREAM3D::Constants::k_1OverRoot2, DREAM3D::Constants::k_1OverRoot2, 0.000000000f, 0.000000000f),
  QuaternionMathF::New(-DREAM3D::Constants::k_1OverRoot2, DREAM3D::Constants::k_1OverRoot2, 0.000000000f, 0.000000000f),
  QuaternionMathF::New(0.000000000f, DREAM3D::Constants::k_1OverRoot2, DREAM3D::Constants::k_1OverRoot2, 0.000000000f),
  QuaternionMathF::New(0.000000000f, -DREAM3D::Constants::k_1OverRoot2, DREAM3D::Constants::k_1OverRoot2, 0.000000000f),
  QuaternionMathF::New(DREAM3D::Constants::k_1OverRoot2, 0.000000000f, DREAM3D::Constants::k_1OverRoot2, 0.000000000f),
  QuaternionMathF::New(-DREAM3D::Constants::k_1OverRoot2, 0.000000000f, DREAM3D::Constants::k_1OverRoot2, 0.000000000f),
  QuaternionMathF::New(0.500000000f, 0.500000000f, 0.500000000f, 0.500000000f),
  QuaternionMathF::New(-0.500000000f, -0.500000000f, -0.500000000f, 0.500000000f),
  QuaternionMathF::New(0.500000000f, -0.500000000f, 0.500000000f, 0.500000000f),
  QuaternionMathF::New(-0.500000000f, 0.500000000f, -0.500000000f, 0.500000000f),
  QuaternionMathF::New(-0.500000000f, 0.500000000f, 0.500000000f, 0.500000000f),
  QuaternionMathF::New(0.500000000f, -0.500000000f, -0.500000000f, 0.500000000f),
  QuaternionMathF::New(-0.500000000f, -0.500000000f, 0.500000000f, 0.500000000f),
  QuaternionMathF::New(0.500000000f, 0.500000000f, -0.500000000f, 0.500000000f)};

static const float CubicRodSym[24][3] = {{0.0f, 0.0f,0.0f},
                                         {10000000000.0f, 0.0f, 0.0f},
                                         {0.0f, 10000000000.0f, 0.0f},
                                         {0.0f, 0.0f, 10000000000.0f},
                                         {1.0f, 0.0f, 0.0f},
                                         {0.0f, 1.0f, 0.0f},
                                         {0.0f, 0.0f, 1.0f},
                                         {-1.0f, 0.0f, 0.0f},
                                         {0.0f, -1.0f, 0.0f},
                                         {0.0f, 0.0f, -1.0f},
                                         {10000000000.0f, 10000000000.0f, 0.0f},
                                         {-10000000000.0f, 10000000000.0f, 0.0f},
                                         {0.0f, 10000000000.0f, 10000000000.0f},
                                         {0.0f, -10000000000.0f, 10000000000.0f},
                                         {10000000000.0f, 0.0f, 10000000000.0f},
                                         {-10000000000.0f, 0.0f, 10000000000.0f},
                                         {1.0f, 1.0f, 1.0f},
                                         {-1.0f, -1.0f, -1.0f},
                                         {1.0f, -1.0f, 1.0f},
                                         {-1.0f, 1.0f, -1.0f},
                                         {-1.0f, 1.0f, 1.0f},
                                         {1.0f, -1.0f, -1.0f},
                                         {-1.0f, -1.0f, 1.0f},
                                         {1.0f, 1.0f, -1.0}};

static const float CubicSlipDirections[12][3] = {{0.0f,1.0f,-1.0f},
                                                 {1.0f,0.0f,-1.0f},
                                                 {1.0f,-1.0f,0.0f},
                                                 {1.0f,-1.0f,0.0f},
                                                 {1.0f,0.0f,1.0f},
                                                 {0.0f,1.0f,1.0f},
                                                 {1.0f,1.0f,0.0f},
                                                 {0.0f,1.0f,1.0f},
                                                 {1.0f,0.0f,-1.0f},
                                                 {1.0f,1.0f,0.0f},
                                                 {1.0f,0.0f,1.0f},
                                                 {0.0f,1.0f,-1.0f}};

static const float CubicSlipPlanes[12][3] = {{1.0f,1.0f,1.0f},
                                             {1.0f,1.0f,1.0f},
                                             {1.0f,1.0f,1.0f},
                                             {1.0f,1.0f,-1.0f},
                                             {1.0f,1.0f,-1.0f},
                                             {1.0f,1.0f,-1.0f},
                                             {1.0f,-1.0f,1.0f},
                                             {1.0f,-1.0f,1.0f},
                                             {1.0f,-1.0f,1.0f},
                                             {-1.0f,1.0f,1.0f},
                                             {-1.0f,1.0f,1.0f},
                                             {-1.0f,1.0f,1.0f}};

static const float CubicMatSym[24][3][3] =
{{{1.0, 0.0, 0.0},
  {0.0, 1.0, 0.0},
  {0.0, 0.0, 1.0}},

 {{1.0, 0.0,  0.0},
  {0.0, 0.0, -1.0},
  {0.0, 1.0,  0.0}},

 {{1.0,  0.0,  0.0},
  {0.0, -1.0,  0.0},
  {0.0,  0.0, -1.0}},

 {{1.0,  0.0, 0.0},
  {0.0,  0.0, 1.0},
  {0.0, -1.0, 0.0}},

 {{0.0, 0.0, -1.0},
  {0.0, 1.0,  0.0},
  {1.0, 0.0,  0.0}},

 {{0.0, 0.0, 1.0},
  {0.0, 1.0, 0.0},
  {-1.0, 0.0, 0.0}},

 {{-1.0, 0.0,  0.0},
  {0.0, 1.0,  0.0},
  {0.0, 0.0, -1.0}},

 {{-1.0,  0.0, 0.0},
  {0.0, -1.0, 0.0},
  {0.0,  0.0, 1.0}},

 {{0.0, 1.0, 0.0},
  {-1.0, 0.0, 0.0},
  {0.0, 0.0, 1.0}},

 {{0.0, -1.0, 0.0},
  {1.0,  0.0, 0.0},
  {0.0,  0.0, 1.0}},

 {{0.0, -1.0, 0.0},
  {0.0,  0.0, 1.0},
  {-1.0,  0.0, 0.0}},

 {{0.0,  0.0, 1.0},
  {-1.0,  0.0, 0.0},
  {0.0, -1.0, 0.0}},

 {{0.0, -1.0,  0.0},
  {0.0,  0.0, -1.0},
  {1.0,  0.0,  0.0}},

 {{0.0,  0.0, -1.0},
  {1.0,  0.0,  0.0},
  {0.0, -1.0,  0.0}},

 {{0.0, 1.0,  0.0},
  {0.0, 0.0, -1.0},
  {-1.0, 0.0,  0.0}},

 {{0.0, 0.0, -1.0},
  {-1.0, 0.0,  0.0},
  {0.0, 1.0,  0.0}},

 {{0.0, 1.0, 0.0},
  {0.0, 0.0, 1.0},
  {1.0, 0.0, 0.0}},

 {{0.0, 0.0, 1.0},
  {1.0, 0.0, 0.0},
  {0.0, 1.0, 0.0}},

 {{0.0, 1.0,  0.0},
  {1.0, 0.0,  0.0},
  {0.0, 0.0, -1.0}},

 {{-1.0, 0.0, 0.0},
  {0.0, 0.0, 1.0},
  {0.0, 1.0, 0.0}},

 {{0.0,  0.0, 1.0},
  {0.0, -1.0, 0.0},
  {1.0,  0.0, 0.0}},

 {{-1.0,  0.0,  0.0},
  {0.0,  0.0, -1.0},
  {0.0, -1.0,  0.0}},

 {{0.0,  0.0, -1.0},
  {0.0, -1.0,  0.0},
  {-1.0,  0.0,  0.0}},

 {{0.0, -1.0,  0.0},
  {-1.0,  0.0,  0.0},
  {0.0,  0.0, -1.0}}};

using namespace Detail;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CubicOps::CubicOps()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CubicOps::~CubicOps()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float CubicOps::getMisoQuat(QuatF &q1, QuatF &q2, float &n1, float &n2, float &n3)
{

  int numsym = 24;

  return _calcMisoQuat(CubicQuatSym, numsym, q1, q2, n1, n2, n3);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float CubicOps::_calcMisoQuat(const QuatF quatsym[24], int numsym,
QuatF &q1, QuatF &q2,
float &n1, float &n2, float &n3)
{
  float wmin=9999999.0f; //,na,nb,nc;
  QuatF qco;
  QuatF qc;
  QuatF q2inv;
  int type = 1;
  float sin_wmin_over_2 = 0.0;
  //  float _1, _2,  _6;

  QuaternionMathF::Copy(q2, q2inv);
  QuaternionMathF::Conjugate(q2inv);

  QuaternionMathF::Multiply(q2inv, q1, qc);
  QuaternionMathF::ElementWiseAbs(qc);

  //if qc.x is smallest
  if ( qc.x <= qc.y && qc.x <= qc.z && qc.x <= qc.w)
  {
    qco.x = qc.x;
    //if qc.y is next smallest
    if (qc.y <= qc.z && qc.y <= qc.w)
    {
      qco.y = qc.y;
      if(qc.z <= qc.w) qco.z = qc.z, qco.w = qc.w;
      else qco.z = qc.w, qco.w = qc.z;
    }
    //if qc.z is next smallest
    else if (qc.z <= qc.y && qc.z <= qc.w)
    {
      qco.y = qc.z;
      if(qc.y <= qc.w) qco.z = qc.y, qco.w = qc.w;
      else qco.z = qc.w, qco.w = qc.y;
    }
    //if qc.w is next smallest
    else
    {
      qco.y = qc.w;
      if(qc.y <= qc.z) qco.z = qc.y, qco.w = qc.z;
      else qco.z = qc.z, qco.w = qc.y;
    }
  }
  //if qc.y is smallest
  else if ( qc.y <= qc.x && qc.y <= qc.z && qc.y <= qc.w)
  {
    qco.x = qc.y;
    //if qc.x is next smallest
    if (qc.x <= qc.z && qc.x <= qc.w)
    {
      qco.y = qc.x;
      if(qc.z <= qc.w) qco.z = qc.z, qco.w = qc.w;
      else qco.z = qc.w, qco.w = qc.z;
    }
    //if qc.z is next smallest
    else if (qc.z <= qc.x && qc.z <= qc.w)
    {
      qco.y = qc.z;
      if(qc.x <= qc.w) qco.z = qc.x, qco.w = qc.w;
      else qco.z = qc.w, qco.w = qc.x;
    }
    //if qc.w is next smallest
    else
    {
      qco.y = qc.w;
      if(qc.x <= qc.z) qco.z = qc.x, qco.w = qc.z;
      else qco.z = qc.z, qco.w = qc.x;
    }
  }
  //if qc.z is smallest
  else if ( qc.z <= qc.x && qc.z <= qc.y && qc.z <= qc.w)
  {
    qco.x = qc.z;
    //if qc.x is next smallest
    if (qc.x <= qc.y && qc.x <= qc.w)
    {
      qco.y = qc.x;
      if(qc.y <= qc.w) qco.z = qc.y, qco.w = qc.w;
      else qco.z = qc.w, qco.w = qc.y;
    }
    //if qc.y is next smallest
    else if (qc.y <= qc.x && qc.y <= qc.w)
    {
      qco.y = qc.y;
      if(qc.x <= qc.w) qco.z = qc.x, qco.w = qc.w;
      else qco.z = qc.w, qco.w = qc.x;
    }
    //if qc.w is next smallest
    else
    {
      qco.y = qc.w;
      if(qc.x <= qc.y) qco.z = qc.x, qco.w = qc.y;
      else qco.z = qc.y, qco.w = qc.x;
    }
  }
  //if qc.w is smallest
  else
  {
    qco.x = qc.w;
    //if qc.x is next smallest
    if (qc.x <= qc.y && qc.x <= qc.z)
    {
      qco.y = qc.x;
      if(qc.y <= qc.z) qco.z = qc.y, qco.w = qc.z;
      else qco.z = qc.z, qco.w = qc.y;
    }
    //if qc.y is next smallest
    else if (qc.y <= qc.x && qc.y <= qc.z)
    {
      qco.y = qc.y;
      if(qc.x <= qc.z) qco.z = qc.x, qco.w = qc.z;
      else qco.z = qc.z, qco.w = qc.x;
    }
    //if qc.z is next smallest
    else
    {
      qco.y = qc.z;
      if(qc.x <= qc.y) qco.z = qc.x, qco.w = qc.y;
      else qco.z = qc.y, qco.w = qc.x;
    }
  }
  wmin = qco.w;
  if (((qco.z + qco.w) / (DREAM3D::Constants::k_Sqrt2)) > wmin)
  {
    wmin = ((qco.z + qco.w) / (DREAM3D::Constants::k_Sqrt2));
    type = 2;
  }
  if (((qco.x + qco.y + qco.z + qco.w) / 2) > wmin)
  {
    wmin = ((qco.x + qco.y + qco.z + qco.w) / 2);
    type = 3;
  }
  if (wmin < -1.0)
  {
    //  wmin = -1.0;
    wmin = DREAM3D::Constants::k_ACosNeg1;
    sin_wmin_over_2 = sinf(wmin);
  }
  else if (wmin > 1.0)
  {
    //   wmin = 1.0;
    wmin = DREAM3D::Constants::k_ACos1;
    sin_wmin_over_2 = sinf(wmin);
  }
  else
  {
    wmin = acos(wmin);
    sin_wmin_over_2 = sinf(wmin);
  }

  if(type == 1)
  {
    n1 = qco.x / sin_wmin_over_2;
    n2 = qco.y / sin_wmin_over_2;
    n3 = qco.z / sin_wmin_over_2;
  }
  if(type == 2)
  {
    n1 = ((qco.x - qco.y) / (DREAM3D::Constants::k_Sqrt2)) / sin_wmin_over_2;
    n2 = ((qco.x + qco.y) / (DREAM3D::Constants::k_Sqrt2)) / sin_wmin_over_2;
    n3 = ((qco.z - qco.w) / (DREAM3D::Constants::k_Sqrt2)) / sin_wmin_over_2;
  }
  if(type == 3)
  {
    n1 = ((qco.x - qco.y + qco.z - qco.w) / (2.0f)) / sin_wmin_over_2;
    n2 = ((qco.x + qco.y - qco.z - qco.w) / (2.0f)) / sin_wmin_over_2;
    n3 = ((-qco.x + qco.y + qco.z - qco.w) / (2.0f)) / sin_wmin_over_2;
  }
  float denom = sqrt((n1*n1+n2*n2+n3*n3));
  n1 = n1/denom;
  n2 = n2/denom;
  n3 = n3/denom;
  if(denom == 0) n1 = 0.0, n2 = 0.0, n3 = 1.0;
  if(wmin == 0) n1 = 0.0, n2 = 0.0, n3 = 1.0;
  wmin = 2.0f * wmin;
  return wmin;

}

void CubicOps::getODFFZRod(float &r1,float &r2, float &r3)
{
  int numsym = 24;

  _calcRodNearestOrigin(CubicRodSym, numsym, r1, r2, r3);
}

void CubicOps::getQuatSymOp(int i, QuatF &q)
{
  QuaternionMathF::Copy(CubicQuatSym[i], q);
}

void CubicOps::getRodSymOp(int i,float *r)
{
  r[0] = CubicRodSym[i][0];
  r[1] = CubicRodSym[i][1];
  r[2] = CubicRodSym[i][2];
}

void CubicOps::getMatSymOp(int i,float g[3][3])
{
  g[0][0] = CubicMatSym[i][0][0];
  g[0][1] = CubicMatSym[i][0][1];
  g[0][2] = CubicMatSym[i][0][2];
  g[1][0] = CubicMatSym[i][1][0];
  g[1][1] = CubicMatSym[i][1][1];
  g[1][2] = CubicMatSym[i][1][2];
  g[2][0] = CubicMatSym[i][2][0];
  g[2][1] = CubicMatSym[i][2][1];
  g[2][2] = CubicMatSym[i][2][2];
}

void CubicOps::getMDFFZRod(float &r1,float &r2, float &r3)
{
  float w, n1, n2, n3;
  float FZw, FZn1, FZn2, FZn3;

  OrientationOps::_calcRodNearestOrigin(CubicRodSym, 24, r1, r2, r3);
  OrientationMath::RodtoAxisAngle(r1, r2, r3, w, n1, n2, n3);

  FZw = w;
  n1 = fabs(n1);
  n2 = fabs(n2);
  n3 = fabs(n3);
  if(n1 > n2)
  {
    if(n1 > n3)
    {
      FZn1 = n1;
      if (n2 > n3) FZn2 = n2, FZn3 = n3;
      else FZn2 = n3, FZn3 = n2;
    }
    else FZn1 = n3, FZn2 = n1, FZn3 = n2;
  }
  else
  {
    if(n2 > n3)
    {
      FZn1 = n2;
      if (n1 > n3) FZn2 = n1, FZn3 = n3;
      else FZn2 = n3, FZn3 = n1;
    }
    else FZn1 = n3, FZn2 = n2, FZn3 = n1;
  }

  OrientationMath::AxisAngletoRod(FZw, FZn1, FZn2, FZn3, r1, r2, r3);
}

void CubicOps::getNearestQuat(QuatF &q1, QuatF &q2)
{
  int numsym = 24;

  _calcNearestQuat(CubicQuatSym, numsym, q1, q2);
}

void CubicOps::getFZQuat(QuatF &qr)
{
  int numsym = 24;

  _calcQuatNearestOrigin(CubicQuatSym, numsym, qr);
}

int CubicOps::getMisoBin(float r1, float r2, float r3)
{
  float dim[3];
  float bins[3];
  float step[3];

  OrientationMath::RodtoHomochoric(r1, r2, r3);

  dim[0] = Detail::CubicDim1InitValue;
  dim[1] = Detail::CubicDim2InitValue;
  dim[2] = Detail::CubicDim3InitValue;
  step[0] = Detail::CubicDim1StepValue;
  step[1] = Detail::CubicDim2StepValue;
  step[2] = Detail::CubicDim3StepValue;
  bins[0] = 18.0f;
  bins[1] = 18.0f;
  bins[2] = 18.0f;

  return _calcMisoBin(dim, bins, step, r1, r2, r3);
}

void CubicOps::determineEulerAngles(int choose, float &synea1, float &synea2, float &synea3)
{
  float init[3];
  float step[3];
  float phi[3];
  float r1, r2, r3;

  init[0] = Detail::CubicDim1InitValue;
  init[1] = Detail::CubicDim2InitValue;
  init[2] = Detail::CubicDim3InitValue;
  step[0] = Detail::CubicDim1StepValue;
  step[1] = Detail::CubicDim2StepValue;
  step[2] = Detail::CubicDim3StepValue;
  phi[0] = static_cast<float>(choose % 18);
  phi[1] = static_cast<float>((choose / 18) % 18);
  phi[2] = static_cast<float>(choose / (18 * 18));

  _calcDetermineHomochoricValues(init, step, phi, choose, r1, r2, r3);
  OrientationMath::HomochorictoRod(r1, r2, r3);
  getODFFZRod(r1, r2, r3);
  OrientationMath::RodtoEuler(r1, r2, r3, synea1, synea2, synea3);
}

void CubicOps::determineRodriguesVector(int choose, float &r1, float &r2, float &r3)
{
  float init[3];
  float step[3];
  float phi[3];

  init[0] = Detail::CubicDim1InitValue;
  init[1] = Detail::CubicDim2InitValue;
  init[2] = Detail::CubicDim3InitValue;
  step[0] = Detail::CubicDim1StepValue;
  step[1] = Detail::CubicDim2StepValue;
  step[2] = Detail::CubicDim3StepValue;
  phi[0] = static_cast<float>(choose % 18);
  phi[1] = static_cast<float>((choose / 18) % 18);
  phi[2] = static_cast<float>(choose / (18 * 18));

  _calcDetermineHomochoricValues(init, step, phi, choose, r1, r2, r3);
  OrientationMath::HomochorictoRod(r1, r2, r3);
  getMDFFZRod(r1, r2, r3);
}
int CubicOps::getOdfBin(float r1, float r2, float r3)
{
  float dim[3];
  float bins[3];
  float step[3];

  OrientationMath::RodtoHomochoric(r1, r2, r3);

  dim[0] = Detail::CubicDim1InitValue;
  dim[1] = Detail::CubicDim2InitValue;
  dim[2] = Detail::CubicDim3InitValue;
  step[0] = Detail::CubicDim1StepValue;
  step[1] = Detail::CubicDim2StepValue;
  step[2] = Detail::CubicDim3StepValue;
  bins[0] = 18.0f;
  bins[1] = 18.0f;
  bins[2] = 18.0f;

  return _calcODFBin(dim, bins, step, r1, r2, r3);
}

void CubicOps::getSchmidFactorAndSS(float loadx, float loady, float loadz, float &schmidfactor, int &slipsys)
{
  float theta1, theta2, theta3, theta4;
  float lambda1, lambda2, lambda3, lambda4, lambda5, lambda6;
  float schmid1, schmid2, schmid3, schmid4, schmid5, schmid6, schmid7, schmid8, schmid9, schmid10, schmid11, schmid12;

  float mag = loadx * loadx + loady * loady + loadz * loadz;
  mag = sqrt(mag);
  theta1 = (loadx + loady + loadz) / (mag * 1.732f);
  theta1 = fabs(theta1);
  theta2 = (loadx + loady - loadz) / (mag * 1.732f);
  theta2 = fabs(theta2);
  theta3 = (loadx - loady + loadz) / (mag * 1.732f);
  theta3 = fabs(theta3);
  theta4 = (-loadx + loady + loadz) / (mag * 1.732f);
  theta4 = fabs(theta4);
  lambda1 = (loadx + loady) / (mag * 1.414f);
  lambda1 = fabs(lambda1);
  lambda2 = (loadx + loadz) / (mag * 1.414f);
  lambda2 = fabs(lambda2);
  lambda3 = (loadx - loady) / (mag * 1.414f);
  lambda3 = fabs(lambda3);
  lambda4 = (loadx - loadz) / (mag * 1.414f);
  lambda4 = fabs(lambda4);
  lambda5 = (loady + loadz) / (mag * 1.414f);
  lambda5 = fabs(lambda5);
  lambda6 = (loady - loadz) / (mag * 1.414f);
  lambda6 = fabs(lambda6);
  schmid1 = theta1 * lambda6;
  schmid2 = theta1 * lambda4;
  schmid3 = theta1 * lambda3;
  schmid4 = theta2 * lambda3;
  schmid5 = theta2 * lambda2;
  schmid6 = theta2 * lambda5;
  schmid7 = theta3 * lambda1;
  schmid8 = theta3 * lambda5;
  schmid9 = theta3 * lambda4;
  schmid10 = theta4 * lambda1;
  schmid11 = theta4 * lambda2;
  schmid12 = theta4 * lambda6;
  schmidfactor = schmid1, slipsys = 0;

  if (schmid2 > schmidfactor) schmidfactor = schmid2, slipsys = 1;
  if (schmid3 > schmidfactor) schmidfactor = schmid3, slipsys = 2;
  if (schmid4 > schmidfactor) schmidfactor = schmid4, slipsys = 3;
  if (schmid5 > schmidfactor) schmidfactor = schmid5, slipsys = 4;
  if (schmid6 > schmidfactor) schmidfactor = schmid6, slipsys = 5;
  if (schmid7 > schmidfactor) schmidfactor = schmid7, slipsys = 6;
  if (schmid8 > schmidfactor) schmidfactor = schmid8, slipsys = 7;
  if (schmid9 > schmidfactor) schmidfactor = schmid9, slipsys = 8;
  if (schmid10 > schmidfactor) schmidfactor = schmid10, slipsys = 9;
  if (schmid11 > schmidfactor) schmidfactor = schmid11, slipsys = 10;
  if (schmid12 > schmidfactor) schmidfactor = schmid12, slipsys = 11;
}

void CubicOps::getmPrime(QuatF &q1, QuatF &q2, float LD[3], float &mPrime)
{
  float g1[3][3];
  float g2[3][3];
  float hkl1[3], uvw1[3];
  float hkl2[3], uvw2[3];
  float slipDirection[3], slipPlane[3];
  float schmidFactor1=0, schmidFactor2=0, maxSchmidFactor=0;
  float directionComponent1=0, planeComponent1=0;
  float directionComponent2=0, planeComponent2=0;
  float planeMisalignment=0, directionMisalignment=0;
  int ss1=0, ss2=0;

  OrientationMath::QuattoMat(q1, g1);
  OrientationMath::QuattoMat(q2, g2);
  MatrixMath::Transpose3x3(g1, g1);
  MatrixMath::Transpose3x3(g2, g2);
  for(int i=0;i<12;i++)
  {
    slipDirection[0] = CubicSlipDirections[i][0];
    slipDirection[1] = CubicSlipDirections[i][1];
    slipDirection[2] = CubicSlipDirections[i][2];
    slipPlane[0] = CubicSlipPlanes[i][0];
    slipPlane[1] = CubicSlipPlanes[i][1];
    slipPlane[2] = CubicSlipPlanes[i][2];
    MatrixMath::Multiply3x3with3x1(g1,slipDirection,hkl1);
    MatrixMath::Multiply3x3with3x1(g1,slipPlane,uvw1);
    MatrixMath::Normalize3x1(hkl1);
    MatrixMath::Normalize3x1(uvw1);
    directionComponent1 = fabs(MatrixMath::DotProduct(LD,uvw1));
    planeComponent1 = fabs(MatrixMath::DotProduct(LD,hkl1));
    schmidFactor1 = directionComponent1*planeComponent1;
    if(schmidFactor1 > maxSchmidFactor)
    {
      maxSchmidFactor = schmidFactor1;
      ss1 = i;
    }
  }
  slipDirection[0] = CubicSlipDirections[ss1][0];
  slipDirection[1] = CubicSlipDirections[ss1][1];
  slipDirection[2] = CubicSlipDirections[ss1][2];
  slipPlane[0] = CubicSlipPlanes[ss1][0];
  slipPlane[1] = CubicSlipPlanes[ss1][1];
  slipPlane[2] = CubicSlipPlanes[ss1][2];
  MatrixMath::Multiply3x3with3x1(g1,slipDirection,hkl1);
  MatrixMath::Multiply3x3with3x1(g1,slipPlane,uvw1);
  MatrixMath::Normalize3x1(hkl1);
  MatrixMath::Normalize3x1(uvw1);

  maxSchmidFactor = 0;
  for(int j=0;j<12;j++)
  {
    slipDirection[0] = CubicSlipDirections[j][0];
    slipDirection[1] = CubicSlipDirections[j][1];
    slipDirection[2] = CubicSlipDirections[j][2];
    slipPlane[0] = CubicSlipPlanes[j][0];
    slipPlane[1] = CubicSlipPlanes[j][1];
    slipPlane[2] = CubicSlipPlanes[j][2];
    MatrixMath::Multiply3x3with3x1(g2,slipDirection,hkl2);
    MatrixMath::Multiply3x3with3x1(g2,slipPlane,uvw2);
    MatrixMath::Normalize3x1(hkl2);
    MatrixMath::Normalize3x1(uvw2);
    directionComponent2 = fabs(MatrixMath::DotProduct(LD,uvw2));
    planeComponent2 = fabs(MatrixMath::DotProduct(LD,hkl2));
    schmidFactor2 = directionComponent2*planeComponent2;
    if(schmidFactor2 > maxSchmidFactor)
    {
      maxSchmidFactor = schmidFactor2;
      ss2 = j;
    }
  }
  slipDirection[0] = CubicSlipDirections[ss2][0];
  slipDirection[1] = CubicSlipDirections[ss2][1];
  slipDirection[2] = CubicSlipDirections[ss2][2];
  slipPlane[0] = CubicSlipPlanes[ss2][0];
  slipPlane[1] = CubicSlipPlanes[ss2][1];
  slipPlane[2] = CubicSlipPlanes[ss2][2];
  MatrixMath::Multiply3x3with3x1(g2,slipDirection,hkl2);
  MatrixMath::Multiply3x3with3x1(g2,slipPlane,uvw2);
  MatrixMath::Normalize3x1(hkl2);
  MatrixMath::Normalize3x1(uvw2);
  planeMisalignment = fabs(MatrixMath::DotProduct(hkl1,hkl2));
  directionMisalignment = fabs(MatrixMath::DotProduct(uvw1,uvw2));
  mPrime = planeMisalignment*directionMisalignment;
}

void CubicOps::getF1(QuatF &q1, QuatF &q2, float LD[3], bool maxSF, float &F1)
{
  float g1[3][3];
  float g2[3][3];
  float hkl1[3], uvw1[3];
  float hkl2[3], uvw2[3];
  float slipDirection[3], slipPlane[3];
  float directionMisalignment=0, totalDirectionMisalignment=0;
  float schmidFactor1=0, schmidFactor2=0, maxSchmidFactor=0;
  float directionComponent1=0, planeComponent1=0;
  float directionComponent2=0, planeComponent2=0;
  float maxF1=0;

  OrientationMath::QuattoMat(q1, g1);
  OrientationMath::QuattoMat(q2, g2);
  MatrixMath::Transpose3x3(g1, g1);
  MatrixMath::Transpose3x3(g2, g2);

  MatrixMath::Normalize3x1(LD);

  if(maxSF == true) maxSchmidFactor = 0;
  for(int i=0;i<12;i++)
  {
    slipDirection[0] = CubicSlipDirections[i][0];
    slipDirection[1] = CubicSlipDirections[i][1];
    slipDirection[2] = CubicSlipDirections[i][2];
    slipPlane[0] = CubicSlipPlanes[i][0];
    slipPlane[1] = CubicSlipPlanes[i][1];
    slipPlane[2] = CubicSlipPlanes[i][2];
    MatrixMath::Multiply3x3with3x1(g1,slipDirection,hkl1);
    MatrixMath::Multiply3x3with3x1(g1,slipPlane,uvw1);
    MatrixMath::Normalize3x1(hkl1);
    MatrixMath::Normalize3x1(uvw1);
    directionComponent1 = fabs(MatrixMath::DotProduct(LD,uvw1));
    planeComponent1 = fabs(MatrixMath::DotProduct(LD,hkl1));
    schmidFactor1 = directionComponent1*planeComponent1;
    if(schmidFactor1 > maxSchmidFactor || maxSF == false)
    {
      totalDirectionMisalignment = 0;
      if(maxSF == true) maxSchmidFactor = schmidFactor1;
      for(int j=0;j<12;j++)
      {
        slipDirection[0] = CubicSlipDirections[j][0];
        slipDirection[1] = CubicSlipDirections[j][1];
        slipDirection[2] = CubicSlipDirections[j][2];
        slipPlane[0] = CubicSlipPlanes[j][0];
        slipPlane[1] = CubicSlipPlanes[j][1];
        slipPlane[2] = CubicSlipPlanes[j][2];
        MatrixMath::Multiply3x3with3x1(g2,slipDirection,hkl2);
        MatrixMath::Multiply3x3with3x1(g2,slipPlane,uvw2);
        MatrixMath::Normalize3x1(hkl2);
        MatrixMath::Normalize3x1(uvw2);
        directionComponent2 = fabs(MatrixMath::DotProduct(LD,uvw2));
        planeComponent2 = fabs(MatrixMath::DotProduct(LD,hkl2));
        schmidFactor2 = directionComponent2*planeComponent2;
        directionMisalignment = fabs(MatrixMath::DotProduct(uvw1,uvw2));
        totalDirectionMisalignment = totalDirectionMisalignment + directionMisalignment;
      }
      F1 = schmidFactor1*directionComponent1*totalDirectionMisalignment;
      if(maxSF == false)
      {
        if(F1 < maxF1) F1 = maxF1;
        else maxF1 = F1;
      }
    }
  }
}

void CubicOps::getF1spt(QuatF &q1, QuatF &q2, float LD[3], bool maxSF, float &F1spt)
{
  float g1[3][3];
  float g2[3][3];
  float hkl1[3], uvw1[3];
  float hkl2[3], uvw2[3];
  float slipDirection[3], slipPlane[3];
  float directionMisalignment=0, totalDirectionMisalignment=0;
  float planeMisalignment=0, totalPlaneMisalignment=0;
  float schmidFactor1=0, schmidFactor2=0, maxSchmidFactor=0;
  float directionComponent1=0, planeComponent1=0;
  float directionComponent2=0, planeComponent2=0;
  float maxF1spt=0;

  OrientationMath::QuattoMat(q1, g1);
  OrientationMath::QuattoMat(q2, g2);
  MatrixMath::Transpose3x3(g1, g1);
  MatrixMath::Transpose3x3(g2, g2);

  MatrixMath::Normalize3x1(LD);

  if(maxSF == true) maxSchmidFactor = 0;
  for(int i=0;i<12;i++)
  {
    slipDirection[0] = CubicSlipDirections[i][0];
    slipDirection[1] = CubicSlipDirections[i][1];
    slipDirection[2] = CubicSlipDirections[i][2];
    slipPlane[0] = CubicSlipPlanes[i][0];
    slipPlane[1] = CubicSlipPlanes[i][1];
    slipPlane[2] = CubicSlipPlanes[i][2];
    MatrixMath::Multiply3x3with3x1(g1,slipDirection,hkl1);
    MatrixMath::Multiply3x3with3x1(g1,slipPlane,uvw1);
    MatrixMath::Normalize3x1(hkl1);
    MatrixMath::Normalize3x1(uvw1);
    directionComponent1 = fabs(MatrixMath::DotProduct(LD,uvw1));
    planeComponent1 = fabs(MatrixMath::DotProduct(LD,hkl1));
    schmidFactor1 = directionComponent1*planeComponent1;
    if(schmidFactor1 > maxSchmidFactor || maxSF == false)
    {
      totalDirectionMisalignment = 0;
      totalPlaneMisalignment = 0;
      if(maxSF == true) maxSchmidFactor = schmidFactor1;
      for(int j=0;j<12;j++)
      {
        slipDirection[0] = CubicSlipDirections[j][0];
        slipDirection[1] = CubicSlipDirections[j][1];
        slipDirection[2] = CubicSlipDirections[j][2];
        slipPlane[0] = CubicSlipPlanes[j][0];
        slipPlane[1] = CubicSlipPlanes[j][1];
        slipPlane[2] = CubicSlipPlanes[j][2];
        MatrixMath::Multiply3x3with3x1(g2,slipDirection,hkl2);
        MatrixMath::Multiply3x3with3x1(g2,slipPlane,uvw2);
        MatrixMath::Normalize3x1(hkl2);
        MatrixMath::Normalize3x1(uvw2);
        directionComponent2 = fabs(MatrixMath::DotProduct(LD,uvw2));
        planeComponent2 = fabs(MatrixMath::DotProduct(LD,hkl2));
        schmidFactor2 = directionComponent2*planeComponent2;
        directionMisalignment = fabs(MatrixMath::DotProduct(uvw1,uvw2));
        planeMisalignment = fabs(MatrixMath::DotProduct(hkl1,hkl2));
        totalDirectionMisalignment = totalDirectionMisalignment + directionMisalignment;
        totalPlaneMisalignment = totalPlaneMisalignment + planeMisalignment;
      }
      F1spt = schmidFactor1*directionComponent1*totalDirectionMisalignment*totalPlaneMisalignment;
      if(maxSF == false)
      {
        if(F1spt < maxF1spt) F1spt = maxF1spt;
        else maxF1spt = F1spt;
      }
    }
  }
}

void CubicOps::getF7(QuatF &q1, QuatF &q2, float LD[3], bool maxSF, float &F7)
{
  float g1[3][3];
  float g2[3][3];
  float hkl1[3], uvw1[3];
  float hkl2[3], uvw2[3];
  float slipDirection[3], slipPlane[3];
  float directionMisalignment=0, totalDirectionMisalignment=0;
  float schmidFactor1=0, schmidFactor2=0, maxSchmidFactor=0;
  float directionComponent1=0, planeComponent1=0;
  float directionComponent2=0, planeComponent2=0;
  float maxF7=0;

  OrientationMath::QuattoMat(q1, g1);
  OrientationMath::QuattoMat(q2, g2);
  MatrixMath::Transpose3x3(g1, g1);
  MatrixMath::Transpose3x3(g2, g2);

  MatrixMath::Normalize3x1(LD);

  for(int i=0;i<12;i++)
  {
    slipDirection[0] = CubicSlipDirections[i][0];
    slipDirection[1] = CubicSlipDirections[i][1];
    slipDirection[2] = CubicSlipDirections[i][2];
    slipPlane[0] = CubicSlipPlanes[i][0];
    slipPlane[1] = CubicSlipPlanes[i][1];
    slipPlane[2] = CubicSlipPlanes[i][2];
    MatrixMath::Multiply3x3with3x1(g1,slipDirection,hkl1);
    MatrixMath::Multiply3x3with3x1(g1,slipPlane,uvw1);
    MatrixMath::Normalize3x1(hkl1);
    MatrixMath::Normalize3x1(uvw1);
    directionComponent1 = fabs(MatrixMath::DotProduct(LD,uvw1));
    planeComponent1 = fabs(MatrixMath::DotProduct(LD,hkl1));
    schmidFactor1 = directionComponent1*planeComponent1;
    if(schmidFactor1 > maxSchmidFactor || maxSF == false)
    {
      totalDirectionMisalignment = 0;
      if(maxSF == true) maxSchmidFactor = schmidFactor1;
      for(int j=0;j<12;j++)
      {
        slipDirection[0] = CubicSlipDirections[j][0];
        slipDirection[1] = CubicSlipDirections[j][1];
        slipDirection[2] = CubicSlipDirections[j][2];
        slipPlane[0] = CubicSlipPlanes[j][0];
        slipPlane[1] = CubicSlipPlanes[j][1];
        slipPlane[2] = CubicSlipPlanes[j][2];
        MatrixMath::Multiply3x3with3x1(g2,slipDirection,hkl2);
        MatrixMath::Multiply3x3with3x1(g2,slipPlane,uvw2);
        MatrixMath::Normalize3x1(hkl2);
        MatrixMath::Normalize3x1(uvw2);
        directionComponent2 = fabs(MatrixMath::DotProduct(LD,uvw2));
        planeComponent2 = fabs(MatrixMath::DotProduct(LD,hkl2));
        schmidFactor2 = directionComponent2*planeComponent2;
        directionMisalignment = fabs(MatrixMath::DotProduct(uvw1,uvw2));
        totalDirectionMisalignment = totalDirectionMisalignment + directionMisalignment;
      }
      F7 = directionComponent1*directionComponent1*totalDirectionMisalignment;
      if(maxSF == false)
      {
        if(F7 < maxF7) F7 = maxF7;
        else maxF7 = F7;
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CubicOps::generateSphereCoordsFromEulers(FloatArrayType *eulers, FloatArrayType *xyz001, FloatArrayType *xyz011, FloatArrayType *xyz111)
{
  size_t nOrientations = eulers->GetNumberOfTuples();
  CubicOps ops;
  float g[3][3];
  float gTranpose[3][3];
  float* currentEuler = NULL;
  float direction[3] = {0.0, 0.0, 0.0};

  // Sanity Check the size of the arrays
  if (xyz001->GetNumberOfTuples() < nOrientations * 6)
  {
    xyz001->Resize(nOrientations * 6 * 3);
  }
  if (xyz011->GetNumberOfTuples() < nOrientations * 12)
  {
    xyz011->Resize(nOrientations * 12 * 3);
  }
  if (xyz111->GetNumberOfTuples() < nOrientations * 8)
  {
    xyz111->Resize(nOrientations * 8 * 3);
  }

  // Geneate all the Coordinates
  for(size_t i = 0; i < nOrientations; ++i)
  {

    currentEuler = eulers->GetPointer(i * 3);

    OrientationMath::EulertoMat(currentEuler[0], currentEuler[1], currentEuler[2], g);
    MatrixMath::Transpose3x3(g, gTranpose);

    // -----------------------------------------------------------------------------
    // 001 Family
    direction[0] = 1.0; direction[1] = 0.0; direction[2] = 0.0;
    MatrixMath::Multiply3x3with3x1(gTranpose, direction, xyz001->GetPointer(i*18));
    MatrixMath::Copy3x1(xyz001->GetPointer(i*18),xyz001->GetPointer(i*18 + 3));
    MatrixMath::Multiply3x1withConstant(xyz001->GetPointer(i*18 + 3),-1);
    direction[0] = 0.0; direction[1] = 1.0; direction[2] = 0.0;
    MatrixMath::Multiply3x3with3x1(gTranpose, direction, xyz001->GetPointer(i*18 + 6));
    MatrixMath::Copy3x1(xyz001->GetPointer(i*18 + 6),xyz001->GetPointer(i*18 + 9));
    MatrixMath::Multiply3x1withConstant(xyz001->GetPointer(i*18 + 9),-1);
    direction[0] = 0.0; direction[1] = 0.0; direction[2] = 1.0;
    MatrixMath::Multiply3x3with3x1(gTranpose, direction, xyz001->GetPointer(i*18 + 12));
    MatrixMath::Copy3x1(xyz001->GetPointer(i*18 + 12),xyz001->GetPointer(i*18 + 15));
    MatrixMath::Multiply3x1withConstant(xyz001->GetPointer(i*18 + 15),-1);

    // -----------------------------------------------------------------------------
    // 011 Family
    direction[0] = DREAM3D::Constants::k_1OverRoot2; direction[1] = DREAM3D::Constants::k_1OverRoot2; direction[2] = 0.0;
    MatrixMath::Multiply3x3with3x1(gTranpose, direction, xyz011->GetPointer(i*36));
    MatrixMath::Copy3x1(xyz011->GetPointer(i*36),xyz011->GetPointer(i*36 + 3));
    MatrixMath::Multiply3x1withConstant(xyz011->GetPointer(i*36 + 3),-1);
    direction[0] = DREAM3D::Constants::k_1OverRoot2; direction[1] = 0.0; direction[2] = DREAM3D::Constants::k_1OverRoot2;
    MatrixMath::Multiply3x3with3x1(gTranpose, direction, xyz011->GetPointer(i*36 + 6));
    MatrixMath::Copy3x1(xyz011->GetPointer(i*36+6),xyz011->GetPointer(i*36 + 9));
    MatrixMath::Multiply3x1withConstant(xyz011->GetPointer(i*36 + 9),-1);
    direction[0] = 0.0; direction[1] = DREAM3D::Constants::k_1OverRoot2; direction[2] = DREAM3D::Constants::k_1OverRoot2;
    MatrixMath::Multiply3x3with3x1(gTranpose, direction, xyz011->GetPointer(i*36 + 12));
    MatrixMath::Copy3x1(xyz011->GetPointer(i*36+12),xyz011->GetPointer(i*36 + 15));
    MatrixMath::Multiply3x1withConstant(xyz011->GetPointer(i*36 + 15),-1);
    direction[0] = -DREAM3D::Constants::k_1OverRoot2; direction[1] = -DREAM3D::Constants::k_1OverRoot2; direction[2] = 0.0;
    MatrixMath::Multiply3x3with3x1(gTranpose, direction, xyz011->GetPointer(i*36 + 18));
    MatrixMath::Copy3x1(xyz011->GetPointer(i*36+18),xyz011->GetPointer(i*36 + 21));
    MatrixMath::Multiply3x1withConstant(xyz011->GetPointer(i*36 + 21),-1);
    direction[0] = -DREAM3D::Constants::k_1OverRoot2; direction[1] = 0.0; direction[2] = DREAM3D::Constants::k_1OverRoot2;
    MatrixMath::Multiply3x3with3x1(gTranpose, direction, xyz011->GetPointer(i*36 + 24));
    MatrixMath::Copy3x1(xyz011->GetPointer(i*36+24),xyz011->GetPointer(i*36 + 27));
    MatrixMath::Multiply3x1withConstant(xyz011->GetPointer(i*36 + 27),-1);
    direction[0] = 0.0; direction[1] = -DREAM3D::Constants::k_1OverRoot2; direction[2] = DREAM3D::Constants::k_1OverRoot2;
    MatrixMath::Multiply3x3with3x1(gTranpose, direction, xyz011->GetPointer(i*36 + 30));
    MatrixMath::Copy3x1(xyz011->GetPointer(i*36+30),xyz011->GetPointer(i*36 + 33));
    MatrixMath::Multiply3x1withConstant(xyz011->GetPointer(i*36 + 33),-1);

    // -----------------------------------------------------------------------------
    // 111 Family
    direction[0] = DREAM3D::Constants::k_1OverRoot3; direction[1] = DREAM3D::Constants::k_1OverRoot3; direction[2] = DREAM3D::Constants::k_1OverRoot3;
    MatrixMath::Multiply3x3with3x1(gTranpose, direction, xyz111->GetPointer(i*24));
    MatrixMath::Copy3x1(xyz111->GetPointer(i*24),xyz111->GetPointer(i*24 + 3));
    MatrixMath::Multiply3x1withConstant(xyz111->GetPointer(i*24 + 3),-1);
    direction[0] = -DREAM3D::Constants::k_1OverRoot3; direction[1] = DREAM3D::Constants::k_1OverRoot3; direction[2] = DREAM3D::Constants::k_1OverRoot3;
    MatrixMath::Multiply3x3with3x1(gTranpose, direction, xyz111->GetPointer(i*24 + 6));
    MatrixMath::Copy3x1(xyz111->GetPointer(i*24+6),xyz111->GetPointer(i*24 + 9));
    MatrixMath::Multiply3x1withConstant(xyz111->GetPointer(i*24 + 9),-1);
    direction[0] = DREAM3D::Constants::k_1OverRoot3; direction[1] = -DREAM3D::Constants::k_1OverRoot3; direction[2] = DREAM3D::Constants::k_1OverRoot3;
    MatrixMath::Multiply3x3with3x1(gTranpose, direction, xyz111->GetPointer(i*24 + 12));
    MatrixMath::Copy3x1(xyz111->GetPointer(i*24+12),xyz111->GetPointer(i*24 + 15));
    MatrixMath::Multiply3x1withConstant(xyz111->GetPointer(i*24 + 15),-1);
    direction[0] = DREAM3D::Constants::k_1OverRoot3; direction[1] = DREAM3D::Constants::k_1OverRoot3; direction[2] = -DREAM3D::Constants::k_1OverRoot3;
    MatrixMath::Multiply3x3with3x1(gTranpose, direction, xyz111->GetPointer(i*24 + 18));
    MatrixMath::Copy3x1(xyz111->GetPointer(i*24+18),xyz111->GetPointer(i*24 + 21));
    MatrixMath::Multiply3x1withConstant(xyz111->GetPointer(i*24 + 21),-1);
  }

}
/**
 * @brief Sorts the 3 values from low to high
 * @param a
 * @param b
 * @param c
 * @param sorted The array to store the sorted values.
 */
template<typename T>
void _TripletSort(T a, T b, T c, T* sorted)
{
  if ( a > b && a > c)
  {
    sorted[2] = a;
    if (b > c) { sorted[1] = b; sorted[0] = c; }
    else { sorted[1] = c; sorted[0] = b; }
  }
  else if ( b > a && b > c)
  {
    sorted[2] = b;
    if (a > c) { sorted[1] = a; sorted[0] = c; }
    else { sorted[1] = c; sorted[0] = a; }
  }
  else if ( a > b )
  {
    sorted[1] = a; sorted[0] = b; sorted[2] = c;
  }
  else if (a >= c && b >=c)
  {
    sorted[0] = c; sorted[1] = a; sorted[2] = b;
  }
  else
  { sorted[0] = a; sorted[1] = b; sorted[2] = c;}
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CubicOps::generateIPFColor(double* eulers, double* refDir, uint8_t* rgb, bool convertDegrees)
{
  generateIPFColor(eulers[0], eulers[1], eulers[2], refDir[0], refDir[1], refDir[2], rgb, convertDegrees);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CubicOps::generateIPFColor(double phi1, double phi, double phi2, double refDir0, double refDir1, double refDir2, uint8_t* rgb, bool degToRad)
{
  if (degToRad == true)
  {
    phi1 = phi1 * DREAM3D::Constants::k_DegToRad;
    phi = phi * DREAM3D::Constants::k_DegToRad;
    phi2 = phi2 * DREAM3D::Constants::k_DegToRad;
  }
  float g[3][3]; // Rotation Matrix?
  float cd[3];
  float d[3];
  float _rgb[3] = { 0.0, 0.0, 0.0 };

  // 1) find rotation matrix from Euler angles
  OrientationMath::EulertoMat(phi1, phi, phi2, g);

  // 2) use rotation matrix to find which crystal direction is aligned with 001
  float refDirection[3] = {refDir0, refDir1, refDir2};
  //      refDirection[0] = refDir0;
  //      refDirection[1] = refDir1;
  //      refDirection[2] = refDir2;
  MatrixMath::Multiply3x3with3x1(g, refDirection, cd);

  //3) move that direction to a single standard triangle - using the 001-011-111 triangle)
  cd[0] = fabs(cd[0]);
  cd[1] = fabs(cd[1]);
  cd[2] = fabs(cd[2]);

  // Sort the cd array from smallest to largest
  _TripletSort(cd[0], cd[1], cd[2], cd);

  float redDir[3] = {0,0,1};
  float theta1 = MatrixMath::DotProduct(cd, redDir);
  theta1 = (DREAM3D::Constants::k_RadToDeg) * acos(theta1);
  d[0] = (cd[1] * 1) - (cd[2] * 0);
  d[1] = (cd[2] * 0) - (cd[0] * 1);
  d[2] = (cd[0] * 0) - (cd[1] * 0);
  d[0] = -(d[1] + d[2]) / d[0];
  d[1] = 1;
  d[2] = 1;
  float theta2 = MatrixMath::DotProduct(d, redDir);
  theta2 = (DREAM3D::Constants::k_RadToDeg) * acos(theta2);
  _rgb[0] = (theta2-theta1)/theta2;
  float norm = sqrt(((d[0] * d[0]) + (d[1] * d[1]) + (d[2] * d[2])));
  d[0] = d[0] / norm;
  d[1] = d[1] / norm;
  d[2] = d[2] / norm;
  float phi_local = (d[0] * 0) + (d[1] * DREAM3D::Constants::k_HalfSqrt2) + (d[2] * DREAM3D::Constants::k_HalfSqrt2);
  phi_local = (DREAM3D::Constants::k_RadToDeg) * acos(phi_local);
  _rgb[1] = (1 - _rgb[0]) * ((35.26f - phi_local) / 35.26f);
  _rgb[2] = (1 - _rgb[0]) - _rgb[1];
  float max = _rgb[0];
  if (_rgb[1] > max) max = _rgb[1];
  if (_rgb[2] > max) max = _rgb[2];

  _rgb[0] = _rgb[0] / max;
  _rgb[1] = _rgb[1] / max;
  _rgb[2] = _rgb[2] / max;
  //_rgb[0] = (0.6f * _rgb[0]) + 0.40f;
  //_rgb[1] = (0.6f * _rgb[1]) + 0.40f;
  //_rgb[2] = (0.6f * _rgb[2]) + 0.40f;

  // Multiply by 255 to get an R/G/B value
  _rgb[0] = _rgb[0] * 255.0f;
  _rgb[1] = _rgb[1] * 255.0f;
  _rgb[2] = _rgb[2] * 255.0f;

  rgb[0] = static_cast<unsigned char>(_rgb[0]);
  rgb[1] = static_cast<unsigned char>(_rgb[1]);
  rgb[2] = static_cast<unsigned char>(_rgb[2]);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CubicOps::generateRodriguesColor(float r1, float r2, float r3, unsigned char* rgb)
{
  float range1 = 2.0f*CubicDim1InitValue;
  float range2 = 2.0f*CubicDim2InitValue;
  float range3 = 2.0f*CubicDim3InitValue;
  float max1 = range1/2.0f;
  float max2 = range2/2.0f;
  float max3 = range3/2.0f;
  float red = (r1+max1)/range1;
  float green = (r2+max2)/range2;
  float blue = (r3+max3)/range3;

  // Multiply by 255 to get an R/G/B value
  red = red * 255.0f;
  green = green * 255.0f;
  blue = blue * 255.0f;

  rgb[0] = static_cast<unsigned char> (red);
  rgb[1] = static_cast<unsigned char> (green);
  rgb[2] = static_cast<unsigned char> (blue);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<UInt8ArrayType::Pointer> CubicOps::generatePoleFigure(FloatArrayType* eulers, int poleFigureDimension, int lambertDimension, int numColors)
{
  std::vector<UInt8ArrayType::Pointer> poleFigures;

  int numOrientations = eulers->GetNumberOfTuples();

  // Create an Array to hold the XYZ Coordinates which are the coords on the sphere.
  // this is size for CUBIC ONLY, <001> Family
  FloatArrayType::Pointer xyz001 = FloatArrayType::CreateArray(numOrientations * 6, 3, "Cubic_<001>_xyzCoords");
  // this is size for CUBIC ONLY, <011> Family
  FloatArrayType::Pointer xyz011 = FloatArrayType::CreateArray(numOrientations * 12, 3, "Cubic_<011>_xyzCoords");
  // this is size for CUBIC ONLY, <111> Family
  FloatArrayType::Pointer xyz111 = FloatArrayType::CreateArray(numOrientations * 8, 3, "Cubic_<111>_xyzCoords");


  float sphereRadius = 1.0f;

  // Generate the coords on the sphere
  generateSphereCoordsFromEulers(eulers, xyz001.get(), xyz011.get(), xyz111.get());
#if WRITE_XYZ_SPHERE_COORD_VTK
  writeVtkFile(xyz001.get(), "c:/Users/GroebeMA/Desktop/Sphere_XYZ_FROM_EULER_001.vtk");
  writeVtkFile(xyz011.get(), "c:/Users/GroebeMA/Desktop/Sphere_XYZ_FROM_EULER_011.vtk");
  writeVtkFile(xyz111.get(), "c:/Users/GroebeMA/Desktop/Sphere_XYZ_FROM_EULER_111.vtk");
#endif
  // These arrays hold the "intensity" images which eventually get converted to an actual Color RGB image
  // Generate the modified Lambert projection images (Squares, 2 of them, 1 for northern hemisphere, 1 for southern hemisphere
  ModifiedLambertProjection::Pointer lambert = ModifiedLambertProjection::CreateProjectionFromXYZCoords(xyz001.get(), lambertDimension, sphereRadius);

  // Now create the intensity image that will become the actual Pole figure image
  {
    DoubleArrayType::Pointer intensity001 = lambert->createStereographicProjection(poleFigureDimension);
    intensity001->SetName("PoleFigure_<001>");
    UInt8ArrayType::Pointer image = ImageUtilities::CreateColorImage(intensity001.get(), poleFigureDimension, poleFigureDimension, numColors, "Cubic PoleFigure <001>");
    poleFigures.push_back(image);
  }

#if WRITE_LAMBERT_SQUARES
  size_t dims[3] = {lambert->getDimension(), lambert->getDimension(), 1 };
  float res[3] = {lambert->getStepSize(), lambert->getStepSize(), lambert->getStepSize() };
  DoubleArrayType::Pointer north = lambert->getNorthSquare();
  DoubleArrayType::Pointer south = lambert->getSouthSquare();
  VtkRectilinearGridWriter::WriteDataArrayToFile("/tmp/ModifiedLambert_North.vtk", north.get(), dims, res, "double", true);
  VtkRectilinearGridWriter::WriteDataArrayToFile("/tmp/ModifiedLambert_South.vtk", south.get(), dims, res, "double", true);
#endif

  // Generate the <011> pole figure which will generate a new set of Lambert Squares
  {
    lambert = ModifiedLambertProjection::CreateProjectionFromXYZCoords(xyz011.get(), lambertDimension, sphereRadius);
    DoubleArrayType::Pointer intensity011 = lambert->createStereographicProjection(poleFigureDimension);
    intensity011->SetName("PoleFigure_<011>");
    UInt8ArrayType::Pointer image = ImageUtilities::CreateColorImage(intensity011.get(), poleFigureDimension, poleFigureDimension, numColors, "Cubic PoleFigure <011>");
    poleFigures.push_back(image);
  }
  // Generate the <111> pole figure which will generate a new set of Lambert Squares
  {
    lambert = ModifiedLambertProjection::CreateProjectionFromXYZCoords(xyz111.get(), lambertDimension, sphereRadius);
    DoubleArrayType::Pointer intensity111 = lambert->createStereographicProjection(poleFigureDimension);
    intensity111->SetName("PoleFigure_<111>");
    UInt8ArrayType::Pointer image = ImageUtilities::CreateColorImage(intensity111.get(), poleFigureDimension, poleFigureDimension, numColors, "Cubic PoleFigure <111>");
    poleFigures.push_back(image);
  }

  return poleFigures;
}
