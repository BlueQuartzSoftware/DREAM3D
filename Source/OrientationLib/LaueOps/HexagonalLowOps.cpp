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

#include <memory>

#include "HexagonalLowOps.h"

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/task_group.h>
#include <tbb/task.h>
#endif

// Include this FIRST because there is a needed define for some compiles
// to expose some of the constants needed below
#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Utilities/ColorTable.h"

#include "OrientationLib/Core/Orientation.hpp"

#include "OrientationLib/Utilities/ComputeStereographicProjection.h"
#include "OrientationLib/Utilities/PoleFigureUtilities.h"

namespace Detail
{

static const double HexDim1InitValue = std::pow((0.75 * (SIMPLib::Constants::k_Pi - std::sin(SIMPLib::Constants::k_Pi))), (1.0 / 3.0));
static const double HexDim2InitValue = std::pow((0.75 * (SIMPLib::Constants::k_Pi - std::sin(SIMPLib::Constants::k_Pi))), (1.0 / 3.0));
static const double HexDim3InitValue = std::pow((0.75 * ((SIMPLib::Constants::k_Pi / 6.0) - std::sin(SIMPLib::Constants::k_Pi / 6.0))), (1.0 / 3.0));
static const double HexDim1StepValue = HexDim1InitValue / 36.0;
static const double HexDim2StepValue = HexDim2InitValue / 36.0;
static const double HexDim3StepValue = HexDim3InitValue / 6.0;

namespace HexagonalLow
{
static const int symSize0 = 2;
static const int symSize1 = 2;
static const int symSize2 = 2;
} // namespace HexagonalLow
}

const int HexagonalLowOps::k_OdfSize = 62208;
const int HexagonalLowOps::k_MdfSize = 62208;
const int HexagonalLowOps::k_NumSymQuats = 6;

static const QuatType HexQuatSym[HexagonalLowOps::k_NumSymQuats] = {QuatType(0.000000000, 0.000000000, 0.000000000, 1.000000000), QuatType(0.000000000, 0.000000000, 0.500000000, 0.866025400),
                                                                    QuatType(0.000000000, 0.000000000, 0.866025400, 0.500000000), QuatType(0.000000000, 0.000000000, 1.000000000, 0.000000000),
                                                                    QuatType(0.000000000, 0.000000000, 0.866025400, -0.50000000), QuatType(0.000000000, 0.000000000, 0.500000000, -0.86602540)};

static const double HexRodSym[HexagonalLowOps::k_NumSymQuats][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.57735}, {0.0, 0.0, 1.73205}, {0.0, 0.0, 1000000000000.0}, {0.0, 0.0, -1.73205}, {0.0, 0.0, -0.57735}};
static const double HexMatSym[HexagonalLowOps::k_NumSymQuats][3][3] = {
    {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}},

    {{-0.5, static_cast<double>(SIMPLib::Constants::k_Root3Over2), 0.0}, {static_cast<double>(-SIMPLib::Constants::k_Root3Over2), -0.5, 0.0}, {0.0, 0.0, 1.0}},

    {{-0.5, static_cast<double>(-SIMPLib::Constants::k_Root3Over2), 0.0}, {static_cast<double>(SIMPLib::Constants::k_Root3Over2), -0.5, 0.0}, {0.0, 0.0, 1.0}},

    {{0.0, static_cast<double>(SIMPLib::Constants::k_Root3Over2), 0.0}, {static_cast<double>(-SIMPLib::Constants::k_Root3Over2), 0.5, 0.0}, {0.0, 0.0, 1.0}},

    {{-1.0, 0.0, 0.0}, {0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}},

    {{0.5, static_cast<double>(-SIMPLib::Constants::k_Root3Over2), 0.0}, {static_cast<double>(SIMPLib::Constants::k_Root3Over2), 0.5, 0.0}, {0.0, 0.0, 1.0}}};

