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

#include "CubicLowOps.h"

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
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

#include "OrientationLib/OrientationMath/OrientationMath.h"
#include "OrientationLib/OrientationMath/OrientationArray.hpp"
#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"
#include "OrientationLib/Utilities/ModifiedLambertProjection.h"


namespace Detail
{
  static const float CubicLowDim1InitValue = powf((0.75f * ((SIMPLib::Constants::k_Pi / 2.0f) - sinf((SIMPLib::Constants::k_Pi / 2.0f)))), (1.0f / 3.0f));
  static const float CubicLowDim2InitValue = powf((0.75f * ((SIMPLib::Constants::k_Pi / 2.0f) - sinf((SIMPLib::Constants::k_Pi / 2.0f)))), (1.0f / 3.0f));
  static const float CubicLowDim3InitValue = powf((0.75f * ((SIMPLib::Constants::k_Pi / 2.0f) - sinf((SIMPLib::Constants::k_Pi / 2.0f)))), (1.0f / 3.0f));
  static const float CubicLowDim1StepValue = CubicLowDim1InitValue / 18.0f;
  static const float CubicLowDim2StepValue = CubicLowDim2InitValue / 18.0f;
  static const float CubicLowDim3StepValue = CubicLowDim3InitValue / 18.0f;
  namespace CubicLow
  {
    static const int symSize0 = 6;
    static const int symSize1 = 12;
    static const int symSize2 = 8;
  }
}

static const QuatF CubicLowQuatSym[12] =
{
  QuaternionMathF::New(0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f),
  QuaternionMathF::New(1.000000000f, 0.000000000f, 0.000000000f, 0.000000000f),
  QuaternionMathF::New(0.000000000f, 1.000000000f, 0.000000000f, 0.000000000f),
  QuaternionMathF::New(0.000000000f, 0.000000000f, 1.000000000f, 0.000000000f),
  QuaternionMathF::New(0.500000000f, 0.500000000f, 0.500000000f, 0.500000000f),
  QuaternionMathF::New(-0.500000000f, -0.500000000f, -0.500000000f, 0.500000000f),
  QuaternionMathF::New(0.500000000f, -0.500000000f, 0.500000000f, 0.500000000f),
  QuaternionMathF::New(-0.500000000f, 0.500000000f, -0.500000000f, 0.500000000f),
  QuaternionMathF::New(-0.500000000f, 0.500000000f, 0.500000000f, 0.500000000f),
  QuaternionMathF::New(0.500000000f, -0.500000000f, -0.500000000f, 0.500000000f),
  QuaternionMathF::New(-0.500000000f, -0.500000000f, 0.500000000f, 0.500000000f),
  QuaternionMathF::New(0.500000000f, 0.500000000f, -0.500000000f, 0.500000000f)
};


static const float CubicLowRodSym[12][3] = {{0.0f, 0.0f, 0.0f},
  {10000000000.0f, 0.0f, 0.0f},
  {0.0f, 10000000000.0f, 0.0f},
  {0.0f, 0.0f, 10000000000.0f},
  {1.0f, 1.0f, 1.0f},
  { -1.0f, -1.0f, -1.0f},
  {1.0f, -1.0f, 1.0f},
  { -1.0f, 1.0f, -1.0f},
  { -1.0f, 1.0f, 1.0f},
  {1.0f, -1.0f, -1.0f},
  { -1.0f, -1.0f, 1.0f},
  {1.0f, 1.0f, -1.0}
};

//static const float CubicLowSlipDirections[12][3] = {{0.0f, 1.0f, -1.0f},
//  {1.0f, 0.0f, -1.0f},
//  {1.0f, -1.0f, 0.0f},
//  {1.0f, -1.0f, 0.0f},
//  {1.0f, 0.0f, 1.0f},
//  {0.0f, 1.0f, 1.0f},
//  {1.0f, 1.0f, 0.0f},
//  {0.0f, 1.0f, 1.0f},
//  {1.0f, 0.0f, -1.0f},
//  {1.0f, 1.0f, 0.0f},
//  {1.0f, 0.0f, 1.0f},
//  {0.0f, 1.0f, -1.0f}
//};
//
//static const float CubicLowSlipPlanes[12][3] = {{1.0f, 1.0f, 1.0f},
//  {1.0f, 1.0f, 1.0f},
//  {1.0f, 1.0f, 1.0f},
//  {1.0f, 1.0f, -1.0f},
//  {1.0f, 1.0f, -1.0f},
//  {1.0f, 1.0f, -1.0f},
//  {1.0f, -1.0f, 1.0f},
//  {1.0f, -1.0f, 1.0f},
//  {1.0f, -1.0f, 1.0f},
//  { -1.0f, 1.0f, 1.0f},
//  { -1.0f, 1.0f, 1.0f},
//  { -1.0f, 1.0f, 1.0f}
//};

