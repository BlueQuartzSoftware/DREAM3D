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

#include "MonoclinicOps.h"

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


namespace Detail
{
static const double MonoclinicDim1InitValue = std::pow((0.75f * ((SIMPLib::Constants::k_Pi)-sinf((SIMPLib::Constants::k_Pi)))), (1.0f / 3.0));
static const double MonoclinicDim2InitValue = std::pow((0.75f * ((SIMPLib::Constants::k_PiOver2)-sinf((SIMPLib::Constants::k_PiOver2)))), (1.0f / 3.0));
static const double MonoclinicDim3InitValue = std::pow((0.75f * ((SIMPLib::Constants::k_Pi)-sinf((SIMPLib::Constants::k_Pi)))), (1.0f / 3.0));
static const double MonoclinicDim1StepValue = MonoclinicDim1InitValue / 36.0f;
static const double MonoclinicDim2StepValue = MonoclinicDim2InitValue / 18.0f;
static const double MonoclinicDim3StepValue = MonoclinicDim3InitValue / 36.0f;
namespace Monoclinic
{
static const int symSize0 = 2;
static const int symSize1 = 2;
static const int symSize2 = 2;
} // namespace Monoclinic
} // namespace Detail

static const QuatType MonoclinicQuatSym[2] = {QuatType(0.000000000, 0.000000000, 0.000000000, 1.000000000), QuatType(0.000000000, 1.000000000, 0.000000000, 0.000000000)};

static const double MonoclinicRodSym[2][3] = {{0.0, 0.0, 0.0}, {0.0, 10000000000.0, 0.0}};

static const double MonoclinicMatSym[2][3][3] = {{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}},

                                                 {{-1.0, 0.0, 0.0}, {0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}}};

