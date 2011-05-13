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
#include "HexagonalOps.h"
// Include this FIRST because there is a needed define for some compiles
// to expose some of the constants needed below
#include "AIM/Common/AIMMath.h"


  const static float m_pi = M_PI;
  static const float HexDim1InitValue = powf((0.75f*((m_pi/2.0f)-sinf((m_pi/2.0f)))),(1.0f/3.0f));
  static const float HexDim2InitValue = powf((0.75f*((m_pi/2.0f)-sinf((m_pi/2.0f)))),(1.0f/3.0f));
  static const float HexDim3InitValue = powf((0.75f*((m_pi/6.0f)-sinf((m_pi/6.0f)))),(1.0f/3.0f));
  static const float HexDim1StepValue = HexDim1InitValue/18.0f;
  static const float HexDim2StepValue = HexDim1InitValue/18.0f;
  static const float HexDim3StepValue = HexDim1InitValue/6.0f;
static const float HexQuatSym[12][5] = {{0.000000000, 0.000000000, 0.000000000, 0.000000000, 1.000000000},
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
                       {0.000000000, -0.86602540, 0.500000000, 0.000000000, 0.000000000}};
static const float HexRodSym[12][3] = {{0,0,0},
                    {0, 0, 0.57735},
                    {0, 0, 1.73205},
                    {0, 0, 10000000000.0},
                    {0, 0, -1.73205},
                    {0, 0, -0.57735},
                    {0, 10000000000.0, 10000000000.0},
                    {10000000000.0, 0, 0},
                    {0, 10000000000.0, 10000000000.0},
                    {0, 10000000000.0, 10000000000.0},
                    {0, 10000000000.0, 0},
                    {0, 10000000000.0, 10000000000.0}};


HexagonalOps::HexagonalOps()
{
}

HexagonalOps::~HexagonalOps()
{
}

float HexagonalOps::getMisoQuat( float q1[5],float q2[5],float &n1,float &n2,float &n3)
{
  int numsym = 0;
  float quatsym[24][5];

  numsym = 12;
  for (int i = 0; i < 12; i++)
  {
    for (int j = 0; j < 5; j++)
    {
      quatsym[i][j] = HexQuatSym[i][j];
    }
  }

  return _calcMisoQuat(quatsym, numsym, q1, q2, n1, n2, n3);
}


void HexagonalOps::getFZRod(float &r1,float &r2, float &r3)
{
  float rodsym[24][3];
  int numsym;
  numsym = 12;
  for(int i=0;i<12;i++)
  {
    for(int j=0;j<3;j++)
    {
      rodsym[i][j] = HexRodSym[i][j];
    }
  }
  _calcFZRod(rodsym, numsym,r1, r2, r3);
}

void HexagonalOps::getNearestQuat( float *q1, float *q2)
{
  int numsym = 0;
  float quatsym[24][5];

  numsym = 12;
     for(int i=0;i<12;i++)
     {
       for(int j=0;j<5;j++)
       {
         quatsym[i][j] = HexQuatSym[i][j];
       }
     }

    _calcNearestQuat(quatsym, numsym, q1, q2);
}

void HexagonalOps::getFZQuat(float *qr)
{
  int numsym = 0;
  float quatsym[24][5];

  numsym = 12;
  for(int i=0;i<12;i++)
  {
    for(int j=0;j<5;j++)
    {
      quatsym[i][j] = HexQuatSym[i][j];
    }
  }
    _calcFZQuat(quatsym, numsym, qr);

}


int HexagonalOps::getMisoBin(float n1, float n2, float n3)
{
  float dim[3];
  float bins[3];

  dim[0] = HexDim1InitValue;
  dim[1] = HexDim2InitValue;
  dim[2] = HexDim3InitValue;
  bins[0] = 36.0f;
  bins[1] = 36.0f;
  bins[2] = 12.0f;

  return _calcMisoBin(dim, bins, n1, n2, n3);
}


void HexagonalOps::determineEulerAngles(int choose, float &synea1, float &synea2, float &synea3)
{
  float init[3];
  float step[3];
  float phi[3];

  init[0] = HexDim1InitValue;
  init[1] = HexDim2InitValue;
  init[2] = HexDim3InitValue;
  step[0] = HexDim1StepValue;
  step[1] = HexDim2StepValue;
  step[2] = HexDim3StepValue;
  phi[0] = choose % 36;
  phi[1] = (choose / 36) % 36;
  phi[2] = choose / (36 * 36);

  _calcDetermineEulerAngles(init, step, phi, choose, synea1, synea2, synea3);
}


void HexagonalOps::determineHomochoricValues( int choose, float &r1, float &r2, float &r3)
{
  //float init1, init2, init3;
  float step[3];
  float phi[3];

  step[0] = HexDim1StepValue/2.0f;
  step[1] = HexDim2StepValue/2.0f;
  step[2] = HexDim3StepValue/2.0f;
  phi[0] = choose % 36;
  phi[1] = (choose / 36) % 36;
  phi[2] = choose / (36 * 36);

  return _calcDetermineHomochoricValues(step, phi, choose, r1, r2, r3);
}

int HexagonalOps::getOdfBin(float r1, float r2, float r3)
{
  float dim[3];
  float bins[3];

  dim[0] = HexDim1InitValue;
  dim[1] = HexDim2InitValue;
  dim[2] = HexDim3InitValue;
  bins[0] = 36.0f;
  bins[1] = 36.0f;
  bins[2] = 12.0f;

  return _calcODFBin(dim, bins, r1, r2, r3);
}







