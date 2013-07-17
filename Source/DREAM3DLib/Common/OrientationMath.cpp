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
#include "OrientationMath.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3DLib/OrientationOps/TrigonalOps.h"
#include "DREAM3DLib/OrientationOps/TetragonalOps.h"

#include "MXA/Common/LogTime.h"

  const static float m_pi = static_cast<float>(M_PI);
  const static float two_pi = 2.0f * m_pi;
  const static float recip_pi = 1.0f/m_pi;
  const static float pi_over_180 = m_pi/180.0f;

  const static float m_OnePointThree = 1.33333333333f;

  const float threesixty_over_pi = 360.0f/m_pi;
  const float oneeighty_over_pi = 180.0f/m_pi;
  const float sqrt_two = powf(2.0f, 0.5f);

  const float acos_neg_one = acosf(-1.0f);
  const float acos_pos_one = acosf(1.0f);
  const float sin_wmin_neg_1_over_2 = static_cast<float>( sinf(acos_neg_one/2.0f) );
  const float sin_wmin_pos_1_over_2 = static_cast<float>( sinf(acos_pos_one/2.0f) );
  const float sin_of_acos_neg_1 = sinf(acos_neg_one);
  const float sin_of_acos_pos_1 = sinf(acos_pos_one);

  const float recip_sin_of_acos_neg_1 = 1.0f/sin_of_acos_neg_1;
  const float recip_sin_of_acos_pos_1 = 1.0f/sin_of_acos_pos_1;

  const static float SinOfHalf = sinf(0.5f);
  const static float CosOfHalf = cosf(0.5f);
  const static float SinOfZero = sinf(0.0f);
  const static float CosOfZero = cosf(0.0f);


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationMath::OrientationMath()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationMath::~OrientationMath()
{
}

