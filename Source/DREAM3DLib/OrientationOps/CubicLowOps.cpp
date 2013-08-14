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
#include "DREAM3DLib/Common/ModifiedLambertProjection.h"
#include "DREAM3DLib/Utilities/ImageUtilities.h"


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

using namespace Detail;

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
  schmidfactor = 0;
  slipsys = 0;
}

void CubicLowOps::getmPrime(QuatF &q1, QuatF &q2, float LD[3], float &mPrime)
{
  mPrime = 0;
}

void CubicLowOps::getF1(QuatF &q1, QuatF &q2, float LD[3], bool maxSF, float &F1)
{
  F1 = 0;
}

void CubicLowOps::getF1spt(QuatF &q1, QuatF &q2, float LD[3], bool maxSF, float &F1spt)
{
  F1spt = 0;
}

void CubicLowOps::getF7(QuatF &q1, QuatF &q2, float LD[3], bool maxSF, float &F7)
{
  F7 = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CubicLowOps::generateSphereCoordsFromEulers(FloatArrayType *eulers, FloatArrayType *xyz001, FloatArrayType *xyz011, FloatArrayType *xyz111)
{
  size_t nOrientations = eulers->GetNumberOfTuples();
  CubicLowOps ops;
  float g[3][3];
  float gTranpose[3][3];
  float* currentEuler = NULL;
  float direction[3] = {0.0, 0.0, 0.0};

  // Sanity Check the size of the arrays
  if (xyz001->GetNumberOfTuples() < nOrientations * 3)
  {
    xyz001->Resize(nOrientations * 6 * 3);
  }
  if (xyz011->GetNumberOfTuples() < nOrientations * 6)
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CubicLowOps::generateIPFColor(double* eulers, double* refDir, uint8_t* rgb, bool convertDegrees)
{
  generateIPFColor(eulers[0], eulers[1], eulers[2], refDir[0], refDir[1], refDir[2], rgb, convertDegrees);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CubicLowOps::generateIPFColor(double phi1, double phi, double phi2, double refDir0, double refDir1, double refDir2, uint8_t* rgb, bool degToRad)
{
  if (degToRad == true)
  {
    phi1 = phi1 * DREAM3D::Constants::k_DegToRad;
    phi = phi * DREAM3D::Constants::k_DegToRad;
    phi2 = phi2 * DREAM3D::Constants::k_DegToRad;
  }
  QuatF qc;
  QuatF q1;
  float g[3][3];
  float p[3];
  float refDirection[3];
  float cd[3];
  float d[3];
  //float theta, phi_local;
  float _rgb[3] = { 0.0, 0.0, 0.0 };

  // 1) find rotation matrix from Euler angles
  OrientationMath::EulertoQuat(q1, phi1, phi, phi2);
  QuatF q2;
  CubicLowOps co;
  for (int j = 0; j < 12; j++)
  {
    co.getQuatSymOp(j, q2);
    QuaternionMathF::Multiply(q1,q2, qc);

    OrientationMath::QuattoMat(qc, g);

    refDirection[0] = refDir0;
    refDirection[1] = refDir1;
    refDirection[2] = refDir2;
    MatrixMath::Multiply3x3with3x1(g, refDirection, p);
    MatrixMath::Normalize3x1(p);


    float theta = (cd[0] * 0) + (cd[1] * -DREAM3D::Constants::k_HalfSqrt2) + (cd[2] * DREAM3D::Constants::k_HalfSqrt2);
    theta = (DREAM3D::Constants::k_RadToDeg) * acos(theta);
    _rgb[0] = (90.0f - theta) / 45.0f;
    d[0] = (cd[1] * 1) - (cd[2] * 0);
    d[1] = (cd[2] * 0) - (cd[0] * 1);
    d[2] = (cd[0] * 0) - (cd[1] * 0);
    d[0] = -(d[1] + d[2]) / d[0];
    d[1] = 1;
    d[2] = 1;
    float norm = sqrt(((d[0] * d[0]) + (d[1] * d[1]) + (d[2] * d[2])));
    d[0] = d[0] / norm;
    d[1] = d[1] / norm;
    d[2] = d[2] / norm;
    float phi_local = (d[0] * 0) + (d[1] * DREAM3D::Constants::k_HalfSqrt2) + (d[2] * DREAM3D::Constants::k_HalfSqrt2);
    phi_local = (DREAM3D::Constants::k_RadToDeg) * acos(phi_local);
    _rgb[1] = (1 - _rgb[0]) * ((35.26f - phi_local) / 35.26f);
    _rgb[2] = (1 - _rgb[0]) - _rgb[1];
  }
  float max = _rgb[0];
  if (_rgb[1] > max) max = _rgb[1];
  if (_rgb[2] > max) max = _rgb[2];

  _rgb[0] = _rgb[0] / max;
  _rgb[1] = _rgb[1] / max;
  _rgb[2] = _rgb[2] / max;

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
void CubicLowOps::generateRodriguesColor(float r1, float r2, float r3, unsigned char* rgb)
{
  float range1 = 2.0f*CubicLowDim1InitValue;
  float range2 = 2.0f*CubicLowDim2InitValue;
  float range3 = 2.0f*CubicLowDim3InitValue;
  float max1 = range1/2.0f;
  float max2 = range2/2.0f;
  float max3 = range3/2.0f;
  float red = (r1+max1)/range1;
  float green = (r2+max2)/range2;
  float blue = (r3+max3)/range3;

  // Scale values from 0 to 1.0
  red = red / max1;
  green = green / max1;
  blue = blue / max2;

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
std::vector<UInt8ArrayType::Pointer> CubicLowOps::generatePoleFigure(FloatArrayType* eulers, int poleFigureDimension, int lambertDimension, int numColors)
{

  std::vector<UInt8ArrayType::Pointer> poleFigures;

  int numOrientations = eulers->GetNumberOfTuples();

  // Create an Array to hold the XYZ Coordinates which are the coords on the sphere.
  // this is size for CUBIC ONLY, <001> Family
  FloatArrayType::Pointer xyz001 = FloatArrayType::CreateArray(numOrientations * 6, 3, "Cubic_<001>_xyzCoords");
  // this is size for CUBIC ONLY, <011> Family
  FloatArrayType::Pointer xyz011 = FloatArrayType::CreateArray(numOrientations * 6, 3, "Cubic_<011>_xyzCoords");
  // this is size for CUBIC ONLY, <111> Family
  FloatArrayType::Pointer xyz111 = FloatArrayType::CreateArray(numOrientations * 8, 3, "Cubic_<111>_xyzCoords");


  float sphereRadius = 1.0f;

  // Generate the coords on the sphere
  generateSphereCoordsFromEulers(eulers, xyz001.get(), xyz011.get(), xyz111.get());

  // These arrays hold the "intensity" images which eventually get converted to an actual Color RGB image
  // Generate the modified Lambert projection images (Squares, 2 of them, 1 for northern hemisphere, 1 for southern hemisphere
  ModifiedLambertProjection::Pointer lambert = ModifiedLambertProjection::CreateProjectionFromXYZCoords(xyz001.get(), lambertDimension, sphereRadius);

  // Now create the intensity image that will become the actual Pole figure image
  {
    DoubleArrayType::Pointer intensity001 = lambert->createStereographicProjection(poleFigureDimension);
    intensity001->SetName("PoleFigure_<001>");
    UInt8ArrayType::Pointer image = ImageUtilities::CreateColorImage(intensity001.get(), poleFigureDimension, poleFigureDimension, numColors, "Cubic Low PoleFigure <001>");
    poleFigures.push_back(image);
  }

  // Generate the <011> pole figure which will generate a new set of Lambert Squares
  {
    lambert = ModifiedLambertProjection::CreateProjectionFromXYZCoords(xyz011.get(), lambertDimension, sphereRadius);
    DoubleArrayType::Pointer intensity011 = lambert->createStereographicProjection(poleFigureDimension);
    intensity011->SetName("PoleFigure_<011>");
    UInt8ArrayType::Pointer image = ImageUtilities::CreateColorImage(intensity011.get(), poleFigureDimension, poleFigureDimension, numColors, "Cubic Low PoleFigure <011>");
    poleFigures.push_back(image);
  }
  // Generate the <111> pole figure which will generate a new set of Lambert Squares
  {
    lambert = ModifiedLambertProjection::CreateProjectionFromXYZCoords(xyz111.get(), lambertDimension, sphereRadius);
    DoubleArrayType::Pointer intensity111 = lambert->createStereographicProjection(poleFigureDimension);
    intensity111->SetName("PoleFigure_<111>");
    UInt8ArrayType::Pointer image = ImageUtilities::CreateColorImage(intensity111.get(), poleFigureDimension, poleFigureDimension, numColors, "Cubic Low PoleFigure <111>");
    poleFigures.push_back(image);
  }

  return poleFigures;

}
