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

#include "TetragonalOps.h"
// Include this FIRST because there is a needed define for some compiles
// to expose some of the constants needed below
#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Math/OrientationMath.hpp"

namespace TetragonalMath {
  namespace Detail {

    static const float TetraDim1InitValue = powf((0.75f*((M_PI/4.0f)-sinf((M_PI/4.0f)))),(1.0f/3.0f));
    static const float TetraDim2InitValue = powf((0.75f*((M_PI/4.0f)-sinf((M_PI/4.0f)))),(1.0f/3.0f));
    static const float TetraDim3InitValue = powf((0.75f*((M_PI/2.0f)-sinf((M_PI/2.0f)))),(1.0f/3.0f));
    static const float TetraDim1StepValue = TetraDim1InitValue/18.0f;
    static const float TetraDim2StepValue = TetraDim2InitValue/18.0f;
    static const float TetraDim3StepValue = TetraDim3InitValue/9.0f;

    static const float TetraRodSym[8][3] = {{0.0f,0.0f,0.0f},
                                            {10000000000.0f,0.0f,0.0f},
                                            {0.0f,10000000000.0f,0.0f},
                                            {0.0f,0.0f,10000000000.0f},
                                            {0.0f,0.0f,-1.0f},
                                            {0.0f,0.0f,1.0f},
                                            {10000000000.0f,10000000000.0f,0.0},
                                            {-10000000000.0f,10000000000.0f,0.0}};

    static const float TetraMatSym[8][3][3] =
    {{{1.0, 0.0, 0.0},
      {0.0, 1.0, 0.0},
      {0.0, 0.0, 1.0}},

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

     {{0.0, -1.0,  0.0},
      {-1.0,  0.0,  0.0},
      {0.0,  0.0, -1.0}}};
  }
}


const static float m_OnePointThree = 1.33333333333f;


