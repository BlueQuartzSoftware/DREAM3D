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

#include "OrthoRhombicOps.h"
// Include this FIRST because there is a needed define for some compiles
// to expose some of the constants needed below
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/Math/QuaternionMath.hpp"

namespace Detail
{

static const float OrthoDim1InitValue = powf((0.75f*((DREAM3D::Constants::k_Pi/2.0f)-sinf((DREAM3D::Constants::k_Pi/2.0f)))),(1.0f/3.0f));
static const float OrthoDim2InitValue = powf((0.75f*((DREAM3D::Constants::k_Pi/2.0f)-sinf((DREAM3D::Constants::k_Pi/2.0f)))),(1.0f/3.0f));
static const float OrthoDim3InitValue = powf((0.75f*((DREAM3D::Constants::k_Pi/2.0f)-sinf((DREAM3D::Constants::k_Pi/2.0f)))),(1.0f/3.0f));
static const float OrthoDim1StepValue = OrthoDim1InitValue/18.0f;
static const float OrthoDim2StepValue = OrthoDim2InitValue/18.0f;
static const float OrthoDim3StepValue = OrthoDim3InitValue/18.0f;
}

static const QuatF OrthoQuatSym[4] = { QuaternionMathF::New(0.000000000f,0.000000000f,0.000000000f,1.000000000f),
                                       QuaternionMathF::New(1.000000000f,0.000000000f,0.000000000f,0.000000000f),
                                       QuaternionMathF::New(0.000000000f,1.000000000f,0.000000000f,0.000000000f),
                                       QuaternionMathF::New(0.000000000f,0.000000000f,1.000000000f,0.000000000)};

static const float OrthoRodSym[4][3] = {{0.0f,0.0f,0.0f},
                                        {10000000000.0f,0.0f,0.0f},
                                        {0.0f,10000000000.0f,0.0f},
                                        {0.0f,0.0f,10000000000.0}};

static const float OrthoMatSym[4][3][3] =
{{{1.0, 0.0, 0.0},
  {0.0, 1.0, 0.0},
  {0.0, 0.0, 1.0}},

 {{1.0,  0.0,  0.0},
  {0.0, -1.0,  0.0},
  {0.0,  0.0, -1.0}},

 {{-1.0,  0.0,  0.0},
  {0.0, 1.0,  0.0},
  {0.0,  0.0, -1.0}},

 {{-1.0,  0.0,  0.0},
  {0.0, -1.0,  0.0},
  {0.0,  0.0, 1.0}}};

