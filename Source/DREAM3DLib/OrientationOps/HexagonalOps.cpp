/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force, 
 * BlueQuartz Software nor the names of its contributors may be used to endorse 
 * or promote products derived from this software without specific prior written
 * permission.
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
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "HexagonalOps.h"
// Include this FIRST because there is a needed define for some compiles
// to expose some of the constants needed below
#include "DREAM3DLib/Common/DREAM3DMath.h"


using namespace HexagonalMath::Detail;


HexagonalOps::HexagonalOps()
{
}

HexagonalOps::~HexagonalOps()
{
}

float HexagonalOps::getMisoQuat( float q1[5],float q2[5],float &n1,float &n2,float &n3)
{
  int numsym = 12;

  return _calcMisoQuat(HexQuatSym, numsym, q1, q2, n1, n2, n3);
}


void HexagonalOps::getFZRod(float &r1,float &r2, float &r3)
{
  int numsym = 12;

  _calcFZRod(HexRodSym, numsym,r1, r2, r3);
}

void HexagonalOps::getNearestQuat( float *q1, float *q2)
{
  int numsym = 12;

    _calcNearestQuat(HexQuatSym, numsym, q1, q2);
}

void HexagonalOps::getFZQuat(float *qr)
{
  int numsym = 12;

    _calcFZQuat(HexQuatSym, numsym, qr);

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
  phi[0] = static_cast<float>(choose % 36);
  phi[1] = static_cast<float>((choose / 36) % 36);
  phi[2] = static_cast<float>(choose / (36 * 36));

  _calcDetermineEulerAngles(init, step, phi, choose, synea1, synea2, synea3);
}