using namespace TetragonalMath::Detail;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TetragonalOps::TetragonalOps()
{
  // TODO Auto-generated constructor stub

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TetragonalOps::~TetragonalOps()
{
  // TODO Auto-generated destructor stub
}

float TetragonalOps::_calcMisoQuat(const QuatF quatsym[8], int numsym,
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

    OrientationMath<float>::QuattoAxisAngle(qc, w, n1, n2, n3);

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

float TetragonalOps::getMisoQuat(QuatF &q1, QuatF &q2, float &n1, float &n2, float &n3)
{
  int numsym = 8;

  return _calcMisoQuat(TetraQuatSym, numsym, q1, q2, n1, n2, n3);
}

void TetragonalOps::getQuatSymOp(int i, QuatF &q)
{
  QuaternionMathF::Copy(TetraQuatSym[i], q);
  //  q.x = TetraQuatSym[i][0];
  //  q.y = TetraQuatSym[i][1];
  //  q.z = TetraQuatSym[i][2];
  //  q.w = TetraQuatSym[i][3];
}

void TetragonalOps::getRodSymOp(int i,float *r)
{
  r[0] = TetraRodSym[i][0];
  r[1] = TetraRodSym[i][1];
  r[2] = TetraRodSym[i][2];
}

void TetragonalOps::getMatSymOp(int i,float g[3][3])
{
  g[0][0] = TetraMatSym[i][0][0];
  g[0][1] = TetraMatSym[i][0][1];
  g[0][2] = TetraMatSym[i][0][2];
  g[1][0] = TetraMatSym[i][1][0];
  g[1][1] = TetraMatSym[i][1][1];
  g[1][2] = TetraMatSym[i][1][2];
  g[2][0] = TetraMatSym[i][2][0];
  g[2][1] = TetraMatSym[i][2][1];
  g[2][2] = TetraMatSym[i][2][2];
}

void TetragonalOps::getODFFZRod(float &r1,float &r2, float &r3)
{
  int  numsym = 8;

  _calcRodNearestOrigin(TetraRodSym, numsym, r1, r2, r3);
}

void TetragonalOps::getMDFFZRod(float &r1,float &r2, float &r3)
{
  float w, n1, n2, n3;
  float FZn1, FZn2, FZn3;

  _calcRodNearestOrigin(TetraRodSym, 4, r1, r2, r3);
  OrientationMath<float>::RodtoAxisAngle(r1, r2, r3, w, n1, n2, n3);

  FZn1 = fabs(n1);
  FZn2 = fabs(n2);
  FZn3 = fabs(n3);

  OrientationMath<float>::AxisAngletoRod(w, FZn1, FZn2, FZn3, r1, r2, r3);
}

void TetragonalOps::getNearestQuat(QuatF &q1, QuatF &q2)
{
  int numsym = 8;

  _calcNearestQuat(TetraQuatSym, numsym, q1, q2);
}

void TetragonalOps::getFZQuat(QuatF &qr)
{
  int numsym = 8;

  _calcQuatNearestOrigin(TetraQuatSym, numsym, qr);

}

int TetragonalOps::getMisoBin(float r1, float r2, float r3)
{
  float dim[3];
  float bins[3];
  float step[3];

  OrientationMath<float>::RodtoHomochoric(r1, r2, r3);

  dim[0] = TetraDim1InitValue;
  dim[1] = TetraDim2InitValue;
  dim[2] = TetraDim3InitValue;
  step[0] = TetraDim1StepValue;
  step[1] = TetraDim2StepValue;
  step[2] = TetraDim3StepValue;
  bins[0] = 36.0;
  bins[1] = 36.0;
  bins[2] = 18.0;

  return _calcMisoBin(dim, bins, step, r1, r2, r3);
}

void TetragonalOps::determineEulerAngles(int choose, float &synea1, float &synea2, float &synea3)
{
  float init[3];
  float step[3];
  float phi[3];
  float r1, r2, r3;

  init[0] = TetraDim1InitValue;
  init[1] = TetraDim2InitValue;
  init[2] = TetraDim3InitValue;
  step[0] = TetraDim1StepValue;
  step[1] = TetraDim2StepValue;
  step[2] = TetraDim3StepValue;
  phi[0] = static_cast<float>(choose % 36);
  phi[1] = static_cast<float>((choose / 36) % 36);
  phi[2] = static_cast<float>(choose / (36 * 36));

  _calcDetermineHomochoricValues(init, step, phi, choose, r1, r2, r3);
  OrientationMath<float>::HomochorictoRod(r1, r2, r3);
  getODFFZRod(r1, r2, r3);
  OrientationMath<float>::RodtoEuler(r1, r2, r3, synea1, synea2, synea3);
}


void TetragonalOps::determineRodriguesVector( int choose, float &r1, float &r2, float &r3)
{
  float init[3];
  float step[3];
  float phi[3];

  init[0] = TetraDim1InitValue;
  init[1] = TetraDim2InitValue;
  init[2] = TetraDim3InitValue;
  step[0] = TetraDim1StepValue;
  step[1] = TetraDim2StepValue;
  step[2] = TetraDim3StepValue;
  phi[0] = static_cast<float>(choose % 36);
  phi[1] = static_cast<float>((choose / 36) % 36);
  phi[2] = static_cast<float>(choose / (36 * 36));

  _calcDetermineHomochoricValues(init, step, phi, choose, r1, r2, r3);
  OrientationMath<float>::HomochorictoRod(r1, r2, r3);
  getMDFFZRod(r1, r2, r3);
}

int TetragonalOps::getOdfBin(float r1, float r2, float r3)
{
  float dim[3];
  float bins[3];
  float step[3];

  OrientationMath<float>::RodtoHomochoric(r1, r2, r3);

  dim[0] = TetraDim1InitValue;
  dim[1] = TetraDim2InitValue;
  dim[2] = TetraDim3InitValue;
  step[0] = TetraDim1StepValue;
  step[1] = TetraDim2StepValue;
  step[2] = TetraDim3StepValue;
  bins[0] = 36.0f;
  bins[1] = 36.0f;
  bins[2] = 18.0f;

  return _calcODFBin(dim, bins, step, r1, r2, r3);
}

void TetragonalOps::getSchmidFactorAndSS(float loadx, float loady, float loadz, float &schmidfactor, int &slipsys)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TetragonalOps::getmPrime(QuatF &q1, QuatF &q2, float LD[3], float &mPrime)
{
  BOOST_ASSERT(false);
#if 0
  float g1[3][3];
  float g2[3][3];
  float h1, k1, l1, u1, v1, w1;
  float h2, k2, l2, u2, v2, w2;
  float denomhkl1, denomhkl2, denomuvw1, denomuvw2;
  float planemisalignment, directionmisalignment;
  QuattoMat(q1, g1);
  QuattoMat(q2, g2);
  // Note the order of multiplication is such that I am actually multiplying by the inverse of g1 and g2
  /*  h1 = CubicSlipSystems[ss1][0]*g1[0][0]+CubicSlipSystems[ss1][1]*g1[1][0]+CubicSlipSystems[ss1][2]*g1[2][0];
  k1 = CubicSlipSystems[ss1][0]*g1[0][1]+CubicSlipSystems[ss1][1]*g1[1][1]+CubicSlipSystems[ss1][2]*g1[2][1];
  l1 = CubicSlipSystems[ss1][0]*g1[0][2]+CubicSlipSystems[ss1][1]*g1[1][2]+CubicSlipSystems[ss1][2]*g1[2][2];
  u1 = CubicSlipSystems[ss1][3]*g1[0][0]+CubicSlipSystems[ss1][4]*g1[1][0]+CubicSlipSystems[ss1][5]*g1[2][0];
  v1 = CubicSlipSystems[ss1][3]*g1[0][1]+CubicSlipSystems[ss1][4]*g1[1][1]+CubicSlipSystems[ss1][5]*g1[2][1];
  w1 = CubicSlipSystems[ss1][3]*g1[0][2]+CubicSlipSystems[ss1][4]*g1[1][2]+CubicSlipSystems[ss1][5]*g1[2][2];
  denomhkl1 = sqrt((h1*h1+k1*k1+l1*l1));
  denomuvw1 = sqrt((u1*u1+v1*v1+w1*w1));
  h2 = CubicSlipSystems[ss2][0]*g2[0][0]+CubicSlipSystems[ss2][1]*g2[1][0]+CubicSlipSystems[ss2][2]*g2[2][0];
  k2 = CubicSlipSystems[ss2][0]*g2[0][1]+CubicSlipSystems[ss2][1]*g2[1][1]+CubicSlipSystems[ss2][2]*g2[2][1];
  l2 = CubicSlipSystems[ss2][0]*g2[0][2]+CubicSlipSystems[ss2][1]*g2[1][2]+CubicSlipSystems[ss2][2]*g2[2][2];
  u2 = CubicSlipSystems[ss2][3]*g2[0][0]+CubicSlipSystems[ss2][4]*g2[1][0]+CubicSlipSystems[ss2][5]*g2[2][0];
  v2 = CubicSlipSystems[ss2][3]*g2[0][1]+CubicSlipSystems[ss2][4]*g2[1][1]+CubicSlipSystems[ss2][5]*g2[2][1];
  w2 = CubicSlipSystems[ss2][3]*g2[0][2]+CubicSlipSystems[ss2][4]*g2[1][2]+CubicSlipSystems[ss2][5]*g2[2][2];
*/
  denomhkl2 = sqrt((h2*h2+k2*k2+l2*l2));
  denomuvw2 = sqrt((u2*u2+v2*v2+w2*w2));
  planemisalignment = fabs((h1*h2+k1*k2+l1*l2)/(denomhkl1*denomhkl2));
  directionmisalignment = fabs((u1*u2+v1*v2+w1*w2)/(denomuvw1*denomuvw2));
  mPrime = planemisalignment*directionmisalignment;
#endif
}


void TetragonalOps::getF1(QuatF &q1, QuatF &q2, float LD[3], bool maxSF, float &F1)
{
  BOOST_ASSERT(false);
#if 0

  float g1[3][3];
  float g2[3][3];
  //  float hkl1[3], uvw1[3];
  //  float hkl2[3], uvw2[3];
  //  float slipDirection[3], slipPlane[3];
  //  float denomhkl1=0, denomhkl2=0, denomuvw1=0, denomuvw2=0;
  //  float directionMisalignment=0, totalDirectionMisalignment=0;
  //  float schmidFactor1=0, schmidFactor2=0;
  float maxSchmidFactor=0;
  //  float directionComponent1=0, planeComponent1=0;
  //  float directionComponent2=0, planeComponent2=0;
  //  float maxF1=0;

  QuattoMat(q1, g1);
  QuattoMat(q2, g2);
  MatrixMath::Normalize3x1(LD);
  // Note the order of multiplication is such that I am actually multiplying by the inverse of g1 and g2
  if(maxSF == true) maxSchmidFactor = 0;
  /*  for(int i=0;i<12;i++)
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
    directionComponent1 = MatrixMath::DotProduct(LD,uvw1);
    planeComponent1 = MatrixMath::DotProduct(LD,hkl1);
    schmidFactor1 = directionComponent1*planeComponent1;
    if(schmidFactor1 > maxSchmidFactor || maxSF == false)
    {
      totalDirectionMisalignment = 0;
      if(maxSF == true) maxSchmidFactor = schmidFactor1;
      for(int j=0;j<12;j++)
      {
        slipDirection[0] = CubicSlipDirections[i][0];
        slipDirection[1] = CubicSlipDirections[i][1];
        slipDirection[2] = CubicSlipDirections[i][2];
        slipPlane[0] = CubicSlipPlanes[i][0];
        slipPlane[1] = CubicSlipPlanes[i][1];
        slipPlane[2] = CubicSlipPlanes[i][2];
        MatrixMath::Multiply3x3with3x1(g2,slipDirection,hkl2);
        MatrixMath::Multiply3x3with3x1(g2,slipPlane,uvw2);
        MatrixMath::Normalize3x1(hkl2);
        MatrixMath::Normalize3x1(uvw2);
        directionComponent2 = MatrixMath::DotProduct(LD,uvw2);
        planeComponent2 = MatrixMath::DotProduct(LD,hkl2);
        schmidFactor2 = directionComponent2*planeComponent2;
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
*/
#endif
}

void TetragonalOps::getF1spt(QuatF &q1, QuatF &q2, float LD[3], bool maxSF, float &F1spt)
{
  BOOST_ASSERT(false);
#if 0
  float g1[3][3];
  float g2[3][3];
  //  float hkl1[3], uvw1[3];
  //  float hkl2[3], uvw2[3];
  //  float slipDirection[3], slipPlane[3];
  //  float directionMisalignment=0, totalDirectionMisalignment=0;
  //  float planeMisalignment=0, totalPlaneMisalignment=0;
  //  float schmidFactor1=0, schmidFactor2=0;
  float maxSchmidFactor=0;
  //  float directionComponent1=0, planeComponent1=0;
  //  float directionComponent2=0, planeComponent2=0;
  //  float maxF1spt=0;

  QuattoMat(q1, g1);
  QuattoMat(q2, g2);
  MatrixMath::Normalize3x1(LD);
  // Note the order of multiplication is such that I am actually multiplying by the inverse of g1 and g2
  if(maxSF == true) maxSchmidFactor = 0;
  /*  for(int i=0;i<12;i++)
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
    directionComponent1 = MatrixMath::DotProduct(LD,uvw1);
    planeComponent1 = MatrixMath::DotProduct(LD,hkl1);
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
        directionComponent2 = MatrixMath::DotProduct(LD,uvw2);
        planeComponent2 = MatrixMath::DotProduct(LD,hkl2);
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
*/
#endif

}

void TetragonalOps::getF7(QuatF &q1, QuatF &q2, float LD[3], bool maxSF, float &F7)
{
  BOOST_ASSERT(false);
#if 0
  float g1[3][3];
  float g2[3][3];
  //  float hkl1[3], uvw1[3];
  //  float hkl2[3], uvw2[3];
  //  float slipDirection[3], slipPlane[3];
  //  float directionMisalignment=0, totalDirectionMisalignment=0;
  //  float schmidFactor1=0, schmidFactor2=0, maxSchmidFactor=0;
  //  float directionComponent1=0, planeComponent1=0;
  //  float directionComponent2=0, planeComponent2=0;
  //  float maxF7=0;

  QuattoMat(q1, g1);
  QuattoMat(q2, g2);
  MatrixMath::Normalize3x1(LD);
  // Note the order of multiplication is such that I am actually multiplying by the inverse of g1 and g2

  /*  for(int i=0;i<12;i++)
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
    directionComponent1 = MatrixMath::DotProduct(LD,uvw1);
    planeComponent1 = MatrixMath::DotProduct(LD,hkl1);
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
        directionComponent2 = MatrixMath::DotProduct(LD,uvw2);
        planeComponent2 = MatrixMath::DotProduct(LD,hkl2);
        schmidFactor2 = directionComponent2*planeComponent2;
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
*/
#endif
}