static const float CubicLowMatSym[12][3][3] =
{
  { {1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 0.0, 1.0}
  },

  { {1.0,  0.0,  0.0},
    {0.0, -1.0,  0.0},
    {0.0,  0.0, -1.0}
  },

  { { -1.0, 0.0,  0.0},
    {0.0, 1.0,  0.0},
    {0.0, 0.0, -1.0}
  },

  { { -1.0,  0.0, 0.0},
    {0.0, -1.0, 0.0},
    {0.0,  0.0, 1.0}
  },

  { {0.0, -1.0, 0.0},
    {0.0,  0.0, 1.0},
    { -1.0,  0.0, 0.0}
  },

  { {0.0,  0.0, 1.0},
    { -1.0,  0.0, 0.0},
    {0.0, -1.0, 0.0}
  },

  { {0.0, -1.0,  0.0},
    {0.0,  0.0, -1.0},
    {1.0,  0.0,  0.0}
  },

  { {0.0,  0.0, -1.0},
    {1.0,  0.0,  0.0},
    {0.0, -1.0,  0.0}
  },

  { {0.0, 1.0,  0.0},
    {0.0, 0.0, -1.0},
    { -1.0, 0.0,  0.0}
  },

  { {0.0, 0.0, -1.0},
    { -1.0, 0.0,  0.0},
    {0.0, 1.0,  0.0}
  },

  { {0.0, 1.0, 0.0},
    {0.0, 0.0, 1.0},
    {1.0, 0.0, 0.0}
  },

  { {0.0, 0.0, 1.0},
    {1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0}
  }
};

