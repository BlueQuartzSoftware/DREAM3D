/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include "LaueOps.h"

#include <chrono>
#include <limits>
#include <random>


#include "SIMPLib/Math/SIMPLibRandom.h"
#include "SIMPLib/Utilities/ColorTable.h"



#include "OrientationLib/LaueOps/CubicLowOps.h"
#include "OrientationLib/LaueOps/CubicOps.h"
#include "OrientationLib/LaueOps/HexagonalLowOps.h"
#include "OrientationLib/LaueOps/HexagonalOps.h"
#include "OrientationLib/LaueOps/MonoclinicOps.h"
#include "OrientationLib/LaueOps/OrthoRhombicOps.h"
#include "OrientationLib/LaueOps/TetragonalLowOps.h"
#include "OrientationLib/LaueOps/TetragonalOps.h"
#include "OrientationLib/LaueOps/TriclinicOps.h"
#include "OrientationLib/LaueOps/TrigonalLowOps.h"
#include "OrientationLib/LaueOps/TrigonalOps.h"

namespace Detail
{

// const static double m_OnePointThree = 1.33333333333f;

// const static double sin_wmin_neg_1_over_2 = static_cast<double>(sinf(SIMPLib::Constants::k_ACosNeg1 / 2.0f));
// const static double sin_wmin_pos_1_over_2 = static_cast<double>(sinf(SIMPLib::Constants::k_ACos1 / 2.0f));
// const static double sin_of_acos_neg_1 = sinf(SIMPLib::Constants::k_ACosNeg1);
// const static double sin_of_acos_pos_1 = sinf(SIMPLib::Constants::k_ACos1);

//  const double recip_sin_of_acos_neg_1 = 1.0f / sin_of_acos_neg_1;
//  const double recip_sin_of_acos_pos_1 = 1.0f / sin_of_acos_pos_1;

// const static double SinOfHalf = sinf(0.5f);
// const static double CosOfHalf = cosf(0.5f);
// const static double SinOfZero = sinf(0.0f);
// const static double CosOfZero = cosf(0.0f);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LaueOps::LaueOps() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LaueOps::~LaueOps() = default;

// -----------------------------------------------------------------------------
QuatType LaueOps::getFZQuat(const QuatType& qr) const
{
  Q_ASSERT(false);
  return QuatType();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double LaueOps::_calcMisoQuat(const QuatType quatsym[24], int numsym, const QuatType& q1, const QuatType& q2, double& n1, double& n2, double& n3) const
{
  double wmin = 9999999.0f; //,na,nb,nc;
  double w = 0.0;
  double n1min = 0.0f;
  double n2min = 0.0f;
  double n3min = 0.0f;

  QuatType qc;

  QuatType q2inv = q2.conjugate();
  QuatType qr = q1 * q2inv;
  for(int i = 0; i < numsym; i++)
  {

    qc = quatsym[i] * qr;

    if(qc.w() < -1)
    {
      qc.w() = -1;
    }
    else if(qc.w() > 1)
    {
      qc.w() = 1;
    }

    OrientationTransformation::qu2ax<QuatType, OrientationType>(qc).toAxisAngle(n1, n2, n3, w);

    if(w > SIMPLib::Constants::k_Pi)
    {
      w = SIMPLib::Constants::k_2Pi - w;
    }
    if(w < wmin)
    {
      wmin = w;
      n1min = n1;
      n2min = n2;
      n3min = n3;
    }
  }
  double denom = sqrt((n1min * n1min + n2min * n2min + n3min * n3min));
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
OrientationType LaueOps::_calcRodNearestOrigin(const double rodsym[24][3], int numsym, const OrientationType& inRod) const
{
  double denom = 0.0f, dist = 0.0f;
  double smallestdist = 100000000.0f;
  double rc1 = 0.0f, rc2 = 0.0f, rc3 = 0.0f;
  OrientationType outRod(4, 0.0f);
  // Turn into an actual 3 Comp Rodrigues Vector
  OrientationType rod = inRod;
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
  double mag = sqrt(outRod[0] * outRod[0] + outRod[1] * outRod[1] + outRod[2] * outRod[2]);
  if(mag == 0.0f)
  {
    outRod[3] = std::numeric_limits<double>::infinity();
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
QuatType LaueOps::_calcNearestQuat(const QuatType quatsym[24], int numsym, const QuatType& q1, const QuatType& q2) const
{
  QuatType out;
  double dist = 0.0;
  double smallestdist = 1000000.0f;
  QuatType qmax;

  for(int i = 0; i < numsym; i++)
  {
    QuatType qc = quatsym[i] * q2;
    if(qc.w() < 0)
    {
      qc.negate();
    }
    dist = static_cast<double>(1 - (qc.w() * q1.w() + qc.x() * q1.x() + qc.y() * q1.y() + qc.z() * q1.z()));
    if(dist < smallestdist)
    {
      smallestdist = dist;
      qmax = qc;
    }
  }
  out = qmax;
  if(out.w() < 0)
  {
    out.negate();
  }
  return out;
}

QuatType LaueOps::_calcQuatNearestOrigin(const QuatType quatsym[24], int numsym, const QuatType& qr) const
{
  double dist = 0.0;
  double smallestdist = 1000000.0f;
  QuatType qmax;

  for(int i = 0; i < numsym; i++)
  {
    QuatType qc = quatsym[i] * qr;

    dist = 1 - (qc.w() * qc.w());
    if(dist < smallestdist)
    {
      smallestdist = dist;
      qmax = qc;
    }
  }
  QuatType out = qmax;

  if(out.w() < 0)
  {
    out.negate();
  }
  return out;
}

int LaueOps::_calcMisoBin(double dim[3], double bins[3], double step[3], const OrientationType& ho) const
{
  int miso1bin = int((ho[0] + dim[0]) / step[0]);
  int miso2bin = int((ho[1] + dim[1]) / step[1]);
  int miso3bin = int((ho[2] + dim[2]) / step[2]);
  if(miso1bin >= bins[0])
  {
    miso1bin = static_cast<int>(bins[0] - 1);
  }
  if(miso2bin >= bins[1])
  {
    miso2bin = static_cast<int>(bins[1] - 1);
  }
  if(miso3bin >= bins[2])
  {
    miso3bin = static_cast<int>(bins[2] - 1);
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
  return (static_cast<int>((bins[0] * bins[1] * miso3bin) + (bins[0] * miso2bin) + miso1bin));
}

void LaueOps::_calcDetermineHomochoricValues(uint64_t seed, double init[3], double step[3], int32_t phi[3], int choose, double& r1, double& r2, double& r3) const
{
  double random;

  SIMPL_RANDOMNG_NEW_SEEDED(seed)
  random = static_cast<double>(rg.genrand_res53());
  r1 = (step[0] * phi[0]) + (step[0] * random) - (init[0]);
  random = static_cast<double>(rg.genrand_res53());
  r2 = (step[1] * phi[1]) + (step[1] * random) - (init[1]);
  random = static_cast<double>(rg.genrand_res53());
  r3 = (step[2] * phi[2]) + (step[2] * random) - (init[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LaueOps::_calcODFBin(double dim[3], double bins[3], double step[3], const OrientationType& ho) const
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
    g1euler1bin = static_cast<int>(bins[0] - 1);
  }
  if(g1euler2bin >= bins[1])
  {
    g1euler2bin = static_cast<int>(bins[1] - 1);
  }
  if(g1euler3bin >= bins[2])
  {
    g1euler3bin = static_cast<int>(bins[2] - 1);
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
  g1odfbin = static_cast<int>((g1euler3bin * bins[0] * bins[1]) + (g1euler2bin * bins[0]) + (g1euler1bin));
  return g1odfbin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<LaueOps::Pointer> LaueOps::getOrientationOpsQVector()
{
  QVector<LaueOps::Pointer> m_OrientationOps;
  m_OrientationOps.push_back(HexagonalOps::New());

  m_OrientationOps.push_back(CubicOps::New());

  m_OrientationOps.push_back(HexagonalLowOps::New()); // Hex Low
  m_OrientationOps.push_back(CubicLowOps::New());     // Cubic Low
  m_OrientationOps.push_back(TriclinicOps::New());    // Triclinic
  m_OrientationOps.push_back(MonoclinicOps::New());   // Monoclinic

  m_OrientationOps.push_back(OrthoRhombicOps::New()); // OrthoRhombic

  m_OrientationOps.push_back(TetragonalLowOps::New()); // Tetragonal-low
  m_OrientationOps.push_back(TetragonalOps::New());    // Tetragonal-high

  m_OrientationOps.push_back(TrigonalLowOps::New()); // Trigonal-low
  m_OrientationOps.push_back(TrigonalOps::New());    // Trigonal-High

  m_OrientationOps.push_back(OrthoRhombicOps::New()); // Axis OrthorhombicOps

  return m_OrientationOps;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<LaueOps::Pointer> LaueOps::getOrientationOpsVector()
{
  std::vector<LaueOps::Pointer> m_OrientationOps;
  m_OrientationOps.push_back(HexagonalOps::New());

  m_OrientationOps.push_back(CubicOps::New());

  m_OrientationOps.push_back(HexagonalLowOps::New()); // Hex Low
  m_OrientationOps.push_back(CubicLowOps::New());     // Cubic Low
  m_OrientationOps.push_back(TriclinicOps::New());    // Triclinic
  m_OrientationOps.push_back(MonoclinicOps::New());   // Monoclinic

  m_OrientationOps.push_back(OrthoRhombicOps::New()); // OrthoRhombic

  m_OrientationOps.push_back(TetragonalLowOps::New()); // Tetragonal-low
  m_OrientationOps.push_back(TetragonalOps::New());    // Tetragonal-high

  m_OrientationOps.push_back(TrigonalLowOps::New()); // Trigonal-low
  m_OrientationOps.push_back(TrigonalOps::New());    // Trigonal-High

  m_OrientationOps.push_back(OrthoRhombicOps::New()); // Axis OrthorhombicOps

  return m_OrientationOps;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<QString> LaueOps::GetLaueNames()
{
  std::vector<QString> names;

  std::vector<LaueOps::Pointer> ops = getOrientationOpsVector();
  names.reserve(ops.size());
  for(const auto& op : ops)
  {
    names.push_back(op->getSymmetryName());
  }

  return names;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
size_t LaueOps::getRandomSymmetryOperatorIndex(int numSymOps) const
{

  using SizeTDistributionType = std::uniform_int_distribution<size_t>;

  const SizeTDistributionType::result_type rangeMin = 0;
  const SizeTDistributionType::result_type rangeMax = static_cast<SizeTDistributionType::result_type>(numSymOps - 1);

  std::random_device randomDevice;           // Will be used to obtain a seed for the random number engine
  std::mt19937_64 generator(randomDevice()); // Standard mersenne_twister_engine seeded with rd()
  std::mt19937_64::result_type seed = static_cast<std::mt19937_64::result_type>(std::chrono::steady_clock::now().time_since_epoch().count());
  generator.seed(seed);
  SizeTDistributionType distribution(rangeMin, rangeMax);

  size_t symOp = distribution(generator); // Random remaining position.
  return symOp;
}

// -----------------------------------------------------------------------------
LaueOps::Pointer LaueOps::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
QString LaueOps::getNameOfClass() const
{
  return QString("LaueOps");
}

// -----------------------------------------------------------------------------
QString LaueOps::ClassName()
{
  return QString("LaueOps");
}
