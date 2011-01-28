/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
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
#include "MisorientationCalculations.h"

#include <math.h>

#include "AIM/Common/Quaternions.h"


const static double m_pi = 3.1415926535897;
const static double m_OnePointThree = 1.33333333333;

const double threesixty_over_pi = 360.0/m_pi;
const double sqrt_two = pow(2.0, 0.5);

const double acos_neg_one = acos(-1.0);
const double acos_pos_one = acos(1.0);
const double sin_wmin_neg_1_over_2 = sin(acos_neg_one/2.0);
const double sin_wmin_pos_1_over_2 = sin(acos_pos_one/2.0);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MisorientationCalculations::MisorientationCalculations()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MisorientationCalculations::~MisorientationCalculations()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double MisorientationCalculations::getMisoQuatCubic(double q1[5],double q2[5],double &n1,double &n2,double &n3)
{
  double wmin=9999999.0; //,na,nb,nc;
  double qc[4];
  double qco[4];
  double axis[3];
  double sin_wmin_over_2 = 0.0;
  double _1, _2,  _6;

  qc[0]=-q1[1]*q2[4]+q1[4]*q2[1]-q1[2]*q2[3]+q1[3]*q2[2];
  qc[1]=-q1[2]*q2[4]+q1[4]*q2[2]-q1[3]*q2[1]+q1[1]*q2[3];
  qc[2]=-q1[3]*q2[4]+q1[4]*q2[3]-q1[1]*q2[2]+q1[2]*q2[1];
  qc[3]=-q1[4]*q2[4]-q1[1]*q2[1]-q1[2]*q2[2]-q1[3]*q2[3];
  qc[0]=fabs(qc[0]);
  qc[1]=fabs(qc[1]);
  qc[2]=fabs(qc[2]);
  qc[3]=fabs(qc[3]);
  for(int i=0;i<4;i++)
  {
    qco[i]=100000;
    for(int j=0;j<4;j++)
    {
      _1 = qc[j];  _2 = qco[i]; _6 = qco[i-1];
      if (_1 < _2)
      {
        if (i == 0 || _1 > _6)
        {
          qco[i] = qc[j];
        }
      }
    }
  }
  wmin = qco[3];
  if (((qco[2] + qco[3]) / (sqrt_two)) > wmin)
  {
    wmin = ((qco[2] + qco[3]) / (sqrt_two));
  }
  if (((qco[0] + qco[1] + qco[2] + qco[3]) / 2) > wmin)
  {
    wmin = ((qco[0] + qco[1] + qco[2] + qco[3]) / 2);
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

  n1 = qco[0] / sin_wmin_over_2;
  n2 = qco[1] / sin_wmin_over_2;
  n3 = qco[2] / sin_wmin_over_2;
  if ( n1 >= n2 && n1 >= n3)
  {
     axis[0] = n1;
     if (n2 > n3) { axis[1] = n2; axis[2] = n3; }
     else { axis[1] = n3; axis[2] = n2; }
  }
  else if ( n2 >= n1 && n2 >= n3)
  {
     axis[0] = n2;
     if (n1 > n3) { axis[1] = n1; axis[2] = n3; }
     else { axis[1] = n3; axis[2] = n1; }
  }
  else if ( n1 >= n2 )
  {
     axis[1] = n1; axis[2] = n2; axis[0] = n3;
  }
  else { axis[2] = n1; axis[1] = n2; axis[0] = n3;}
  if(wmin == 0) axis[0] = 0.0, axis[1] = 0.0, axis[2] = 0.0;
  wmin = (threesixty_over_pi) * wmin;
  return wmin;
}

double MisorientationCalculations::getMisoQuatHexagonal(double q1[5],double q2[5],double &n1,double &n2,double &n3)
{
  double wmin = 9999999; //,na,nb,nc;
  double w = 0;
  double n1min, n2min, n3min;
  double qr[4];
  double qc[4];

  qr[0] = -q1[1] * q2[4] + q1[4] * q2[1] - q1[2] * q2[3] + q1[3] * q2[2];
  qr[1] = -q1[2] * q2[4] + q1[4] * q2[2] - q1[3] * q2[1] + q1[1] * q2[3];
  qr[2] = -q1[3] * q2[4] + q1[4] * q2[3] - q1[1] * q2[2] + q1[2] * q2[1];
  qr[3] = -q1[4] * q2[4] - q1[1] * q2[1] - q1[2] * q2[2] - q1[3] * q2[3];
  for (int i = 0; i < 12; i++)
  {
    AIM::Quaternions::Hex_MultiplyByUnitQuaterion(qr, i, qc);
    if (qc[3] < -1) qc[3] = -1;
    if (qc[3] > 1) qc[3] = 1;
    w = acos(qc[3]);
    w = 2 * w;
    n1 = qc[0] / sin(w / 2.0);
    n2 = qc[1] / sin(w / 2.0);
    n3 = qc[2] / sin(w / 2.0);
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

int MisorientationCalculations::getMisoBinCubic(double n1, double n2, double n3)
{
	double dim1 = pow((0.75*((m_pi/4.0)-sin((m_pi/4.0)))),(1.0/3.0));
	double dim2 = pow((0.75*((m_pi/4.0)-sin((m_pi/4.0)))),(1.0/3.0));
	double dim3 = pow((0.75*((m_pi/4.0)-sin((m_pi/4.0)))),(1.0/3.0));
	int miso1bin = int(n1*18.0/dim1);
	int miso2bin = int(n2*18.0/dim2);
	int miso3bin = int(n3*18.0/dim3);
	if(miso1bin >= 18) miso1bin = 17;
	if(miso2bin >= 18) miso2bin = 17;
	if(miso3bin >= 18) miso3bin = 17;
	return ((18*18*miso3bin)+(18*miso2bin)+miso1bin);
}
int MisorientationCalculations::getMisoBinHexagonal(double n1, double n2, double n3)
{
	double dim1 = pow((0.75*((m_pi/2.0)-sin((m_pi/2.0)))),(1.0/3.0));
	double dim2 = pow((0.75*((m_pi/2.0)-sin((m_pi/2.0)))),(1.0/3.0));
	double dim3 = pow((0.75*((m_pi/6.0)-sin((m_pi/6.0)))),(1.0/3.0));
	int miso1bin = int(n1*36.0/dim1);
	int miso2bin = int(n2*36.0/dim2);
	int miso3bin = int(n3*12.0/dim3);
	if(miso1bin >= 36) miso1bin = 35;
	if(miso2bin >= 36) miso2bin = 35;
	if(miso3bin >= 12) miso3bin = 11;
	return ((36*36*miso3bin)+(36*miso2bin)+miso1bin);
}