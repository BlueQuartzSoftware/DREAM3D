/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "TrigonalLowOps.h"

#include <limits>

// Include this FIRST because there is a needed define for some compiles
// to expose some of the constants needed below
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/Common/ModifiedLambertProjection.h"
#include "DREAM3DLib/Utilities/ImageUtilities.h"
#include "DREAM3DLib/Utilities/ColorTable.h"

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/task_group.h>
#include <tbb/task.h>
#endif

namespace Detail
{

  static const float TrigDim1InitValue = powf((0.75f * ((float(M_PI)) - sinf((float(M_PI))))), (1.0f / 3.0f));
  static const float TrigDim2InitValue = powf((0.75f * ((float(M_PI)) - sinf((float(M_PI))))), (1.0f / 3.0f));
  static const float TrigDim3InitValue = powf((0.75f * ((float(M_PI) / 3.0f) - sinf((float(M_PI) / 3.0f)))), (1.0f / 3.0f));
  static const float TrigDim1StepValue = TrigDim1InitValue / 36.0f;
  static const float TrigDim2StepValue = TrigDim2InitValue / 36.0f;
  static const float TrigDim3StepValue = TrigDim3InitValue / 12.0f;
  namespace TrigonalLow
  {
    static const int symSize0 = 2;
    static const int symSize1 = 2;
    static const int symSize2 = 2;
  }
}

static const int TrigNumSym = 3;

static const QuatF TrigQuatSym[3] = {QuaternionMathF::NewXYZW(0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f),
                                     QuaternionMathF::NewXYZW(0.000000000f, 0.000000000f, 0.866025400f, 0.500000000f),
                                     QuaternionMathF::NewXYZW(0.000000000f, 0.000000000f, 0.866025400f, -0.50000000f)
                                    };

static const float TrigRodSym[3][3] = {{0.0f, 0.0f, 0.0f},
  {0.0f, 0.0f, 1.73205f},
  {0.0f, 0.0f, -1.73205f}
};

static const float TrigMatSym[3][3][3] =
{
  { {1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 0.0, 1.0}
  },

  { { -0.5, DREAM3D::Constants::k_Root3Over2,  0.0},
    { -DREAM3D::Constants::k_Root3Over2, -0.5, 0.0},
    {0.0, 0.0,  1.0}
  },

  { { -0.5, -DREAM3D::Constants::k_Root3Over2,  0.0},
    {DREAM3D::Constants::k_Root3Over2, -0.5, 0.0},
    {0.0, 0.0,  1.0}
  }
};



