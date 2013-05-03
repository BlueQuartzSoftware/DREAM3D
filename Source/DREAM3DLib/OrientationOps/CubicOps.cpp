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
#include "CubicOps.h"
// Include this FIRST because there is a needed define for some compiles
// to expose some of the constants needed below
#include "DREAM3DLib/Common/DREAM3DMath.h"


  const static float m_pi = static_cast<float>(M_PI);

  const float threesixty_over_pi = 360.0f/m_pi;
  const float oneeighty_over_pi = 180.0f/m_pi;
  const float sqrt_two = powf(2.0f, 0.5f);

  const float acos_neg_one = acosf(-1.0f);
  const float acos_pos_one = acosf(1.0f);

  static const float CubicDim1InitValue = powf((0.75f*((m_pi/4.0f)-sinf((m_pi/4.0f)))),(1.0f/3.0f));
  static const float CubicDim2InitValue = powf((0.75f*((m_pi/4.0f)-sinf((m_pi/4.0f)))),(1.0f/3.0f));
  static const float CubicDim3InitValue = powf((0.75f*((m_pi/4.0f)-sinf((m_pi/4.0f)))),(1.0f/3.0f));
  static const float CubicDim1StepValue = CubicDim1InitValue/9.0f;
  static const float CubicDim2StepValue = CubicDim2InitValue/9.0f;
  static const float CubicDim3StepValue = CubicDim3InitValue/9.0f;

  static const float CubicQuatSym[24][5] = {
                       {0.000000000f, 0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f},
                       {0.000000000f, 1.000000000f, 0.000000000f, 0.000000000f, 0.000000000f},
                       {0.000000000f, 0.000000000f, 1.000000000f, 0.000000000f, 0.000000000f},
                       {0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f, 0.000000000f},
                       {0.000000000f, 0.707106781f, 0.000000000f, 0.000000000f, 0.707106781f},
                       {0.000000000f, 0.000000000f, 0.707106781f, 0.000000000f, 0.707106781f},
                       {0.000000000f, 0.000000000f, 0.000000000f, 0.707106781f, 0.707106781f},
                       {0.000000000f, -0.707106781f, 0.000000000f, 0.000000000f, 0.707106781f},
                       {0.000000000f, 0.000000000f, -0.707106781f, 0.000000000f, 0.707106781f},
                       {0.000000000f, 0.000000000f, 0.000000000f, -0.707106781f, 0.707106781f},
                       {0.000000000f, 0.707106781f, 0.707106781f, 0.000000000f, 0.000000000f},
                       {0.000000000f, -0.707106781f, 0.707106781f, 0.000000000f, 0.000000000f},
                       {0.000000000f, 0.000000000f, 0.707106781f, 0.707106781f, 0.000000000f},
                       {0.000000000f, 0.000000000f, -0.707106781f, 0.707106781f, 0.000000000f},
                       {0.000000000f, 0.707106781f, 0.000000000f, 0.707106781f, 0.000000000f},
                       {0.000000000f, -0.707106781f, 0.000000000f, 0.707106781f, 0.000000000f},
                       {0.000000000f, 0.500000000f, 0.500000000f, 0.500000000f, 0.500000000f},
                       {0.000000000f, -0.500000000f, -0.500000000f, -0.500000000f, 0.500000000f},
                       {0.000000000f, 0.500000000f, -0.500000000f, 0.500000000f, 0.500000000f},
                       {0.000000000f, -0.500000000f, 0.500000000f, -0.500000000f, 0.500000000f},
                       {0.000000000f, -0.500000000f, 0.500000000f, 0.500000000f, 0.500000000f},
                       {0.000000000f, 0.500000000f, -0.500000000f, -0.500000000f, 0.500000000f},
                       {0.000000000f, -0.500000000f, -0.500000000f, 0.500000000f, 0.500000000f},
                       {0.000000000f, 0.500000000f, 0.500000000f, -0.500000000f, 0.500000000f}};

  static const float CubicRodSym[24][3] = {{0.0f, 0.0f,0.0f},
                      {10000000000.0f, 0.0f, 0.0f},
                      {0.0f, 10000000000.0f, 0.0f},
                      {0.0f, 0.0f, 10000000000.0f},
                      {1.0f, 0.0f, 0.0f},
                      {0.0f, 1.0f, 0.0f},
                      {0.0f, 0.0f, 1.0f},
                      {-1.0f, 0.0f, 0.0f},
                      {0.0f, -1.0f, 0.0f},
                      {0.0f, 0.0f, -1.0f},
                      {10000000000.0f, 10000000000.0f, 0.0f},
                      {-10000000000.0f, 10000000000.0f, 0.0f},
                      {0.0f, 10000000000.0f, 10000000000.0f},
                      {0.0f, -10000000000.0f, 10000000000.0f},
                      {10000000000.0f, 0.0f, 10000000000.0f},
                      {-10000000000.0f, 0.0f, 10000000000.0f},
                      {1.0f, 1.0f, 1.0f},
                      {-1.0f, -1.0f, -1.0f},
                      {1.0f, -1.0f, 1.0f},
                      {-1.0f, 1.0f, -1.0f},
                      {-1.0f, 1.0f, 1.0f},
                      {1.0f, -1.0f, -1.0f},
                      {-1.0f, -1.0f, 1.0f},
                      {1.0f, 1.0f, -1.0}};

    static const float CubicSlipDirections[12][3] = {{0.0f,1.0f,-1.0f},
												{1.0f,0.0f,-1.0f},
												{1.0f,-1.0f,0.0f},
												{1.0f,-1.0f,0.0f},
												{1.0f,0.0f,1.0f},
												{0.0f,1.0f,1.0f},
												{1.0f,1.0f,0.0f},
												{0.0f,1.0f,1.0f},
												{1.0f,0.0f,-1.0f},
												{1.0f,1.0f,0.0f},
												{1.0f,0.0f,1.0f},
												{0.0f,1.0f,-1.0f}};

    static const float CubicSlipPlanes[12][3] = {{1.0f,1.0f,1.0f},
												{1.0f,1.0f,1.0f},
												{1.0f,1.0f,1.0f},
												{1.0f,1.0f,-1.0f},
												{1.0f,1.0f,-1.0f},
												{1.0f,1.0f,-1.0f},
												{1.0f,-1.0f,1.0f},
												{1.0f,-1.0f,1.0f},
												{1.0f,-1.0f,1.0f},
												{-1.0f,1.0f,1.0f},
												{-1.0f,1.0f,1.0f},
												{-1.0f,1.0f,1.0f}};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CubicOps::CubicOps()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CubicOps::~CubicOps()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float CubicOps::getMisoQuat( float q1[5],float q2[5],float &n1,float &n2,float &n3)
{

  int numsym = 24;

  return _calcMisoQuat(CubicQuatSym, numsym, q1, q2, n1, n2, n3);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float CubicOps::_calcMisoQuat(const float quatsym[24][5], int numsym,
                                      float q1[5], float q2[5],
                                      float &n1, float &n2, float &n3)
{
  float wmin=9999999.0f; //,na,nb,nc;
   float qc[5];
   float qco[5];
   int type = 1;
   float sin_wmin_over_2 = 0.0;
 //  float _1, _2,  _6;


   qc[1]=-q1[1]*q2[4]+q1[4]*q2[1]-q1[2]*q2[3]+q1[3]*q2[2];
   qc[2]=-q1[2]*q2[4]+q1[4]*q2[2]-q1[3]*q2[1]+q1[1]*q2[3];
   qc[3]=-q1[3]*q2[4]+q1[4]*q2[3]-q1[1]*q2[2]+q1[2]*q2[1];
   qc[4]=-q1[4]*q2[4]-q1[1]*q2[1]-q1[2]*q2[2]-q1[3]*q2[3];
   qc[1]=fabs(qc[1]);
   qc[2]=fabs(qc[2]);
   qc[3]=fabs(qc[3]);
   qc[4]=fabs(qc[4]);
   //if qc[1] is smallest
   if ( qc[1] <= qc[2] && qc[1] <= qc[3] && qc[1] <= qc[4])
   {
      qco[1] = qc[1];
    //if qc[2] is next smallest
      if (qc[2] <= qc[3] && qc[2] <= qc[4])
    {
       qco[2] = qc[2];
     if(qc[3] <= qc[4]) qco[3] = qc[3], qco[4] = qc[4];
     else qco[3] = qc[4], qco[4] = qc[3];
    }
    //if qc[3] is next smallest
      else if (qc[3] <= qc[2] && qc[3] <= qc[4])
    {
       qco[2] = qc[3];
     if(qc[2] <= qc[4]) qco[3] = qc[2], qco[4] = qc[4];
     else qco[3] = qc[4], qco[4] = qc[2];
    }
    //if qc[4] is next smallest
    else
    {
       qco[2] = qc[4];
     if(qc[2] <= qc[3]) qco[3] = qc[2], qco[4] = qc[3];
     else qco[3] = qc[3], qco[4] = qc[2];
    }
   }
 //if qc[2] is smallest
   else if ( qc[2] <= qc[1] && qc[2] <= qc[3] && qc[2] <= qc[4])
   {
      qco[1] = qc[2];
    //if qc[1] is next smallest
      if (qc[1] <= qc[3] && qc[1] <= qc[4])
    {
       qco[2] = qc[1];
     if(qc[3] <= qc[4]) qco[3] = qc[3], qco[4] = qc[4];
     else qco[3] = qc[4], qco[4] = qc[3];
    }
    //if qc[3] is next smallest
      else if (qc[3] <= qc[1] && qc[3] <= qc[4])
    {
       qco[2] = qc[3];
     if(qc[1] <= qc[4]) qco[3] = qc[1], qco[4] = qc[4];
     else qco[3] = qc[4], qco[4] = qc[1];
    }
    //if qc[4] is next smallest
    else
    {
       qco[2] = qc[4];
     if(qc[1] <= qc[3]) qco[3] = qc[1], qco[4] = qc[3];
     else qco[3] = qc[3], qco[4] = qc[1];
    }
   }
 //if qc[3] is smallest
   else if ( qc[3] <= qc[1] && qc[3] <= qc[2] && qc[3] <= qc[4])
   {
      qco[1] = qc[3];
    //if qc[1] is next smallest
      if (qc[1] <= qc[2] && qc[1] <= qc[4])
    {
       qco[2] = qc[1];
     if(qc[2] <= qc[4]) qco[3] = qc[2], qco[4] = qc[4];
     else qco[3] = qc[4], qco[4] = qc[2];
    }
    //if qc[2] is next smallest
      else if (qc[2] <= qc[1] && qc[2] <= qc[4])
    {
       qco[2] = qc[2];
     if(qc[1] <= qc[4]) qco[3] = qc[1], qco[4] = qc[4];
     else qco[3] = qc[4], qco[4] = qc[1];
    }
    //if qc[4] is next smallest
    else
    {
       qco[2] = qc[4];
     if(qc[1] <= qc[2]) qco[3] = qc[1], qco[4] = qc[2];
     else qco[3] = qc[2], qco[4] = qc[1];
    }
   }
 //if qc[4] is smallest
   else
   {
      qco[1] = qc[4];
    //if qc[1] is next smallest
      if (qc[1] <= qc[2] && qc[1] <= qc[3])
    {
       qco[2] = qc[1];
     if(qc[2] <= qc[3]) qco[3] = qc[2], qco[4] = qc[3];
     else qco[3] = qc[3], qco[4] = qc[2];
    }
    //if qc[2] is next smallest
      else if (qc[2] <= qc[1] && qc[2] <= qc[3])
    {
       qco[2] = qc[2];
     if(qc[1] <= qc[3]) qco[3] = qc[1], qco[4] = qc[3];
     else qco[3] = qc[3], qco[4] = qc[1];
    }
    //if qc[3] is next smallest
    else
    {
       qco[2] = qc[3];
     if(qc[1] <= qc[2]) qco[3] = qc[1], qco[4] = qc[2];
     else qco[3] = qc[2], qco[4] = qc[1];
    }
   }
   wmin = qco[4];
   if (((qco[3] + qco[4]) / (sqrt_two)) > wmin)
   {
     wmin = ((qco[3] + qco[4]) / (sqrt_two));
	 type = 2;
   }
   if (((qco[1] + qco[2] + qco[3] + qco[4]) / 2) > wmin)
   {
     wmin = ((qco[1] + qco[2] + qco[3] + qco[4]) / 2);
	 type = 3;
   }
   if (wmin < -1.0)
   {
   //  wmin = -1.0;
     wmin = acos_neg_one;
     sin_wmin_over_2 = sinf(wmin);
   }
   else if (wmin > 1.0)
   {
  //   wmin = 1.0;
     wmin = acos_pos_one;
     sin_wmin_over_2 = sinf(wmin);
   }
   else
   {
     wmin = acos(wmin);
     sin_wmin_over_2 = sinf(wmin);
   }

   if(type == 1)
   {
	   n1 = qco[1] / sin_wmin_over_2;
	   n2 = qco[2] / sin_wmin_over_2;
	   n3 = qco[3] / sin_wmin_over_2;
   }
   if(type == 2)
   {
	   n1 = ((qco[1] - qco[2]) / (sqrt_two)) / sin_wmin_over_2;
	   n2 = ((qco[1] + qco[2]) / (sqrt_two)) / sin_wmin_over_2;
	   n3 = ((qco[3] - qco[4]) / (sqrt_two)) / sin_wmin_over_2;
   }
   if(type == 3)
   {
	   n1 = ((qco[1] - qco[2] + qco[3] - qco[4]) / (2.0f)) / sin_wmin_over_2;
	   n2 = ((qco[1] + qco[2] - qco[3] - qco[4]) / (2.0f)) / sin_wmin_over_2;
	   n3 = ((-qco[1] + qco[2] + qco[3] - qco[4]) / (2.0f)) / sin_wmin_over_2;
   }
   float denom = sqrt((n1*n1+n2*n2+n3*n3));
   n1 = n1/denom;
   n2 = n2/denom;
   n3 = n3/denom;
   if(denom == 0) n1 = 0.0, n2 = 0.0, n3 = 1.0;
   if(wmin == 0) n1 = 0.0, n2 = 0.0, n3 = 1.0;
   wmin = 2.0f * wmin;
   return wmin;

}

void CubicOps::getODFFZRod(float &r1,float &r2, float &r3)
{
  int numsym = 24;

  _calcRodNearestOrigin(CubicRodSym, numsym, r1, r2, r3);
}

void CubicOps::getQuatSymOp(int i,float *q)
{
  q[0] = CubicQuatSym[i][0];
  q[1] = CubicQuatSym[i][1];
  q[2] = CubicQuatSym[i][2];
  q[3] = CubicQuatSym[i][3];
  q[4] = CubicQuatSym[i][4];
}

void CubicOps::getRodSymOp(int i,float *r)
{
  r[0] = CubicRodSym[i][0];
  r[1] = CubicRodSym[i][1];
  r[2] = CubicRodSym[i][2];
}

void CubicOps::getMDFFZRod(float &r1,float &r2, float &r3)
{
	float w, n1, n2, n3;
	float FZw, FZn1, FZn2, FZn3;

	OrientationMath::_calcRodNearestOrigin(CubicRodSym, 24, r1, r2, r3);
	OrientationMath::RodtoAxisAngle(r1, r2, r3, w, n1, n2, n3);

	FZw = w;
	n1 = fabs(n1);
	n2 = fabs(n2);
	n3 = fabs(n3);
	if(n1 > n2)
	{
		if(n1 > n3)
		{
			FZn1 = n1;
			if (n2 > n3) FZn2 = n2, FZn3 = n3;
			else FZn2 = n3, FZn3 = n2;
		}
		else FZn1 = n3, FZn2 = n1, FZn3 = n2;
	}
	else
	{
		if(n2 > n3)
		{
			FZn1 = n2;
			if (n1 > n3) FZn2 = n1, FZn3 = n3;
			else FZn2 = n3, FZn3 = n1;
		}
		else FZn1 = n3, FZn2 = n2, FZn3 = n1;
	}

	OrientationMath::axisAngletoRod(FZw, FZn1, FZn2, FZn3, r1, r2, r3);
}

void CubicOps::getNearestQuat( float *q1, float *q2)
{
  int numsym = 24;

  _calcNearestQuat(CubicQuatSym, numsym, q1, q2);
}

void CubicOps::getFZQuat(float *qr)
{
  int numsym = 24;

    _calcQuatNearestOrigin(CubicQuatSym, numsym, qr);
}

int CubicOps::getMisoBin(float r1, float r2, float r3)
{
  float dim[3];
  float bins[3];
  float step[3];

  RodtoHomochoric(r1, r2, r3);

  dim[0] = CubicDim1InitValue;
  dim[1] = CubicDim2InitValue;
  dim[2] = CubicDim3InitValue;
  step[0] = CubicDim1StepValue;
  step[1] = CubicDim2StepValue;
  step[2] = CubicDim3StepValue;
  bins[0] = 18.0f;
  bins[1] = 18.0f;
  bins[2] = 18.0f;

  return _calcMisoBin(dim, bins, step, r1, r2, r3);
}

void CubicOps::determineEulerAngles(int choose, float &synea1, float &synea2, float &synea3)
{
  float init[3];
  float step[3];
  float phi[3];
  float r1, r2, r3;

  init[0] = CubicDim1InitValue;
  init[1] = CubicDim2InitValue;
  init[2] = CubicDim3InitValue;
  step[0] = CubicDim1StepValue;
  step[1] = CubicDim2StepValue;
  step[2] = CubicDim3StepValue;
  phi[0] = static_cast<float>(choose % 18);
  phi[1] = static_cast<float>((choose / 18) % 18);
  phi[2] = static_cast<float>(choose / (18 * 18));

  _calcDetermineHomochoricValues(init, step, phi, choose, r1, r2, r3);
  HomochorictoRod(r1, r2, r3);
  getODFFZRod(r1, r2, r3);
  RodtoEuler(r1, r2, r3, synea1, synea2, synea3);
}

void CubicOps::determineRodriguesVector(int choose, float &r1, float &r2, float &r3)
{
  float init[3];
  float step[3];
  float phi[3];

  init[0] = CubicDim1InitValue;
  init[1] = CubicDim2InitValue;
  init[2] = CubicDim3InitValue;
  step[0] = CubicDim1StepValue;
  step[1] = CubicDim2StepValue;
  step[2] = CubicDim3StepValue;
  phi[0] = static_cast<float>(choose % 18);
  phi[1] = static_cast<float>((choose / 18) % 18);
  phi[2] = static_cast<float>(choose / (18 * 18));

  _calcDetermineHomochoricValues(init, step, phi, choose, r1, r2, r3);
  HomochorictoRod(r1, r2, r3);
  getMDFFZRod(r1, r2, r3);
}
int CubicOps::getOdfBin(float r1, float r2, float r3)
{
  float dim[3];
  float bins[3];
  float step[3];

  RodtoHomochoric(r1, r2, r3);

  dim[0] = CubicDim1InitValue;
  dim[1] = CubicDim2InitValue;
  dim[2] = CubicDim3InitValue;
  step[0] = CubicDim1StepValue;
  step[1] = CubicDim2StepValue;
  step[2] = CubicDim3StepValue;
  bins[0] = 18.0f;
  bins[1] = 18.0f;
  bins[2] = 18.0f;

  return _calcODFBin(dim, bins, step, r1, r2, r3);
}

void CubicOps::getSchmidFactorAndSS(float loadx, float loady, float loadz, float &schmidfactor, int &slipsys)
{
  float theta1, theta2, theta3, theta4;
  float lambda1, lambda2, lambda3, lambda4, lambda5, lambda6;
  float schmid1, schmid2, schmid3, schmid4, schmid5, schmid6, schmid7, schmid8, schmid9, schmid10, schmid11, schmid12;

  float mag = loadx * loadx + loady * loady + loadz * loadz;
  mag = sqrt(mag);
  theta1 = (loadx + loady + loadz) / (mag * 1.732f);
  theta1 = fabs(theta1);
  theta2 = (loadx + loady - loadz) / (mag * 1.732f);
  theta2 = fabs(theta2);
  theta3 = (loadx - loady + loadz) / (mag * 1.732f);
  theta3 = fabs(theta3);
  theta4 = (-loadx + loady + loadz) / (mag * 1.732f);
  theta4 = fabs(theta4);
  lambda1 = (loadx + loady) / (mag * 1.414f);
  lambda1 = fabs(lambda1);
  lambda2 = (loadx + loadz) / (mag * 1.414f);
  lambda2 = fabs(lambda2);
  lambda3 = (loadx - loady) / (mag * 1.414f);
  lambda3 = fabs(lambda3);
  lambda4 = (loadx - loadz) / (mag * 1.414f);
  lambda4 = fabs(lambda4);
  lambda5 = (loady + loadz) / (mag * 1.414f);
  lambda5 = fabs(lambda5);
  lambda6 = (loady - loadz) / (mag * 1.414f);
  lambda6 = fabs(lambda6);
  schmid1 = theta1 * lambda6;
  schmid2 = theta1 * lambda4;
  schmid3 = theta1 * lambda3;
  schmid4 = theta2 * lambda3;
  schmid5 = theta2 * lambda2;
  schmid6 = theta2 * lambda5;
  schmid7 = theta3 * lambda1;
  schmid8 = theta3 * lambda5;
  schmid9 = theta3 * lambda4;
  schmid10 = theta4 * lambda1;
  schmid11 = theta4 * lambda2;
  schmid12 = theta4 * lambda6;
  schmidfactor = schmid1, slipsys = 0;

  if (schmid2 > schmidfactor) schmidfactor = schmid2, slipsys = 1;
  if (schmid3 > schmidfactor) schmidfactor = schmid3, slipsys = 2;
  if (schmid4 > schmidfactor) schmidfactor = schmid4, slipsys = 3;
  if (schmid5 > schmidfactor) schmidfactor = schmid5, slipsys = 4;
  if (schmid6 > schmidfactor) schmidfactor = schmid6, slipsys = 5;
  if (schmid7 > schmidfactor) schmidfactor = schmid7, slipsys = 6;
  if (schmid8 > schmidfactor) schmidfactor = schmid8, slipsys = 7;
  if (schmid9 > schmidfactor) schmidfactor = schmid9, slipsys = 8;
  if (schmid10 > schmidfactor) schmidfactor = schmid10, slipsys = 9;
  if (schmid11 > schmidfactor) schmidfactor = schmid11, slipsys = 10;
  if (schmid12 > schmidfactor) schmidfactor = schmid12, slipsys = 11;
}

void CubicOps::getmPrime(float q1[5], float q2[5], float LD[3], float &mPrime)
{
  float g1[3][3];
  float g2[3][3];
  float hkl1[3], uvw1[3];
  float hkl2[3], uvw2[3];
  float slipDirection[3], slipPlane[3];
  float schmidFactor1=0, schmidFactor2=0, maxSchmidFactor=0;
  float directionComponent1=0, planeComponent1=0;
  float directionComponent2=0, planeComponent2=0;
  float planeMisalignment=0, directionMisalignment=0;
  int ss1=0, ss2=0;

  QuattoMat(q1, g1);
  QuattoMat(q2, g2);
  MatrixMath::transpose3x3(g1, g1);
  MatrixMath::transpose3x3(g2, g2);
  for(int i=0;i<12;i++)
  {
	  slipDirection[0] = CubicSlipDirections[i][0];
	  slipDirection[1] = CubicSlipDirections[i][1];
	  slipDirection[2] = CubicSlipDirections[i][2];
	  slipPlane[0] = CubicSlipPlanes[i][0];
	  slipPlane[1] = CubicSlipPlanes[i][1];
	  slipPlane[2] = CubicSlipPlanes[i][2];
	  MatrixMath::multiply3x3with3x1(g1,slipDirection,hkl1);
	  MatrixMath::multiply3x3with3x1(g1,slipPlane,uvw1);
	  MatrixMath::normalize3x1(hkl1);
	  MatrixMath::normalize3x1(uvw1);
	  directionComponent1 = fabs(MatrixMath::dotProduct(LD,uvw1));
	  planeComponent1 = fabs(MatrixMath::dotProduct(LD,hkl1));
	  schmidFactor1 = directionComponent1*planeComponent1;
	  if(schmidFactor1 > maxSchmidFactor)
	  {
		  maxSchmidFactor = schmidFactor1;
		  ss1 = i;
	  }
  }
  slipDirection[0] = CubicSlipDirections[ss1][0];
  slipDirection[1] = CubicSlipDirections[ss1][1];
  slipDirection[2] = CubicSlipDirections[ss1][2];
  slipPlane[0] = CubicSlipPlanes[ss1][0];
  slipPlane[1] = CubicSlipPlanes[ss1][1];
  slipPlane[2] = CubicSlipPlanes[ss1][2];
  MatrixMath::multiply3x3with3x1(g1,slipDirection,hkl1);
  MatrixMath::multiply3x3with3x1(g1,slipPlane,uvw1);
  MatrixMath::normalize3x1(hkl1);
  MatrixMath::normalize3x1(uvw1);

  maxSchmidFactor = 0;
  for(int j=0;j<12;j++)
  {
	  slipDirection[0] = CubicSlipDirections[j][0];
	  slipDirection[1] = CubicSlipDirections[j][1];
	  slipDirection[2] = CubicSlipDirections[j][2];
	  slipPlane[0] = CubicSlipPlanes[j][0];
	  slipPlane[1] = CubicSlipPlanes[j][1];
	  slipPlane[2] = CubicSlipPlanes[j][2];
	  MatrixMath::multiply3x3with3x1(g2,slipDirection,hkl2);
	  MatrixMath::multiply3x3with3x1(g2,slipPlane,uvw2);
	  MatrixMath::normalize3x1(hkl2);
	  MatrixMath::normalize3x1(uvw2);
	  directionComponent2 = fabs(MatrixMath::dotProduct(LD,uvw2));
	  planeComponent2 = fabs(MatrixMath::dotProduct(LD,hkl2));
	  schmidFactor2 = directionComponent2*planeComponent2;
	  if(schmidFactor2 > maxSchmidFactor)
	  {
		  maxSchmidFactor = schmidFactor2;
		  ss2 = j;
	  }
  }
  slipDirection[0] = CubicSlipDirections[ss2][0];
  slipDirection[1] = CubicSlipDirections[ss2][1];
  slipDirection[2] = CubicSlipDirections[ss2][2];
  slipPlane[0] = CubicSlipPlanes[ss2][0];
  slipPlane[1] = CubicSlipPlanes[ss2][1];
  slipPlane[2] = CubicSlipPlanes[ss2][2];
  MatrixMath::multiply3x3with3x1(g2,slipDirection,hkl2);
  MatrixMath::multiply3x3with3x1(g2,slipPlane,uvw2);
  MatrixMath::normalize3x1(hkl2);
  MatrixMath::normalize3x1(uvw2);
  planeMisalignment = fabs(MatrixMath::dotProduct(hkl1,hkl2));
  directionMisalignment = fabs(MatrixMath::dotProduct(uvw1,uvw2));
  mPrime = planeMisalignment*directionMisalignment;
}

void CubicOps::getF1(float q1[5], float q2[5], float LD[3], bool maxSF, float &F1)
{
  float g1[3][3];
  float g2[3][3];
  float hkl1[3], uvw1[3];
  float hkl2[3], uvw2[3];
  float slipDirection[3], slipPlane[3];
  float directionMisalignment=0, totalDirectionMisalignment=0;
  float schmidFactor1=0, schmidFactor2=0, maxSchmidFactor=0;
  float directionComponent1=0, planeComponent1=0;
  float directionComponent2=0, planeComponent2=0;
  float maxF1=0;

  QuattoMat(q1, g1);
  QuattoMat(q2, g2);
  MatrixMath::transpose3x3(g1, g1);
  MatrixMath::transpose3x3(g2, g2);

  MatrixMath::normalize3x1(LD);

  if(maxSF == true) maxSchmidFactor = 0;
  for(int i=0;i<12;i++)
  {
	  slipDirection[0] = CubicSlipDirections[i][0];
	  slipDirection[1] = CubicSlipDirections[i][1];
	  slipDirection[2] = CubicSlipDirections[i][2];
	  slipPlane[0] = CubicSlipPlanes[i][0];
	  slipPlane[1] = CubicSlipPlanes[i][1];
	  slipPlane[2] = CubicSlipPlanes[i][2];
	  MatrixMath::multiply3x3with3x1(g1,slipDirection,hkl1);
	  MatrixMath::multiply3x3with3x1(g1,slipPlane,uvw1);
	  MatrixMath::normalize3x1(hkl1);
	  MatrixMath::normalize3x1(uvw1);
	  directionComponent1 = fabs(MatrixMath::dotProduct(LD,uvw1));
	  planeComponent1 = fabs(MatrixMath::dotProduct(LD,hkl1));
	  schmidFactor1 = directionComponent1*planeComponent1;
	  if(schmidFactor1 > maxSchmidFactor || maxSF == false)
	  {
		  totalDirectionMisalignment = 0;
		  if(maxSF == true) maxSchmidFactor = schmidFactor1;
		  for(int j=0;j<12;j++)
		  {
			  slipDirection[0] = CubicSlipDirections[j][0];
			  slipDirection[1] = CubicSlipDirections[j][1];
			  slipDirection[2] = CubicSlipDirections[j][2];
			  slipPlane[0] = CubicSlipPlanes[j][0];
			  slipPlane[1] = CubicSlipPlanes[j][1];
			  slipPlane[2] = CubicSlipPlanes[j][2];
			  MatrixMath::multiply3x3with3x1(g2,slipDirection,hkl2);
			  MatrixMath::multiply3x3with3x1(g2,slipPlane,uvw2);
			  MatrixMath::normalize3x1(hkl2);
			  MatrixMath::normalize3x1(uvw2);
			  directionComponent2 = fabs(MatrixMath::dotProduct(LD,uvw2));
			  planeComponent2 = fabs(MatrixMath::dotProduct(LD,hkl2));
			  schmidFactor2 = directionComponent2*planeComponent2;
			  directionMisalignment = fabs(MatrixMath::dotProduct(uvw1,uvw2));
			  totalDirectionMisalignment = totalDirectionMisalignment + directionMisalignment;
		  }
		  F1 = schmidFactor1*directionComponent1*totalDirectionMisalignment;
		  if(maxSF == false)
		  {
			  if(F1 < maxF1) F1 = maxF1;
			  else maxF1 = F1;
		  }
	  }
  }
}

void CubicOps::getF1spt(float q1[5], float q2[5], float LD[3], bool maxSF, float &F1spt)
{
  float g1[3][3];
  float g2[3][3];
  float hkl1[3], uvw1[3];
  float hkl2[3], uvw2[3];
  float slipDirection[3], slipPlane[3];
  float directionMisalignment=0, totalDirectionMisalignment=0;
  float planeMisalignment=0, totalPlaneMisalignment=0;
  float schmidFactor1=0, schmidFactor2=0, maxSchmidFactor=0;
  float directionComponent1=0, planeComponent1=0;
  float directionComponent2=0, planeComponent2=0;
  float maxF1spt=0;

  QuattoMat(q1, g1);
  QuattoMat(q2, g2);
  MatrixMath::transpose3x3(g1, g1);
  MatrixMath::transpose3x3(g2, g2);

  MatrixMath::normalize3x1(LD);

  if(maxSF == true) maxSchmidFactor = 0;
  for(int i=0;i<12;i++)
  {
	  slipDirection[0] = CubicSlipDirections[i][0];
	  slipDirection[1] = CubicSlipDirections[i][1];
	  slipDirection[2] = CubicSlipDirections[i][2];
	  slipPlane[0] = CubicSlipPlanes[i][0];
	  slipPlane[1] = CubicSlipPlanes[i][1];
	  slipPlane[2] = CubicSlipPlanes[i][2];
	  MatrixMath::multiply3x3with3x1(g1,slipDirection,hkl1);
	  MatrixMath::multiply3x3with3x1(g1,slipPlane,uvw1);
	  MatrixMath::normalize3x1(hkl1);
	  MatrixMath::normalize3x1(uvw1);
	  directionComponent1 = fabs(MatrixMath::dotProduct(LD,uvw1));
	  planeComponent1 = fabs(MatrixMath::dotProduct(LD,hkl1));
	  schmidFactor1 = directionComponent1*planeComponent1;
	  if(schmidFactor1 > maxSchmidFactor || maxSF == false)
	  {
		  totalDirectionMisalignment = 0;
		  totalPlaneMisalignment = 0;
		  if(maxSF == true) maxSchmidFactor = schmidFactor1;
		  for(int j=0;j<12;j++)
		  {
			  slipDirection[0] = CubicSlipDirections[j][0];
			  slipDirection[1] = CubicSlipDirections[j][1];
			  slipDirection[2] = CubicSlipDirections[j][2];
			  slipPlane[0] = CubicSlipPlanes[j][0];
			  slipPlane[1] = CubicSlipPlanes[j][1];
			  slipPlane[2] = CubicSlipPlanes[j][2];
			  MatrixMath::multiply3x3with3x1(g2,slipDirection,hkl2);
			  MatrixMath::multiply3x3with3x1(g2,slipPlane,uvw2);
			  MatrixMath::normalize3x1(hkl2);
			  MatrixMath::normalize3x1(uvw2);
			  directionComponent2 = fabs(MatrixMath::dotProduct(LD,uvw2));
			  planeComponent2 = fabs(MatrixMath::dotProduct(LD,hkl2));
			  schmidFactor2 = directionComponent2*planeComponent2;
			  directionMisalignment = fabs(MatrixMath::dotProduct(uvw1,uvw2));
			  planeMisalignment = fabs(MatrixMath::dotProduct(hkl1,hkl2));
			  totalDirectionMisalignment = totalDirectionMisalignment + directionMisalignment;
			  totalPlaneMisalignment = totalPlaneMisalignment + planeMisalignment;
		  }
		  F1spt = schmidFactor1*directionComponent1*totalDirectionMisalignment*totalPlaneMisalignment;
		  if(maxSF == false)
		  {
			  if(F1spt < maxF1spt) F1spt = maxF1spt;
			  else maxF1spt = F1spt;
		  }
	  }
  }
}

void CubicOps::getF7(float q1[5], float q2[5], float LD[3], bool maxSF, float &F7)
{
  float g1[3][3];
  float g2[3][3];
  float hkl1[3], uvw1[3];
  float hkl2[3], uvw2[3];
  float slipDirection[3], slipPlane[3];
  float directionMisalignment=0, totalDirectionMisalignment=0;
  float schmidFactor1=0, schmidFactor2=0, maxSchmidFactor=0;
  float directionComponent1=0, planeComponent1=0;
  float directionComponent2=0, planeComponent2=0;
  float maxF7=0;

  QuattoMat(q1, g1);
  QuattoMat(q2, g2);
  MatrixMath::transpose3x3(g1, g1);
  MatrixMath::transpose3x3(g2, g2);

  MatrixMath::normalize3x1(LD);

  for(int i=0;i<12;i++)
  {
	  slipDirection[0] = CubicSlipDirections[i][0];
	  slipDirection[1] = CubicSlipDirections[i][1];
	  slipDirection[2] = CubicSlipDirections[i][2];
	  slipPlane[0] = CubicSlipPlanes[i][0];
	  slipPlane[1] = CubicSlipPlanes[i][1];
	  slipPlane[2] = CubicSlipPlanes[i][2];
	  MatrixMath::multiply3x3with3x1(g1,slipDirection,hkl1);
	  MatrixMath::multiply3x3with3x1(g1,slipPlane,uvw1);
	  MatrixMath::normalize3x1(hkl1);
	  MatrixMath::normalize3x1(uvw1);
	  directionComponent1 = fabs(MatrixMath::dotProduct(LD,uvw1));
	  planeComponent1 = fabs(MatrixMath::dotProduct(LD,hkl1));
	  schmidFactor1 = directionComponent1*planeComponent1;
	  if(schmidFactor1 > maxSchmidFactor || maxSF == false)
	  {
		  totalDirectionMisalignment = 0;
		  if(maxSF == true) maxSchmidFactor = schmidFactor1;
		  for(int j=0;j<12;j++)
		  {
			  slipDirection[0] = CubicSlipDirections[j][0];
			  slipDirection[1] = CubicSlipDirections[j][1];
			  slipDirection[2] = CubicSlipDirections[j][2];
			  slipPlane[0] = CubicSlipPlanes[j][0];
			  slipPlane[1] = CubicSlipPlanes[j][1];
			  slipPlane[2] = CubicSlipPlanes[j][2];
			  MatrixMath::multiply3x3with3x1(g2,slipDirection,hkl2);
			  MatrixMath::multiply3x3with3x1(g2,slipPlane,uvw2);
			  MatrixMath::normalize3x1(hkl2);
			  MatrixMath::normalize3x1(uvw2);
			  directionComponent2 = fabs(MatrixMath::dotProduct(LD,uvw2));
			  planeComponent2 = fabs(MatrixMath::dotProduct(LD,hkl2));
			  schmidFactor2 = directionComponent2*planeComponent2;
			  directionMisalignment = fabs(MatrixMath::dotProduct(uvw1,uvw2));
			  totalDirectionMisalignment = totalDirectionMisalignment + directionMisalignment;
		  }
		  F7 = directionComponent1*directionComponent1*totalDirectionMisalignment;
		  if(maxSF == false)
		  {
			  if(F7 < maxF7) F7 = maxF7;
			  else maxF7 = F7;
		  }
	  }
  }
}
