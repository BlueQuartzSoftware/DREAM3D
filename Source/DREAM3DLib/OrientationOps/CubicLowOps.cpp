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
#include "CubicLowOps.h"
// Include this FIRST because there is a needed define for some compiles
// to expose some of the constants needed below
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Math/OrientationMath.h"

namespace Detail
{
  static const float CubicLowDim1InitValue = powf((0.75f*((DREAM3D::Constants::k_Pi/2.0f)-sinf((DREAM3D::Constants::k_Pi/2.0f)))),(1.0f/3.0f));
  static const float CubicLowDim2InitValue = powf((0.75f*((DREAM3D::Constants::k_Pi/2.0f)-sinf((DREAM3D::Constants::k_Pi/2.0f)))),(1.0f/3.0f));
  static const float CubicLowDim3InitValue = powf((0.75f*((DREAM3D::Constants::k_Pi/2.0f)-sinf((DREAM3D::Constants::k_Pi/2.0f)))),(1.0f/3.0f));
  static const float CubicLowDim1StepValue = CubicLowDim1InitValue/18.0f;
  static const float CubicLowDim2StepValue = CubicLowDim2InitValue/18.0f;
  static const float CubicLowDim3StepValue = CubicLowDim3InitValue/18.0f;
}

static const QuatF CubicLowQuatSym[12] = {
  QuaternionMathF::New(0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f),
  QuaternionMathF::New(1.000000000f, 0.000000000f, 0.000000000f, 0.000000000f),
  QuaternionMathF::New(0.000000000f, 1.000000000f, 0.000000000f, 0.000000000f),
  QuaternionMathF::New(0.000000000f, 0.000000000f, 1.000000000f, 0.000000000f),
  QuaternionMathF::New(0.500000000f, 0.500000000f, 0.500000000f, 0.500000000f),
  QuaternionMathF::New(-0.500000000f, -0.500000000f, -0.500000000f, 0.500000000f),
  QuaternionMathF::New(0.500000000f, -0.500000000f, 0.500000000f, 0.500000000f),
  QuaternionMathF::New(-0.500000000f, 0.500000000f, -0.500000000f, 0.500000000f),
  QuaternionMathF::New(-0.500000000f, 0.500000000f, 0.500000000f, 0.500000000f),
  QuaternionMathF::New(0.500000000f, -0.500000000f, -0.500000000f, 0.500000000f),
  QuaternionMathF::New(-0.500000000f, -0.500000000f, 0.500000000f, 0.500000000f),
  QuaternionMathF::New(0.500000000f, 0.500000000f, -0.500000000f, 0.500000000f)};

static const float CubicLowRodSym[12][3] = {{0.0f, 0.0f,0.0f},
                                         {10000000000.0f, 0.0f, 0.0f},
                                         {0.0f, 10000000000.0f, 0.0f},
                                         {0.0f, 0.0f, 10000000000.0f},
                                         {1.0f, 1.0f, 1.0f},
                                         {-1.0f, -1.0f, -1.0f},
                                         {1.0f, -1.0f, 1.0f},
                                         {-1.0f, 1.0f, -1.0f},
                                         {-1.0f, 1.0f, 1.0f},
                                         {1.0f, -1.0f, -1.0f},
                                         {-1.0f, -1.0f, 1.0f},
                                         {1.0f, 1.0f, -1.0}};