using namespace Detail;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TrigonalLowOps::TrigonalLowOps()
{
  float junk1 =  TrigDim1StepValue * 1.0f;
  float junk2 = junk1 / TrigDim2StepValue;
  float junk3 = junk2 / TrigDim3StepValue;
  junk1 = junk3 / junk2;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TrigonalLowOps::~TrigonalLowOps()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float TrigonalLowOps::_calcMisoQuat(const QuatF quatsym[6], int numsym,
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

float TrigonalLowOps::getMisoQuat(QuatF& q1, QuatF& q2, float& n1, float& n2, float& n3)
{
  int numsym = 3;

  return _calcMisoQuat(TrigQuatSym, numsym, q1, q2, n1, n2, n3);
}

void TrigonalLowOps::getQuatSymOp(int i, QuatF& q)
{
  QuaternionMathF::Copy(TrigQuatSym[i], q);

}

int TrigonalLowOps::getNumSymOp()
{
  return TrigNumSym;
}

std::vector<QuatF> TrigonalLowOps::getQuatSymOpsVector()
{
  std::vector<QuatF> quatSymOpsVector (TrigNumSym);
  for(int i=0; i<TrigNumSym; i++)
  {
    getQuatSymOp(i, quatSymOpsVector[i]);
  }
  return quatSymOpsVector;
}

void TrigonalLowOps::getRodSymOp(int i, float* r)
{
  r[0] = TrigRodSym[i][0];
  r[1] = TrigRodSym[i][1];
  r[2] = TrigRodSym[i][2];
}

void TrigonalLowOps::getMatSymOp(int i, float g[3][3])
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

void TrigonalLowOps::getODFFZRod(float& r1, float& r2, float& r3)
{
  int numsym = 3;

  _calcRodNearestOrigin(TrigRodSym, numsym, r1, r2, r3);
}

void TrigonalLowOps::getMDFFZRod(float& r1, float& r2, float& r3)
{
  float w, n1, n2, n3;
  float FZn1, FZn2, FZn3;
  float n1n2mag;

  _calcRodNearestOrigin(TrigRodSym, 12, r1, r2, r3);
  OrientationMath::RodtoAxisAngle(r1, r2, r3, w, n1, n2, n3);

  float denom = sqrt((n1 * n1 + n2 * n2 + n3 * n3));
  n1 = n1 / denom;
  n2 = n2 / denom;
  n3 = n3 / denom;
  if(n3 < 0) { n1 = -n1, n2 = -n2, n3 = -n3; }
  float newangle = 0;
  float angle = 180.0f * atan2(n2, n1) * DREAM3D::Constants::k_1OverPi;
  if(angle < 0) { angle = angle + 360.0f; }
  FZn1 = n1;
  FZn2 = n2;
  FZn3 = n3;
  if(angle > 60.0f)
  {
    n1n2mag = sqrt(n1 * n1 + n2 * n2);
    if (int(angle / 60) % 2 == 0)
    {
      newangle = angle - (60.0f * int(angle / 60.0f));
      newangle = newangle * DREAM3D::Constants::k_PiOver180;
      FZn1 = n1n2mag * cosf(newangle);
      FZn2 = n1n2mag * sinf(newangle);
    }
    else
    {
      newangle = angle - (60.0f * int(angle / 60.0f));
      newangle = 60.0f - newangle;
      newangle = newangle * DREAM3D::Constants::k_PiOver180;
      FZn1 = n1n2mag * cosf(newangle);
      FZn2 = n1n2mag * sinf(newangle);
    }
  }

  OrientationMath::AxisAngletoRod(w, FZn1, FZn2, FZn3, r1, r2, r3);
}
void TrigonalLowOps::getNearestQuat(QuatF& q1, QuatF& q2)
{
  int numsym = 3;

  _calcNearestQuat(TrigQuatSym, numsym, q1, q2);
}

int TrigonalLowOps::getMisoBin(float r1, float r2, float r3)
{
  float dim[3];
  float bins[3];
  float step[3];

  OrientationMath::RodtoHomochoric(r1, r2, r3);

  dim[0] = TrigDim1InitValue;
  dim[1] = TrigDim2InitValue;
  dim[2] = TrigDim3InitValue;
  step[0] = TrigDim1StepValue;
  step[1] = TrigDim2StepValue;
  step[2] = TrigDim3StepValue;
  bins[0] = 72.0f;
  bins[1] = 72.0f;
  bins[2] = 24.0f;

  return _calcMisoBin(dim, bins, step, r1, r2, r3);
}


void TrigonalLowOps::determineEulerAngles(int choose, float& synea1, float& synea2, float& synea3)
{
  float init[3];
  float step[3];
  float phi[3];
  float r1, r2, r3;

  init[0] = TrigDim1InitValue;
  init[1] = TrigDim2InitValue;
  init[2] = TrigDim3InitValue;
  step[0] = TrigDim1StepValue;
  step[1] = TrigDim2StepValue;
  step[2] = TrigDim3StepValue;
  phi[0] = static_cast<float>(choose % 72);
  phi[1] = static_cast<float>((choose / 72) % 72);
  phi[2] = static_cast<float>(choose / (72 * 72));

  _calcDetermineHomochoricValues(init, step, phi, choose, r1, r2, r3);
  OrientationMath::HomochorictoRod(r1, r2, r3);
  getODFFZRod(r1, r2, r3);
  OrientationMath::RodtoEuler(r1, r2, r3, synea1, synea2, synea3);
}

void TrigonalLowOps::randomizeEulerAngles(float& synea1, float& synea2, float& synea3)
{
  QuatF q;
  QuatF qc;
  OrientationMath::EulertoQuat(q, synea1, synea2, synea3);
  int symOp = rand() % k_NumSymQuats + 1;
  QuaternionMathF::Multiply(q, TrigQuatSym[symOp], qc);
  OrientationMath::QuattoEuler(qc, synea1, synea2, synea3);
}

void TrigonalLowOps::determineRodriguesVector( int choose, float& r1, float& r2, float& r3)
{
  float init[3];
  float step[3];
  float phi[3];

  init[0] = TrigDim1InitValue;
  init[1] = TrigDim2InitValue;
  init[2] = TrigDim3InitValue;
  step[0] = TrigDim1StepValue;
  step[1] = TrigDim2StepValue;
  step[2] = TrigDim3StepValue;
  phi[0] = static_cast<float>(choose % 72);
  phi[1] = static_cast<float>((choose / 72) % 72);
  phi[2] = static_cast<float>(choose / (72 * 72));

  _calcDetermineHomochoricValues(init, step, phi, choose, r1, r2, r3);
  OrientationMath::HomochorictoRod(r1, r2, r3);
  getMDFFZRod(r1, r2, r3);
}

int TrigonalLowOps::getOdfBin(float r1, float r2, float r3)
{
  float dim[3];
  float bins[3];
  float step[3];

  OrientationMath::RodtoHomochoric(r1, r2, r3);

  dim[0] = TrigDim1InitValue;
  dim[1] = TrigDim2InitValue;
  dim[2] = TrigDim3InitValue;
  step[0] = TrigDim1StepValue;
  step[1] = TrigDim2StepValue;
  step[2] = TrigDim3StepValue;
  bins[0] = 72.0f;
  bins[1] = 72.0f;
  bins[2] = 24.0f;

  return _calcODFBin(dim, bins, step, r1, r2, r3);
}

void TrigonalLowOps::getSchmidFactorAndSS(float loadx, float loady, float loadz, float& schmidfactor, int& slipsys)
{
  schmidfactor = 0;
  slipsys = 0;
}

void TrigonalLowOps::getmPrime(QuatF& q1, QuatF& q2, float LD[3], float& mPrime)
{
  mPrime = 0;
}

void TrigonalLowOps::getF1(QuatF& q1, QuatF& q2, float LD[3], bool maxSF, float& F1)
{
  F1 = 0;
}

void TrigonalLowOps::getF1spt(QuatF& q1, QuatF& q2, float LD[3], bool maxSF, float& F1spt)
{
  F1spt = 0;
}

void TrigonalLowOps::getF7(QuatF& q1, QuatF& q2, float LD[3], bool maxSF, float& F7)
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
        FloatArrayType* eulers;
        FloatArrayType* m_xyz001;
        FloatArrayType* m_xyz011;
        FloatArrayType* m_xyz111;

      public:
        GenerateSphereCoordsImpl(FloatArrayType* eulerAngles, FloatArrayType* xyz001Coords, FloatArrayType* xyz011Coords, FloatArrayType* xyz111Coords) :
          eulers(eulerAngles),
          m_xyz001(xyz001Coords),
          m_xyz011(xyz011Coords),
          m_xyz111(xyz111Coords)
        {}
        virtual ~GenerateSphereCoordsImpl() {}

        void generate(size_t start, size_t end) const
        {
          float g[3][3];
          float gTranpose[3][3];
          float* currentEuler = NULL;
          float direction[3] = {0.0, 0.0, 0.0};


          for(size_t i = start; i < end; ++i)
          {
            currentEuler = eulers->GetPointer(i * 3);

            OrientationMath::EulerToMat(currentEuler[0], currentEuler[1], currentEuler[2], g);
            MatrixMath::Transpose3x3(g, gTranpose);

            // -----------------------------------------------------------------------------
            // 001 Family
            direction[0] = 0.0;
            direction[1] = 0.0;
            direction[2] = 1.0;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz001->GetPointer(i * 6));
            MatrixMath::Copy3x1(m_xyz001->GetPointer(i * 6), m_xyz001->GetPointer(i * 6 + 3));
            MatrixMath::Multiply3x1withConstant(m_xyz001->GetPointer(i * 6 + 3), -1);


            // -----------------------------------------------------------------------------
            // 011 Family
            direction[0] = -0.5;
            direction[1] = DREAM3D::Constants::k_Root3Over2;
            direction[2] = 0.0;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz011->GetPointer(i * 6));
            MatrixMath::Copy3x1(m_xyz011->GetPointer(i * 6), m_xyz011->GetPointer(i * 6 + 3));
            MatrixMath::Multiply3x1withConstant(m_xyz011->GetPointer(i * 6 + 3), -1);


            // -----------------------------------------------------------------------------
            // 111 Family
            direction[0] = 1;
            direction[1] = 0;
            direction[2] = 0;
            MatrixMath::Multiply3x3with3x1(gTranpose, direction, m_xyz111->GetPointer(i * 6));
            MatrixMath::Copy3x1(m_xyz111->GetPointer(i * 6), m_xyz111->GetPointer(i * 6 + 3));
            MatrixMath::Multiply3x1withConstant(m_xyz111->GetPointer(i * 6 + 3), -1);


          }
        }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
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
void TrigonalLowOps::generateSphereCoordsFromEulers(FloatArrayType* eulers, FloatArrayType* xyz001, FloatArrayType* xyz011, FloatArrayType* xyz111)
{
  BOOST_ASSERT(false);
  size_t nOrientations = eulers->GetNumberOfTuples();

  // Sanity Check the size of the arrays
  if (xyz001->GetNumberOfTuples() < nOrientations * Detail::TrigonalLow::symSize0)
  {
    xyz001->Resize(nOrientations * Detail::TrigonalLow::symSize0 * 3);
  }
  if (xyz011->GetNumberOfTuples() < nOrientations * Detail::TrigonalLow::symSize1)
  {
    xyz011->Resize(nOrientations * Detail::TrigonalLow::symSize1 * 3);
  }
  if (xyz111->GetNumberOfTuples() < nOrientations * Detail::TrigonalLow::symSize2)
  {
    xyz111->Resize(nOrientations * Detail::TrigonalLow::symSize2 * 3);
  }

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::task_scheduler_init init;
  bool doParallel = true;
#endif

#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  if (doParallel == true)
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
bool TrigonalLowOps::inUnitTriangle(float eta, float chi)
{
  if( eta < (-120.0 * DREAM3D::Constants::k_PiOver180) || eta > 0.0 || chi < 0 || chi > (90.0 * DREAM3D::Constants::k_PiOver180) ) { return false; }
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3D::Rgb TrigonalLowOps::generateIPFColor(double* eulers, double* refDir, bool convertDegrees)
{
  return generateIPFColor(eulers[0], eulers[1], eulers[2], refDir[0], refDir[1], refDir[2], convertDegrees);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3D::Rgb TrigonalLowOps::generateIPFColor(double phi1, double phi, double phi2, double refDir0, double refDir1, double refDir2, bool degToRad)
{
  if (degToRad == true)
  {
    phi1 = phi1 * DREAM3D::Constants::k_DegToRad;
    phi = phi * DREAM3D::Constants::k_DegToRad;
    phi2 = phi2 * DREAM3D::Constants::k_DegToRad;
  }
  QuatF qc;
  QuatF q1;
  float g[3][3];
  float p[3];
  float refDirection[3];
  float eta, chi;
  float _rgb[3] = { 0.0, 0.0, 0.0 };

  OrientationMath::EulertoQuat(q1, phi1, phi, phi2);

  for (int j = 0; j < 3; j++)
  {
    QuaternionMathF::Multiply(q1, TrigQuatSym[j], qc);

    OrientationMath::QuattoMat(qc, g);

    refDirection[0] = refDir0;
    refDirection[1] = refDir1;
    refDirection[2] = refDir2;
    MatrixMath::Multiply3x3with3x1(g, refDirection, p);
    MatrixMath::Normalize3x1(p);

    if(getHasInversion() == false && p[2] < 0) { continue; }
    else if(getHasInversion() == true && p[2] < 0) { p[0] = -p[0], p[1] = -p[1], p[2] = -p[2]; }
    chi = acos(p[2]);
    eta = atan2(p[1], p[0]);
    if(inUnitTriangle(eta, chi) == false) { continue; }
    else {break;}
  }

  float etaMin = -120.0;
  float etaMax = 0.0;
  float chiMax = 90.0;
  float etaDeg = eta * DREAM3D::Constants::k_180OverPi;
  float chiDeg = chi * DREAM3D::Constants::k_180OverPi;

  _rgb[0] = 1.0 - chiDeg / chiMax;
  _rgb[2] = fabs(etaDeg - etaMin) / (etaMax - etaMin);
  _rgb[1] = 1 - _rgb[2];
  _rgb[1] *= chiDeg / chiMax;
  _rgb[2] *= chiDeg / chiMax;
  _rgb[0] = sqrt(_rgb[0]);
  _rgb[1] = sqrt(_rgb[1]);
  _rgb[2] = sqrt(_rgb[2]);

  float max = _rgb[0];
  if (_rgb[1] > max) { max = _rgb[1]; }
  if (_rgb[2] > max) { max = _rgb[2]; }

  _rgb[0] = _rgb[0] / max;
  _rgb[1] = _rgb[1] / max;
  _rgb[2] = _rgb[2] / max;

  return RgbColor::dRgb(_rgb[0] * 255, _rgb[1] * 255, _rgb[2] * 255, 255);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3D::Rgb TrigonalLowOps::generateRodriguesColor(float r1, float r2, float r3)
{
  float range1 = 2.0f * TrigDim1InitValue;
  float range2 = 2.0f * TrigDim2InitValue;
  float range3 = 2.0f * TrigDim3InitValue;
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
std::vector<UInt8ArrayType::Pointer> TrigonalLowOps::generatePoleFigure(PoleFigureConfiguration_t& config)
{
  std::vector<UInt8ArrayType::Pointer> poleFigures;
  std::string label0("Trigonal Low <0001>");
  std::string label1("Trigonal Low <-1-120>");
  std::string label2("Trigonal Low <2-1-10>");


  int numOrientations = config.eulers->GetNumberOfTuples();

  // Create an Array to hold the XYZ Coordinates which are the coords on the sphere.
  // this is size for CUBIC ONLY, <001> Family
  FloatArrayType::Pointer xyz001 = FloatArrayType::CreateArray(numOrientations * Detail::TrigonalLow::symSize0, 3, label0 + std::string("xyzCoords"));
  // this is size for CUBIC ONLY, <011> Family
  FloatArrayType::Pointer xyz011 = FloatArrayType::CreateArray(numOrientations * Detail::TrigonalLow::symSize1, 3, label1 + std::string("xyzCoords"));
  // this is size for CUBIC ONLY, <111> Family
  FloatArrayType::Pointer xyz111 = FloatArrayType::CreateArray(numOrientations * Detail::TrigonalLow::symSize2, 3, label2 + std::string("xyzCoords"));

  config.sphereRadius = 1.0f;

  // Generate the coords on the sphere **** Parallelized
  generateSphereCoordsFromEulers(config.eulers, xyz001.get(), xyz011.get(), xyz111.get());


  // These arrays hold the "intensity" images which eventually get converted to an actual Color RGB image
  // Generate the modified Lambert projection images (Squares, 2 of them, 1 for northern hemisphere, 1 for southern hemisphere
  DoubleArrayType::Pointer intensity001 = DoubleArrayType::CreateArray(config.imageDim * config.imageDim, 1, label0 + "_Intensity_Image");
  DoubleArrayType::Pointer intensity011 = DoubleArrayType::CreateArray(config.imageDim * config.imageDim, 1, label1 + "_Intensity_Image");
  DoubleArrayType::Pointer intensity111 = DoubleArrayType::CreateArray(config.imageDim * config.imageDim, 1, label2 + "_Intensity_Image");
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
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

  double* dPtr = intensity001->GetPointer(0);
  size_t count = intensity001->GetNumberOfTuples();
  for(size_t i = 0; i < count; ++i)
  {
    if (dPtr[i] > max) { max = dPtr[i]; }
    if (dPtr[i] < min) { min = dPtr[i]; }
  }


  dPtr = intensity011->GetPointer(0);
  count = intensity011->GetNumberOfTuples();
  for(size_t i = 0; i < count; ++i)
  {
    if (dPtr[i] > max) { max = dPtr[i]; }
    if (dPtr[i] < min) { min = dPtr[i]; }
  }

  dPtr = intensity111->GetPointer(0);
  count = intensity111->GetNumberOfTuples();
  for(size_t i = 0; i < count; ++i)
  {
    if (dPtr[i] > max) { max = dPtr[i]; }
    if (dPtr[i] < min) { min = dPtr[i]; }
  }

  config.minScale = min;
  config.maxScale = max;


  UInt8ArrayType::Pointer image001 = UInt8ArrayType::CreateArray(config.imageDim * config.imageDim, 4, label0);
  UInt8ArrayType::Pointer image011 = UInt8ArrayType::CreateArray(config.imageDim * config.imageDim, 4, label1);
  UInt8ArrayType::Pointer image111 = UInt8ArrayType::CreateArray(config.imageDim * config.imageDim, 4, label2);
#ifdef DREAM3D_USE_PARALLEL_ALGORITHMS
  poleFigures.push_back(image001);
  poleFigures.push_back(image011);
  poleFigures.push_back(image111);

  g = new tbb::task_group;

  if(doParallel == true)
  {
    g->run(GenerateRgbaImageImpl(intensity001.get(), &config, image001.get()));
    g->run(GenerateRgbaImageImpl(intensity011.get(), &config, image011.get()));
    g->run(GenerateRgbaImageImpl(intensity111.get(), &config, image111.get()));
    g->wait(); // Wait for all the threads to complete before moving on.
    delete g;
    g = NULL;
  }
  else
#endif
  {
    GenerateRgbaImageImpl m001(intensity001.get(), &config, image001.get());
    m001();
    GenerateRgbaImageImpl m011(intensity011.get(), &config, image011.get());
    m011();
    GenerateRgbaImageImpl m111(intensity111.get(), &config, image111.get());
    m111();
  }

  return poleFigures;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3D::Rgb TrigonalLowOps::generateMisorientationColor(const QuatF& q, const QuatF& refFrame)
{
  DREAM3D::Rgb rgb = RgbColor::dRgb(0, 0, 0, 0);

  BOOST_ASSERT(false);

  return rgb;
}

