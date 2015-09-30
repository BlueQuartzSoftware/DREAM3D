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

#include "OrientationMath.h"

#include <cmath>

#include "SIMPLib/Math/SIMPLibMath.h"
#include "SIMPLib/Math/MatrixMath.h"
#include "SIMPLib/Math/GeometryMath.h"

#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"
#include "OrientationLib/SpaceGroupOps/CubicOps.h"
#include "OrientationLib/SpaceGroupOps/HexagonalOps.h"
#include "OrientationLib/SpaceGroupOps/OrthoRhombicOps.h"
#include "OrientationLib/SpaceGroupOps/TrigonalOps.h"
#include "OrientationLib/SpaceGroupOps/TetragonalOps.h"


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
#if OM_ORIENTATION_FUNCS
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::AxisAngletoHomochoric(float w, float n1, float n2, float n3, float& r1, float& r2, float& r3)
{
  // Bring omega into proper range.
  Mod<float> mod;
  w = mod.WrapTwoPI(w);

  float denom = 0.0f;

  denom = (n1 * n1) + (n2 * n2) + (n3 * n3);
  denom = sqrt(denom);
  if(denom == 0.0)
  {
    r1 = 0.0, r2 = 0.0, r3 = 0.0;
    return;
  }
  n1 = n1 / denom;
  n2 = n2 / denom;
  n3 = n3 / denom;
  if (w >= 0.0f && w < SIMPLib::Constants::k_Pi)
  {
    float temp = powf(((0.75f) * (w - sinf(w))), SIMPLib::Constants::k_1Over3);
    r1 = n1 * temp;
    r2 = n2 * temp;
    r3 = n3 * temp;
  }
  else
  {
    float temp = powf(((0.75f) * (SIMPLib::Constants::k_2Pi - w + sinf(w))), SIMPLib::Constants::k_1Over3);
    r1 = n1 * temp;
    r2 = n2 * temp;
    r3 = n3 * temp;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::AxisAngletoRod(float w, float n1, float n2, float n3, float& r1, float& r2, float& r3)
{
  float denom;

  denom = (n1 * n1) + (n2 * n2) + (n3 * n3);
  denom = sqrt(denom);
  if(denom == 0.0)
  {
    r1 = 0.0, r2 = 0.0, r3 = 0.0;
    return;
  }
  n1 = n1 / denom;
  n2 = n2 / denom;
  n3 = n3 / denom;
  float const1 = tanf(w / 2.0f);
  r1 = n1 * const1;
  r2 = n2 * const1;
  r3 = n3 * const1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::AxisAngletoQuat(float w, float n1, float n2, float n3, QuatF& q)
{
  float denom;

  denom = (n1 * n1) + (n2 * n2) + (n3 * n3);
  denom = sqrt(denom);
  if(denom == 0.0)
  {
    q.x = 0.0, q.y = 0.0, q.z = 0.0, q.w = 1.0;
    return;
  }
  n1 = n1 / denom;
  n2 = n2 / denom;
  n3 = n3 / denom;
  float const1 = cosf(w * 0.5f);
  float const2 = sinf(w * 0.5f);
  q.x = n1 * const2;
  q.y = n2 * const2;
  q.z = n3 * const2;
  q.w = const1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::AxisAngletoMat(float w, float n1, float n2, float n3, float g[3][3])
{
  float c, s, t, denom;

  c = cosf(w);
  s = sinf(w);
  t = 1 - c;

  denom = (n1 * n1) + (n2 * n2) + (n3 * n3);
  denom = sqrt(denom);
  if(denom == 0.0)
  {
    MatrixMath::Identity3x3(g);
    return;
  }
  n1 = n1 / denom;
  n2 = n2 / denom;
  n3 = n3 / denom;

  g[0][0] = t * n1 * n1 + c;
  g[0][1] = t * n1 * n2 - n3 * s;
  g[0][2] = t * n1 * n3 + n2 * s;
  g[1][0] = t * n1 * n2 + n3 * s;
  g[1][1] = t * n2 * n2 + c;
  g[1][2] = t * n2 * n3 - n1 * s;
  g[2][0] = t * n1 * n3 - n2 * s;
  g[2][1] = t * n2 * n3 + n1 * s;
  g[2][2] = t * n3 * n3 + c;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::RodtoHomochoric(float& r1, float& r2, float& r3)
{
  float rmag, w;

  rmag = (r1 * r1) + (r2 * r2) + (r3 * r3);
  rmag = sqrt(rmag);
  if(rmag == 0.0)
  {
    r1 = 0.0f, r2 = 0.0f, r3 = 0.0f;
    return;
  }
  r1 = r1 / rmag;
  r2 = r2 / rmag;
  r3 = r3 / rmag;
  w = static_cast<float>( 2.0 * atan(rmag) );
  float const1 = powf(((3.0f / 4.0f) * (w - sinf(w))), SIMPLib::Constants::k_1Over3);
  r1 = r1 * const1;
  r2 = r2 * const1;
  r3 = r3 * const1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::HomochorictoRod(float& r1, float& r2, float& r3)
{
  float hmag, w;
  static double c[7] = { -0.5000096149170321, -0.02486606148871731, -0.004549381779362819,
                         0.0005118668366387526, -0.0016500827333575548, 0.0007593352203388718, -0.0002040422502566876
                       };

  hmag = (r1 * r1) + (r2 * r2) + (r3 * r3);
  hmag = sqrt(hmag);
  if(hmag == 0.0)
  {
    r1 = 0.0f, r2 = 0.0f, r3 = 0.0f;
    return;
  }
  r1 = r1 / hmag;
  r2 = r2 / hmag;
  r3 = r3 / hmag;
  double sum = 0.0;
  for(int i = 0; i < 7; ++i)
  {
    sum = sum + c[i] * pow(hmag, 2 * (i + 1));
  }
  w = (1.0 + sum);
  float const1 = sqrt(1.0 - (w * w)) / w;
  r1 = r1 * const1;
  r2 = r2 * const1;
  r3 = r3 * const1;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::RodtoAxisAngle(float r1, float r2, float r3, float& w, float& n1, float& n2, float& n3)
{
  float rmag;

  rmag = (r1 * r1) + (r2 * r2) + (r3 * r3);
  rmag = sqrt(rmag);
  if(rmag == 0.0)
  {
    w = 0.0, n1 = 0.0f, n2 = 0.0f, n3 = 1.0f;
    return;
  }
  w = static_cast<float>( 2.0 * atan(rmag) );
  n1 = r1 / rmag;
  n2 = r2 / rmag;
  n3 = r3 / rmag;
  if(w > SIMPLib::Constants::k_Pi)
  {
    w = (2 * SIMPLib::Constants::k_Pi) - w;
    n1 = -n1;
    n2 = -n2;
    n3 = -n3;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::RodtoQuat(QuatF& q, float r1, float r2, float r3)
{
  float rmag, w;

  rmag = (r1 * r1) + (r2 * r2) + (r3 * r3);
  rmag = sqrt(rmag);
  if(rmag == 0.0)
  {
    q.x = 0.0, q.y = 0.0, q.z = 0.0, q.w = 1.0;
    return;
  }
  r1 = r1 / rmag;
  r2 = r2 / rmag;
  r3 = r3 / rmag;
  w = 2.0f * atan(rmag);
  float const1 = sinf(w / 2.0f);
  q.x = r1 * const1;
  q.y = r2 * const1;
  q.z = r3 * const1;
  q.w = static_cast<float>( cosf(w / 2.0f) );
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::RodtoEuler(float r1, float r2, float r3, float& ea1, float& ea2, float& ea3)
{
  float sum = atan(r3);
  float diff = atan(r2 / r1);
  ea1 = sum + diff;
  ea2 = static_cast<float>( 2. * atan(r1 * cosf(sum) / cosf(diff)) );
  ea3 = sum - diff;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::QuattoAxisAngle(QuatF& q, float& w, float& n1, float& n2, float& n3)
{
  if(q.w == 1.0)
  {
    w = 0.0, n1 = 0.0f, n2 = 0.0f, n3 = 1.0f;
    return;
  }
  w = static_cast<float>( 2.0 * acos(q.w) );
  n1 = q.x / sqrt(1 - (q.w * q.w));
  n2 = q.y / sqrt(1 - (q.w * q.w));
  n3 = q.z / sqrt(1 - (q.w * q.w));
  if(w > SIMPLib::Constants::k_Pi)
  {
    w = (2 * SIMPLib::Constants::k_Pi) - w;
    n1 = -n1;
    n2 = -n2;
    n3 = -n3;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::QuattoMat(QuatF& q, float g[3][3])
{
  g[0][0] = (1 - (2 * q.y * q.y) - (2 * q.z * q.z));
  g[0][1] = ((2 * q.x * q.y) + (2 * q.z * q.w));
  g[0][2] = ((2 * q.x * q.z) - (2 * q.y * q.w));
  g[1][0] = ((2 * q.x * q.y) - (2 * q.z * q.w));
  g[1][1] = (1 - (2 * q.x * q.x) - (2 * q.z * q.z));
  g[1][2] = ((2 * q.y * q.z) + (2 * q.x * q.w));
  g[2][0] = ((2 * q.x * q.z) + (2 * q.y * q.w));
  g[2][1] = ((2 * q.y * q.z) - (2 * q.x * q.w));
  g[2][2] = (1 - (2 * q.x * q.x) - (2 * q.y * q.y));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::QuattoRod(QuatF& q, float& r1, float& r2, float& r3)
{
  float qmag, w;
  float n1, n2, n3;

  qmag = (q.x * q.x) + (q.y * q.y) + (q.z * q.z);
  qmag = sqrt(qmag);
  if(qmag == 0.0)
  {
    r1 = 0.0f, r2 = 0.0f, r3 = 0.0f;
    return;
  }
  n1 = q.x / qmag;
  n2 = q.y / qmag;
  n3 = q.z / qmag;
  w = static_cast<float>( 2.0 * acos(q.w) );
  float const1 = tanf(w * 0.5f);
  r1 = n1 * const1;
  r2 = n2 * const1;
  r3 = n3 * const1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::QuattoEuler(QuatF& q, float& ea1, float& ea2, float& ea3)
{
  float diff, sum, tmp;
  diff = atan2(-q.y, -q.x);
  sum = atan2(-q.z, -q.w);
  ea1 = (diff + sum);
  ea3 = (sum - diff);
  tmp = (q.z * q.z) + (q.w * q.w);
  tmp = sqrt(tmp);
  if(tmp > 1.0f)
  {
    tmp = 1.0f;
  }
  ea2 = 2 * acos(tmp);
  ea1 = ea1 + SIMPLib::Constants::k_2Pi;
  ea3 = ea3 + SIMPLib::Constants::k_2Pi;
  ea1 = fmodf(ea1, SIMPLib::Constants::k_2Pi);
  ea3 = fmodf(ea3, SIMPLib::Constants::k_2Pi);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::EulertoQuat(float e1, float e2, float e3, QuatF& q)
{
  float s, c, s1, c1, s2, c2;
  s = sinf(0.5f * e2);
  c = cosf(0.5f * e2);
  s1 = sinf(0.5f * (e1 - e3));
  c1 = cosf(0.5f * (e1 - e3));
  s2 = sinf(0.5f * (e1 + e3));
  c2 = cosf(0.5f * (e1 + e3));
  q.x = s * c1;
  q.y = s * s1;
  q.z = c * s2;
  q.w = c * c2;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuatF OrientationMath::EulertoQuat(float e1, float e2, float e3)
{
  QuatF q;
  float s, c, s1, c1, s2, c2;
  s = sinf(0.5f * e2);
  c = cosf(0.5f * e2);
  s1 = sinf(0.5f * (e1 - e3));
  c1 = cosf(0.5f * (e1 - e3));
  s2 = sinf(0.5f * (e1 + e3));
  c2 = cosf(0.5f * (e1 + e3));
  q.x = s * c1;
  q.y = s * s1;
  q.z = c * s2;
  q.w = c * c2;
  return q;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::EulertoQuat(const float* e, QuatF& q)
{
  float s, c, s1, c1, s2, c2;
  s = sinf(0.5f * e[1]);
  c = cosf(0.5f * e[1]);
  s1 = sinf(0.5f * (e[0] - e[2]));
  c1 = cosf(0.5f * (e[0] - e[2]));
  s2 = sinf(0.5f * (e[0] + e[2]));
  c2 = cosf(0.5f * (e[0] + e[2]));
  q.x = s * c1;
  q.y = s * s1;
  q.z = c * s2;
  q.w = c * c2;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuatF OrientationMath::EulertoQuat(const float* e)
{
  QuatF q;
  float s, c, s1, c1, s2, c2;
  s = sinf(0.5f * e[1]);
  c = cosf(0.5f * e[1]);
  s1 = sinf(0.5f * (e[0] - e[2]));
  c1 = cosf(0.5f * (e[0] - e[2]));
  s2 = sinf(0.5f * (e[0] + e[2]));
  c2 = cosf(0.5f * (e[0] + e[2]));
  q.x = s * c1;
  q.y = s * s1;
  q.z = c * s2;
  q.w = c * c2;
  return q;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::EulertoMat(float ea1, float ea2, float ea3, float g[3][3])
{
  // Calcuate all the values once
  float cp1 = cosf(ea1);
  float sp1 = sinf(ea1);
  float cp = cosf(ea2);
  float sp = sinf(ea2);
  float cp2 = cosf(ea3);
  float sp2 = sinf(ea3);

  // 1) find rotation matrix from Euler angles
  // g[row][col]
  g[0][0] = cp1 * cp2 - sp1 * sp2 * cp;
  g[0][1] = sp1 * cp2 + cp1 * sp2 * cp;
  g[0][2] = sp2 * sp;
  g[1][0] = -cp1 * sp2 - sp1 * cp2 * cp;
  g[1][1] = -sp1 * sp2 + cp1 * cp2 * cp;
  g[1][2] = cp2 * sp;
  g[2][0] = sp1 * sp;
  g[2][1] = -cp1 * sp;
  g[2][2] = cp;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool closeEnough(const float& a, const float& b,
                 const float& epsilon = std::numeric_limits<float>::epsilon())
{
  return (epsilon > std::abs(a - b));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::MattoEuler(float g[3][3], float& phi1, float& Phi, float& phi2)
{
  if(closeEnough(g[2][2], 1.0) )
  {
    phi1 = atan(g[0][1] / g[0][0]) / 2.0f;
    Phi = 0.0;
    phi2 = phi1;
  }
  else if ( closeEnough(g[2][2], -1.0) )
  {
    phi1 = atan(g[0][1] / g[0][0]) / 2.0f;
    Phi = 180.0;
    phi2 = -phi1;
  }
  else
  {
    Phi = acos(g[2][2]);
    double s = sin(Phi);
    phi1 = atan2(g[2][0] / s, -g[2][1] / s );
    phi2 = atan2(g[0][2] / s, g[1][2] / s );
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::EulertoRod(float ea1, float ea2, float ea3, float& r1, float& r2, float& r3)
{
  float sum, diff, csum, cdiff, sdiff, t2;
  sum = (ea1 + ea3) * 0.5;
  diff = (ea1 - ea3) * 0.5;
  csum = cosf(sum);
  cdiff = cosf(diff);
  sdiff = sinf(diff);
  t2 = tanf(ea2 * 0.5);
  r1 = t2 * cdiff / csum;
  r2 = t2 * sdiff / csum;
  r3 = tanf(sum);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::EulerToAxisAngle(float ea1, float ea2, float ea3, float& w, float& n1, float& n2, float& n3)
{
  float r1, r2, r3;
  // First Convert to Rodrigues
  EulertoRod(ea1, ea2, ea3, r1, r2, r3);
  // Convert the Rodrigues to Axis Angle
  RodtoAxisAngle(r1, r2, r3, w, n1, n2, n3);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OrientationMath::MultiplyQuaternionVector(QuatF& inQuat, float* inVec, float* outVec)
{
  float g[3][3];
  float gInv[3][3];

  FOrientArrayType om(9, 0.0f);
  FOrientTransformsType::qu2om(FOrientArrayType(inQuat), om);
  om.toGMatrix(g);

  MatrixMath::Invert3x3(g, gInv);
  MatrixMath::Multiply3x3with3x1(gInv, inVec, outVec);
}

#endif

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
