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

#include "TrigonalLowOps.h"

#include <cmath>

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

#include "OrientationLib/OrientationMath/OrientationArray.hpp"
#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"
#include "OrientationLib/Utilities/ComputeStereographicProjection.h"

namespace Detail
{

static const double TrigDim1InitValue = std::pow((0.75f * (SIMPLib::Constants::k_Pi - sinf(SIMPLib::Constants::k_Pi))), (1.0f / 3.0));
static const double TrigDim2InitValue = std::pow((0.75f * (SIMPLib::Constants::k_Pi - sinf(SIMPLib::Constants::k_Pi))), (1.0f / 3.0));
static const double TrigDim3InitValue = std::pow((0.75f * ((SIMPLib::Constants::k_Pi / 3.0) - sinf((SIMPLib::Constants::k_Pi / 3.0)))), (1.0f / 3.0));
static const double TrigDim1StepValue = TrigDim1InitValue / 36.0f;
static const double TrigDim2StepValue = TrigDim2InitValue / 36.0f;
static const double TrigDim3StepValue = TrigDim3InitValue / 12.0f;

namespace TrigonalLow
{
static const int symSize0 = 2;
static const int symSize1 = 2;
static const int symSize2 = 2;
} // namespace TrigonalLow
} // namespace Detail
static const QuatType TrigQuatSym[3] = {QuaternionMathType::New(0.000000000, 0.000000000, 0.000000000, 1.000000000), QuaternionMathType::New(0.000000000, 0.000000000, 0.866025400, 0.500000000),
                                        QuaternionMathType::New(0.000000000, 0.000000000, 0.866025400, -0.50000000)};

static const double TrigRodSym[3][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 1.73205}, {0.0, 0.0, -1.73205}};

static const double TrigMatSym[3][3][3] = {{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}},

                                           {{-0.5, SIMPLib::Constants::k_Root3Over2, 0.0}, {-SIMPLib::Constants::k_Root3Over2, -0.5, 0.0}, {0.0, 0.0, 1.0}},

                                           {{-0.5, -SIMPLib::Constants::k_Root3Over2, 0.0}, {SIMPLib::Constants::k_Root3Over2, -0.5, 0.0}, {0.0, 0.0, 1.0}}};