void HexagonalOps::determineHomochoricValues( int choose, float &r1, float &r2, float &r3)
{
  float step[3];
  float phi[3];

  step[0] = HexDim1StepValue/2.0f;
  step[1] = HexDim2StepValue/2.0f;
  step[2] = HexDim3StepValue/2.0f;
  phi[0] = static_cast<float>(choose % 36);
  phi[1] = static_cast<float>((choose / 36) % 36);
  phi[2] = static_cast<float>(choose / (36 * 36));

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

void HexagonalOps::getSchmidFactorAndSS(float loadx, float loady, float loadz, float &schmidfactor, int &slipsys)
{
	float theta1, theta2, theta3, theta4, theta5, theta6, theta7, theta8, theta9;
	float lambda1, lambda2, lambda3, lambda4, lambda5, lambda6, lambda7, lambda8, lambda9, lambda10;
	float schmid1, schmid2, schmid3, schmid4, schmid5, schmid6, schmid7, schmid8, schmid9, schmid10, schmid11, schmid12;
	float schmid13, schmid14, schmid15, schmid16, schmid17, schmid18, schmid19, schmid20, schmid21, schmid22, schmid23, schmid24;
	float caratio = 1.633f;
	float ph1sdx1 = 1.0f;
	float ph1sdy1 = 0.0f;
	float ph1sdz1 = 0.0f;
	float ph1sdx2 = 0.0f;
	float ph1sdy2 = 1.0f;
	float ph1sdz2 = 0.0f;
	float ph1sdx3 = -0.707f;
	float ph1sdy3 = -0.707f;
	float ph1sdz3 = 0.0f;
	float ph1sdx4 = 0.0f;
	float ph1sdy4 = -0.707f;
	float ph1sdz4 = 0.707f;
	float ph1sdx5 = -0.57735f;
	float ph1sdy5 = -0.57735f;
	float ph1sdz5 = 0.57735f;
	float ph1sdx6 = 0.707f;
	float ph1sdy6 = 0.0f;
	float ph1sdz6 = 0.707f;
	float ph1sdx7 = 0.57735f;
	float ph1sdy7 = 0.57735f;
	float ph1sdz7 = 0.57735f;
	float ph1sdx8 = 0.0f;
	float ph1sdy8 = 0.707f;
	float ph1sdz8 = 0.707f;
	float ph1sdx9 = -0.707f;
	float ph1sdy9 = 0.0f;
	float ph1sdz9 = 0.707f;
	float ph1spnx1 = 0.0f;
	float ph1spny1 = 0.0f;
	float ph1spnz1 = 1.0f;
	float ph1spnx2 = 0.4472f;
	float ph1spny2 = 0.8944f;
	float ph1spnz2 = 0.0f;
	float ph1spnx3 = 0.8944f;
	float ph1spny3 = 0.4472f;
	float ph1spnz3 = 0.0f;
	float ph1spnx4 = -0.707f;
	float ph1spny4 = 0.707f;
	float ph1spnz4 = 0.0f;
	float ph1spnx5 = 0.4082f;
	float ph1spny5 = 0.8164f;
	float ph1spnz5 = -0.4082f;
	float ph1spnx6 = 0.4082f;
	float ph1spny6 = 0.8164f;
	float ph1spnz6 = 0.4082f;
	float ph1spnx7 = 0.8164f;
	float ph1spny7 = 0.4082f;
	float ph1spnz7 = -0.4082f;
	float ph1spnx8 = 0.8164f;
	float ph1spny8 = 0.4082f;
	float ph1spnz8 = 0.4082f;
	float ph1spnx9 = -0.57735f;
	float ph1spny9 = 0.57735f;
	float ph1spnz9 = -0.57735f;
	float ph1spnx10 = -0.57735f;
	float ph1spny10 = 0.57735f;
	float ph1spnz10 = 0.57735f;
	float t1x = (0.866025f*ph1sdx1)+(0.0f*ph1sdy1)+(0.0f*ph1sdz1);
	float t1y = (-0.5f*ph1sdx1)+(1.0f*ph1sdy1)+(0.0f*ph1sdz1);
	float t1z = (0.0f*ph1sdx1)+(0.0f*ph1sdy1)+(caratio*ph1sdz1);
	float denomt1 = powf((t1x*t1x+t1y*t1y+t1z*t1z),0.5f);
	t1x = t1x/denomt1;
	t1y = t1y/denomt1;
	t1z = t1z/denomt1;
	theta1 = ((t1x*loadx)+(t1y*loady)+(t1z*loadz));
	theta1 = fabs(theta1);
	float t2x = (0.866025f*ph1sdx2)+(0.0f*ph1sdy2)+(0.0f*ph1sdz2);
	float t2y = (-0.5f*ph1sdx2)+(1.0f*ph1sdy2)+(0.0f*ph1sdz2);
	float t2z = (0.0f*ph1sdx2)+(0.0f*ph1sdy2)+(caratio*ph1sdz2);
	float denomt2 = powf((t2x*t2x+t2y*t2y+t2z*t2z),0.5f);
	t2x = t2x/denomt2;
	t2y = t2y/denomt2;
	t2z = t2z/denomt2;
	theta2 = ((t2x*loadx)+(t2y*loady)+(t2z*loadz));
	theta2 = fabs(theta2);
	float t3x = (0.866025f*ph1sdx3)+(0.0f*ph1sdy3)+(0.0f*ph1sdz3);
	float t3y = (-0.5f*ph1sdx3)+(1.0f*ph1sdy3)+(0.0f*ph1sdz3);
	float t3z = (0.0f*ph1sdx3)+(0.0f*ph1sdy3)+(caratio*ph1sdz3);
	float denomt3 = powf((t3x*t3x+t3y*t3y+t3z*t3z),0.5f);
	t3x = t3x/denomt3;
	t3y = t3y/denomt3;
	t3z = t3z/denomt3;
	theta3 = ((t3x*loadx)+(t3y*loady)+(t3z*loadz));
	theta3 = fabs(theta3);
	float l1nx = (0.866025f*ph1spnx1)+(0*ph1spny1);
	float l1ny = (-0.5f*ph1spnx1)+(1*ph1spny1);
	float l1nz = -caratio*ph1spnz1;
	float denoml1 = powf((l1nx*l1nx+l1ny*l1ny+l1nz*l1nz),0.5f);
	l1nx = l1nx/denoml1;
	l1ny = l1ny/denoml1;
	l1nz = l1nz/denoml1;
	lambda1 = ((l1nx*loadx)+(l1ny*loady)+(l1nz*loadz));
	lambda1 = fabs(lambda1);
	schmid1 = theta1*lambda1;
	schmid2 = theta2*lambda1;
	schmid3 = theta3*lambda1;
	float l2nx = (0.866025f*ph1spnx2)+(0*ph1spny2);
	float l2ny = (-0.5f*ph1spnx2)+(1*ph1spny2);
	float l2nz = -caratio*ph1spnz2;
	float denoml2 = powf((l2nx*l2nx+l2ny*l2ny+l2nz*l2nz),0.5f);
	l2nx = l2nx/denoml2;
	l2ny = l2ny/denoml2;
	l2nz = l2nz/denoml2;
	lambda2 = ((l2nx*loadx)+(l2ny*loady)+(l2nz*loadz));
	lambda2 = fabs(lambda2);
	float l3nx = (0.866025f*ph1spnx3)+(0*ph1spny3);
	float l3ny = (-0.5f*ph1spnx3)+(1*ph1spny3);
	float l3nz = -caratio*ph1spnz3;
	float denoml3 = powf((l3nx*l3nx+l3ny*l3ny+l3nz*l3nz),0.5);
	l3nx = l3nx/denoml3;
	l3ny = l3ny/denoml3;
	l3nz = l3nz/denoml3;
	lambda3 = ((l3nx*loadx)+(l3ny*loady)+(l3nz*loadz));
	lambda3 = fabs(lambda3);
	float l4nx = (0.866025f*ph1spnx4)+(0.0f*ph1spny4);
	float l4ny = (-0.5f*ph1spnx4)+(1.0f*ph1spny4);
	float l4nz = -caratio*ph1spnz4;
	float denoml4 = powf((l4nx*l4nx+l4ny*l4ny+l4nz*l4nz),0.5f);
	l4nx = l4nx/denoml4;
	l4ny = l4ny/denoml4;
	l4nz = l4nz/denoml4;
	lambda4 = ((l4nx*loadx)+(l4ny*loady)+(l4nz*loadz));
	lambda4 = fabs(lambda4);
	schmid4 = theta1*lambda2;
	schmid5 = theta2*lambda3;
	schmid6 = theta3*lambda4;
	float l5nx = (0.866025f*ph1spnx5)+(0*ph1spny5);
	float l5ny = (-0.5f*ph1spnx5)+(1.0f*ph1spny5);
	float l5nz = float((l5nx*-l5nx+l5ny*-l5ny))/(caratio*0.8164f);
	float denoml5 = powf((l5nx*l5nx+l5ny*l5ny+l5nz*l5nz),0.5f);
	l5nx = l5nx/denoml5;
	l5ny = l5ny/denoml5;
	l5nz = l5nz/denoml5;
	float l6nx = (0.866025f*ph1spnx6)+(0.0f*ph1spny6);
	float l6ny = (-0.5f*ph1spnx6)+(1*ph1spny6);
	float l6nz = float(-(l6nx*-l6nx+l6ny*-l6ny))/(caratio*0.8164f);
	float denoml6 = powf((l6nx*l6nx+l6ny*l6ny+l6nz*l6nz),0.5f);
	l6nx = l6nx/denoml6;
	l6ny = l6ny/denoml6;
	l6nz = l6nz/denoml6;
	float l7nx = (0.866025f*ph1spnx7)+(0*ph1spny7);
	float l7ny = (-0.5f*ph1spnx7)+(1*ph1spny7);
	float l7nz = float((l7nx*-l7nx+l7ny*-l7ny))/(caratio*0.8164f);
	float denoml7 = powf((l7nx*l7nx+l7ny*l7ny+l7nz*l7nz),0.5f);
	l7nx = l7nx/denoml7;
	l7ny = l7ny/denoml7;
	l7nz = l7nz/denoml7;
	float l8nx = (0.866025f*ph1spnx8)+(0.0f*ph1spny8);
	float l8ny = (-0.5f*ph1spnx8)+(1.0f*ph1spny8);
	float l8nz = float(-(l8nx*-l8nx+l8ny*-l8ny))/(caratio*0.8164f);
	float denoml8 = powf((l8nx*l8nx+l8ny*l8ny+l8nz*l8nz),0.5f);
	l8nx = l8nx/denoml8;
	l8ny = l8ny/denoml8;
	l8nz = l8nz/denoml8;
	float l9nx = (0.866025f*ph1spnx9)+(0.0f*ph1spny9);
	float l9ny = (-0.5f*ph1spnx9)+(1.0f*ph1spny9);
	float l9nz = float((l9nx*-l9nx+l9ny*-l9ny))/(caratio*1.154f);
	float denoml9 = powf((l9nx*l9nx+l9ny*l9ny+l9nz*l9nz),0.5f);
	l9nx = l9nx/denoml9;
	l9ny = l9ny/denoml9;
	l9nz = l9nz/denoml9;
	float l10nx = (0.866025f*ph1spnx10)+(0*ph1spny10);
	float l10ny = (-0.5f*ph1spnx10)+(1*ph1spny10);
	float l10nz = float(-(l10nx*-l10nx+l10ny*-l10ny))/(caratio*1.154f);
	float denoml10 = powf((l10nx*l10nx+l10ny*l10ny+l10nz*l10nz),0.5f);
	l10nx = l10nx/denoml10;
	l10ny = l10ny/denoml10;
	l10nz = l10nz/denoml10;
	lambda5 = ((l5nx*loadx)+(l5ny*loady)+(-l5nz*loadz));
	lambda5 = fabs(lambda5);
	lambda6 = ((l5nx*loadx)+(l5ny*loady)+(l5nz*loadz));
	lambda6 = fabs(lambda6);
	lambda7 = ((l7nx*loadx)+(l7ny*loady)+(-l7nz*loadz));
	lambda7 = fabs(lambda7);
	lambda8 = ((l7nx*loadx)+(l7ny*loady)+(l7nz*loadz));
	lambda8 = fabs(lambda8);
	lambda9 = ((l9nx*loadx)+(l9ny*loady)+(-l9nz*loadz));
	lambda9 = fabs(lambda9);
	lambda10 = ((l9nx*loadx)+(l9ny*loady)+(l9nz*loadz));
	lambda10 = fabs(lambda10);
	schmid7 = theta1*lambda5;
	schmid8 = theta1*lambda6;
	schmid9 = theta2*lambda7;
	schmid10 = theta2*lambda8;
	schmid11 = theta3*lambda9;
	schmid12 = theta3*lambda10;
	float t4x = (0.866025f*ph1sdx4)+(0.0f*ph1sdy4)+(0.0f*ph1sdz4);
	float t4y = (-0.5f*ph1sdx4)+(1.0f*ph1sdy4)+(0.0f*ph1sdz4);
	float t4z = (0.0f*ph1sdx4)+(0.0f*ph1sdy4)+(caratio*ph1sdz4);
	float denomt4 = powf((t4x*t4x+t4y*t4y+t4z*t4z),0.5f);
	t4x = t4x/denomt4;
	t4y = t4y/denomt4;
	t4z = t4z/denomt4;
	theta4 = ((t4x*loadx)+(t4y*loady)+(t4z*loadz));
	theta4 = fabs(theta4);
	float t5x = (0.866025f*ph1sdx5)+(0.0f*ph1sdy5)+(0.0f*ph1sdz5);
	float t5y = (-0.5f*ph1sdx5)+(1.0f*ph1sdy5)+(0.0f*ph1sdz5);
	float t5z = (0.0f*ph1sdx5)+(0.0f*ph1sdy5)+(caratio*ph1sdz5);
	float denomt5 = powf((t5x*t5x+t5y*t5y+t5z*t5z),0.5f);
	t5x = t5x/denomt5;
	t5y = t5y/denomt5;
	t5z = t5z/denomt5;
	theta5 = ((t5x*loadx)+(t5y*loady)+(t5z*loadz));
	theta5 = fabs(theta5);
	float t6x = (0.866025f*ph1sdx6)+(0.0f*ph1sdy6)+(0.0f*ph1sdz6);
	float t6y = (-0.5f*ph1sdx6)+(1.0f*ph1sdy6)+(0.0f*ph1sdz6);
	float t6z = (0.0f*ph1sdx6)+(0.0f*ph1sdy6)+(caratio*ph1sdz6);
	float denomt6 = powf((t6x*t6x+t6y*t6y+t6z*t6z),0.5f);
	t6x = t6x/denomt6;
	t6y = t6y/denomt6;
	t6z = t6z/denomt6;
	theta6 = ((t6x*loadx)+(t6y*loady)+(t6z*loadz));
	theta6 = fabs(theta6);
	float t7x = (0.866025f*ph1sdx7)+(0.0f*ph1sdy7)+(0.0f*ph1sdz7);
	float t7y = (-0.5f*ph1sdx7)+(1.0f*ph1sdy7)+(0.0f*ph1sdz7);
	float t7z = (0.0f*ph1sdx7)+(0.0f*ph1sdy7)+(caratio*ph1sdz7);
	float denomt7 = powf((t7x*t7x+t7y*t7y+t7z*t7z),0.5f);
	t7x = t7x/denomt7;
	t7y = t7y/denomt7;
	t7z = t7z/denomt7;
	theta7 = ((t7x*loadx)+(t7y*loady)+(t7z*loadz));
	theta7 = fabs(theta7);
	float t8x = (0.866025f*ph1sdx8)+(0.0f*ph1sdy8)+(0.0f*ph1sdz8);
	float t8y = (-0.5f*ph1sdx8)+(1.0f*ph1sdy8)+(0.0f*ph1sdz8);
	float t8z = (0.0f*ph1sdx8)+(0.0f*ph1sdy8)+(caratio*ph1sdz8);
	float denomt8 = powf((t8x*t8x+t8y*t8y+t8z*t8z),0.5f);
	t8x = t8x/denomt8;
	t8y = t8y/denomt8;
	t8z = t8z/denomt8;
	theta8 = ((t8x*loadx)+(t8y*loady)+(t8z*loadz));
	theta8 = fabs(theta8);
	float t9x = (0.866025f*ph1sdx9)+(0.0f*ph1sdy9)+(0.0f*ph1sdz9);
	float t9y = (-0.5f*ph1sdx9)+(1.0f*ph1sdy9)+(0.0f*ph1sdz9);
	float t9z = (0.0f*ph1sdx9)+(0.0f*ph1sdy9)+(caratio*ph1sdz9);
	float denomt9 = powf((t9x*t9x+t9y*t9y+t9z*t9z),0.5f);
	t9x = t9x/denomt9;
	t9y = t9y/denomt9;
	t9z = t9z/denomt9;
	theta9 = ((t9x*loadx)+(t9y*loady)+(t9z*loadz));
	theta9 = fabs(theta9);
	schmid13 = theta7*lambda6;
	schmid14 = theta8*lambda6;
	schmid15 = theta6*lambda9;
	schmid16 = theta4*lambda9;
	schmid17 = theta6*lambda8;
	schmid18 = theta7*lambda8;
	schmid19 = theta4*lambda5;
	schmid20 = theta5*lambda5;
	schmid21 = theta8*lambda10;
	schmid22 = theta9*lambda10;
	schmid23 = theta9*lambda7;
	schmid24 = theta5*lambda7;
	if(schmid1 > schmidfactor) schmidfactor = schmid1, slipsys = 1;
	if(schmid2 > schmidfactor) schmidfactor = schmid2, slipsys = 2;
	if(schmid3 > schmidfactor) schmidfactor = schmid3, slipsys = 3;
	if(schmid4 > schmidfactor) schmidfactor = schmid4, slipsys = 4;
	if(schmid5 > schmidfactor) schmidfactor = schmid5, slipsys = 5;
	if(schmid6 > schmidfactor) schmidfactor = schmid6, slipsys = 6;
	if(schmid7 > schmidfactor) schmidfactor = schmid7, slipsys = 7;
	if(schmid8 > schmidfactor) schmidfactor = schmid8, slipsys = 8;
	if(schmid9 > schmidfactor) schmidfactor = schmid9, slipsys = 9;
	if(schmid10 > schmidfactor) schmidfactor = schmid10, slipsys = 10;
	if(schmid11 > schmidfactor) schmidfactor = schmid11, slipsys = 11;
	if(schmid12 > schmidfactor) schmidfactor = schmid12, slipsys = 12;
	if(schmid13 > schmidfactor) schmidfactor = schmid13, slipsys = 13;
	if(schmid14 > schmidfactor) schmidfactor = schmid14, slipsys = 14;
	if(schmid15 > schmidfactor) schmidfactor = schmid15, slipsys = 15;
	if(schmid16 > schmidfactor) schmidfactor = schmid16, slipsys = 16;
	if(schmid17 > schmidfactor) schmidfactor = schmid17, slipsys = 17;
	if(schmid18 > schmidfactor) schmidfactor = schmid18, slipsys = 18;
	if(schmid19 > schmidfactor) schmidfactor = schmid19, slipsys = 19;
	if(schmid20 > schmidfactor) schmidfactor = schmid20, slipsys = 20;
	if(schmid21 > schmidfactor) schmidfactor = schmid21, slipsys = 21;
	if(schmid22 > schmidfactor) schmidfactor = schmid22, slipsys = 22;
	if(schmid23 > schmidfactor) schmidfactor = schmid23, slipsys = 23;
	if(schmid24 > schmidfactor) schmidfactor = schmid24, slipsys = 24;
}