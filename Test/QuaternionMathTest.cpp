/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Math/QuaternionMath.hpp"
#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"

#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
 // MXADir::remove();
#endif
}

void TestCubicOps()
{
   float e0[3] = {0.0f, 0.0f, 0.0f};
   float e1[3] = {0.0f, 0.0f, 30.0f * M_PI/180.f};

   QuaternionMath<float>::Quaternion q0 = QuaternionMath<float>::New(0.0f, 0.0f, 0.0f, 0.0f);
   QuaternionMath<float>::Quaternion q1 = QuaternionMath<float>::New(0.0f, 0.0f, 0.0f, 0.0f);

   OrientationMath<float>::EulertoQuat(q0, e0[0], e0[1], e0[2]);
   OrientationMath<float>::EulertoQuat(q1, e1[0], e1[1], e1[2]);

   CubicOps co;

    float n[3] = {0.0, 0.0, 1.0f};
    float w = co.getMisoQuat(q0, q1, n[0], n[1], n[2]);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestQuat_t()
{



  QuatF p = QuaternionMath<float>::New(1.0f, 0.0f, 0.0f, 1.0f);
  QuatF q = QuaternionMath<float>::New(0.0f, 1.0f, 0.0f, 2.0f);
  QuatF out = QuaternionMath<float>::New(0.0f, 0.0f, 0.0f, 0.0f);
  QuatF out2 = QuaternionMath<float>::New(10.0f, 20.0f, 30.0f, 40.0f);

  QuaternionMath<float>::Negate(out2);
  DREAM3D_REQUIRE_EQUAL(out2.x, -10.0)
  DREAM3D_REQUIRE_EQUAL(out2.y, -20.0)
  DREAM3D_REQUIRE_EQUAL(out2.z, -30.0)
  DREAM3D_REQUIRE_EQUAL(out2.w, -40.0)

  QuaternionMath<float>::Copy(p, out);
  DREAM3D_REQUIRE_EQUAL(p.x, out.x)
  DREAM3D_REQUIRE_EQUAL(p.y, out.y)
  DREAM3D_REQUIRE_EQUAL(p.z, out.z)
  DREAM3D_REQUIRE_EQUAL(p.w, out.w)

  QuaternionMath<float>::Identity(out);
  DREAM3D_REQUIRE_EQUAL(out.x, 0.0)
  DREAM3D_REQUIRE_EQUAL(out.y, 0.0)
  DREAM3D_REQUIRE_EQUAL(out.z, 0.0)
  DREAM3D_REQUIRE_EQUAL(out.w, 1.0)

  out = QuaternionMath<float>::New(-10.5f, -1.5f, -30.66f, -40.987f);
  QuaternionMath<float>::ElementWiseAbs(out);
  DREAM3D_REQUIRE_EQUAL(out.x, 10.5f)
  DREAM3D_REQUIRE_EQUAL(out.y, 1.5f)
  DREAM3D_REQUIRE_EQUAL(out.z, 30.66f)
  DREAM3D_REQUIRE_EQUAL(out.w, 40.987f)

  out = QuaternionMath<float>::New(10.0f, 20.0f, 30.0f, 40.0f);
  QuaternionMath<float>::ScalarMultiply(out, -1.0f);
  DREAM3D_REQUIRE_EQUAL(out.x, -10.0)
  DREAM3D_REQUIRE_EQUAL(out.y, -20.0)
  DREAM3D_REQUIRE_EQUAL(out.z, -30.0)
  DREAM3D_REQUIRE_EQUAL(out.w, -40.0)

  QuaternionMath<float>::ElementWiseAssign(out, 5.0f);
  DREAM3D_REQUIRE_EQUAL(out.x, 5.0)
  DREAM3D_REQUIRE_EQUAL(out.y, 5.0)
  DREAM3D_REQUIRE_EQUAL(out.z, 5.0)
  DREAM3D_REQUIRE_EQUAL(out.w, 5.0)


  QuaternionMath<float>::ScalarAdd(out, 50.0f);
  DREAM3D_REQUIRE_EQUAL(out.x, 55.0)
  DREAM3D_REQUIRE_EQUAL(out.y, 55.0)
  DREAM3D_REQUIRE_EQUAL(out.z, 55.0)
  DREAM3D_REQUIRE_EQUAL(out.w, 55.0)

// Conjugate Tests where conjugate of a Quaternion is q*
// (q*)* = q

  p.x = 1.0f; p.y = 2.0f; p.z = 3.0f; p.w = 1.0f;
  QuaternionMath<float>::Conjugate(p);
  DREAM3D_REQUIRE_EQUAL(p.x, -1.0)
  DREAM3D_REQUIRE_EQUAL(p.y, -2.0)
  DREAM3D_REQUIRE_EQUAL(p.z, -3.0)
  DREAM3D_REQUIRE_EQUAL(p.w, 1.0)
  QuaternionMath<float>::Conjugate(p);
  DREAM3D_REQUIRE_EQUAL(p.x, 1.0)
  DREAM3D_REQUIRE_EQUAL(p.y, 2.0)
  DREAM3D_REQUIRE_EQUAL(p.z, 3.0)
  DREAM3D_REQUIRE_EQUAL(p.w, 1.0)

// (pq)* = q*p*
  q.x = 1.0f; q.y = 0.0f; q.z = 1.0f; q.w = 1.0f;
  QuaternionMath<float>::Multiply(p, q, out);
  QuaternionMath<float>::Conjugate(out);
  QuaternionMath<float>::Conjugate(p);
  QuaternionMath<float>::Conjugate(q);
  QuaternionMath<float>::Multiply(q, p, out2);
  DREAM3D_REQUIRE_EQUAL(out.x, out2.x)
  DREAM3D_REQUIRE_EQUAL(out.y, out2.y)
  DREAM3D_REQUIRE_EQUAL(out.z, out2.z)
  DREAM3D_REQUIRE_EQUAL(out.w, out2.w)

//(p+q)* = p*+q*
  p.x = 1.0f; p.y = 2.0f; p.z = 3.0f; p.w = 1.0f;
  q.x = 1.0f; q.y = 0.0f; q.z = 1.0f; q.w = 1.0f;
  QuaternionMath<float>::Add(p, q, out);
  QuaternionMath<float>::Conjugate(out);
  QuaternionMath<float>::Conjugate(p);
  QuaternionMath<float>::Conjugate(q);
  QuaternionMath<float>::Add(p, q, out2);
  DREAM3D_REQUIRE_EQUAL(out.x, out2.x)
  DREAM3D_REQUIRE_EQUAL(out.y, out2.y)
  DREAM3D_REQUIRE_EQUAL(out.z, out2.z)
  DREAM3D_REQUIRE_EQUAL(out.w, out2.w)

// Multiplication Test
// pq != qp
  p.x = 1.0f; p.y = 0.0f; p.z = 0.0f; p.w = 1.0f;
  q.x = 0.0f; q.y = 1.0f; q.z = 0.0f; q.w = 2.0f;

  QuaternionMath<float>::Multiply(p, q, out);
  DREAM3D_REQUIRE_EQUAL(out.x, 2.0)
  DREAM3D_REQUIRE_EQUAL(out.y, 1.0)
  DREAM3D_REQUIRE_EQUAL(out.z, 1.0)
  DREAM3D_REQUIRE_EQUAL(out.w, 2.0)

  QuaternionMath<float>::Multiply(q, p, out);
  DREAM3D_REQUIRE_EQUAL(out.x, 2.0)
  DREAM3D_REQUIRE_EQUAL(out.y, 1.0)
  DREAM3D_REQUIRE_EQUAL(out.z, -1.0)
  DREAM3D_REQUIRE_EQUAL(out.w, 2.0)

// Norm Test
// N(q*) = N(q)
  p.x = 1.0f; p.y = 0.0f; p.z = 0.0f; p.w = 1.0f;
  q.x = 0.0f; q.y = 1.0f; q.z = 0.0f; q.w = 2.0f;
  float norm = QuaternionMath<float>::Norm(p);
  QuaternionMath<float>::Conjugate(p);
  float cnorm =  QuaternionMath<float>::Norm(p);
  DREAM3D_REQUIRE_EQUAL(norm, cnorm)

// Length and Unit Quaternion Tests
  p.x = 2.0f; p.y = 2.0f; p.z = 2.0f; p.w = 2.0f;
  float length = QuaternionMath<float>::Length(p);
  DREAM3D_REQUIRE_EQUAL(length, 4.0);
  QuaternionMath<float>::UnitQuaternion(p);
  DREAM3D_REQUIRE_EQUAL(p.x, 0.5)
  DREAM3D_REQUIRE_EQUAL(p.y, 0.5)
  DREAM3D_REQUIRE_EQUAL(p.z, 0.5)
  DREAM3D_REQUIRE_EQUAL(p.w, 0.5)

}

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST( TestQuat_t() )
  DREAM3D_REGISTER_TEST( TestCubicOps() )

  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  PRINT_TEST_SUMMARY();
  return err;
}