using namespace Detail;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TrigonalLowOps::TrigonalLowOps()
{
  double junk1 = TrigDim1StepValue * 1.0f;
  double junk2 = junk1 / TrigDim2StepValue;
  double junk3 = junk2 / TrigDim3StepValue;
  junk1 = junk3 / junk2;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TrigonalLowOps::~TrigonalLowOps() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool TrigonalLowOps::getHasInversion() const
{
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TrigonalLowOps::getODFSize() const
{
  return k_OdfSize;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TrigonalLowOps::getMDFSize() const
{
  return k_MdfSize;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TrigonalLowOps::getNumSymOps() const
{
  return k_NumSymQuats;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString TrigonalLowOps::getSymmetryName() const
{
  return "Trigonal -3";;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double TrigonalLowOps::_calcMisoQuat(const QuatType quatsym[6], int numsym, QuatType& q1, QuatType& q2, double& n1, double& n2, double& n3) const
{
  double wmin = 9999999.0f; //,na,nb,nc;
  double w = 0;
  double n1min = 0.0f;
  double n2min = 0.0f;
  double n3min = 0.0f;
  QuatType qr;
  QuatType qc;
  QuatType q2inv;

  QuaternionMathType::Copy(q2, q2inv);
  QuaternionMathType::Conjugate(q2inv);

  QuaternionMathType::Multiply(q1, q2inv, qr);
  for (int i = 0; i < numsym; i++)
  {
    QuaternionMathType::Multiply(quatsym[i], qr, qc);
    if (qc.w < -1)
    {
      qc.w = -1;
    }
    else if (qc.w > 1)
    {
      qc.w = 1;
    }

    OrientArrayType ax(4, 0.0);
    OrientTransformsType::qu2ax(OrientArrayType(qc.x, qc.y, qc.z, qc.w), ax);
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

double TrigonalLowOps::getMisoQuat(QuatType& q1, QuatType& q2, double& n1, double& n2, double& n3) const
{
  return _calcMisoQuat(TrigQuatSym, k_NumSymQuats, q1, q2, n1, n2, n3);
}

// -----------------------------------------------------------------------------
float TrigonalLowOps::getMisoQuat(QuatF& q1f, QuatF& q2f, float& n1f, float& n2f, float& n3f) const
{
  QuatType q1 = QuaternionMathType::FromType<float>(q1f);
  QuatType q2 = QuaternionMathType::FromType<float>(q2f);
  double n1 = n1f;
  double n2 = n2f;
  double n3 = n3f;
  float w = static_cast<float>(_calcMisoQuat(TrigQuatSym, k_NumSymQuats, q1, q2, n1, n2, n3));
  n1f = n1;
  n2f = n2;
  n3f = n3;
  return w;
}

void TrigonalLowOps::getQuatSymOp(int i, QuatType& q) const
{
  QuaternionMathD::Copy(TrigQuatSym[i], q);
}

void TrigonalLowOps::getRodSymOp(int i, double* r) const
{
  r[0] = TrigRodSym[i][0];
  r[1] = TrigRodSym[i][1];
  r[2] = TrigRodSym[i][2];
}

void TrigonalLowOps::getMatSymOp(int i, double g[3][3]) const
{
  g[0][0] = TrigMatSym[i][0][0];
  g[0][1] = TrigMatSym[i][0][1];
  g[0][2] = TrigMatSym[i][0][2];
  g[1][0] = TrigMatSym[i][1][0];
  g[1][1] = TrigMatSym[i][1][1];
  g[1][2] = TrigMatSym[i][1][2];
  g[2][0] = TrigMatSym[i][2][0];
  g[2][1] = TrigMatSym[i][2][1];
  g[2][2] = TrigMatSym[i][2][2];
}

void TrigonalLowOps::getMatSymOp(int i, float g[3][3]) const
{
  g[0][0] = TrigMatSym[i][0][0];
  g[0][1] = TrigMatSym[i][0][1];
  g[0][2] = TrigMatSym[i][0][2];
  g[1][0] = TrigMatSym[i][1][0];
  g[1][1] = TrigMatSym[i][1][1];
  g[1][2] = TrigMatSym[i][1][2];
  g[2][0] = TrigMatSym[i][2][0];
  g[2][1] = TrigMatSym[i][2][1];
  g[2][2] = TrigMatSym[i][2][2];
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientArrayType TrigonalLowOps::getODFFZRod(OrientArrayType rod) const
{
  int numsym = 3;
  return _calcRodNearestOrigin(TrigRodSym, numsym, rod);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientArrayType TrigonalLowOps::getMDFFZRod(OrientArrayType rod) const
{
  double FZn1 = 0.0, FZn2 = 0.0, FZn3 = 0.0, FZw = 0.0f;
  double n1n2mag = 0.0f;

  rod = _calcRodNearestOrigin(TrigRodSym, 12, rod);
  OrientArrayType ax(4, 0.0);
  OrientationTransforms<OrientArrayType, double>::ro2ax(rod, ax);

  double denom = sqrt(ax[0] * ax[0] + ax[1] * ax[1] + ax[2] * ax[2]);
  ax[0] = ax[0] / denom;
  ax[1] = ax[1] / denom;
  ax[1] = ax[2] / denom;
  if(ax[2] < 0)
  {
    ax[0] = -ax[0], ax[1] = -ax[1], ax[2] = -ax[2];
  }
  double angle = 180.0f * atan2(ax[1], ax[0]) * SIMPLib::Constants::k_1OverPi;
  if(angle < 0)
  {
    angle = angle + 360.0f;
  }
  FZn1 = ax[0];
  FZn2 = ax[1];
  FZn3 = ax[2];
  if(angle > 60.0)
  {
    n1n2mag = sqrt(ax[0] * ax[0] + ax[1] * ax[1]);
    if (int(angle / 60) % 2 == 0)
    {
      FZw = angle - (60.0f * int(angle / 60.0));
      FZw = FZw * SIMPLib::Constants::k_PiOver180;
      FZn1 = n1n2mag * cosf(FZw);
      FZn2 = n1n2mag * sinf(FZw);
    }
    else
    {
      FZw = angle - (60.0f * int(angle / 60.0));
      FZw = 60.0f - FZw;
      FZw = FZw * SIMPLib::Constants::k_PiOver180;
      FZn1 = n1n2mag * cosf(FZw);
      FZn2 = n1n2mag * sinf(FZw);
    }
  }

  ax.fromAxisAngle(FZn1, FZn2, FZn3, FZw);
  OrientationTransforms<OrientArrayType, double>::ax2ro(ax, rod);
  return rod;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TrigonalLowOps::getNearestQuat(QuatType& q1, QuatType& q2) const
{
  _calcNearestQuat(TrigQuatSym, k_NumSymQuats, q1, q2);
}
void TrigonalLowOps::getNearestQuat(QuatF& q1f, QuatF& q2f) const
{
  QuatType q1 = QuaternionMathType::FromType<float>(q1f);
  QuatType q2 = QuaternionMathType::FromType<float>(q2f);
  _calcNearestQuat(TrigQuatSym, k_NumSymQuats, q1, q2);
  q2f.x = q2.x;
  q2f.y = q2.y;
  q2f.z = q2.z;
  q2f.w = q2.w;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TrigonalLowOps::getMisoBin(OrientArrayType rod) const
{
  double dim[3];
  double bins[3];
  double step[3];

  OrientArrayType ho(3);
  OrientationTransforms<OrientArrayType, double>::ro2ho(rod, ho);

  dim[0] = TrigDim1InitValue;
  dim[1] = TrigDim2InitValue;
  dim[2] = TrigDim3InitValue;
  step[0] = TrigDim1StepValue;
  step[1] = TrigDim2StepValue;
  step[2] = TrigDim3StepValue;
  bins[0] = 72.0f;
  bins[1] = 72.0f;
  bins[2] = 24.0f;

  return _calcMisoBin(dim, bins, step, ho);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientArrayType TrigonalLowOps::determineEulerAngles(uint64_t seed, int choose) const
{
  double init[3];
  double step[3];
  int32_t phi[3];
  double h1, h2, h3;

  init[0] = TrigDim1InitValue;
  init[1] = TrigDim2InitValue;
  init[2] = TrigDim3InitValue;
  step[0] = TrigDim1StepValue;
  step[1] = TrigDim2StepValue;
  step[2] = TrigDim3StepValue;
  phi[0] = static_cast<int32_t>(choose % 72);
  phi[1] = static_cast<int32_t>((choose / 72) % 72);
  phi[2] = static_cast<int32_t>(choose / (72 * 72));

  _calcDetermineHomochoricValues(seed, init, step, phi, choose, h1, h2, h3);

  OrientArrayType ho(h1, h2, h3);
  OrientArrayType ro(4);
  OrientationTransforms<OrientArrayType, double>::ho2ro(ho, ro);

  ro = getODFFZRod(ro);
  OrientArrayType eu(4);
  OrientationTransforms<OrientArrayType, double>::ro2eu(ro, eu);
  return eu;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientArrayType TrigonalLowOps::randomizeEulerAngles(OrientArrayType synea) const
{
  QuatType q;
  QuatType qc;
  size_t symOp = getRandomSymmetryOperatorIndex(k_NumSymQuats);

  OrientArrayType quat(4, 0.0);
  OrientationTransforms<OrientArrayType, double>::eu2qu(synea, quat);
  q = quat.toQuaternion<double>();
  QuaternionMathType::Multiply(TrigQuatSym[symOp], q, qc);

  quat.fromQuaternion(qc);
  OrientationTransforms<OrientArrayType, double>::qu2eu(quat, synea);
  return synea;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientArrayType TrigonalLowOps::determineRodriguesVector(uint64_t seed, int choose) const
{
  double init[3];
  double step[3];
  int32_t phi[3];
  double h1, h2, h3;

  init[0] = TrigDim1InitValue;
  init[1] = TrigDim2InitValue;
  init[2] = TrigDim3InitValue;
  step[0] = TrigDim1StepValue;
  step[1] = TrigDim2StepValue;
  step[2] = TrigDim3StepValue;
  phi[0] = static_cast<int32_t>(choose % 72);
  phi[1] = static_cast<int32_t>((choose / 72) % 72);
  phi[2] = static_cast<int32_t>(choose / (72 * 72));

  _calcDetermineHomochoricValues(seed, init, step, phi, choose, h1, h2, h3);
  OrientArrayType ho(h1, h2, h3);
  OrientArrayType ro(4);
  OrientationTransforms<OrientArrayType, double>::ho2ro(ho, ro);
  ro = getMDFFZRod(ro);
  return ro;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int TrigonalLowOps::getOdfBin(OrientArrayType rod) const
{
  double dim[3];
  double bins[3];
  double step[3];

  OrientArrayType ho(3);
  OrientationTransforms<OrientArrayType, double>::ro2ho(rod, ho);

  dim[0] = TrigDim1InitValue;
  dim[1] = TrigDim2InitValue;
  dim[2] = TrigDim3InitValue;
  step[0] = TrigDim1StepValue;
  step[1] = TrigDim2StepValue;
  step[2] = TrigDim3StepValue;
  bins[0] = 72.0f;
  bins[1] = 72.0f;
  bins[2] = 24.0f;

  return _calcODFBin(dim, bins, step, ho);
}

void TrigonalLowOps::getSchmidFactorAndSS(double load[3], double& schmidfactor, double angleComps[2], int& slipsys) const
{
  schmidfactor = 0;
  slipsys = 0;
}

void TrigonalLowOps::getSchmidFactorAndSS(double load[3], double plane[3], double direction[3], double& schmidfactor, double angleComps[2], int& slipsys) const
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
    slipPlane[2] = TrigMatSym[i][2][0] * plane[0] + TrigMatSym[i][2][1] * plane[1] + TrigMatSym[i][2][2] * plane[2];

    //dont consider negative z planes (to avoid duplicates)
    if( slipPlane[2] >= 0)
    {
      slipPlane[0] = TrigMatSym[i][0][0] * plane[0] + TrigMatSym[i][0][1] * plane[1] + TrigMatSym[i][0][2] * plane[2];
      slipPlane[1] = TrigMatSym[i][1][0] * plane[0] + TrigMatSym[i][1][1] * plane[1] + TrigMatSym[i][1][2] * plane[2];

      double slipDirection[3] = {0};
      slipDirection[0] = TrigMatSym[i][0][0] * direction[0] + TrigMatSym[i][0][1] * direction[1] + TrigMatSym[i][0][2] * direction[2];
      slipDirection[1] = TrigMatSym[i][1][0] * direction[0] + TrigMatSym[i][1][1] * direction[1] + TrigMatSym[i][1][2] * direction[2];
      slipDirection[2] = TrigMatSym[i][2][0] * direction[0] + TrigMatSym[i][2][1] * direction[1] + TrigMatSym[i][2][2] * direction[2];

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

void TrigonalLowOps::getmPrime(QuatType& q1, QuatType& q2, double LD[3], double& mPrime) const
{
  mPrime = 0;
}

void TrigonalLowOps::getF1(QuatType& q1, QuatType& q2, double LD[3], bool maxS, double& F1) const
{
  F1 = 0;
}

void TrigonalLowOps::getF1spt(QuatType& q1, QuatType& q2, double LD[3], bool maxS, double& F1spt) const
{
  F1spt = 0;
}

void TrigonalLowOps::getF7(QuatType& q1, QuatType& q2, double LD[3], bool maxS, double& F7) const
{
  F7 = 0;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
namespace Detail
{
  namespace TrigonalLow
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

          // Geneate all the Coordinates
          for(size_t i = start; i < end; ++i)
          {
            OrientArrayType eu(m_Eulers->getValue(i * 3), m_Eulers->getValue(i * 3 + 1), m_Eulers->getValue(i * 3 + 2));
            OrientArrayType om(9, 0.0);
            OrientationTransforms<OrientArrayType, double>::eu2om(eu, om);
            om.toGMatrix(g);
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
void TrigonalLowOps::generateSphereCoordsFromEulers(FloatArrayType* eulers, FloatArrayType* xyz001, FloatArrayType* xyz011, FloatArrayType* xyz111) const
{
  size_t nOrientations = eulers->getNumberOfTuples();

  // Sanity Check the size of the arrays
  if (xyz001->getNumberOfTuples() < nOrientations * Detail::TrigonalLow::symSize0)
  {
    xyz001->resizeTuples(nOrientations * Detail::TrigonalLow::symSize0 * 3);
  }
  if (xyz011->getNumberOfTuples() < nOrientations * Detail::TrigonalLow::symSize1)
  {
    xyz011->resizeTuples(nOrientations * Detail::TrigonalLow::symSize1 * 3);
  }
  if (xyz111->getNumberOfTuples() < nOrientations * Detail::TrigonalLow::symSize2)
  {
    xyz111->resizeTuples(nOrientations * Detail::TrigonalLow::symSize2 * 3);
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  if(doParallel)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, nOrientations),
                      Detail::TrigonalLow::GenerateSphereCoordsImpl(eulers, xyz001, xyz011, xyz111), tbb::auto_partitioner());
  }
  else
#endif
  {
    Detail::TrigonalLow::GenerateSphereCoordsImpl serial(eulers, xyz001, xyz011, xyz111);
    serial.generate(0, nOrientations);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool TrigonalLowOps::inUnitTriangle(double eta, double chi) const
{
  return !(eta < (-120.0 * SIMPLib::Constants::k_PiOver180) || eta > 0.0 || chi < 0 || chi > (90.0 * SIMPLib::Constants::k_PiOver180));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPL::Rgb TrigonalLowOps::generateIPFColor(double* eulers, double* refDir, bool convertDegrees) const
{
  return generateIPFColor(eulers[0], eulers[1], eulers[2], refDir[0], refDir[1], refDir[2], convertDegrees);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPL::Rgb TrigonalLowOps::generateIPFColor(double phi1, double phi, double phi2, double refDir0, double refDir1, double refDir2, bool degToRad) const
{
  using OrientArrayType = OrientationArray<double>;
  using QuatType = QuaternionMath<double>::Quaternion;
  using QuaternionMathType = QuaternionMath<double>;
  if(degToRad)
  {
    phi1 = phi1 * SIMPLib::Constants::k_DegToRad;
    phi = phi * SIMPLib::Constants::k_DegToRad;
    phi2 = phi2 * SIMPLib::Constants::k_DegToRad;
  }
  QuatType qc = QuaternionMathType::New();
  QuatType q1 = QuaternionMathType::New();
  QuatType q2 = QuaternionMathType::New();
  double g[3][3];
  double p[3];
  double refDirection[3] = {0.0, 0.0, 0.0};
  double eta = 0.0;
  double chi = 0.0;
  double _rgb[3] = {0.0, 0.0, 0.0};

  // 1) find rotation matrix from Euler angles
  OrientArrayType eu(phi1, phi, phi2);
  OrientArrayType qu(4);
  OrientArrayType om(9); // Reusable for the loop
  OrientationTransforms<OrientArrayType, double>::qu2om(qu, om);
  q1 = qu.toQuaternion<double>();

  for (int j = 0; j < 3; j++)
  {
    getQuatSymOp(j, q2);
    QuaternionMathType::Multiply(q2, q1, qc);

    qu.fromQuaternion(qc);
    OrientationTransforms<OrientArrayType, double>::qu2om(qu, om);
    om.toGMatrix(g);

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

  double etaMin = -120.0;
  double etaMax = 0.0;
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
SIMPL::Rgb TrigonalLowOps::generateRodriguesColor(double r1, double r2, double r3) const
{
  double range1 = 2.0f * TrigDim1InitValue;
  double range2 = 2.0f * TrigDim2InitValue;
  double range3 = 2.0f * TrigDim3InitValue;
  double max1 = range1 / 2.0f;
  double max2 = range2 / 2.0f;
  double max3 = range3 / 2.0f;
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
QVector<UInt8ArrayType::Pointer> TrigonalLowOps::generatePoleFigure(PoleFigureConfiguration_t& config) const
{
  QString label0 = QString("<0001>");
  QString label1 = QString("<-1-120>");
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
  FloatArrayType::Pointer xyz001 = FloatArrayType::CreateArray(numOrientations * Detail::TrigonalLow::symSize0, dims, label0 + QString("xyzCoords"), true);
  // this is size for CUBIC ONLY, <011> Family
  FloatArrayType::Pointer xyz011 = FloatArrayType::CreateArray(numOrientations * Detail::TrigonalLow::symSize1, dims, label1 + QString("xyzCoords"), true);
  // this is size for CUBIC ONLY, <111> Family
  FloatArrayType::Pointer xyz111 = FloatArrayType::CreateArray(numOrientations * Detail::TrigonalLow::symSize2, dims, label2 + QString("xyzCoords"), true);

  config.sphereRadius = 1.0f;

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
UInt8ArrayType::Pointer TrigonalLowOps::generateIPFTriangleLegend(int imageDim) const
{

  std::vector<size_t> dims(1, 4);
  UInt8ArrayType::Pointer image = UInt8ArrayType::CreateArray(imageDim * imageDim, dims, getSymmetryName() + " Triangle Legend", true);
  uint32_t* pixelPtr = reinterpret_cast<uint32_t*>(image->getPointer(0));

  double xInc = 1.0f / static_cast<double>(imageDim);
  double yInc = 1.0f / static_cast<double>(imageDim);
  double rad = 1.0f;

  double x = 0.0f;
  double y = 0.0f;
  double a = 0.0f;
  double b = 0.0f;
  double c = 0.0f;

  double val = 0.0f;
  double x1 = 0.0f;
  double y1 = 0.0f;
  double z1 = 0.0f;
  double denom = 0.0f;

  // Find the slope of the bounding line.
  static const double m = sinf(60.0 * SIMPLib::Constants::k_PiOver180) / cosf(60.0 * SIMPLib::Constants::k_PiOver180);

  SIMPL::Rgb color;
  size_t idx = 0;
  size_t yScanLineIndex = 0; // We use this to control where the data is drawn. Otherwise the image will come out flipped vertically
  // Loop over every pixel in the image and project up to the sphere to get the angle and then figure out the RGB from
  // there.
  for (int32_t yIndex = 0; yIndex < imageDim; ++yIndex)
  {

    for (int32_t xIndex = 0; xIndex < imageDim; ++xIndex)
    {
      idx = (imageDim * yScanLineIndex) + xIndex;

      x = -1.0f + 2.0f * xIndex * xInc; // X Scales from ( -1 -> +1)
      y = 1.0f - 2.0f * yIndex * yInc; // Y Scales from (+1 -> -1)

      double sumSquares = (x * x) + (y * y);
      if(sumSquares > 1.0f || y > 0.0) // Outside unit circle
      {
        color = 0xFFFFFFFF;
      }
      else if(fabs(y - yInc) <= yInc && x >= 0.0) // Black Border line
      {
        color = 0xFF000000;
      }
      else if ( x <= 0.0f && y <= 0.0 && x < y/m)
      {
        color = 0xFFFFFFFF;
      }
      else if( x < 0.0f && y < 0.0 && fabs(x - y/m) < 0.005) // Black Diagonal Border line
      {
        color = 0xFF000000;
      }
      else if ( sumSquares > (rad-2*xInc) && sumSquares < (rad+2*xInc)) // Black Border line on circle
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
    yScanLineIndex++;
  }
  return image;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPL::Rgb TrigonalLowOps::generateMisorientationColor(const QuatType& q, const QuatType& refFrame) const
{
  SIMPL::Rgb rgb = RgbColor::dRgb(0, 0, 0, 0);

  Q_ASSERT(false);

  return rgb;
}

