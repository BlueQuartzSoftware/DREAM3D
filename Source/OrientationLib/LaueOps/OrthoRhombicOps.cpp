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


#include "OrthoRhombicOps.h"

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

#include "OrientationLib/OrientationMath/OrientationArray.hpp"
#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"
#include "OrientationLib/Utilities/PoleFigureUtilities.h"
#include "OrientationLib/Utilities/ComputeStereographicProjection.h"

namespace Detail
{

  static const float OrthoDim1InitValue = powf((0.75f * ((SIMPLib::Constants::k_Pi / 2.0f) - sinf((SIMPLib::Constants::k_Pi / 2.0f)))), (1.0f / 3.0f));
  static const float OrthoDim2InitValue = powf((0.75f * ((SIMPLib::Constants::k_Pi / 2.0f) - sinf((SIMPLib::Constants::k_Pi / 2.0f)))), (1.0f / 3.0f));
  static const float OrthoDim3InitValue = powf((0.75f * ((SIMPLib::Constants::k_Pi / 2.0f) - sinf((SIMPLib::Constants::k_Pi / 2.0f)))), (1.0f / 3.0f));
  static const float OrthoDim1StepValue = OrthoDim1InitValue / 18.0f;
  static const float OrthoDim2StepValue = OrthoDim2InitValue / 18.0f;
  static const float OrthoDim3StepValue = OrthoDim3InitValue / 18.0f;

  namespace Orthorhombic
  {
    static const int symSize0 = 2;
    static const int symSize1 = 2;
    static const int symSize2 = 2;
  }
}

static const QuatF OrthoQuatSym[4] = { QuaternionMathF::New(0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f),
                                       QuaternionMathF::New(1.000000000f, 0.000000000f, 0.000000000f, 0.000000000f),
                                       QuaternionMathF::New(0.000000000f, 1.000000000f, 0.000000000f, 0.000000000f),
                                       QuaternionMathF::New(0.000000000f, 0.000000000f, 1.000000000f, 0.000000000)
                                     };

static const float OrthoRodSym[4][3] = {{0.0f, 0.0f, 0.0f},
  {10000000000.0f, 0.0f, 0.0f},
  {0.0f, 10000000000.0f, 0.0f},
  {0.0f, 0.0f, 10000000000.0}
};

static const float OrthoMatSym[4][3][3] =
{
  { {1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 0.0, 1.0}
  },

  { {1.0,  0.0,  0.0},
    {0.0, -1.0,  0.0},
    {0.0,  0.0, -1.0}
  },

  { { -1.0,  0.0,  0.0},
    {0.0, 1.0,  0.0},
    {0.0,  0.0, -1.0}
  },

  { { -1.0,  0.0,  0.0},
    {0.0, -1.0,  0.0},
    {0.0,  0.0, 1.0}
  }
};

