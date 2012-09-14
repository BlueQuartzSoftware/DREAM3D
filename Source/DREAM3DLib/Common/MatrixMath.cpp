/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Michael A. Groeber (US Air Force Research Laboratory)
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "MatrixMath.h"

// Include this FIRST because there is a needed define for some compiles
// to expose some of the constants needed below
#include "DREAM3DLib/Common/DREAM3DMath.h"

#include "MXA/Common/LogTime.h"

#include "DREAM3DLib/OrientationOps/CubicOps.h"


	const static float m_pi = static_cast<float>(M_PI);
	const static float two_pi = 2.0f * m_pi;
	const static float recip_pi = 1.0f/m_pi;
	const static float pi_over_180 = m_pi/180.0f;

	const static float m_OnePointThree = 1.33333333333f;

	const float threesixty_over_pi = 360.0f/m_pi;
	const float oneeighty_over_pi = 180.0f/m_pi;
	const float sqrt_two = powf(2.0f, 0.5f);

	const float acos_neg_one = acosf(-1.0f);
	const float acos_pos_one = acosf(1.0f);
	const float sin_wmin_neg_1_over_2 = static_cast<float>( sinf(acos_neg_one/2.0f) );
	const float sin_wmin_pos_1_over_2 = static_cast<float>( sinf(acos_pos_one/2.0f) );
  const float sin_of_acos_neg_1 = sinf(acos_neg_one);
  const float sin_of_acos_pos_1 = sinf(acos_pos_one);

  const float recip_sin_of_acos_neg_1 = 1.0f/sin_of_acos_neg_1;
  const float recip_sin_of_acos_pos_1 = 1.0f/sin_of_acos_pos_1;

	const static float SinOfHalf = sinf(0.5f);
	const static float CosOfHalf = cosf(0.5f);
	const static float SinOfZero = sinf(0.0f);
	const static float CosOfZero = cosf(0.0f);



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MatrixMath::MatrixMath()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MatrixMath::~MatrixMath()
{
}


float MatrixMath::multiply3x3with3x3(float g1[3][3], float g2[3][3])
{
	float deltaG[3][3];
	deltaG[0][0] = g1[0][0]*g2[0][0] + g1[1][0]*g2[1][0] + g1[2][0]*g2[2][0];
	deltaG[1][1] = g1[0][1]*g2[0][1] + g1[1][1]*g2[1][1] + g1[2][1]*g2[2][1];
	deltaG[2][2] = g1[0][2]*g2[0][2] + g1[1][2]*g2[1][2] + g1[2][2]*g2[2][2];
	float value = ((deltaG[0][0]+deltaG[1][1]+deltaG[2][2])-1.0)/2.0;
	if(value > 1.0) value = 1.0;
	if(value < -1.0) value = -1.0;
	return acosf(value);
}

float MatrixMath::multiply3x3with3x1(float g1[3][3], float g2[3])
{
	float deltaG[3][3];
	deltaG[0][0] = g1[0][0]*g2[0][0] + g1[1][0]*g2[1][0] + g1[2][0]*g2[2][0];
	deltaG[1][1] = g1[0][1]*g2[0][1] + g1[1][1]*g2[1][1] + g1[2][1]*g2[2][1];
	deltaG[2][2] = g1[0][2]*g2[0][2] + g1[1][2]*g2[1][2] + g1[2][2]*g2[2][2];
	float value = ((deltaG[0][0]+deltaG[1][1]+deltaG[2][2])-1.0)/2.0;
	if(value > 1.0) value = 1.0;
	if(value < -1.0) value = -1.0;
	return acosf(value);
}

float MatrixMath::transpose3x3(float g[3][3])
{
	float transG[3][3];
	deltaG[0][0] = g1[0][0]*g2[0][0] + g1[1][0]*g2[1][0] + g1[2][0]*g2[2][0];
	deltaG[1][1] = g1[0][1]*g2[0][1] + g1[1][1]*g2[1][1] + g1[2][1]*g2[2][1];
	deltaG[2][2] = g1[0][2]*g2[0][2] + g1[1][2]*g2[1][2] + g1[2][2]*g2[2][2];
	float value = ((deltaG[0][0]+deltaG[1][1]+deltaG[2][2])-1.0)/2.0;
	if(value > 1.0) value = 1.0;
	if(value < -1.0) value = -1.0;
	return acosf(value);
}
