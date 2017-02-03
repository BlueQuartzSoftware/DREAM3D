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

#include "OrientationMath.h"

#include <cmath>

#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Math/MatrixMath.h"
#include "SIMPLib/Math/GeometryMath.h"

#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"
#include "OrientationLib/LaueOps/CubicOps.h"
#include "OrientationLib/LaueOps/HexagonalOps.h"
#include "OrientationLib/LaueOps/OrthoRhombicOps.h"
#include "OrientationLib/LaueOps/TrigonalOps.h"
#include "OrientationLib/LaueOps/TetragonalOps.h"


#define ASSERT(condition, message) \
  do { \
    if (! (condition)) { \
      std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                << " line " << __LINE__ << ": " << message << std::endl; \
      std::exit(EXIT_FAILURE); \
    } \
  } while (false)


// Floating-point modulo
// The result (the remainder) has same sign as the divisor.
// Similar to matlab's mod(); Not similar to fmod() -   Mod(-3,4)= 1   fmod(-3,4)= -3
template<typename T>
class Mod
{
  public:
    Mod() {}
    virtual ~Mod() {}

    T operator()(T x, T y)
    {
      ASSERT(!std::numeric_limits<T>::is_exact , "Mod: floating-point type expected");

      if (0. == y)
      { return x; }

      double m = x - y * floor(x / y);

      // handle boundary cases resulted from floating-point cut off:

      if (y > 0)              // modulo range: [0..y)
      {
        if (m >= y)         // Mod(-1e-16             , 360.    ): m= 360.
        { return 0; }

        if (m < 0 )
        {
          if (y + m == y)
          { return 0  ; } // just in case...
          else
          { return y + m; } // Mod(106.81415022205296 , _TWO_PI ): m= -1.421e-14
        }
      }
      else                    // modulo range: (y..0]
      {
        if (m <= y)         // Mod(1e-16              , -360.   ): m= -360.
        { return 0; }

        if (m > 0 )
        {
          if (y + m == y)
          { return 0  ; } // just in case...
          else
          { return y + m; } // Mod(-106.81415022205296, -_TWO_PI): m= 1.421e-14
        }
      }

      return m;
    }

    // wrap [rad] angle to [-PI..PI)
    inline static T WrapPosNegPI(T fAng)
    {
      return Mod()(fAng + SIMPLib::Constants::k_Pi, SIMPLib::Constants::k_2Pi) - SIMPLib::Constants::k_Pi;
    }

    // wrap [rad] angle to [0..TWO_PI)
    inline T WrapTwoPI(T fAng)
    {
      return Mod()(fAng, SIMPLib::Constants::k_2Pi);
    }

    // wrap [deg] angle to [-180..180)
    inline T WrapPosNeg180(T fAng)
    {
      return Mod()(fAng + 180.0, 360.0) - 180.0;
    }

    // wrap [deg] angle to [0..360)
    inline T Wrap360(T fAng)
    {
      return Mod()(fAng , 360.0);
    }

  private:
    Mod(const Mod&); // Copy Constructor Not Implemented
    void operator=(const Mod&); // Operator '=' Not Implemented
};
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

