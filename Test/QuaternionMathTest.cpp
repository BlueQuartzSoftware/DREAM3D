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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestQuat_t()
{



  QuatF q1 = QuaternionMathF::New(1.0f, 0.0f, 0.0f, 1.0f);
  QuatF q2 = QuaternionMathF::New(0.0f, 1.0f, 0.0f, 2.0f);
  QuatF out = QuaternionMathF::New(0.0f, 0.0f, 0.0f, 0.0f);



  QuaternionMathF::Copy(q1, out);
  DREAM3D_REQUIRE_EQUAL(q1.x, out.x)
  DREAM3D_REQUIRE_EQUAL(q1.y, out.y)
  DREAM3D_REQUIRE_EQUAL(q1.z, out.z)
  DREAM3D_REQUIRE_EQUAL(q1.w, out.w)

  QuaternionMathF::Identity(out);
  DREAM3D_REQUIRE_EQUAL(out.x, 0.0)
  DREAM3D_REQUIRE_EQUAL(out.y, 0.0)
  DREAM3D_REQUIRE_EQUAL(out.z, 0.0)
  DREAM3D_REQUIRE_EQUAL(out.w, 1.0)

  out = QuaternionMathF::New(-10.0f, -20.0f, -30.0f, -40.0f);
  QuaternionMathF::ElementWiseAbs(out);
  DREAM3D_REQUIRE_EQUAL(out.x, 10.0)
  DREAM3D_REQUIRE_EQUAL(out.y, 20.0)
  DREAM3D_REQUIRE_EQUAL(out.z, 30.0)
  DREAM3D_REQUIRE_EQUAL(out.w, 40.0)


  QuaternionMathF::ElementWiseMultiply(out, -1.0f);
  DREAM3D_REQUIRE_EQUAL(out.x, -10.0)
  DREAM3D_REQUIRE_EQUAL(out.y, -20.0)
  DREAM3D_REQUIRE_EQUAL(out.z, -30.0)
  DREAM3D_REQUIRE_EQUAL(out.w, -40.0)

  QuaternionMathF::ElementWiseAssign(out, 5.0f);
  DREAM3D_REQUIRE_EQUAL(out.x, 5.0)
  DREAM3D_REQUIRE_EQUAL(out.y, 5.0)
  DREAM3D_REQUIRE_EQUAL(out.z, 5.0)
  DREAM3D_REQUIRE_EQUAL(out.w, 5.0)


  QuaternionMathF::ElementWiseAdd(out, 50.0f);
  DREAM3D_REQUIRE_EQUAL(out.x, 55.0)
  DREAM3D_REQUIRE_EQUAL(out.y, 55.0)
  DREAM3D_REQUIRE_EQUAL(out.z, 55.0)
  DREAM3D_REQUIRE_EQUAL(out.w, 55.0)

  QuaternionMathF::InvertQuaternion(out);
  DREAM3D_REQUIRE_EQUAL(out.x, -55.0)
  DREAM3D_REQUIRE_EQUAL(out.y, -55.0)
  DREAM3D_REQUIRE_EQUAL(out.z, -55.0)
  DREAM3D_REQUIRE_EQUAL(out.w, 55.0)


  QuaternionMathF::Multiply(q1, q2, out);
  DREAM3D_REQUIRE_EQUAL(out.x, 2.0)
  DREAM3D_REQUIRE_EQUAL(out.y, 1.0)
  DREAM3D_REQUIRE_EQUAL(out.z, 1.0)
  DREAM3D_REQUIRE_EQUAL(out.w, 2.0)

  QuaternionMathF::Multiply(q2, q1, out);
  DREAM3D_REQUIRE_EQUAL(out.x, 2.0)
  DREAM3D_REQUIRE_EQUAL(out.y, 1.0)
  DREAM3D_REQUIRE_EQUAL(out.z, -1.0)
  DREAM3D_REQUIRE_EQUAL(out.w, 2.0)

  QuaternionMathF::Normalize(out);
}

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST( TestQuat_t() )

  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  PRINT_TEST_SUMMARY();
  return err;
}
