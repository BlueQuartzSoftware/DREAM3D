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




#include "DREAM3DLib/Math/QuaternionMath.hpp"
#include "DREAM3DLib/Math/OrientationMath.h"


#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"

/*
 * IDL> print, eulers[*,0]
     2.72167     0.148401     0.148886

IDL> print, eulerang2quat(eulers[*,0])
    0.134731   -0.0208004   -0.0711543    -0.988105

IDL> print, makeeulerrot(eulers[*,0])
   -0.962830     0.269217    0.0219326
   -0.263297    -0.953569     0.146221
   0.0602794     0.135011     0.989009

IDL> print, makeeulerrot(eulers[*,0]) ## [[1.,2.,3.]]
   -0.358598
    -1.73177
     3.29733

IDL> print, quat_vector(eulerang2quat(eulers[*,0]), [1,2,3.])
   -0.358598     -1.73177      3.29733
   */

void Print(std::ostream& o, QuatF &q)
{
  o << "{" << "<" << q.w << "> (" << q.x << ", " << q.y << ", " << q.z << ") }";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestQuatMath()
{

  float euler[3] = {  2.72167, 0.148401, 0.148886 };
  QuatF q;
  OrientationMath::EulertoQuat(q, euler);
  Print(std::cout, q);
  std::cout << "" << std::endl;

  float g[3][3];
  OrientationMath::EulertoMat(euler[0], euler[1], euler[2], g);
  std::cout << g[0][0] <<  " " << g[0][1] << " " << g[0][2] << std::endl;
  std::cout << g[1][0] <<  " " << g[1][1] << " " << g[1][2] << std::endl;
  std::cout << g[2][0] <<  " " << g[2][1] << " " << g[2][2] << std::endl;


}




// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  int err = EXIT_SUCCESS;

  DREAM3D_REGISTER_TEST( TestQuatMath() )

      PRINT_TEST_SUMMARY();
  return err;
}