static const float CubicLowSlipDirections[12][3] = {{0.0f,1.0f,-1.0f},
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

static const float CubicLowSlipPlanes[12][3] = {{1.0f,1.0f,1.0f},
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

static const float CubicLowMatSym[12][3][3] =
{{{1.0, 0.0, 0.0},
  {0.0, 1.0, 0.0},
  {0.0, 0.0, 1.0}},

 {{1.0,  0.0,  0.0},
  {0.0, -1.0,  0.0},
  {0.0,  0.0, -1.0}},

 {{-1.0, 0.0,  0.0},
  {0.0, 1.0,  0.0},
  {0.0, 0.0, -1.0}},

 {{-1.0,  0.0, 0.0},
  {0.0, -1.0, 0.0},
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
  {0.0, 1.0, 0.0}}};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CubicLowOps::CubicLowOps()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CubicLowOps::~CubicLowOps()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float CubicLowOps::getMisoQuat(QuatF &q1, QuatF &q2, float &n1, float &n2, float &n3)
{

  int numsym = 12;

  return _calcMisoQuat(CubicLowQuatSym, numsym, q1, q2, n1, n2, n3);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float CubicLowOps::_calcMisoQuat(const QuatF quatsym[24], int numsym,
QuatF &q1, QuatF &q2,
float &n1, float &n2, float &n3)
{
  float wmin = 9999999.0f; //,na,nb,nc;
  float w = 0;
  float n1min = 0.0f;
  float n2min = 0.0f;
  float n3min = 0.0f;
  QuatF qr;
  QuatF qc;
  QuatF q2inv;
  QuaternionMathF::Copy(q2, q2inv);
  QuaternionMathF::Conjugate(q2inv);

  QuaternionMathF::Multiply(q2inv, q1, qr);
  for (int i = 0; i < numsym; i++)
  {
     QuaternionMathF::Multiply(qr, quatsym[i], qc);
        if (qc.w < -1) {
      qc.w = -1;
    }
    else if (qc.w > 1) {
      qc.w = 1;
    }

    OrientationMath::QuattoAxisAngle(qc, w, n1, n2, n3);

    if (w > DREAM3D::Constants::k_Pi) {
      w = DREAM3D::Constants::k_2Pi - w;
    }
    if (w < wmin)
    {
      wmin = w;
      n1min = n1;
      n2min = n2;
      n3min = n3;
    }
  }
  float denom = sqrt((n1min*n1min+n2min*n2min+n3min*n3min));
  n1 = n1min/denom;
  n2 = n2min/denom;
  n3 = n3min/denom;
  if(denom == 0) n1 = 0.0, n2 = 0.0, n3 = 1.0;
  if(wmin == 0) n1 = 0.0, n2 = 0.0, n3 = 1.0;
  return wmin;

}

void CubicLowOps::getODFFZRod(float &r1,float &r2, float &r3)
{
  int numsym = 12;

  _calcRodNearestOrigin(CubicLowRodSym, numsym, r1, r2, r3);
}

void CubicLowOps::getQuatSymOp(int i, QuatF &q)
{
  QuaternionMathF::Copy(CubicLowQuatSym[i], q);
}

void CubicLowOps::getRodSymOp(int i,float *r)
{
  r[0] = CubicLowRodSym[i][0];
  r[1] = CubicLowRodSym[i][1];
  r[2] = CubicLowRodSym[i][2];
}

void CubicLowOps::getMatSymOp(int i,float g[3][3])
{
  g[0][0] = CubicLowMatSym[i][0][0];
  g[0][1] = CubicLowMatSym[i][0][1];
  g[0][2] = CubicLowMatSym[i][0][2];
  g[1][0] = CubicLowMatSym[i][1][0];
  g[1][1] = CubicLowMatSym[i][1][1];
  g[1][2] = CubicLowMatSym[i][1][2];
  g[2][0] = CubicLowMatSym[i][2][0];
  g[2][1] = CubicLowMatSym[i][2][1];
  g[2][2] = CubicLowMatSym[i][2][2];
}

void CubicLowOps::getMDFFZRod(float &r1,float &r2, float &r3)
{
  float w, n1, n2, n3;
  float FZw, FZn1, FZn2, FZn3;

  OrientationOps::_calcRodNearestOrigin(CubicLowRodSym, 24, r1, r2, r3);
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

void CubicLowOps::getNearestQuat(QuatF &q1, QuatF &q2)
{
  int numsym = 12;

  _calcNearestQuat(CubicLowQuatSym, numsym, q1, q2);
}

void CubicLowOps::getFZQuat(QuatF &qr)
{
  int numsym = 12;

  _calcQuatNearestOrigin(CubicLowQuatSym, numsym, qr);
}

int CubicLowOps::getMisoBin(float r1, float r2, float r3)
{
  float dim[3];
  float bins[3];
  float step[3];

  OrientationMath::RodtoHomochoric(r1, r2, r3);

  dim[0] = Detail::CubicLowDim1InitValue;
  dim[1] = Detail::CubicLowDim2InitValue;
  dim[2] = Detail::CubicLowDim3InitValue;
  step[0] = Detail::CubicLowDim1StepValue;
  step[1] = Detail::CubicLowDim2StepValue;
  step[2] = Detail::CubicLowDim3StepValue;
  bins[0] = 36.0f;
  bins[1] = 36.0f;
  bins[2] = 36.0f;

  return _calcMisoBin(dim, bins, step, r1, r2, r3);
}

void CubicLowOps::determineEulerAngles(int choose, float &synea1, float &synea2, float &synea3)
{
  float init[3];
  float step[3];
  float phi[3];
  float r1, r2, r3;

  init[0] = Detail::CubicLowDim1InitValue;
  init[1] = Detail::CubicLowDim2InitValue;
  init[2] = Detail::CubicLowDim3InitValue;
  step[0] = Detail::CubicLowDim1StepValue;
  step[1] = Detail::CubicLowDim2StepValue;
  step[2] = Detail::CubicLowDim3StepValue;
  phi[0] = static_cast<float>(choose % 36);
  phi[1] = static_cast<float>((choose / 36) % 36);
  phi[2] = static_cast<float>(choose / (36 * 36));

  _calcDetermineHomochoricValues(init, step, phi, choose, r1, r2, r3);
  OrientationMath::HomochorictoRod(r1, r2, r3);
  getODFFZRod(r1, r2, r3);
  OrientationMath::RodtoEuler(r1, r2, r3, synea1, synea2, synea3);
}

void CubicLowOps::determineRodriguesVector(int choose, float &r1, float &r2, float &r3)
{
  float init[3];
  float step[3];
  float phi[3];

  init[0] = Detail::CubicLowDim1InitValue;
  init[1] = Detail::CubicLowDim2InitValue;
  init[2] = Detail::CubicLowDim3InitValue;
  step[0] = Detail::CubicLowDim1StepValue;
  step[1] = Detail::CubicLowDim2StepValue;
  step[2] = Detail::CubicLowDim3StepValue;
  phi[0] = static_cast<float>(choose % 36);
  phi[1] = static_cast<float>((choose / 36) % 36);
  phi[2] = static_cast<float>(choose / (36 * 36));

  _calcDetermineHomochoricValues(init, step, phi, choose, r1, r2, r3);
  OrientationMath::HomochorictoRod(r1, r2, r3);
  getMDFFZRod(r1, r2, r3);
}
int CubicLowOps::getOdfBin(float r1, float r2, float r3)
{
  float dim[3];
  float bins[3];
  float step[3];

  OrientationMath::RodtoHomochoric(r1, r2, r3);

  dim[0] = Detail::CubicLowDim1InitValue;
  dim[1] = Detail::CubicLowDim2InitValue;
  dim[2] = Detail::CubicLowDim3InitValue;
  step[0] = Detail::CubicLowDim1StepValue;
  step[1] = Detail::CubicLowDim2StepValue;
  step[2] = Detail::CubicLowDim3StepValue;
  bins[0] = 36.0f;
  bins[1] = 36.0f;
  bins[2] = 36.0f;

  return _calcODFBin(dim, bins, step, r1, r2, r3);
}

void CubicLowOps::getSchmidFactorAndSS(float loadx, float loady, float loadz, float &schmidfactor, int &slipsys)
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

void CubicLowOps::getmPrime(QuatF &q1, QuatF &q2, float LD[3], float &mPrime)
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
    slipDirection[0] = CubicLowSlipDirections[i][0];
    slipDirection[1] = CubicLowSlipDirections[i][1];
    slipDirection[2] = CubicLowSlipDirections[i][2];
    slipPlane[0] = CubicLowSlipPlanes[i][0];
    slipPlane[1] = CubicLowSlipPlanes[i][1];
    slipPlane[2] = CubicLowSlipPlanes[i][2];
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
  slipDirection[0] = CubicLowSlipDirections[ss1][0];
  slipDirection[1] = CubicLowSlipDirections[ss1][1];
  slipDirection[2] = CubicLowSlipDirections[ss1][2];
  slipPlane[0] = CubicLowSlipPlanes[ss1][0];
  slipPlane[1] = CubicLowSlipPlanes[ss1][1];
  slipPlane[2] = CubicLowSlipPlanes[ss1][2];
  MatrixMath::Multiply3x3with3x1(g1,slipDirection,hkl1);
  MatrixMath::Multiply3x3with3x1(g1,slipPlane,uvw1);
  MatrixMath::Normalize3x1(hkl1);
  MatrixMath::Normalize3x1(uvw1);

  maxSchmidFactor = 0;
  for(int j=0;j<12;j++)
  {
    slipDirection[0] = CubicLowSlipDirections[j][0];
    slipDirection[1] = CubicLowSlipDirections[j][1];
    slipDirection[2] = CubicLowSlipDirections[j][2];
    slipPlane[0] = CubicLowSlipPlanes[j][0];
    slipPlane[1] = CubicLowSlipPlanes[j][1];
    slipPlane[2] = CubicLowSlipPlanes[j][2];
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
  slipDirection[0] = CubicLowSlipDirections[ss2][0];
  slipDirection[1] = CubicLowSlipDirections[ss2][1];
  slipDirection[2] = CubicLowSlipDirections[ss2][2];
  slipPlane[0] = CubicLowSlipPlanes[ss2][0];
  slipPlane[1] = CubicLowSlipPlanes[ss2][1];
  slipPlane[2] = CubicLowSlipPlanes[ss2][2];
  MatrixMath::Multiply3x3with3x1(g2,slipDirection,hkl2);
  MatrixMath::Multiply3x3with3x1(g2,slipPlane,uvw2);
  MatrixMath::Normalize3x1(hkl2);
  MatrixMath::Normalize3x1(uvw2);
  planeMisalignment = fabs(MatrixMath::DotProduct(hkl1,hkl2));
  directionMisalignment = fabs(MatrixMath::DotProduct(uvw1,uvw2));
  mPrime = planeMisalignment*directionMisalignment;
}

void CubicLowOps::getF1(QuatF &q1, QuatF &q2, float LD[3], bool maxSF, float &F1)
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
    slipDirection[0] = CubicLowSlipDirections[i][0];
    slipDirection[1] = CubicLowSlipDirections[i][1];
    slipDirection[2] = CubicLowSlipDirections[i][2];
    slipPlane[0] = CubicLowSlipPlanes[i][0];
    slipPlane[1] = CubicLowSlipPlanes[i][1];
    slipPlane[2] = CubicLowSlipPlanes[i][2];
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
        slipDirection[0] = CubicLowSlipDirections[j][0];
        slipDirection[1] = CubicLowSlipDirections[j][1];
        slipDirection[2] = CubicLowSlipDirections[j][2];
        slipPlane[0] = CubicLowSlipPlanes[j][0];
        slipPlane[1] = CubicLowSlipPlanes[j][1];
        slipPlane[2] = CubicLowSlipPlanes[j][2];
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

void CubicLowOps::getF1spt(QuatF &q1, QuatF &q2, float LD[3], bool maxSF, float &F1spt)
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
    slipDirection[0] = CubicLowSlipDirections[i][0];
    slipDirection[1] = CubicLowSlipDirections[i][1];
    slipDirection[2] = CubicLowSlipDirections[i][2];
    slipPlane[0] = CubicLowSlipPlanes[i][0];
    slipPlane[1] = CubicLowSlipPlanes[i][1];
    slipPlane[2] = CubicLowSlipPlanes[i][2];
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
        slipDirection[0] = CubicLowSlipDirections[j][0];
        slipDirection[1] = CubicLowSlipDirections[j][1];
        slipDirection[2] = CubicLowSlipDirections[j][2];
        slipPlane[0] = CubicLowSlipPlanes[j][0];
        slipPlane[1] = CubicLowSlipPlanes[j][1];
        slipPlane[2] = CubicLowSlipPlanes[j][2];
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

void CubicLowOps::getF7(QuatF &q1, QuatF &q2, float LD[3], bool maxSF, float &F7)
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
    slipDirection[0] = CubicLowSlipDirections[i][0];
    slipDirection[1] = CubicLowSlipDirections[i][1];
    slipDirection[2] = CubicLowSlipDirections[i][2];
    slipPlane[0] = CubicLowSlipPlanes[i][0];
    slipPlane[1] = CubicLowSlipPlanes[i][1];
    slipPlane[2] = CubicLowSlipPlanes[i][2];
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
        slipDirection[0] = CubicLowSlipDirections[j][0];
        slipDirection[1] = CubicLowSlipDirections[j][1];
        slipDirection[2] = CubicLowSlipDirections[j][2];
        slipPlane[0] = CubicLowSlipPlanes[j][0];
        slipPlane[1] = CubicLowSlipPlanes[j][1];
        slipPlane[2] = CubicLowSlipPlanes[j][2];
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
void CubicLowOps::generateSphereCoordsFromEulers(FloatArrayType *eulers, FloatArrayType *xyz001, FloatArrayType *xyz011, FloatArrayType *xyz111)
{
  size_t nOrientations = eulers->GetNumberOfTuples();
  QuaternionMath<float>::Quaternion q1;
  CubicLowOps ops;
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