using namespace Detail;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CubicLowOps::CubicLowOps()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CubicLowOps::~CubicLowOps()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float CubicLowOps::getMisoQuat(QuatF& q1, QuatF& q2, float& n1, float& n2, float& n3)
{

  int numsym = 12;

  return _calcMisoQuat(CubicLowQuatSym, numsym, q1, q2, n1, n2, n3);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float CubicLowOps::_calcMisoQuat(const QuatF quatsym[24], int numsym,
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

void CubicLowOps::getQuatSymOp(int i, QuatF& q)
{
  QuaternionMathF::Copy(CubicLowQuatSym[i], q);
}

void CubicLowOps::getRodSymOp(int i, float* r)
{
  r[0] = CubicLowRodSym[i][0];
  r[1] = CubicLowRodSym[i][1];
  r[2] = CubicLowRodSym[i][2];
}

void CubicLowOps::getMatSymOp(int i, float g[3][3])
{
  g[0][0] = CubicLowMatSym[i][0][0];
  g[0][1] = CubicLowMatSym[i][0][1];
  g[0][2] = CubicLowMatSym[i][0][2];
  g[1][0] = CubicLowMatSym[i][1][0];
  g[1][1] = CubicLowMatSym[i][1][1];
  g[1][2] = CubicLowMatSym[i][1][2];
  g[2][0] = CubicLowMatSym[i][2][0];
  g[2][1] = CubicLowMatSym[i][2][1];
  g[2][2] = CubicLowMatSym[i][2][2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FOrientArrayType CubicLowOps::getODFFZRod(FOrientArrayType rod)
{
  int numsym = 12;
  return  _calcRodNearestOrigin(CubicLowRodSym, numsym, rod);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FOrientArrayType CubicLowOps::getMDFFZRod(FOrientArrayType rod)
{
  float w, n1, n2, n3;
  float FZw, FZn1, FZn2, FZn3;

  rod = _calcRodNearestOrigin(CubicLowRodSym, 12, rod);
  FOrientArrayType ax(4, 0.0f);
  OrientationTransforms<FOrientArrayType, float>::ro2ax(rod, ax);

  n1 = ax[0];
  n2 = ax[1], n3 = ax[2], w = ax[3];

  FZw = w;
  n1 = fabs(n1);
  n2 = fabs(n2);
  n3 = fabs(n3);
  if(n1 > n2)
  {
    if(n1 > n3)
    {
      FZn1 = n1;
      if (n2 > n3)
      {
        FZn2 = n2, FZn3 = n3;
      }
      else
      {
        FZn2 = n3, FZn3 = n2;
      }
    }
    else
    {
      FZn1 = n3, FZn2 = n1, FZn3 = n2;
    }
  }
  else
  {
    if(n2 > n3)
    {
      FZn1 = n2;
      if (n1 > n3)
      {
        FZn2 = n1, FZn3 = n3;
      }
      else
      {
        FZn2 = n3, FZn3 = n1;
      }
    }
    else
    {
      FZn1 = n3, FZn2 = n2, FZn3 = n1;
    }
  }

  ax.fromAxisAngle(FZn1, FZn2, FZn3, FZw);
  OrientationTransforms<FOrientArrayType, float>::ax2ro(ax, rod);
  return rod;
}

void CubicLowOps::getNearestQuat(QuatF& q1, QuatF& q2)
{
  int numsym = 12;

  _calcNearestQuat(CubicLowQuatSym, numsym, q1, q2);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CubicLowOps::getMisoBin(FOrientArrayType rod)
{
  float dim[3];
  float bins[3];
  float step[3];

  FOrientArrayType ho(3);
  OrientationTransforms<FOrientArrayType, float>::ro2ho(rod, ho);

  dim[0] = Detail::CubicLowDim1InitValue;
  dim[1] = Detail::CubicLowDim2InitValue;
  dim[2] = Detail::CubicLowDim3InitValue;
  step[0] = Detail::CubicLowDim1StepValue;
  step[1] = Detail::CubicLowDim2StepValue;
  step[2] = Detail::CubicLowDim3StepValue;
  bins[0] = 36.0f;
  bins[1] = 36.0f;
  bins[2] = 36.0f;

  return _calcMisoBin(dim, bins, step, ho);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FOrientArrayType CubicLowOps::determineEulerAngles(uint64_t seed, int choose)
{
  float init[3];
  float step[3];
  int32_t phi[3];
  float h1, h2, h3;;

  init[0] = Detail::CubicLowDim1InitValue;
  init[1] = Detail::CubicLowDim2InitValue;
  init[2] = Detail::CubicLowDim3InitValue;
  step[0] = Detail::CubicLowDim1StepValue;
  step[1] = Detail::CubicLowDim2StepValue;
  step[2] = Detail::CubicLowDim3StepValue;
  phi[0] = static_cast<int32_t>(choose % 36);
  phi[1] = static_cast<int32_t>((choose / 36) % 36);
  phi[2] = static_cast<int32_t>(choose / (36 * 36));

  _calcDetermineHomochoricValues(seed, init, step, phi, choose, h1, h2, h3);

  FOrientArrayType ho(h1, h2, h3);
  FOrientArrayType ro(4);
  OrientationTransforms<FOrientArrayType, float>::ho2ro(ho, ro);

  ro = getODFFZRod(ro);
  FOrientArrayType eu(4);
  OrientationTransforms<FOrientArrayType, float>::ro2eu(ro, eu);
  return eu;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FOrientArrayType CubicLowOps::randomizeEulerAngles(FOrientArrayType synea)
{
  QuatF q;
  QuatF qc;
  size_t symOp = getRandomSymmetryOperatorIndex(k_NumSymQuats);

  FOrientArrayType quat(4, 0.0f);
  OrientationTransforms<FOrientArrayType, float>::eu2qu(synea, quat);
  q = quat.toQuaternion();
  QuaternionMathF::Multiply(CubicLowQuatSym[symOp], q, qc);

  quat.fromQuaternion(qc);
  OrientationTransforms<FOrientArrayType, float>::qu2eu(quat, synea);
  return synea;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FOrientArrayType CubicLowOps::determineRodriguesVector(uint64_t seed, int choose)
{
  float init[3];
  float step[3];
  int32_t phi[3];
  float h1, h2, h3;

  init[0] = Detail::CubicLowDim1InitValue;
  init[1] = Detail::CubicLowDim2InitValue;
  init[2] = Detail::CubicLowDim3InitValue;
  step[0] = Detail::CubicLowDim1StepValue;
  step[1] = Detail::CubicLowDim2StepValue;
  step[2] = Detail::CubicLowDim3StepValue;
  phi[0] = static_cast<int32_t>(choose % 36);
  phi[1] = static_cast<int32_t>((choose / 36) % 36);
  phi[2] = static_cast<int32_t>(choose / (36 * 36));

  _calcDetermineHomochoricValues(seed, init, step, phi, choose, h1, h2, h3);
  FOrientArrayType ho(h1, h2, h3);
  FOrientArrayType ro(4);
  OrientationTransforms<FOrientArrayType, float>::ho2ro(ho, ro);
  ro = getMDFFZRod(ro);
  return ro;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CubicLowOps::getOdfBin(FOrientArrayType rod)
{
  float dim[3];
  float bins[3];
  float step[3];

  FOrientArrayType ho(3);
  OrientationTransforms<FOrientArrayType, float>::ro2ho(rod, ho);

  dim[0] = Detail::CubicLowDim1InitValue;
  dim[1] = Detail::CubicLowDim2InitValue;
  dim[2] = Detail::CubicLowDim3InitValue;
  step[0] = Detail::CubicLowDim1StepValue;
  step[1] = Detail::CubicLowDim2StepValue;
  step[2] = Detail::CubicLowDim3StepValue;
  bins[0] = 36.0f;
  bins[1] = 36.0f;
  bins[2] = 36.0f;

  return _calcODFBin(dim, bins, step, ho);
}

void CubicLowOps::getSchmidFactorAndSS(float load[3], float& schmidfactor, float angleComps[2], int& slipsys)
{
  schmidfactor = 0;
  slipsys = 0;
  angleComps[0] = 0;
  angleComps[1] = 0;
}

void CubicLowOps::getSchmidFactorAndSS(float load[3], float plane[3], float direction[3], float& schmidfactor, float angleComps[2], int& slipsys)
{
  schmidfactor = 0;
  slipsys = 0;
  angleComps[0] = 0;
  angleComps[1] = 0;

  //compute mags
  float loadMag = sqrt( load[0] * load[0] + load[1] * load[1] + load[2] * load[2] );
  float planeMag = sqrt( plane[0] * plane[0] + plane[1] * plane[1] + plane[2] * plane[2] );
  float directionMag = sqrt( direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2] );
  planeMag *= loadMag;
  directionMag *= loadMag;

  //loop over symmetry operators finding highest schmid factor
  for(int i = 0; i < k_NumSymQuats; i++)
  {
    //compute slip system
    float slipPlane[3] = {0};
    slipPlane[2] = CubicLowMatSym[i][2][0] * plane[0] + CubicLowMatSym[i][2][1] * plane[1] + CubicLowMatSym[i][2][2] * plane[2];

    //dont consider negative z planes (to avoid duplicates)
    if( slipPlane[2] >= 0)
    {
      slipPlane[0] = CubicLowMatSym[i][0][0] * plane[0] + CubicLowMatSym[i][0][1] * plane[1] + CubicLowMatSym[i][0][2] * plane[2];
      slipPlane[1] = CubicLowMatSym[i][1][0] * plane[0] + CubicLowMatSym[i][1][1] * plane[1] + CubicLowMatSym[i][1][2] * plane[2];

      float slipDirection[3] = {0};
      slipDirection[0] = CubicLowMatSym[i][0][0] * direction[0] + CubicLowMatSym[i][0][1] * direction[1] + CubicLowMatSym[i][0][2] * direction[2];
      slipDirection[1] = CubicLowMatSym[i][1][0] * direction[0] + CubicLowMatSym[i][1][1] * direction[1] + CubicLowMatSym[i][1][2] * direction[2];
      slipDirection[2] = CubicLowMatSym[i][2][0] * direction[0] + CubicLowMatSym[i][2][1] * direction[1] + CubicLowMatSym[i][2][2] * direction[2];

      float cosPhi = fabs( load[0] * slipPlane[0] + load[1] * slipPlane[1] + load[2] * slipPlane[2] ) / planeMag;
      float cosLambda = fabs( load[0] * slipDirection[0] + load[1] * slipDirection[1] + load[2] * slipDirection[2] ) / directionMag;

      float schmid = cosPhi * cosLambda;
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

void CubicLowOps::getmPrime(QuatF& q1, QuatF& q2, float LD[3], float& mPrime)
{
  mPrime = 0;
}

void CubicLowOps::getF1(QuatF& q1, QuatF& q2, float LD[3], bool maxSF, float& F1)
{
  F1 = 0;
}

void CubicLowOps::getF1spt(QuatF& q1, QuatF& q2, float LD[3], bool maxSF, float& F1spt)
{
  F1spt = 0;
}

void CubicLowOps::getF7(QuatF& q1, QuatF& q2, float LD[3], bool maxSF, float& F7)
{
  F7 = 0;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
namespace Detail
{
  namespace CubicLow
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
        virtual ~GenerateSphereCoordsImpl() {}

        void generate(size_t start, size_t end) const
        {
          float g[3][3];
          float gTranpose[3][3];
          float direction[3] = {0.0, 0.0, 0.0};

          for(size_t i = start; i < end; ++i)
          {
            FOrientArrayType eu(m_Eulers->getPointer(i * 3), 3);
            FOrientArrayType om(9, 0.0);
            OrientationTransforms<FOrientArrayType, float>::eu2om(eu, om);
            om.toGMatrix(g);
            MatrixMath::Transpose3x3(g, gTranpose);

            // -----------------------------------------------------------------------------
            // 001 Family
            direction[0] = 1.0;
            direction[1] = 0.0;
            direction[2] = 0.0;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz001->getPointer(i * 18));
            MatrixMath::Copy3x1(m_xyz001->getPointer(i * 18), m_xyz001->getPointer(i * 18 + 3));
            MatrixMath::Multiply3x1withConstant(m_xyz001->getPointer(i * 18 + 3), -1);
            direction[0] = 0.0;
            direction[1] = 1.0;
            direction[2] = 0.0;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz001->getPointer(i * 18 + 6));
            MatrixMath::Copy3x1(m_xyz001->getPointer(i * 18 + 6), m_xyz001->getPointer(i * 18 + 9));
            MatrixMath::Multiply3x1withConstant(m_xyz001->getPointer(i * 18 + 9), -1);
            direction[0] = 0.0;
            direction[1] = 0.0;
            direction[2] = 1.0;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz001->getPointer(i * 18 + 12));
            MatrixMath::Copy3x1(m_xyz001->getPointer(i * 18 + 12), m_xyz001->getPointer(i * 18 + 15));
            MatrixMath::Multiply3x1withConstant(m_xyz001->getPointer(i * 18 + 15), -1);

            // -----------------------------------------------------------------------------
            // 011 Family
            direction[0] = SIMPLib::Constants::k_1OverRoot2;
            direction[1] = SIMPLib::Constants::k_1OverRoot2;
            direction[2] = 0.0;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz011->getPointer(i * 36));
            MatrixMath::Copy3x1(m_xyz011->getPointer(i * 36), m_xyz011->getPointer(i * 36 + 3));
            MatrixMath::Multiply3x1withConstant(m_xyz011->getPointer(i * 36 + 3), -1);
            direction[0] = SIMPLib::Constants::k_1OverRoot2;
            direction[1] = 0.0;
            direction[2] = SIMPLib::Constants::k_1OverRoot2;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz011->getPointer(i * 36 + 6));
            MatrixMath::Copy3x1(m_xyz011->getPointer(i * 36 + 6), m_xyz011->getPointer(i * 36 + 9));
            MatrixMath::Multiply3x1withConstant(m_xyz011->getPointer(i * 36 + 9), -1);
            direction[0] = 0.0;
            direction[1] = SIMPLib::Constants::k_1OverRoot2;
            direction[2] = SIMPLib::Constants::k_1OverRoot2;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz011->getPointer(i * 36 + 12));
            MatrixMath::Copy3x1(m_xyz011->getPointer(i * 36 + 12), m_xyz011->getPointer(i * 36 + 15));
            MatrixMath::Multiply3x1withConstant(m_xyz011->getPointer(i * 36 + 15), -1);
            direction[0] = -SIMPLib::Constants::k_1OverRoot2;
            direction[1] = -SIMPLib::Constants::k_1OverRoot2;
            direction[2] = 0.0;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz011->getPointer(i * 36 + 18));
            MatrixMath::Copy3x1(m_xyz011->getPointer(i * 36 + 18), m_xyz011->getPointer(i * 36 + 21));
            MatrixMath::Multiply3x1withConstant(m_xyz011->getPointer(i * 36 + 21), -1);
            direction[0] = -SIMPLib::Constants::k_1OverRoot2;
            direction[1] = 0.0;
            direction[2] = SIMPLib::Constants::k_1OverRoot2;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz011->getPointer(i * 36 + 24));
            MatrixMath::Copy3x1(m_xyz011->getPointer(i * 36 + 24), m_xyz011->getPointer(i * 36 + 27));
            MatrixMath::Multiply3x1withConstant(m_xyz011->getPointer(i * 36 + 27), -1);
            direction[0] = 0.0;
            direction[1] = -SIMPLib::Constants::k_1OverRoot2;
            direction[2] = SIMPLib::Constants::k_1OverRoot2;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz011->getPointer(i * 36 + 30));
            MatrixMath::Copy3x1(m_xyz011->getPointer(i * 36 + 30), m_xyz011->getPointer(i * 36 + 33));
            MatrixMath::Multiply3x1withConstant(m_xyz011->getPointer(i * 36 + 33), -1);

            // -----------------------------------------------------------------------------
            // 111 Family
            direction[0] = SIMPLib::Constants::k_1OverRoot3;
            direction[1] = SIMPLib::Constants::k_1OverRoot3;
            direction[2] = SIMPLib::Constants::k_1OverRoot3;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz111->getPointer(i * 24));
            MatrixMath::Copy3x1(m_xyz111->getPointer(i * 24), m_xyz111->getPointer(i * 24 + 3));
            MatrixMath::Multiply3x1withConstant(m_xyz111->getPointer(i * 24 + 3), -1);
            direction[0] = -SIMPLib::Constants::k_1OverRoot3;
            direction[1] = SIMPLib::Constants::k_1OverRoot3;
            direction[2] = SIMPLib::Constants::k_1OverRoot3;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz111->getPointer(i * 24 + 6));
            MatrixMath::Copy3x1(m_xyz111->getPointer(i * 24 + 6), m_xyz111->getPointer(i * 24 + 9));
            MatrixMath::Multiply3x1withConstant(m_xyz111->getPointer(i * 24 + 9), -1);
            direction[0] = SIMPLib::Constants::k_1OverRoot3;
            direction[1] = -SIMPLib::Constants::k_1OverRoot3;
            direction[2] = SIMPLib::Constants::k_1OverRoot3;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz111->getPointer(i * 24 + 12));
            MatrixMath::Copy3x1(m_xyz111->getPointer(i * 24 + 12), m_xyz111->getPointer(i * 24 + 15));
            MatrixMath::Multiply3x1withConstant(m_xyz111->getPointer(i * 24 + 15), -1);
            direction[0] = SIMPLib::Constants::k_1OverRoot3;
            direction[1] = SIMPLib::Constants::k_1OverRoot3;
            direction[2] = -SIMPLib::Constants::k_1OverRoot3;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz111->getPointer(i * 24 + 18));
            MatrixMath::Copy3x1(m_xyz111->getPointer(i * 24 + 18), m_xyz111->getPointer(i * 24 + 21));
            MatrixMath::Multiply3x1withConstant(m_xyz111->getPointer(i * 24 + 21), -1);
          }

        }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
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
void CubicLowOps::generateSphereCoordsFromEulers(FloatArrayType* eulers, FloatArrayType* xyz001, FloatArrayType* xyz011, FloatArrayType* xyz111)
{
  size_t nOrientations = eulers->getNumberOfTuples();


  // Sanity Check the size of the arrays
  if (xyz001->getNumberOfTuples() < nOrientations * Detail::CubicLow::symSize0)
  {
    xyz001->resize(nOrientations * Detail::CubicLow::symSize0 * 3);
  }
  if (xyz011->getNumberOfTuples() < nOrientations * Detail::CubicLow::symSize1)
  {
    xyz011->resize(nOrientations * Detail::CubicLow::symSize1 * 3);
  }
  if (xyz111->getNumberOfTuples() < nOrientations * Detail::CubicLow::symSize2)
  {
    xyz111->resize(nOrientations * Detail::CubicLow::symSize2 * 3);
  }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, nOrientations),
                      Detail::CubicLow::GenerateSphereCoordsImpl(eulers, xyz001, xyz011, xyz111), tbb::auto_partitioner());
  }
  else
