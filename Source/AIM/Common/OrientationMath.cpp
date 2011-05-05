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
#include "OrientationMath.h"

// Include this FIRST because there is a needed define for some compiles
// to expose some of the constants needed below
#include "AIM/Common/AIMMath.h"

#include "MXA/Common/LogTime.h"

#include "AIM/Common/OrientationOps/CubicOps.h"


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

  static const double CubicSlipSystems[12][6] = {{1,1,1,0,1,-1},
												{1,1,1,1,0,-1},
												{1,1,1,1,-1,0},
												{1,1,-1,1,-1,0},
												{1,1,-1,1,0,1},
												{1,1,-1,0,1,1},
												{1,-1,1,1,1,0},
												{1,-1,1,0,1,1},
												{1,-1,1,1,0,-1},
												{-1,1,1,1,1,0},
												{-1,1,1,1,0,1},
												{-1,1,1,0,1,-1}};


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



double OrientationMath::_calcMisoQuat(double quatsym[24][5], int numsym,
                  double q1[5], double q2[5],
                  double &n1, double &n2, double &n3){
  double wmin = 9999999; //,na,nb,nc;
  double w = 0;
  double n1min, n2min, n3min;
  double qr[5];
  double qc[5];

  qr[1] = -q1[1] * q2[4] + q1[4] * q2[1] - q1[2] * q2[3] + q1[3] * q2[2];
  qr[2] = -q1[2] * q2[4] + q1[4] * q2[2] - q1[3] * q2[1] + q1[1] * q2[3];
  qr[3] = -q1[3] * q2[4] + q1[4] * q2[3] - q1[1] * q2[2] + q1[2] * q2[1];
  qr[4] = -q1[4] * q2[4] - q1[1] * q2[1] - q1[2] * q2[2] - q1[3] * q2[3];
  for (int i = 0; i < numsym; i++)
  {
	  OrientationMath::multiplyQuaternions(qr, quatsym[i], qc);
    if (qc[4] < -1) qc[4] = -1;
    if (qc[4] > 1) qc[4] = 1;
    w = acos(qc[4]);
    w = 2 * w;
    n1 = qc[1] / sin(w / 2.0);
    n2 = qc[2] / sin(w / 2.0);
    n3 = qc[3] / sin(w / 2.0);
    if (w > m_pi) w = (2 * m_pi) - w;
    if (w < wmin)
    {
      wmin = w;
      n1min = n1;
      n2min = n2;
      n3min = n3;
    }
  }
  n1 = fabs(n1min);
  n2 = fabs(n2min);
  n3 = fabs(n3min);
  double denom = pow((n1*n1+n2*n2+n3*n3),0.5);
  n1 = n1/denom;
  n2 = n2/denom;
  n3 = n3/denom;
  double newangle = 0;
  double angle = 180*atan(n2/n1)/m_pi;
  if(angle > 30.0)
  {
  if(int(angle/30)%2 == 0)
  {
    newangle = angle - (30*int(angle/30));
    newangle = newangle*m_pi/180.0;
    n1 = cos(newangle);
    n2 = sin(newangle);
  }
  if(int(angle/30)%2 == 1)
  {
    newangle = angle - (30*int(angle/30));
    newangle = 30 - newangle;
    newangle = newangle*m_pi/180.0;
    n1 = cos(newangle);
    n2 = sin(newangle);
  }
  }
  wmin = (180.0 / m_pi) * wmin;
  return wmin;
}


