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
#include "DREAM3D/Common/AIMMath.h"

#include "MXA/Common/LogTime.h"

#include "DREAM3D/Common/OrientationOps/CubicOps.h"


	const static float m_pi = M_PI;
	const static float two_pi = 2.0f * m_pi;
	const static float recip_pi = 1.0f/m_pi;
	const static float pi_over_180 = m_pi/180.0f;

	const static float m_OnePointThree = 1.33333333333f;

	const float threesixty_over_pi = 360.0f/m_pi;
	const float oneeighty_over_pi = 180.0f/m_pi;
	const float sqrt_two = powf(2.0, 0.5);

	const float acos_neg_one = acosf(-1.0);
	const float acos_pos_one = acosf(1.0);
	const float sin_wmin_neg_1_over_2 = sinf(acos_neg_one/2.0);
	const float sin_wmin_pos_1_over_2 = sinf(acos_pos_one/2.0);
  const float sin_of_acos_neg_1 = sinf(acos_neg_one);
  const float sin_of_acos_pos_1 = sinf(acos_pos_one);

  const float recip_sin_of_acos_neg_1 = 1.0/sin_of_acos_neg_1;
  const float recip_sin_of_acos_pos_1 = 1.0/sin_of_acos_pos_1;

	const static float SinOfHalf = sinf(0.5);
	const static float CosOfHalf = cosf(0.5);
	const static float SinOfZero = sinf(0.0);
	const static float CosOfZero = cosf(0.0);

  static const float CubicSlipSystems[12][6] = {{1,1,1,0,1,-1},
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

float OrientationMath::_calcMisoQuat(const float quatsym[24][5], int numsym,
                                      float q1[5], float q2[5],
                                      float &n1, float &n2, float &n3)
{
  float wmin = 9999999.0f; //,na,nb,nc;
  float w = 0;
  float n1min, n2min, n3min;
  float qr[5];
  float qc[5];
  float temp;

  qr[1] = -q1[1] * q2[4] + q1[4] * q2[1] - q1[2] * q2[3] + q1[3] * q2[2];
  qr[2] = -q1[2] * q2[4] + q1[4] * q2[2] - q1[3] * q2[1] + q1[1] * q2[3];
  qr[3] = -q1[3] * q2[4] + q1[4] * q2[3] - q1[1] * q2[2] + q1[2] * q2[1];
  qr[4] = -q1[4] * q2[4] - q1[1] * q2[1] - q1[2] * q2[2] - q1[3] * q2[3];
  for (int i = 0; i < numsym; i++)
  {
	//  OrientationMath::multiplyQuaternions(qr, quatsym[i], qc);
	  MULT_QUAT(qr, quatsym[i], qc)
    if (qc[4] < -1) {
      qc[4] = -1;
      w = acos_neg_one;
      temp = recip_sin_of_acos_neg_1;
    }
    else if (qc[4] > 1) {
      qc[4] = 1;
      w = acos_pos_one;
      temp = recip_sin_of_acos_pos_1;
    }
    else {
      /* ***************************************************
       * THIS SECTION IS TAKING A LOT OF TIME DURING THE RUNS. If this is truly
       * the code path that needs to happen we might consider a lookup table at
       * the expense of some memory but it would avoid the acos and sin functions
       */
      w = acos(qc[4]);
      temp = 1.0/sinf(w);
    }

    n1 = qc[1] * temp;
    n2 = qc[2] * temp;
    n3 = qc[3] * temp;
    w = 2 * w;
    if (w > m_pi) {
      w = two_pi - w;
    }
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
  float denom = sqrt((n1*n1+n2*n2+n3*n3));
  n1 = n1/denom;
  n2 = n2/denom;
  n3 = n3/denom;
  float newangle = 0;
  float angle = 180*atan(n2/n1) * recip_pi;
  if(angle > 30.0)
  {
  if(int(angle/30)%2 == 0)
  {
    newangle = angle - (30*int(angle/30));
    newangle = newangle* pi_over_180;
    n1 = cosf(newangle);
    n2 = sinf(newangle);
  }
  if(int(angle/30)%2 == 1)
  {
    newangle = angle - (30*int(angle/30));
    newangle = 30 - newangle;
    newangle = newangle* pi_over_180;
    n1 = cosf(newangle);
    n2 = sinf(newangle);
  }
  }
  wmin = oneeighty_over_pi * wmin;
  return wmin;
}


void OrientationMath::_calcFZRod(const float rodsym[24][3], int numsym, float &r1,float &r2, float &r3)
{
  float denom, dist;
//  int index;
  float smallestdist = 100000000.0f;
  float rc1, rc2, rc3;
  float r1min, r2min, r3min;
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

void OrientationMath::_calcNearestQuat(const float quatsym[24][5], int numsym, float *q1, float *q2)
{
  float dist = 0;
  float smallestdist = 1000000.0f;
  float qc[5];
  float qmax[5];


  for(int i=0;i<5;i++)
  {
    qc[i] = q2[i];
  }
  for(int i=0;i<numsym;i++)
  {
//	OrientationMath::multiplyQuaternions(q2, quatsym[i], qc);
  MULT_QUAT(q2, quatsym[i], qc);
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
 //   q2[0] = q2[0];
    q2[1] = -q2[1];
    q2[2] = -q2[2];
    q2[3] = -q2[3];
    q2[4] = -q2[4];
  }
}

void OrientationMath::_calcFZQuat(const float quatsym[24][5], int numsym, float *qr)
{
  float dist = 0;
  float smallestdist = 1000000.0f;
  float qc[5];
  float qmax[5];

  for(int i=0;i<5;i++)
  {
    qc[i] = qr[i];
  }
  for(int i=0;i<numsym;i++)
  {
    //OrientationMath::multiplyQuaternions(qr, quatsym[i], qc);
	  MULT_QUAT(qr, quatsym[i], qc);
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
   // qr[0] = qr[0];
    qr[1] = -qr[1];
    qr[2] = -qr[2];
    qr[3] = -qr[3];
    qr[4] = -qr[4];
  }
}


int OrientationMath::_calcMisoBin(float dim[3], float bins[3], float n1, float n2, float n3)
{
  size_t miso1bin = size_t(n1*bins[0]/dim[0]);
  size_t miso2bin = size_t(n2*bins[1]/dim[1]);
  size_t miso3bin = size_t(n3*bins[2]/dim[2]);
  if(miso1bin >= bins[0]) miso1bin = bins[0]-1;
  if(miso2bin >= bins[1]) miso2bin = bins[1]-1;
  if(miso3bin >= bins[2]) miso3bin = bins[2]-1;
  return ((18*18*miso3bin)+(18*miso2bin)+miso1bin);
}



void OrientationMath::_calcDetermineEulerAngles(float init[3], float step[3], float phi[3],
                                                int choose, float &synea1, float &synea2, float &synea3)
{
  AIM_RANDOMNG_NEW()
  float random = rg.Random();
  float synh1 = (step[0] * phi[0]) + (step[0] * random) - (init[0]);
  random = rg.Random();
  float synh2 = (step[1] * phi[1]) + (step[1] * random) - (init[1]);
  random = rg.Random();
  float synh3 = (step[2] * phi[2]) + (step[2] * random) - (init[2]);
  float hmag = sqrt((synh1 * synh1 + synh2 * synh2 + synh3 * synh3));
  float angle = powf((8 * hmag * hmag * hmag), (1.0 / 3.0));
  float synr1 = tanf(angle / 2.0) * (synh1 / hmag);
  float synr2 = tanf(angle / 2.0) * (synh2 / hmag);
  float synr3 = tanf(angle / 2.0) * (synh3 / hmag);
  if (hmag == 0.0) synr1 = 0.0, synr2 = 0.0, synr3 = 0.0;
  float sum = atan(synr3);
  float diff = atan(synr2 / synr1);
  synea1 = sum + diff;
  synea2 = 2. * atan(synr1 * cosf(sum) / cosf(diff));
  synea3 = sum - diff;
}

void OrientationMath::_calcDetermineHomochoricValues( float step[3], float phi[3], int choose, float &r1, float &r2, float &r3)
{
  float random;

  AIM_RANDOMNG_NEW()
  random = rg.Random();
  r1 = (step[0] * phi[0]) + (step[0] * random);
  random = rg.Random();
  r2 = (step[1] * phi[1]) + (step[1] * random);
  random = rg.Random();
  r3 = (step[2] * phi[2]) + (step[2] * random);
}

int OrientationMath::_calcODFBin(float dim[3], float bins[3], float r1, float r2, float r3)
{
  OrientationMath::RodtoHomochoric(r1,r2,r3);
  size_t g1euler1bin;
  size_t g1euler2bin;
  size_t g1euler3bin;
  size_t g1odfbin;
  g1euler1bin = size_t(fabs(r1+dim[0])*bins[0]/(2.0*dim[0]));
  g1euler2bin = size_t(fabs(r2+dim[1])*bins[1]/(2.0*dim[1]));
  g1euler3bin = size_t(fabs(r3+dim[2])*bins[2]/(2.0*dim[2]));
  if(g1euler1bin >= bins[0]) g1euler1bin = bins[0]-1;
  if(g1euler2bin >= bins[1]) g1euler2bin = bins[1]-1;
  if(g1euler3bin >= bins[2]) g1euler3bin = bins[2]-1;
  g1odfbin = (g1euler3bin*bins[0]*bins[1])+(g1euler2bin*bins[0])+(g1euler1bin);
  return g1odfbin;
}

void OrientationMath::axisAngletoHomochoric(float w, float n1, float n2, float n3, float &r1, float &r2, float &r3)
{
  float degtorad = m_pi / 180.0;
  float denom;

  w = w * degtorad;
  denom = (n1 * n1) + (n2 * n2) + (n3 * n3);
  denom = sqrt(denom);
  n1 = n1 / denom;
  n2 = n2 / denom;
  n3 = n3 / denom;
  if(denom == 0.0) n1 = 0.0, n2 = 0.0, n3 = 1.0;
  r1 = n1 * powf(((0.75f) * (w - sinf(w))), (1.0f / 3.0f));
  r2 = n2 * powf(((0.75f) * (w - sinf(w))), (1.0f / 3.0f));
  r3 = n3 * powf(((0.75f) * (w - sinf(w))), (1.0f / 3.0f));
}


void OrientationMath::axisAngletoRod(float w, float n1, float n2, float n3, float &r1, float &r2, float &r3)
{
  float degtorad = m_pi / 180.0;
  float denom;

  w = w * degtorad;
  denom = (n1 * n1) + (n2 * n2) + (n3 * n3);
  denom = sqrt(denom);
  n1 = n1 / denom;
  n2 = n2 / denom;
  n3 = n3 / denom;
  if(denom == 0.0) n1 = 0.0, n2 = 0.0, n3 = 1.0;
  r1 = n1 * tanf(w/2.0);
  r2 = n2 * tanf(w/2.0);
  r3 = n3 * tanf(w/2.0);
}


void OrientationMath::RodtoHomochoric(float &r1, float &r2, float &r3)
{
  float rmag, w;

  rmag = (r1 * r1) + (r2 * r2) + (r3 * r3);
  rmag = sqrt(rmag);
  r1 = r1 / rmag;
  r2 = r2 / rmag;
  r3 = r3 / rmag;
  if(rmag == 0.0) r1 = 0.0, r2 = 0.0, r3 = 0.0;
  w = 2.0*atan(rmag);
  r1 = r1 * powf(((3.0 / 4.0) * (w - sinf(w))), (1.0 / 3.0));
  r2 = r2 * powf(((3.0 / 4.0) * (w - sinf(w))), (1.0 / 3.0));
  r3 = r3 * powf(((3.0 / 4.0) * (w - sinf(w))), (1.0 / 3.0));
}


void OrientationMath::HomochorictoRod(float &r1, float &r2, float &r3)
{
  float hmag, w;

  hmag = (r1 * r1) + (r2 * r2) + (r3 * r3);
  hmag = sqrt(hmag);
  r1 = r1 / hmag;
  r2 = r2 / hmag;
  r3 = r3 / hmag;
  if(hmag == 0.0) r1 = 0.0, r2 = 0.0, r3 = 0.0;
  w = powf((8*hmag*hmag*hmag),(1.0/3.0));
  r1 = r1 * tanf(w/2.0);
  r2 = r2 * tanf(w/2.0);
  r3 = r3 * tanf(w/2.0);
}


void OrientationMath::RodtoAxisAngle(float r1, float r2, float r3, float &w, float &n1, float &n2, float &n3)
{
  float rmag;

  rmag = (r1 * r1) + (r2 * r2) + (r3 * r3);
  rmag = sqrt(rmag);
  w = 2.0*atan(rmag);
  n1 = r1 / rmag;
  n2 = r2 / rmag;
  n3 = r3 / rmag;
  if(rmag == 0.0) n1 = 0.0, n2 = 0.0, n3 = 1.0;
}

void OrientationMath::RodtoQuat(float *q, float r1, float r2, float r3)
{
  float rmag, w;

  rmag = (r1 * r1) + (r2 * r2) + (r3 * r3);
  rmag = sqrt(rmag);
  r1 = r1 / rmag;
  r2 = r2 / rmag;
  r3 = r3 / rmag;
  if(rmag == 0.0) r1 = 0.0, r2 = 0.0, r3 = 0.0;
  w = 2.0*atan(rmag);
  q[1] = r1 * sinf(w/2.0);
  q[2] = r2 * sinf(w/2.0);
  q[3] = r3 * sinf(w/2.0);
  q[4] = cosf(w/2.0);
}

void OrientationMath::QuattoRod(float *q, float &r1, float &r2, float &r3)
{
  float qmag, w;
  float n1, n2, n3;

  qmag = (q[1] * q[1]) + (q[2] * q[2]) + (q[3] * q[3]);
  qmag = sqrt(qmag);
  n1 = q[1] / qmag;
  n2 = q[2] / qmag;
  n3 = q[3] / qmag;
  if(qmag == 0.0) n1 = 0.0, n2 = 0.0, n3 = 1.0;
  w = 2.0*acos(q[4]);
  r1 = n1 * tanf(w * 0.5f);
  r2 = n2 * tanf(w * 0.5f);
  r3 = n3 * tanf(w * 0.5f);
}

void OrientationMath::QuattoEuler(float *q, float &ea1, float &ea2, float &ea3)
{
	float diff, sum, tmp;
    diff=atan2(q[2],q[1]);
    sum=atan2(q[3],q[4]);
    ea1=(diff+sum);
    ea3=(sum-diff);
    tmp=(q[3]*q[3])+(q[4]*q[4]);
    tmp = sqrt(tmp);
    if(tmp > 1.0) tmp=1.0;
    ea2=2*acos(tmp);
}
void OrientationMath::eulertoQuat(float* q, float e1, float e2, float e3)
{
  float s, c, s1, c1, s2, c2;
  s = sinf(0.5 * e2);
  c = cosf(0.5 * e2);
  s1 = sinf(0.5 * (e1 - e3));
  c1 = cosf(0.5 * (e1 - e3));
  s2 = sinf(0.5 * (e1 + e3));
  c2 = cosf(0.5 * (e1 + e3));
  q[1] = s*c1;
  q[2] = s*s1;
  q[3] = c*s2;
  q[4] = c*c2;
}




void OrientationMath::eulertoRod(float &r1, float &r2, float &r3, float ea1, float ea2, float ea3)
{
	float sum, diff, csum, cdiff, sdiff, t2;
	sum = (ea1+ea3)/2.0;
	diff = (ea1-ea3)/2.0;
	csum = cosf(sum);
	cdiff = cosf(diff);
	sdiff = sinf(diff);
	t2 = tanf(ea2/2.0);
	r1 = t2*cdiff/csum;
	r2 = t2*sdiff/csum;
	r3 = tanf(sum);
}

void OrientationMath::multiplyQuaternions(float* inQuat, float* multQuat, float* outQuat)
{
  MULT_QUAT(inQuat, multQuat, outQuat);

//  outQuat[1] = multQuat[4] * inQuat[1] + multQuat[1] * inQuat[4] + multQuat[3] * inQuat[2] - multQuat[2] * inQuat[3];
//  outQuat[2] = multQuat[4] * inQuat[2] + multQuat[2] * inQuat[4] + multQuat[1] * inQuat[3] - multQuat[3] * inQuat[1];
//  outQuat[3] = multQuat[4] * inQuat[3] + multQuat[3] * inQuat[4] + multQuat[2] * inQuat[1] - multQuat[1] * inQuat[2];
//  outQuat[4] = multQuat[4] * inQuat[4] - multQuat[1] * inQuat[1] - multQuat[2] * inQuat[2] - multQuat[3] * inQuat[3];
}

void OrientationMath::getSlipMisalignment(int ss1, float q1[5], float q2[5], float &ssap)
{
  float g1[3][3];
  float g2[3][3];
  float maxssap = 0;
  float h1, k1, l1, u1, v1, w1;
  float h2, k2, l2, u2, v2, w2;
  float denomhkl1, denomhkl2, denomuvw1, denomuvw2;
  float planemisalignment, directionmisalignment;
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
  denomhkl1 = sqrt((h1*h1+k1*k1+l1*l1));
  denomuvw1 = sqrt((u1*u1+v1*v1+w1*w1));
  for(int i=0;i<12;i++)
  {
    h2 = CubicSlipSystems[i][0]*g2[0][0]+CubicSlipSystems[i][1]*g2[1][0]+CubicSlipSystems[i][2]*g2[2][0];
    k2 = CubicSlipSystems[i][0]*g2[0][1]+CubicSlipSystems[i][1]*g2[1][1]+CubicSlipSystems[i][2]*g2[2][1];
    l2 = CubicSlipSystems[i][0]*g2[0][2]+CubicSlipSystems[i][1]*g2[1][2]+CubicSlipSystems[i][2]*g2[2][2];
    u2 = CubicSlipSystems[i][3]*g2[0][0]+CubicSlipSystems[i][4]*g2[1][0]+CubicSlipSystems[i][5]*g2[2][0];
    v2 = CubicSlipSystems[i][3]*g2[0][1]+CubicSlipSystems[i][4]*g2[1][1]+CubicSlipSystems[i][5]*g2[2][1];
    w2 = CubicSlipSystems[i][3]*g2[0][2]+CubicSlipSystems[i][4]*g2[1][2]+CubicSlipSystems[i][5]*g2[2][2];
    denomhkl2 = sqrt((h2*h2+k2*k2+l2*l2));
    denomuvw2 = sqrt((u2*u2+v2*v2+w2*w2));
    planemisalignment = fabs((h1*h2+k1*k2+l1*l2)/(denomhkl1*denomhkl2));
    directionmisalignment = fabs((u1*u2+v1*v2+w1*w2)/(denomuvw1*denomuvw2));
    ssap = planemisalignment*directionmisalignment;
    if(ssap > maxssap) maxssap = ssap;
  }
  ssap = maxssap;
}