#endif
  {
    Detail::CubicLow::GenerateSphereCoordsImpl serial(eulers, xyz001, xyz011, xyz111);
    serial.generate(0, nOrientations);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool CubicLowOps::inUnitTriangle(float eta, float chi)
{
  float etaDeg = eta * SIMPLib::Constants::k_180OverPi;
  float chiMax;
  if(etaDeg > 45.0)
  {
    chiMax = sqrt(1.0 / (2.0 + tanf(0.5 * SIMPLib::Constants::k_Pi - eta) * tanf(0.5 * SIMPLib::Constants::k_Pi - eta)));
  }
  else
  {
    chiMax = sqrt(1.0 / (2.0 + tanf(eta) * tanf(eta)));
  }
  SIMPLibMath::boundF(chiMax, -1.0f, 1.0f);
  chiMax = acos(chiMax);
  if( eta < 0.0 || eta > (90.0 * SIMPLib::Constants::k_PiOver180) || chi < 0.0 || chi > chiMax )
  {
    return false;
  }
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3D::Rgb CubicLowOps::generateIPFColor(double* eulers, double* refDir, bool convertDegrees)
{
  return generateIPFColor(eulers[0], eulers[1], eulers[2], refDir[0], refDir[1], refDir[2], convertDegrees);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3D::Rgb CubicLowOps::generateIPFColor(double phi1, double phi, double phi2, double refDir0, double refDir1, double refDir2, bool degToRad)
{
  if (degToRad == true)
  {
    phi1 = phi1 * SIMPLib::Constants::k_DegToRad;
    phi = phi * SIMPLib::Constants::k_DegToRad;
    phi2 = phi2 * SIMPLib::Constants::k_DegToRad;
  }
  QuatF qc;
  QuatF q1;
  float g[3][3];
  float p[3];
  float refDirection[3];
  float eta, chi;
  float _rgb[3] = { 0.0, 0.0, 0.0 };

  // 1) find rotation matrix from Euler angles
  FOrientArrayType eu(phi1, phi, phi2);
  FOrientArrayType qu(4);
  FOrientArrayType om(9); // Reusable for the loop
  OrientationTransforms<FOrientArrayType, float>::eu2qu(eu, qu);
  q1 = qu.toQuaternion();

  QuatF q2;
  CubicLowOps co;
  for (int j = 0; j < 12; j++)
  {
    co.getQuatSymOp(j, q2);
    QuaternionMathF::Multiply(q2, q1, qc);

    qu.fromQuaternion(qc);
    OrientationTransforms<FOrientArrayType, float>::qu2om(qu, om);
    om.toGMatrix(g);

    refDirection[0] = refDir0;
    refDirection[1] = refDir1;
    refDirection[2] = refDir2;
    MatrixMath::Multiply3x3with3x1(g, refDirection, p);
    MatrixMath::Normalize3x1(p);

    if(getHasInversion() == false && p[2] < 0)
    {
      continue;
    }
    else if(getHasInversion() == true && p[2] < 0)
    {
      p[0] = -p[0], p[1] = -p[1], p[2] = -p[2];
    }
    chi = acos(p[2]);
    eta = atan2(p[1], p[0]);
    if(inUnitTriangle(eta, chi) == false)
    {
      continue;
    }
    else
    {
      break;
    }
  }

  float etaMin = 0.0;
  float etaMax = 90.0;
  float etaDeg = eta * SIMPLib::Constants::k_180OverPi;
  float chiMax;
  if(etaDeg > 45.0)
  {
    chiMax = sqrt(1.0 / (2.0 + tanf(0.5 * SIMPLib::Constants::k_Pi - eta) * tanf(0.5 * SIMPLib::Constants::k_Pi - eta)));
  }
  else
  {
    chiMax = sqrt(1.0 / (2.0 + tanf(eta) * tanf(eta)));
  }
  SIMPLibMath::boundF(chiMax, -1.0f, 1.0f);
  chiMax = acos(chiMax);

  _rgb[0] = 1.0 - chi / chiMax;
  _rgb[2] = fabs(etaDeg - etaMin) / (etaMax - etaMin);
  _rgb[1] = 1 - _rgb[2];
  _rgb[1] *= chi / chiMax;
  _rgb[2] *= chi / chiMax;
  _rgb[0] = sqrt(_rgb[0]);
  _rgb[1] = sqrt(_rgb[1]);
  _rgb[2] = sqrt(_rgb[2]);

  return RgbColor::dRgb(_rgb[0] * 255, _rgb[1] * 255, _rgb[2] * 255, 255);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3D::Rgb CubicLowOps::generateRodriguesColor(float r1, float r2, float r3)
{
  float range1 = 2.0f * CubicLowDim1InitValue;
  float range2 = 2.0f * CubicLowDim2InitValue;
  float range3 = 2.0f * CubicLowDim3InitValue;
  float max1 = range1 / 2.0f;
  float max2 = range2 / 2.0f;
  float max3 = range3 / 2.0f;
  float red = (r1 + max1) / range1;
  float green = (r2 + max2) / range2;
  float blue = (r3 + max3) / range3;

  // Scale values from 0 to 1.0
  red = red / max1;
  green = green / max1;
  blue = blue / max2;

  return RgbColor::dRgb(red * 255, green * 255, blue * 255, 255);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<UInt8ArrayType::Pointer> CubicLowOps::generatePoleFigure(PoleFigureConfiguration_t& config)
{
  QString label0("Cubic Low <001>");
  QString label1("Cubic Low <011>");
  QString label2("Cubic Low <111>");
  if(config.labels.size() > 0) { label0 = config.labels.at(0); }
  if(config.labels.size() > 1) { label1 = config.labels.at(1); }
  if(config.labels.size() > 2) { label2 = config.labels.at(2); }

  int numOrientations = config.eulers->getNumberOfTuples();

  // Create an Array to hold the XYZ Coordinates which are the coords on the sphere.
  // this is size for CUBIC ONLY, <001> Family
  QVector<size_t> dims(1, 3);
  FloatArrayType::Pointer xyz001 = FloatArrayType::CreateArray(numOrientations * Detail::CubicLow::symSize0, dims, label0 + QString("xyzCoords"));
  // this is size for CUBIC ONLY, <011> Family
  FloatArrayType::Pointer xyz011 = FloatArrayType::CreateArray(numOrientations * Detail::CubicLow::symSize1, dims, label1 + QString("xyzCoords"));
  // this is size for CUBIC ONLY, <111> Family
  FloatArrayType::Pointer xyz111 = FloatArrayType::CreateArray(numOrientations * Detail::CubicLow::symSize2, dims, label2 + QString("xyzCoords"));

  config.sphereRadius = 1.0f;

  // Generate the coords on the sphere **** Parallelized
  generateSphereCoordsFromEulers(config.eulers, xyz001.get(), xyz011.get(), xyz111.get());


  // These arrays hold the "intensity" images which eventually get converted to an actual Color RGB image
  // Generate the modified Lambert projection images (Squares, 2 of them, 1 for northern hemisphere, 1 for southern hemisphere
  DoubleArrayType::Pointer intensity001 = DoubleArrayType::CreateArray(config.imageDim * config.imageDim, label0 + "_Intensity_Image");
  DoubleArrayType::Pointer intensity011 = DoubleArrayType::CreateArray(config.imageDim * config.imageDim, label1 + "_Intensity_Image");
  DoubleArrayType::Pointer intensity111 = DoubleArrayType::CreateArray(config.imageDim * config.imageDim, label2 + "_Intensity_Image");
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
  tbb::task_group* g = new tbb::task_group;

  if(doParallel == true)
  {
    g->run(GenerateIntensityMapImpl(xyz001.get(), &config, intensity001.get()));
    g->run(GenerateIntensityMapImpl(xyz011.get(), &config, intensity011.get()));
    g->run(GenerateIntensityMapImpl(xyz111.get(), &config, intensity111.get()));
    g->wait(); // Wait for all the threads to complete before moving on.
    delete g;
    g = NULL;
  }
  else
#endif
  {
    GenerateIntensityMapImpl m001(xyz001.get(), &config, intensity001.get());
    m001();
    GenerateIntensityMapImpl m011(xyz011.get(), &config, intensity011.get());
    m011();
    GenerateIntensityMapImpl m111(xyz111.get(), &config, intensity111.get());
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
  UInt8ArrayType::Pointer image001 = UInt8ArrayType::CreateArray(config.imageDim * config.imageDim, dims, label0);
  UInt8ArrayType::Pointer image011 = UInt8ArrayType::CreateArray(config.imageDim * config.imageDim, dims, label1);
  UInt8ArrayType::Pointer image111 = UInt8ArrayType::CreateArray(config.imageDim * config.imageDim, dims, label2);

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

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS

  if(doParallel == true)
  {
    g = new tbb::task_group;
    g->run(GeneratePoleFigureRgbaImageImpl(intensity001.get(), &config, image001.get()));
    g->run(GeneratePoleFigureRgbaImageImpl(intensity011.get(), &config, image011.get()));
    g->run(GeneratePoleFigureRgbaImageImpl(intensity111.get(), &config, image111.get()));
    g->wait(); // Wait for all the threads to complete before moving on.
    delete g;
    g = NULL;
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
DREAM3D::Rgb CubicLowOps::generateMisorientationColor(const QuatF& q, const QuatF& refFrame)
{
  BOOST_ASSERT(false);

  float n1, n2, n3, w;
  float x, x1, x2, x3, x4;
  float y, y1, y2, y3, y4;
  float z, z1, z2, z3, z4;
  float k, h, s, v, c, r, g, b;

  QuatF q1, q2;
  QuaternionMathF::Copy(q, q1);
  QuaternionMathF::Copy(refFrame, q2);

  w = getMisoQuat(q1, q2, n1, n2, n3);

  //eq c7.1
  k = tan(w / 2.0f);
  x = n1;
  y = n2;
  z = n3;
  FOrientArrayType rod(x, y, z, k);
  rod = getMDFFZRod(rod);
  x = rod[0];
  y = rod[1];
  z = rod[2];
  k = rod[3];

  //eq c7.2
  k = atan2(y, x);
  if(k < M_PI_4)
  {
    x1 = z;
    y1 = x;
    z1 = y;
  }
  else
  {
    x1 = y;
    y1 = z;
    z1 = x;
  }

  //eq c7.3
  //3 rotation matricies (in paper) can be multiplied into one (here) for simplicity / speed
  //g1*g2*g3 = {{sqrt(2/3), 0, 1/sqrt(3)},{-1/sqrt(6), 1/sqrt(2), 1/sqrt(3)},{-1/sqrt(6), 1/sqrt(2), 1/sqrt(3)}}
  x2 = x1 * sqrt(2.0f / 3.0f) - (y1 + z1) / sqrt(6.0f);
  y2 = (y1 - z1) / sqrt(2.0f);
  z2 = (x1 + y1 + z1) / sqrt(3.0f);

  //eq c7.4
  k = (sqrt(3.0f) * y2 + x2) / (2.0f * pow(x2 * x2 + y2 * y2, 1.5f));
  x3 = x2 * (x2 + sqrt(3.0f) * y2) * (x2 - sqrt(3.0f) * y2) * k;
  y3 = y2 * (y2 + sqrt(3.0f) * x2) * (sqrt(3.0f) * x2 - y2) * k;
  z3 = z2 * sqrt(3.0f);

  //eq c7.5 these hsv are from 0 to 1 in cartesian coordinates
  x4 = -x3;
  y4 = -y3;
  z4 = z3;

  //convert to traditional hsv (0-1)
  h = fmod(atan2f(y4, x4) + 2.0f * M_PI, 2.0f * M_PI) / (2.0f * M_PI);
  s = sqrt(x4 * x4 + y4 * y4);
  v = z4;
  if(v > 0)
  {
    s = s / v;
  }

  //hsv to rgb (from wikipedia hsv/hsl page)
  c = v * s;
  k = c * (1 - fabs(fmod(h * 6, 2) - 1)); //x in wiki article
  h = h * 6;
  r = 0;
  g = 0;
  b = 0;

  if(h >= 0)
  {
    if(h < 1)
    {
      r = c;
      g = k;
    }
    else if(h < 2)
    {
      r = k;
      g = c;
    }
    else if(h < 3)
    {
      g = c;
      b = k;
    }
    else if(h < 4)
    {
      g = k;
      b = c;
    }
    else if (h < 5)
    {
      r = k;
      b = c;
    }
    else if(h < 6)
    {
      r = c;
      b = k;
    }
  }

  //adjust lumosity and invert
  r = (r + (v - c));
  g = (g + (v - c));
  b = (b + (v - c));

  DREAM3D::Rgb rgb = RgbColor::dRgb(r * 255, g * 255, b * 255, 0);

  return rgb;
}



