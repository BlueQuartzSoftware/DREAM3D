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


  const static double m_pi = M_PI;
  static const double HexDim1InitValue = pow((0.75*((m_pi/2.0)-sin((m_pi/2.0)))),(1.0/3.0));
  static const double HexDim2InitValue = pow((0.75*((m_pi/2.0)-sin((m_pi/2.0)))),(1.0/3.0));
  static const double HexDim3InitValue = pow((0.75*((m_pi/6.0)-sin((m_pi/6.0)))),(1.0/3.0));
  static const double HexDim1StepValue = HexDim1InitValue/18.0;
  static const double HexDim2StepValue = HexDim1InitValue/18.0;
  static const double HexDim3StepValue = HexDim1InitValue/6.0;
static const double HexQuatSym[12][5] = {{0.000000000, 0.000000000, 0.000000000, 0.000000000, 1.000000000},
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
static const double HexRodSym[12][3] = {{0,0,0},
                    {0, 0, 0.57735},
                    {0, 0, 1.73205},
                    {0, 0, 10000000.0},
                    {0, 0, -1.73205},
                    {0, 0, -0.57735},
                    {0, 10000000.0, 10000000.0},
                    {10000000.0, 0, 0},
                    {0, 10000000.0, 10000000.0},
                    {0, 10000000.0, 10000000.0},
                    {0, 10000000.0, 0},
                    {0, 10000000.0, 10000000.0}};


HexagonalOps::HexagonalOps()
{
}

HexagonalOps::~HexagonalOps()
{
}

double HexagonalOps::getMisoQuat( double q1[5],double q2[5],double &n1,double &n2,double &n3)
{
  int numsym = 0;
  double quatsym[24][5];

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


void HexagonalOps::getFZRod(double &r1,double &r2, double &r3)
{
  double rodsym[24][3];
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

void HexagonalOps::getNearestQuat( double *q1, double *q2)
{
  int numsym = 0;
  double quatsym[24][5];

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

void HexagonalOps::getFZQuat(double *qr)
{
  int numsym = 0;
  double quatsym[24][5];

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


int HexagonalOps::getMisoBin(double n1, double n2, double n3)
{
  double dim[3];
  double bins[3];

  dim[0] = HexDim1InitValue;
  dim[1] = HexDim2InitValue;
  dim[2] = HexDim3InitValue;
  bins[0] = 36.0;
  bins[1] = 36.0;
  bins[2] = 12.0;

  return _calcMisoBin(dim, bins, n1, n2, n3);
}


void HexagonalOps::determineEulerAngles(int choose, double &synea1, double &synea2, double &synea3)
{
  double init[3];
  double step[3];
  double phi[3];

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


void HexagonalOps::determineHomochoricValues( int choose, double &r1, double &r2, double &r3)
{
  //double init1, init2, init3;
  double step[3];
  double phi[3];

  step[0] = HexDim1StepValue/2.0;
  step[1] = HexDim2StepValue/2.0;
  step[2] = HexDim3StepValue/2.0;
  phi[0] = choose % 36;
  phi[1] = (choose / 36) % 36;
  phi[2] = choose / (36 * 36);

  return _calcDetermineHomochoricValues(step, phi, choose, r1, r2, r3);
}

int HexagonalOps::getOdfBin(double r1, double r2, double r3)
{
  double dim[3];
  double bins[3];

  dim[0] = HexDim1InitValue;
  dim[1] = HexDim2InitValue;
  dim[2] = HexDim3InitValue;
  bins[0] = 36.0;
  bins[1] = 36.0;
  bins[2] = 12.0;

  return _calcODFBin(dim, bins, r1, r2, r3);
}