float OrientationMath::_calcMisoQuat(const float quatsym[24][5], int numsym,
                                      float q1[5], float q2[5],
                                      float &n1, float &n2, float &n3)
{
  float wmin = 9999999.0f; //,na,nb,nc;
  float w = 0;
    float n1min = 0.0f;
    float n2min = 0.0f;
    float n3min = 0.0f;
	float qr[5];
  float qc[5];
   float q2inv[5];

   for(int i=0;i<5;i++)
   {
	   q2inv[i] = q2[i];
   }
   OrientationMath::invertQuaternion(q2inv);
   OrientationMath::multiplyQuaternions(q2inv, q1, qr);
  for (int i = 0; i < numsym; i++)
  {
  //  OrientationMath::multiplyQuaternions(qr, quatsym[i], qc);
    MULT_QUAT(qr, quatsym[i], qc)
    if (qc[4] < -1) {
      qc[4] = -1;
    }
    else if (qc[4] > 1) {
      qc[4] = 1;
    }

  QuattoAxisAngle(qc, w, n1, n2, n3);

  if (w > m_pi) {
      w = two_pi - w;
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


void OrientationMath::_calcRodNearestOrigin(const float rodsym[24][3], int numsym, float &r1,float &r2, float &r3)
{
  float denom, dist;
//  int index;
  float smallestdist = 100000000.0f;
  float rc1 = 0.0f, rc2 = 0.0f, rc3 = 0.0f;
  float r1min = 0.0f, r2min = 0.0f, r3min = 0.0f;
  for(int i=0;i<numsym;i++)
  {
    denom = 1-(r1*rodsym[i][0]+r2*rodsym[i][1]+r3*rodsym[i][2]);
    rc1 = (r1+rodsym[i][0]-(r3*rodsym[i][1]-r2*rodsym[i][2]))/denom;
    rc2 = (r2+rodsym[i][1]-(r1*rodsym[i][2]-r3*rodsym[i][0]))/denom;
    rc3 = (r3+rodsym[i][2]-(r2*rodsym[i][0]-r1*rodsym[i][1]))/denom;
    dist = rc1*rc1+rc2*rc2+rc3*rc3;
    if(dist < smallestdist)
    {
      smallestdist = dist;
      r1min = rc1;
      r2min = rc2;
      r3min = rc3;
    }
  }
  r1 = r1min;
  r2 = r2min;
  r3 = r3min;
}

void OrientationMath::_calcNearestQuat(const float quatsym[24][5], int numsym, float *q1, float *q2)
{
  float dist = 0;
  float smallestdist = 1000000.0f;
  float qc[5];
  float qmax[5];


  for(int i=0;i<5;i++)
  {
    qc[i] = q2[i];
  }
  for(int i=0;i<numsym;i++)
  {
//	OrientationMath::multiplyQuaternions(q2, quatsym[i], qc);
  MULT_QUAT(q2, quatsym[i], qc);
  if(qc[4] < 0)
  {
    qc[1] = -qc[1];
    qc[2] = -qc[2];
    qc[3] = -qc[3];
    qc[4] = -qc[4];
  }
    dist = static_cast<float>(1-(qc[4]*q1[4]+qc[1]*q1[1]+qc[2]*q1[2]+qc[3]*q1[3]));
    if(dist < smallestdist)
    {
      smallestdist = dist;
      qmax[0] = qc[0];
      qmax[1] = qc[1];
      qmax[2] = qc[2];
      qmax[3] = qc[3];
      qmax[4] = qc[4];
    }
  }
  q2[0] = qmax[0];
  q2[1] = qmax[1];
  q2[2] = qmax[2];
  q2[3] = qmax[3];
  q2[4] = qmax[4];
  if(q2[4] < 0)
  {
 //   q2[0] = q2[0];
    q2[1] = -q2[1];
    q2[2] = -q2[2];
    q2[3] = -q2[3];
    q2[4] = -q2[4];
  }
}

void OrientationMath::_calcQuatNearestOrigin(const float quatsym[24][5], int numsym, float *qr)
{
  float dist = 0;
  float smallestdist = 1000000.0f;
  float qc[5];
  float qmax[5];

  for(int i=0;i<5;i++)
  {
    qc[i] = qr[i];
  }
  for(int i=0;i<numsym;i++)
  {
    //OrientationMath::multiplyQuaternions(qr, quatsym[i], qc);
    MULT_QUAT(qr, quatsym[i], qc);
    dist = 1-(qc[4]*qc[4]);
    if(dist < smallestdist)
    {
      smallestdist = dist;
      qmax[0] = qc[0];
      qmax[1] = qc[1];
      qmax[2] = qc[2];
      qmax[3] = qc[3];
      qmax[4] = qc[4];
    }
  }
  qr[0] = qmax[0];
  qr[1] = qmax[1];
  qr[2] = qmax[2];
  qr[3] = qmax[3];
  qr[4] = qmax[4];
  if(qr[4] < 0)
  {
   // qr[0] = qr[0];
    qr[1] = -qr[1];
    qr[2] = -qr[2];
    qr[3] = -qr[3];
    qr[4] = -qr[4];
  }
}


int OrientationMath::_calcMisoBin(float dim[3], float bins[3], float step[3], float r1, float r2, float r3)
{
  int miso1bin = int((r1+dim[0])/step[0]);
  int miso2bin = int((r2+dim[1])/step[1]);
  int miso3bin = int((r3+dim[2])/step[2]);
  if(miso1bin >= bins[0]) miso1bin = static_cast<int>( bins[0]-1 );
  if(miso2bin >= bins[1]) miso2bin = static_cast<int>( bins[1]-1 );
  if(miso3bin >= bins[2]) miso3bin = static_cast<int>( bins[2]-1 );
  if(miso1bin < 0) miso1bin = 0;
  if(miso2bin < 0) miso2bin = 0;
  if(miso3bin < 0) miso3bin = 0;
  return (static_cast<int>( (bins[0]*bins[1]*miso3bin)+(bins[0]*miso2bin)+miso1bin ));
}

void OrientationMath::_calcDetermineHomochoricValues(float init[3], float step[3], float phi[3], int choose, float &r1, float &r2, float &r3)
{
  float random;

  DREAM3D_RANDOMNG_NEW()
  random = static_cast<float>( rg.genrand_res53() );
  r1 = (step[0] * phi[0]) + (step[0] * random) - (init[0]);
  random = static_cast<float>( rg.genrand_res53() );
  r2 = (step[1] * phi[1]) + (step[1] * random) - (init[1]);
  random = static_cast<float>( rg.genrand_res53() );
  r3 = (step[2] * phi[2]) + (step[2] * random) - (init[2]);
}

int OrientationMath::_calcODFBin(float dim[3], float bins[3], float step[3], float r1, float r2, float r3)
{
  int g1euler1bin;
  int g1euler2bin;
  int g1euler3bin;
  int g1odfbin;
  g1euler1bin = int((r1+dim[0])/step[0]);
  g1euler2bin = int((r2+dim[1])/step[1]);
  g1euler3bin = int((r3+dim[2])/step[2]);
  if(g1euler1bin >= bins[0]) g1euler1bin = static_cast<int>( bins[0]-1 );
  if(g1euler2bin >= bins[1]) g1euler2bin = static_cast<int>( bins[1]-1 );
  if(g1euler3bin >= bins[2]) g1euler3bin = static_cast<int>( bins[2]-1 );
  if(g1euler1bin < 0) g1euler1bin = 0;
  if(g1euler2bin < 0) g1euler2bin = 0;
  if(g1euler3bin < 0) g1euler3bin = 0;
  g1odfbin = static_cast<int>( (g1euler3bin*bins[0]*bins[1])+(g1euler2bin*bins[0])+(g1euler1bin) );
  return g1odfbin;
}

std::vector<OrientationMath::Pointer> OrientationMath::getOrientationOpsVector()
{
  std::vector<OrientationMath::Pointer> m_OrientationOps;
  HexagonalOps::Pointer m_HexOps = HexagonalOps::New(); // Hex High
  m_OrientationOps.push_back((m_HexOps));

  CubicOps::Pointer m_CubicOps = CubicOps::New(); // Cubic High
  m_OrientationOps.push_back((m_CubicOps));

  m_OrientationOps.push_back(OrientationMath::NullPointer()); // Hex Low
  m_OrientationOps.push_back(OrientationMath::NullPointer()); // Cubic Low
  m_OrientationOps.push_back(OrientationMath::NullPointer()); // Triclinic
  m_OrientationOps.push_back(OrientationMath::NullPointer()); // Monoclinic

  OrthoRhombicOps::Pointer m_OrthoOps = OrthoRhombicOps::New(); // OrthoRhombic
  m_OrientationOps.push_back((m_OrthoOps));


  m_OrientationOps.push_back(OrientationMath::NullPointer()); // Tetragonal-low
  TetragonalOps::Pointer m_TetraOps = TetragonalOps::New(); // Tetragonal-high
  m_OrientationOps.push_back((m_TetraOps));

  m_OrientationOps.push_back(OrientationMath::NullPointer()); // Trigonal-low
  TrigonalOps::Pointer m_TrigOps = TrigonalOps::New(); // Trigonal-High
  m_OrientationOps.push_back((m_TrigOps));


  OrthoRhombicOps::Pointer m_AxisOrthoOps = OrthoRhombicOps::New();
  m_OrientationOps.push_back((m_AxisOrthoOps));

  return m_OrientationOps;
}

void OrientationMath::axisAngletoHomochoric(float w, float n1, float n2, float n3, float &r1, float &r2, float &r3)
{
  float denom;

  denom = (n1 * n1) + (n2 * n2) + (n3 * n3);
  denom = sqrt(denom);
  n1 = n1 / denom;
  n2 = n2 / denom;
  n3 = n3 / denom;
  if(denom == 0.0) n1 = 0.0, n2 = 0.0, n3 = 1.0;
  r1 = n1 * powf(((0.75f) * (w - sinf(w))), (1.0f / 3.0f));
  r2 = n2 * powf(((0.75f) * (w - sinf(w))), (1.0f / 3.0f));
  r3 = n3 * powf(((0.75f) * (w - sinf(w))), (1.0f / 3.0f));
}

void OrientationMath::AxisAngletoRod(float w, float n1, float n2, float n3, float &r1, float &r2, float &r3)
{
  float denom;

  denom = (n1 * n1) + (n2 * n2) + (n3 * n3);
  denom = sqrt(denom);
  n1 = n1 / denom;
  n2 = n2 / denom;
  n3 = n3 / denom;
  if(denom == 0.0) n1 = 0.0f, n2 = 0.0f, n3 = 1.0f;
  float const1 = tanf(w/2.0f);
  r1 = n1 * const1;
  r2 = n2 * const1;
  r3 = n3 * const1;
}

void OrientationMath::axisAngletoQuat(float w, float n1, float n2, float n3, float *q)
{
  float denom;

  denom = (n1 * n1) + (n2 * n2) + (n3 * n3);
  denom = sqrt(denom);
  n1 = n1 / denom;
  n2 = n2 / denom;
  n3 = n3 / denom;
  if(denom == 0.0) n1 = 0.0f, n2 = 0.0f, n3 = 1.0f;
  float const1 = cosf(w/2.0f);
  float const2 = sinf(w/2.0f);
  q[1] = n1 * const2;
  q[2] = n2 * const2;
  q[3] = n3 * const2;
  q[4] = const1;
}

void OrientationMath::axisAngletoMat(float w, float n1, float n2, float n3, float g[3][3])
{
  float c, s, t, denom;

  c = cosf(w);
  s = sinf(w);
  t = 1-c;

  denom = (n1 * n1) + (n2 * n2) + (n3 * n3);
  denom = sqrt(denom);
  n1 = n1 / denom;
  n2 = n2 / denom;
  n3 = n3 / denom;
  if(denom == 0.0) n1 = 0.0f, n2 = 0.0f, n3 = 1.0f;

  g[0][0] = t*n1*n1+c;
  g[0][1] = t*n1*n2-n3*s;
  g[0][2] = t*n1*n3+n2*s;
  g[1][0] = t*n1*n2+n3*s;
  g[1][1] = t*n2*n2+c;
  g[1][2] = t*n2*n3-n1*s;
  g[2][0] = t*n1*n3-n2*s;
  g[2][1] = t*n2*n3+n1*s;
  g[2][2] = t*n3*n3+c;
}

void OrientationMath::RodtoHomochoric(float &r1, float &r2, float &r3)
{
  float rmag, w;

  rmag = (r1 * r1) + (r2 * r2) + (r3 * r3);
  rmag = sqrt(rmag);
  r1 = r1 / rmag;
  r2 = r2 / rmag;
  r3 = r3 / rmag;
  if(rmag == 0.0) r1 = 0.0f, r2 = 0.0f, r3 = 0.0f;
  w = static_cast<float>( 2.0*atan(rmag) );
  float const1 = powf(((3.0f / 4.0f) * (w - sinf(w))), (1.0f / 3.0f));
  r1 = r1 * const1;
  r2 = r2 * const1;
  r3 = r3 * const1;
}


void OrientationMath::HomochorictoRod(float &r1, float &r2, float &r3)
{
  float hmag, x, w, w_new;

  hmag = (r1 * r1) + (r2 * r2) + (r3 * r3);
  hmag = sqrt(hmag);
  r1 = r1 / hmag;
  r2 = r2 / hmag;
  r3 = r3 / hmag;
  if(hmag == 0.0) r1 = 0.0, r2 = 0.0, r3 = 0.0;
  w = static_cast<float>( powf((8*hmag*hmag*hmag),(1.0f/3.0f)) );
  x = m_OnePointThree*hmag*hmag*hmag;
  for(size_t i = 1; i < 10; i++)
  {
  w_new = w - ((x-w+sin(w))/(-1+cos(w)));
  w = w_new;
  }
  float const1 = tanf(w/2.0f);
  r1 = r1 * const1;
  r2 = r2 * const1;
  r3 = r3 * const1;
}

void OrientationMath::RodtoAxisAngle(float r1, float r2, float r3, float &w, float &n1, float &n2, float &n3)
{
  float rmag;

  rmag = (r1 * r1) + (r2 * r2) + (r3 * r3);
  rmag = sqrt(rmag);
  w = static_cast<float>( 2.0*atan(rmag) );
  n1 = r1 / rmag;
  n2 = r2 / rmag;
  n3 = r3 / rmag;
  if(rmag == 0.0) n1 = 0.0f, n2 = 0.0f, n3 = 1.0f;
  if(w > m_pi)
  {
  w = (2*m_pi)-w;
  n1 = -n1;
  n2 = -n2;
  n3 = -n3;
  }
}

void OrientationMath::QuattoAxisAngle(float *q, float &w, float &n1, float &n2, float &n3)
{
  w = static_cast<float>( 2.0*acos(q[4]) );
  n1 = q[1] / sqrt(1-(q[4]*q[4]));
  n2 = q[2] / sqrt(1-(q[4]*q[4]));
  n3 = q[3] / sqrt(1-(q[4]*q[4]));
  if(q[4] == 1.0) n1 = 0.0f, n2 = 0.0f, n3 = 1.0f;
  if(w > m_pi)
  {
  w = (2*m_pi)-w;
  n1 = -n1;
  n2 = -n2;
  n3 = -n3;
  }
}

void OrientationMath::QuattoMat(float *q, float g[3][3])
{
   g[0][0] = (1 - (2 * q[2] * q[2]) - (2 * q[3] * q[3]));
   g[0][1] = ((2 * q[1] * q[2]) + (2 * q[3] * q[4]));
   g[0][2] = ((2 * q[1] * q[3]) - (2 * q[2] * q[4]));
   g[1][0] = ((2 * q[1] * q[2]) - (2 * q[3] * q[4]));
   g[1][1] = (1 - (2 * q[1] * q[1]) - (2 * q[3] * q[3]));
   g[1][2] = ((2 * q[2] * q[3]) + (2 * q[1] * q[4]));
   g[2][0] = ((2 * q[1] * q[3]) + (2 * q[2] * q[4]));
   g[2][1] = ((2 * q[2] * q[3]) - (2 * q[1] * q[4]));
   g[2][2] = (1 - (2 * q[1] * q[1]) - (2 * q[2] * q[2]));
}

void OrientationMath::RodtoQuat(float *q, float r1, float r2, float r3)
{
  float rmag, w;

  rmag = (r1 * r1) + (r2 * r2) + (r3 * r3);
  rmag = sqrt(rmag);
  r1 = r1 / rmag;
  r2 = r2 / rmag;
  r3 = r3 / rmag;
  if(rmag == 0.0) r1 = 0.0f, r2 = 0.0f, r3 = 0.0f;
  w = 2.0f*atan(rmag);
  float const1 = sinf(w/2.0f);
  q[1] = r1 * const1;
  q[2] = r2 * const1;
  q[3] = r3 * const1;
  q[4] = static_cast<float>( cosf(w/2.0f) );
}

void OrientationMath::QuattoRod(float *q, float &r1, float &r2, float &r3)
{
  float qmag, w;
  float n1, n2, n3;

  qmag = (q[1] * q[1]) + (q[2] * q[2]) + (q[3] * q[3]);
  qmag = sqrt(qmag);
  n1 = q[1] / qmag;
  n2 = q[2] / qmag;
  n3 = q[3] / qmag;
  if(qmag == 0.0) n1 = 0.0f, n2 = 0.0f, n3 = 1.0f;
  w = static_cast<float>( 2.0*acos(q[4]) );
  float const1 = tanf(w * 0.5f);
  r1 = n1 * const1;
  r2 = n2 * const1;
  r3 = n3 * const1;
}

void OrientationMath::QuattoEuler(float *q, float &ea1, float &ea2, float &ea3)
{
  float diff, sum, tmp;
    diff=atan2(-q[2],-q[1]);
    sum=atan2(-q[3],-q[4]);
    ea1=(diff+sum);
    ea3=(sum-diff);
    tmp=(q[3]*q[3])+(q[4]*q[4]);
    tmp = sqrt(tmp);
    if(tmp > 1.0f) tmp=1.0f;
    ea2=2*acos(tmp);
  ea1=ea1+two_pi;
  ea3=ea3+two_pi;
  ea1 = fmodf(ea1,two_pi);
  ea3 = fmodf(ea3,two_pi);
}
void OrientationMath::eulertoQuat(float* q, float e1, float e2, float e3)
{
  float s, c, s1, c1, s2, c2;
  s = sinf(0.5f * e2);
  c = cosf(0.5f * e2);
  s1 = sinf(0.5f * (e1 - e3));
  c1 = cosf(0.5f * (e1 - e3));
  s2 = sinf(0.5f * (e1 + e3));
  c2 = cosf(0.5f * (e1 + e3));
  q[1] = s*c1;
  q[2] = s*s1;
  q[3] = c*s2;
  q[4] = c*c2;
}
void OrientationMath::normalizeQuat(float* qr)
{
  double norm = qr[1]*qr[1]+qr[2]*qr[2]+qr[3]*qr[3]+qr[4]*qr[4];
  norm = sqrt(norm);
  qr[1] = static_cast<float>( qr[1]/norm );
  qr[2] = static_cast<float>( qr[2]/norm );
  qr[3] = static_cast<float>( qr[3]/norm );
  qr[4] = static_cast<float>( qr[4]/norm );
}

void OrientationMath::invertQuaternion(float* q)
{
  q[0] = q[0];
  q[1] = -q[1];
  q[2] = -q[2];
  q[3] = -q[3];
  q[4] = q[4];
}
void OrientationMath::eulertoMat(float ea1, float ea2, float ea3, float g[3][3])
{
      // Calcuate all the values once
      float cos_phi1 = cosf(ea1);
      float sin_phi1 = sinf(ea1);
      float cos_phi = cosf(ea2);
      float sin_phi = sinf(ea2);
      float cos_phi2 = cosf(ea3);
      float sin_phi2 = sinf(ea3);

      // 1) find rotation matrix from Euler angles
      g[0][0] = cos_phi1 * cos_phi2 - sin_phi1 * sin_phi2 * cos_phi;
      g[0][1] = sin_phi1 * cos_phi2 + cos_phi1 * sin_phi2 * cos_phi;
      g[0][2] = sin_phi2 * sin_phi;
      g[1][0] = -cos_phi1 * sin_phi2 - sin_phi1 * cos_phi2 * cos_phi;
      g[1][1] = -sin_phi1 * sin_phi2 + cos_phi1 * cos_phi2 * cos_phi;
      g[1][2] = cos_phi2 * sin_phi;
      g[2][0] = sin_phi1 * sin_phi;
      g[2][1] = -cos_phi1 * sin_phi;
      g[2][2] = cos_phi;
}

void OrientationMath::mattoEuler(float g[3][3], float &ea1, float &ea2, float &ea3)
{
  ea2 = acos(g[2][2]);
  float sin_ea2 = sin(ea2);
  float cosine3 = (g[1][2]/sin_ea2);
  float sine3 = (g[0][2]/sin_ea2);
  float cosine1 = (-g[2][1]/sin_ea2);
  float sine1 = (g[2][0]/sin_ea2);
  ea3 = acos(cosine3);
  ea1 = acos(cosine1);
  if(sine3 < 0) ea3 = (2.0f * m_pi)-ea3;
  if(sine1 < 0) ea1 = (2.0f * m_pi)-ea1;
}

void OrientationMath::eulertoRod(float &r1, float &r2, float &r3, float ea1, float ea2, float ea3)
{
  float sum, diff, csum, cdiff, sdiff, t2;
  sum = (ea1+ea3)/2.0f;
  diff = (ea1-ea3)/2.0f;
  csum = cosf(sum);
  cdiff = cosf(diff);
  sdiff = sinf(diff);
  t2 = tanf(ea2/2.0f);
  r1 = t2*cdiff/csum;
  r2 = t2*sdiff/csum;
  r3 = tanf(sum);
}

void OrientationMath::RodtoEuler(float r1, float r2, float r3, float &ea1, float &ea2, float &ea3)
{
  float sum = atan(r3);
  float diff = atan(r2 / r1);
  ea1 = sum + diff;
  ea2 = static_cast<float>( 2. * atan(r1 * cosf(sum) / cosf(diff)) );
  ea3 = sum - diff;
}

void OrientationMath::multiplyQuaternions(float* inQuat, float* multQuat, float* outQuat)
{
  MULT_QUAT(inQuat, multQuat, outQuat);
}

void OrientationMath::MultiplyQuaternionVector(float* inQuat, float* inVec, float* outVec)
{
  float g[3][3];
  OrientationMath::QuattoMat(inQuat, g);
  MatrixMath::multiply3x3with3x1(g, inVec, outVec);

  //int j;
  //  double vtemp[3], temp[3], r[3], w, temp2[3];
  ///*
  // A function that multiplies a quaterion by a vector (or many quaterions, many vectors)
  // openmp is used to multi-thread the operation
  // */
  //  w    = inQuat[4];
  //r[0] = inQuat[1];
  //r[1] = inQuat[2];
  //r[2] = inQuat[3];

  //for (j=0; j<3; j++)
  //{
  //  vtemp[j] = inVec[j];
  //}

  //MatrixMath::CrossProduct(r, vtemp, temp);

  //for (j=0; j<3; j++)
  //{
  //  temp[j] += w * vtemp[j];
  //}

  //MatrixMath::CrossProduct(r, temp, temp2);

  //for (j=0; j<3; j++)
  //{
  //  outVec[j] = 2.0*temp2[j]+vtemp[j];
  //}
}

float OrientationMath::matrixMisorientation(float g1[3][3], float g2[3][3])
{
  float deltaG[3][3];
  //only need to calculate diagonal terms to allow for trace calculation
  deltaG[0][0] = g1[0][0]*g2[0][0] + g1[1][0]*g2[1][0] + g1[2][0]*g2[2][0];
  deltaG[1][1] = g1[0][1]*g2[0][1] + g1[1][1]*g2[1][1] + g1[2][1]*g2[2][1];
  deltaG[2][2] = g1[0][2]*g2[0][2] + g1[1][2]*g2[1][2] + g1[2][2]*g2[2][2];
  float value = ((deltaG[0][0]+deltaG[1][1]+deltaG[2][2])-1.0)/2.0;
  if(value > 1.0) value = 1.0;
  if(value < -1.0) value = -1.0;
  return acosf(value);
}

void OrientationMath::changeAxisReferenceFrame(float q[5], float &n1, float &n2, float &n3)
{
  float g[3][3];
  float n[3];
  float nNew[3];

  n[0] = n1;
  n[1] = n2;
  n[2] = n3;

  QuattoMat(q, g);
  MatrixMath::multiply3x3with3x1(g, n, nNew);
  MatrixMath::normalize3x1(nNew);
  n1 = nNew[0];
  n2 = nNew[1];
  n3 = nNew[2];
}
