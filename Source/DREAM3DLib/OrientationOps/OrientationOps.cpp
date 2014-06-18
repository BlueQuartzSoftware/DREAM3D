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
#include "OrientationOps.h"


#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include <QtCore/QDateTime>

#include "DREAM3DLib/Utilities/DREAM3DRandom.h"
#include "DREAM3DLib/Math/OrientationMath.h"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/CubicLowOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalLowOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3DLib/OrientationOps/TrigonalOps.h"
#include "DREAM3DLib/OrientationOps/TetragonalOps.h"
#include "DREAM3DLib/OrientationOps/TrigonalLowOps.h"
#include "DREAM3DLib/OrientationOps/TetragonalLowOps.h"
#include "DREAM3DLib/OrientationOps/TriclinicOps.h"
#include "DREAM3DLib/OrientationOps/MonoclinicOps.h"
#include "DREAM3DLib/Utilities/ColorTable.h"

namespace Detail
{

  //const static float m_OnePointThree = 1.33333333333f;

  const float sin_wmin_neg_1_over_2 = static_cast<float>( sinf(DREAM3D::Constants::k_ACosNeg1 / 2.0f) );
  const float sin_wmin_pos_1_over_2 = static_cast<float>( sinf(DREAM3D::Constants::k_ACos1 / 2.0f) );
  const float sin_of_acos_neg_1 = sinf(DREAM3D::Constants::k_ACosNeg1);
  const float sin_of_acos_pos_1 = sinf(DREAM3D::Constants::k_ACos1);

//  const float recip_sin_of_acos_neg_1 = 1.0f / sin_of_acos_neg_1;
//  const float recip_sin_of_acos_pos_1 = 1.0f / sin_of_acos_pos_1;

  const static float SinOfHalf = sinf(0.5f);
  const static float CosOfHalf = cosf(0.5f);
  const static float SinOfZero = sinf(0.0f);
  const static float CosOfZero = cosf(0.0f);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationOps::OrientationOps()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationOps::~OrientationOps()
{
}

void OrientationOps::getFZQuat(QuatF& qr)
{
  BOOST_ASSERT(false);
}

float OrientationOps::_calcMisoQuat(const QuatF quatsym[24], int numsym,
                                    QuatF& q1, QuatF& q2,
                                    float& n1, float& n2, float& n3)
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
    if (qc.w < -1)
    {
      qc.w = -1;
    }
    else if (qc.w > 1)
    {
      qc.w = 1;
    }

    OrientationMath::QuattoAxisAngle(qc, w, n1, n2, n3);

