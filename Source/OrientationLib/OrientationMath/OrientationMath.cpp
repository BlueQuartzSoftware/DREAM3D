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