using namespace Detail;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrthoRhombicOps::OrthoRhombicOps()
{
  // TODO Auto-generated constructor stub
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrthoRhombicOps::~OrthoRhombicOps()
{
  // TODO Auto-generated destructor stub
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float OrthoRhombicOps::_calcMisoQuat(const QuatF quatsym[4], int numsym,
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
    //MULT_QUAT(qr, quatsym[i], qc)
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

float OrthoRhombicOps::getMisoQuat(QuatF &q1, QuatF &q2, float &n1, float &n2, float &n3)
{
  int numsym = 4;

  return _calcMisoQuat(OrthoQuatSym, numsym, q1, q2, n1, n2, n3);
}

void OrthoRhombicOps::getQuatSymOp(int i, QuatF &q)
{
  QuaternionMathF::Copy(OrthoQuatSym[i], q);
  //  q.x = OrthoQuatSym[i][0];
  //  q.y = OrthoQuatSym[i][1];
  //  q.z = OrthoQuatSym[i][2];
  //  q.w = OrthoQuatSym[i][3];

}

void OrthoRhombicOps::getRodSymOp(int i,float *r)
{
  r[0] = OrthoRodSym[i][0];
  r[1] = OrthoRodSym[i][1];
  r[2] = OrthoRodSym[i][2];
}

void OrthoRhombicOps::getMatSymOp(int i,float g[3][3])
{
  g[0][0] = OrthoMatSym[i][0][0];
  g[0][1] = OrthoMatSym[i][0][1];
  g[0][2] = OrthoMatSym[i][0][2];
  g[1][0] = OrthoMatSym[i][1][0];
  g[1][1] = OrthoMatSym[i][1][1];
  g[1][2] = OrthoMatSym[i][1][2];
  g[2][0] = OrthoMatSym[i][2][0];
  g[2][1] = OrthoMatSym[i][2][1];
  g[2][2] = OrthoMatSym[i][2][2];
}

void OrthoRhombicOps::getODFFZRod(float &r1,float &r2, float &r3)
{
  int  numsym = 4;

  _calcRodNearestOrigin(OrthoRodSym, numsym, r1, r2, r3);
}

void OrthoRhombicOps::getMDFFZRod(float &r1,float &r2, float &r3)
{
  float w, n1, n2, n3;
  float FZn1, FZn2, FZn3;

  _calcRodNearestOrigin(OrthoRodSym, 4, r1, r2, r3);
  OrientationMath::RodtoAxisAngle(r1, r2, r3, w, n1, n2, n3);

  FZn1 = fabs(n1);
  FZn2 = fabs(n2);
  FZn3 = fabs(n3);

  OrientationMath::AxisAngletoRod(w, FZn1, FZn2, FZn3, r1, r2, r3);
}

void OrthoRhombicOps::getNearestQuat(QuatF &q1, QuatF &q2)
{
  int numsym = 4;

  _calcNearestQuat(OrthoQuatSym, numsym, q1, q2);
}

void OrthoRhombicOps::getFZQuat(QuatF &qr)
{
  int numsym = 4;

  _calcQuatNearestOrigin(OrthoQuatSym, numsym, qr);

}

int OrthoRhombicOps::getMisoBin(float r1, float r2, float r3)
{
  float dim[3];
  float bins[3];
  float step[3];

  OrientationMath::RodtoHomochoric(r1, r2, r3);

  dim[0] = OrthoDim1InitValue;
  dim[1] = OrthoDim2InitValue;
  dim[2] = OrthoDim3InitValue;
  step[0] = OrthoDim1StepValue;
  step[1] = OrthoDim2StepValue;
  step[2] = OrthoDim3StepValue;
  bins[0] = 36.0;
  bins[1] = 36.0;
  bins[2] = 36.0;

  return _calcMisoBin(dim, bins, step, r1, r2, r3);
}

void OrthoRhombicOps::determineEulerAngles(int choose, float &synea1, float &synea2, float &synea3)
{
  float init[3];
  float step[3];
  float phi[3];
  float r1, r2, r3;

  init[0] = OrthoDim1InitValue;
  init[1] = OrthoDim2InitValue;
  init[2] = OrthoDim3InitValue;
  step[0] = OrthoDim1StepValue;
  step[1] = OrthoDim2StepValue;
  step[2] = OrthoDim3StepValue;
  phi[0] = static_cast<float>(choose % 36);
  phi[1] = static_cast<float>((choose / 36) % 36);
  phi[2] = static_cast<float>(choose / (36 * 36));

  _calcDetermineHomochoricValues(init, step, phi, choose, r1, r2, r3);
  OrientationMath::HomochorictoRod(r1, r2, r3);
  getODFFZRod(r1, r2, r3);
  OrientationMath::RodtoEuler(r1, r2, r3, synea1, synea2, synea3);
}


void OrthoRhombicOps::determineRodriguesVector( int choose, float &r1, float &r2, float &r3)
{
  float init[3];
  float step[3];
  float phi[3];

  init[0] = OrthoDim1InitValue;
  init[1] = OrthoDim2InitValue;
  init[2] = OrthoDim3InitValue;
  step[0] = OrthoDim1StepValue;
  step[1] = OrthoDim2StepValue;
  step[2] = OrthoDim3StepValue;
  phi[0] = static_cast<float>(choose % 36);
  phi[1] = static_cast<float>((choose / 36) % 36);
  phi[2] = static_cast<float>(choose / (36 * 36));

  _calcDetermineHomochoricValues(init, step, phi, choose, r1, r2, r3);
  OrientationMath::HomochorictoRod(r1, r2, r3);
  getMDFFZRod(r1, r2, r3);
}

int OrthoRhombicOps::getOdfBin(float r1, float r2, float r3)
{
  float dim[3];
  float bins[3];
  float step[3];

  OrientationMath::RodtoHomochoric(r1, r2, r3);

  dim[0] = OrthoDim1InitValue;
  dim[1] = OrthoDim2InitValue;
  dim[2] = OrthoDim3InitValue;
  step[0] = OrthoDim1StepValue;
  step[1] = OrthoDim2StepValue;
  step[2] = OrthoDim3StepValue;
  bins[0] = 36.0f;
  bins[1] = 36.0f;
  bins[2] = 36.0f;

  return _calcODFBin(dim, bins, step, r1, r2, r3);
}

void OrthoRhombicOps::getSchmidFactorAndSS(float loadx, float loady, float loadz, float &schmidfactor, int &slipsys)
{
  schmidfactor = 0;
  slipsys = 0;
}

void OrthoRhombicOps::getmPrime(QuatF &q1, QuatF &q2, float LD[3], float &mPrime)
{
  mPrime = 0;
}

void OrthoRhombicOps::getF1(QuatF &q1, QuatF &q2, float LD[3], bool maxSF, float &F1)
{
  F1 = 0;
}

void OrthoRhombicOps::getF1spt(QuatF &q1, QuatF &q2, float LD[3], bool maxSF, float &F1spt)
{
  F1spt = 0;
}

void OrthoRhombicOps::getF7(QuatF &q1, QuatF &q2, float LD[3], bool maxSF, float &F7)
{
  F7 = 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrthoRhombicOps::generateSphereCoordsFromEulers(FloatArrayType *eulers, FloatArrayType *xyz100, FloatArrayType *xyz010, FloatArrayType *xyz001)
{
  size_t nOrientations = eulers->GetNumberOfTuples();
  float g[3][3];
  float gTranpose[3][3];
  float* currentEuler = NULL;
  float direction[3] = {0.0, 0.0, 0.0};

  // Sanity Check the size of the arrays
  if (xyz100->GetNumberOfTuples() < nOrientations * 2)
  {
    xyz100->Resize(nOrientations * 2 * 3);
  }
  if (xyz010->GetNumberOfTuples() < nOrientations * 2)
  {
    xyz010->Resize(nOrientations * 2 * 3);
  }
  if (xyz001->GetNumberOfTuples() < nOrientations * 2)
  {
    xyz001->Resize(nOrientations * 2 * 3);
  }

  // Geneate all the Coordinates
  for(size_t i = 0; i < nOrientations; ++i)
  {

    currentEuler = eulers->GetPointer(i * 3);

    OrientationMath::EulertoMat(currentEuler[0], currentEuler[1], currentEuler[2], g);
    MatrixMath::Transpose3x3(g, gTranpose);

    // -----------------------------------------------------------------------------
    // 100 Family
    direction[0] = 1.0; direction[1] = 0.0; direction[2] = 0.0;
    MatrixMath::Multiply3x3with3x1(gTranpose, direction, xyz100->GetPointer(i*6));
    MatrixMath::Copy3x1(xyz100->GetPointer(i*6),xyz100->GetPointer(i*6 + 3));
    MatrixMath::Multiply3x1withConstant(xyz100->GetPointer(i*6 + 3),-1);

    // -----------------------------------------------------------------------------
    // 010 Family
    direction[0] = 0.0; direction[1] = 1.0; direction[2] = 0.0;
    MatrixMath::Multiply3x3with3x1(gTranpose, direction, xyz010->GetPointer(i*6));
    MatrixMath::Copy3x1(xyz010->GetPointer(i*6),xyz010->GetPointer(i*6 + 3));
    MatrixMath::Multiply3x1withConstant(xyz010->GetPointer(i*6 + 3),-1);

    // -----------------------------------------------------------------------------
    // 001 Family
    direction[0] = 0.0; direction[1] = 0.0; direction[2] = 1.0;
    MatrixMath::Multiply3x3with3x1(gTranpose, direction, xyz001->GetPointer(i*6));
    MatrixMath::Copy3x1(xyz001->GetPointer(i*6),xyz001->GetPointer(i*6 + 3));
    MatrixMath::Multiply3x1withConstant(xyz001->GetPointer(i*6 + 3),-1);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrthoRhombicOps::generateIPFColor(double* eulers, double* refDir, uint8_t* rgb, bool convertDegrees)
{
  generateIPFColor(eulers[0], eulers[1], eulers[2], refDir[0], refDir[1], refDir[2], rgb, convertDegrees);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrthoRhombicOps::generateIPFColor(double phi1, double phi, double phi2, double refDir0, double refDir1, double refDir2, uint8_t* rgb, bool degToRad)
{
  if (degToRad == true)
  {
    phi1 = phi1 * DREAM3D::Constants::k_DegToRad;
    phi = phi * DREAM3D::Constants::k_DegToRad;
    phi2 = phi2 * DREAM3D::Constants::k_DegToRad;
  }
  QuatF qc;
  QuatF q2;
  QuatF q1;
  float g[3][3];
  float p[3];
  float refDirection[3];
  float d[3];
  float theta, phi_local;
  float _rgb[3] = { 0.0, 0.0, 0.0 };

  OrientationMath::EulertoQuat(q1, phi1, phi, phi2);
  OrthoRhombicOps ops;
  for (int j = 0; j < 4; j++)
  {
    ops.getQuatSymOp(j, q2);
    QuaternionMathF::Multiply(q1, q2, qc);

    OrientationMath::QuattoMat(qc, g);

    refDirection[0] = refDir0;
    refDirection[1] = refDir1;
    refDirection[2] = refDir2;
    MatrixMath::Multiply3x3with3x1(g, refDirection, p);
    MatrixMath::Normalize3x1(p);


    if (p[2] < 0)
    {
      p[0] = -p[0];
      p[1] = -p[1];
      p[2] = -p[2];
    }
    d[0] = p[0];
    d[1] = p[1];
    d[2] = 0;
    MatrixMath::Normalize3x1(d);
    if (atan2(d[1], d[0]) >= 0 && atan2(d[1], d[0]) < (90.0 * DREAM3D::Constants::k_DegToRad))
    {
      theta = (p[0] * 0) + (p[1] * 0) + (p[2] * 1);
      if (theta > 1) theta = 1;

      if (theta < -1) theta = -1;

      theta = (DREAM3D::Constants::k_RadToDeg) * acos(theta);
      _rgb[0] = (90.0f - theta) / 90.0f;
      phi_local = (d[0] * 1) + (d[1] * 0) + (d[2] * 0);
      if (phi_local > 1) phi_local = 1;

      if (phi_local < -1) phi_local = -1;

      phi_local = (DREAM3D::Constants::k_RadToDeg) * acos(phi_local);
      _rgb[1] = (1 - _rgb[0]) * ((90.0f - phi_local) / 90.0f);
      _rgb[2] = (1 - _rgb[0]) - _rgb[1];
      break;
    }
  }

  float max = _rgb[0];
  if (_rgb[1] > max) max = _rgb[1];
  if (_rgb[2] > max) max = _rgb[2];

  _rgb[0] = _rgb[0] / max;
  _rgb[1] = _rgb[1] / max;
  _rgb[2] = _rgb[2] / max;
//  _rgb[0] = (0.85f * _rgb[0]) + 0.15f;
//  _rgb[1] = (0.85f * _rgb[1]) + 0.15f;
//  _rgb[2] = (0.85f * _rgb[2]) + 0.15f;

  // Multiply by 255 to get an R/G/B value
  _rgb[0] = _rgb[0] * 255.0f;
  _rgb[1] = _rgb[1] * 255.0f;
  _rgb[2] = _rgb[2] * 255.0f;

  rgb[0] = static_cast<unsigned char>(_rgb[0]);
  rgb[1] = static_cast<unsigned char>(_rgb[1]);
  rgb[2] = static_cast<unsigned char>(_rgb[2]);
}

void OrthoRhombicOps::generateRodriguesColor(float r1, float r2, float r3, unsigned char* rgb)
{
  float range1 = 2.0f*OrthoDim1InitValue;
  float range2 = 2.0f*OrthoDim2InitValue;
  float range3 = 2.0f*OrthoDim3InitValue;
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