    if (w > DREAM3D::Constants::k_Pi)
    {
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
  float denom = sqrt((n1min * n1min + n2min * n2min + n3min * n3min));
  n1 = n1min / denom;
  n2 = n2min / denom;
  n3 = n3min / denom;
  if(denom == 0) { n1 = 0.0, n2 = 0.0, n3 = 1.0; }
  if(wmin == 0) { n1 = 0.0, n2 = 0.0, n3 = 1.0; }
  return wmin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationOps::_calcRodNearestOrigin(const float rodsym[24][3], int numsym, float& r1, float& r2, float& r3)
{
  float denom, dist;
  //  int index;
  float smallestdist = 100000000.0f;
  float rc1 = 0.0f, rc2 = 0.0f, rc3 = 0.0f;
  float r1min = 0.0f, r2min = 0.0f, r3min = 0.0f;
  for(int i = 0; i < numsym; i++)
  {
    denom = 1 - (r1 * rodsym[i][0] + r2 * rodsym[i][1] + r3 * rodsym[i][2]);
    rc1 = (r1 + rodsym[i][0] - (r3 * rodsym[i][1] - r2 * rodsym[i][2])) / denom;
    rc2 = (r2 + rodsym[i][1] - (r1 * rodsym[i][2] - r3 * rodsym[i][0])) / denom;
    rc3 = (r3 + rodsym[i][2] - (r2 * rodsym[i][0] - r1 * rodsym[i][1])) / denom;
    dist = rc1 * rc1 + rc2 * rc2 + rc3 * rc3;
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationOps::_calcNearestQuat(const QuatF quatsym[24], int numsym,
                                      QuatF& q1,
                                      QuatF& q2)
{
  float dist = 0;
  float smallestdist = 1000000.0f;
  QuatF qc;
  QuatF qmax;

  QuaternionMathF::Copy(q2, qc);

  for(int i = 0; i < numsym; i++)
  {
    QuaternionMathF::Multiply(q2, quatsym[i], qc);
    if(qc.w < 0)
    {
      qc.x = -qc.x;
      qc.y = -qc.y;
      qc.z = -qc.z;
      qc.w = -qc.w;
    }
    dist = static_cast<float>(1 - (qc.w * q1.w + qc.x * q1.x + qc.y * q1.y + qc.z * q1.z));
    if(dist < smallestdist)
    {
      smallestdist = dist;
      QuaternionMathF::Copy(qc, qmax);
    }
  }
  QuaternionMathF::Copy(qmax, q2);
  if(q2.w < 0)
  {
    QuaternionMathF::Negate(q2);
  }
}

void OrientationOps::_calcQuatNearestOrigin(const QuatF quatsym[24], int numsym, QuatF& qr)
{
  float dist = 0;
  float smallestdist = 1000000.0f;
  QuatF qc;
  QuatF qmax;

  QuaternionMathF::Copy(qr, qc);

  for(int i = 0; i < numsym; i++)
  {
    QuaternionMathF::Multiply(qr, quatsym[i], qc);
    dist = 1 - (qc.w * qc.w);
    if(dist < smallestdist)
    {
      smallestdist = dist;
      QuaternionMathF::Copy(qc, qmax);

    }
  }
  QuaternionMathF::Copy(qmax, qr);
  if(qr.w < 0)
  {
    QuaternionMathF::Negate(qr);
  }
}


int OrientationOps::_calcMisoBin(float dim[3], float bins[3], float step[3], float r1, float r2, float r3)
{
  int miso1bin = int((r1 + dim[0]) / step[0]);
  int miso2bin = int((r2 + dim[1]) / step[1]);
  int miso3bin = int((r3 + dim[2]) / step[2]);
  if(miso1bin >= bins[0]) { miso1bin = static_cast<int>( bins[0] - 1 ); }
  if(miso2bin >= bins[1]) { miso2bin = static_cast<int>( bins[1] - 1 ); }
  if(miso3bin >= bins[2]) { miso3bin = static_cast<int>( bins[2] - 1 ); }
  if(miso1bin < 0) { miso1bin = 0; }
  if(miso2bin < 0) { miso2bin = 0; }
  if(miso3bin < 0) { miso3bin = 0; }
  return (static_cast<int>( (bins[0] * bins[1] * miso3bin) + (bins[0] * miso2bin) + miso1bin ));
}

void OrientationOps::_calcDetermineHomochoricValues(float init[3], float step[3], float phi[3], int choose, float& r1, float& r2, float& r3)
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

int OrientationOps::_calcODFBin(float dim[3], float bins[3], float step[3], float r1, float r2, float r3)
{
  int g1euler1bin;
  int g1euler2bin;
  int g1euler3bin;
  int g1odfbin;
  g1euler1bin = int((r1 + dim[0]) / step[0]);
  g1euler2bin = int((r2 + dim[1]) / step[1]);
  g1euler3bin = int((r3 + dim[2]) / step[2]);
  if(g1euler1bin >= bins[0]) { g1euler1bin = static_cast<int>( bins[0] - 1 ); }
  if(g1euler2bin >= bins[1]) { g1euler2bin = static_cast<int>( bins[1] - 1 ); }
  if(g1euler3bin >= bins[2]) { g1euler3bin = static_cast<int>( bins[2] - 1 ); }
  if(g1euler1bin < 0) { g1euler1bin = 0; }
  if(g1euler2bin < 0) { g1euler2bin = 0; }
  if(g1euler3bin < 0) { g1euler3bin = 0; }
  g1odfbin = static_cast<int>( (g1euler3bin * bins[0] * bins[1]) + (g1euler2bin * bins[0]) + (g1euler1bin) );
  return g1odfbin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<OrientationOps::Pointer> OrientationOps::getOrientationOpsVector()
{
  QVector<OrientationOps::Pointer> m_OrientationOps;
  m_OrientationOps.push_back(HexagonalOps::New());

  m_OrientationOps.push_back(CubicOps::New());

  m_OrientationOps.push_back(HexagonalLowOps::New()); // Hex Low
  m_OrientationOps.push_back(CubicLowOps::New()); // Cubic Low
  m_OrientationOps.push_back(TriclinicOps::New()); // Triclinic
  m_OrientationOps.push_back(MonoclinicOps::New()); // Monoclinic

  m_OrientationOps.push_back(OrthoRhombicOps::New());// OrthoRhombic


  m_OrientationOps.push_back(TetragonalLowOps::New()); // Tetragonal-low
  m_OrientationOps.push_back(TetragonalOps::New());// Tetragonal-high

  m_OrientationOps.push_back(TrigonalLowOps::New()); // Trigonal-low
  m_OrientationOps.push_back(TrigonalOps::New());// Trigonal-High

  m_OrientationOps.push_back(OrthoRhombicOps::New()); // Axis OrthorhombicOps

  return m_OrientationOps;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t OrientationOps::getRandomSymmetryOperatorIndex(int numSymOps)
{
  const int rangeMin = 0;
  const int rangeMax = numSymOps;
  typedef boost::uniform_int<int> NumberDistribution;
  typedef boost::mt19937 RandomNumberGenerator;
  typedef boost::variate_generator<RandomNumberGenerator&, NumberDistribution> Generator;

  NumberDistribution distribution(rangeMin, rangeMax);
  RandomNumberGenerator generator;
  Generator numberGenerator(generator, distribution);
  generator.seed(static_cast<boost::uint32_t>( QDateTime::currentMSecsSinceEpoch() )); // seed with the current time
  size_t symOp = numberGenerator(); // Random remaining position.
  return symOp;
}