using namespace Detail;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HexagonalLowOps::HexagonalLowOps()
{
  double junk1 = Detail::HexDim1StepValue * 1.0;
  double junk2 = junk1 / Detail::HexDim2StepValue;
  double junk3 = junk2 / Detail::HexDim3StepValue;
  junk1 = junk3 / junk2;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HexagonalLowOps::~HexagonalLowOps() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool HexagonalLowOps::getHasInversion() const
{
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int HexagonalLowOps::getODFSize() const
{
  return k_OdfSize;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int HexagonalLowOps::getMDFSize() const
{
  return k_MdfSize;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int HexagonalLowOps::getNumSymOps() const
{
  return k_NumSymQuats;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString HexagonalLowOps::getSymmetryName() const
{
  return "Hexagonal 6/m";;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double HexagonalLowOps::_calcMisoQuat(const QuatType quatsym[12], int numsym, QuatType& q1, QuatType& q2, double& n1, double& n2, double& n3) const
{
  double wmin = 9999999.0; //,na,nb,nc;
  double w = 0.0;
  double n1min = 0.0;
  double n2min = 0.0;
  double n3min = 0.0;
  QuatType qc;

  QuatType qr = q1 * (q2.conjugate());

  for (int i = 0; i < numsym; i++)
  {
    qc = quatsym[i] * qr;

    if(qc.w() < -1)
    {
      qc.w() = -1.0;
    }
    else if(qc.w() > 1)
    {
      qc.w() = 1.0;
    }

    OrientationType ax = OrientationTransformation::qu2ax<QuatType, OrientationType>(qc);
    n1 = ax[0];
    n2 = ax[1];
    n3 = ax[2];
    w = ax[3];

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
  double denom = sqrt((n1min * n1min + n2min * n2min + n3min * n3min));
  n1 = n1min / denom;
  n2 = n2min / denom;
  n3 = n3min / denom;
  if(denom == 0)
  {
    n1 = 0.0;
    n2 = 0.0;
    n3 = 1.0;
  }
  if(wmin == 0)
  {
    n1 = 0.0;
    n2 = 0.0;
    n3 = 1.0;
  }
  return wmin;
}

double HexagonalLowOps::getMisoQuat(QuatType& q1, QuatType& q2, double& n1, double& n2, double& n3) const
{
  return _calcMisoQuat(HexQuatSym, k_NumSymQuats, q1, q2, n1, n2, n3);
}

// -----------------------------------------------------------------------------
float HexagonalLowOps::getMisoQuat(QuatF& q1f, QuatF& q2f, float& n1f, float& n2f, float& n3f) const
{
  QuatType q1(q1f[0], q1f[1], q1f[2], q1f[3]);
  QuatType q2(q2f[0], q2f[1], q2f[2], q2f[3]);
  double n1 = n1f;
  double n2 = n2f;
  double n3 = n3f;
  float w = static_cast<float>(_calcMisoQuat(HexQuatSym, k_NumSymQuats, q1, q2, n1, n2, n3));
  n1f = n1;
  n2f = n2;
  n3f = n3;
  return w;
}

QuatType HexagonalLowOps::getQuatSymOp(int32_t i) const
{
  return HexQuatSym[i];
}

void HexagonalLowOps::getRodSymOp(int i, double* r) const
{
  r[0] = HexRodSym[i][0];
  r[1] = HexRodSym[i][1];
  r[2] = HexRodSym[i][2];
}

void HexagonalLowOps::getMatSymOp(int i, double g[3][3]) const
{
  g[0][0] = HexMatSym[i][0][0];
  g[0][1] = HexMatSym[i][0][1];
  g[0][2] = HexMatSym[i][0][2];
  g[1][0] = HexMatSym[i][1][0];
  g[1][1] = HexMatSym[i][1][1];
  g[1][2] = HexMatSym[i][1][2];
  g[2][0] = HexMatSym[i][2][0];
  g[2][1] = HexMatSym[i][2][1];
  g[2][2] = HexMatSym[i][2][2];
}

void HexagonalLowOps::getMatSymOp(int i, float g[3][3]) const
{
  g[0][0] = HexMatSym[i][0][0];
  g[0][1] = HexMatSym[i][0][1];
  g[0][2] = HexMatSym[i][0][2];
  g[1][0] = HexMatSym[i][1][0];
  g[1][1] = HexMatSym[i][1][1];
  g[1][2] = HexMatSym[i][1][2];
  g[2][0] = HexMatSym[i][2][0];
  g[2][1] = HexMatSym[i][2][1];
  g[2][2] = HexMatSym[i][2][2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationType HexagonalLowOps::getODFFZRod(const OrientationType& rod) const
{
  int numsym = 6;
  return _calcRodNearestOrigin(HexRodSym, numsym, rod);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationType HexagonalLowOps::getMDFFZRod(const OrientationType& inRod) const
{
  double w = 0.0, n1 = 0.0, n2 = 0.0, n3 = 0.0;
  double FZn1 = 0.0, FZn2 = 0.0, FZn3 = 0.0, FZw = 0.0;
  double n1n2mag = 0.0;

  OrientationType rod = _calcRodNearestOrigin(HexRodSym, 12, inRod);

  OrientationType ax = OrientationTransformation::ro2ax<OrientationType, OrientationType>(rod);

  n1 = ax[0];
  n2 = ax[1], n3 = ax[2], w = ax[3];

  float denom = sqrt((n1 * n1 + n2 * n2 + n3 * n3));
  n1 = n1 / denom;
  n2 = n2 / denom;
  n3 = n3 / denom;
  if(n3 < 0)
  {
    n1 = -n1, n2 = -n2, n3 = -n3;
  }
  float angle = 180.0 * atan2(n2, n1) * SIMPLib::Constants::k_1OverPi;
  if(angle < 0)
  {
    angle = angle + 360.0;
  }
  FZn1 = n1;
  FZn2 = n2;
  FZn3 = n3;
  if(angle > 30.0)
  {
    n1n2mag = sqrt(n1 * n1 + n2 * n2);
    if(int(angle / 30) % 2 == 0)
    {
      FZw = angle - (30.0 * int(angle / 30.0));
      FZw = FZw * SIMPLib::Constants::k_PiOver180;
      FZn1 = n1n2mag * cosf(FZw);
      FZn2 = n1n2mag * sinf(FZw);
    }
    else
    {
      FZw = angle - (30.0 * int(angle / 30.0));
      FZw = 30.0f - FZw;
      FZw = FZw * SIMPLib::Constants::k_PiOver180;
      FZn1 = n1n2mag * cosf(FZw);
      FZn2 = n1n2mag * sinf(FZw);
    }
  }

  return OrientationTransformation::ax2ro<OrientationType, OrientationType>(OrientationType(FZn1, FZn2, FZn3, w));
}

QuatType HexagonalLowOps::getNearestQuat(const QuatType& q1, const QuatType& q2) const
{
  return _calcNearestQuat(HexQuatSym, k_NumSymQuats, q1, q2);
}

QuatF HexagonalLowOps::getNearestQuat(const QuatF& q1f, const QuatF& q2f) const
{
  QuatType q1(q1f[0], q1f[1], q1f[2], q1f[3]);
  QuatType q2(q2f[0], q2f[1], q2f[2], q2f[3]);
  QuatType temp = _calcNearestQuat(HexQuatSym, k_NumSymQuats, q1, q2);
  QuatF out(temp.x(), temp.y(), temp.z(), temp.w());
  return out;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuatType HexagonalLowOps::getFZQuat(const QuatType& qr) const
{
  return _calcQuatNearestOrigin(HexQuatSym, k_NumSymQuats, qr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int HexagonalLowOps::getMisoBin(const OrientationType& rod) const
{
  double dim[3];
  double bins[3];
  double step[3];

  OrientationType ho = OrientationTransformation::ro2ho<OrientationType, OrientationType>(rod);

  dim[0] = HexDim1InitValue;
  dim[1] = HexDim2InitValue;
  dim[2] = HexDim3InitValue;
  step[0] = HexDim1StepValue;
  step[1] = HexDim2StepValue;
  step[2] = HexDim3StepValue;
  bins[0] = 72.0;
  bins[1] = 72.0;
  bins[2] = 12.0;

  return _calcMisoBin(dim, bins, step, ho);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationType HexagonalLowOps::determineEulerAngles(uint64_t seed, int choose) const
{
  double init[3];
  double step[3];
  int32_t phi[3];
  double h1, h2, h3;

  init[0] = HexDim1InitValue;
  init[1] = HexDim2InitValue;
  init[2] = HexDim3InitValue;
  step[0] = HexDim1StepValue;
  step[1] = HexDim2StepValue;
  step[2] = HexDim3StepValue;
  phi[0] = static_cast<int32_t>(choose % 72);
  phi[1] = static_cast<int32_t>((choose / 72) % 72);
  phi[2] = static_cast<int32_t>(choose / (72 * 72));

  _calcDetermineHomochoricValues(seed, init, step, phi, choose, h1, h2, h3);

  OrientationType ho(h1, h2, h3);
  OrientationType ro = OrientationTransformation::ho2ro<OrientationType, OrientationType>(ho);
  ro = getODFFZRod(ro);
  OrientationType eu = OrientationTransformation::ro2eu<OrientationType, OrientationType>(ro);
  return eu;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationType HexagonalLowOps::randomizeEulerAngles(const OrientationType& synea) const
{
  size_t symOp = getRandomSymmetryOperatorIndex(k_NumSymQuats);
  QuatType quat = OrientationTransformation::eu2qu<OrientationType, QuatType>(synea);
  QuatType qc = HexQuatSym[symOp] * quat;
  return OrientationTransformation::qu2eu<QuatType, OrientationType>(qc);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationType HexagonalLowOps::determineRodriguesVector(uint64_t seed, int choose) const
{
  double init[3];
  double step[3];
  int32_t phi[3];
  double h1, h2, h3;

  init[0] = HexDim1InitValue;
  init[1] = HexDim2InitValue;
  init[2] = HexDim3InitValue;
  step[0] = HexDim1StepValue;
  step[1] = HexDim2StepValue;
  step[2] = HexDim3StepValue;
  phi[0] = static_cast<int32_t>(choose % 72);
  phi[1] = static_cast<int32_t>((choose / 72) % 72);
  phi[2] = static_cast<int32_t>(choose / (72 * 72));

  _calcDetermineHomochoricValues(seed, init, step, phi, choose, h1, h2, h3);
  OrientationType ho(h1, h2, h3);
  OrientationType ro = OrientationTransformation::ho2ro<OrientationType, OrientationType>(ho);
  ro = getMDFFZRod(ro);
  return ro;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int HexagonalLowOps::getOdfBin(const OrientationType& rod) const
{
  double dim[3];
  double bins[3];
  double step[3];

  OrientationType ho = OrientationTransformation::ro2ho<OrientationType, OrientationType>(rod);

  dim[0] = HexDim1InitValue;
  dim[1] = HexDim2InitValue;
  dim[2] = HexDim3InitValue;
  step[0] = HexDim1StepValue;
  step[1] = HexDim2StepValue;
  step[2] = HexDim3StepValue;
  bins[0] = 72.0;
  bins[1] = 72.0;
  bins[2] = 12.0;

  return _calcODFBin(dim, bins, step, ho);
}

void HexagonalLowOps::getSchmidFactorAndSS(double load[3], double& schmidfactor, double angleComps[2], int& slipsys) const
{
  double theta1, theta2, theta3, theta4, theta5, theta6, theta7, theta8, theta9;
  double lambda1, lambda2, lambda3, lambda4, lambda5, lambda6, lambda7, lambda8, lambda9, lambda10;
  double schmid1, schmid2, schmid3, schmid4, schmid5, schmid6;
  double caratio = 1.633;
  double ph1sdx1 = 1.0;
  double ph1sdy1 = 0.0;
  double ph1sdz1 = 0.0;
  double ph1sdx2 = 0.0;
  double ph1sdy2 = 1.0;
  double ph1sdz2 = 0.0;
  double ph1sdx3 = -0.707;
  double ph1sdy3 = -0.707;
  double ph1sdz3 = 0.0;
  double ph1sdx4 = 0.0;
  double ph1sdy4 = -0.707;
  double ph1sdz4 = 0.707;
  double ph1sdx5 = -0.57735;
  double ph1sdy5 = -0.57735;
  double ph1sdz5 = 0.57735;
  double ph1sdx6 = 0.707;
  double ph1sdy6 = 0.0;
  double ph1sdz6 = 0.707;
  double ph1sdx7 = 0.57735;
  double ph1sdy7 = 0.57735;
  double ph1sdz7 = 0.57735;
  double ph1sdx8 = 0.0;
  double ph1sdy8 = 0.707;
  double ph1sdz8 = 0.707;
  double ph1sdx9 = -0.707;
  double ph1sdy9 = 0.0;
  double ph1sdz9 = 0.707;
  double ph1spnx1 = 0.0;
  double ph1spny1 = 0.0;
  double ph1spnz1 = 1.0;
  double ph1spnx2 = 0.4472;
  double ph1spny2 = 0.8944;
  double ph1spnz2 = 0.0;
  double ph1spnx3 = 0.8944;
  double ph1spny3 = 0.4472;
  double ph1spnz3 = 0.0;
  double ph1spnx4 = -0.707;
  double ph1spny4 = 0.707;
  double ph1spnz4 = 0.0;
  double ph1spnx5 = 0.4082;
  double ph1spny5 = 0.8164;
  //  double ph1spnz5 = -0.4082;
  double ph1spnx6 = 0.4082;
  double ph1spny6 = 0.8164;
  //  double ph1spnz6 = 0.4082;
  double ph1spnx7 = 0.8164;
  double ph1spny7 = 0.4082;
  //  double ph1spnz7 = -0.4082;
  double ph1spnx8 = 0.8164;
  double ph1spny8 = 0.4082;
  //  double ph1spnz8 = 0.4082;
  double ph1spnx9 = -0.57735;
  double ph1spny9 = 0.57735;
  //  double ph1spnz9 = -0.57735;
  double ph1spnx10 = -0.57735;
  double ph1spny10 = 0.57735;
  //  double ph1spnz10 = 0.57735;

  double loadx = load[0];
  double loady = load[1];
  double loadz = load[2];

  double t1x = (0.866025 * ph1sdx1) + (0.0 * ph1sdy1) + (0.0 * ph1sdz1);
  double t1y = (-0.5 * ph1sdx1) + (1.0 * ph1sdy1) + (0.0 * ph1sdz1);
  double t1z = (0.0 * ph1sdx1) + (0.0 * ph1sdy1) + (caratio * ph1sdz1);
  double denomt1 = std::pow((t1x * t1x + t1y * t1y + t1z * t1z), 0.5);
  t1x = t1x / denomt1;
  t1y = t1y / denomt1;
  t1z = t1z / denomt1;
  theta1 = ((t1x * loadx) + (t1y * loady) + (t1z * loadz));
  theta1 = fabs(theta1);
  double t2x = (0.866025 * ph1sdx2) + (0.0 * ph1sdy2) + (0.0 * ph1sdz2);
  double t2y = (-0.5 * ph1sdx2) + (1.0 * ph1sdy2) + (0.0 * ph1sdz2);
  double t2z = (0.0 * ph1sdx2) + (0.0 * ph1sdy2) + (caratio * ph1sdz2);
  double denomt2 = std::pow((t2x * t2x + t2y * t2y + t2z * t2z), 0.5);
  t2x = t2x / denomt2;
  t2y = t2y / denomt2;
  t2z = t2z / denomt2;
  theta2 = ((t2x * loadx) + (t2y * loady) + (t2z * loadz));
  theta2 = fabs(theta2);
  double t3x = (0.866025 * ph1sdx3) + (0.0 * ph1sdy3) + (0.0 * ph1sdz3);
  double t3y = (-0.5 * ph1sdx3) + (1.0 * ph1sdy3) + (0.0 * ph1sdz3);
  double t3z = (0.0 * ph1sdx3) + (0.0 * ph1sdy3) + (caratio * ph1sdz3);
  double denomt3 = std::pow((t3x * t3x + t3y * t3y + t3z * t3z), 0.5);
  t3x = t3x / denomt3;
  t3y = t3y / denomt3;
  t3z = t3z / denomt3;
  theta3 = ((t3x * loadx) + (t3y * loady) + (t3z * loadz));
  theta3 = fabs(theta3);
  double l1nx = (0.866025 * ph1spnx1) + (0.0 * ph1spny1);
  double l1ny = (-0.5 * ph1spnx1) + (1.0 * ph1spny1);
  double l1nz = -caratio * ph1spnz1;
  double denoml1 = std::pow((l1nx * l1nx + l1ny * l1ny + l1nz * l1nz), 0.5);
  l1nx = l1nx / denoml1;
  l1ny = l1ny / denoml1;
  l1nz = l1nz / denoml1;
  lambda1 = ((l1nx * loadx) + (l1ny * loady) + (l1nz * loadz));
  lambda1 = fabs(lambda1);
  schmid1 = theta1 * lambda1;
  schmid2 = theta2 * lambda1;
  schmid3 = theta3 * lambda1;
  double l2nx = (0.866025 * ph1spnx2) + (0.0 * ph1spny2);
  double l2ny = (-0.5 * ph1spnx2) + (1 * ph1spny2);
  double l2nz = -caratio * ph1spnz2;
  double denoml2 = std::pow((l2nx * l2nx + l2ny * l2ny + l2nz * l2nz), 0.5);
  l2nx = l2nx / denoml2;
  l2ny = l2ny / denoml2;
  l2nz = l2nz / denoml2;
  lambda2 = ((l2nx * loadx) + (l2ny * loady) + (l2nz * loadz));
  lambda2 = fabs(lambda2);
  double l3nx = (0.866025 * ph1spnx3) + (0.0 * ph1spny3);
  double l3ny = (-0.5 * ph1spnx3) + (1.0 * ph1spny3);
  double l3nz = -caratio * ph1spnz3;
  double denoml3 = std::pow((l3nx * l3nx + l3ny * l3ny + l3nz * l3nz), 0.5);
  l3nx = l3nx / denoml3;
  l3ny = l3ny / denoml3;
  l3nz = l3nz / denoml3;
  lambda3 = ((l3nx * loadx) + (l3ny * loady) + (l3nz * loadz));
  lambda3 = fabs(lambda3);
  double l4nx = (0.866025 * ph1spnx4) + (0.0 * ph1spny4);
  double l4ny = (-0.5 * ph1spnx4) + (1 * ph1spny4);
  double l4nz = -caratio * ph1spnz4;
  double denoml4 = std::pow((l4nx * l4nx + l4ny * l4ny + l4nz * l4nz), 0.5);
  l4nx = l4nx / denoml4;
  l4ny = l4ny / denoml4;
  l4nz = l4nz / denoml4;
  lambda4 = ((l4nx * loadx) + (l4ny * loady) + (l4nz * loadz));
  lambda4 = fabs(lambda4);
  schmid4 = theta1 * lambda2;
  schmid5 = theta2 * lambda3;
  schmid6 = theta3 * lambda4;
  double l5nx = (0.866025 * ph1spnx5) + (0.0 * ph1spny5);
  double l5ny = (-0.5 * ph1spnx5) + (1 * ph1spny5);
  double l5nz = double((l5nx * -l5nx + l5ny * -l5ny)) / (caratio * 0.8164);
  double denoml5 = std::pow((l5nx * l5nx + l5ny * l5ny + l5nz * l5nz), 0.5);
  l5nx = l5nx / denoml5;
  l5ny = l5ny / denoml5;
  l5nz = l5nz / denoml5;
  double l6nx = (0.866025 * ph1spnx6) + (0.0 * ph1spny6);
  double l6ny = (-0.5 * ph1spnx6) + (1.0 * ph1spny6);
  double l6nz = double(-(l6nx * -l6nx + l6ny * -l6ny)) / (caratio * 0.8164);
  double denoml6 = std::pow((l6nx * l6nx + l6ny * l6ny + l6nz * l6nz), 0.5);
  l6nx = l6nx / denoml6;
  l6ny = l6ny / denoml6;
  l6nz = l6nz / denoml6;
  double l7nx = (0.866025 * ph1spnx7) + (0.0 * ph1spny7);
  double l7ny = (-0.5 * ph1spnx7) + (1.0 * ph1spny7);
  double l7nz = double((l7nx * -l7nx + l7ny * -l7ny)) / (caratio * 0.8164);
  double denoml7 = std::pow((l7nx * l7nx + l7ny * l7ny + l7nz * l7nz), 0.5);
  l7nx = l7nx / denoml7;
  l7ny = l7ny / denoml7;
  l7nz = l7nz / denoml7;
  double l8nx = (0.866025 * ph1spnx8) + (0 * ph1spny8);
  double l8ny = (-0.5 * ph1spnx8) + (1.0 * ph1spny8);
  double l8nz = double(-(l8nx * -l8nx + l8ny * -l8ny)) / (caratio * 0.8164);
  double denoml8 = std::pow((l8nx * l8nx + l8ny * l8ny + l8nz * l8nz), 0.5);
  l8nx = l8nx / denoml8;
  l8ny = l8ny / denoml8;
  l8nz = l8nz / denoml8;
  double l9nx = (0.866025 * ph1spnx9) + (0.0 * ph1spny9);
  double l9ny = (-0.5 * ph1spnx9) + (1.0 * ph1spny9);
  double l9nz = double((l9nx * -l9nx + l9ny * -l9ny)) / (caratio * 1.154);
  double denoml9 = std::pow((l9nx * l9nx + l9ny * l9ny + l9nz * l9nz), 0.5);
  l9nx = l9nx / denoml9;
  l9ny = l9ny / denoml9;
  l9nz = l9nz / denoml9;
  double l10nx = (0.866025 * ph1spnx10) + (0.0 * ph1spny10);
  double l10ny = (-0.5 * ph1spnx10) + (1 * ph1spny10);
  double l10nz = double(-(l10nx * -l10nx + l10ny * -l10ny)) / (caratio * 1.154);
  double denoml10 = std::pow((l10nx * l10nx + l10ny * l10ny + l10nz * l10nz), 0.5);
  l10nx = l10nx / denoml10;
  l10ny = l10ny / denoml10;
  l10nz = l10nz / denoml10;
  lambda5 = ((l5nx * loadx) + (l5ny * loady) + (-l5nz * loadz));
  lambda5 = fabs(lambda5);
  lambda6 = ((l5nx * loadx) + (l5ny * loady) + (l5nz * loadz));
  lambda6 = fabs(lambda6);
  lambda7 = ((l7nx * loadx) + (l7ny * loady) + (-l7nz * loadz));
  lambda7 = fabs(lambda7);
  lambda8 = ((l7nx * loadx) + (l7ny * loady) + (l7nz * loadz));
  lambda8 = fabs(lambda8);
  lambda9 = ((l9nx * loadx) + (l9ny * loady) + (-l9nz * loadz));
  lambda9 = fabs(lambda9);
  lambda10 = ((l9nx * loadx) + (l9ny * loady) + (l9nz * loadz));
  lambda10 = fabs(lambda10);
  //  schmid7 = theta1 * lambda5;
  //  schmid8 = theta1 * lambda6;
  //  schmid9 = theta2 * lambda7;
  //  schmid10 = theta2 * lambda8;
  //  schmid11 = theta3 * lambda9;
  //  schmid12 = theta3 * lambda10;
  double t4x = (0.866025 * ph1sdx4) + (0.0 * ph1sdy4) + (0.0 * ph1sdz4);
  double t4y = (-0.5 * ph1sdx4) + (1.0 * ph1sdy4) + (0.0 * ph1sdz4);
  double t4z = (0.0 * ph1sdx4) + (0.0 * ph1sdy4) + (caratio * ph1sdz4);
  double denomt4 = std::pow((t4x * t4x + t4y * t4y + t4z * t4z), 0.5);
  t4x = t4x / denomt4;
  t4y = t4y / denomt4;
  t4z = t4z / denomt4;
  theta4 = ((t4x * loadx) + (t4y * loady) + (t4z * loadz));
  theta4 = fabs(theta4);
  double t5x = (0.866025 * ph1sdx5) + (0.0 * ph1sdy5) + (0.0 * ph1sdz5);
  double t5y = (-0.5 * ph1sdx5) + (1.0 * ph1sdy5) + (0.0 * ph1sdz5);
  double t5z = (0.0 * ph1sdx5) + (0.0 * ph1sdy5) + (caratio * ph1sdz5);
  double denomt5 = std::pow((t5x * t5x + t5y * t5y + t5z * t5z), 0.5);
  t5x = t5x / denomt5;
  t5y = t5y / denomt5;
  t5z = t5z / denomt5;
  theta5 = ((t5x * loadx) + (t5y * loady) + (t5z * loadz));
  theta5 = fabs(theta5);
  double t6x = (0.866025 * ph1sdx6) + (0.0 * ph1sdy6) + (0.0 * ph1sdz6);
  double t6y = (-0.5 * ph1sdx6) + (1.0 * ph1sdy6) + (0.0 * ph1sdz6);
  double t6z = (0.0 * ph1sdx6) + (0.0 * ph1sdy6) + (caratio * ph1sdz6);
  double denomt6 = std::pow((t6x * t6x + t6y * t6y + t6z * t6z), 0.5);
  t6x = t6x / denomt6;
  t6y = t6y / denomt6;
  t6z = t6z / denomt6;
  theta6 = ((t6x * loadx) + (t6y * loady) + (t6z * loadz));
  theta6 = fabs(theta6);
  double t7x = (0.866025 * ph1sdx7) + (0.0 * ph1sdy7) + (0.0 * ph1sdz7);
  double t7y = (-0.5 * ph1sdx7) + (1.0 * ph1sdy7) + (0.0 * ph1sdz7);
  double t7z = (0.0 * ph1sdx7) + (0.0 * ph1sdy7) + (caratio * ph1sdz7);
  double denomt7 = std::pow((t7x * t7x + t7y * t7y + t7z * t7z), 0.5);
  t7x = t7x / denomt7;
  t7y = t7y / denomt7;
  t7z = t7z / denomt7;
  theta7 = ((t7x * loadx) + (t7y * loady) + (t7z * loadz));
  theta7 = fabs(theta7);
  double t8x = (0.866025 * ph1sdx8) + (0.0 * ph1sdy8) + (0.0 * ph1sdz8);
  double t8y = (-0.5 * ph1sdx8) + (1.0 * ph1sdy8) + (0.0 * ph1sdz8);
  double t8z = (0.0 * ph1sdx8) + (0.0 * ph1sdy8) + (caratio * ph1sdz8);
  double denomt8 = std::pow((t8x * t8x + t8y * t8y + t8z * t8z), 0.5);
  t8x = t8x / denomt8;
  t8y = t8y / denomt8;
  t8z = t8z / denomt8;
  theta8 = ((t8x * loadx) + (t8y * loady) + (t8z * loadz));
  theta8 = fabs(theta8);
  double t9x = (0.866025 * ph1sdx9) + (0.0 * ph1sdy9) + (0.0 * ph1sdz9);
  double t9y = (-0.5 * ph1sdx9) + (1 * ph1sdy9) + (0 * ph1sdz9);
  double t9z = (0.0 * ph1sdx9) + (0.0 * ph1sdy9) + (caratio * ph1sdz9);
  double denomt9 = std::pow((t9x * t9x + t9y * t9y + t9z * t9z), 0.5);
  t9x = t9x / denomt9;
  t9y = t9y / denomt9;
  t9z = t9z / denomt9;
  theta9 = ((t9x * loadx) + (t9y * loady) + (t9z * loadz));
  theta9 = fabs(theta9);
  //  schmid13 = theta7 * lambda6;
  //  schmid14 = theta8 * lambda6;
  //  schmid15 = theta6 * lambda9;
  //  schmid16 = theta4 * lambda9;
  //  schmid17 = theta6 * lambda8;
  //  schmid18 = theta7 * lambda8;
  //  schmid19 = theta4 * lambda5;
  //  schmid20 = theta5 * lambda5;
  //  schmid21 = theta8 * lambda10;
  //  schmid22 = theta9 * lambda10;
  //  schmid23 = theta9 * lambda7;
  //  schmid24 = theta5 * lambda7;
  if(schmid1 > schmidfactor)
  {
    schmidfactor = schmid1, slipsys = 1, angleComps[0] = theta1, angleComps[1] = lambda1;
  }
  if(schmid2 > schmidfactor)
  {
    schmidfactor = schmid2, slipsys = 2, angleComps[0] = theta2, angleComps[1] = lambda1;
  }
  if(schmid3 > schmidfactor)
  {
    schmidfactor = schmid3, slipsys = 3, angleComps[0] = theta3, angleComps[1] = lambda1;
  }
  if(schmid4 > schmidfactor)
  {
    schmidfactor = schmid4, slipsys = 4, angleComps[0] = theta1, angleComps[1] = lambda2;
  }
  if(schmid5 > schmidfactor)
  {
    schmidfactor = schmid5, slipsys = 5, angleComps[0] = theta2, angleComps[1] = lambda3;
  }
  if(schmid6 > schmidfactor)
  {
    schmidfactor = schmid6, slipsys = 6, angleComps[0] = theta3, angleComps[1] = lambda4;
  }
  //if(schmid7 > schmidfactor) { schmidfactor = schmid7, slipsys = 7; }
  //if(schmid8 > schmidfactor) { schmidfactor = schmid8, slipsys = 8; }
  //if(schmid9 > schmidfactor) { schmidfactor = schmid9, slipsys = 9; }
  //if(schmid10 > schmidfactor) { schmidfactor = schmid10, slipsys = 10; }
  //if(schmid11 > schmidfactor) { schmidfactor = schmid11, slipsys = 11; }
  //if(schmid12 > schmidfactor) { schmidfactor = schmid12, slipsys = 12; }
  //if(schmid13 > schmidfactor) { schmidfactor = schmid13, slipsys = 13; }
  //if(schmid14 > schmidfactor) { schmidfactor = schmid14, slipsys = 14; }
  //if(schmid15 > schmidfactor) { schmidfactor = schmid15, slipsys = 15; }
  //if(schmid16 > schmidfactor) { schmidfactor = schmid16, slipsys = 16; }
  //if(schmid17 > schmidfactor) { schmidfactor = schmid17, slipsys = 17; }
  //if(schmid18 > schmidfactor) { schmidfactor = schmid18, slipsys = 18; }
  //if(schmid19 > schmidfactor) { schmidfactor = schmid19, slipsys = 19; }
  //if(schmid20 > schmidfactor) { schmidfactor = schmid20, slipsys = 20; }
  //if(schmid21 > schmidfactor) { schmidfactor = schmid21, slipsys = 21; }
  //if(schmid22 > schmidfactor) { schmidfactor = schmid22, slipsys = 22; }
  //if(schmid23 > schmidfactor) { schmidfactor = schmid23, slipsys = 23; }
  //if(schmid24 > schmidfactor) { schmidfactor = schmid24, slipsys = 24; }
}

void HexagonalLowOps::getSchmidFactorAndSS(double load[3], double plane[3], double direction[3], double& schmidfactor, double angleComps[2], int& slipsys) const
{
  schmidfactor = 0;
  slipsys = 0;
  angleComps[0] = 0;
  angleComps[1] = 0;

  //compute mags
  double loadMag = sqrt(load[0] * load[0] + load[1] * load[1] + load[2] * load[2]);
  double planeMag = sqrt(plane[0] * plane[0] + plane[1] * plane[1] + plane[2] * plane[2]);
  double directionMag = sqrt(direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);
  planeMag *= loadMag;
  directionMag *= loadMag;

  //loop over symmetry operators finding highest schmid factor
  for(int i = 0; i < k_NumSymQuats; i++)
  {
    //compute slip system
    double slipPlane[3] = {0};
    slipPlane[2] = HexMatSym[i][2][0] * plane[0] + HexMatSym[i][2][1] * plane[1] + HexMatSym[i][2][2] * plane[2];

    //dont consider negative z planes (to avoid duplicates)
    if( slipPlane[2] >= 0)
    {
      slipPlane[0] = HexMatSym[i][0][0] * plane[0] + HexMatSym[i][0][1] * plane[1] + HexMatSym[i][0][2] * plane[2];
      slipPlane[1] = HexMatSym[i][1][0] * plane[0] + HexMatSym[i][1][1] * plane[1] + HexMatSym[i][1][2] * plane[2];

      double slipDirection[3] = {0};
      slipDirection[0] = HexMatSym[i][0][0] * direction[0] + HexMatSym[i][0][1] * direction[1] + HexMatSym[i][0][2] * direction[2];
      slipDirection[1] = HexMatSym[i][1][0] * direction[0] + HexMatSym[i][1][1] * direction[1] + HexMatSym[i][1][2] * direction[2];
      slipDirection[2] = HexMatSym[i][2][0] * direction[0] + HexMatSym[i][2][1] * direction[1] + HexMatSym[i][2][2] * direction[2];

      double cosPhi = fabs(load[0] * slipPlane[0] + load[1] * slipPlane[1] + load[2] * slipPlane[2]) / planeMag;
      double cosLambda = fabs(load[0] * slipDirection[0] + load[1] * slipDirection[1] + load[2] * slipDirection[2]) / directionMag;

      double schmid = cosPhi * cosLambda;
      if(schmid > schmidfactor)
      {
        schmidfactor = schmid;
        slipsys = i;
        angleComps[0] = acos(cosPhi);
        angleComps[1] = acos(cosLambda);
      }
    }
  }
}

double HexagonalLowOps::getmPrime(const QuatType& q1, const QuatType& q2, double LD[3]) const
{
  Q_ASSERT(false);
#if 0
  /* I am asserting here because this code will simply give junk results and if someone uses it
   * they could unknowningly get really bad results
   */
  double g1[3][3];
  double g2[3][3];
  double h1, k1, l1, u1, v1, w1;
  double h2, k2, l2, u2, v2, w2;
  double denomhkl1, denomhkl2, denomuvw1, denomuvw2;
  double planemisalignment, directionmisalignment;
  QuattoMat(q1, g1);
  QuattoMat(q2, g2);
  // Note the order of multiplication is such that I am actually multiplying by the inverse of g1 and g2
  /*  h1 = CubicSlipSystems[ss1][0]*g1[0][0]+CubicSlipSystems[ss1][1]*g1[1][0]+CubicSlipSystems[ss1][2]*g1[2][0];
  k1 = CubicSlipSystems[ss1][0]*g1[0][1]+CubicSlipSystems[ss1][1]*g1[1][1]+CubicSlipSystems[ss1][2]*g1[2][1];
  l1 = CubicSlipSystems[ss1][0]*g1[0][2]+CubicSlipSystems[ss1][1]*g1[1][2]+CubicSlipSystems[ss1][2]*g1[2][2];
  u1 = CubicSlipSystems[ss1][3]*g1[0][0]+CubicSlipSystems[ss1][4]*g1[1][0]+CubicSlipSystems[ss1][5]*g1[2][0];
  v1 = CubicSlipSystems[ss1][3]*g1[0][1]+CubicSlipSystems[ss1][4]*g1[1][1]+CubicSlipSystems[ss1][5]*g1[2][1];
  w1 = CubicSlipSystems[ss1][3]*g1[0][2]+CubicSlipSystems[ss1][4]*g1[1][2]+CubicSlipSystems[ss1][5]*g1[2][2];
  denomhkl1 = sqrtf((h1*h1+k1*k1+l1*l1));
  denomuvw1 = sqrtf((u1*u1+v1*v1+w1*w1));
  h2 = CubicSlipSystems[ss2][0]*g2[0][0]+CubicSlipSystems[ss2][1]*g2[1][0]+CubicSlipSystems[ss2][2]*g2[2][0];
  k2 = CubicSlipSystems[ss2][0]*g2[0][1]+CubicSlipSystems[ss2][1]*g2[1][1]+CubicSlipSystems[ss2][2]*g2[2][1];
  l2 = CubicSlipSystems[ss2][0]*g2[0][2]+CubicSlipSystems[ss2][1]*g2[1][2]+CubicSlipSystems[ss2][2]*g2[2][2];
  u2 = CubicSlipSystems[ss2][3]*g2[0][0]+CubicSlipSystems[ss2][4]*g2[1][0]+CubicSlipSystems[ss2][5]*g2[2][0];
  v2 = CubicSlipSystems[ss2][3]*g2[0][1]+CubicSlipSystems[ss2][4]*g2[1][1]+CubicSlipSystems[ss2][5]*g2[2][1];
  w2 = CubicSlipSystems[ss2][3]*g2[0][2]+CubicSlipSystems[ss2][4]*g2[1][2]+CubicSlipSystems[ss2][5]*g2[2][2];
  */
  denomhkl2 = sqrtf((h2 * h2 + k2 * k2 + l2 * l2));
  denomuvw2 = sqrtf((u2 * u2 + v2 * v2 + w2 * w2));
  planemisalignment = fabs((h1 * h2 + k1 * k2 + l1 * l2) / (denomhkl1 * denomhkl2));
  directionmisalignment = fabs((u1 * u2 + v1 * v2 + w1 * w2) / (denomuvw1 * denomuvw2));
  mPrime = planemisalignment * directionmisalignment;
#endif
return 0.0;
}

double HexagonalLowOps::getF1(const QuatType& q1, const QuatType& q2, double LD[3], bool maxS) const
{
  Q_ASSERT(false);
  return 0.0;
#if 0
  /* I am asserting here because this code will simply give junk results and if someone uses it
   * they could unknowningly get really bad results
   */
  double g1[3][3];
  double g2[3][3];
  //  double hkl1[3], uvw1[3];
  //  double hkl2[3], uvw2[3];
  //  double slipDirection[3], slipPlane[3];
  //  double denomhkl1=0, denomhkl2=0, denomuvw1=0, denomuvw2=0;
  //  double directionMisalignment=0, totalDirectionMisalignment=0;
  //  double schmidFactor1=0, schmidFactor2=0;
  double maxSchmidFactor = 0;
  //  double directionComponent1=0, planeComponent1=0;
  //  double directionComponent2=0, planeComponent2=0;
  //  double maxF1=0;

  QuattoMat(q1, g1);
  QuattoMat(q2, g2);
  MatrixMath::Normalize3x1(LD);
  // Note the order of multiplication is such that I am actually multiplying by the inverse of g1 and g2
  if(maxSF == true)
  {
    maxSchmidFactor = 0;
  }
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
double HexagonalLowOps::getF1spt(const QuatType& q1, const QuatType& q2, double LD[3], bool maxS) const
{
  Q_ASSERT(false);
  return 0.0;
#if 0
  double g1[3][3];
  double g2[3][3];
  //  double hkl1[3], uvw1[3];
  //  double hkl2[3], uvw2[3];
  //  double slipDirection[3], slipPlane[3];
  //  double directionMisalignment=0, totalDirectionMisalignment=0;
  //  double planeMisalignment=0, totalPlaneMisalignment=0;
  //  double schmidFactor1=0, schmidFactor2=0;
  double maxSchmidFactor = 0;
  //  double directionComponent1=0, planeComponent1=0;
  //  double directionComponent2=0, planeComponent2=0;
  //  double maxF1spt=0;

  QuattoMat(q1, g1);
  QuattoMat(q2, g2);
  MatrixMath::Normalize3x1(LD);
  // Note the order of multiplication is such that I am actually multiplying by the inverse of g1 and g2
  if(maxSF == true)
  {
    maxSchmidFactor = 0;
  }
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

double HexagonalLowOps::getF7(const QuatType& q1, const QuatType& q2, double LD[3], bool maxS) const
{
  Q_ASSERT(false);
  return 0.0;
#if 0
  double g1[3][3];
  double g2[3][3];
  //  double hkl1[3], uvw1[3];
  //  double hkl2[3], uvw2[3];
  //  double slipDirection[3], slipPlane[3];
  //  double directionMisalignment=0, totalDirectionMisalignment=0;
  //  double schmidFactor1=0, schmidFactor2=0, maxSchmidFactor=0;
  //  double directionComponent1=0, planeComponent1=0;
  //  double directionComponent2=0, planeComponent2=0;
  //  double maxF7=0;

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
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
namespace Detail
{
  namespace HexagonalLow
  {
    class GenerateSphereCoordsImpl
    {
        FloatArrayType* m_Eulers;
        FloatArrayType* m_xyz001;
        FloatArrayType* m_xyz011;
        FloatArrayType* m_xyz111;

      public:
        GenerateSphereCoordsImpl(FloatArrayType* eulerAngles, FloatArrayType* xyz001Coords, FloatArrayType* xyz011Coords, FloatArrayType* xyz111Coords) :
          m_Eulers(eulerAngles),
          m_xyz001(xyz001Coords),
          m_xyz011(xyz011Coords),
          m_xyz111(xyz111Coords)
        {}
        virtual ~GenerateSphereCoordsImpl() = default;

        void generate(size_t start, size_t end) const
        {
          double g[3][3];
          double gTranpose[3][3];
          double direction[3] = {0.0, 0.0, 0.0};

          for(size_t i = start; i < end; ++i)
          {
            OrientationType eu(m_Eulers->getValue(i * 3), m_Eulers->getValue(i * 3 + 1), m_Eulers->getValue(i * 3 + 2));
            OrientationTransformation::eu2om<OrientationType, OrientationType>(eu).toGMatrix(g);

            MatrixMath::Transpose3x3(g, gTranpose);

            // -----------------------------------------------------------------------------
            // 001 Family
            direction[0] = 0.0;
            direction[1] = 0.0;
            direction[2] = 1.0;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz001->getPointer(i * 6));
            MatrixMath::Copy3x1(m_xyz001->getPointer(i * 6), m_xyz001->getPointer(i * 6 + 3));
            MatrixMath::Multiply3x1withConstant(m_xyz001->getPointer(i * 6 + 3), -1.0f);

            // -----------------------------------------------------------------------------
            // 011 Family
            direction[0] = -0.5;
            direction[1] = SIMPLib::Constants::k_Root3Over2;
            direction[2] = 0.0;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz011->getPointer(i * 6));
            MatrixMath::Copy3x1(m_xyz011->getPointer(i * 6), m_xyz011->getPointer(i * 6 + 3));
            MatrixMath::Multiply3x1withConstant(m_xyz011->getPointer(i * 6 + 3), -1.0f);

            // -----------------------------------------------------------------------------
            // 111 Family
            direction[0] = 1;
            direction[1] = 0;
            direction[2] = 0;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz111->getPointer(i * 6));
            MatrixMath::Copy3x1(m_xyz111->getPointer(i * 6), m_xyz111->getPointer(i * 6 + 3));
            MatrixMath::Multiply3x1withConstant(m_xyz111->getPointer(i * 6 + 3), -1.0f);
          }

        }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
        void operator()(const tbb::blocked_range<size_t>& r) const
        {
          generate(r.begin(), r.end());
        }
#endif
    };
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HexagonalLowOps::generateSphereCoordsFromEulers(FloatArrayType* eulers, FloatArrayType* xyz0001, FloatArrayType* xyz1010, FloatArrayType* xyz1120) const
{
  size_t nOrientations = eulers->getNumberOfTuples();


  // Sanity Check the size of the arrays
  if (xyz0001->getNumberOfTuples() < nOrientations * Detail::HexagonalLow::symSize0)
  {
    xyz0001->resizeTuples(nOrientations * Detail::HexagonalLow::symSize0 * 3);
  }
  if (xyz1010->getNumberOfTuples() < nOrientations * Detail::HexagonalLow::symSize1)
  {
    xyz1010->resizeTuples(nOrientations * Detail::HexagonalLow::symSize1 * 3);
  }
  if (xyz1120->getNumberOfTuples() < nOrientations * Detail::HexagonalLow::symSize2)
  {
    xyz1120->resizeTuples(nOrientations * Detail::HexagonalLow::symSize2 * 3);
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  if(doParallel)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, nOrientations),
                      Detail::HexagonalLow::GenerateSphereCoordsImpl(eulers, xyz0001, xyz1010, xyz1120), tbb::auto_partitioner());
  }
  else
#endif
  {
    Detail::HexagonalLow::GenerateSphereCoordsImpl serial(eulers, xyz0001, xyz1010, xyz1120);
    serial.generate(0, nOrientations);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool HexagonalLowOps::inUnitTriangle(double eta, double chi) const
{
  return !(eta < 0 || eta > (60.0 * SIMPLib::Constants::k_PiOver180) || chi < 0 || chi > (90.0 * SIMPLib::Constants::k_PiOver180));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPL::Rgb HexagonalLowOps::generateIPFColor(double* eulers, double* refDir, bool convertDegrees) const
{
  return generateIPFColor(eulers[0], eulers[1], eulers[2], refDir[0], refDir[1], refDir[2], convertDegrees);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPL::Rgb HexagonalLowOps::generateIPFColor(double phi1, double phi, double phi2, double refDir0, double refDir1, double refDir2, bool degToRad) const
{
  if(degToRad)
  {
    phi1 = phi1 * SIMPLib::Constants::k_DegToRad;
    phi = phi * SIMPLib::Constants::k_DegToRad;
    phi2 = phi2 * SIMPLib::Constants::k_DegToRad;
  }

  double g[3][3];
  double p[3];
  double refDirection[3] = {0.0f, 0.0f, 0.0f};
  double chi = 0.0f, eta = 0.0;
  double _rgb[3] = {0.0, 0.0, 0.0};

  OrientationType eu(phi1, phi, phi2);
  OrientationType om(9); // Reusable for the loop
  QuatType q1 = OrientationTransformation::eu2qu<OrientationType, QuatType>(eu);

  for(int j = 0; j < k_NumSymQuats; j++)
  {
    QuatType qu = getQuatSymOp(j) * q1;
    OrientationTransformation::qu2om<QuatType, OrientationType>(qu).toGMatrix(g);

    refDirection[0] = refDir0;
    refDirection[1] = refDir1;
    refDirection[2] = refDir2;
    MatrixMath::Multiply3x3with3x1(g, refDirection, p);
    MatrixMath::Normalize3x1(p);

    if(!getHasInversion() && p[2] < 0)
    {
      continue;
    }
    if(getHasInversion() && p[2] < 0)
    {
      p[0] = -p[0], p[1] = -p[1], p[2] = -p[2];
    }
    chi = std::acos(p[2]);
    eta = std::atan2(p[1], p[0]);
    if(!inUnitTriangle(eta, chi))
    {
      continue;
    }

    break;
  }

  double etaMin = 0.0;
  double etaMax = 60.0;
  double chiMax = 90.0;
  double etaDeg = eta * SIMPLib::Constants::k_180OverPi;
  double chiDeg = chi * SIMPLib::Constants::k_180OverPi;

  _rgb[0] = 1.0 - chiDeg / chiMax;
  _rgb[2] = fabs(etaDeg - etaMin) / (etaMax - etaMin);
  _rgb[1] = 1 - _rgb[2];
  _rgb[1] *= chiDeg / chiMax;
  _rgb[2] *= chiDeg / chiMax;
  _rgb[0] = sqrt(_rgb[0]);
  _rgb[1] = sqrt(_rgb[1]);
  _rgb[2] = sqrt(_rgb[2]);

  double max = _rgb[0];
  if (_rgb[1] > max)
  {
    max = _rgb[1];
  }
  if (_rgb[2] > max)
  {
    max = _rgb[2];
  }

  _rgb[0] = _rgb[0] / max;
  _rgb[1] = _rgb[1] / max;
  _rgb[2] = _rgb[2] / max;

  return RgbColor::dRgb(_rgb[0] * 255, _rgb[1] * 255, _rgb[2] * 255, 255);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPL::Rgb HexagonalLowOps::generateRodriguesColor(double r1, double r2, double r3) const
{
  double range1 = 2.0 * HexDim1InitValue;
  double range2 = 2.0 * HexDim2InitValue;
  double range3 = 2.0 * HexDim3InitValue;
  double max1 = range1 / 2.0;
  double max2 = range2 / 2.0;
  double max3 = range3 / 2.0;
  double red = (r1 + max1) / range1;
  double green = (r2 + max2) / range2;
  double blue = (r3 + max3) / range3;

  // Scale values from 0 to 1.0
  red = red / max1;
  green = green / max1;
  blue = blue / max2;

  return RgbColor::dRgb(red * 255, green * 255, blue * 255, 255);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<UInt8ArrayType::Pointer> HexagonalLowOps::generatePoleFigure(PoleFigureConfiguration_t& config) const
{
  QString label0 = QString("<0001>");
  QString label1 = QString("<11-20>");
  QString label2 = QString("<2-1-10>");
  if(!config.labels.empty())
  {
    label0 = config.labels.at(0);
  }
  if(config.labels.size() > 1) { label1 = config.labels.at(1); }
  if(config.labels.size() > 2) { label2 = config.labels.at(2); }

  int numOrientations = config.eulers->getNumberOfTuples();

  // Create an Array to hold the XYZ Coordinates which are the coords on the sphere.
  // this is size for CUBIC ONLY, <001> Family
  std::vector<size_t> dims(1, 3);
  FloatArrayType::Pointer xyz001 = FloatArrayType::CreateArray(numOrientations * Detail::HexagonalLow::symSize0, dims, label0 + QString("xyzCoords"), true);
  // this is size for CUBIC ONLY, <011> Family
  FloatArrayType::Pointer xyz011 = FloatArrayType::CreateArray(numOrientations * Detail::HexagonalLow::symSize1, dims, label1 + QString("xyzCoords"), true);
  // this is size for CUBIC ONLY, <111> Family
  FloatArrayType::Pointer xyz111 = FloatArrayType::CreateArray(numOrientations * Detail::HexagonalLow::symSize2, dims, label2 + QString("xyzCoords"), true);

  config.sphereRadius = 1.0;

  // Generate the coords on the sphere **** Parallelized
  generateSphereCoordsFromEulers(config.eulers, xyz001.get(), xyz011.get(), xyz111.get());


  // These arrays hold the "intensity" images which eventually get converted to an actual Color RGB image
  // Generate the modified Lambert projection images (Squares, 2 of them, 1 for northern hemisphere, 1 for southern hemisphere
  DoubleArrayType::Pointer intensity001 = DoubleArrayType::CreateArray(config.imageDim * config.imageDim, label0 + "_Intensity_Image", true);
  DoubleArrayType::Pointer intensity011 = DoubleArrayType::CreateArray(config.imageDim * config.imageDim, label1 + "_Intensity_Image", true);
  DoubleArrayType::Pointer intensity111 = DoubleArrayType::CreateArray(config.imageDim * config.imageDim, label2 + "_Intensity_Image", true);
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;

  if(doParallel)
  {
    std::shared_ptr<tbb::task_group> g(new tbb::task_group);
    g->run(ComputeStereographicProjection(xyz001.get(), &config, intensity001.get()));
    g->run(ComputeStereographicProjection(xyz011.get(), &config, intensity011.get()));
    g->run(ComputeStereographicProjection(xyz111.get(), &config, intensity111.get()));
    g->wait(); // Wait for all the threads to complete before moving on.

  }
  else
#endif
  {
    ComputeStereographicProjection m001(xyz001.get(), &config, intensity001.get());
    m001();
    ComputeStereographicProjection m011(xyz011.get(), &config, intensity011.get());
    m011();
    ComputeStereographicProjection m111(xyz111.get(), &config, intensity111.get());
    m111();
  }

  // Find the Max and Min values based on ALL 3 arrays so we can color scale them all the same
  double max = std::numeric_limits<double>::min();
  double min = std::numeric_limits<double>::max();

  double* dPtr = intensity001->getPointer(0);
  size_t count = intensity001->getNumberOfTuples();
  for(size_t i = 0; i < count; ++i)
  {
    if (dPtr[i] > max)
    {
      max = dPtr[i];
    }
    if (dPtr[i] < min)
    {
      min = dPtr[i];
    }
  }


  dPtr = intensity011->getPointer(0);
  count = intensity011->getNumberOfTuples();
  for(size_t i = 0; i < count; ++i)
  {
    if (dPtr[i] > max)
    {
      max = dPtr[i];
    }
    if (dPtr[i] < min)
    {
      min = dPtr[i];
    }
  }

  dPtr = intensity111->getPointer(0);
  count = intensity111->getNumberOfTuples();
  for(size_t i = 0; i < count; ++i)
  {
    if (dPtr[i] > max)
    {
      max = dPtr[i];
    }
    if (dPtr[i] < min)
    {
      min = dPtr[i];
    }
  }

  config.minScale = min;
  config.maxScale = max;

  dims[0] = 4;
  UInt8ArrayType::Pointer image001 = UInt8ArrayType::CreateArray(config.imageDim * config.imageDim, dims, label0, true);
  UInt8ArrayType::Pointer image011 = UInt8ArrayType::CreateArray(config.imageDim * config.imageDim, dims, label1, true);
  UInt8ArrayType::Pointer image111 = UInt8ArrayType::CreateArray(config.imageDim * config.imageDim, dims, label2, true);

  QVector<UInt8ArrayType::Pointer> poleFigures(3);
  if(config.order.size() == 3)
  {
    poleFigures[config.order[0]] = image001;
    poleFigures[config.order[1]] = image011;
    poleFigures[config.order[2]] = image111;
  }
  else
  {
    poleFigures[0] = image001;
    poleFigures[1] = image011;
    poleFigures[2] = image111;
  }
#ifdef SIMPL_USE_PARALLEL_ALGORITHMS

  if(doParallel)
  {
    std::shared_ptr<tbb::task_group> g(new tbb::task_group);
    g->run(GeneratePoleFigureRgbaImageImpl(intensity001.get(), &config, image001.get()));
    g->run(GeneratePoleFigureRgbaImageImpl(intensity011.get(), &config, image011.get()));
    g->run(GeneratePoleFigureRgbaImageImpl(intensity111.get(), &config, image111.get()));
    g->wait(); // Wait for all the threads to complete before moving on.

  }
  else
#endif
  {
    GeneratePoleFigureRgbaImageImpl m001(intensity001.get(), &config, image001.get());
    m001();
    GeneratePoleFigureRgbaImageImpl m011(intensity011.get(), &config, image011.get());
    m011();
    GeneratePoleFigureRgbaImageImpl m111(intensity111.get(), &config, image111.get());
    m111();
  }

  return poleFigures;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UInt8ArrayType::Pointer HexagonalLowOps::generateIPFTriangleLegend(int imageDim) const
{

  std::vector<size_t> dims(1, 4);
  UInt8ArrayType::Pointer image = UInt8ArrayType::CreateArray(imageDim * imageDim, dims, getSymmetryName() + " Triangle Legend", true);
  uint32_t* pixelPtr = reinterpret_cast<uint32_t*>(image->getPointer(0));

  double xInc = 1.0 / static_cast<double>(imageDim);
  double yInc = 1.0 / static_cast<double>(imageDim);
  double rad = 1.0;

  double x = 0.0;
  double y = 0.0;
  double a = 0.0;
  double b = 0.0;
  double c = 0.0;

  double val = 0.0;
  double x1 = 0.0;
  double y1 = 0.0;
  double z1 = 0.0;
  double denom = 0.0;

  // Find the slope of the bounding line.
  static const double m = sinf(60.0 * SIMPLib::Constants::k_PiOver180) / cosf(60.0 * SIMPLib::Constants::k_PiOver180);

  SIMPL::Rgb color;
  size_t idx = 0;
  size_t yScanLineIndex = imageDim-1; // We use this to control where the data is drawn. Otherwise the image will come out flipped vertically
  // Loop over every pixel in the image and project up to the sphere to get the angle and then figure out the RGB from
  // there.
  for (int32_t yIndex = 0; yIndex < imageDim; ++yIndex)
  {

    for (int32_t xIndex = 0; xIndex < imageDim; ++xIndex)
    {
      idx = (imageDim * yScanLineIndex) + xIndex;

      x = xIndex * xInc;
      y = yIndex * yInc;

      double sumSquares = (x * x) + (y * y);
      if( sumSquares > 1.0f || x < y/m) // Outside unit circle
      {
        color = 0xFFFFFFF;
      }
      else if ( sumSquares > (rad-2*xInc) && sumSquares < (rad+2*xInc)) // Black Border line
      {
        color = 0xFF000000;
      }
      else if( x - y/m < 0.001)
      {
        color = 0xFF000000;
      }
      else if (xIndex == 0 || yIndex == 0 )
      {
        color = 0xFF000000;
      }
      else
      {
        a = (x * x + y * y + 1);
        b = (2 * x * x + 2 * y * y);
        c = (x * x + y * y - 1);

        val = (-b + sqrtf(b * b - 4.0 * a * c)) / (2.0 * a);
        x1 = (1 + val) * x;
        y1 = (1 + val) * y;
        z1 = val;
        denom = (x1 * x1) + (y1 * y1) + (z1 * z1);
        denom = sqrtf(denom);
        x1 = x1 / denom;
        y1 = y1 / denom;
        z1 = z1 / denom;

        color = generateIPFColor(0.0, 0.0, 0.0, x1, y1, z1, false);
      }

      pixelPtr[idx] = color;
    }
    yScanLineIndex--;
  }
  return image;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPL::Rgb HexagonalLowOps::generateMisorientationColor(const QuatType& q, const QuatType& refFrame) const
{
  SIMPL::Rgb rgb = RgbColor::dRgb(0, 0, 0, 0);

  Q_ASSERT(false);

  return rgb;
}

// -----------------------------------------------------------------------------
HexagonalLowOps::Pointer HexagonalLowOps::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
QString HexagonalLowOps::getNameOfClass() const
{
  return QString("HexagonalLowOps");
}

// -----------------------------------------------------------------------------
QString HexagonalLowOps::ClassName()
{
  return QString("HexagonalLowOps");
}

// -----------------------------------------------------------------------------
HexagonalLowOps::Pointer HexagonalLowOps::New()
{
  Pointer sharedPtr(new(HexagonalLowOps));
  return sharedPtr;
}


