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

#include "SpaceGroupOps.h"

#include <limits>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include <QtCore/QDateTime>

#include "SIMPLib/Utilities/SIMPLibRandom.h"
#include "SIMPLib/Utilities/ColorTable.h"

#include "OrientationLib/OrientationMath/OrientationMath.h"
#include "OrientationLib/SpaceGroupOps/CubicOps.h"
#include "OrientationLib/SpaceGroupOps/CubicLowOps.h"
#include "OrientationLib/SpaceGroupOps/HexagonalOps.h"
#include "OrientationLib/SpaceGroupOps/HexagonalLowOps.h"
#include "OrientationLib/SpaceGroupOps/OrthoRhombicOps.h"
#include "OrientationLib/SpaceGroupOps/TrigonalOps.h"
#include "OrientationLib/SpaceGroupOps/TetragonalOps.h"
#include "OrientationLib/SpaceGroupOps/TrigonalLowOps.h"
#include "OrientationLib/SpaceGroupOps/TetragonalLowOps.h"
#include "OrientationLib/SpaceGroupOps/TriclinicOps.h"
#include "OrientationLib/SpaceGroupOps/MonoclinicOps.h"


namespace Detail
{

//const static float m_OnePointThree = 1.33333333333f;

  const float sin_wmin_neg_1_over_2 = static_cast<float>( sinf(SIMPLib::Constants::k_ACosNeg1 / 2.0f) );
  const float sin_wmin_pos_1_over_2 = static_cast<float>( sinf(SIMPLib::Constants::k_ACos1 / 2.0f) );
  const float sin_of_acos_neg_1 = sinf(SIMPLib::Constants::k_ACosNeg1);
  const float sin_of_acos_pos_1 = sinf(SIMPLib::Constants::k_ACos1);

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
SpaceGroupOps::SpaceGroupOps()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SpaceGroupOps::~SpaceGroupOps()
{
}

void SpaceGroupOps::getFZQuat(QuatF& qr)
{
  BOOST_ASSERT(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float SpaceGroupOps::_calcMisoQuat(const QuatF quatsym[24], int numsym,
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
  QuaternionMathF::Multiply(q1, q2inv, qr);
  for (int i = 0; i < numsym; i++)
  {

    QuaternionMathF::Multiply(quatsym[i], qr, qc);
    if (qc.w < -1)
    {
      qc.w = -1;
    }
    else if (qc.w > 1)
    {
      qc.w = 1;
    }

    FOrientArrayType ax(4, 0.0f);
    FOrientTransformsType::qu2ax(FOrientArrayType(qc.x, qc.y, qc.z, qc.w), ax);
    ax.toAxisAngle(n1, n2, n3, w);

    if (w > SIMPLib::Constants::k_Pi)
    {
      w = SIMPLib::Constants::k_2Pi - w;
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
  if(denom == 0)
  {
    n1 = 0.0, n2 = 0.0, n3 = 1.0;
  }
  if(wmin == 0)
  {
    n1 = 0.0, n2 = 0.0, n3 = 1.0;
  }
  return wmin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FOrientArrayType SpaceGroupOps::_calcRodNearestOrigin(const float rodsym[24][3], int numsym, FOrientArrayType rod)
{
  float denom = 0.0f, dist = 0.0f;
  float smallestdist = 100000000.0f;
  float rc1 = 0.0f, rc2 = 0.0f, rc3 = 0.0f;
  FOrientArrayType outRod(4, 0.0f);
  // Turn into an actual 3 Comp Rodrigues Vector
  rod[0] *= rod[3];
  rod[1] *= rod[3];
  rod[2] *= rod[3];
  for(int i = 0; i < numsym; i++)
  {
    denom = 1 - (rod[0] * rodsym[i][0] + rod[1] * rodsym[i][1] + rod[2] * rodsym[i][2]);
    rc1 = (rod[0] + rodsym[i][0] - (rod[1] * rodsym[i][2] - rod[2] * rodsym[i][1])) / denom;
    rc2 = (rod[1] + rodsym[i][1] - (rod[2] * rodsym[i][0] - rod[0] * rodsym[i][2])) / denom;
    rc3 = (rod[2] + rodsym[i][2] - (rod[0] * rodsym[i][1] - rod[1] * rodsym[i][0])) / denom;
    dist = rc1 * rc1 + rc2 * rc2 + rc3 * rc3;
    if(dist < smallestdist)
    {
      smallestdist = dist;
      outRod[0] = rc1;
      outRod[1] = rc2;
      outRod[2] = rc3;
    }
  }
  float mag = sqrt(outRod[0] * outRod[0] + outRod[1] * outRod[1] + outRod[2] * outRod[2]);
  if(mag == 0.0f)
  {
    outRod[3] = std::numeric_limits<float>::infinity();
  }
  else
  {
    outRod[3] = mag;
    outRod[0] = outRod[0] / outRod[3];
    outRod[1] = outRod[1] / outRod[3];
    outRod[2] = outRod[2] / outRod[3];
  }
  return outRod;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SpaceGroupOps::_calcNearestQuat(const QuatF quatsym[24], int numsym,
                                     QuatF& q1,
                                     QuatF& q2)
{
  float dist = 0;
  float smallestdist = 1000000.0f;
  QuatF qc = QuaternionMath<float>::New();
  QuatF qmax = QuaternionMath<float>::New();

  QuaternionMathF::Copy(q2, qc);

  for(int i = 0; i < numsym; i++)
  {
    QuaternionMathF::Multiply(quatsym[i], q2, qc);
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

void SpaceGroupOps::_calcQuatNearestOrigin(const QuatF quatsym[24], int numsym, QuatF& qr)
{
  float dist = 0;
  float smallestdist = 1000000.0f;
  QuatF qc = QuaternionMath<float>::New();
  QuatF qmax = QuaternionMath<float>::New();

  QuaternionMathF::Copy(qr, qc);

  for(int i = 0; i < numsym; i++)
  {
    QuaternionMathF::Multiply(quatsym[i], qr, qc);
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


int SpaceGroupOps::_calcMisoBin(float dim[3], float bins[3], float step[3], const FOrientArrayType& ho)
{
  int miso1bin = int((ho[0] + dim[0]) / step[0]);
  int miso2bin = int((ho[1] + dim[1]) / step[1]);
  int miso3bin = int((ho[2] + dim[2]) / step[2]);
  if(miso1bin >= bins[0])
  {
    miso1bin = static_cast<int>( bins[0] - 1 );
  }
  if(miso2bin >= bins[1])
  {
    miso2bin = static_cast<int>( bins[1] - 1 );
  }
  if(miso3bin >= bins[2])
  {
    miso3bin = static_cast<int>( bins[2] - 1 );
  }
  if(miso1bin < 0)
  {
    miso1bin = 0;
  }
  if(miso2bin < 0)
  {
    miso2bin = 0;
  }
  if(miso3bin < 0)
  {
    miso3bin = 0;
  }
  return (static_cast<int>( (bins[0] * bins[1] * miso3bin) + (bins[0] * miso2bin) + miso1bin ));
}

void SpaceGroupOps::_calcDetermineHomochoricValues(uint64_t seed, float init[3], float step[3], int32_t phi[3], int choose, float& r1, float& r2, float& r3)
{
  float random;

  SIMPL_RANDOMNG_NEW_SEEDED(seed)
  random = static_cast<float>( rg.genrand_res53() );
  r1 = (step[0] * phi[0]) + (step[0] * random) - (init[0]);
  random = static_cast<float>( rg.genrand_res53() );
  r2 = (step[1] * phi[1]) + (step[1] * random) - (init[1]);
  random = static_cast<float>( rg.genrand_res53() );
  r3 = (step[2] * phi[2]) + (step[2] * random) - (init[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SpaceGroupOps::_calcODFBin(float dim[3], float bins[3], float step[3], FOrientArrayType ho)
{
  int g1euler1bin;
  int g1euler2bin;
  int g1euler3bin;
  int g1odfbin;
  g1euler1bin = int((ho[0] + dim[0]) / step[0]);
  g1euler2bin = int((ho[1] + dim[1]) / step[1]);
  g1euler3bin = int((ho[2] + dim[2]) / step[2]);
  if(g1euler1bin >= bins[0])
  {
    g1euler1bin = static_cast<int>( bins[0] - 1 );
  }
  if(g1euler2bin >= bins[1])
  {
    g1euler2bin = static_cast<int>( bins[1] - 1 );
  }
  if(g1euler3bin >= bins[2])
  {
    g1euler3bin = static_cast<int>( bins[2] - 1 );
  }
  if(g1euler1bin < 0)
  {
    g1euler1bin = 0;
  }
  if(g1euler2bin < 0)
  {
    g1euler2bin = 0;
  }
  if(g1euler3bin < 0)
  {
    g1euler3bin = 0;
  }
  g1odfbin = static_cast<int>( (g1euler3bin * bins[0] * bins[1]) + (g1euler2bin * bins[0]) + (g1euler1bin) );
  return g1odfbin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<SpaceGroupOps::Pointer> SpaceGroupOps::getOrientationOpsQVector()
{
  QVector<SpaceGroupOps::Pointer> m_OrientationOps;
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
std::vector<SpaceGroupOps::Pointer> SpaceGroupOps::getOrientationOpsVector()
{
  std::vector<SpaceGroupOps::Pointer> m_OrientationOps;
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
size_t SpaceGroupOps::getRandomSymmetryOperatorIndex(int numSymOps)
{
  const int rangeMin = 0;
  const int rangeMax = numSymOps-1;
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
