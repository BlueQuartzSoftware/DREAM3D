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

#include "MisoOrthoRhombic.h"

#include <assert.h>

#include "AIM/Common/AIMMath.h"

#include "AIM/Common/Quaternions.h"

const static double m_pi = M_PI;
const static double m_OnePointThree = 1.33333333333;

const double threesixty_over_pi = 360.0/m_pi;
const double sqrt_two = pow(2.0, 0.5);

const double acos_neg_one = acos(-1.0);
const double acos_pos_one = acos(1.0);
const double sin_wmin_neg_1_over_2 = sin(acos_neg_one/2.0);
const double sin_wmin_pos_1_over_2 = sin(acos_pos_one/2.0);


const static double SinOfHalf = sin(0.5);
const static double CosOfHalf = cos(0.5);
const static double SinOfZero = sin(0.0);
const static double CosOfZero = cos(0.0);

static const double HexDim1InitValue = pow((0.75*((m_pi/2.0)-sin((m_pi/2.0)))),(1.0/3.0));
static const double HexDim2InitValue = pow((0.75*((m_pi/2.0)-sin((m_pi/2.0)))),(1.0/3.0));
static const double HexDim3InitValue = pow((0.75*((m_pi/6.0)-sin((m_pi/6.0)))),(1.0/3.0));

MisoOrthoRhombic::MisoOrthoRhombic()
{
}

MisoOrthoRhombic::~MisoOrthoRhombic()
{
}



double MisoOrthoRhombic::getQuaternion(double q1[5],double q2[5],double &n1,double &n2,double &n3)
{
  assert(false);
  return 0.0;
}


void MisoOrthoRhombic::getFZRod(double &r1,double &r2, double &r3)
{
  double rodsym[4][3] = {{0,0,0},
  {10000000.0, 0, 0},
  {0, 10000000.0, 0},
  {0, 0, 10000000.0}};
  double denom, dist;
//  int index;
  double smallestdist = 100000000;
  double rc1, rc2, rc3;
  double r1min, r2min, r3min;
  for(int i=0;i<4;i++)
  {
    denom = 1-(r1*rodsym[i][0]+r2*rodsym[i][1]+r3*rodsym[i][2]);
    rc1 = (r1+rodsym[i][0]-(r3*rodsym[i][1]-r2*rodsym[i][2]))/denom;
    rc2 = (r2+rodsym[i][1]-(r1*rodsym[i][2]-r3*rodsym[i][0]))/denom;
    rc3 = (r3+rodsym[i][2]-(r2*rodsym[i][0]-r1*rodsym[i][1]))/denom;
    dist = rc1*rc1+rc2*rc2+rc3*rc3;
    if(dist < smallestdist)
    {
      smallestdist = dist;
      r1min = rc1;
      r2min = rc2;
      r3min = rc3;
    }
  }
  r1 = r1min;
  r2 = r2min;
  r3 = r3min;
}

void MisoOrthoRhombic::getNearestQuaternion(double* q1, double* q2)
{
  assert(false);
}


void MisoOrthoRhombic::getFZQuaternion(double* qr)
{
  assert(false);
}

int MisoOrthoRhombic::getMisoBin(double n1, double n2, double n3)
{
  assert(false);
  return 0;
}

size_t MisoOrthoRhombic::calculateODFBin( double q1[5], double qref[5])
{
  assert(false);
  return 0;
}

void MisoOrthoRhombic::determineEulerAngles(int choose, double &synea1, double &synea2, double &synea3)
{
  assert(false);
}

void MisoOrthoRhombic::initializeDims(double &dim1, double &dim2,  double &dim3, int &numbins)
{
  assert(false);
}






















