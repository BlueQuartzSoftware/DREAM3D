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

#include <limits>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Math/QuaternionMath.hpp"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"

#include "OrientationLib/OrientationMath/OrientationMath.h"
#include "OrientationLib/OrientationMath/OrientationArray.hpp"
#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"
#include "OrientationLib/SpaceGroupOps/SpaceGroupOps.h"
#include "OrientationLib/SpaceGroupOps/CubicOps.h"


#include "OrientationLibTestFileLocations.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  // QFile::remove();
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestQuat_t()
{
  QuatF p = QuaternionMathF::New(1.0f, 0.0f, 0.0f, 1.0f);
  QuatF q = QuaternionMathF::New(0.0f, 1.0f, 0.0f, 2.0f);
  QuatF out = QuaternionMathF::New(0.0f, 0.0f, 0.0f, 0.0f);
  QuatF out2 = QuaternionMathF::New(10.0f, 20.0f, 30.0f, 40.0f);

  QuaternionMathF::Negate(out2);
  DREAM3D_REQUIRE_EQUAL(out2.x, -10.0)
  DREAM3D_REQUIRE_EQUAL(out2.y, -20.0)
  DREAM3D_REQUIRE_EQUAL(out2.z, -30.0)
  DREAM3D_REQUIRE_EQUAL(out2.w, -40.0)

  QuaternionMathF::Copy(p, out);
  DREAM3D_REQUIRE_EQUAL(p.x, out.x)
  DREAM3D_REQUIRE_EQUAL(p.y, out.y)
  DREAM3D_REQUIRE_EQUAL(p.z, out.z)
  DREAM3D_REQUIRE_EQUAL(p.w, out.w)

  QuaternionMathF::Identity(out);
  DREAM3D_REQUIRE_EQUAL(out.x, 0.0)
  DREAM3D_REQUIRE_EQUAL(out.y, 0.0)
  DREAM3D_REQUIRE_EQUAL(out.z, 0.0)
  DREAM3D_REQUIRE_EQUAL(out.w, 1.0)

  out = QuaternionMathF::New(-10.5f, -1.5f, -30.66f, -40.987f);
  QuaternionMathF::ElementWiseAbs(out);
  DREAM3D_REQUIRE_EQUAL(out.x, 10.5f)
  DREAM3D_REQUIRE_EQUAL(out.y, 1.5f)
  DREAM3D_REQUIRE_EQUAL(out.z, 30.66f)
  DREAM3D_REQUIRE_EQUAL(out.w, 40.987f)

  out = QuaternionMathF::New(10.0f, 20.0f, 30.0f, 40.0f);
  QuaternionMathF::ScalarMultiply(out, -1.0f);
  DREAM3D_REQUIRE_EQUAL(out.x, -10.0)
  DREAM3D_REQUIRE_EQUAL(out.y, -20.0)
  DREAM3D_REQUIRE_EQUAL(out.z, -30.0)
  DREAM3D_REQUIRE_EQUAL(out.w, -40.0)

  QuaternionMathF::ScalarDivide(out, -1.0f);

  QuaternionMathF::ScalarAdd(out, 50.0f);
  DREAM3D_REQUIRE_EQUAL(out.x, 60.0)
  DREAM3D_REQUIRE_EQUAL(out.y, 70.0)
  DREAM3D_REQUIRE_EQUAL(out.z, 80.0)
  DREAM3D_REQUIRE_EQUAL(out.w, 90.0)

  QuaternionMathF::ElementWiseAssign(out, 5.0f);
  DREAM3D_REQUIRE_EQUAL(out.x, 5.0)
  DREAM3D_REQUIRE_EQUAL(out.y, 5.0)
  DREAM3D_REQUIRE_EQUAL(out.z, 5.0)
  DREAM3D_REQUIRE_EQUAL(out.w, 5.0)

  QuaternionMathF::Negate(out);


  QuaternionMathF::Add(p, q, out);
  QuaternionMathF::Subtract(p, q, out);



  // Conjugate Tests where conjugate of a Quaternion is q*
  // (q*)* = q

  p.x = 1.0f;
  p.y = 2.0f;
  p.z = 3.0f;
  p.w = 1.0f;
  QuaternionMathF::Conjugate(p);
  DREAM3D_REQUIRE_EQUAL(p.x, -1.0)
  DREAM3D_REQUIRE_EQUAL(p.y, -2.0)
  DREAM3D_REQUIRE_EQUAL(p.z, -3.0)
  DREAM3D_REQUIRE_EQUAL(p.w, 1.0)
  QuaternionMathF::Conjugate(p);
  DREAM3D_REQUIRE_EQUAL(p.x, 1.0)
  DREAM3D_REQUIRE_EQUAL(p.y, 2.0)
  DREAM3D_REQUIRE_EQUAL(p.z, 3.0)
  DREAM3D_REQUIRE_EQUAL(p.w, 1.0)

  // (pq)* = q*p*
  q.x = 1.0f;
  q.y = 0.0f;
  q.z = 1.0f;
  q.w = 1.0f;
  QuaternionMathF::Multiply(p, q, out);
  QuaternionMathF::Conjugate(out);
  QuaternionMathF::Conjugate(p);
  QuaternionMathF::Conjugate(q);
  QuaternionMathF::Multiply(q, p, out2);
  DREAM3D_REQUIRE_EQUAL(out.x, out2.x)
  DREAM3D_REQUIRE_EQUAL(out.y, out2.y)
  DREAM3D_REQUIRE_EQUAL(out.z, out2.z)
  DREAM3D_REQUIRE_EQUAL(out.w, out2.w)

  //(p+q)* = p*+q*
  p.x = 1.0f;
  p.y = 2.0f;
  p.z = 3.0f;
  p.w = 1.0f;
  q.x = 1.0f;
  q.y = 0.0f;
  q.z = 1.0f;
  q.w = 1.0f;
  QuaternionMathF::Add(p, q, out);
  QuaternionMathF::Conjugate(out);
  QuaternionMathF::Conjugate(p);
  QuaternionMathF::Conjugate(q);
  QuaternionMathF::Add(p, q, out2);
  DREAM3D_REQUIRE_EQUAL(out.x, out2.x)
  DREAM3D_REQUIRE_EQUAL(out.y, out2.y)
  DREAM3D_REQUIRE_EQUAL(out.z, out2.z)
  DREAM3D_REQUIRE_EQUAL(out.w, out2.w)

  // Multiplication Test
  // pq != qp
  p.x = 1.0f;
  p.y = 0.0f;
  p.z = 0.0f;
  p.w = 1.0f;

  q.x = 0.0f;
  q.y = 1.0f;
  q.z = 0.0f;
  q.w = 2.0f;

  QuaternionMathF::Multiply(p, q, out);
  DREAM3D_REQUIRE_EQUAL(out.x, 2.0)
  DREAM3D_REQUIRE_EQUAL(out.y, 1.0)
  DREAM3D_REQUIRE_EQUAL(out.z, 1.0)
  DREAM3D_REQUIRE_EQUAL(out.w, 2.0)

  QuaternionMathF::Multiply(q, p, out);
  DREAM3D_REQUIRE_EQUAL(out.x, 2.0)
  DREAM3D_REQUIRE_EQUAL(out.y, 1.0)
  DREAM3D_REQUIRE_EQUAL(out.z, -1.0)
  DREAM3D_REQUIRE_EQUAL(out.w, 2.0)

  // Norm Test
  // N(q*) = N(q)
  p.x = 1.0f;
  p.y = 0.0f;
  p.z = 0.0f;
  p.w = 1.0f;
  q.x = 0.0f;
  q.y = 1.0f;
  q.z = 0.0f;
  q.w = 2.0f;
  float norm = QuaternionMathF::Norm(p);
  QuaternionMathF::Conjugate(p);
  float cnorm =  QuaternionMathF::Norm(p);
  DREAM3D_REQUIRE_EQUAL(norm, cnorm)

  // Length and Unit Quaternion Tests
  p.x = 2.0f;
  p.y = 2.0f;
  p.z = 2.0f;
  p.w = 2.0f;
  float length = QuaternionMathF::Length(p);
  DREAM3D_REQUIRE_EQUAL(length, 4.0);
  QuaternionMathF::UnitQuaternion(p);
  DREAM3D_REQUIRE_EQUAL(p.x, 0.5)
  DREAM3D_REQUIRE_EQUAL(p.y, 0.5)
  DREAM3D_REQUIRE_EQUAL(p.z, 0.5)
  DREAM3D_REQUIRE_EQUAL(p.w, 0.5)


  float vec[3] = { 0.0f, 0.0f, 0.0f};
  float ovec[3] = { 0.0f, 0.0f, 0.0f};

  QuaternionMathF::GetMisorientationVector(p, vec);
  QuaternionMathF::MultiplyQuatVec(q, vec, ovec);
}

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  int err = EXIT_SUCCESS;
  DREAM3D_REGISTER_TEST( TestQuat_t() )
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  PRINT_TEST_SUMMARY();

  return err;
}