void OrientationMath::_calcFZRod(double rodsym[24][3], int numsym, double &r1,double &r2, double &r3)
{
  double denom, dist;
//  int index;
  double smallestdist = 100000000;
  double rc1, rc2, rc3;
  double r1min, r2min, r3min;
  for(int i=0;i<numsym;i++)
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

void OrientationMath::_calcNearestQuat(double quatsym[24][5], int numsym, double *q1, double *q2)
{
  double dist = 0;
  double smallestdist = 1000000;
  double qc[5];
  double qmax[5];


  for(int i=0;i<5;i++)
  {
    qc[i] = q2[i];
  }
  for(int i=0;i<numsym;i++)
  {
	OrientationMath::multiplyQuaternions(q2, quatsym[i], qc);
    dist = 2.0*(1-(qc[4]*q1[4]+qc[1]*q1[1]+qc[2]*q1[2]+qc[3]*q1[3]));
    if(dist < smallestdist)
    {
      smallestdist = dist;
      qmax[0] = qc[0];
      qmax[1] = qc[1];
      qmax[2] = qc[2];
      qmax[3] = qc[3];
      qmax[4] = qc[4];
    }
  }
  q2[0] = qmax[0];
  q2[1] = qmax[1];
  q2[2] = qmax[2];
  q2[3] = qmax[3];
  q2[4] = qmax[4];
  if(q2[4] < 0)
  {
    q2[0] = q2[0];
    q2[1] = -q2[1];
    q2[2] = -q2[2];
    q2[3] = -q2[3];
    q2[4] = -q2[4];
  }
}

void OrientationMath::_calcFZQuat(double quatsym[24][5], int numsym, double *qr)
{
  double dist = 0;
  double smallestdist = 1000000;
  double qc[5];
  double qmax[5];

  for(int i=0;i<5;i++)
  {
    qc[i] = qr[i];
  }
  for(int i=0;i<numsym;i++)
  {
	OrientationMath::multiplyQuaternions(qr, quatsym[i], qc);
    dist = qc[1]*qc[1]+qc[2]*qc[2]+qc[3]*qc[3];
    if(dist < smallestdist)
    {
      smallestdist = dist;
      qmax[0] = qc[0];
      qmax[1] = qc[1];
      qmax[2] = qc[2];
      qmax[3] = qc[3];
      qmax[4] = qc[4];
    }
  }
  qr[0] = qmax[0];
  qr[1] = qmax[1];
  qr[2] = qmax[2];
  qr[3] = qmax[3];
  qr[4] = qmax[4];
  if(qr[4] < 0)
  {
    qr[0] = qr[0];
    qr[1] = -qr[1];
    qr[2] = -qr[2];
    qr[3] = -qr[3];
    qr[4] = -qr[4];
  }
}


int OrientationMath::_calcMisoBin(double dim[3], double bins[3], double n1, double n2, double n3)
{
  size_t miso1bin = size_t(n1*bins[0]/dim[0]);
  size_t miso2bin = size_t(n2*bins[1]/dim[1]);
  size_t miso3bin = size_t(n3*bins[2]/dim[2]);
  if(miso1bin >= bins[0]) miso1bin = bins[0]-1;
  if(miso2bin >= bins[1]) miso2bin = bins[1]-1;
  if(miso3bin >= bins[2]) miso3bin = bins[2]-1;
  return ((18*18*miso3bin)+(18*miso2bin)+miso1bin);
}



void OrientationMath::_calcDetermineEulerAngles(double init[3], double step[3], double phi[3],
                                                int choose, double &synea1, double &synea2, double &synea3)
{
  AIMRandomNG rg;
  rg.RandomInit((static_cast<unsigned int> (MXA::getMilliSeconds())));
  double random = rg.Random();
  double synh1 = (step[0] * phi[0]) + (step[0] * random) - (init[0]);
  random = rg.Random();
  double synh2 = (step[1] * phi[1]) + (step[1] * random) - (init[1]);
  random = rg.Random();
  double synh3 = (step[2] * phi[2]) + (step[2] * random) - (init[2]);
  double hmag = pow((synh1 * synh1 + synh2 * synh2 + synh3 * synh3), 0.5);
  double angle = pow((8 * hmag * hmag * hmag), (1.0 / 3.0));
  double synr1 = tan(angle / 2.0) * (synh1 / hmag);
  double synr2 = tan(angle / 2.0) * (synh2 / hmag);
  double synr3 = tan(angle / 2.0) * (synh3 / hmag);
  double sum = atan(synr3);
  double diff = atan(synr2 / synr1);
  synea1 = sum + diff;
  synea2 = 2. * atan(synr1 * cos(sum) / cos(diff));
  synea3 = sum - diff;
}


void OrientationMath::_calcDetermineAxisAngle( double step[3], double phi[3],
                                               int choose, double &w, double &n1, double &n2, double &n3)
{
  AIMRandomNG rg;
  rg.RandomInit((static_cast<unsigned int> (MXA::getMilliSeconds())));
  double random = rg.Random();
  double synh1 = (step[0] * phi[0]) + (step[0] * random);
  random = rg.Random();
  double synh2 = (step[1] * phi[1]) + (step[1] * random);
  random = rg.Random();
  double synh3 = (step[2] * phi[2]) + (step[2] * random);
  double hmag = pow((synh1 * synh1 + synh2 * synh2 + synh3 * synh3), 0.5);
  double angle = pow((8 * hmag * hmag * hmag), (1.0 / 3.0));
  w = angle * 180.0 / M_PI;
  if (w > 65)
  {
    int stop = 0;
  }
}


int OrientationMath::_calcODFBin(double dim[3], double bins[3], double q1[5], double qref[5])
{
  double w;
  double n1, n2, n3;

  CubicOps c;
  w = c.getMisoQuat(q1, qref, n1, n2, n3);
  OrientationMath::axisAngletoHomochoric(w, n1, n2, n3);


  size_t g1euler1bin;
  size_t g1euler2bin;
  size_t g1euler3bin;
  size_t g1odfbin;
  g1euler1bin = size_t(n1*bins[0]/dim[0]);
  g1euler2bin = size_t(n2*bins[1]/dim[1]);
  g1euler3bin = size_t(n3*bins[2]/dim[2]);
  if(g1euler1bin >= bins[0]) g1euler1bin = bins[0]-1;
  if(g1euler2bin >= bins[1]) g1euler2bin = bins[1]-1;
  if(g1euler3bin >= bins[2]) g1euler3bin = bins[2]-1;
  g1odfbin = (g1euler3bin*bins[0]*bins[1])+(g1euler2bin*bins[0])+(g1euler1bin);
  return g1odfbin;
}

void OrientationMath::axisAngletoHomochoric(double &w, double &miso1, double &miso2, double &miso3)
{
  double degtorad = m_pi / 180.0;
  double denom;
  double n1 = miso1;
  double n2 = miso2;
  double n3 = miso3;

  w = w * degtorad;
  denom = (n1 * n1) + (n2 * n2) + (n3 * n3);
  denom = pow(denom, 0.5);
  n1 = n1 / denom;
  n2 = n2 / denom;
  n3 = n3 / denom;
  n1 = n1 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
  n2 = n2 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
  n3 = n3 * pow(((3.0 / 4.0) * (w - sin(w))), (1.0 / 3.0));
  miso1 = n1;
  miso2 = n2;
  miso3 = n3;
}


void OrientationMath::eulertoQuat(double* q, double e1, double e2, double e3)
{
  double s, c, s1, c1, s2, c2;
  s = sin(0.5 * e2);
  c = cos(0.5 * e2);
  s1 = sin(0.5 * (e1 - e3));
  c1 = cos(0.5 * (e1 - e3));
  s2 = sin(0.5 * (e1 + e3));
  c2 = cos(0.5 * (e1 + e3));
  q[1] = s*c1;
  q[2] = s*s1;
  q[3] = c*s2;
  q[4] = c*c2;
}



void OrientationMath::multiplyQuaternions(double* inQuat, double* multQuat, double* outQuat)
{
  outQuat[1]=multQuat[4]*inQuat[1]+multQuat[1]*inQuat[4]+multQuat[3]*inQuat[2]-multQuat[2]*inQuat[3];
  outQuat[2]=multQuat[4]*inQuat[2]+multQuat[2]*inQuat[4]+multQuat[1]*inQuat[3]-multQuat[3]*inQuat[1];
  outQuat[3]=multQuat[4]*inQuat[3]+multQuat[3]*inQuat[4]+multQuat[2]*inQuat[1]-multQuat[1]*inQuat[2];
  outQuat[4]=multQuat[4]*inQuat[4]-multQuat[1]*inQuat[1]-multQuat[2]*inQuat[2]-multQuat[3]*inQuat[3];
}

void OrientationMath::getSlipMisalignment(int ss1, double q1[5], double q2[5], double &ssap)
{
  double g1[3][3];
  double g2[3][3];
  double maxssap = 0;
  double h1, k1, l1, u1, v1, w1;
  double h2, k2, l2, u2, v2, w2;
  double denomhkl1, denomhkl2, denomuvw1, denomuvw2;
  double planemisalignment, directionmisalignment;
  g1[0][0] = 1-(2*q1[2]*q1[2])-(2*q1[3]*q1[3]);
  g1[0][1] = (2*q1[1]*q1[2])-(2*q1[3]*q1[4]);
  g1[0][2] = (2*q1[1]*q1[3])+(2*q1[2]*q1[4]);
  g1[1][0] = (2*q1[1]*q1[2])+(2*q1[3]*q1[4]);
  g1[1][1] = 1-(2*q1[1]*q1[1])-(2*q1[3]*q1[3]);
  g1[1][2] = (2*q1[2]*q1[3])-(2*q1[1]*q1[4]);
  g1[2][0] = (2*q1[1]*q1[3])-(2*q1[2]*q1[4]);
  g1[2][1] = (2*q1[2]*q1[3])+(2*q1[1]*q1[4]);
  g1[2][2] = 1-(2*q1[1]*q1[1])-(2*q1[2]*q1[2]);
  g2[0][0] = 1-(2*q2[2]*q2[2])-(2*q2[3]*q2[3]);
  g2[0][1] = (2*q2[1]*q2[2])-(2*q2[3]*q2[4]);
  g2[0][2] = (2*q2[1]*q2[3])+(2*q2[2]*q2[4]);
  g2[1][0] = (2*q2[1]*q2[2])+(2*q2[3]*q2[4]);
  g2[1][1] = 1-(2*q2[1]*q2[1])-(2*q2[3]*q2[3]);
  g2[1][2] = (2*q2[2]*q2[3])-(2*q2[1]*q2[4]);
  g2[2][0] = (2*q2[1]*q2[3])-(2*q2[2]*q2[4]);
  g2[2][1] = (2*q2[2]*q2[3])+(2*q2[1]*q2[4]);
  g2[2][2] = 1-(2*q2[1]*q2[1])-(2*q2[2]*q2[2]);
  // Note the order of multiplication is such that I am actually multiplying by the inverse of g1 and g2
  h1 = CubicSlipSystems[ss1][0]*g1[0][0]+CubicSlipSystems[ss1][1]*g1[1][0]+CubicSlipSystems[ss1][2]*g1[2][0];
  k1 = CubicSlipSystems[ss1][0]*g1[0][1]+CubicSlipSystems[ss1][1]*g1[1][1]+CubicSlipSystems[ss1][2]*g1[2][1];
  l1 = CubicSlipSystems[ss1][0]*g1[0][2]+CubicSlipSystems[ss1][1]*g1[1][2]+CubicSlipSystems[ss1][2]*g1[2][2];
  u1 = CubicSlipSystems[ss1][3]*g1[0][0]+CubicSlipSystems[ss1][4]*g1[1][0]+CubicSlipSystems[ss1][5]*g1[2][0];
  v1 = CubicSlipSystems[ss1][3]*g1[0][1]+CubicSlipSystems[ss1][4]*g1[1][1]+CubicSlipSystems[ss1][5]*g1[2][1];
  w1 = CubicSlipSystems[ss1][3]*g1[0][2]+CubicSlipSystems[ss1][4]*g1[1][2]+CubicSlipSystems[ss1][5]*g1[2][2];
  denomhkl1 = pow((h1*h1+k1*k1+l1*l1),0.5);
  denomuvw1 = pow((u1*u1+v1*v1+w1*w1),0.5);
  for(int i=0;i<12;i++)
  {
    h2 = CubicSlipSystems[i][0]*g2[0][0]+CubicSlipSystems[i][1]*g2[1][0]+CubicSlipSystems[i][2]*g2[2][0];
    k2 = CubicSlipSystems[i][0]*g2[0][1]+CubicSlipSystems[i][1]*g2[1][1]+CubicSlipSystems[i][2]*g2[2][1];
    l2 = CubicSlipSystems[i][0]*g2[0][2]+CubicSlipSystems[i][1]*g2[1][2]+CubicSlipSystems[i][2]*g2[2][2];
    u2 = CubicSlipSystems[i][3]*g2[0][0]+CubicSlipSystems[i][4]*g2[1][0]+CubicSlipSystems[i][5]*g2[2][0];
    v2 = CubicSlipSystems[i][3]*g2[0][1]+CubicSlipSystems[i][4]*g2[1][1]+CubicSlipSystems[i][5]*g2[2][1];
    w2 = CubicSlipSystems[i][3]*g2[0][2]+CubicSlipSystems[i][4]*g2[1][2]+CubicSlipSystems[i][5]*g2[2][2];
    denomhkl2 = pow((h2*h2+k2*k2+l2*l2),0.5);
    denomuvw2 = pow((u2*u2+v2*v2+w2*w2),0.5);
    planemisalignment = fabs((h1*h2+k1*k2+l1*l2)/(denomhkl1*denomhkl2));
    directionmisalignment = fabs((u1*u2+v1*v2+w1*w2)/(denomuvw1*denomuvw2));
    ssap = planemisalignment*directionmisalignment;
    if(ssap > maxssap) maxssap = ssap;
  }
  ssap = maxssap;
}
