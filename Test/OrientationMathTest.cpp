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





#include "DREAM3DLib/OrientationOps/OrientationOps.h"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3DLib/OrientationOps/TrigonalOps.h"

#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestQuatMath()
{

  float q1[5] = { 0.0f, 0.0, 0.0, 0.22495, .97437};
  float q2[5] = { 0.0f, .343, -.2185, .90933, 0.08756 };
  float out[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

  MULT_QUAT(q1, q2, out);


  std::cout << "Done" << std::endl;
}


void thereAndBack(float e[3], float R[3][3], bool debug=false)
{
  float g[3][3];
  float gNew[3][3];
  float eTrans[3] = { 0.0, 0.0, 0.0 };
  std::cout << "---------------------------------------------------------" << std::endl;
  printf("ORIG   : %+6.6f\t%+6.6f\t%+6.6f\n", e[0], e[1], e[2]);

  OrientationMath::EulerToMat(e[0], e[1], e[2], g); // Convert the Euler into a Rotation Matrix
  if(debug) {
    std::cout << "Rotation Matrix (g) based off Euler Angle ("
              << e[0] << " " << e[1] << " "  << e[2] << ")" << std::endl;
    std::cout << g[0][0] << "\t" << g[0][1] << "\t"  << g[0][2] << std::endl;
    std::cout << g[1][0] << "\t" << g[1][1] << "\t"  << g[1][2] << std::endl;
    std::cout << g[2][0] << "\t" << g[2][1] << "\t"  << g[2][2] << std::endl;
  }

  OrientationMath::MatToEuler(g, eTrans[0], eTrans[1], eTrans[2]); // COnvert BACK to a Euler Angle
  printf("   To RotMatrix and back\n");
  printf("DREAM3D: %+6.6f\t%+6.6f\t%+6.6f\n", eTrans[0], eTrans[1], eTrans[2]);
  OrientationMath::MatToEuler(g, eTrans[0], eTrans[1], eTrans[2]);
  printf("TONY   : %+6.6f\t%+6.6f\t%+6.6f\n", eTrans[0], eTrans[1], eTrans[2]);


  MatrixMath::Multiply3x3with3x3(g, R, gNew); // Multiply the Pair of Rotation Matrix together
  if(debug) {
    std::cout << "Multiply Pair of Rotation Matrix together Results in gNew" << std::endl;
    std::cout << gNew[0][0] << "\t" << gNew[0][1] << "\t"  << gNew[0][2] << std::endl;
    std::cout << gNew[1][0] << "\t" << gNew[1][1] << "\t"  << gNew[1][2] << std::endl;
    std::cout << gNew[2][0] << "\t" << gNew[2][1] << "\t"  << gNew[2][2] << std::endl;
  }

  MatrixMath::Normalize3x3(gNew); // Normalize the Result
  if(debug) {
    std::cout << "Normalizing gNew:" << std::endl;
    std::cout << gNew[0][0] << "\t" << gNew[0][1] << "\t"  << gNew[0][2] << std::endl;
    std::cout << gNew[1][0] << "\t" << gNew[1][1] << "\t"  << gNew[1][2] << std::endl;
    std::cout << gNew[2][0] << "\t" << gNew[2][1] << "\t"  << gNew[2][2] << std::endl;
  }


  printf(" Transformed Angles from gNew \n");
  OrientationMath::MatToEuler(gNew, eTrans[0], eTrans[1], eTrans[2]); // COnvert BACK to a Euler Angle
  printf("DREAM3D: %+6.6f\t%+6.6f\t%+6.6f\n", eTrans[0], eTrans[1], eTrans[2]);

  OrientationMath::MatToEuler(gNew, eTrans[0], eTrans[1], eTrans[2]);
  printf("TONY   : %+6.6f\t%+6.6f\t%+6.6f\n", eTrans[0], eTrans[1], eTrans[2]);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void testEulerRotMatrix()
{

  float R[3][3];
  float angle = M_PI;
  float axis[3] = {0.0, 1.0, 0.0};
  OrientationMath::AxisAngletoMat(angle, axis[0], axis[1], axis[2], R);

  std::cout << "Rotation Matrix based off of " << angle << " about <"
            << axis[0] << " " << axis[1] << " "  << axis[2] << ">" << std::endl;
  std::cout << R[0][0] << "\t" << R[0][1] << "\t"  << R[0][2] << std::endl;
  std::cout << R[1][0] << "\t" << R[1][1] << "\t"  << R[1][2] << std::endl;
  std::cout << R[2][0] << "\t" << R[2][1] << "\t"  << R[2][2] << std::endl;


  float test0[3] = {2.82795691, 0.76504761, 0.000287979317}; // NAN in third position
  thereAndBack(test0, R);
  float test1[3] = {6.28283644, 0.749985456, 3.1684823}; // NAN in first position
  thereAndBack(test1, R);
  float test2[3] = {3.14141822, 1.68939137, 4.70505857}; // NAN in first position
 thereAndBack(test2, R);

#if 0
  OrientationMath::EulertoMat(e[0], e[1], e[2], R);
  std::cout << "Euler Angle" << std::endl;
  std::cout << e[0] << "\t" << e[1] << "\t"  << e[2] << std::endl;
#endif


#if 0
  OrientationMath::MattoEuler(R, e[0], e[1], e[2]);
  std::cout << "Euler Angle" << std::endl;
  std::cout << e[0] << "\t" << e[1] << "\t"  << e[2] << std::endl;
#endif
}



// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv) {
  int err = EXIT_SUCCESS;

  DREAM3D_REGISTER_TEST( testEulerRotMatrix() )

      DREAM3D_REGISTER_TEST( TestQuatMath() )

      PRINT_TEST_SUMMARY();
  return err;
}