#if 0

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::EulertoMatActive(float ea1, float ea2, float ea3, float g[3][3])
{
  // Calcuate all the values once
  float cp1 = cosf(ea1);
  float sp1 = sinf(ea1);
  float cp = cosf(ea2);
  float sp = sinf(ea2);
  float cp2 = cosf(ea3);
  float sp2 = sinf(ea3);

  // 1) find rotation matrix from Euler angles
  // g[row][col] This is an ACTIVE rotation
  g[0][0] =  cp1 * cp2 - sp1 * sp2 * cp;
  g[0][1] = -cp1 * sp2 - sp1 * cp2 * cp;
  g[0][2] =  sp1 * sp;
  g[1][0] = sp1 * cp2 + cp1 * sp2 * cp;
  g[1][1] = -sp1 * sp2 + cp1 * cp2 * cp;
  g[1][2] = -cp1 * sp;
  g[2][0] = sp2 * sp;
  g[2][1] = cp2 * sp;
  g[2][2] = cp;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float OrientationMath::MatrixMisorientation(float g1[3][3], float g2[3][3])
{
  float deltaG[3][3];
  //only need to calculate diagonal terms to allow for trace calculation
  deltaG[0][0] = g1[0][0] * g2[0][0] + g1[1][0] * g2[1][0] + g1[2][0] * g2[2][0];
  deltaG[1][1] = g1[0][1] * g2[0][1] + g1[1][1] * g2[1][1] + g1[2][1] * g2[2][1];
  deltaG[2][2] = g1[0][2] * g2[0][2] + g1[1][2] * g2[1][2] + g1[2][2] * g2[2][2];
  float value = 0.5 * ((deltaG[0][0] + deltaG[1][1] + deltaG[2][2]) - 1.0);
  if(value > 1.0)
  {
    value = 1.0;
  }
  if(value < -1.0)
  {
    value = -1.0;
  }
  return acosf(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::ChangeAxisReferenceFrame(QuatF& q, float& n1, float& n2, float& n3)
{
  float g[3][3];
  float n[3];
  float nNew[3];

  n[0] = n1;
  n[1] = n2;
  n[2] = n3;

  FOrientArrayType om(9, 0.0f);
  FOrientTransformsType::qu2om(FOrientArrayType(q), om);
  om.toGMatrix(g);

  MatrixMath::Multiply3x3with3x1(g, n, nNew);
  MatrixMath::Normalize3x1(nNew);
  n1 = nNew[0];
  n2 = nNew[1];
  n3 = nNew[2];
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuatF OrientationMath::PassiveRotation(float angle, float xAxis, float yAxis, float zAxis, float x, float y, float z)
{
  QuatF q;
  QuatF qStar;

  FOrientArrayType quat(4);
  FOrientTransformsType::ax2qu(FOrientArrayType(xAxis, yAxis, zAxis, angle), quat);
  q = quat.toQuaternion();

  QuaternionMathF::Conjugate(q, qStar);
  QuatF v = QuaternionMathF::New(x, y, z, 0); // Make the Pure quaternion

  QuatF temp = QuaternionMathF::Multiply(qStar, v);
  QuatF passive = QuaternionMathF::Multiply(temp, q);
  return passive;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuatF OrientationMath::ActiveRotation(float angle, float xAxis, float yAxis, float zAxis, float x, float y, float z)
{
  QuatF q;
  QuatF qStar;
  FOrientArrayType quat(4);
  FOrientTransformsType::ax2qu(FOrientArrayType(xAxis, yAxis, zAxis, angle), quat);
  q = quat.toQuaternion();

  QuaternionMathF::Conjugate(q, qStar);
  QuatF v = QuaternionMathF::New(x, y, z, 0); // Make the Pure quaternion

  QuatF temp = QuaternionMathF::Multiply(q, v);
  QuatF active = QuaternionMathF::Multiply(temp, qStar);
  return active;
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::MetricTensorFromLatticeParameters(float a, float b, float c, float alpha, float beta, float gamma, float mt[3][3])
{
  mt[0][0] = a * a;
  mt[0][1] = a * b * cosf(gamma);
  mt[0][2] = a * c * cosf(beta);
  mt[1][0] = a * b * cosf(gamma);
  mt[1][1] = b * b;
  mt[1][2] = b * c * cosf(alpha);
  mt[2][0] = a * c * cosf(beta);
  mt[2][1] = b * c * cosf(alpha);
  mt[2][2] = c * c;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::RootTensorFromLatticeParameters(float a, float b, float c, float alpha, float beta, float gamma, float rt[3][3])
{
  rt[0][0] = a * sinf(beta) * sinf(gamma);
  rt[0][1] = 0.0;
  rt[0][2] = 0.0;
  rt[1][0] = -a * sinf(beta) * cosf(gamma);
  rt[1][1] = b * sinf(alpha);
  rt[1][2] = 0.0;
  rt[2][0] = a * cosf(beta);
  rt[2][1] = b * cosf(alpha);
  rt[2][2] = c;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::MillerBravaisToMillerDirection(int millerBravais[4], int miller[3])
{
  miller[0] = millerBravais[0] - millerBravais[2];
  miller[1] = millerBravais[1] - millerBravais[2];
  miller[2] = millerBravais[3];
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::MillerToMillerBravaisDirection(int miller[3], int millerBravais[4])
{
  millerBravais[0] = 0.33333f * (2 * miller[0] - miller[1]);
  millerBravais[1] = 0.33333f * (2 * miller[1] - miller[0]);
  millerBravais[2] = -0.33333f * (miller[0] + miller[1]);
  millerBravais[3] = miller[2];
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::MillerBravaisToMillerPlane(int millerBravais[4], int miller[3])
{
  miller[0] = millerBravais[0];
  miller[1] = millerBravais[1];
  miller[2] = millerBravais[3];
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::MillerToMillerBravaisPlane(int miller[3], int millerBravais[4])
{
  millerBravais[0] = miller[0];
  millerBravais[1] = miller[1];
  millerBravais[2] = - (miller[0] + miller[1]);
  millerBravais[3] = miller[2];
}