using namespace Detail;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrthoRhombicOps::OrthoRhombicOps()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
OrthoRhombicOps::~OrthoRhombicOps()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool OrthoRhombicOps::getHasInversion()
{
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int OrthoRhombicOps::getODFSize()
{
  return k_OdfSize;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int OrthoRhombicOps::getMDFSize()
{
  return k_MdfSize;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int OrthoRhombicOps::getNumSymOps()
{
  return k_NumSymQuats;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString OrthoRhombicOps::getSymmetryName()
{
  return "OrthoRhombic mmm";;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float OrthoRhombicOps::_calcMisoQuat(const QuatF quatsym[4], int numsym,
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
    //MULT_QUAT(qr, quatsym[i], qc)
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

float OrthoRhombicOps::getMisoQuat(QuatF& q1, QuatF& q2, float& n1, float& n2, float& n3)
{
  int numsym = 4;

  return _calcMisoQuat(OrthoQuatSym, numsym, q1, q2, n1, n2, n3);
}

void OrthoRhombicOps::getQuatSymOp(int i, QuatF& q)
{
  QuaternionMathF::Copy(OrthoQuatSym[i], q);

}

void OrthoRhombicOps::getRodSymOp(int i, float* r)
{
  r[0] = OrthoRodSym[i][0];
  r[1] = OrthoRodSym[i][1];
  r[2] = OrthoRodSym[i][2];
}

void OrthoRhombicOps::getMatSymOp(int i, float g[3][3])
{
  g[0][0] = OrthoMatSym[i][0][0];
  g[0][1] = OrthoMatSym[i][0][1];
  g[0][2] = OrthoMatSym[i][0][2];
  g[1][0] = OrthoMatSym[i][1][0];
  g[1][1] = OrthoMatSym[i][1][1];
  g[1][2] = OrthoMatSym[i][1][2];
  g[2][0] = OrthoMatSym[i][2][0];
  g[2][1] = OrthoMatSym[i][2][1];
  g[2][2] = OrthoMatSym[i][2][2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FOrientArrayType OrthoRhombicOps::getODFFZRod(FOrientArrayType rod)
{
  int  numsym = 4;
  return _calcRodNearestOrigin(OrthoRodSym, numsym, rod);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FOrientArrayType OrthoRhombicOps::getMDFFZRod(FOrientArrayType rod)
{
  float w, n1, n2, n3;
  float FZn1 = 0.0f, FZn2 = 0.0f, FZn3 = 0.0f, FZw = 0.0f;

  rod = _calcRodNearestOrigin(OrthoRodSym, 4, rod);

  FOrientArrayType ax(4, 0.0f);
  OrientationTransforms<FOrientArrayType, float>::ro2ax(rod, ax);
  n1 = ax[0];
  n2 = ax[1], n3 = ax[2], w = ax[3];

  ///FIXME: Are we missing code for Orthorhombic MDF FZ Rodrigues calculation?

  ax.fromAxisAngle(FZn1, FZn2, FZn3, FZw);
  OrientationTransforms<FOrientArrayType, float>::ax2ro(ax, rod);
  return rod;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrthoRhombicOps::getNearestQuat(QuatF& q1, QuatF& q2)
{
  int numsym = 4;

  _calcNearestQuat(OrthoQuatSym, numsym, q1, q2);
}

void OrthoRhombicOps::getFZQuat(QuatF& qr)
{
  int numsym = 4;

  _calcQuatNearestOrigin(OrthoQuatSym, numsym, qr);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int OrthoRhombicOps::getMisoBin(FOrientArrayType rod)
{
  float dim[3];
  float bins[3];
  float step[3];
  FOrientArrayType ho(3);
  OrientationTransforms<FOrientArrayType, float>::ro2ho(rod, ho);

  dim[0] = OrthoDim1InitValue;
  dim[1] = OrthoDim2InitValue;
  dim[2] = OrthoDim3InitValue;
  step[0] = OrthoDim1StepValue;
  step[1] = OrthoDim2StepValue;
  step[2] = OrthoDim3StepValue;
  bins[0] = 36.0;
  bins[1] = 36.0;
  bins[2] = 36.0;

  return _calcMisoBin(dim, bins, step, ho);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FOrientArrayType OrthoRhombicOps::determineEulerAngles(uint64_t seed, int choose)
{
  float init[3];
  float step[3];
  int32_t phi[3];
  float h1, h2, h3;

  init[0] = OrthoDim1InitValue;
  init[1] = OrthoDim2InitValue;
  init[2] = OrthoDim3InitValue;
  step[0] = OrthoDim1StepValue;
  step[1] = OrthoDim2StepValue;
  step[2] = OrthoDim3StepValue;
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
FOrientArrayType OrthoRhombicOps::randomizeEulerAngles(FOrientArrayType synea)
{
  QuatF q;
  QuatF qc;
  size_t symOp = getRandomSymmetryOperatorIndex(k_NumSymQuats);

  FOrientArrayType quat(4, 0.0f);
  OrientationTransforms<FOrientArrayType, float>::eu2qu(synea, quat);
  q = quat.toQuaternion();
  QuaternionMathF::Multiply(OrthoQuatSym[symOp], q, qc);

  quat.fromQuaternion(qc);
  OrientationTransforms<FOrientArrayType, float>::qu2eu(quat, synea);
  return synea;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FOrientArrayType OrthoRhombicOps::determineRodriguesVector(uint64_t seed, int choose)
{
  float init[3];
  float step[3];
  int32_t phi[3];
  float h1, h2, h3;

  init[0] = OrthoDim1InitValue;
  init[1] = OrthoDim2InitValue;
  init[2] = OrthoDim3InitValue;
  step[0] = OrthoDim1StepValue;
  step[1] = OrthoDim2StepValue;
  step[2] = OrthoDim3StepValue;
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
int OrthoRhombicOps::getOdfBin(FOrientArrayType rod)
{
  float dim[3];
  float bins[3];
  float step[3];

  FOrientArrayType ho(3);
  OrientationTransforms<FOrientArrayType, float>::ro2ho(rod, ho);

  dim[0] = OrthoDim1InitValue;
  dim[1] = OrthoDim2InitValue;
  dim[2] = OrthoDim3InitValue;
  step[0] = OrthoDim1StepValue;
  step[1] = OrthoDim2StepValue;
  step[2] = OrthoDim3StepValue;
  bins[0] = 36.0f;
  bins[1] = 36.0f;
  bins[2] = 36.0f;

  return _calcODFBin(dim, bins, step, ho);
}

void OrthoRhombicOps::getSchmidFactorAndSS(float load[3], float& schmidfactor, float angleComps[2], int& slipsys)
{
  schmidfactor = 0;
  slipsys = 0;
}

void OrthoRhombicOps::getSchmidFactorAndSS(float load[3], float plane[3], float direction[3], float& schmidfactor, float angleComps[2], int& slipsys)
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
    slipPlane[2] = OrthoMatSym[i][2][0] * plane[0] + OrthoMatSym[i][2][1] * plane[1] + OrthoMatSym[i][2][2] * plane[2];

    //dont consider negative z planes (to avoid duplicates)
    if( slipPlane[2] >= 0)
    {
      slipPlane[0] = OrthoMatSym[i][0][0] * plane[0] + OrthoMatSym[i][0][1] * plane[1] + OrthoMatSym[i][0][2] * plane[2];
      slipPlane[1] = OrthoMatSym[i][1][0] * plane[0] + OrthoMatSym[i][1][1] * plane[1] + OrthoMatSym[i][1][2] * plane[2];

      float slipDirection[3] = {0};
      slipDirection[0] = OrthoMatSym[i][0][0] * direction[0] + OrthoMatSym[i][0][1] * direction[1] + OrthoMatSym[i][0][2] * direction[2];
      slipDirection[1] = OrthoMatSym[i][1][0] * direction[0] + OrthoMatSym[i][1][1] * direction[1] + OrthoMatSym[i][1][2] * direction[2];
      slipDirection[2] = OrthoMatSym[i][2][0] * direction[0] + OrthoMatSym[i][2][1] * direction[1] + OrthoMatSym[i][2][2] * direction[2];

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

void OrthoRhombicOps::getmPrime(QuatF& q1, QuatF& q2, float LD[3], float& mPrime)
{
  mPrime = 0;
}

void OrthoRhombicOps::getF1(QuatF& q1, QuatF& q2, float LD[3], bool maxSF, float& F1)
{
  F1 = 0;
}

void OrthoRhombicOps::getF1spt(QuatF& q1, QuatF& q2, float LD[3], bool maxSF, float& F1spt)
{
  F1spt = 0;
}

void OrthoRhombicOps::getF7(QuatF& q1, QuatF& q2, float LD[3], bool maxSF, float& F7)
{
  F7 = 0;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
namespace Detail
{
  namespace OrthoRhombic
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
            direction[0] = 0.0;
            direction[1] = 0.0;
            direction[2] = 1.0;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz001->getPointer(i * 6));
            MatrixMath::Copy3x1(m_xyz001->getPointer(i * 6), m_xyz001->getPointer(i * 6 + 3));
            MatrixMath::Multiply3x1withConstant(m_xyz001->getPointer(i * 6 + 3), -1);


            // -----------------------------------------------------------------------------
            // 011 Family
            direction[0] = 1.0;
            direction[1] = 0.0;
            direction[2] = 0.0;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz011->getPointer(i * 6));
            MatrixMath::Copy3x1(m_xyz011->getPointer(i * 6), m_xyz011->getPointer(i * 6 + 3));
            MatrixMath::Multiply3x1withConstant(m_xyz011->getPointer(i * 6 + 3), -1);


            // -----------------------------------------------------------------------------
            // 111 Family
            direction[0] = 0.0;
            direction[1] = 1.0;
            direction[2] = 0.0;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz111->getPointer(i * 6));
            MatrixMath::Copy3x1(m_xyz111->getPointer(i * 6), m_xyz111->getPointer(i * 6 + 3));
            MatrixMath::Multiply3x1withConstant(m_xyz111->getPointer(i * 6 + 3), -1);
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
void OrthoRhombicOps::generateSphereCoordsFromEulers(FloatArrayType* eulers, FloatArrayType* xyz001, FloatArrayType* xyz011, FloatArrayType* xyz111)
{
  size_t nOrientations = eulers->getNumberOfTuples();

  // Sanity Check the size of the arrays
  if (xyz001->getNumberOfTuples() < nOrientations * Detail::Orthorhombic::symSize0)
  {
    xyz001->resize(nOrientations * Detail::Orthorhombic::symSize0 * 3);
  }
  if (xyz011->getNumberOfTuples() < nOrientations * Detail::Orthorhombic::symSize1)
  {
    xyz011->resize(nOrientations * Detail::Orthorhombic::symSize1 * 3);
  }
  if (xyz111->getNumberOfTuples() < nOrientations * Detail::Orthorhombic::symSize2)
  {
    xyz111->resize(nOrientations * Detail::Orthorhombic::symSize2 * 3);
  }


#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
  {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, nOrientations),
                      Detail::OrthoRhombic::GenerateSphereCoordsImpl(eulers, xyz001, xyz011, xyz111), tbb::auto_partitioner());
  }
  else
#endif
  {
    Detail::OrthoRhombic::GenerateSphereCoordsImpl serial(eulers, xyz001, xyz011, xyz111);
    serial.generate(0, nOrientations);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool OrthoRhombicOps::inUnitTriangle(float eta, float chi)
{
  if( eta < 0 || eta > (90.0 * SIMPLib::Constants::k_PiOver180) || chi < 0 || chi > (90.0 * SIMPLib::Constants::k_PiOver180) )
  {
    return false;
  }
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPL::Rgb OrthoRhombicOps::generateIPFColor(double* eulers, double* refDir, bool convertDegrees)
{
  return generateIPFColor(eulers[0], eulers[1], eulers[2], refDir[0], refDir[1], refDir[2], convertDegrees);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPL::Rgb OrthoRhombicOps::generateIPFColor(double phi1, double phi, double phi2, double refDir0, double refDir1, double refDir2, bool degToRad)
{
  if (degToRad == true)
  {
    phi1 = phi1 * SIMPLib::Constants::k_DegToRad;
    phi = phi * SIMPLib::Constants::k_DegToRad;
    phi2 = phi2 * SIMPLib::Constants::k_DegToRad;
  }
  QuatF qc;
  QuatF q2;
  QuatF q1;
  float g[3][3];
  float p[3];
  float refDirection[3];
  float eta, chi;
  float _rgb[3] = { 0.0, 0.0, 0.0 };


  FOrientArrayType eu(phi1, phi, phi2);
  FOrientArrayType qu(4);
  FOrientArrayType om(9); // Reusable for the loop
  OrientationTransforms<FOrientArrayType, float>::eu2qu(eu, qu);
  q1 = qu.toQuaternion();

  for (int j = 0; j < 4; j++)
  {
    getQuatSymOp(j, q2);
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
  float chiMax = 90.0;
  float etaDeg = eta * SIMPLib::Constants::k_180OverPi;
  float chiDeg = chi * SIMPLib::Constants::k_180OverPi;

  _rgb[0] = 1.0 - chiDeg / chiMax;
  _rgb[2] = fabs(etaDeg - etaMin) / (etaMax - etaMin);
  _rgb[1] = 1 - _rgb[2];
  _rgb[1] *= chiDeg / chiMax;
  _rgb[2] *= chiDeg / chiMax;
  _rgb[0] = sqrt(_rgb[0]);
  _rgb[1] = sqrt(_rgb[1]);
  _rgb[2] = sqrt(_rgb[2]);

  float max = _rgb[0];
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
SIMPL::Rgb OrthoRhombicOps::generateRodriguesColor(float r1, float r2, float r3)
{
  float range1 = 2.0f * OrthoDim1InitValue;
  float range2 = 2.0f * OrthoDim2InitValue;
  float range3 = 2.0f * OrthoDim3InitValue;
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
QVector<UInt8ArrayType::Pointer> OrthoRhombicOps::generatePoleFigure(PoleFigureConfiguration_t& config)
{

  QString label0("Orthorhombic <001>");
  QString label1("Orthorhombic <100>");
  QString label2("Orthorhombic <010>");
  if(config.labels.size() > 0) { label0 = config.labels.at(0); }
  if(config.labels.size() > 1) { label1 = config.labels.at(1); }
  if(config.labels.size() > 2) { label2 = config.labels.at(2); }

  int numOrientations = config.eulers->getNumberOfTuples();

  // Create an Array to hold the XYZ Coordinates which are the coords on the sphere.
  QVector<size_t> dims(1, 3);
  std::vector<FloatArrayType::Pointer> coords(3);
  coords[0] = FloatArrayType::CreateArray(numOrientations * Detail::Orthorhombic::symSize0, dims, label0 + QString("001_Coords"));
  coords[1] = FloatArrayType::CreateArray(numOrientations * Detail::Orthorhombic::symSize1, dims, label1 + QString("100_Coords"));
  coords[2] = FloatArrayType::CreateArray(numOrientations * Detail::Orthorhombic::symSize2, dims, label2 + QString("010_Coords"));

  config.sphereRadius = 1.0f;

  // Generate the coords on the sphere **** Parallelized
  generateSphereCoordsFromEulers(config.eulers, coords[0].get(), coords[1].get(), coords[2].get());


  // These arrays hold the "intensity" images which eventually get converted to an actual Color RGB image
  // Generate the modified Lambert projection images (Squares, 2 of them, 1 for northern hemisphere, 1 for southern hemisphere
  DoubleArrayType::Pointer intensity001 = DoubleArrayType::CreateArray(config.imageDim * config.imageDim, label0 + "_Intensity_Image");
  DoubleArrayType::Pointer intensity100 = DoubleArrayType::CreateArray(config.imageDim * config.imageDim, label1 + "_Intensity_Image");
  DoubleArrayType::Pointer intensity010 = DoubleArrayType::CreateArray(config.imageDim * config.imageDim, label2 + "_Intensity_Image");
#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
  tbb::task_group* g = new tbb::task_group;

  if(doParallel == true)
  {
    g->run(ComputeStereographicProjection(coords[0].get(), &config, intensity001.get()));
    g->run(ComputeStereographicProjection(coords[1].get(), &config, intensity100.get()));
    g->run(ComputeStereographicProjection(coords[2].get(), &config, intensity010.get()));
    g->wait(); // Wait for all the threads to complete before moving on.
    delete g;
    g = nullptr;
  }
  else
#endif
  {
    ComputeStereographicProjection m001(coords[0].get(), &config, intensity001.get());
    m001();
    ComputeStereographicProjection m011(coords[1].get(), &config, intensity100.get());
    m011();
    ComputeStereographicProjection m111(coords[2].get(), &config, intensity010.get());
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


  dPtr = intensity100->getPointer(0);
  count = intensity100->getNumberOfTuples();
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

  dPtr = intensity010->getPointer(0);
  count = intensity010->getNumberOfTuples();
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
  UInt8ArrayType::Pointer image100 = UInt8ArrayType::CreateArray(config.imageDim * config.imageDim, dims, label1);
  UInt8ArrayType::Pointer image010 = UInt8ArrayType::CreateArray(config.imageDim * config.imageDim, dims, label2);

  QVector<UInt8ArrayType::Pointer> poleFigures(3);
  if(config.order.size() == 3)
  {
    poleFigures[config.order[0]] = image001;
    poleFigures[config.order[1]] = image100;
    poleFigures[config.order[2]] = image010;
  }
  else
  {
    poleFigures[0] = image001;
    poleFigures[1] = image100;
    poleFigures[2] = image010;
  }

#ifdef SIMPLib_USE_PARALLEL_ALGORITHMS
  if(doParallel == true)
  {
    g = new tbb::task_group;
    g->run(GeneratePoleFigureRgbaImageImpl(intensity001.get(), &config, image001.get()));
    g->run(GeneratePoleFigureRgbaImageImpl(intensity100.get(), &config, image100.get()));
    g->run(GeneratePoleFigureRgbaImageImpl(intensity010.get(), &config, image010.get()));
    g->wait(); // Wait for all the threads to complete before moving on.
    delete g;
    g = nullptr;
  }
  else
#endif
  {
    GeneratePoleFigureRgbaImageImpl m001(intensity001.get(), &config, image001.get());
    m001();
    GeneratePoleFigureRgbaImageImpl m011(intensity100.get(), &config, image100.get());
    m011();
    GeneratePoleFigureRgbaImageImpl m111(intensity010.get(), &config, image010.get());
    m111();
  }

  return poleFigures;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UInt8ArrayType::Pointer OrthoRhombicOps::generateIPFTriangleLegend(int imageDim)
{

  QVector<size_t> dims(1, 4);
  UInt8ArrayType::Pointer image = UInt8ArrayType::CreateArray(imageDim * imageDim, dims, getSymmetryName() + " Triangle Legend");
  uint32_t* pixelPtr = reinterpret_cast<uint32_t*>(image->getPointer(0));

  float xInc = 1.0f / static_cast<float>(imageDim);
  float yInc = 1.0f / static_cast<float>(imageDim);
  float rad = 1.0f;

  float x = 0.0f;
  float y = 0.0f;
  float a = 0.0f;
  float b = 0.0f;
  float c = 0.0f;

  float val = 0.0f;
  float x1 = 0.0f;
  float y1 = 0.0f;
  float z1 = 0.0f;
  float denom = 0.0f;

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

      x = xIndex * xInc;
      y = yIndex * yInc;

      float sumSquares = (x * x) + (y * y);
      if( sumSquares > 1.0f) // Outside unit circle
      {
        color = 0xFFFFFFFF;
      }
      else if ( sumSquares > (rad-2*xInc) && sumSquares < (rad+2*xInc))
      {
        color = 0xFF000000;
      }
      else if (xIndex == 0 || yIndex == 0)
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
SIMPL::Rgb OrthoRhombicOps::generateMisorientationColor(const QuatF& q, const QuatF& refFrame)
{
  Q_ASSERT(false);

  float n1, n2, n3, w;
  float x, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11;
  float y, y1, y2, y3, y4, y5, y6, y7, y8, y9, y10, y11;
  float z, z1, z2, z3, z4, z5, z6, z7, z8, z9, z10, z11;
  float k, h, s, v, c, r, g, b;

  QuatF q1, q2;
  QuaternionMathF::Copy(q, q1);
  QuaternionMathF::Copy(refFrame, q2);

  //get disorientation
  w = getMisoQuat(q1, q2, n1, n2, n3);
  n1 = fabs(n1);
  n2 = fabs(n2);
  n3 = fabs(n3);

  //eq c1.1
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

  //eq c1.2
  k = std::max(x, y);
  k = std::max(k, z);
  k = (k * sqrt(3.0f)) / (x + y + z);
  x1 = x * k;
  y1 = y * k;
  z1 = z * k;

  //eq c1.3
  //3 rotation matricies (in paper) can be multiplied into one (here) for simplicity / speed
  //g1*g2*g3 = {{sqrt(2/3), 0, 1/sqrt(3)},{-1/sqrt(6), 1/sqrt(2), 1/sqrt(3)},{-1/sqrt(6), 1/sqrt(2), 1/sqrt(3)}}
  x2 = x1 * sqrt(2.0f / 3.0f) - (y1 + z1) / sqrt(6.0f);
  y2 = (y1 - z1) / sqrt(2.0f);
  z2 = (x1 + y1 + z1) / sqrt(3.0f);

  //eq c1.4
  k = fmodf(atan2f(y2, x2) + 2.0f * SIMPLib::Constants::k_Pi, 2.0f * SIMPLib::Constants::k_Pi);
  x3 = cos(k) * sqrt((x2 * x2 + y2 * y2) / 2.0) * sin(SIMPLib::Constants::k_Pi / 6.0 + fmodf(k, 2.0f * SIMPLib::Constants::k_Pi / 3.0f)) / 0.5f;
  y3 = sin(k) * sqrt((x2 * x2 + y2 * y2) / 2.0) * sin(SIMPLib::Constants::k_Pi / 6.0 + fmodf(k, 2.0f * SIMPLib::Constants::k_Pi / 3.0f)) / 0.5f;
  z3 = z2 - 1.0f;

  //eq c1.5
  k = (sqrt(x3 * x3 + y3 * y3) - z3) / sqrt(x3 * x3 + y3 * y3 + z3 * z3);
  x4 = x3 * k;
  y4 = y3 * k;
  z4 = z3 * k;

  //eq c1.6, 7, and 8 (from matlab code not paper)
  k = fmod(atan2(y4, x4) + 2 * M_PI, 2 * M_PI);

  int type;
  if(k >= 0.0f && k < 2.0f * M_PI / 3.0f)
  {
    type = 1;
    x5 = (x4 + y4 * sqrt(3.0f)) / 2.0f;
    y5 = (-x4 * sqrt(3.0f) + y4) / 2.0f;
  }
  else if(k >= 2.0f * M_PI / 3.0f && k < 4.0f * M_PI / 3.0f)
  {
    type = 2;
    x5 = x4;
    y5 = y4;
  }
  else//k>=4*pi/3 && <2*pi
  {
    type = 3;
    x5 = (x4 - y4 * sqrt(3.0f)) / 2.0f;
    y5 = (x4 * sqrt(3.0f) + y4) / 2.0f;
  }
  z5 = z4;

  k = 1.5f * atan2(y5, x5);
  x6 = sqrt(x5 * x5 + y5 * y5) * cos(k);
  y6 = sqrt(x5 * x5 + y5 * y5) * sin(k);
  z6 = z5;

  k = 2.0f * atan2(x6, -z6);
  x7 = sqrt(x6 * x6 + z6 * z6) * sin(k);
  y7 = y6;
  z7 = -sqrt(x6 * x6 + z6 * z6) * cos(k);

  k = (2.0f / 3.0f) * atan2(y7, x7);
  x8 = sqrt(x7 * x7 + y7 * y7) * cos(k);
  y8 = sqrt(x7 * x7 + y7 * y7) * sin(k);
  z8 = z7;

  if(type == 1)
  {
    x9 = (x8 - y8 * sqrt(3.0f)) / 2.0f;
    y9 = (x8 * sqrt(3.0f) + y8) / 2.0f;
  }
  else if(type == 2)
  {
    x9 = x8;
    y9 = y8;
  }
  else//type==3;
  {
    x9 = (x8 + y8 * sqrt(3.0f)) / 2.0f;
    y9 = (-x8 * sqrt(3.0f) + y8) / 2.0f;
  }
  z9 = z8;

  //c1.9
  x10 = (x9 - y9 * sqrt(3.0f)) / 2.0f;
  y10 = (x9 * sqrt(3.0f) + y9) / 2.0f;
  z10 = z9;

  //cartesian to traditional hsv
  x11 = sqrt(x10 * x10 + y10 * y10 + z10 * z10); //r
  y11 = acos(z10 / x11) / M_PI; //theta
  z11 = fmod(fmod(atan2(y10, x10) + 2.0f * M_PI, 2.0f * M_PI) + 4.0f * M_PI / 3.0f, 2.0f * M_PI) / (2.0f * M_PI); //rho

  if(x11 == 0)
  {
    y11 = 0;
    z11 = 0;
  }

  h = z11;
  if(y11 >= 0.5f)
  {
    s = (1.0f - x11);
    v = 2.0f * x11 * (1.0f - y11) + (1.0f - x11) / 2.0f;
    if(v > 0)
    {
      s = s / (2.0f * v);
    }
    s = 1.0f - s;
  }
  else
  {
    s = (4.0f * x11 * y11) / (1.0f + x11);
    v = 0.5f + x11 / 2;
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
  r = 1 - (r + (v - c));
  g = 1 - (g + (v - c));
  b = 1 - (b + (v - c));

  SIMPL::Rgb rgb = RgbColor::dRgb(r * 255, g * 255, b * 255, 0);

  return rgb;
}

