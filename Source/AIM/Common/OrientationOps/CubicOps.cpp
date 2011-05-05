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

const int CubicOps::ODFSize;
const int CubicOps::MDFSize;

  const static double m_pi = M_PI;
  static const double CubicDim1InitValue = pow((0.75*((m_pi/4.0)-sin((m_pi/4.0)))),(1.0/3.0));
  static const double CubicDim2InitValue = pow((0.75*((m_pi/4.0)-sin((m_pi/4.0)))),(1.0/3.0));
  static const double CubicDim3InitValue = pow((0.75*((m_pi/4.0)-sin((m_pi/4.0)))),(1.0/3.0));
  static const double CubicDim1StepValue = CubicDim1InitValue/9.0;
  static const double CubicDim2StepValue = CubicDim2InitValue/9.0;
  static const double CubicDim3StepValue = CubicDim3InitValue/9.0;

  static const double CubicQuatSym[24][5] = {{0.000000000, 0.000000000, 0.000000000, 0.000000000, 1.000000000},
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
                       {0.000000000, 0.500000000, 0.500000000, -0.500000000, 0.500000000}};

  static const double CubicRodSym[24][3] = {{0,0,0},
                      {10000000.0, 0, 0},
                      {0, 10000000.0, 0},
                      {0, 0, 10000000.0},
                      {1.0, 0, 0},
                      {0, 1.0, 0},
                      {0, 0, 1.0},
                      {-1.0, 0, 0},
                      {0, -1.0, 0},
                      {0, 0, -1.0},
                      {10000000.0, 10000000.0, 0},
                      {-10000000.0, 10000000.0, 0},
                      {0, 10000000.0, 10000000.0},
                      {0, -10000000.0, 10000000.0},
                      {10000000.0, 0, 10000000.0},
                      {-10000000.0, 0, 10000000.0},
                      {1.0, 1.0, 1.0},
                      {-1.0, -1.0, -1.0},
                      {1.0, -1.0, 1.0},
                      {-1.0, 1.0, -1.0},
                      {-1.0, 1.0, 1.0},
                      {1.0, -1.0, -1.0},
                      {-1.0, -1.0, 1.0},
                      {1.0, 1.0, -1.0}};

CubicOps::CubicOps()
{
}

CubicOps::~CubicOps()
{
}


double CubicOps::getMisoQuat( double q1[5],double q2[5],double &n1,double &n2,double &n3)
{

  int numsym = 0;
  double quatsym[24][5];

  numsym = 24;
  for (int i = 0; i < 24; i++)
  {
    for (int j = 0; j < 5; j++)
    {
      quatsym[i][j] = CubicQuatSym[i][j];
    }
  }

  return _calcMisoQuat(quatsym, numsym, q1, q2, n1, n2, n3);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CubicOps::getFZRod(double &r1,double &r2, double &r3)
{
  double rodsym[24][3];
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CubicOps::getNearestQuat( double *q1, double *q2)
{
  int numsym = 0;
  double quatsym[24][5];

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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CubicOps::getFZQuat(double *qr)
{
  int numsym = 0;
  double quatsym[24][5];

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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CubicOps::getMisoBin(double n1, double n2, double n3)
{
  double dim[3];
  double bins[3];

  dim[0] = CubicDim1InitValue;
  dim[1] = CubicDim2InitValue;
  dim[2] = CubicDim3InitValue;
  bins[0] = 18.0;
  bins[1] = 18.0;
  bins[2] = 18.0;

  return _calcMisoBin(dim, bins, n1, n2, n3);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CubicOps::determineEulerAngles(int choose, double &synea1, double &synea2, double &synea3)
{
  double init[3];
  double step[3];
  double phi[3];

  init[0] = CubicDim1InitValue;
  init[1] = CubicDim2InitValue;
  init[2] = CubicDim3InitValue;
  step[0] = CubicDim1StepValue;
  step[1] = CubicDim2StepValue;
  step[2] = CubicDim3StepValue;
  phi[0] = choose % 18;
  phi[1] = (choose / 18) % 18;
  phi[2] = choose / (18 * 18);

  _calcDetermineEulerAngles(init, step, phi, choose, synea1, synea2, synea3);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CubicOps::determineAxisAngle( int choose, double &w, double &n1, double &n2, double &n3)
{
  //double init1, init2, init3;
  double step[3];
  double phi[3];

  step[0] = CubicDim1StepValue/2.0;
  step[1] = CubicDim2StepValue/2.0;
  step[2] = CubicDim3StepValue/2.0;
  phi[0] = choose % 18;
  phi[1] = (choose / 18) % 18;
  phi[2] = choose / (18 * 18);

  _calcDetermineAxisAngle(step, phi, choose, w, n1, n2, n3);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CubicOps::getOdfBin(double q1[5], double qref[5])
{
  double dim[3];
  double bins[3];

  dim[0] = CubicDim1InitValue;
  dim[1] = CubicDim2InitValue;
  dim[2] = CubicDim3InitValue;
  bins[0] = 18.0;
  bins[1] = 18.0;
  bins[2] = 18.0;

  return _calcODFBin(dim, bins, q1, qref);
}