using namespace Detail;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MonoclinicOps::MonoclinicOps() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MonoclinicOps::~MonoclinicOps() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MonoclinicOps::getHasInversion() const
{
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MonoclinicOps::getODFSize() const
{
  return k_OdfSize;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MonoclinicOps::getMDFSize() const
{
  return k_MdfSize;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MonoclinicOps::getNumSymOps() const
{
  return k_NumSymQuats;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MonoclinicOps::getSymmetryName() const
{
  return "Monoclinic 2/m";;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double MonoclinicOps::getMisoQuat(QuatType& q1, QuatType& q2, double& n1, double& n2, double& n3) const
{
  return _calcMisoQuat(MonoclinicQuatSym, k_NumSymQuats, q1, q2, n1, n2, n3);
}

// -----------------------------------------------------------------------------
float MonoclinicOps::getMisoQuat(QuatF& q1f, QuatF& q2f, float& n1f, float& n2f, float& n3f) const
{
  QuatType q1(q1f[0], q1f[1], q1f[2], q1f[3]);
  QuatType q2(q2f[0], q2f[1], q2f[2], q2f[3]);
  double n1 = n1f;
  double n2 = n2f;
  double n3 = n3f;
  float w = static_cast<float>(_calcMisoQuat(MonoclinicQuatSym, k_NumSymQuats, q1, q2, n1, n2, n3));
  n1f = n1;
  n2f = n2;
  n3f = n3;
  return w;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double MonoclinicOps::_calcMisoQuat(const QuatType quatsym[24], int numsym, QuatType& q1, QuatType& q2, double& n1, double& n2, double& n3) const
{
  double wmin = 9999999.0f; //,na,nb,nc;
  double w = 0.0;
  double n1min = 0.0f;
  double n2min = 0.0f;
  double n3min = 0.0f;
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
    n1 = 0.0, n2 = 0.0, n3 = 1.0;
  }
  if(wmin == 0)
  {
    n1 = 0.0, n2 = 0.0, n3 = 1.0;
  }
  return wmin;
}

QuatType MonoclinicOps::getQuatSymOp(int32_t i) const
{
  return MonoclinicQuatSym[i];
}

void MonoclinicOps::getRodSymOp(int i, double* r) const
{
  r[0] = MonoclinicRodSym[i][0];
  r[1] = MonoclinicRodSym[i][1];
  r[2] = MonoclinicRodSym[i][2];
}

void MonoclinicOps::getMatSymOp(int i, double g[3][3]) const
{
  g[0][0] = MonoclinicMatSym[i][0][0];
  g[0][1] = MonoclinicMatSym[i][0][1];
  g[0][2] = MonoclinicMatSym[i][0][2];
  g[1][0] = MonoclinicMatSym[i][1][0];
  g[1][1] = MonoclinicMatSym[i][1][1];
  g[1][2] = MonoclinicMatSym[i][1][2];
  g[2][0] = MonoclinicMatSym[i][2][0];
  g[2][1] = MonoclinicMatSym[i][2][1];
  g[2][2] = MonoclinicMatSym[i][2][2];
}

void MonoclinicOps::getMatSymOp(int i, float g[3][3]) const
{
  g[0][0] = MonoclinicMatSym[i][0][0];
  g[0][1] = MonoclinicMatSym[i][0][1];
  g[0][2] = MonoclinicMatSym[i][0][2];
  g[1][0] = MonoclinicMatSym[i][1][0];
  g[1][1] = MonoclinicMatSym[i][1][1];
  g[1][2] = MonoclinicMatSym[i][1][2];
  g[2][0] = MonoclinicMatSym[i][2][0];
  g[2][1] = MonoclinicMatSym[i][2][1];
  g[2][2] = MonoclinicMatSym[i][2][2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationType MonoclinicOps::getODFFZRod(const OrientationType& rod) const
{
  int numsym = 2;
  return _calcRodNearestOrigin(MonoclinicRodSym, numsym, rod);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationType MonoclinicOps::getMDFFZRod(const OrientationType& inRod) const
{
  double w = 0.0, n1 = 0.0, n2 = 0.0, n3 = 0.0;
  double FZw = 0.0, FZn1 = 0.0, FZn2 = 0.0, FZn3 = 0.0;

  OrientationType rod = LaueOps::_calcRodNearestOrigin(MonoclinicRodSym, 24, inRod);
  OrientationType ax = OrientationTransformation::ro2ax<OrientationType, OrientationType>(rod);
  n1 = ax[0];
  n2 = ax[1], n3 = ax[2], w = ax[3];

  /// FIXME: Are we missing code for MonoclinicOps MDF FZ Rodrigues calculation?

  return OrientationTransformation::ax2ro<OrientationType, OrientationType>(OrientationType(FZn1, FZn2, FZn3, FZw));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuatType MonoclinicOps::getNearestQuat(const QuatType& q1, const QuatType& q2) const
{
  return _calcNearestQuat(MonoclinicQuatSym, k_NumSymQuats, q1, q2);
}

QuatF MonoclinicOps::getNearestQuat(const QuatF& q1f, const QuatF& q2f) const
{
  QuatType q1(q1f[0], q1f[1], q1f[2], q1f[3]);
  QuatType q2(q2f[0], q2f[1], q2f[2], q2f[3]);
  QuatType temp = _calcNearestQuat(MonoclinicQuatSym, k_NumSymQuats, q1, q2);
  QuatF out(temp.x(), temp.y(), temp.z(), temp.w());
  return out;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MonoclinicOps::getMisoBin(const OrientationType& rod) const
{
  double dim[3];
  double bins[3];
  double step[3];

  OrientationType ho = OrientationTransformation::ro2ho<OrientationType, OrientationType>(rod);

  dim[0] = Detail::MonoclinicDim1InitValue;
  dim[1] = Detail::MonoclinicDim2InitValue;
  dim[2] = Detail::MonoclinicDim3InitValue;
  step[0] = Detail::MonoclinicDim1StepValue;
  step[1] = Detail::MonoclinicDim2StepValue;
  step[2] = Detail::MonoclinicDim3StepValue;
  bins[0] = 72.0f;
  bins[1] = 36.0f;
  bins[2] = 72.0f;

  return _calcMisoBin(dim, bins, step, ho);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationType MonoclinicOps::determineEulerAngles(uint64_t seed, int choose) const
{
  double init[3];
  double step[3];
  int32_t phi[3];
  double h1, h2, h3;

  init[0] = Detail::MonoclinicDim1InitValue;
  init[1] = Detail::MonoclinicDim2InitValue;
  init[2] = Detail::MonoclinicDim3InitValue;
  step[0] = Detail::MonoclinicDim1StepValue;
  step[1] = Detail::MonoclinicDim2StepValue;
  step[2] = Detail::MonoclinicDim3StepValue;
  phi[0] = static_cast<int32_t>(choose % 72);
  phi[1] = static_cast<int32_t>((choose / 72) % 36);
  phi[2] = static_cast<int32_t>(choose / (72 * 36));

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
OrientationType MonoclinicOps::randomizeEulerAngles(const OrientationType& synea) const
{
  size_t symOp = getRandomSymmetryOperatorIndex(k_NumSymQuats);
  QuatType quat = OrientationTransformation::eu2qu<OrientationType, QuatType>(synea);
  QuatType qc = MonoclinicQuatSym[symOp] * quat;
  return OrientationTransformation::qu2eu<QuatType, OrientationType>(qc);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrientationType MonoclinicOps::determineRodriguesVector(uint64_t seed, int choose) const
{
  double init[3];
  double step[3];
  int32_t phi[3];
  double h1, h2, h3;

  init[0] = Detail::MonoclinicDim1InitValue;
  init[1] = Detail::MonoclinicDim2InitValue;
  init[2] = Detail::MonoclinicDim3InitValue;
  step[0] = Detail::MonoclinicDim1StepValue;
  step[1] = Detail::MonoclinicDim2StepValue;
  step[2] = Detail::MonoclinicDim3StepValue;
  phi[0] = static_cast<int32_t>(choose % 72);
  phi[1] = static_cast<int32_t>((choose / 72) % 36);
  phi[2] = static_cast<int32_t>(choose / (72 * 36));

  _calcDetermineHomochoricValues(seed, init, step, phi, choose, h1, h2, h3);
  OrientationType ho(h1, h2, h3);
  OrientationType ro = OrientationTransformation::ho2ro<OrientationType, OrientationType>(ho);
  ro = getMDFFZRod(ro);
  return ro;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int MonoclinicOps::getOdfBin(const OrientationType& rod) const
{
  double dim[3];
  double bins[3];
  double step[3];

  OrientationType ho = OrientationTransformation::ro2ho<OrientationType, OrientationType>(rod);

  dim[0] = Detail::MonoclinicDim1InitValue;
  dim[1] = Detail::MonoclinicDim2InitValue;
  dim[2] = Detail::MonoclinicDim3InitValue;
  step[0] = Detail::MonoclinicDim1StepValue;
  step[1] = Detail::MonoclinicDim2StepValue;
  step[2] = Detail::MonoclinicDim3StepValue;
  bins[0] = 72.0f;
  bins[1] = 36.0f;
  bins[2] = 72.0f;

  return _calcODFBin(dim, bins, step, ho);
}

void MonoclinicOps::getSchmidFactorAndSS(double load[3], double& schmidfactor, double angleComps[2], int& slipsys) const
{
  schmidfactor = 0;
  slipsys = 0;
}

void MonoclinicOps::getSchmidFactorAndSS(double load[3], double plane[3], double direction[3], double& schmidfactor, double angleComps[2], int& slipsys) const
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
    slipPlane[2] = MonoclinicMatSym[i][2][0] * plane[0] + MonoclinicMatSym[i][2][1] * plane[1] + MonoclinicMatSym[i][2][2] * plane[2];

    //dont consider negative z planes (to avoid duplicates)
    if( slipPlane[2] >= 0)
    {
      slipPlane[0] = MonoclinicMatSym[i][0][0] * plane[0] + MonoclinicMatSym[i][0][1] * plane[1] + MonoclinicMatSym[i][0][2] * plane[2];
      slipPlane[1] = MonoclinicMatSym[i][1][0] * plane[0] + MonoclinicMatSym[i][1][1] * plane[1] + MonoclinicMatSym[i][1][2] * plane[2];

      double slipDirection[3] = {0};
      slipDirection[0] = MonoclinicMatSym[i][0][0] * direction[0] + MonoclinicMatSym[i][0][1] * direction[1] + MonoclinicMatSym[i][0][2] * direction[2];
      slipDirection[1] = MonoclinicMatSym[i][1][0] * direction[0] + MonoclinicMatSym[i][1][1] * direction[1] + MonoclinicMatSym[i][1][2] * direction[2];
      slipDirection[2] = MonoclinicMatSym[i][2][0] * direction[0] + MonoclinicMatSym[i][2][1] * direction[1] + MonoclinicMatSym[i][2][2] * direction[2];

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

double MonoclinicOps::getmPrime(const QuatType& q1, const QuatType& q2, double LD[3]) const
{
  return 0.0;
}

double MonoclinicOps::getF1(const QuatType& q1, const QuatType& q2, double LD[3], bool maxS) const
{
  return 0.0;
}

double MonoclinicOps::getF1spt(const QuatType& q1, const QuatType& q2, double LD[3], bool maxS) const
{
  return 0.0;
}

double MonoclinicOps::getF7(const QuatType& q1, const QuatType& q2, double LD[3], bool maxS) const
{
  return 0.0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
namespace Detail
{
  namespace Monoclinic
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
            direction[0] = 1.0;
            direction[1] = 0.0;
            direction[2] = 0.0;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz011->getPointer(i * 6));
            MatrixMath::Copy3x1(m_xyz011->getPointer(i * 6), m_xyz011->getPointer(i * 6 + 3));
            MatrixMath::Multiply3x1withConstant(m_xyz011->getPointer(i * 6 + 3), -1.0f);

            // -----------------------------------------------------------------------------
            // 111 Family
            direction[0] = 0.0;
            direction[1] = 1.0;
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
void MonoclinicOps::generateSphereCoordsFromEulers(FloatArrayType* eulers, FloatArrayType* xyz001, FloatArrayType* xyz011, FloatArrayType* xyz111) const
{
  size_t nOrientations = eulers->getNumberOfTuples();


  // Sanity Check the size of the arrays
  if (xyz001->getNumberOfTuples() < nOrientations * Detail::Monoclinic::symSize0)
  {
    xyz001->resizeTuples(nOrientations * Detail::Monoclinic::symSize0 * 3);
  }
  if (xyz011->getNumberOfTuples() < nOrientations * Detail::Monoclinic::symSize1)
  {
    xyz011->resizeTuples(nOrientations * Detail::Monoclinic::symSize1 * 3);
  }
  if (xyz111->getNumberOfTuples() < nOrientations * Detail::Monoclinic::symSize2)
  {
    xyz111->resizeTuples(nOrientations * Detail::Monoclinic::symSize2 * 3);
  }

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef SIMPL_USE_PARALLEL_ALGORITHMS
  if(doParallel)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, nOrientations),
                      Detail::Monoclinic::GenerateSphereCoordsImpl(eulers, xyz001, xyz011, xyz111), tbb::auto_partitioner());
  }
  else
#endif
  {
    Detail::Monoclinic::GenerateSphereCoordsImpl serial(eulers, xyz001, xyz011, xyz111);
    serial.generate(0, nOrientations);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool MonoclinicOps::inUnitTriangle(double eta, double chi) const
{
  return !(eta < 0 || eta > (180.0 * SIMPLib::Constants::k_PiOver180) || chi < 0 || chi > (90.0 * SIMPLib::Constants::k_PiOver180));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPL::Rgb MonoclinicOps::generateIPFColor(double* eulers, double* refDir, bool convertDegrees) const
{
  return generateIPFColor(eulers[0], eulers[1], eulers[2], refDir[0], refDir[1], refDir[2], convertDegrees);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPL::Rgb MonoclinicOps::generateIPFColor(double phi1, double phi, double phi2, double refDir0, double refDir1, double refDir2, bool degToRad) const
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
  double chi = 0.0f, eta = 0.0f;
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
  double etaMax = 180.0;
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
SIMPL::Rgb MonoclinicOps::generateRodriguesColor(double r1, double r2, double r3) const
{
  double range1 = 2.0f * MonoclinicDim1InitValue;
  double range2 = 2.0f * MonoclinicDim2InitValue;
  double range3 = 2.0f * MonoclinicDim3InitValue;
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
QVector<UInt8ArrayType::Pointer> MonoclinicOps::generatePoleFigure(PoleFigureConfiguration_t& config) const
{
  QString label0 = QString("<001>");
  QString label1 = QString("<100>");
  QString label2 = QString("<010>");
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
  FloatArrayType::Pointer xyz001 = FloatArrayType::CreateArray(numOrientations * Detail::Monoclinic::symSize0, dims, label0 + QString("xyzCoords"), true);
  // this is size for CUBIC ONLY, <011> Family
  FloatArrayType::Pointer xyz011 = FloatArrayType::CreateArray(numOrientations * Detail::Monoclinic::symSize1, dims, label1 + QString("xyzCoords"), true);
  // this is size for CUBIC ONLY, <111> Family
  FloatArrayType::Pointer xyz111 = FloatArrayType::CreateArray(numOrientations * Detail::Monoclinic::symSize2, dims, label2 + QString("xyzCoords"), true);

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
UInt8ArrayType::Pointer MonoclinicOps::generateIPFTriangleLegend(int imageDim) const
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

      x = -1.0f + 2.0f * xIndex * xInc;
      y = 2.0f * yIndex * yInc;

      double sumSquares = (x * x) + (y * y);
      if(sumSquares > 1.0) // Outside unit circle
      {
        color = 0xFFFFFFFF;
      }
      else if ( sumSquares > (rad-2*xInc) && sumSquares < (rad+2*xInc)) // Black Border line
      {
        color = 0xFF000000;
      }

      else if (xIndex == 0 ) // Black Border line
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
SIMPL::Rgb MonoclinicOps::generateMisorientationColor(const QuatType& q, const QuatType& refFrame) const
{
  Q_ASSERT(false);
  return RgbColor::dRgb(0, 0, 0, 0);
}

// -----------------------------------------------------------------------------
MonoclinicOps::Pointer MonoclinicOps::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
QString MonoclinicOps::getNameOfClass() const
{
  return QString("MonoclinicOps");
}

// -----------------------------------------------------------------------------
QString MonoclinicOps::ClassName()
{
  return QString("MonoclinicOps");
}

// -----------------------------------------------------------------------------
MonoclinicOps::Pointer MonoclinicOps::New()
{
  Pointer sharedPtr(new(MonoclinicOps));
  return sharedPtr;
}

