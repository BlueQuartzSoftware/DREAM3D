/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
#include "CubicOps.h"
// Include this FIRST because there is a needed define for some compiles
// to expose some of the constants needed below
#include "AIM/Common/AIMMath.h"


  const static float m_pi = M_PI;

  const float threesixty_over_pi = 360.0f/m_pi;
  const float oneeighty_over_pi = 180.0f/m_pi;
  const float sqrt_two = powf(2.0f, 0.5f);

  const float acos_neg_one = acosf(-1.0f);
  const float acos_pos_one = acosf(1.0f);
  const float sin_wmin_neg_1_over_2 = sinf(acos_neg_one/2.0f);
  const float sin_wmin_pos_1_over_2 = sinf(acos_pos_one/2.0f);

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
  //float quatsym[24][5];


//  for (int i = 0; i < 24; i++)
//  {
//    for (int j = 0; j < 5; j++)
//    {
//      quatsym[i][j] = CubicQuatSym[i][j];
//    }
//  }
  return _calcMisoQuat(CubicQuatSym, numsym, q1, q2, n1, n2, n3);
 // return _calcMisoQuat(quatsym, numsym, q1, q2, n1, n2, n3);
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
   }
   if (((qco[1] + qco[2] + qco[3] + qco[4]) / 2) > wmin)
   {
     wmin = ((qco[1] + qco[2] + qco[3] + qco[4]) / 2);
   }
   if (wmin < -1.0)
   {
   //  wmin = -1.0;
     wmin = acos_neg_one;
     sin_wmin_over_2 = sin_wmin_neg_1_over_2;
   }
   else if (wmin > 1.0)
   {
  //   wmin = 1.0;
     wmin = acos_pos_one;
     sin_wmin_over_2 = sin_wmin_pos_1_over_2;
   }
   else
   {
     wmin = acos(wmin);
     sin_wmin_over_2 = sinf(wmin / 2.0f);
   }

   n1 = qco[1] / sin_wmin_over_2;
   n2 = qco[2] / sin_wmin_over_2;
   n3 = qco[3] / sin_wmin_over_2;
   float denom = powf((n1*n1+n2*n2+n3*n3),0.5);
   n1 = n1/denom;
   n2 = n2/denom;
   n3 = n3/denom;
   if(wmin == 0) n1 = 0.0, n2 = 0.0, n3 = 1.0;
   wmin = (threesixty_over_pi) * wmin;
   return wmin;

}

void CubicOps::getFZRod(float &r1,float &r2, float &r3)
{
  float rodsym[24][3];
  int numsym;
  numsym = 24;
  for(int i=0;i<24;i++)
  {
    for(int j=0;j<3;j++)
    {
      rodsym[i][j] = CubicRodSym[i][j];
    }
  }
  _calcFZRod(rodsym, numsym, r1, r2, r3);
}

void CubicOps::getNearestQuat( float *q1, float *q2)
{
  int numsym = 0;
  float quatsym[24][5];

    numsym = 24;
    for (int i = 0; i < 24; i++)
    {
      for (int j = 0; j < 5; j++)
      {
        quatsym[i][j] = CubicQuatSym[i][j];
      }
    }

    _calcNearestQuat(quatsym, numsym, q1, q2);
}

void CubicOps::getFZQuat(float *qr)
{
  int numsym = 0;
  float quatsym[24][5];

    numsym = 24;
    for(int i=0;i<24;i++)
    {
    for(int j=0;j<5;j++)
    {
      quatsym[i][j] = CubicQuatSym[i][j];
    }
    }
    _calcFZQuat(quatsym, numsym, qr);

}

int CubicOps::getMisoBin(float n1, float n2, float n3)
{
  float dim[3];
  float bins[3];

  dim[0] = CubicDim1InitValue;
  dim[1] = CubicDim2InitValue;
  dim[2] = CubicDim3InitValue;
  bins[0] = 18.0f;
  bins[1] = 18.0f;
  bins[2] = 18.0f;

  return _calcMisoBin(dim, bins, n1, n2, n3);
}

void CubicOps::determineEulerAngles(int choose, float &synea1, float &synea2, float &synea3)
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

  _calcDetermineEulerAngles(init, step, phi, choose, synea1, synea2, synea3);
}

void CubicOps::determineHomochoricValues(int choose, float &r1, float &r2, float &r3)
{
  //float init1, init2, init3;
  float step[3];
  float phi[3];

  step[0] = CubicDim1StepValue/2.0f;
  step[1] = CubicDim2StepValue/2.0f;
  step[2] = CubicDim3StepValue/2.0f;
  phi[0] = static_cast<float>(choose % 18);
  phi[1] = static_cast<float>((choose / 18) % 18);
  phi[2] = static_cast<float>(choose / (18 * 18));

  return _calcDetermineHomochoricValues(step, phi, choose, r1, r2, r3);

}
int CubicOps::getOdfBin(float r1, float r2, float r3)
{
  float dim[3];
  float bins[3];

  dim[0] = CubicDim1InitValue;
  dim[1] = CubicDim2InitValue;
  dim[2] = CubicDim3InitValue;
  bins[0] = 18.0f;
  bins[1] = 18.0f;
  bins[2] = 18.0f;

  return _calcODFBin(dim, bins, r1, r2, r3);
}

