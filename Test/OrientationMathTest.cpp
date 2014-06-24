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


#include "DREAM3DLib/Utilities/UnitTestSupport.hpp"
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


float outVec[3] = { 0.5f, 0.5f, 0.5f};
float inVec[3] = {1.0f, 1.0f, 1.0f };
float w = 1.0f;
float n1 = 1.0f;
float n2 = 1.0f;
float n3 = 1.0f;
float r1 = 1.0f;
float r2 = 1.0f;
float r3 = 1.0f;
float ea1 = 1.0f; float ea2 = 1.0f; float ea3 = 1.0f;
QuatF q = QuaternionMathF::New(0.0f, 0.0f, 1.0, 0.5);
float g[3][3] =   { {1.0, 0.0, 0.0},
                    {0.0, 1.0, 0.0},
                    {0.0, 0.0, 1.0}
                  };
float euler[3] = {0.7, -0.7f, -0.7f};
float x = 1.0; float y = 1.0; float z = 1.0;
float angle = 3.14;
float xAxis = 0.5; float yAxis = 0.5; float zAxis = 0.5;



void Print(std::ostream& o, QuatF &q)
{
  o << "{" << "<" << q.w << "> (" << q.x << ", " << q.y << ", " << q.z << ") }";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QuatF EulertoQuatActive(const float* e)
{
  QuatF q;
  float s, c, s1, c1, s2, c2;
  s = sinf(0.5f * e[1]);
  c = cosf(0.5f * e[1]);
  s1 = sinf(0.5f * (e[0] - e[2]));
  c1 = cosf(0.5f * (e[0] - e[2]));
  s2 = sinf(0.5f * (e[0] + e[2]));
  c2 = cosf(0.5f * (e[0] + e[2]));

  q.w = c * c2;

  if(q.w < 0.0)
  {
    q.w = -q.w;
    q.x = s * c1;
    q.y = s * s1;
    q.z = c * s2;
  }
  else
  {

    q.x = -s * c1;
    q.y = -s * s1;
    q.z = -c * s2;
  }
  return q;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestQuatMath()
{

  float euler[3] = {  2.72167, 3.148401, 0.148886 };
  printf("Euler Angle & %0.6f, %0.6f, %0.6f\n", euler[0], euler[1], euler[2]);

  QuatF q;
  OrientationMath::EulertoQuat(euler, q);

  printf("DREAM3D & %0.6f & %0.6f & %0.6f & %0.6f\n", q.w, q.x, q.y, q.z);

  q = EulertoQuatActive(euler);
  printf("MARC & %0.6f & %0.6f & %0.6f & %0.6f\n", q.w, q.x, q.y, q.z);

  OrientationMath::EulertoRod(euler[0], euler[1], euler[2], r1, r2, r3);
  printf("Euler to Rodrigues & %0.6f, %0.6f, %0.6f\n", r1, r2, r3);
  printf("Euler To Homochoric & MISSING\n");

  // Convert the Rodrigues to a Quaternion
  QuatF r2q;
  OrientationMath::RodtoQuat(r2q, r1,r2,r3);
  printf("--------------------------\n");
  printf("DREAM3D & %0.6f & %0.6f & %0.6f & %0.6f\n", r2q.w, r2q.x, r2q.y, r2q.z);
  OrientationMath::QuattoMat(r2q, g);

  printf("%0.6f & %0.6f & %0.6f\n", g[0][0] , g[0][1] , g[0][2]);
  printf("%0.6f & %0.6f & %0.6f\n", g[1][0] , g[1][1] , g[1][2]);
  printf("%0.6f & %0.6f & %0.6f\n", g[2][0] , g[2][1] , g[2][2]);
  printf("--------------------------\n");

  OrientationMath::EulertoMat(euler[0], euler[1], euler[2], g);
  //  printf("Euler to Passive Rotation Matrix & %0.6f  %0.6f  %0.6f\n", g[0][0] , g[0][1] , g[0][2]);
  //  printf("                                 & %0.6f  %0.6f  %0.6f\n", g[1][0] , g[1][1] , g[1][2]);
  //  printf("                                 & %0.6f  %0.6f  %0.6f\n", g[2][0] , g[2][1] , g[2][2]);


  printf("\\begin{equation}\n\\textbf{R}_{Passive} = \\begin{bmatrix}\n");
  printf("%0.6f & %0.6f & %0.6f\\\\\n", g[0][0] , g[0][1] , g[0][2]);
  printf("%0.6f & %0.6f & %0.6f\\\\\n", g[1][0] , g[1][1] , g[1][2]);
  printf("%0.6f & %0.6f & %0.6f\n", g[2][0] , g[2][1] , g[2][2]);
  printf("\\end{bmatrix}\n\\end{equation}\n");


  OrientationMath::EulertoMatActive(euler[0], euler[1], euler[2], g);
  //  printf("Euler to Active Rotation Matrix & %0.6f  %0.6f  %0.6f\n", g[0][0] , g[0][1] , g[0][2]);
  //  printf("                                &  %0.6f  %0.6f  %0.6f\n", g[1][0] , g[1][1] , g[1][2]);
  //  printf("                                &  %0.6f  %0.6f  %0.6f\n", g[2][0] , g[2][1] , g[2][2]);

  printf("\\begin{equation}\n\\textbf{R}_{Active} = \\begin{bmatrix}\n");
  printf("%0.6f & %0.6f & %0.6f\\\\\n", g[0][0] , g[0][1] , g[0][2]);
  printf("%0.6f & %0.6f & %0.6f\\\\\n", g[1][0] , g[1][1] , g[1][2]);
  printf("%0.6f & %0.6f & %0.6f\n", g[2][0] , g[2][1] , g[2][2]);
  printf("\\end{bmatrix}\n\\end{equation}\n");



  QuatF quat = OrientationMath::EulertoQuat(euler);
  quat = OrientationMath::EulertoQuat(euler[0], euler[1], euler[2]);

  float aa[4];
  OrientationMath::EulerToAxisAngle(euler[0], euler[1], euler[2], aa[0], aa[1], aa[2], aa[3]);
  printf("Euler to Axis Angle\n");
  printf("%0.8f, %0.8f, %0.8f, Angle=%0.8f\n", aa[1], aa[2], aa[3], aa[0]*DREAM3D::Constants::k_180OverPi);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestAxisAngle()
{
  OrientationMath::AxisAngletoHomochoric(w, n1, n2, n3, r1, r2, r3);
  OrientationMath::AxisAngletoRod(w, n1, n2, n3, r1, r2, r3);
  OrientationMath::AxisAngletoQuat(w, n1, n2, n3, q);
  OrientationMath::AxisAngletoMat(w, n1, n2, n3, g);
  OrientationMath::ChangeAxisReferenceFrame(q, n1, n2, n3);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestRodregues()
{

  OrientationMath::HomochorictoRod(r1, r2, r3);

  OrientationMath::RodtoAxisAngle(r1, r2, r3,  w, n1, n2, n3);
  OrientationMath::RodtoQuat(q, r1, r2, r3);
  OrientationMath::RodtoHomochoric(r1, r2, r3);
  OrientationMath::RodtoEuler(r1, r2, r3,  ea1,  ea2,  ea3);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestQuaternion()
{
  OrientationMath::QuattoAxisAngle(q,  w, n1, n2, n3);
  OrientationMath::QuattoMat(q, g);
  OrientationMath::QuattoRod(q, r1, r2, r3);
  OrientationMath::QuattoEuler(q,  ea1,  ea2,  ea3);
  OrientationMath::MultiplyQuaternionVector(q, inVec, outVec);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestEuler()
{
  OrientationMath::EulertoQuat(ea1, ea2, ea3, q);
  OrientationMath::EulertoQuat(euler, q);
  OrientationMath::EulertoMat(ea1, ea2, ea3, g);
  OrientationMath::EulertoRod(r1, r2, r3, ea1, ea2, ea3);


  OrientationMath::MattoEuler(g, ea1, ea2, ea3);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestMisc()
{
  QuatF passive = OrientationMath::PassiveRotation(angle, xAxis, yAxis, zAxis, x, y, z);
  Print(std::cout, passive);
  QuatF active = OrientationMath::ActiveRotation( angle, xAxis, yAxis, zAxis, x, y, z);
  Print(std::cout, active);
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




