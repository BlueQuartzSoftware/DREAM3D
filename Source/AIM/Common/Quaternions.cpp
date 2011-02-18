/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "Quaternions.h"


using namespace AIM;

/**
 * @brief Initialize our Symmetry Arrays
 */

double Quaternions::quat_symmcubic[24][5] = {
 {0.000000000, 0.000000000, 0.000000000, 0.000000000, 1.000000000},
 {0.000000000, 1.000000000, 0.000000000, 0.000000000, 0.000000000},
 {0.000000000, 0.000000000, 1.000000000, 0.000000000, 0.000000000},
 {0.000000000, 0.000000000, 0.000000000, 1.000000000, 0.000000000},
 {0.000000000, 0.707106781, 0.000000000, 0.000000000, 0.707106781},
 {0.000000000, 0.000000000, 0.707106781, 0.000000000, 0.707106781},
 {0.000000000, 0.000000000, 0.000000000, 0.707106781, 0.707106781},
 {0.000000000, -0.707106781, 0.000000000, 0.000000000, 0.707106781},
 {0.000000000, 0.000000000, -0.707106781, 0.000000000, 0.707106781},
 {0.000000000, 0.000000000, 0.000000000, -0.707106781, 0.707106781},
 {0.000000000, 0.707106781, 0.707106781, 0.000000000, 0.000000000},
 {0.000000000, -0.707106781, 0.707106781, 0.000000000, 0.000000000},
 {0.000000000, 0.000000000, 0.707106781, 0.707106781, 0.000000000},
 {0.000000000, 0.000000000, -0.707106781, 0.707106781, 0.000000000},
 {0.000000000, 0.707106781, 0.000000000, 0.707106781, 0.000000000},
 {0.000000000, -0.707106781, 0.000000000, 0.707106781, 0.000000000},
 {0.000000000, 0.500000000, 0.500000000, 0.500000000, 0.500000000},
 {0.000000000, -0.500000000, -0.500000000, -0.500000000, 0.500000000},
 {0.000000000, 0.500000000, -0.500000000, 0.500000000, 0.500000000},
 {0.000000000, -0.500000000, 0.500000000, -0.500000000, 0.500000000},
 {0.000000000, -0.500000000, 0.500000000, 0.500000000, 0.500000000},
 {0.000000000, 0.500000000, -0.500000000, -0.500000000, 0.500000000},
 {0.000000000, -0.500000000, -0.500000000, 0.500000000, 0.500000000},
 {0.000000000, 0.500000000, 0.500000000, -0.500000000, 0.500000000}
};

double Quaternions::quat_symmhex[12][5] = {
 {0.000000000, 0.000000000, 0.000000000, 0.000000000, 1.000000000},
 {0.000000000, 0.000000000, 0.000000000, 0.500000000, 0.866025400},
 {0.000000000, 0.000000000, 0.000000000, 0.866025400, 0.500000000},
 {0.000000000, 0.000000000, 0.000000000, 1.000000000, 0.000000000},
 {0.000000000, 0.000000000, 0.000000000, 0.866025400, -0.50000000},
 {0.000000000, 0.000000000, 0.000000000, 0.500000000, -0.86602540},
 {0.000000000, 1.000000000, 0.000000000, 0.000000000, 0.000000000},
 {0.000000000, 0.866025400, 0.500000000, 0.000000000, 0.000000000},
 {0.000000000, 0.500000000, 0.866025400, 0.000000000, 0.000000000},
 {0.000000000, 0.000000000, 1.000000000, 0.000000000, 0.000000000},
 {0.000000000, -0.50000000, 0.866025400, 0.000000000, 0.000000000},
 {0.000000000, -0.86602540, 0.500000000, 0.000000000, 0.000000000}
};



#define QUAT_SYMMHEX AIM::Quaternions::quat_symmhex
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Quaternions::Hex_MultiplyByUnitQuaterion(double* unitQuat, size_t i, double* outQuat)
{

//  if (Quaternions::isInitialized == false)
//  {
//    Quaternions::initialize();
//    // This will make sure things are initialized
//  }
  outQuat[1]=QUAT_SYMMHEX[i][1]*unitQuat[4]+QUAT_SYMMHEX[i][4]*unitQuat[1]-QUAT_SYMMHEX[i][2]*unitQuat[3]+QUAT_SYMMHEX[i][3]*unitQuat[2];
  outQuat[2]=QUAT_SYMMHEX[i][2]*unitQuat[4]+QUAT_SYMMHEX[i][4]*unitQuat[2]-QUAT_SYMMHEX[i][3]*unitQuat[1]+QUAT_SYMMHEX[i][1]*unitQuat[3];
  outQuat[3]=QUAT_SYMMHEX[i][3]*unitQuat[4]+QUAT_SYMMHEX[i][4]*unitQuat[3]-QUAT_SYMMHEX[i][1]*unitQuat[2]+QUAT_SYMMHEX[i][2]*unitQuat[1];
  outQuat[4]=QUAT_SYMMHEX[i][4]*unitQuat[4]-QUAT_SYMMHEX[i][1]*unitQuat[1]-QUAT_SYMMHEX[i][2]*unitQuat[2]-QUAT_SYMMHEX[i][3]*unitQuat[3];
}

#define QUAT_SYMMCUBIC AIM::Quaternions::quat_symmcubic
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Quaternions::Cubic_MultiplyByUnitQuaterion(double* unitQuat, size_t i, double* outQuat)
{

//  if (Quaternions::isInitialized == false)
//  {
//    Quaternions::initialize();
//    // This will make sure things are initialized
//  }
  outQuat[1]=QUAT_SYMMCUBIC[i][1]*unitQuat[4]+QUAT_SYMMCUBIC[i][4]*unitQuat[1]-QUAT_SYMMCUBIC[i][2]*unitQuat[3]+QUAT_SYMMCUBIC[i][3]*unitQuat[2];
  outQuat[2]=QUAT_SYMMCUBIC[i][2]*unitQuat[4]+QUAT_SYMMCUBIC[i][4]*unitQuat[2]-QUAT_SYMMCUBIC[i][3]*unitQuat[1]+QUAT_SYMMCUBIC[i][1]*unitQuat[3];
  outQuat[3]=QUAT_SYMMCUBIC[i][3]*unitQuat[4]+QUAT_SYMMCUBIC[i][4]*unitQuat[3]-QUAT_SYMMCUBIC[i][1]*unitQuat[2]+QUAT_SYMMCUBIC[i][2]*unitQuat[1];
  outQuat[4]=QUAT_SYMMCUBIC[i][4]*unitQuat[4]-QUAT_SYMMCUBIC[i][1]*unitQuat[1]-QUAT_SYMMCUBIC[i][2]*unitQuat[2]-QUAT_SYMMCUBIC[i][3]*unitQuat[3];
}