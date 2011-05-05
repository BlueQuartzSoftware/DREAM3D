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

#include "MisoHexagonal.h"

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

MisoHexagonal::MisoHexagonal()
{
}

MisoHexagonal::~MisoHexagonal()
{
}



double MisoHexagonal::getQuaternion(double q1[5],double q2[5],double &n1,double &n2,double &n3)
{
  double wmin = 9999999; //,na,nb,nc;
  double w = 0;
  double n1min, n2min, n3min;
  double qr[5];
  double qc[5];

  qr[1] = -q1[1] * q2[4] + q1[4] * q2[1] - q1[2] * q2[3] + q1[3] * q2[2];
  qr[2] = -q1[2] * q2[4] + q1[4] * q2[2] - q1[3] * q2[1] + q1[1] * q2[3];
  qr[3] = -q1[3] * q2[4] + q1[4] * q2[3] - q1[1] * q2[2] + q1[2] * q2[1];
  qr[4] = -q1[4] * q2[4] - q1[1] * q2[1] - q1[2] * q2[2] - q1[3] * q2[3];
  for (int i = 0; i < 12; i++)
  {
    AIM::Quaternions::Hex_MultiplyByUnitQuaterion(qr, i, qc);
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


void MisoHexagonal::getFZRod(double &r1,double &r2, double &r3)
{
  double rodsym[12][3] = {{0,0,0},
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
  double denom, dist;
//  int index;
  double smallestdist = 100000000;
  double rc1, rc2, rc3;
  double r1min, r2min, r3min;
  for(int i=0;i<12;i++)
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

void MisoHexagonal::getNearestQuaternion(double* q1, double* q2)
{
  double dist = 0;
  double smallestdist = 1000000;
  double qc[5];
  double qmax[5];
  for(int i=0;i<5;i++)
  {
    qc[i] = q2[i];
  }
  for(int i=0;i<12;i++)
  {
      AIM::Quaternions::Hex_MultiplyByUnitQuaterion(q2, i, qc);
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


void MisoHexagonal::getFZQuaternion(double* qr)
{
  double dist = 0;
  double smallestdist = 1000000;
  double qc[5];
  double qmax[5];
  for(int i=0;i<5;i++)
  {
    qc[i] = qr[i];
  }
  for(int i=0;i<12;i++)
  {
      AIM::Quaternions::Hex_MultiplyByUnitQuaterion(qr, i, qc);
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

int MisoHexagonal::getMisoBin(double n1, double n2, double n3)
{
  double dim1 = HexDim1InitValue;
  double dim2 = HexDim2InitValue;
  double dim3 = HexDim3InitValue;
  size_t miso1bin = size_t(n1*36.0/dim1);
  size_t miso2bin = size_t(n2*36.0/dim2);
  size_t miso3bin = size_t(n3*12.0/dim3);
  if(miso1bin >= 36) miso1bin = 35;
  if(miso2bin >= 36) miso2bin = 35;
  if(miso3bin >= 12) miso3bin = 11;
  return ((36*36*miso3bin)+(36*miso2bin)+miso1bin);
}

size_t MisoHexagonal::calculateODFBin( double q1[5], double qref[5])
{
  double w;
  double n1;
  double n2;
  double n3;
//  double degtorad = m_pi / 180.0;
//  double denom;
  double dim1 = HexDim1InitValue;
  double dim2 = HexDim2InitValue;
  double dim3 = HexDim3InitValue;
  size_t g1euler1bin;
  size_t g1euler2bin;
  size_t g1euler3bin;
  size_t g1odfbin;

  w = getQuaternion(q1, qref, n1, n2, n3);
  calculateMisorientationAngles(w, n1, n2, n3);

  g1euler1bin = size_t(n1 * 36.0 / dim1);
  g1euler2bin = size_t(n2 * 36.0 / dim2);
  g1euler3bin = size_t(n3 * 12.0 / dim3);
  if (g1euler1bin >= 36) g1euler1bin = 35;
  if (g1euler2bin >= 36) g1euler2bin = 35;
  if (g1euler3bin >= 12) g1euler3bin = 11;
  g1odfbin = (g1euler3bin * 36 * 36) + (g1euler2bin * 36) + (g1euler1bin);
  return g1odfbin;
}




void MisoHexagonal::determineEulerAngles(int choose, double &synea1, double &synea2, double &synea3)
{
  double dim1, dim2, dim3;
  double phi1, PHI, phi2;
  AIMRandomNG rg;
  rg.RandomInit((static_cast<unsigned int> (time(NULL))));

  dim1 = HexDim1InitValue;
  dim2 = HexDim2InitValue;
  dim3 = HexDim3InitValue;
  phi1 = choose % 36;
  PHI = (choose / 36) % 36;
  phi2 = choose / (36 * 36);

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


void MisoHexagonal::initializeDims(double &dim1, double &dim2,  double &dim3, int &numbins)
{
  dim1 = HexDim1InitValue;
  dim2 = HexDim2InitValue;
  dim3 = HexDim3InitValue;
  numbins = 36*36*12;
}






















