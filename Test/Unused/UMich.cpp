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


#include <stdlib.h>
#include <math.h>

#include <iostream>

#include "SIMPLib/Math/SIMPLibMath.h"

static const float CubicQuatSym[24][4] =
{
  { 0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f},
  { 1.000000000f, 0.000000000f, 0.000000000f, 0.000000000f},
  { 0.000000000f, 1.000000000f, 0.000000000f, 0.000000000f},
  { 0.000000000f, 0.000000000f, 1.000000000f, 0.000000000f},
  { SIMPLib::Constants::k_1OverRoot2, 0.000000000f, 0.000000000f, SIMPLib::Constants::k_1OverRoot2},
  { 0.000000000f, SIMPLib::Constants::k_1OverRoot2, 0.000000000f, SIMPLib::Constants::k_1OverRoot2},
  { 0.000000000f, 0.000000000f, SIMPLib::Constants::k_1OverRoot2, SIMPLib::Constants::k_1OverRoot2},
  { -SIMPLib::Constants::k_1OverRoot2, 0.000000000f, 0.000000000f, SIMPLib::Constants::k_1OverRoot2},
  { 0.000000000f, -SIMPLib::Constants::k_1OverRoot2, 0.000000000f, SIMPLib::Constants::k_1OverRoot2},
  { 0.000000000f, 0.000000000f, -SIMPLib::Constants::k_1OverRoot2, SIMPLib::Constants::k_1OverRoot2},
  { SIMPLib::Constants::k_1OverRoot2, SIMPLib::Constants::k_1OverRoot2, 0.000000000f, 0.000000000f},
  { -SIMPLib::Constants::k_1OverRoot2, SIMPLib::Constants::k_1OverRoot2, 0.000000000f, 0.000000000f},
  { 0.000000000f, SIMPLib::Constants::k_1OverRoot2, SIMPLib::Constants::k_1OverRoot2, 0.000000000f},
  { 0.000000000f, -SIMPLib::Constants::k_1OverRoot2, SIMPLib::Constants::k_1OverRoot2, 0.000000000f},
  { SIMPLib::Constants::k_1OverRoot2, 0.000000000f, SIMPLib::Constants::k_1OverRoot2, 0.000000000f},
  { -SIMPLib::Constants::k_1OverRoot2, 0.000000000f, SIMPLib::Constants::k_1OverRoot2, 0.000000000f},
  { 0.500000000f, 0.500000000f, 0.500000000f, 0.500000000f},
  { -0.500000000f, -0.500000000f, -0.500000000f, 0.500000000f},
  { 0.500000000f, -0.500000000f, 0.500000000f, 0.500000000f},
  { -0.500000000f, 0.500000000f, -0.500000000f, 0.500000000f},
  { -0.500000000f, 0.500000000f, 0.500000000f, 0.500000000f},
  { 0.500000000f, -0.500000000f, -0.500000000f, 0.500000000f},
  { -0.500000000f, -0.500000000f, 0.500000000f, 0.500000000f},
  { 0.500000000f, 0.500000000f, -0.500000000f, 0.500000000f}
};

/**
 * @brief eulertoQuat This converts an Euler Angle to a Quaternion
 * @param q The output Quaterion
 * @param ea1 Input Euler Angle 1 (phi1)
 * @param ea2 Input Euler Angle 2 (Phi)
 * @param ea3 Input Euler Angle 3 (phi2)
 */
void EulertoQuat(float* q, float ea1, float ea2, float ea3)
{
  float s, c, s1, c1, s2, c2;
  s = sinf(0.5f * ea2);
  c = cosf(0.5f * ea2);
  s1 = sinf(0.5f * (ea1 - ea3));
  c1 = cosf(0.5f * (ea1 - ea3));
  s2 = sinf(0.5f * (ea1 + ea3));
  c2 = cosf(0.5f * (ea1 + ea3));
  q[0] = s * c1;
  q[1] = s * s1;
  q[2] = c * s2;
  q[3] = c * c2;
}

/**
 * @brief QuattoEuler This converts a Quaternion to a Euler Angle
 * @param q Input Quaternion (1x4 Vector)
 * @param ea1 Output Euler Angle 1 (phi1)
 * @param ea2 Output Euler Angle 2 (Phi)
 * @param ea3 Output Euler Angle 3 (phi2)
 */
void QuattoEuler(float* q, float& ea1, float& ea2, float& ea3)
{
  float diff, sum, tmp;
  diff = atan2(q[1], q[0]);
  sum = atan2(q[2], q[3]);
  ea1 = (diff + sum);
  ea3 = (sum - diff);
  tmp = (q[2] * q[2]) + (q[3] * q[3]);
  tmp = sqrt(tmp);
  if(tmp > 1.0f) { tmp = 1.0f; }
  ea2 = 2 * acos(tmp);
}


/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main (int argc, char** argv)
{
  if (argc != 4)
  {
    std::cout << "This program needs 3 Euler Angles as arguments" << std::endl;
    return EXIT_FAILURE;
  }
  // Gather the arguments from the command line and convert them into floating point values
  float ea0 = static_cast<float>(atof(argv[1]));
  float ea1 = static_cast<float>(atof(argv[2]));
  float ea2 = static_cast<float>(atof(argv[3]));

  std::cout << "Input Angle:\n  ";
  std::cout << ea0 << ", " << ea1 << ", " << ea2 << std::endl;
  std::cout << "\nOutput Euler Angles" << std::endl;
  // We need to convert the Euler Angles into a Quaternion for the Math to work correctly.
  float q[4]; // Quaterion for use to use

  // Convert the Euler Angle to a Quaternion
  EulertoQuat(q, ea0, ea1, ea2);

  // Create a variable to hold the tranform each time through the loop of 24 symmetries
  float out[4];

  // This stores the Euler Angle for each of the 24 Symmetries. You might want to store these in an array or something
  // like that. Here we will just print them out.
  float outEa0, outEa1, outEa2;

  // Loop on all 24 Symmetries. Note that the first symmetry will give you back the input angle so you can use this
  // as a check to make sure things are going correctly.
  for (int i = 0; i < 24; ++i)
  {
    out[0] = CubicQuatSym[i][3] * q[0] + CubicQuatSym[i][0] * q[3] + CubicQuatSym[i][2] * q[1] - CubicQuatSym[i][1] * q[2];
    out[1] = CubicQuatSym[i][3] * q[1] + CubicQuatSym[i][1] * q[3] + CubicQuatSym[i][0] * q[2] - CubicQuatSym[i][2] * q[0];
    out[2] = CubicQuatSym[i][3] * q[2] + CubicQuatSym[i][2] * q[3] + CubicQuatSym[i][1] * q[0] - CubicQuatSym[i][0] * q[1];
    out[3] = CubicQuatSym[i][3] * q[3] - CubicQuatSym[i][0] * q[0] - CubicQuatSym[i][1] * q[1] - CubicQuatSym[i][2] * q[2];

    QuattoEuler(out, outEa0, outEa1, outEa2);
    std::cout << "[" << i << "]:    " << outEa0 << ", " << outEa1 << ", " << outEa2 << std::endl;
  }


  return EXIT_SUCCESS;
}
