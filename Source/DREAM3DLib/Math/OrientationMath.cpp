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

#include <cmath>

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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::AxisAngletoHomochoric(float w, float n1, float n2, float n3, float &r1, float &r2, float &r3)
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::AxisAngletoQuat(float w, float n1, float n2, float n3, QuatF &q)
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
  q.x = n1 * const2;
  q.y = n2 * const2;
  q.z = n3 * const2;
  q.w = const1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::AxisAngletoMat(float w, float n1, float n2, float n3, float g[3][3])
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::QuattoAxisAngle(QuatF &q, float &w, float &n1, float &n2, float &n3)
{
  w = static_cast<float>( 2.0*acos(q.w) );
  n1 = q.x / sqrt(1-(q.w*q.w));
  n2 = q.y / sqrt(1-(q.w*q.w));
  n3 = q.z / sqrt(1-(q.w*q.w));
  if(q.w == 1.0) n1 = 0.0f, n2 = 0.0f, n3 = 1.0f;
  if(w > m_pi)
  {
    w = (2*m_pi)-w;
    n1 = -n1;
    n2 = -n2;
    n3 = -n3;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::QuattoMat(QuatF &q, float g[3][3])
{
  g[0][0] = (1 - (2 * q.y * q.y) - (2 * q.z * q.z));
  g[0][1] = ((2 * q.x * q.y) + (2 * q.z * q.w));
  g[0][2] = ((2 * q.x * q.z) - (2 * q.y * q.w));
  g[1][0] = ((2 * q.x * q.y) - (2 * q.z * q.w));
  g[1][1] = (1 - (2 * q.x * q.x) - (2 * q.z * q.z));
  g[1][2] = ((2 * q.y * q.z) + (2 * q.x * q.w));
  g[2][0] = ((2 * q.x * q.z) + (2 * q.y * q.w));
  g[2][1] = ((2 * q.y * q.z) - (2 * q.x * q.w));
  g[2][2] = (1 - (2 * q.x * q.x) - (2 * q.y * q.y));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::RodtoQuat(QuatF &q, float r1, float r2, float r3)
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
  q.x = r1 * const1;
  q.y = r2 * const1;
  q.z = r3 * const1;
  q.w = static_cast<float>( cosf(w/2.0f) );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::QuattoRod(QuatF &q, float &r1, float &r2, float &r3)
{
  float qmag, w;
  float n1, n2, n3;

  qmag = (q.x * q.x) + (q.y * q.y) + (q.z * q.z);
  qmag = sqrt(qmag);
  n1 = q.x / qmag;
  n2 = q.y / qmag;
  n3 = q.z / qmag;
  if(qmag == 0.0) n1 = 0.0f, n2 = 0.0f, n3 = 1.0f;
  w = static_cast<float>( 2.0*acos(q.w) );
  float const1 = tanf(w * 0.5f);
  r1 = n1 * const1;
  r2 = n2 * const1;
  r3 = n3 * const1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::QuattoEuler(QuatF &q, float &ea1, float &ea2, float &ea3)
{
  float diff, sum, tmp;
  diff=atan2(-q.y,-q.x);
  sum=atan2(-q.z,-q.w);
  ea1=(diff+sum);
  ea3=(sum-diff);
  tmp=(q.z*q.z)+(q.w*q.w);
  tmp = sqrt(tmp);
  if(tmp > 1.0f) tmp=1.0f;
  ea2=2*acos(tmp);
  ea1=ea1+two_pi;
  ea3=ea3+two_pi;
  ea1 = fmodf(ea1,two_pi);
  ea3 = fmodf(ea3,two_pi);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::EulertoQuat(QuatF &q, float e1, float e2, float e3)
{
  float s, c, s1, c1, s2, c2;
  s = sinf(0.5f * e2);
  c = cosf(0.5f * e2);
  s1 = sinf(0.5f * (e1 - e3));
  c1 = cosf(0.5f * (e1 - e3));
  s2 = sinf(0.5f * (e1 + e3));
  c2 = cosf(0.5f * (e1 + e3));
  q.x = s*c1;
  q.y = s*s1;
  q.z = c*s2;
  q.w = c*c2;
}


// -----------------------------------------------------------------------------
// Based on a ZXZ Set of transforms
// -----------------------------------------------------------------------------
void OrientationMath::EulerToMat(float ea1, float ea2, float ea3, float g[3][3])
{
  // Calcuate all the values once
  float c1 = cosf(ea1);
  float s1 = sinf(ea1);
  float c2 = cosf(ea2);
  float s2 = sinf(ea2);
  float c3 = cosf(ea3);
  float s3 = sinf(ea3);

  // Row 0
  g[0][0] = c1*c3 - c2*s1*s3;
  g[0][1] = c3*s1 + c1 *c2 * s3;
  g[0][2] = s2*s3;
  // Row 1
  g[1][0] = -c1 * s3 - c2 * c3 * s1;
  g[1][1] = c1 * c2 * c3 - s1 * s3;
  g[1][2] = c3 * s2;
  // Row 2
  g[2][0] = s1 * s2;
  g[2][1] = -c1 * s2;
  g[2][2] = c2;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool closeEnough(const float& a, const float& b,
                 const float& epsilon = std::numeric_limits<float>::epsilon())
{
  return (epsilon > std::abs(a - b));
}

void OrientationMath::MatToEuler(float g[3][3], float &phi1, float &Phi, float &phi2)
{
  if(closeEnough(g[2][2], 1.0) || closeEnough(g[2][2], -1.0))
  {
    phi1 = atan(g[0][1]/g[0][0]) / 2.0f;
    Phi = 0.0;
    phi2 = phi1;
  } else {
    Phi = acos(g[2][2]);
    double s = sin(Phi);
    phi1 = atan2(g[2][0] / s, -g[2][1] / s );
    phi2 = atan2(g[0][2] / s, g[1][2] / s );
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::EulertoRod(float &r1, float &r2, float &r3, float phi1, float phi, float phi2)
{
  float sum, diff, csum, cdiff, sdiff, t2;
  sum = (phi1+phi2)/2.0f;
  diff = (phi1-phi2)/2.0f;
  csum = cosf(sum);
  cdiff = cosf(diff);
  sdiff = sinf(diff);
  t2 = tanf(phi/2.0f);
  r1 = t2*cdiff/csum;
  r2 = t2*sdiff/csum;
  r3 = tanf(sum);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::RodtoEuler(float r1, float r2, float r3, float &ea1, float &ea2, float &ea3)
{
  float sum = atan(r3);
  float diff = atan(r2 / r1);
  ea1 = sum + diff;
  ea2 = static_cast<float>( 2. * atan(r1 * cosf(sum) / cosf(diff)) );
  ea3 = sum - diff;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::MultiplyQuaternionVector(QuatF &inQuat, float* inVec, float* outVec)
{
  float g[3][3];
  OrientationMath::QuattoMat(inQuat, g);
  MatrixMath::Multiply3x3with3x1(g, inVec, outVec);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float OrientationMath::MatrixMisorientation(float g1[3][3], float g2[3][3])
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::ChangeAxisReferenceFrame(QuatF &q, float &n1, float &n2, float &n3)
{
  float g[3][3];
  float n[3];
  float nNew[3];

  n[0] = n1;
  n[1] = n2;
  n[2] = n3;

  QuattoMat(q, g);
  MatrixMath::Multiply3x3with3x1(g, n, nNew);
  MatrixMath::Normalize3x1(nNew);
  n1 = nNew[0];
  n2 = nNew[1];
  n3 = nNew[2];
}
