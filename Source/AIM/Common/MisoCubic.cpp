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
#include "MisoCubic.h"
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

static const double CubicDim1InitValue = pow((0.75*((m_pi/4.0)-sin((m_pi/4.0)))),(1.0/3.0));
static const double CubicDim2InitValue = pow((0.75*((m_pi/4.0)-sin((m_pi/4.0)))),(1.0/3.0));
static const double CubicDim3InitValue = pow((0.75*((m_pi/4.0)-sin((m_pi/4.0)))),(1.0/3.0));



MisoCubic::MisoCubic()
{
}

MisoCubic::~MisoCubic()
{
}

double MisoCubic::getQuaternion(double q1[5],double q2[5],double &n1,double &n2,double &n3)
{

  double wmin=9999999.0; //,na,nb,nc;
  double qc[5];
  double qco[5];
  double sin_wmin_over_2 = 0.0;
//  double _1, _2,  _6;


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
    sin_wmin_over_2 = sin(wmin / 2.0);
  }

  n1 = qco[1] / sin_wmin_over_2;
  n2 = qco[2] / sin_wmin_over_2;
  n3 = qco[3] / sin_wmin_over_2;
  double denom = pow((n1*n1+n2*n2+n3*n3),0.5);
  n1 = n1/denom;
  n2 = n2/denom;
  n3 = n3/denom;
  if(wmin == 0) n1 = 0.0, n2 = 0.0, n3 = 1.0;
  wmin = (threesixty_over_pi) * wmin;
  return wmin;

}


void MisoCubic::getFZRod(double &r1,double &r2, double &r3)
{

  double rodsym[24][3] = {{0,0,0},
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
  double denom, dist;
//  int index;
  double smallestdist = 100000000;
  double rc1, rc2, rc3;
  double r1min, r2min, r3min;
  for(int i=0;i<24;i++)
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

void MisoCubic::getNearestQuaternion(double* q1, double* q2)
{

  double dist = 0;
  double smallestdist = 1000000;
  double qc[5];
  double qmax[5];
  for(int i=0;i<5;i++)
  {
    qc[i] = q2[i];
  }
  for(int i=0;i<24;i++)
  {
    AIM::Quaternions::Cubic_MultiplyByUnitQuaterion(q2, i, qc);
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


void MisoCubic::getFZQuaternion(double* qr)
{

  double dist = 0;
  double smallestdist = 1000000;
  double qc[5];
  double qmax[5];
  for(int i=0;i<5;i++)
  {
    qc[i] = qr[i];
  }
  for(int i=0;i<24;i++)
  {
      AIM::Quaternions::Cubic_MultiplyByUnitQuaterion(qr, i, qc);
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

int MisoCubic::getMisoBin(double n1, double n2, double n3)
{

  double dim1 = CubicDim1InitValue;
  double dim2 = CubicDim2InitValue;
  double dim3 = CubicDim3InitValue;
  size_t miso1bin = size_t(n1*18.0/dim1);
  size_t miso2bin = size_t(n2*18.0/dim2);
  size_t miso3bin = size_t(n3*18.0/dim3);
  if(miso1bin >= 18) miso1bin = 17;
  if(miso2bin >= 18) miso2bin = 17;
  if(miso3bin >= 18) miso3bin = 17;
  return ((18*18*miso3bin)+(18*miso2bin)+miso1bin);

}

size_t MisoCubic::calculateODFBin( double q1[5], double qref[5])
{

  double w;
  double n1;
  double n2;
  double n3;
  double dim1 = CubicDim1InitValue;
  double dim2 = CubicDim2InitValue;
  double dim3 = CubicDim3InitValue;
  size_t g1euler1bin;
  size_t g1euler2bin;
  size_t g1euler3bin;
  size_t g1odfbin;

  w = getQuaternion(q1, qref, n1, n2, n3);
  calculateMisorientationAngles(w, n1, n2, n3);

  g1euler1bin = size_t(n1*18.0/dim1);
  g1euler2bin = size_t(n2*18.0/dim2);
  g1euler3bin = size_t(n3*18.0/dim3);
  if(g1euler1bin >= 18) g1euler1bin = 17;
  if(g1euler2bin >= 18) g1euler2bin = 17;
  if(g1euler3bin >= 18) g1euler3bin = 17;
  g1odfbin = (g1euler3bin*18*18)+(g1euler2bin*18)+(g1euler1bin);
  return g1odfbin;

}


void MisoCubic::determineEulerAngles(int choose, double &synea1, double &synea2, double &synea3)
{
  double dim1, dim2, dim3;
  double phi1, PHI, phi2;
  AIMRandomNG rg;
  rg.RandomInit((static_cast<unsigned int> (time(NULL))));

  dim1 = CubicDim1InitValue;
  dim2 = CubicDim2InitValue;
  dim3 = CubicDim3InitValue;
  phi1 = choose % 18;
  PHI = (choose / 18) % 18;
  phi2 = choose / (18 * 18);
  double random = rg.Random();
  double synh1 = (dim1 * phi1) + (dim1 * random);
  random = rg.Random();
  double synh2 = (dim2 * PHI) + (dim2 * random);
  random = rg.Random();
  double synh3 = (dim3 * phi2) + (dim3 * random);
  double hmag = pow((synh1 * synh1 + synh2 * synh2 + synh3 * synh3), 0.5);
  double angle = pow((8.25 * hmag * hmag * hmag), (1.0 / 3.0));
  double synr1 = tan(angle / 2.0) * (synh1 / hmag);
  double synr2 = tan(angle / 2.0) * (synh2 / hmag);
  double synr3 = tan(angle / 2.0) * (synh3 / hmag);
  double sum = atan(synr3);
  double diff = atan(synr2 / synr1);
  synea1 = sum + diff;
  synea2 = 2. * atan(synr1 * cos(sum) / cos(diff));
  synea3 = sum - diff;
}

void MisoCubic::initializeDims(double &dim1, double &dim2,  double &dim3, int &numbins)
{
  dim1 = CubicDim1InitValue;
  dim2 = CubicDim2InitValue;
  dim3 = CubicDim3InitValue;
  numbins = 18*18*18;
}


