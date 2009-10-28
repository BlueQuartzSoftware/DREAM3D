///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////

#include <time.h>
#include <stdlib.h>


#include <string>
#include <iostream>
#include <cmath>
#include <fstream>
#include <list>
#include <vector>
#include <algorithm>
#include <numeric>

#ifndef M_PI
#define M_PI 3.1415926535897;
#endif

static const double m_pi = M_PI;

#include <AIM/Common/AIMRandomNG.h>

#ifdef SVG_3D_LIBRARY
#include <MXA/Utilities/MXAFileSystemPath.h>
#include <AIM/Common/Constants.h>

#define CREATE_INPUT_FILENAME(f, n)\
    std::string f = inDir + MXAFileSystemPath::Separator + n;\
    f = MXAFileSystemPath::toNativeSeparators(f);\
    if (MXAFileSystemPath::exists(f) == false) { return -1; }

#define CREATE_OUTPUT_FILENAME(f, n)\
    std::string f = outDir + MXAFileSystemPath::Separator + n;\
    f = MXAFileSystemPath::toNativeSeparators(f);

#endif

using namespace std;

/********************************** Start of Misorientation.h *************************/

void computeMOg(double g1ea1,double g1ea2,double g1ea3,double gret[3][3],int cubicSymOp)
{
  double o[3][3];

  if (cubicSymOp == 0)
  {
    o[0][0] = 1.0; o[0][1] = 0.0; o[0][2] = 0.0;
    o[1][0] = 0.0; o[1][1] = 1.0; o[1][2] = 0.0;
    o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = 1.0;
  }
  else if (cubicSymOp == 1)
  {
    o[0][0] = 1.0; o[0][1] = 0.0; o[0][2] =  0.0;
    o[1][0] = 0.0; o[1][1] = 0.0; o[1][2] = -1.0;
    o[2][0] = 0.0; o[2][1] = 1.0; o[2][2] =  0.0;
  }
  else if (cubicSymOp == 2)
  {
    o[0][0] = 1.0; o[0][1] =  0.0; o[0][2] =  0.0;
    o[1][0] = 0.0; o[1][1] = -1.0; o[1][2] =  0.0;
    o[2][0] = 0.0; o[2][1] =  0.0; o[2][2] = -1.0;
  }
  else if (cubicSymOp == 3)
  {
    o[0][0] = 1.0; o[0][1] =  0.0; o[0][2] = 0.0;
    o[1][0] = 0.0; o[1][1] =  0.0; o[1][2] = 1.0;
    o[2][0] = 0.0; o[2][1] = -1.0; o[2][2] = 0.0;
  }
  else if (cubicSymOp == 4)
  {
    o[0][0] = 0.0; o[0][1] = 0.0; o[0][2] = -1.0;
    o[1][0] = 0.0; o[1][1] = 1.0; o[1][2] =  0.0;
    o[2][0] = 1.0; o[2][1] = 0.0; o[2][2] =  0.0;
  }
  else if (cubicSymOp == 5)
  {
    o[0][0] =  0.0; o[0][1] = 0.0; o[0][2] = 1.0;
    o[1][0] =  0.0; o[1][1] = 1.0; o[1][2] = 0.0;
    o[2][0] = -1.0; o[2][1] = 0.0; o[2][2] = 0.0;
  }
  else if (cubicSymOp == 6)
  {
    o[0][0] = -1.0; o[0][1] = 0.0; o[0][2] =  0.0;
    o[1][0] =  0.0; o[1][1] = 1.0; o[1][2] =  0.0;
    o[2][0] =  0.0; o[2][1] = 0.0; o[2][2] = -1.0;
  }
  else if (cubicSymOp == 7)
  {
    o[0][0] = -1.0; o[0][1] =  0.0; o[0][2] = 0.0;
    o[1][0] =  0.0; o[1][1] = -1.0; o[1][2] = 0.0;
    o[2][0] =  0.0; o[2][1] =  0.0; o[2][2] = 1.0;
  }
  else if (cubicSymOp == 8)
  {
    o[0][0] =  0.0; o[0][1] = 1.0; o[0][2] = 0.0;
    o[1][0] = -1.0; o[1][1] = 0.0; o[1][2] = 0.0;
    o[2][0] =  0.0; o[2][1] = 0.0; o[2][2] = 1.0;
  }
  else if (cubicSymOp == 9)
  {
    o[0][0] = 0.0; o[0][1] = -1.0; o[0][2] = 0.0;
    o[1][0] = 1.0; o[1][1] =  0.0; o[1][2] = 0.0;
    o[2][0] = 0.0; o[2][1] =  0.0; o[2][2] = 1.0;
  }
  else if (cubicSymOp == 10)
  {
    o[0][0] =  0.0; o[0][1] = -1.0; o[0][2] = 0.0;
    o[1][0] =  0.0; o[1][1] =  0.0; o[1][2] = 1.0;
    o[2][0] = -1.0; o[2][1] =  0.0; o[2][2] = 0.0;
  }
  else if (cubicSymOp == 11)
  {
    o[0][0] =  0.0; o[0][1] =  0.0; o[0][2] = 1.0;
    o[1][0] = -1.0; o[1][1] =  0.0; o[1][2] = 0.0;
    o[2][0] =  0.0; o[2][1] = -1.0; o[2][2] = 0.0;
  }
  else if (cubicSymOp == 12)
  {
    o[0][0] = 0.0; o[0][1] = -1.0; o[0][2] =  0.0;
    o[1][0] = 0.0; o[1][1] =  0.0; o[1][2] = -1.0;
    o[2][0] = 1.0; o[2][1] =  0.0; o[2][2] =  0.0;
  }
  else if (cubicSymOp == 13)
  {
    o[0][0] = 0.0; o[0][1] =  0.0; o[0][2] = -1.0;
    o[1][0] = 1.0; o[1][1] =  0.0; o[1][2] =  0.0;
    o[2][0] = 0.0; o[2][1] = -1.0; o[2][2] =  0.0;
  }
  else if (cubicSymOp == 14)
  {
    o[0][0] =  0.0; o[0][1] = 1.0; o[0][2] =  0.0;
    o[1][0] =  0.0; o[1][1] = 0.0; o[1][2] = -1.0;
    o[2][0] = -1.0; o[2][1] = 0.0; o[2][2] =  0.0;
  }
  else if (cubicSymOp == 15)
  {
    o[0][0] =  0.0; o[0][1] = 0.0; o[0][2] = -1.0;
    o[1][0] = -1.0; o[1][1] = 0.0; o[1][2] =  0.0;
    o[2][0] =  0.0; o[2][1] = 1.0; o[2][2] =  0.0;
  }
  else if (cubicSymOp == 16)
  {
    o[0][0] = 0.0; o[0][1] = 1.0; o[0][2] = 0.0;
    o[1][0] = 0.0; o[1][1] = 0.0; o[1][2] = 1.0;
    o[2][0] = 1.0; o[2][1] = 0.0; o[2][2] = 0.0;
  }
  else if (cubicSymOp == 17)
  {
    o[0][0] = 0.0; o[0][1] = 0.0; o[0][2] = 1.0;
    o[1][0] = 1.0; o[1][1] = 0.0; o[1][2] = 0.0;
    o[2][0] = 0.0; o[2][1] = 1.0; o[2][2] = 0.0;
  }
  else if (cubicSymOp == 18)
  {
    o[0][0] = 0.0; o[0][1] = 1.0; o[0][2] =  0.0;
    o[1][0] = 1.0; o[1][1] = 0.0; o[1][2] =  0.0;
    o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = -1.0;
  }
  else if (cubicSymOp == 19)
  {
    o[0][0] = -1.0; o[0][1] = 0.0; o[0][2] = 0.0;
    o[1][0] =  0.0; o[1][1] = 0.0; o[1][2] = 1.0;
    o[2][0] =  0.0; o[2][1] = 1.0; o[2][2] = 0.0;
  }
  else if (cubicSymOp == 20)
  {
    o[0][0] = 0.0; o[0][1] =  0.0; o[0][2] = 1.0;
    o[1][0] = 0.0; o[1][1] = -1.0; o[1][2] = 0.0;
    o[2][0] = 1.0; o[2][1] =  0.0; o[2][2] = 0.0;
  }
  else if (cubicSymOp == 21)
  {
    o[0][0] = -1.0; o[0][1] =  0.0; o[0][2] =  0.0;
    o[1][0] =  0.0; o[1][1] =  0.0; o[1][2] = -1.0;
    o[2][0] =  0.0; o[2][1] = -1.0; o[2][2] =  0.0;
  }
  else if (cubicSymOp == 22)
  {
    o[0][0] =  0.0; o[0][1] =  0.0; o[0][2] = -1.0;
    o[1][0] =  0.0; o[1][1] = -1.0; o[1][2] =  0.0;
    o[2][0] = -1.0; o[2][1] =  0.0; o[2][2] =  0.0;
  }
  else if (cubicSymOp == 23)
  {
    o[0][0] =  0.0; o[0][1] = -1.0; o[0][2] =  0.0;
    o[1][0] = -1.0; o[1][1] =  0.0; o[1][2] =  0.0;
    o[2][0] =  0.0; o[2][1] =  0.0; o[2][2] = -1.0;
  }

  double ga[3][3];
  ga[0][0] = cos(g1ea1)*cos(g1ea3)-sin(g1ea1)*sin(g1ea3)*cos(g1ea2);
  ga[0][1] = sin(g1ea1)*cos(g1ea3)+cos(g1ea1)*sin(g1ea3)*cos(g1ea2);
  ga[0][2] = sin(g1ea3)*sin(g1ea2);

  ga[1][0] = -cos(g1ea1)*sin(g1ea3)-sin(g1ea1)*cos(g1ea3)*cos(g1ea2);
  ga[1][1] = -sin(g1ea1)*sin(g1ea3)+cos(g1ea1)*cos(g1ea3)*cos(g1ea2);
  ga[1][2] =  cos(g1ea3)*sin(g1ea2);

  ga[2][0] =  sin(g1ea1)*sin(g1ea2);
  ga[2][1] = -cos(g1ea1)*sin(g1ea2);
  ga[2][2] =  cos(g1ea2);

  gret[0][0] = o[0][0]*ga[0][0] + o[0][1]*ga[1][0] + o[0][2]*ga[2][0];
  gret[0][1] = o[0][0]*ga[0][1] + o[0][1]*ga[1][1] + o[0][2]*ga[2][1];
  gret[0][2] = o[0][0]*ga[0][2] + o[0][1]*ga[1][2] + o[0][2]*ga[2][2];

  gret[1][0] = o[1][0]*ga[0][0] + o[1][1]*ga[1][0] + o[1][2]*ga[2][0];
  gret[1][1] = o[1][0]*ga[0][1] + o[1][1]*ga[1][1] + o[1][2]*ga[2][1];
  gret[1][2] = o[1][0]*ga[0][2] + o[1][1]*ga[1][2] + o[1][2]*ga[2][2];

  gret[2][0] = o[2][0]*ga[0][0] + o[2][1]*ga[1][0] + o[2][2]*ga[2][0];
  gret[2][1] = o[2][0]*ga[0][1] + o[2][1]*ga[1][1] + o[2][2]*ga[2][1];
  gret[2][2] = o[2][0]*ga[0][2] + o[2][1]*ga[1][2] + o[2][2]*ga[2][2];
}

void computeMOHelperHCP(double g1ea1,double g1ea2,double g1ea3,double gret[3][3],int HexSymOp)
{
  double o[3][3];
  double a=sqrt(double(3))/2;
  if (HexSymOp == 0)
  {
    o[0][0] = 1.0; o[0][1] = 0.0; o[0][2] = 0.0;
    o[1][0] = 0.0; o[1][1] = 1.0; o[1][2] = 0.0;
    o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = 1.0;
  }
  else if (HexSymOp == 1)
  {
    o[0][0] = -0.5; o[0][1] = a; o[0][2] =  0.0;
    o[1][0] = -a; o[1][1] = -0.5; o[1][2] = 0.0;
    o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = 1.0;
  }
  else if (HexSymOp == 2)
  {
    o[0][0] = -0.5; o[0][1] =  -a; o[0][2] =  0.0;
    o[1][0] = a; o[1][1] = -0.5; o[1][2] =  0.0;
    o[2][0] = 0.0; o[2][1] =  0.0; o[2][2] = 1.0;
  }
  else if (HexSymOp == 3)
  {
    o[0][0] = 0.5; o[0][1] =  a; o[0][2] = 0.0;
    o[1][0] = -a; o[1][1] =  0.5; o[1][2] = 0.0;
    o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = 1.0;
  }
  else if (HexSymOp == 4)
  {
    o[0][0] = -1.0; o[0][1] = 0.0; o[0][2] = 0.0;
    o[1][0] = 0.0; o[1][1] = -1.0; o[1][2] =  0.0;
    o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] =  1.0;
  }
  else if (HexSymOp == 5)
  {
    o[0][0] =  0.5; o[0][1] = -a; o[0][2] = 0.0;
    o[1][0] =  a; o[1][1] = 0.5; o[1][2] = 0.0;
    o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = 1.0;
  }
  else if (HexSymOp == 6)
  {
    o[0][0] = -0.5; o[0][1] = -a; o[0][2] =  0.0;
    o[1][0] =  -a; o[1][1] = 0.5; o[1][2] =  0.0;
    o[2][0] =  0.0; o[2][1] = 0.0; o[2][2] = -1.0;
  }
  else if (HexSymOp == 7)
  {
    o[0][0] = 1.0; o[0][1] =  0.0; o[0][2] = 0.0;
    o[1][0] =  0.0; o[1][1] = -1.0; o[1][2] = 0.0;
    o[2][0] =  0.0; o[2][1] =  0.0; o[2][2] = -1.0;
  }
  else if (HexSymOp == 8)
  {
    o[0][0] =  -0.5; o[0][1] = a; o[0][2] = 0.0;
    o[1][0] = a; o[1][1] = 0.5; o[1][2] = 0.0;
    o[2][0] =  0.0; o[2][1] = 0.0; o[2][2] = -1.0;
  }
  else if (HexSymOp == 9)
  {
    o[0][0] = 0.5; o[0][1] = a; o[0][2] = 0.0;
    o[1][0] = a; o[1][1] =  -0.5; o[1][2] = 0.0;
    o[2][0] = 0.0; o[2][1] =  0.0; o[2][2] = -1.0;
  }
  else if (HexSymOp == 10)
  {
    o[0][0] =  -1.0; o[0][1] = 0.0; o[0][2] = 0.0;
    o[1][0] =  0.0; o[1][1] =  1.0; o[1][2] = 0.0;
    o[2][0] = 0.0; o[2][1] =  0.0; o[2][2] = -1.0;
  }
  else
  {
    o[0][0] =  0.5; o[0][1] =  -a; o[0][2] = 0.0;
    o[1][0] = -a; o[1][1] =  -0.5; o[1][2] = 0.0;
    o[2][0] =  0.0; o[2][1] = 0.0; o[2][2] = -1.0;
  }

  double ga[3][3];
  ga[0][0] = cos(g1ea1)*cos(g1ea3)-sin(g1ea1)*sin(g1ea3)*cos(g1ea2);
  ga[0][1] = sin(g1ea1)*cos(g1ea3)+cos(g1ea1)*sin(g1ea3)*cos(g1ea2);
  ga[0][2] = sin(g1ea3)*sin(g1ea2);

  ga[1][0] = -cos(g1ea1)*sin(g1ea3)-sin(g1ea1)*cos(g1ea3)*cos(g1ea2);
  ga[1][1] = -sin(g1ea1)*sin(g1ea3)+cos(g1ea1)*cos(g1ea3)*cos(g1ea2);
  ga[1][2] =  cos(g1ea3)*sin(g1ea2);

  ga[2][0] =  sin(g1ea1)*sin(g1ea2);
  ga[2][1] = -cos(g1ea1)*sin(g1ea2);
  ga[2][2] =  cos(g1ea2);

  gret[0][0] = o[0][0]*ga[0][0] + o[0][1]*ga[1][0] + o[0][2]*ga[2][0];
  gret[0][1] = o[0][0]*ga[0][1] + o[0][1]*ga[1][1] + o[0][2]*ga[2][1];
  gret[0][2] = o[0][0]*ga[0][2] + o[0][1]*ga[1][2] + o[0][2]*ga[2][2];

  gret[1][0] = o[1][0]*ga[0][0] + o[1][1]*ga[1][0] + o[1][2]*ga[2][0];
  gret[1][1] = o[1][0]*ga[0][1] + o[1][1]*ga[1][1] + o[1][2]*ga[2][1];
  gret[1][2] = o[1][0]*ga[0][2] + o[1][1]*ga[1][2] + o[1][2]*ga[2][2];

  gret[2][0] = o[2][0]*ga[0][0] + o[2][1]*ga[1][0] + o[2][2]*ga[2][0];
  gret[2][1] = o[2][0]*ga[0][1] + o[2][1]*ga[1][1] + o[2][2]*ga[2][1];
  gret[2][2] = o[2][0]*ga[0][2] + o[2][1]*ga[1][2] + o[2][2]*ga[2][2];

}

double computemisorientation(double crystruct,double g1ea1,double g1ea2,double g1ea3,double g2ea1,double g2ea2,double g2ea3,double &n1,double &n2,double &n3)
{
  double wmin=9999999,na,nb,nc;
  for (int i = 0; i < (crystruct*12); i++)
  {
    double gb[3][3];
    if(crystruct == 2)
    {
      computeMOg(g2ea1,g2ea2,g2ea3,gb,i);
    }
    if(crystruct == 1)
    {
      computeMOHelperHCP(g2ea1,g2ea2,g2ea3,gb,i);
    }
    for (int j = 0; j < (crystruct*12); j++)
    {
      double ga[3][3];
      if(crystruct == 2)
      {
        computeMOg(g1ea1,g1ea2,g1ea3,ga,j);
      }
      if(crystruct == 1)
      {
        computeMOHelperHCP(g1ea1,g1ea2,g1ea3,ga,j);
      }
      double dg[3][3];
      dg[0][0] = gb[0][0]*ga[0][0] + gb[0][1]*ga[0][1] + gb[0][2]*ga[0][2];
      dg[0][1] = gb[0][0]*ga[1][0] + gb[0][1]*ga[1][1] + gb[0][2]*ga[1][2];
      dg[0][2] = gb[0][0]*ga[2][0] + gb[0][1]*ga[2][1] + gb[0][2]*ga[2][2];
      dg[1][0] = gb[1][0]*ga[0][0] + gb[1][1]*ga[0][1] + gb[1][2]*ga[0][2];
      dg[1][1] = gb[1][0]*ga[1][0] + gb[1][1]*ga[1][1] + gb[1][2]*ga[1][2];
      dg[1][2] = gb[1][0]*ga[2][0] + gb[1][1]*ga[2][1] + gb[1][2]*ga[2][2];
      dg[2][0] = gb[2][0]*ga[0][0] + gb[2][1]*ga[0][1] + gb[2][2]*ga[0][2];
      dg[2][1] = gb[2][0]*ga[1][0] + gb[2][1]*ga[1][1] + gb[2][2]*ga[1][2];
      dg[2][2] = gb[2][0]*ga[2][0] + gb[2][1]*ga[2][1] + gb[2][2]*ga[2][2];
      double w=acos((dg[0][0]+dg[1][1]+dg[2][2]-1)/2);
      double den=sqrt((dg[1][2]-dg[2][1])*(dg[1][2]-dg[2][1])+(dg[2][0]-dg[0][2])*(dg[2][0]-dg[0][2])+(dg[0][1]-dg[1][0])*(dg[0][1]-dg[1][0]));
      if(den == 0)
      {
        na=0;
        nb=0;
        nc=0;
      }
      if(den != 0)
      {
        na=(dg[1][2]-dg[2][1])/den;
        nb=(dg[2][0]-dg[0][2])/den;
        nc=(dg[0][1]-dg[1][0])/den;
      }
   //   double nval=1/sqrt(3.0);
      if (wmin>fabs(w))
      {
        wmin=fabs(w);
        n1=na;
        n2=nb;
        n3=nc;
      }
    }
  }
  return wmin;
}

double GetMisorientationOnly(int crystruct,double g1ea1,double g1ea2,double g1ea3,double g2ea1,double g2ea2,double g2ea3,double &n1,double &n2,double &n3)
{
  double wret=computemisorientation(crystruct,g1ea1,g1ea2,g1ea3,g2ea1,g2ea2,g2ea3,n1,n2,n3);
  return wret;
}

/********************************** End of Misorientation.h *************************/

double gamma(double x)
{
	int i,k,m;
	double ga,gr,r,z;
	// double m_pi = 3.1415926535897;

	static double g[] = {
		1.0,
		0.5772156649015329,
		-0.6558780715202538,
		-0.420026350340952e-1,
		0.1665386113822915,
		-0.421977345555443e-1,
		-0.9621971527877e-2,
		0.7218943246663e-2,
		-0.11651675918591e-2,
		-0.2152416741149e-3,
		0.1280502823882e-3,
		-0.201348547807e-4,
		-0.12504934821e-5,
		0.1133027232e-5,
		-0.2056338417e-6,
		0.6116095e-8,
		0.50020075e-8,
		-0.11812746e-8,
		0.1043427e-9,
		0.77823e-11,
		-0.36968e-11,
		0.51e-12,
		-0.206e-13,
		-0.54e-14,
		0.14e-14};

		if (x > 171.0) return 1e308;    // This value is an overflow flag.
		if (x == (int)x) {
			if (x > 0.0) {
				ga = 1.0;               // use factorial
				for (i=2;i<x;i++) {
					ga *= i;
				}
			}
			else
				ga = 1e308;
		}
		else {
			if (fabs(x) > 1.0) {
				z = fabs(x);
				m = (int)z;
				r = 1.0;
				for (k=1;k<=m;k++) {
					r *= (z-k);
				}
				z -= m;
			}
			else
				z = x;
			gr = g[24];
			for (k=23;k>=0;k--) {
				gr = gr*z+g[k];
			}
			ga = 1.0/(gr*z);
			if (fabs(x) > 1.0) {
				ga *= r;
				if (x < 0.0) {
					ga = -1*m_pi/(x*ga*sin(m_pi*x));
				}
			}
		}
		return ga;
}


// class that holds seed coordinates loaded from data file
struct grains
{
	double euler1;
	double euler2;
	double euler3;
	double rad1x;
	double rad1y;
	double rad1z;
	double rad2x;
	double rad2y;
	double rad2z;
	double rad3x;
	double rad3y;
	double rad3z;
	int grainname;
	int neighnum;
	double volume;
	double axisa;
	double axisb;
	double axisc;
	double Nvalue;
};
struct bins
{
	double euler1;
	double euler2;
	double euler3;
	double dprobability;
	double sprobability;
	double Nprobability;
	double Nvalue;
	double bctotprobability;
	double diameter;
	double rad;
	double bctotal;
	float difference;
	float height;
	float binrank;
};

struct orients
{
	double rad1x;
	double rad1y;
	double rad1z;
	double rad2x;
	double rad2y;
	double rad2z;
	double rad3x;
	double rad3y;
	double rad3z;
};

struct gridpoints
{
	int neighbor1;
	int neighbor2;
	int neighbor3;
	int neighbor4;
	int neighbor5;
	int neighbor6;
	int boundarypoint;
	int available;
	int available90;
	int inside;
	int grainname;
	int neighbor;
	int hasneigh;
	list<int> voxneighlist;
};

struct boundarypoints
{
	 double x;
	 double y;
	 double z;
};

struct packedgrains
{
	int grainname;
	int vol;
	double nserror;
	double x;
	double y;
	double z;
	int currentsize;
	int initsize;
	int edgegrain;
	int temponsurf;
	int tempneighnum;
	double radius1;
	double radius2;
	double radius3;
	int neighnum;
	double xc;
	double yc;
	double zc;
	double Ixx;
	double Iyy;
	double Izz;
	double Ixy;
	double Iyz;
	double Ixz;
	double axis1;
	double axis2;
	double axis3;
	double euler1;
	double euler2;
	double euler3;
	double red;
	double green;
	double blue;
	list<float> misorientationlist;
	double lowanglefraction;
	double grainrank;
	double grainmicrorank;
	double picked;
	double frozen;
	double oeuler1;
	double oeuler2;
	double oeuler3;
};


int32 seeder = time(0);
AIMRandomNG rg;

grains *tempgrain;
grains *grain;
int *gsizes;
int *gremovals;
bins *diambin;
bins *boverabin;
bins *coverabin;
bins *coverbbin;
bins *seNbin;
int ***eulercount;
bins *eulerbin;
orients * orient;
gridpoints *gridfine;
gridpoints *gridcourse;
packedgrains *packedgrain;
bins *actualmisobin;
bins *simmisobin;
bins *actualmicrobin;
bins *simmicrobin;
std::vector< std::vector<int> > voxelvector;
std::vector< std::vector<int> > neighborvector;
double **svn;
double **svs;
double **nsdist;
double **bcent;
int *takencheck;
int numdiambins;
int numshapebins;
int numseNbins;
int numsizebins;
int numorients;
int numeulers;
int nummisobins;
int nummicrobins;
int misoiter;
int nummisomoves;
int nummicromoves;
int numgrains;
int shapeclass;
double overlapallowed;
int overlapassignment;
double sizex;
double sizey;
double sizez;
double resx;
double resy;
double resz;
double resx1;
double resy1;
double resz1;
int numneighbins;
int nummicros = 1;
int crystruct;
void loadVolData(string inname1, int numvolbins);
void loadboveraData(string inname2, int numshapebins);
void loadcoveraData(string inname3, int numshapebins);
void loadcoverbData(string inname4, int numshapebins);
void loadNData(string inname5, int numshapebins);
void loadorientData(string inname6, int numorients);
void loadeulerData(string inname7, int numeulers);
void generate_grains(int numgrains);
void assign_eulers(int numgrains);
void loadSVNData(string inname8);
void loadSVSData(string inname9);
void make_points(int numgrains);
void fill_gaps(int numgrains);
void find_neighbors(int numgrains);
void find_quadgroups1(int numgrains);
void find_centroids(int numgrains);
void find_moments(int numgrains);
void find_axis(int numgrains);
void find_colors(int numgrains);
void volume_stats(int numgrains, string outname2);
void writeCube(string outname1, int numgrains);
void write_grains(string outname4, int numgrains);
void find_boundarycenters(string outname3,int numgrains);
void loadMisoData(string inname10);
void loadMicroData(string inname11);
void measure_misorientations(int numgrains);
void rank_misobins(int numgrains);
void count_misorientations(int numgrains);
int rank_microbins(int numgrains);
void rank_grains1(int numgrains);
void identify_grains1(int numgrains,int nummisomoves);
void move_grains1(int numgrains);
void rank_grains2(int numgrains);
void identify_grains2(int numgrains,int nummicromoves);
void move_grains2(int numgrains);
void freeze_grains(int numgrains);
void write_volume(string outname5);



#ifdef SVG_3D_LIBRARY
int SVG_3D_Main(const std::string &inDir, const std::string &outDir,
                int ng, int sc, double rx, double ry, double rz,
                double oallowed, int oassignment, int crxst)
{
  numgrains = ng;
  shapeclass = sc;
  resx = rx;
  resy = ry;
  resz = rz;
  overlapallowed = oallowed;
  overlapassignment = oassignment;
  crystruct = crxst;

  CREATE_INPUT_FILENAME(readname1, AIM::Representation::VolBinFile)
  CREATE_INPUT_FILENAME(readname2, AIM::Representation::BOverABinsFile)
  CREATE_INPUT_FILENAME(readname3, AIM::Representation::COverABinsFile)
  CREATE_INPUT_FILENAME(readname4, AIM::Representation::COverBBinsFile)
  CREATE_INPUT_FILENAME(readname5, AIM::Representation::SeNBinsFile)
  CREATE_INPUT_FILENAME(readname6, AIM::Representation::AxisOrientationsFile)
  CREATE_INPUT_FILENAME(readname7, AIM::Representation::EulerAnglesFile)
  CREATE_INPUT_FILENAME(readname8, AIM::Representation::SVNFile)
  CREATE_INPUT_FILENAME(readname9, AIM::Representation::SVSFile)
  CREATE_INPUT_FILENAME(readname10, AIM::Representation::MisorientationBinsFile)
  CREATE_INPUT_FILENAME(readname11, AIM::Representation::MicroBinsFile)

  CREATE_OUTPUT_FILENAME(writename1, AIM::Representation::CubeFile)
  CREATE_OUTPUT_FILENAME(writename2, AIM::Representation::AnalysisFile)
  CREATE_OUTPUT_FILENAME(writename3, AIM::Representation::BoundaryCentersFile)
  CREATE_OUTPUT_FILENAME(writename4, AIM::Representation::GrainsFile)
  CREATE_OUTPUT_FILENAME(writename5, AIM::Representation::VolumeFile)

#else
int main(int argc, char **argv)

{
  rg.RandomInit(seeder);
  string readname1;
  string readname2;
  string readname3;
  string readname4;
  string readname5;
  string readname6;
  string readname7;
  string readname8;
  string readname9;
  string readname10;
  string readname11;
  string writename1;
  string writename2;
  string writename3;
  string writename4;
  string writename5;

  readname1 = "volbins.txt";
  readname2 = "boverabins.txt";
  readname3 = "coverabins.txt";
  readname4 = "coverbbins.txt";
  readname5 = "seNbins.txt";
  readname6 = "axisorientations.txt";
  readname7 = "eulerangles.txt";
  readname8 = "svn.txt";
  readname9 = "svs.txt";
  readname10 = "misobins.txt";
  readname11 = "microbins.txt";

  writename1 = "cube.vtk";
  writename2 = "analysis.txt";
  writename3 = "boundarycenters.txt";
  writename4 = "grains.txt";
  writename5 = "volume.txt";
  cout << "Enter the number of grains desired in the volume: ";
  cin >> numgrains;
  cout << "Enter the shape class you desire (1)Ellipsoid (2)Superellipsoid (3)Cube-Octahedron:";
  cin >> shapeclass;
  cout << "Enter the x resolution of your desired volume: ";
  cin >> resx;
  cout << "Enter the y resolution of your desired volume: ";
  cin >> resy;
  cout << "Enter the z resolution of your desired volume: ";
  cin >> resz;
  cout << "Enter the overlap between grains that is acceptable: ";
  cin >> overlapallowed;
  cout << "Enter how the overlap between grains is handled (1)Rigid (2)Progressive: ";
  cin >> overlapassignment;
  cout << "Do you have a HCP (1) or FCC (2) material:" << endl;
  cin >> crystruct;
#endif


  resx1 = 4*resx;
  resy1 = 4*resy;
  resz1 = 4*resz;
  numsizebins = 11;
  numneighbins = 100;
  numdiambins = 1000;
  numshapebins = 1000;
  numseNbins = 1001;
  nummisobins = 10;
  nummicrobins = 10;
  misoiter = 0;
  nummisomoves = 250;
  nummicromoves = 25;


  ifstream inputFile1;
  inputFile1.open(readname6.c_str());
  inputFile1 >> numorients;
  inputFile1.close();
  ifstream inputFile2;
  inputFile2.open(readname7.c_str());
  inputFile2 >> numeulers;
  inputFile2.close();
  gsizes = new int[numgrains];
  gremovals = new int[numgrains];
  grain = (struct grains *)malloc((numgrains)*sizeof(struct grains));
  tempgrain = (struct grains *)malloc((numgrains)*sizeof(struct grains));
  diambin = (struct bins *)malloc((numdiambins)*sizeof(struct bins));
  orient = (struct orients *)malloc((numorients)*sizeof(struct orients));
  boverabin = (struct bins *)malloc((numsizebins*numshapebins)*sizeof(struct bins));
  coverabin = (struct bins *)malloc((numsizebins*numshapebins)*sizeof(struct bins));
  coverbbin = (struct bins *)malloc((numsizebins*numshapebins)*sizeof(struct bins));
  seNbin = (struct bins *)malloc((numsizebins*numseNbins)*sizeof(struct bins));
  eulercount = new int **[18];
  eulerbin = (struct bins *)malloc((18*18*18*100)*sizeof(struct bins));
  for(int i = 0; i < 18; i++)
  {
	  eulercount[i] = new int *[18];
	  for(int j = 0; j < 18; j++)
	  {
		  eulercount[i][j] = new int [18];
		  for(int k = 0; k < 18; k++)
		  {
			  eulercount[i][j][k] = 0;
		  }
	  }
  }
  loadVolData(readname1,numdiambins);
  loadboveraData(readname2,numshapebins);
  loadcoveraData(readname3,numshapebins);
  loadcoverbData(readname4,numshapebins);
  loadNData(readname5,numshapebins);
  loadorientData(readname6,numorients);
  loadeulerData(readname7,numeulers);
  takencheck = new int[numgrains];
  generate_grains(numgrains);
  assign_eulers(numgrains);
  svn = new double *[numsizebins];
  svs = new double *[numsizebins];
  nsdist = new double *[numgrains];
  bcent = new double *[100000];
  for(int temp = 0; temp < numsizebins; temp++)
  {
		svn[temp] = new double [numneighbins];
		svs[temp] = new double [numsizebins];
  }
  for(int temp1 = 0; temp1 < 100000; temp1++)
  {
		bcent[temp1] = new double [5];
  }
  for(int temp2 = 0; temp2 < numgrains; temp2++)
  {
		nsdist[temp2] = new double [numsizebins];
  }
  for(int temp3 = 0; temp3 < numgrains; temp3++)
  {
		for(int temp4 = 0; temp4 < numsizebins; temp4++)
		{
			nsdist[temp3][temp4] = 0;
		}
  }
  cout << "grains made" << endl;
  gridfine = (struct gridpoints *)malloc(((sizex/resx))*((sizey/resy))*((sizez/resz))*sizeof(struct gridpoints));
  cout << "fine points made" << endl;
  gridcourse = (struct gridpoints *)malloc(((sizex/resx1))*((sizey/resy1))*((sizez/resz1))*sizeof(struct gridpoints));
  cout << "coarse points made" << endl;
  packedgrain = (struct packedgrains *)malloc((numgrains)*sizeof(struct packedgrains));
  voxelvector.resize(numgrains);
  neighborvector.resize(numgrains);
  int bigerror = 10;
  for(int temp5 = 0; temp5 < numgrains; temp5++)
  {
	  packedgrain[temp5].nserror = bigerror;
  }
  loadSVNData(readname8);
  loadSVSData(readname9);
  make_points(numgrains);
  fill_gaps(numgrains);
  find_neighbors(numgrains);
  find_centroids(numgrains);
  find_moments(numgrains);
  find_axis(numgrains);
  find_colors(numgrains);
  actualmisobin = (struct bins *)malloc((nummisobins)*sizeof(struct bins));
  simmisobin = (struct bins *)malloc((nummisobins)*sizeof(struct bins));
  actualmicrobin = (struct bins *)malloc((nummicrobins)*sizeof(struct bins));
  simmicrobin = (struct bins *)malloc((nummicrobins)*sizeof(struct bins));
  loadMisoData(readname10);
  loadMicroData(readname11);
  for(int iter = 0; iter < misoiter; iter++)
  {
	measure_misorientations(numgrains);
	rank_misobins(numgrains);
	count_misorientations(numgrains);
	freeze_grains(numgrains);
	rank_grains1(numgrains);
	identify_grains1(numgrains,nummisomoves);
	move_grains1(numgrains);
  }
  while(nummicros != 1)
  {
	measure_misorientations(numgrains);
	count_misorientations(numgrains);
	nummicros = rank_microbins(numgrains);
	rank_grains2(numgrains);
	identify_grains2(numgrains,nummicromoves);
	move_grains2(numgrains);
  }
  for(int iter3 = 0; iter3 < misoiter; iter3++)
  {
	measure_misorientations(numgrains);
	rank_misobins(numgrains);
	count_misorientations(numgrains);
	freeze_grains(numgrains);
	rank_grains1(numgrains);
	identify_grains1(numgrains,nummisomoves);
	move_grains1(numgrains);
  }
  volume_stats(numgrains,writename2);
  writeCube(writename1,numgrains);
  write_grains(writename4,numgrains);
  write_volume(writename5);
  find_boundarycenters(writename3,numgrains);


 delete [] grain;
 delete [] gridfine;
 delete [] gridcourse;
 delete [] packedgrain;
 delete [] orient;
 delete [] diambin;
 delete [] boverabin;
 delete [] coverabin;
 delete [] coverbbin;
 delete [] seNbin;
 delete [] actualmisobin;
 delete [] simmisobin;
 delete [] actualmicrobin;
 delete [] simmicrobin;
 return 0;
}

void loadVolData(string inname1, int numvolbins)
{
    ifstream inputFile;
    inputFile.open(inname1.c_str());
	int count = 0;
	double dprob;
	double diam;
	for (long k = 0; k < numvolbins; k++)
    {
		inputFile >> dprob >> diam;
		diambin[k].dprobability = dprob;
		diambin[k].diameter = diam;
        count++;
    }
    inputFile.close();
}

void loadboveraData(string inname2, int numshapebins)
{
    ifstream inputFile;
    inputFile.open(inname2.c_str());
	int count = 0;
	double sprob;
	double r;
	int bnum=0;
	for (long j = 0; j < numsizebins; j++)
	{
		for (long k = 0; k < numshapebins; k++)
		{
			inputFile >> r >> sprob;
			bnum = (j*numshapebins)+k;
			boverabin[bnum].sprobability = sprob;
			boverabin[bnum].rad = r;
	        count++;
		}
	}
   inputFile.close();
}

void loadcoveraData(string inname3, int numshapebins)
{
    ifstream inputFile;
    inputFile.open(inname3.c_str());
	int count = 0;
	double sprob;
	double r;
	int bnum=0;
	for (long j = 0; j < numsizebins; j++)
	{
		for (long k = 0; k < numshapebins; k++)
		{
			inputFile >> r >> sprob;
			bnum = (j*numshapebins)+k;
			coverabin[bnum].sprobability = sprob;
			coverabin[bnum].rad = r;
	        count++;
		}
	}
   inputFile.close();
}

void loadcoverbData(string inname4, int numshapebins)
{
    ifstream inputFile;
    inputFile.open(inname4.c_str());
	int count = 0;
	double sprob;
	double r;
	int bnum=0;
	for (long j = 0; j < numsizebins; j++)
	{
		for (long k = 0; k < numshapebins; k++)
		{
			inputFile >> r >> sprob;
			bnum = (j*numshapebins)+k;
			coverbbin[bnum].sprobability = sprob;
			coverbbin[bnum].rad = r;
	        count++;
		}
	}
   inputFile.close();
}

void loadNData(string inname5, int numshapebins)
{
    ifstream inputFile;
    inputFile.open(inname5.c_str());
	int count = 0;
	double Nprob;
	double N;
	double junk;
	int bnum=0;
	for (long j = 0; j < numsizebins; j++)
	{
		for (long k = 0; k < numseNbins; k++)
		{
			inputFile >> junk >> Nprob >> N;
			bnum = (j*numseNbins)+k;
			seNbin[bnum].Nprobability = Nprob;
			seNbin[bnum].Nvalue = N;
	        count++;
		}
	}
   inputFile.close();
}

void loadorientData(string inname6, int numorients)
{
    ifstream inputFile;
    inputFile.open(inname6.c_str());
	int count = 0;
	double r1x;
	double r1y;
	double r1z;
	double r2x;
	double r2y;
	double r2z;
	double r3x;
	double r3y;
	double r3z;
	inputFile >> numorients;
	for (long k = 0; k < numorients; k++)
    {
		inputFile >> r1x >> r1y >> r1z >> r2x >> r2y >> r2z >> r3x >> r3y >> r3z;
		double denom1 = pow((r1x*r1x+r1y*r1y+r1z*r1z),0.5);
		double denom2 = pow((r2x*r2x+r2y*r2y+r2z*r2z),0.5);
		double denom3 = pow((r3x*r3x+r3y*r3y+r3z*r3z),0.5);
		r1x = r1x/denom1;
		r1y = r1y/denom1;
		r1z = r1z/denom1;
		r2x = r2x/denom2;
		r2y = r2y/denom2;
		r2z = r2z/denom2;
		r3x = r3x/denom3;
		r3y = r3y/denom3;
		r3z = r3z/denom3;
		orient[k].rad1x = r1x;
		orient[k].rad1y = r1y;
		orient[k].rad1z = r1z;
		orient[k].rad2x = r2x;
		orient[k].rad2y = r2y;
		orient[k].rad2z = r2z;
		orient[k].rad3x = r3x;
		orient[k].rad3y = r3y;
		orient[k].rad3z = r3z;
        count++;
    }
    inputFile.close();
}

void loadeulerData(string inname7, int numeulers)
{
    ifstream inputFile;
    inputFile.open(inname7.c_str());
//	int count = 0;
	double g1ea1;
	double g1ea2;
	double g1ea3;
	double ea1;
	double ea2;
	double ea3;
	int totalcount = 0;
//	int bnum=0;
	inputFile >> numeulers;
	for (long iter = 0; iter < numeulers; iter++)
    {
		inputFile >> g1ea1 >> g1ea2 >> g1ea3;
		for(int k = 0; k < 24; k++)
		{
			double o[3][3];
			double ga[3][3];
			double m1[3][3];
			double mo[3][3];
			if (k == 0)
			{
				o[0][0] = 1.0; o[0][1] = 0.0; o[0][2] = 0.0;
				o[1][0] = 0.0; o[1][1] = 1.0; o[1][2] = 0.0;
				o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = 1.0;
			}
			else if (k == 1)
			{
				o[0][0] = 1.0; o[0][1] = 0.0; o[0][2] =  0.0;
				o[1][0] = 0.0; o[1][1] = 0.0; o[1][2] = -1.0;
				o[2][0] = 0.0; o[2][1] = 1.0; o[2][2] =  0.0;
			}
			else if (k == 2)
			{
				o[0][0] = 1.0; o[0][1] =  0.0; o[0][2] =  0.0;
				o[1][0] = 0.0; o[1][1] = -1.0; o[1][2] =  0.0;
				o[2][0] = 0.0; o[2][1] =  0.0; o[2][2] = -1.0;
			}
			else if (k == 3)
			{
				o[0][0] = 1.0; o[0][1] =  0.0; o[0][2] = 0.0;
				o[1][0] = 0.0; o[1][1] =  0.0; o[1][2] = 1.0;
				o[2][0] = 0.0; o[2][1] = -1.0; o[2][2] = 0.0;
			}
			else if (k == 4)
			{
				o[0][0] = 0.0; o[0][1] = 0.0; o[0][2] = -1.0;
				o[1][0] = 0.0; o[1][1] = 1.0; o[1][2] =  0.0;
				o[2][0] = 1.0; o[2][1] = 0.0; o[2][2] =  0.0;
			}
			else if (k == 5)
			{
				o[0][0] =  0.0; o[0][1] = 0.0; o[0][2] = 1.0;
				o[1][0] =  0.0; o[1][1] = 1.0; o[1][2] = 0.0;
				o[2][0] = -1.0; o[2][1] = 0.0; o[2][2] = 0.0;
			}
			else if (k == 6)
			{
				o[0][0] = -1.0; o[0][1] = 0.0; o[0][2] =  0.0;
				o[1][0] =  0.0; o[1][1] = 1.0; o[1][2] =  0.0;
				o[2][0] =  0.0; o[2][1] = 0.0; o[2][2] = -1.0;
			}
			else if (k == 7)
			{
				o[0][0] = -1.0; o[0][1] =  0.0; o[0][2] = 0.0;
				o[1][0] =  0.0; o[1][1] = -1.0; o[1][2] = 0.0;
				o[2][0] =  0.0; o[2][1] =  0.0; o[2][2] = 1.0;
			}
			else if (k == 8)
			{
				o[0][0] =  0.0; o[0][1] = 1.0; o[0][2] = 0.0;
				o[1][0] = -1.0; o[1][1] = 0.0; o[1][2] = 0.0;
				o[2][0] =  0.0; o[2][1] = 0.0; o[2][2] = 1.0;
			}
			else if (k == 9)
			{
				o[0][0] = 0.0; o[0][1] = -1.0; o[0][2] = 0.0;
				o[1][0] = 1.0; o[1][1] =  0.0; o[1][2] = 0.0;
				o[2][0] = 0.0; o[2][1] =  0.0; o[2][2] = 1.0;
			}
			else if (k == 10)
			{
				o[0][0] =  0.0; o[0][1] = -1.0; o[0][2] = 0.0;
				o[1][0] =  0.0; o[1][1] =  0.0; o[1][2] = 1.0;
				o[2][0] = -1.0; o[2][1] =  0.0; o[2][2] = 0.0;
			}
			else if (k == 11)
			{
				o[0][0] =  0.0; o[0][1] =  0.0; o[0][2] = 1.0;
				o[1][0] = -1.0; o[1][1] =  0.0; o[1][2] = 0.0;
				o[2][0] =  0.0; o[2][1] = -1.0; o[2][2] = 0.0;
			}
			else if (k == 12)
			{
				o[0][0] = 0.0; o[0][1] = -1.0; o[0][2] =  0.0;
				o[1][0] = 0.0; o[1][1] =  0.0; o[1][2] = -1.0;
				o[2][0] = 1.0; o[2][1] =  0.0; o[2][2] =  0.0;
			}
			else if (k == 13)
			{
				o[0][0] = 0.0; o[0][1] =  0.0; o[0][2] = -1.0;
				o[1][0] = 1.0; o[1][1] =  0.0; o[1][2] =  0.0;
				o[2][0] = 0.0; o[2][1] = -1.0; o[2][2] =  0.0;
			}
			else if (k == 14)
			{
				o[0][0] =  0.0; o[0][1] = 1.0; o[0][2] =  0.0;
				o[1][0] =  0.0; o[1][1] = 0.0; o[1][2] = -1.0;
				o[2][0] = -1.0; o[2][1] = 0.0; o[2][2] =  0.0;
			}
			else if (k == 15)
			{
				o[0][0] =  0.0; o[0][1] = 0.0; o[0][2] = -1.0;
				o[1][0] = -1.0; o[1][1] = 0.0; o[1][2] =  0.0;
				o[2][0] =  0.0; o[2][1] = 1.0; o[2][2] =  0.0;
			}
			else if (k == 16)
			{
				o[0][0] = 0.0; o[0][1] = 1.0; o[0][2] = 0.0;
				o[1][0] = 0.0; o[1][1] = 0.0; o[1][2] = 1.0;
				o[2][0] = 1.0; o[2][1] = 0.0; o[2][2] = 0.0;
			}
			else if (k == 17)
			{
				o[0][0] = 0.0; o[0][1] = 0.0; o[0][2] = 1.0;
				o[1][0] = 1.0; o[1][1] = 0.0; o[1][2] = 0.0;
				o[2][0] = 0.0; o[2][1] = 1.0; o[2][2] = 0.0;
			}
			else if (k == 18)
			{
				o[0][0] = 0.0; o[0][1] = 1.0; o[0][2] =  0.0;
				o[1][0] = 1.0; o[1][1] = 0.0; o[1][2] =  0.0;
				o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = -1.0;
			}
			else if (k == 19)
			{
				o[0][0] = -1.0; o[0][1] = 0.0; o[0][2] = 0.0;
				o[1][0] =  0.0; o[1][1] = 0.0; o[1][2] = 1.0;
				o[2][0] =  0.0; o[2][1] = 1.0; o[2][2] = 0.0;
			}
			else if (k == 20)
			{
				o[0][0] = 0.0; o[0][1] =  0.0; o[0][2] = 1.0;
				o[1][0] = 0.0; o[1][1] = -1.0; o[1][2] = 0.0;
				o[2][0] = 1.0; o[2][1] =  0.0; o[2][2] = 0.0;
			}
			else if (k == 21)
			{
				o[0][0] = -1.0; o[0][1] =  0.0; o[0][2] =  0.0;
				o[1][0] =  0.0; o[1][1] =  0.0; o[1][2] = -1.0;
				o[2][0] =  0.0; o[2][1] = -1.0; o[2][2] =  0.0;
			}
			else if (k == 22)
			{
				o[0][0] =  0.0; o[0][1] =  0.0; o[0][2] = -1.0;
				o[1][0] =  0.0; o[1][1] = -1.0; o[1][2] =  0.0;
				o[2][0] = -1.0; o[2][1] =  0.0; o[2][2] =  0.0;
			}
			else if (k == 23)
			{
				o[0][0] =  0.0; o[0][1] = -1.0; o[0][2] =  0.0;
				o[1][0] = -1.0; o[1][1] =  0.0; o[1][2] =  0.0;
				o[2][0] =  0.0; o[2][1] =  0.0; o[2][2] = -1.0;
			}
			for(int p = 0; p < 4; p++)
			{
				double o2[3][3];
				if (p == 0)
				{
					o2[0][0] = 1.0; o2[0][1] = 0.0; o2[0][2] = 0.0;
					o2[1][0] = 0.0; o2[1][1] = 1.0; o2[1][2] = 0.0;
					o2[2][0] = 0.0; o2[2][1] = 0.0; o2[2][2] = 1.0;
				}
				else if (p == 1)
				{
					o2[0][0] = 1.0; o2[0][1] = 0.0; o2[0][2] =  0.0;
					o2[1][0] = 0.0; o2[1][1] = -1.0; o2[1][2] = 0.0;
					o2[2][0] = 0.0; o2[2][1] = 0.0; o2[2][2] =  -1.0;
				}
				else if (p == 2)
				{
					o2[0][0] = -1.0; o2[0][1] =  0.0; o2[0][2] =  0.0;
					o2[1][0] = 0.0; o2[1][1] = 1.0; o2[1][2] =  0.0;
					o2[2][0] = 0.0; o2[2][1] =  0.0; o2[2][2] = -1.0;
				}
				else if (p == 3)
				{
					o2[0][0] = -1.0; o2[0][1] =  0.0; o2[0][2] = 0.0;
					o2[1][0] = 0.0; o2[1][1] = -1.0; o2[1][2] = 0.0;
					o2[2][0] = 0.0; o2[2][1] = 0.0; o2[2][2] = 1.0;
				}
				ga[0][0] = cos(g1ea1)*cos(g1ea3)-sin(g1ea1)*sin(g1ea3)*cos(g1ea2);
				ga[0][1] = sin(g1ea1)*cos(g1ea3)+cos(g1ea1)*sin(g1ea3)*cos(g1ea2);
				ga[0][2] = sin(g1ea3)*sin(g1ea2);
				ga[1][0] = -cos(g1ea1)*sin(g1ea3)-sin(g1ea1)*cos(g1ea3)*cos(g1ea2);
				ga[1][1] = -sin(g1ea1)*sin(g1ea3)+cos(g1ea1)*cos(g1ea3)*cos(g1ea2);
				ga[1][2] =  cos(g1ea3)*sin(g1ea2);
				ga[2][0] =  sin(g1ea1)*sin(g1ea2);
				ga[2][1] = -cos(g1ea1)*sin(g1ea2);
				ga[2][2] =  cos(g1ea2);

				mo[0][0] = o[0][0]*ga[0][0] + o[0][1]*ga[1][0] + o[0][2]*ga[2][0];
				mo[0][1] = o[0][0]*ga[0][1] + o[0][1]*ga[1][1] + o[0][2]*ga[2][1];
				mo[0][2] = o[0][0]*ga[0][2] + o[0][1]*ga[1][2] + o[0][2]*ga[2][2];
				mo[1][0] = o[1][0]*ga[0][0] + o[1][1]*ga[1][0] + o[1][2]*ga[2][0];
				mo[1][1] = o[1][0]*ga[0][1] + o[1][1]*ga[1][1] + o[1][2]*ga[2][1];
				mo[1][2] = o[1][0]*ga[0][2] + o[1][1]*ga[1][2] + o[1][2]*ga[2][2];
				mo[2][0] = o[2][0]*ga[0][0] + o[2][1]*ga[1][0] + o[2][2]*ga[2][0];
				mo[2][1] = o[2][0]*ga[0][1] + o[2][1]*ga[1][1] + o[2][2]*ga[2][1];
				mo[2][2] = o[2][0]*ga[0][2] + o[2][1]*ga[1][2] + o[2][2]*ga[2][2];

				m1[0][0] = mo[0][0]*o2[0][0] + mo[0][1]*o2[1][0] + mo[0][2]*o2[2][0];
				m1[0][1] = mo[0][0]*o2[0][1] + mo[0][1]*o2[1][1] + mo[0][2]*o2[2][1];
				m1[0][2] = mo[0][0]*o2[0][2] + mo[0][1]*o2[1][2] + mo[0][2]*o2[2][2];
				m1[1][0] = mo[1][0]*o2[0][0] + mo[1][1]*o2[1][0] + mo[1][2]*o2[2][0];
				m1[1][1] = mo[1][0]*o2[0][1] + mo[1][1]*o2[1][1] + mo[1][2]*o2[2][1];
				m1[1][2] = mo[1][0]*o2[0][2] + mo[1][1]*o2[1][2] + mo[1][2]*o2[2][2];
				m1[2][0] = mo[2][0]*o2[0][0] + mo[2][1]*o2[1][0] + mo[2][2]*o2[2][0];
				m1[2][1] = mo[2][0]*o2[0][1] + mo[2][1]*o2[1][1] + mo[2][2]*o2[2][1];
				m1[2][2] = mo[2][0]*o2[0][2] + mo[2][1]*o2[1][2] + mo[2][2]*o2[2][2];

				ea2 = acos(m1[2][2]);
				double cosine3 = (m1[1][2]/sin(ea2));
				double sine3 = (m1[0][2]/sin(ea2));
				double cosine1 = (-m1[2][1]/sin(ea2));
				double sine1 = (m1[2][0]/sin(ea2));
				ea3 = acos(cosine3);
				ea1 = acos(cosine1);
				if(sine3 < 0) ea3 = (2*3.1415926535897)-ea3;
				if(sine1 < 0) ea1 = (2*3.1415926535897)-ea1;
				int ea1bin = ea1/(3.1415926535897/36);
				int ea2bin = ea2/(3.1415926535897/36);
				int ea3bin = ea3/(3.1415926535897/36);
				if(ea1 >= 0 && ea2 >= 0 && ea3 >= 0 && ea1 <= (3.1415926535897/2) && ea2 <= (3.1415926535897/2) && ea3 <= (3.1415926535897/2))
				{
				//	int curcount = eulercount[ea1bin][ea2bin][ea3bin];
					int bnum = (ea1bin*18*18)+(ea2bin*18)+ea3bin;
					eulerbin[bnum].euler1 = ea1;
					eulerbin[bnum].euler2 = ea2;
					eulerbin[bnum].euler3 = ea3;
					eulercount[ea1bin][ea2bin][ea3bin]++;
					totalcount++;
				}
			}
		}
    }
    inputFile.close();
}

void generate_grains(int numgrains)
{
	int good = 1;
	double totvol = 0;
	int curbin1 = 0;
	int curbin2 = 0;
	int curbin3 = 0;
	//int curbin4 = 0;
	int curbin5 = 0;
	double r1 = 1;
	int bnum=0;
	srand(static_cast<unsigned int>(time(NULL)));
	for(int l = 0; l < numgrains; l++)
	{
		curbin1 = 0;
		good = 1;
		double random = rg.Random();
		double random1 = random;
		for(int i = 0; i < numdiambins; i++)
		{
			double dprob = diambin[i].dprobability;
			if(dprob < random1) curbin1 = i;
		}
		double diam = diambin[curbin1].diameter;
		double vol = (4.0/3.0)*(3.1415926535897)*((diam/2.0)*(diam/2.0)*(diam/2.0));
		int diameter = int(diam);
		if(diameter >= numsizebins) diameter = numsizebins-1;
		int good = 0;
		while(good == 0)
		{
			random = rg.Random();
			double random2 = random;
			random = rg.Random();
			double random3 = random;
			for(int j = 0; j < numshapebins; j++)
			{
				bnum = (diameter*numshapebins)+j;
				double sprob1 = boverabin[bnum].sprobability;
				double sprob2 = coverabin[bnum].sprobability;
				if(sprob1 < random2) curbin2 = j;
				if(sprob2 < random3) curbin3 = j;
			}
			random = rg.Random();
			double random4 = 0.01*random;
			bnum = (diameter*numshapebins)+curbin2;
			double tempr2 = boverabin[bnum].rad;
			bnum = (diameter*numshapebins)+curbin3;
			double tempr3 = coverabin[bnum].rad;
			double coverb = tempr3/tempr2;
			double sprob3 = 0;
			for(int k = 0; k < numshapebins; k++)
			{
				bnum = (diameter*numshapebins)+k;
				double checkcoverb = coverbbin[bnum].rad;
				if(checkcoverb < coverb)
				{
					sprob3 = coverbbin[bnum].sprobability;
				}
			}
			if(sprob3 > random4) good = 1;
			if(coverb > 1) good = 0;
		}
		bnum = (diameter*numshapebins)+curbin2;
		double r2 = boverabin[bnum].rad;
		bnum = (diameter*numshapebins)+curbin3;
		double r3 = coverabin[bnum].rad;
		random = rg.Random();
		double random5 = random;
		int onum = random5*numorients;
		double r1x = orient[onum].rad1x;
		double r1y = orient[onum].rad1y;
		double r1z = orient[onum].rad1z;
		double r2x = orient[onum].rad2x;
		double r2y = orient[onum].rad2y;
		double r2z = orient[onum].rad2z;
		double r3x = orient[onum].rad3x;
		double r3y = orient[onum].rad3y;
		double r3z = orient[onum].rad3z;
		random = rg.Random();
		double random6 = random;
		for(int p = 0; p < numseNbins; p++)
		{
			bnum = (diameter*numseNbins)+p;
			double Nprob = seNbin[bnum].Nprobability;
			if(Nprob < random6) curbin5 = p;
		}
		bnum = (diameter*numshapebins)+curbin5;
		double N = seNbin[bnum].Nvalue;
		int gnum = l;
		tempgrain[l].grainname = gnum;
		tempgrain[l].volume = vol;
		tempgrain[l].axisa = r1;
		tempgrain[l].axisb = r2;
		tempgrain[l].axisc = r3;
		tempgrain[l].rad1x = r1x;
		tempgrain[l].rad1y = r1y;
		tempgrain[l].rad1z = r1z;
		tempgrain[l].rad2x = r2x;
		tempgrain[l].rad2y = r2y;
		tempgrain[l].rad2z = r2z;
		tempgrain[l].rad3x = r3x;
		tempgrain[l].rad3y = r3y;
		tempgrain[l].rad3z = r3z;
		tempgrain[l].Nvalue = N;
		totvol = totvol + vol;
	}
	sizex = int(pow((totvol*0.9),0.33333));
	sizey = int(pow((totvol*0.9),0.33333));
	sizez = int(pow((totvol*0.9),0.33333));
	for(int i=0;i<numgrains;i++)
	{
		int maxgrain = 0;
		double maxvol = 0;
		for(int j=0;j<numgrains;j++)
		{
			double vol = tempgrain[j].volume;
			if(vol > maxvol && takencheck[j] != 1)
			{
				maxvol = vol;
				maxgrain = j;
			}
		}
		takencheck[maxgrain] = 1;
		double vol = tempgrain[maxgrain].volume;
		double r1 = tempgrain[maxgrain].axisa;
		double r2 = tempgrain[maxgrain].axisb;
		double r3 = tempgrain[maxgrain].axisc;
		double r1x = tempgrain[maxgrain].rad1x;
		double r1y = tempgrain[maxgrain].rad1y;
		double r1z = tempgrain[maxgrain].rad1z;
		double r2x = tempgrain[maxgrain].rad2x;
		double r2y = tempgrain[maxgrain].rad2y;
		double r2z = tempgrain[maxgrain].rad2z;
		double r3x = tempgrain[maxgrain].rad3x;
		double r3y = tempgrain[maxgrain].rad3y;
		double r3z = tempgrain[maxgrain].rad3z;
		double N = tempgrain[maxgrain].Nvalue;
		grain[i].grainname = i;
		grain[i].volume = vol;
		grain[i].axisa = r1;
		grain[i].axisb = r2;
		grain[i].axisc = r3;
		grain[i].rad1x = r1x;
		grain[i].rad1y = r1y;
		grain[i].rad1z = r1z;
		grain[i].rad2x = r2x;
		grain[i].rad2y = r2y;
		grain[i].rad2z = r2z;
		grain[i].rad3x = r3x;
		grain[i].rad3y = r3y;
		grain[i].rad3z = r3z;
		grain[i].Nvalue = N;
	}
}

void assign_eulers(int numgrains)
{
	int count = 0;
//	double grainsleft = numgrains-count;
	double density = 0;
	double synea1=0,synea2=0,synea3=0;
	for(int j = 0; j < 18; j++)
	{
		for(int k = 0; k < 18; k++)
		{
			for(int l = 0; l < 18; l++)
			{
				double num = (numgrains)*(double(eulercount[j][k][l]))/numeulers;
				density = density + (eulercount[j][k][l]*eulercount[j][k][l]);
		//		double adjust = double(numgrains)/double(numeulers);
				num = num;
				int numint = num;
				for(int m = 0; m < numint; m++)
				{
					double random = rg.Random();
					double random1 = random;
					random = rg.Random();
					double random2 = random;
					random = rg.Random();
					double random3 = random;
					synea1 = ((3.1415926535897/36)*j)+((3.1415926535897/36)*random1);
					synea2 = ((3.1415926535897/36)*k)+((3.1415926535897/36)*random2);
					synea3 = ((3.1415926535897/36)*l)+((3.1415926535897/36)*random3);
					grain[count].euler1 = synea1;
					grain[count].euler2 = synea2;
					grain[count].euler3 = synea3;
					count++;
					if(count == numgrains) {break;}
				}
				if(count == numgrains) {break;}
			}
			if(count == numgrains) {break;}
		}
		if(count == numgrains) {break;}
	}
	double lastcur = 1;
	while(count < numgrains)
	{
		double cur = 0;
		for(int m = 0; m < 18; m++)
		{
			for(int n = 0; n < 18; n++)
			{
				for(int p = 0; p < 18; p++)
				{
					double num = eulercount[m][n][p]/numeulers;
					if(num > cur && num < lastcur)
					{
						cur = num;
						double random = rg.Random();
						double random4 = random;
						random = rg.Random();
						double random5 = random;
						random = rg.Random();
						double random6 = random;
						synea1 = ((3.1415926535897/36)*m)+((3.1415926535897/36)*random4);
						synea2 = ((3.1415926535897/36)*n)+((3.1415926535897/36)*random5);
						synea3 = ((3.1415926535897/36)*p)+((3.1415926535897/36)*random6);
					}
				}
			}
		}
		lastcur = cur;
		synea1 = rg.Random()*2*3.141592;
		synea2 = rg.Random()*1*3.141592;
		synea3 = rg.Random()*2*3.141592;
		grain[count].euler1 = synea1;
		grain[count].euler2 = synea2;
		grain[count].euler3 = synea3;
		count++;
	}
}

void loadSVNData(string inname8)
{
    ifstream inputFile;
    inputFile.open(inname8.c_str());
	double prob;
	int nnum;
    for (int k = 0; k < numsizebins; k++)
    {
		for (int l = 0; l < numneighbins; l++)
		{
			inputFile >> nnum >> prob;
			svn[k][l] = prob;
		}
    }
    inputFile.close();
}

void loadSVSData(string inname9)
{
    ifstream inputFile;
    inputFile.open(inname9.c_str());
    int bin;
	double prob;

    for (int k = 0; k < numsizebins; k++)
    {
		inputFile >> bin;
		for (int l = 0; l < numsizebins; l++)
		{
			inputFile >> prob;
			svs[k][l] = prob;
		}
    }
    inputFile.close();
}

void make_points(int numgrains)
{
	int ncount = 0;
	int count = 0;
	int good = 0;
	int column = 0;
	int row = 0;
	int plane = 0;
	int columncourse = 0;
	int rowcourse = 0;
	int planecourse = 0;
	int tempsurf = 0;
	int	xmin = 0;
	int	xmax = 0;
	int	ymin = 0;
	int	ymax = 0;
	int	zmin = 0;
	int	zmax = 0;
	long j = 0;
	double xc = 0;
	double yc = 0;
	double zc = 0;
	double x = 0;
	double y = 0;
	double z = 0;
	int init = 1;
	int in = 1;
	int out = 0;
	int taken90 = 0;
	long pointsleft = 0;
	double insidecount = 0;
	double badcount = 0;
	long triedcount = 0;
	int uniquecursize = 0;
	int totalcursize = 0;
	int uniquecurcoursesize = 0;
	list<long> alreadytriedlist;
	srand(static_cast<unsigned int>(time(NULL)));
	int xpoints = (sizex/resx);
	int ypoints = (sizey/resy);
	int zpoints = (sizez/resz);
	int xpointscourse = (sizex/resx1);
	int ypointscourse = (sizey/resy1);
	int zpointscourse = (sizez/resz1);
	long nottakencount = (xpoints*ypoints*zpoints);
	long nottakencountcourse = (xpointscourse*ypointscourse*zpointscourse);
	for (long a = 0; a < nottakencountcourse; a++)
	{
		gridcourse[a].available = init;
		gridcourse[a].available90 = init;
	}
	for (long b = 0; b < nottakencount; b++)
	{
		gridfine[b].available = init;
		gridfine[b].available90 = init;
	}
	for (int i = 0; i < numgrains; i++)
	{
		good = 0;
		triedcount = 0;
		while(!alreadytriedlist.empty())
		{
			int firstgridpoint = alreadytriedlist.front();
			gridcourse[firstgridpoint].available = init;
			alreadytriedlist.pop_front();
		}
		double volcur = grain[i].volume;
	//	double aovera = grain[i].axisa;
		double bovera = grain[i].axisb;
		double covera = grain[i].axisc;
	//	double coverb = covera/bovera;
		double Nvalue = grain[i].Nvalue;
		double beta1 = (gamma((1.0/Nvalue))*gamma((1/Nvalue)))/gamma((2/Nvalue));
		double beta2 = (gamma((2.0/Nvalue))*gamma((1/Nvalue)))/gamma((3/Nvalue));
		double radcur1cube = 1;
		if(shapeclass == 3)
		{
			if(Nvalue >= 0 && Nvalue <= 1)
			{
				radcur1cube = (volcur*6.0)/(6-(Nvalue*Nvalue*Nvalue));
			}
			if(Nvalue > 1 && Nvalue <= 2)
			{
				radcur1cube = (volcur*6.0)/(3+(9*Nvalue)-(9*Nvalue*Nvalue)+(2*Nvalue*Nvalue*Nvalue));
			}
		}
		if(shapeclass == 2)
		{
			radcur1cube = (volcur*(3.0/2.0)*(1.0/bovera)*(1.0/covera)*((Nvalue*Nvalue)/4.0)*(1.0/beta1)*(1.0/beta2));
		}
		if(shapeclass == 1)
		{
			radcur1cube = (volcur*(3.0/4.0)*(1.0/3.1415926535897)*(1.0/bovera)*(1.0/covera));
		}
		double rad = pow(radcur1cube,0.333333333333);
		if(shapeclass == 3) rad = rad/2.0;
		double radcur1 = rad;
		double radcur2 = (rad*bovera);
		double radcur3 = (rad*covera);
		double rad1x = grain[i].rad1x;
		double rad1y = grain[i].rad1y;
		double rad1z = grain[i].rad1z;
		double rad2x = grain[i].rad2x;
		double rad2y = grain[i].rad2y;
		double rad2z = grain[i].rad2z;
		double rad3x = grain[i].rad3x;
		double rad3y = grain[i].rad3y;
		double rad3z = grain[i].rad3z;
		while(good == 0)
		{
			double random = rg.Random();
			double random1 = random;
			pointsleft = nottakencountcourse-triedcount;
			int remainder = random1*pointsleft-1;
			int selector = -1;
			int tempcounter = 0;
			while(selector != remainder)
			{
				int picked = gridcourse[tempcounter].available;
				int noway2 = gridcourse[tempcounter].available90;
				if(picked == 1 && noway2 == 1)
				{
					selector++;
					j = tempcounter;
				}
				tempcounter++;
			}
			int alreadypicked = 0;
			int tried = gridcourse[j].available;
			gridcourse[j].available = alreadypicked;
			int noway = gridcourse[j].available90;
			if(noway == 1 && tried == 1)
			{
				for(int c=0;c<i;c++)
				{
					gremovals[c]=0;
				}
				alreadytriedlist.push_back(j);
				good = 1;
				columncourse = (j%xpointscourse);
				rowcourse = ((j/xpointscourse)%ypointscourse);
				planecourse = (j/(xpointscourse*ypointscourse));
				xc = (columncourse*resx1)+(resx1/2);
				yc = (rowcourse*resy1)+(resy1/2);
				zc = (planecourse*resz1)+(resz1/2);
				column = (xc-(resx1/2))/resx;
				row = (yc-(resy1/2))/resy;
				plane = (zc-(resz1/2))/resz;
				xc = (column*resx)+(resx/2);
				yc = (row*resy)+(resy/2);
				zc = (plane*resz)+(resz/2);
				insidecount = 0;
				badcount = 0;
				xmin = 0;
				xmax = (sizex/resx)-1;
				ymin = 0;
				ymax = (sizey/resy)-1;
				zmin = 0;
				zmax = (sizez/resz)-1;
				if(column-((radcur1/resx)+1) > 0)
				{
					xmin = column-((radcur1/resx)+1);
				}
				if(column+((radcur1/resx)+1) < (sizex/resx)-1)
				{
					xmax = column+((radcur1/resx)+1);
				}
				if(row-((radcur1/resy)+1) > 0)
				{
					ymin = row-((radcur1/resy)+1);
				}
				if(row+((radcur1/resy)+1) < (sizey/resy)-1)
				{
					ymax = row+((radcur1/resy)+1);
				}
				if(plane-((radcur1/resz)+1) > 0)
				{
					zmin = plane-((radcur1/resz)+1);
				}
				if(plane+((radcur1/resz)+1) < (sizez/resz)-1)
				{
					zmax = plane+((radcur1/resz)+1);
				}
				for(int iter1 = xmin; iter1 < xmax+1; iter1++)
				{
					for(int iter2 = ymin; iter2 < ymax+1; iter2++)
					{
						for(int iter3 = zmin; iter3 < zmax+1; iter3++)
						{
							double inside = -1;
							column = iter1;
							row = iter2;
							plane = iter3;
							x = (column*resx)+(resx/2);
							y = (row*resy)+(resy/2);
							z = (plane*resz)+(resz/2);
							double axis[3][3];
							double diff[3][1];
							double axiselim[3][3];
							double diffelim[3][1];
							double constmat[3][1];
							axis[0][0] = rad1x;
							axis[0][1] = rad2x;
							axis[0][2] = rad3x;
							axis[1][0] = rad1y;
							axis[1][1] = rad2y;
							axis[1][2] = rad3y;
							axis[2][0] = rad1z;
							axis[2][1] = rad2z;
							axis[2][2] = rad3z;
							diff[0][0] = x-xc;
							diff[1][0] = y-yc;
							diff[2][0] = z-zc;
							int elimcount = 0;
							int elimcount1 = 0;
							double q = 0;
							double sum = 0;
							double c = 0;
							for(int a = 0; a < 3; a++)
							{
								elimcount1 = 0;
								for(int b = 0; b < 3; b++)
								{
									axiselim[elimcount][elimcount1] = axis[a][b];
									if(axiselim[elimcount][elimcount1] == 0)
									{
										axiselim[elimcount][elimcount1] = 0.0001;
									}
									elimcount1++;
								}
								diffelim[elimcount][0] = diff[a][0];
								elimcount++;
							}
							for(int k = 0; k < elimcount-1; k++)
							{
								for(int l = k+1; l < elimcount; l++)
								{
									c = axiselim[l][k]/axiselim[k][k];
									for(int m = k+1; m < elimcount; m++)
									{
										axiselim[l][m] = axiselim[l][m] - c*axiselim[k][m];
									}
									diffelim[l][0] = diffelim[l][0] - c*diffelim[k][0];
								}
							}
							diffelim[elimcount-1][0] = diffelim[elimcount-1][0]/axiselim[elimcount-1][elimcount-1];
							for(int l = 1; l < elimcount; l++)
							{
								int m = (elimcount-1)-l;
								sum = 0;
								for(int n = m+1; n < elimcount; n++)
								{
									sum = sum + (axiselim[m][n]*diffelim[n][0]);
								}
								diffelim[m][0] = (diffelim[m][0]-sum)/axiselim[m][m];
							}
							for(int p = 0; p < elimcount; p++)
							{
								q = diffelim[p][0];
								constmat[p][0] = q;
							}
							double axis1comp = constmat[0][0]/radcur1;
							double axis2comp = constmat[1][0]/radcur2;
							double axis3comp = constmat[2][0]/radcur3;
							if(shapeclass == 3)
							{
								if(fabs(axis1comp) <= 1 && fabs(axis2comp) <= 1 && fabs(axis3comp) <= 1)
								{
									inside = 1;
									axis1comp = axis1comp+1;
									axis2comp = axis2comp+1;
									axis3comp = axis3comp+1;
									if(((-axis1comp)+(-axis2comp)+(axis3comp)-((-0.5*Nvalue)+(-0.5*Nvalue)+2)) > 0) inside = -1;
									if(((axis1comp)+(-axis2comp)+(axis3comp)-((2-(0.5*Nvalue))+(-0.5*Nvalue)+2)) > 0) inside = -1;
									if(((axis1comp)+(axis2comp)+(axis3comp)-((2-(0.5*Nvalue))+(2-(0.5*Nvalue))+2)) > 0) inside = -1;
									if(((-axis1comp)+(axis2comp)+(axis3comp)-((-0.5*Nvalue)+(2-(0.5*Nvalue))+2)) > 0) inside = -1;
									if(((-axis1comp)+(-axis2comp)+(-axis3comp)-((-0.5*Nvalue)+(-0.5*Nvalue))) > 0) inside = -1;
									if(((axis1comp)+(-axis2comp)+(-axis3comp)-((2-(0.5*Nvalue))+(-0.5*Nvalue))) > 0) inside = -1;
									if(((axis1comp)+(axis2comp)+(-axis3comp)-((2-(0.5*Nvalue))+(2-(0.5*Nvalue)))) > 0) inside = -1;
									if(((-axis1comp)+(axis2comp)+(-axis3comp)-((-0.5*Nvalue)+(2-(0.5*Nvalue)))) > 0) inside = -1;
								}
							}
							if(shapeclass == 2)
							{
								axis1comp = pow(axis1comp,Nvalue);
								axis2comp = pow(axis2comp,Nvalue);
								axis3comp = pow(axis3comp,Nvalue);
								inside = 1-axis1comp-axis2comp-axis3comp;
							}
							if(shapeclass == 1)
							{
								axis1comp = pow(axis1comp,2);
								axis2comp = pow(axis2comp,2);
								axis3comp = pow(axis3comp,2);
								inside = 1-axis1comp-axis2comp-axis3comp;
							}
							if(inside >= 0)
							{
								int currentpoint = (xpoints*ypoints*plane)+(xpoints*row)+column;
								int resdiff = double(resx1)/double(resx)+0.00000000001;
								int columnleft = column%resdiff;
								int rowleft = row%resdiff;
								int planeleft = plane%resdiff;
								if(columnleft == (resdiff/2) && rowleft == (resdiff/2) && planeleft == (resdiff/2))
								{
									int columnc = column/resdiff;
									int rowc = row/resdiff;
									int planec = plane/resdiff;
									int currentpointcourse = (xpointscourse*ypointscourse*planec)+(xpointscourse*rowc)+columnc;
									gridcourse[currentpointcourse].inside = in;
								}
								gridfine[currentpoint].inside = in;
								insidecount++;
								int used = gridfine[currentpoint].available90;
								if(used == 0 && overlapassignment == 1) badcount++;
								if(used == 0 && overlapassignment == 2)
								{
									int usedgrain = gridfine[currentpoint].grainname;
									if((i%2) == 0)
									{
										gremovals[usedgrain]++;
									}
									if((i%2) == 1)
									{
										badcount++;
									}
								}
							}
						}
					}
				}
				double uberincrease = 0;
				double nsdistchange = 0;
				for (int n = 0; n < i; n++)
				{
					double xold = packedgrain[n].x;
					double yold = packedgrain[n].y;
					double zold = packedgrain[n].z;
					double radold1 = packedgrain[n].radius1;
					double radold2 = packedgrain[n].radius2;
					double radold3 = packedgrain[n].radius3;
					double xdist = fabs(xc-xold);
					double ydist = fabs(yc-yold);
					double zdist = fabs(zc-zold);
					double totdist = (xdist*xdist)+(ydist*ydist)+(zdist*zdist);
					totdist = pow(totdist,0.5);
					if((totdist < (0.7*(radcur1+radold1))) && (totdist < (0.7*(radcur1+radold1))) && (totdist < (0.7*(radcur1+radold1))))
					{
						int nnum = packedgrain[n].tempneighnum;
						double lognnum = log10(double(nnum));
						if(nnum == 0) lognnum = 0;
						double nsizevol = 1.3333333333*(3.1415926535897)*radold1*radold2*radold3;
						double nsize3 = ((nsizevol*(0.75))/(3.1415926535897));
						double nsize = 2*(pow(nsize3,0.33333333333));
						double sizevol = 1.3333333333*(3.1415926535897)*radcur1*radcur2*radcur3;
						double size3 = ((sizevol*(0.75))/(3.1415926535897));
						double size = 2*(pow(size3,0.33333333333));
						int nbin = nsize;
						int bin = size;
						if(nbin >= numsizebins) nbin = numsizebins-1;
						if(bin >= numsizebins) bin = numsizebins-1;
						int was = nnum;
						nnum = nnum + 1;
						int now = nnum;
						double wasprob = svn[nbin][was];
						double nowprob = svn[nbin][now];
						double increase = (nowprob - wasprob);
						if(increase > 0) increase = 1;
						if(increase < 0) increase = -1;
						uberincrease = uberincrease + increase;
						nsdist[n][bin]++;
						int total = 0;
						for(int q = 0; q < numsizebins; q++)
						{
							total = total + nsdist[n][q];
						}
						double error = 0;
						for(int r = 0; r < numsizebins; r++)
						{
							error = error + (svs[nbin][r] - (nsdist[n][r]/total))*(svs[nbin][r] - (nsdist[n][r]/total));
						}
						double nserror = packedgrain[n].nserror;
						nsdistchange = nsdistchange + (error-nserror);
						nsdist[n][bin] = nsdist[n][bin] - 1;
					}
				}
			    double randomsize = rg.Random();
			//	double randomsize1 = randomsize;
				double acceptable = badcount/insidecount;
//				if(insidecount == 0)
//				{
//					int stop = 0;
//				}
				int toomuch = 0;
				if(overlapassignment == 2)
				{
					for(int d=0;d<i;d++)
					{
						double ratio = double(packedgrain[d].currentsize-gremovals[d])/double(packedgrain[d].initsize);
						if(ratio < (1-(overlapallowed/2.0))) toomuch = 1;
					}
				}
				if(acceptable > overlapallowed || uberincrease < 0 || nsdistchange > 0 || toomuch == 1) good = 0;
				triedcount++;
			}
			if(good == 0)
			{
				for(int temp1 = xmin; temp1 < xmax+1; temp1++)
				{
					for(int temp2 = ymin; temp2 < ymax+1; temp2++)
					{
						for(int temp3 = zmin; temp3 < zmax+1; temp3++)
						{
							int point = (temp1+(temp2*xpoints)+(temp3*xpoints*ypoints));
							int inside = gridfine[point].inside;
							if(inside == 1)
							{
								gridfine[point].inside = out;
							}
						}
					}
				}
				for(int temp4 = xmin; temp4 < xmax+1; temp4++)
				{
					for(int temp5 = ymin; temp5 < ymax+1; temp5++)
					{
						for(int temp6 = zmin; temp6 < zmax+1; temp6++)
						{
							int column = temp4;
							int row = temp5;
							int plane = temp6;
							int resdiff = double(resx1)/double(resx)+0.00000000001;
							column = column/resdiff;
							row = row/resdiff;
							plane = plane/resdiff;
							int point = (column+(row*xpointscourse)+(plane*xpointscourse*ypointscourse));
							int inside = gridcourse[point].inside;
							if(inside == 1)
							{
								gridcourse[point].inside = out;
							}
						}
					}
				}
			}
		}
		ncount = 0;
		for (int p = 0; p < i; p++)
		{
			double xold = packedgrain[p].x;
			double yold = packedgrain[p].y;
			double zold = packedgrain[p].z;
			double radold1 = packedgrain[p].radius1;
			double radold2 = packedgrain[p].radius2;
			double radold3 = packedgrain[p].radius3;
			double xdist = fabs(xc-xold);
			double ydist = fabs(yc-yold);
			double zdist = fabs(zc-zold);
			if((xdist < (0.7*(radcur1+radold1))) && (ydist < (0.7*(radcur1+radold1))) && (zdist < (0.7*(radcur1+radold1))))
			{
				ncount++;
				int nnum = packedgrain[p].tempneighnum;
				nnum = nnum + 1;
				packedgrain[p].tempneighnum = nnum;
				double volume = 1.3333333333*(3.1415926535897)*radcur1*radcur2*radcur3;
				double volbin = volume*0.75/3.1415926535897;
				int vbin = 2*pow(volbin,0.3333333333);
				if(vbin >= numsizebins) vbin = numsizebins-1;
				nsdist[p][vbin]++;
				double nvolume = 1.3333333333*(3.1415926535897)*radold1*radold2*radold3;
				double nvolbin = nvolume*0.75/3.1415926535897;
				int nvbin = 2*pow(nvolbin,0.3333333333);
				if(nvbin >= numsizebins) nvbin = numsizebins-1;
				int total = 0;
				for(int q = 0; q < numsizebins; q++)
				{
					total = total + nsdist[p][q];
				}
				double error = 0;
				for(int r = 0; r < numsizebins; r++)
				{
					error = error + (svs[nvbin][r] - (nsdist[p][r]/total))*(svs[nvbin][r] - (nsdist[p][r]/total));
				}
				packedgrain[p].nserror = error;
			}
		}
		tempsurf = 0;
		for(int temp7 = xmin; temp7 < xmax+1; temp7++)
		{
			for(int temp8 = ymin; temp8 < ymax+1; temp8++)
			{
				for(int temp9 = zmin; temp9 < zmax+1; temp9++)
				{
					int point = (temp7+(temp8*xpoints)+(temp9*xpoints*ypoints));
					int inside = gridfine[point].inside;
					if(inside == 1)
					{
						int columncheck = point%xpoints;
						int rowcheck = (point/xpoints)%ypoints;
						int planecheck = point/(xpoints*ypoints);
						if(columncheck == 0 || rowcheck == 0 || planecheck == 0 || columncheck == (xpoints-1) || rowcheck == (ypoints-1) || planecheck == (zpoints-1)) tempsurf = 1;
						int current = gridfine[point].available90;
						if(current == 1)
						{
							uniquecursize++;
							totalcursize++;
							gridfine[point].grainname = i;
							voxelvector[i].push_back(point);
						}
						if(current == 0 && overlapassignment == 2)
						{
							int checkgrain = gridfine[point].grainname;
							if((gremovals[checkgrain]/packedgrain[checkgrain].initsize) < (overlapallowed/5.0))
							{
								gridfine[point].grainname = i;
								voxelvector[i].push_back(point);
								totalcursize++;
							}
						}
						gridfine[point].inside = out;
						gridfine[point].available90 = taken90;
					}
				}
			}
		}
		for(int temp10 = xmin; temp10 < xmax+1; temp10++)
		{
			for(int temp11 = ymin; temp11 < ymax+1; temp11++)
			{
				for(int temp12 = zmin; temp12 < zmax+1; temp12++)
				{
					int column = temp10;
					int row = temp11;
					int plane = temp12;
					int resdiff = double(resx1)/double(resx)+0.00000000001;
					column = column/resdiff;
					row = row/resdiff;
					plane = plane/resdiff;
					int point = (column+(row*xpointscourse)+(plane*xpointscourse*ypointscourse));
					int inside = gridcourse[point].inside;
					if(inside == 1)
					{
						int currentcourse = gridcourse[point].available90;
						if(currentcourse == 1)
						{
							uniquecurcoursesize++;
						}
						gridcourse[point].available90 = taken90;
					}
				}
			}
		}
		if(overlapassignment == 2)
		{
			for(int e=0;e<i;e++)
			{
				if((gremovals[e]/packedgrain[e].initsize) < (overlapallowed/5.0))
				{
					int newsize = packedgrain[e].currentsize-gremovals[e];
					packedgrain[e].currentsize = newsize;
				}
			}
		}
		nottakencountcourse = nottakencountcourse - uniquecurcoursesize;
		nottakencount = nottakencount - uniquecursize;
		uniquecursize = 0;
		uniquecurcoursesize = 0;
		int pointcolumn = j%xpointscourse;
		int pointrow = (j/xpointscourse)%ypointscourse;
		int pointplane = j/(xpointscourse*ypointscourse);
		double pointx = (pointcolumn*resx1)+(resx1/2);
		double pointy = (pointrow*resy1)+(resy1/2);
		double pointz = (pointplane*resz1)+(resz1/2);
		double ea1 = grain[i].euler1;
		double ea2 = grain[i].euler2;
		double ea3 = grain[i].euler3;
		packedgrain[i].initsize = totalcursize;
		packedgrain[i].currentsize = totalcursize;
		packedgrain[i].grainname = i;
		packedgrain[i].xc = pointx;
		packedgrain[i].yc = pointy;
		packedgrain[i].zc = pointz;
		packedgrain[i].axis1 = radcur1;
		packedgrain[i].axis2 = radcur2;
		packedgrain[i].axis3 = radcur3;
		packedgrain[i].temponsurf = tempsurf;
		packedgrain[i].euler1 = ea1;
		packedgrain[i].euler2 = ea2;
		packedgrain[i].euler3 = ea3;
		packedgrain[i].tempneighnum = ncount;
		totalcursize = 0;
		cout << i << "-" << pointx << "-" << pointy << "-" << pointz << "-" << radcur1 << "-" << radcur2 << "-" << radcur3 << endl;
		count++;
	}
	for(int u=0;u<numgrains;u++)
	{
		gsizes[u] = 0;
	}
	for(int t=0;t<(xpoints*ypoints*zpoints);t++)
	{
		int available = gridfine[t].available90;
		if(available == 0)
		{
			int gname = gridfine[t].grainname;
			gsizes[gname]++;
		}
	}
	for(int v=0;v<numgrains;v++)
	{
		int cursize = gsizes[v];
		packedgrain[v].currentsize = cursize;
	}
}

void fill_gaps(int numgrains)
{
	int count = 1;
	//int dup = 0;
	int fixed = 0;
	list<int> neighs;
	list<int> remove;
	int xpoints = (sizex/resx);
	int ypoints = (sizey/resy);
	int zpoints = (sizez/resz);
	int column = 0;
	int row = 0;
	int plane = 0;
	int *n;
	n = new int[numgrains];
	while(count != 0)
	{
		count = 0;
		for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
		{
			int taken = gridfine[i].available90;
			if(taken == 1)
			{
				for(int c = 0; c < numgrains; c++)
				{
					n[c] = 0;
				}
				column = i%xpoints;
				row = (i/xpoints)%ypoints;
				plane = i/(xpoints*ypoints);
				count++;
				if(column > 0)
				{
					int grain1 = gridfine[i-1].grainname;
					int taken1 = gridfine[i-1].available90;
					if(taken1 == 0)
					{
						neighs.push_back(grain1);
					}
				}
				if(column < (sizex/resx)-1)
				{
					int grain2 = gridfine[i+1].grainname;
					int taken2 = gridfine[i+1].available90;
					if(taken2 == 0)
					{
						neighs.push_back(grain2);
					}
				}
				if(row > 0)
				{
					int grain3 = gridfine[i-(xpoints)].grainname;
					int taken3 = gridfine[i-(xpoints)].available90;
					if(taken3 == 0)
					{
						neighs.push_back(grain3);
					}
				}
				if(row < (sizey/resy)-1)
				{
					int grain4 = gridfine[i+(xpoints)].grainname;
					int taken4 = gridfine[i+(xpoints)].available90;
					if(taken4 == 0)
					{
						neighs.push_back(grain4);
					}
				}
				if(plane > 0)
				{
					int grain5 = gridfine[i-(xpoints*ypoints)].grainname;
					int taken5 = gridfine[i-(xpoints*ypoints)].available90;
					if(taken5 == 0)
					{
						neighs.push_back(grain5);
					}
				}
				if(plane < (sizez/resz)-1)
				{
					int grain6 = gridfine[i+(xpoints*ypoints)].grainname;
					int taken6 = gridfine[i+(xpoints*ypoints)].available90;
					if(taken6 == 0)
					{
						neighs.push_back(grain6);
					}
				}
				int now = 0;
				int then = 0;
				double thendiff = 0;
				double nowdiff = 0;
				double nowsizegoal = 0;
				double nowsize = 0;
				int nowtemponsurf = 0;
				int curgrain = 0;
				int size = neighs.size();
				while(!neighs.empty())
				{
					int first = neighs.front();
					n[first]++;
					now = n[first];
					nowsize = packedgrain[first].currentsize;
					nowsizegoal = grain[first].volume;
					nowtemponsurf = packedgrain[first].temponsurf;
					nowdiff = nowsizegoal-nowsize;
					if(now > then)
//					if(now > then || (now == then && nowdiff > thendiff))
					{
						then = now;
						thendiff = nowdiff;
						curgrain = first;
					}
					neighs.pop_front();
				}
				if(size > 0)
				{
					int bordered = 1;
					gridfine[i].hasneigh = bordered;
					gridfine[i].neighbor = curgrain;
					neighs.clear();
				}
			}
		}
		for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
		{
			int taken = gridfine[j].available90;
			int hasneigh = gridfine[j].hasneigh;
			if(taken == 1 && hasneigh == 1)
			{
				int neighbor = gridfine[j].neighbor;
				gridfine[j].grainname = neighbor;
				gridfine[j].available90 = fixed;
			}
		}
	}
}


void find_neighbors(int numgrains)
{
	int count = 0;
//	int dup = 0;
	int column = 0;
	int row = 0;
	int plane = 0;
//	int shell = 0;
	int onsurf = 0;
	list<int> nlist;
	list<int> nlist2;
	list<int> nlistgood;
	int neighcount = 0;
	int xpoints = (sizex/resx);
	int ypoints = (sizey/resy);
	int zpoints = (sizez/resz);
	for(int i = 0; i < numgrains; i++)
	{
		int size = voxelvector[i].size();
		for(int j = 0; j < size; j++)
		{
			onsurf = 0;
			int point = voxelvector[i][j];
			column = point%xpoints;
			row = (point/xpoints)%ypoints;
			plane = point/(xpoints*ypoints);
		//	int first = gridfine[point].grainname;
			if(column == 0 || column == (xpoints-1) || row == 0 || row == (ypoints-1) || plane == 0 || plane == (zpoints-1)) onsurf = 1;
			if(column > 0)
			{
				int grain1 = gridfine[point-1].grainname;
				if(grain1 != i)
				{
					onsurf = 1;
					neighborvector[i].push_back(grain1);
					gridfine[point].neighbor1 = grain1;
				}
			}
			if(column < (sizex/resx)-1)
			{
				int grain2 = gridfine[point+1].grainname;
				if(grain2 != i)
				{
					onsurf = 1;
					neighborvector[i].push_back(grain2);
					gridfine[point].neighbor2 = grain2;
				}
			}
			if(row > 0)
			{
				int grain3 = gridfine[point-(xpoints)].grainname;
				if(grain3 != i)
				{
					onsurf = 1;
					neighborvector[i].push_back(grain3);
					gridfine[point].neighbor3 = grain3;
				}
			}
			if(row < (sizey/resy)-1)
			{
				int grain4 = gridfine[point+(xpoints)].grainname;
				if(grain4 != i)
				{
					onsurf = 1;
					neighborvector[i].push_back(grain4);
					gridfine[point].neighbor4 = grain4;
				}
			}
			if(plane > 0)
			{
				int grain5 = gridfine[point-(xpoints*ypoints)].grainname;
				if(grain5 != i)
				{
					onsurf = 1;
					neighborvector[i].push_back(grain5);
					gridfine[point].neighbor5 = grain5;
				}
			}
			if(plane < (sizez/resz)-1)
			{
				int grain6 = gridfine[point+(xpoints*ypoints)].grainname;
				if(grain6 != i)
				{
					onsurf = 1;
					neighborvector[i].push_back(grain6);
					gridfine[point].neighbor6 = grain6;
				}
			}
			if(onsurf == 1)
			{
				gridfine[point].boundarypoint = onsurf;
			}
		}
		vector<int>::iterator newend;
		sort(neighborvector[i].begin(),neighborvector[i].end());
		newend = unique(neighborvector[i].begin(),neighborvector[i].end());
		neighborvector[i].erase(newend,neighborvector[i].end());
		neighcount = neighborvector[i].size();
		packedgrain[i].neighnum = neighcount;
		count++;
	}
}

void find_centroids(int numgrains)
{
//	int size = 0;
	int count = 0;
	int onedge = 0;
	double sumx = 0;
	double sumy = 0;
	double sumz = 0;
	double centerx = 0;
	double centery = 0;
	double centerz = 0;
	int xpoints = (sizex/resx);
	int ypoints = (sizey/resy);
	int zpoints = (sizez/resz);
	for(int i = 0; i < numgrains; i++)
	{
		count = 0;
		sumx = 0;
		sumy = 0;
		sumz = 0;
		onedge = 0;
		int size = voxelvector[i].size();
		for(int j = 0; j < size; j++)
		{
			int point = voxelvector[i][j];
			int column = point%xpoints;
			int row = (point/xpoints)%ypoints;
			int plane = point/(xpoints*ypoints);
			if(column == 0 || column == (xpoints-1) || row == 0 || row == (ypoints-1) || plane == 0 || plane == (zpoints-1)) onedge = 1;
			double x = (column*resx)+(resx/2);
			double y = (row*resy)+(resy/2);
			double z = (plane*resz)+(resz/2);
			sumx = sumx + x;
			sumy = sumy + y;
			sumz = sumz + z;
			count++;
		}
		centerx = sumx/count;
		centery = sumy/count;
		centerz = sumz/count;
		packedgrain[i].edgegrain = onedge;
		packedgrain[i].xc = centerx;
		packedgrain[i].yc = centery;
		packedgrain[i].zc = centerz;
		packedgrain[i].vol = count;
	}
}

void find_moments(int numgrains)
{
	int count = 0;
	double sumxx = 0;
	double sumyy = 0;
	double sumzz = 0;
	double sumxy = 0;
	double sumxz = 0;
	double sumyz = 0;
	int xpoints = (sizex/resx);
	int ypoints = (sizey/resy);
	//int zpoints = (sizez/resz);
	for(int i = 0; i < numgrains; i++)
	{
		sumxx = 0;
		sumyy = 0;
		sumzz = 0;
		sumxy = 0;
		sumxz = 0;
		sumyz = 0;
		double xc = packedgrain[i].xc;
		double yc = packedgrain[i].yc;
		double zc = packedgrain[i].zc;
		int size = voxelvector[i].size();
		for(int j = 0; j < size; j++)
		{
			int point = voxelvector[i][j];
			int column = point%xpoints;
			int row = (point/xpoints)%ypoints;
			int plane = point/(xpoints*ypoints);
			double x = (column*resx)+(resx/2);
			double y = (row*resy)+(resy/2);
			double z = (plane*resz)+(resz/2);
			double x1 = x+(resx/2);
			double x2 = x-(resx/2);
			double y1 = y+(resy/2);
			double y2 = y-(resy/2);
			double z1 = z+(resz/2);
			double z2 = z-(resz/2);
			double xdist1 = (x1-xc);
			double ydist1 = (y1-yc);
			double zdist1 = (z1-zc);
			double xdist2 = (x1-xc);
			double ydist2 = (y1-yc);
			double zdist2 = (z2-zc);
			double xdist3 = (x1-xc);
			double ydist3 = (y2-yc);
			double zdist3 = (z1-zc);
			double xdist4 = (x1-xc);
			double ydist4 = (y2-yc);
			double zdist4 = (z2-zc);
			double xdist5 = (x2-xc);
			double ydist5 = (y1-yc);
			double zdist5 = (z1-zc);
			double xdist6 = (x2-xc);
			double ydist6 = (y1-yc);
			double zdist6 = (z2-zc);
			double xdist7 = (x2-xc);
			double ydist7 = (y2-yc);
			double zdist7 = (z1-zc);
			double xdist8 = (x2-xc);
			double ydist8 = (y2-yc);
			double zdist8 = (z2-zc);
			sumxx = sumxx + ((ydist1)*(ydist1))+((zdist1)*(zdist1)) + ((ydist2)*(ydist2))+((zdist2)*(zdist2)) + ((ydist3)*(ydist3))+((zdist3)*(zdist3)) + ((ydist4)*(ydist4))+((zdist4)*(zdist4)) + ((ydist5)*(ydist5))+((zdist5)*(zdist5)) + ((ydist6)*(ydist6))+((zdist6)*(zdist6)) + ((ydist7)*(ydist7))+((zdist7)*(zdist7)) + ((ydist8)*(ydist8))+((zdist8)*(zdist8));
			sumyy = sumyy + ((xdist1)*(xdist1))+((zdist1)*(zdist1)) + ((xdist2)*(xdist2))+((zdist2)*(zdist2)) + ((xdist3)*(xdist3))+((zdist3)*(zdist3)) + ((xdist4)*(xdist4))+((zdist4)*(zdist4)) + ((xdist5)*(xdist5))+((zdist5)*(zdist5)) + ((xdist6)*(xdist6))+((zdist6)*(zdist6)) + ((xdist7)*(xdist7))+((zdist7)*(zdist7)) + ((xdist8)*(xdist8))+((zdist8)*(zdist8));
			sumzz = sumzz + ((xdist1)*(xdist1))+((ydist1)*(ydist1)) + ((xdist2)*(xdist2))+((ydist2)*(ydist2)) + ((xdist3)*(xdist3))+((ydist3)*(ydist3)) + ((xdist4)*(xdist4))+((ydist4)*(ydist4)) + ((xdist5)*(xdist5))+((ydist5)*(ydist5)) + ((xdist6)*(xdist6))+((ydist6)*(ydist6)) + ((xdist7)*(xdist7))+((ydist7)*(ydist7)) + ((xdist8)*(xdist8))+((ydist8)*(ydist8));
			sumxy = sumxy + ((xdist1)*(ydist1)) + ((xdist2)*(ydist2)) + ((xdist3)*(ydist3)) + ((xdist4)*(ydist4)) + ((xdist5)*(ydist5)) + ((xdist6)*(ydist6)) + ((xdist7)*(ydist7)) + ((xdist8)*(ydist8));
			sumyz = sumyz + ((ydist1)*(zdist1)) + ((ydist2)*(zdist2)) + ((ydist3)*(zdist3)) + ((ydist4)*(zdist4)) + ((ydist5)*(zdist5)) + ((ydist6)*(zdist6)) + ((ydist7)*(zdist7)) + ((ydist8)*(zdist8));
			sumxz = sumxz + ((xdist1)*(zdist1)) + ((xdist2)*(zdist2)) + ((xdist3)*(zdist3)) + ((xdist4)*(zdist4)) + ((xdist5)*(zdist5)) + ((xdist6)*(zdist6)) + ((xdist7)*(zdist7)) + ((xdist8)*(zdist8));
		}
		sumxx = sumxx*(resx/2)*(resy/2)*(resz/2);
		sumyy = sumyy*(resx/2)*(resy/2)*(resz/2);
		sumzz = sumzz*(resx/2)*(resy/2)*(resz/2);
		sumxy = sumxy*(resx/2)*(resy/2)*(resz/2);
		sumyz = sumyz*(resx/2)*(resy/2)*(resz/2);
		sumxz = sumxz*(resx/2)*(resy/2)*(resz/2);
		packedgrain[i].Ixx = sumxx;
		packedgrain[i].Iyy = sumyy;
		packedgrain[i].Izz = sumzz;
		packedgrain[i].Ixy = -sumxy;
		packedgrain[i].Ixz = -sumxz;
		packedgrain[i].Iyz = -sumyz;
		count++;
	}
}

void find_axis(int numgrains)
{
	int count = 0;
	for (int i = 0; i < numgrains; i++)
	{
		double Ixx = packedgrain[i].Ixx;
		double Iyy = packedgrain[i].Iyy;
		double Izz = packedgrain[i].Izz;
		double Ixy = packedgrain[i].Ixy;
		double Iyz = packedgrain[i].Iyz;
		double Ixz = packedgrain[i].Ixz;
		double a = 1;
		double b = -Ixx-Iyy-Izz;
		double c = ((Ixx*Izz)+(Ixx*Iyy)+(Iyy*Izz)-(Ixz*Ixz)-(Ixy*Ixy)-(Iyz*Iyz));
		double d = ((Ixz*Iyy*Ixz)+(Ixy*Izz*Ixy)+(Iyz*Ixx*Iyz)-(Ixx*Iyy*Izz)-(Ixy*Iyz*Ixz)-(Ixy*Iyz*Ixz));
		double f = ((3*c/a)-((b/a)*(b/a)))/3;
		double g = ((2*(b/a)*(b/a)*(b/a))-(9*b*c/(a*a))+(27*(d/a)))/27;
		double h = (g*g/4)+(f*f*f/27);
		double rsquare = (g*g/4)-h;
		double r = pow(rsquare,0.5);
		double theta = 0;
		if(r == 0)
		{
			theta = 0;
		}
		if(r != 0)
		{
			if((-g/(2*r)) < -1) r = (g/2);
			if((-g/(2*r)) > 1) r = (-g/2);
			theta = acos(-g/(2*r));
		}
//		if(theta < 0)
//		{
//			int stop = 0;
//		}
		double r1 = 2*pow(r,0.33333333333)*cos(theta/3)-(b/(3*a));
		double r2 = -pow(r,0.33333333333)*(cos(theta/3)-(1.7320508*sin(theta/3)))-(b/(3*a));
		double r3 = -pow(r,0.33333333333)*(cos(theta/3)+(1.7320508*sin(theta/3)))-(b/(3*a));
		packedgrain[i].axis1 = r1;
		packedgrain[i].axis2 = r2;
		packedgrain[i].axis3 = r3;
	//	double test = cos(theta/3);
		count++;
	}
}

void find_colors(int numgrains)
{
	for(int i = 0; i < numgrains; i++)
	{
		double g1ea1 = packedgrain[i].euler1;
		double g1ea2 = packedgrain[i].euler2;
		double g1ea3 = packedgrain[i].euler3;
		double go[3][3];
		double cd[3][1];
		double sd[3][1];
		go[0][0] = cos(g1ea1)*cos(g1ea3)-sin(g1ea1)*sin(g1ea3)*cos(g1ea2);
		go[0][1] = sin(g1ea1)*cos(g1ea3)+cos(g1ea1)*sin(g1ea3)*cos(g1ea2);
		go[0][2] = sin(g1ea3)*sin(g1ea2);
		go[1][0] = -cos(g1ea1)*sin(g1ea3)-sin(g1ea1)*cos(g1ea3)*cos(g1ea2);
		go[1][1] = -sin(g1ea1)*sin(g1ea3)+cos(g1ea1)*cos(g1ea3)*cos(g1ea2);
		go[1][2] =  cos(g1ea3)*sin(g1ea2);
		go[2][0] =  sin(g1ea1)*sin(g1ea2);
		go[2][1] = -cos(g1ea1)*sin(g1ea2);
		go[2][2] =  cos(g1ea2);
		cd[0][0] = go[0][0]*0+go[0][1]*0+go[0][2]*1;
		cd[1][0] = go[1][0]*0+go[1][1]*0+go[1][2]*1;
		cd[2][0] = go[2][0]*0+go[2][1]*0+go[2][2]*1;
		sd[0][0] = fabs(cd[0][0]/cd[2][0]);
		sd[1][0] = fabs(cd[1][0]/cd[2][0]);
		sd[2][0] = fabs(cd[2][0]/cd[2][0]);
		double smallest = 100000;
		double biggest = 100000;
		double middle = 100000;
		int place = 0;
		for(int j = 0; j < 3; j++)
		{
			place = 0;
			for(int k = 0; k < 3; k++)
			{
				if(sd[j][0] > sd[k][0]) place++;
				if((sd[j][0] == sd[k][0]) && (j != k) && (j < k)) place++;
			}
			if(place == 0) smallest = sd[j][0];
			if(place == 1) middle = sd[j][0];
			if(place == 2) biggest = sd[j][0];
		}
		double b = smallest;
		double g = middle-b;
		double r = biggest-g-b;
		double cur = r;
		if(g > cur) cur = g;
		if(b > cur) cur = b;
		r = (0.75*(r/cur)+0.25);
		g = (0.75*(g/cur)+0.25);
		b = (0.75*(b/cur)+0.25);
		packedgrain[i].red = r;
		packedgrain[i].green = g;
		packedgrain[i].blue = b;
	}
}

void loadMisoData(string inname10)
{
    ifstream inputFile;
    inputFile.open(inname10.c_str());
	int count = 0;
	float height = 0;
    for (int k = 0; k < nummisobins; k++)
    {
		inputFile >> height;
		actualmisobin[count].height = height;
        count++;
    }
    inputFile.close();
}

void loadMicroData(string inname11)
{
    ifstream inputFile;
    inputFile.open(inname11.c_str());
	int count = 0;
	float height = 0;
    for (int k = 0; k < nummicrobins; k++)
    {
		inputFile >> height;
		actualmicrobin[count].height = height;
        count++;
    }
    inputFile.close();
}

void measure_misorientations (int numgrains)
{
	int count = 0;
	list<float> misolist;
	double n1;
	double n2;
	double n3;
	for (int i = 0; i < numgrains; i++)
	{
		double g1ea1 = packedgrain[i].euler1;
		double g1ea2 = packedgrain[i].euler2;
		double g1ea3 = packedgrain[i].euler3;
		int size = neighborvector[i].size();
		for(int j = 0; j < size; j++)
		{
			int nname = neighborvector[i][j];
			double g2ea1 = packedgrain[nname].euler1;
			double g2ea2 = packedgrain[nname].euler2;
			double g2ea3 = packedgrain[nname].euler3;
			double angcur = GetMisorientationOnly(crystruct,g1ea1,g1ea2,g1ea3,g2ea1,g2ea2,g2ea3,n1,n2,n3);
			angcur = 180.0*angcur/3.1415926535897;
			misolist.push_back(static_cast<float>(angcur));
		}
		packedgrain[i].misorientationlist = misolist;
		count++;
		misolist.clear();
	}
}

void rank_misobins(int numgrains)
{
	int count = 0;
	int check = 0;
	for(int h = 0; h < nummisobins; h++)
	{
		int tempheight = 0;
		simmisobin[h].height = tempheight;
	}
	for(int i = 0; i < numgrains; i++)
	{
		list<float> misolist = packedgrain[i].misorientationlist;
		while(!misolist.empty())
		{
			float misofirst = misolist.front();
			int misocur = misofirst/(100/nummisobins);
			float height = simmisobin[misocur].height;
			height = height + 1;
			simmisobin[misocur].height = height;
			misolist.pop_front();
			count++;
		}
	}
	for(int j = 0; j < nummisobins; j ++)
	{
		float actualheight = actualmisobin[j].height;
		actualheight = actualheight*count;
		float simheight = simmisobin[j].height;
		float diff = fabs(simheight-actualheight);
		simmisobin[j].difference = diff;
		float temp = 0;
		simmisobin[j].binrank = temp;
	}
//	int difflast = count;
	int curbin = 0;
	while(check < nummisobins)
	{
		int diffcur = 0;
		for(int k = 0; k < nummisobins; k++)
		{
			float diff = simmisobin[k].difference;
			float ranked = simmisobin[k].binrank;
			if(diff > diffcur && ranked == 0)
			{
				diffcur = diff;
				curbin = k;
			}
		}
		float rank = nummisobins-check;
		float actual = actualmisobin[curbin].height;
		actual = actual*count;
		float sim = simmisobin[curbin].height;
		if((sim-actual) < 0)
		{
			rank = -rank;
		}
		simmisobin[curbin].binrank = rank;
		check++;
	}
}

void count_misorientations(int numgrains)
{
	int count = 0;
	float lowangle = 0;
	for(int i = 0; i < numgrains; i++)
	{
		lowangle = 0;
		list<float> misolist = packedgrain[i].misorientationlist;
		float size = misolist.size();
		while(!misolist.empty())
		{
			float misofirst = misolist.front();
			if(misofirst < 15) lowangle++;
			misolist.pop_front();
		}
		float fraction = lowangle/size;
		packedgrain[i].lowanglefraction = fraction;
		count++;
	}
}

int rank_microbins(int numgrains)
{
	int count = 0;
	int check = 0;
	for(int h = 0; h < nummicrobins; h++)
	{
		int tempheight = 0;
		simmicrobin[h].height = tempheight;
	}
	for(int i = 0; i < numgrains; i++)
	{
		double microtexture = packedgrain[i].lowanglefraction;
		int microcur = (microtexture*(nummicrobins));
		float height = simmicrobin[microcur].height;
		height = height + 1;
		simmicrobin[microcur].height = height;
	}
	for(int j = 0; j < nummicrobins; j ++)
	{
		float actualheight = actualmicrobin[j].height;
		actualheight = actualheight*numgrains;
		float simheight = simmicrobin[j].height;
		float diff = fabs(simheight-actualheight);
		simmicrobin[j].difference = diff;
		float temp = 0;
		simmicrobin[j].binrank = temp;
	}
	int curbin = 0;
	while(check < nummicrobins)
	{
		int diffcur = 0;
		for(int k = 0; k < nummicrobins; k++)
		{
			float diff = simmicrobin[k].difference;
			float ranked = simmicrobin[k].binrank;
			if(diff > diffcur && ranked == 0)
			{
				diffcur = diff;
				curbin = k;
			}
		}
		float rank = nummicrobins-check;
		float actual = actualmicrobin[curbin].height;
		actual = actual*numgrains;
		float sim = simmicrobin[curbin].height;
		if((sim-actual) < 0)
		{
			rank = -rank;
		}
		simmicrobin[curbin].binrank = rank;
		check++;
	}
	float heightcheck = simmicrobin[nummicrobins-1].height;
	float heightcheck2temp = actualmicrobin[nummicrobins-1].height;
	float heightcheck2 = heightcheck2temp*numgrains;
	float percenterror = (heightcheck2-heightcheck)/heightcheck2;
	if(percenterror < 0.15)
	{
		count = 1;
	}
return count;
}

void rank_grains1(int numgrains)
{
	int count = 0;
	for(int i = 0; i < numgrains; i++)
	{
		float rank = 0;
		float temprank = 0;
		list<float> misolist = packedgrain[i].misorientationlist;
		while(!misolist.empty())
		{
			float misofirst = misolist.front();
			int misocur = misofirst/(100/nummisobins);
			temprank = simmisobin[misocur].binrank;
			rank = rank + temprank;
			if(misofirst == 0)
			{
				rank = rank + 100;
			}
			misolist.pop_front();
		}
//		float frozen = packedgrain[i].frozen;
		packedgrain[i].grainrank = rank;
		count++;
	}
}

void identify_grains1(int numgrains,int nummisomoves)
{
	int check = 0;
	int worst = 0;
	int clear = 0;
	float bad = 1;
	int count = 0;
	for(int h = 0; h < numgrains; h++)
	{
		packedgrain[h].picked = clear;
	}
	while(check < nummisomoves)
	{
		float rankcur = -10000000;
		count = 0;
		for(int i = 0; i < numgrains; i++)
		{
			double rank = packedgrain[i].grainrank;
			double picked = packedgrain[i].picked;
			double froze = packedgrain[i].frozen;
			if(rank > rankcur && picked == 0 && froze == 0)
			{
				rankcur = rank;
				worst = i;
				count++;
			}
		}
		packedgrain[worst].picked = bad;
		check++;
	}
}

void move_grains1(int numgrains)
{
	int check = 0;
	list<int> pickedlist;
	list<int> temppickedlist;
	for(int i = 0; i < numgrains; i++)
	{
		double ea1temp = packedgrain[i].euler1;
		double ea2temp = packedgrain[i].euler2;
		double ea3temp = packedgrain[i].euler3;
		packedgrain[i].oeuler1 = ea1temp;
		packedgrain[i].oeuler2 = ea2temp;
		packedgrain[i].oeuler3 = ea3temp;
		double picked = packedgrain[i].picked;
		if(picked == 1)
		{
			pickedlist.push_back(i);
			check++;
		}
	}
	for(int j = 0; j < numgrains; j++)
	{
		float picked = packedgrain[j].picked;
		if(picked == 1)
		{
			int size = pickedlist.size();
			double random = rg.Random();
			double random1 = random;
			int remainder = random1*size;
			if(size <= remainder)
			{
				remainder = remainder - 1;
			}
			for(int k = 0; k < remainder; k++)
			{
				int pickedfront = pickedlist.front();
				temppickedlist.push_back(pickedfront);
				pickedlist.pop_front();
			}
			int swap = pickedlist.front();
			int sizecheck = pickedlist.size();
			if(swap == j && sizecheck > 1)
			{
				temppickedlist.push_back(swap);
				pickedlist.pop_front();
				swap = pickedlist.front();
			}
			pickedlist.pop_front();
	//		int grainname = packedgrain[j].grainname;
			double ea1 = packedgrain[swap].oeuler1;
			double ea2 = packedgrain[swap].oeuler2;
			double ea3 = packedgrain[swap].oeuler3;
	//		int nnum = packedgrain[j].neighnum;
			packedgrain[j].euler1 = ea1;
			packedgrain[j].euler2 = ea2;
			packedgrain[j].euler3 = ea3;
			while(!pickedlist.empty())
			{
				int pickedfront = pickedlist.front();
				temppickedlist.push_back(pickedfront);
				pickedlist.pop_front();
			}
			pickedlist = temppickedlist;
			temppickedlist.clear();
		}
	}
}

void rank_grains2(int numgrains)
{
	int count = 0;
	for(int i = 0; i < numgrains; i++)
	{
		float microrank = 0;
		double temprank = 0;
		list<float> misolist = packedgrain[i].misorientationlist;
		int size = neighborvector[i].size();
		for(int j = 0; j < size; j++)
		{
			int neighfirst = neighborvector[i][j];
			double neighfrac = packedgrain[neighfirst].lowanglefraction;
			if(neighfrac > 0.24) neighfrac = neighfrac*4;
			if(neighfrac > 0.49) neighfrac = neighfrac;
			if(neighfrac > 0.74) neighfrac = neighfrac*2;
			temprank = neighfrac;
			float misofirst = misolist.front();
			if(misofirst < 15)
			{
				temprank = 0;
				if(neighfrac > 4)
				{
					microrank = 0;
					{break;}
				}
			}
			double ownfrac = packedgrain[i].lowanglefraction;
			if(ownfrac < 0.4) temprank = temprank*2;
			microrank = microrank + temprank;
			misolist.pop_front();
		}
		packedgrain[i].grainrank = microrank;
		count++;
	}
}

void identify_grains2(int numgrains,int nummicromoves)
{
	int check = 0;
	int check2 = 0;
	int worst = 0;
	int clear = 0;
	float bad = 1;
	float none = 2;
	nummicromoves = 150;
	for(int h = 0; h < numgrains; h++)
	{
		packedgrain[h].picked = clear;
	}
	while(check < nummicromoves)
	{
		double rankcur = 0;
		for(int i = 0; i < numgrains; i++)
		{
			double rank = packedgrain[i].grainmicrorank;
			double picked = packedgrain[i].picked;
			if(rank > rankcur && picked == 0)
			{
				rankcur = rank;
				worst = i;
			}
		}
		packedgrain[worst].picked = bad;
		check++;
	}
	while(check2 < nummicromoves)
	{
		float rankcur2 = 0;
		for(int j = 0; j < numgrains; j++)
		{
			double rank2 = packedgrain[j].grainmicrorank;
			double picked2 = packedgrain[j].picked;
			if(rank2 > rankcur2 && picked2 == 0)
			{
				rankcur2 = rank2;
				worst = j;
			}
		}
		packedgrain[worst].picked = none;
		check2++;
	}
}

void move_grains2(int numgrains)
{
	int check = 0;
	list<int> pickedlist;
	list<int> nonelist;
	list<int> tempnonelist;
	for(int i = 0; i < numgrains; i++)
	{
		double ea1temp = packedgrain[i].euler1;
		double ea2temp = packedgrain[i].euler2;
		double ea3temp = packedgrain[i].euler3;
		packedgrain[i].oeuler1 = ea1temp;
		packedgrain[i].oeuler2 = ea2temp;
		packedgrain[i].oeuler3 = ea3temp;
		double picked = packedgrain[i].picked;
		if(picked == 2)
		{
			nonelist.push_back(i);
			check++;
		}
	}
	for(int j = 0; j < numgrains; j++)
	{
		double picked = packedgrain[j].picked;
		if(picked == 1)
		{
			int size = nonelist.size();
			double random = rg.Random();
			double random1 = random;
			int remainder = random1*size;
			if(size <= remainder)
			{
				remainder = remainder - 1;
			}
			for(int k = 0; k < remainder; k++)
			{
				int nonefront = nonelist.front();
				tempnonelist.push_back(nonefront);
				nonelist.pop_front();
			}
			int swap = nonelist.front();
			nonelist.pop_front();
		//	int grainname = packedgrain[j].grainname;
			double ea1 = packedgrain[swap].oeuler1;
			double ea2 = packedgrain[swap].oeuler2;
			double ea3 = packedgrain[swap].oeuler3;
		//	int nnum = packedgrain[j].neighnum;
			packedgrain[j].euler1 = ea1;
			packedgrain[j].euler2 = ea2;
			packedgrain[j].euler3 = ea3;
	//		int sgrainname = packedgrain[swap].grainname;
			double sea1 = packedgrain[j].oeuler1;
			double sea2 = packedgrain[j].oeuler2;
			double sea3 = packedgrain[j].oeuler3;
		//	int snnum = packedgrain[swap].neighnum;
			packedgrain[swap].euler1 = sea1;
			packedgrain[swap].euler2 = sea2;
			packedgrain[swap].euler3 = sea3;
			while(!nonelist.empty())
			{
				int nonefront = nonelist.front();
				tempnonelist.push_back(nonefront);
				nonelist.pop_front();
			}
			nonelist = tempnonelist;
			tempnonelist.clear();
		}
	}
}

void freeze_grains(int numgrains)
{
 //int count = 0;
	float froze = 0;
	for(int i = 0; i < numgrains; i++)
	{
		froze = 0;
		double fraction = packedgrain[i].lowanglefraction;
		if(fraction > 0.85)
		{
			froze = 1;
		}
		list<float> misolist = packedgrain[i].misorientationlist;
		int size = neighborvector[i].size();
		for(int j = 0; j < size; j++)
		{
			int neighfirst = neighborvector[i][j];
			float neighmiso = misolist.front();
			double neighfrac = packedgrain[neighfirst].lowanglefraction;
			if(neighfrac > 0.85 && neighmiso < 15)
			{
				froze = 1;
			}
			misolist.pop_front();
		}
		packedgrain[i].frozen = froze;
	}
}


void volume_stats(int numgrains, string outname2)
{
	double actualgrains = 0;
//	double misocount = 0;
	double avgvol = 0;
	double avglnvol = 0;
	double avgneigh = 0;
	double avglnneigh = 0;
	double avgbovera = 0;
	double avgcovera = 0;
	double avgcoverb = 0;
//	double avgsurfarea = 0;
	double avgdiam = 0;
	double avglogdiam = 0;
	double avgdiam2 = 0;
//	double avgschmid = 0;
//	double avgem = 0;
	double neighcount = 0;
	double maxvol = 0;
	double maxneigh = 0;
//	double maxsurfarea = 0;
	double maxdiam = 0;
	double maxbovera = 0;
	double maxcovera = 0;
	double maxcoverb = 0;
//	double maxschmid = 0;
//	double maxem = 0;
	double svn[100][5];
	double svs[100][5];
	double svbovera[100][5];
	double svcovera[100][5];
	double svcoverb[100][5];
//	double svsa[100][5];
//	double svschmid[100][5];
//	double svem[100][5];
	for(int temp = 0; temp < 100; temp++)
	{
		for(int temp2 = 0; temp2 < 5; temp2++)
		{
			svn[temp][temp2] = 0;
			svs[temp][temp2] = 0;
			svbovera[temp][temp2] = 0;
			svcovera[temp][temp2] = 0;
			svcoverb[temp][temp2] = 0;
//			svsa[temp][temp2] = 0;
//			svschmid[temp][temp2] = 0;
//			svem[temp][temp2] = 0;
		}
	}
	for(int i = 0; i < numgrains; i++)
	{
		int onedge = packedgrain[i].edgegrain;
		if(onedge == 0)
		{
			actualgrains++;
			int vol = packedgrain[i].vol;
			double voxvol = vol*resx*resy*resz;
			double logvol = log(voxvol);
			double rad3 = 0.75*(1/3.1415926535897)*voxvol;
			double diam = 2*pow(rad3,0.333333333);
			int diamint = diam;
			double logdiam = log(diam);
			double I1 = packedgrain[i].axis1;
			double I2 = packedgrain[i].axis2;
			double I3 = packedgrain[i].axis3;
			I1 = (15*I1)/(4*3.1415926535897);
			I2 = (15*I2)/(4*3.1415926535897);
			I3 = (15*I3)/(4*3.1415926535897);
			double A = (I1+I2-I3)/2;
			double B = (I1+I3-I2)/2;
			double C = (I2+I3-I1)/2;
			double a = (A*A*A*A)/(B*C);
			a = pow(a,0.1);
			double b = B/A;
			b = pow(b,0.5)*a;
			double c = A/(a*a*a*b);
			double bovera = b/a;
			double covera = c/a;
			double coverb = c/b;
//			if(bovera < 0)
//			{
//				int stop = 0;
//			}
			int nnum = packedgrain[i].neighnum;
			double lognnum = log(double(nnum));
//			int surfarea = packedgrain[i].getsurfarea2();
//			double surfarea2 = (double)surfarea/(double)vol;
//			double schmid = packedgrain[i].schmidfactor;
//			double em = packedgrain[i].convexity;
			avgvol = avgvol+voxvol;
			avglnvol = avglnvol+logvol;
			avgneigh = avgneigh+nnum;
			avglnneigh = avglnneigh+lognnum;
			avgbovera = avgbovera+bovera;
			avgcovera = avgcovera+covera;
			avgcoverb = avgcoverb+coverb;
//			avgsurfarea = avgsurfarea+surfarea2;
			avgdiam = avgdiam + diam;
			avglogdiam = avglogdiam+logdiam;
//			avgschmid = avgschmid+schmid;
//			avgem = avgem+em;
			svn[diamint][0]++;
			svbovera[diamint][0]++;
			svcovera[diamint][0]++;
			svcoverb[diamint][0]++;
//			svsa[diamint][0]++;
//			svschmid[diamint][0]++;
//			svem[diamint][0]++;
			svn[diamint][1] = svn[diamint][1] + nnum;
			svn[diamint][3] = svn[diamint][3] + lognnum;
			svbovera[diamint][1] = svbovera[diamint][1] + bovera;
			svcovera[diamint][1] = svcovera[diamint][1] + covera;
			svcoverb[diamint][1] = svcoverb[diamint][1] + coverb;
//			svsa[diamint][1] = svsa[diamint][1] + surfarea2;
//			svschmid[diamint][1] = svschmid[diamint][1] + schmid;
//			svem[diamint][1] = svem[diamint][1] + em;
			int size = neighborvector[i].size();
			for(int j = 0; j < size; j++)
			{
				int neigh = neighborvector[i][j];
				int neighvol = packedgrain[neigh].vol;
				double neighvoxvol = neighvol*resx*resy*resz;
		//		double neighlogvol = log(neighvoxvol);
				double neighrad3 = 0.75*(1/3.1415926535897)*neighvoxvol;
				double neighdiam = 2*pow(neighrad3,0.333333333);
	//			int neighdiamint = neighdiam;
				svs[diamint][0]++;
				svs[diamint][1] = svs[diamint][1] + neighdiam;
				avgdiam2 = avgdiam2 + neighdiam;
				neighcount++;
			}
			if(voxvol > maxvol) maxvol = voxvol;
			if(nnum > maxneigh) maxneigh = nnum;
			if(bovera > maxbovera) maxbovera = bovera;
			if(covera > maxcovera) maxcovera = covera;
			if(coverb > maxcoverb) maxcoverb = coverb;
//			if(surfarea2 > maxsurfarea) maxsurfarea = surfarea2;
			if(diam > maxdiam) maxdiam = diam;
//			if(schmid > maxschmid) maxschmid = schmid;
//			if(em > maxem) maxem = em;
		}
	}
	int maxdiamint = maxdiam;
	for(int temp3 = 0; temp3 < (maxdiamint+1); temp3++)
	{
		if(svn[temp3][0] != 0)
		{
			svn[temp3][1] = svn[temp3][1]/svn[temp3][0];
			svn[temp3][3] = svn[temp3][3]/svn[temp3][0];
			svs[temp3][1] = svs[temp3][1]/svs[temp3][0];
			svbovera[temp3][1] = svbovera[temp3][1]/svbovera[temp3][0];
			svcovera[temp3][1] = svcovera[temp3][1]/svcovera[temp3][0];
			svcoverb[temp3][1] = svcoverb[temp3][1]/svcoverb[temp3][0];
//			svsa[temp3][1] = svsa[temp3][1]/svsa[temp3][0];
//			svschmid[temp3][1] = svschmid[temp3][1]/svschmid[temp3][0];
//			svem[temp3][1] = svem[temp3][1]/svem[temp3][0];
		}
	}
	avgvol = avgvol/actualgrains;
	avglnvol = avglnvol/actualgrains;
	avgneigh = avgneigh/actualgrains;
	avglnneigh = avglnneigh/actualgrains;
	avgbovera = avgbovera/actualgrains;
	avgcovera = avgcovera/actualgrains;
	avgcoverb = avgcoverb/actualgrains;
//	avgsurfarea = avgsurfarea/actualgrains;
	avgdiam = avgdiam/actualgrains;
	avglogdiam = avglogdiam/actualgrains;
	avgdiam2 = avgdiam2/neighcount;
//	avgschmid = avgschmid/actualgrains;
//	avgem = avgem/actualgrains;
	maxvol = maxvol/avgvol;
	int vbw = (maxvol/25)+1;
	int nbw = (maxneigh/25)+1;
	double sbw1 = 1.0/25.0;
	double sbw2 = 1.0/25.0;
	double sbw3 = 1.0/25.0;
//	double sabw = 1.0/25.0;
//	double schw = 0.5/25.0;
//	double emw = 1.0/25.0;
	double vbin[25];
	double nbin[25];
	double sbin1[25];
	double sbin2[25];
	double sbin3[25];
//	double sabin[25];
//	double schbin[25];
//	double embin[25];
	double svnbin[100][25];
	double svsbin[100][100];
	double svboverabin[100][25];
	double svcoverabin[100][25];
	double svcoverbbin[100][25];
//	double svsabin[100][25];
//	double svschmidbin[100][25];
//	double svembin[100][25];
	double misobin[10];
	double microbin[10];
	for(int a = 0; a < 100; a++)
	{
		for(int b = 0; b < 25; b++)
		{
			svnbin[a][b] = 0;
			svboverabin[a][b] = 0;
			svcoverabin[a][b] = 0;
			svcoverbbin[a][b] = 0;
//			svsabin[a][b] = 0;
//			svschmidbin[a][b] = 0;
//			svembin[a][b] = 0;
		}
		for(int d = 0; d < 100; d++)
		{
			svsbin[a][d] = 0;
		}
	}
	for(int c = 0; c < 25; c++)
	{
		vbin[c] = 0;
		nbin[c] = 0;
		sbin1[c] = 0;
		sbin2[c] = 0;
		sbin3[c] = 0;
//		sabin[c] = 0;
//		schbin[c] = 0;
//		embin[c] = 0;
	}
	for(int e = 0; e < 10; e++)
	{
		misobin[e] = 0;
		microbin[e] = 0;
	}
	double sdvol = 0;
	double sdlnvol = 0;
	double sdneigh = 0;
	double sdlnneigh = 0;
	double sdbovera = 0;
	double sdcovera = 0;
	double sdcoverb = 0;
//	double sdsurfarea = 0;
	double sddiam = 0;
	double sdlogdiam = 0;
	double sddiam2 = 0;
//	double sdschmid = 0;
//	double sdem = 0;
	for(int j = 0; j < numgrains; j++)
	{
		int onedge = packedgrain[j].edgegrain;
		if(onedge == 0)
		{
			int vol = packedgrain[j].vol;
			double voxvol = vol*resx*resy*resz;
			double logvol = log(voxvol);
			double rad3 = 0.75*(1/3.1415926535897)*voxvol;
			double diam = 2*pow(rad3,0.333333333);
			int diamint = diam;
			double logdiam = log(diam);
			double I1 = packedgrain[j].axis1;
			double I2 = packedgrain[j].axis2;
			double I3 = packedgrain[j].axis3;
			I1 = (15*I1)/(4*3.1415926535897);
			I2 = (15*I2)/(4*3.1415926535897);
			I3 = (15*I3)/(4*3.1415926535897);
			double A = (I1+I2-I3)/2;
			double B = (I1+I3-I2)/2;
			double C = (I2+I3-I1)/2;
			double a = (A*A*A*A)/(B*C);
			a = pow(a,0.1);
			double b = B/A;
			b = pow(b,0.5)*a;
			double c = A/(a*a*a*b);
			double bovera = b/a;
			double covera = c/a;
			double coverb = c/b;
			int nnum = packedgrain[j].neighnum;
			double lognnum = log(double(nnum));
//			int surfarea = packedgrain[j].getsurfarea2();
//			double surfarea2 = (double)surfarea/(double)vol;
//			double schmid = packedgrain[j].schmidfactor;
//			double em = packedgrain[j].convexity;
			sdvol = sdvol + ((voxvol-avgvol)*(voxvol-avgvol));
			sdlnvol = sdlnvol + ((logvol-avglnvol)*(logvol-avglnvol));
			sdneigh = sdneigh + ((nnum-avgneigh)*(nnum-avgneigh));
			sdlnneigh = sdlnneigh + ((lognnum-avglnneigh)*(lognnum-avglnneigh));
			sdbovera = sdbovera + ((bovera-avgbovera)*(bovera-avgbovera));
			sdcovera = sdcovera + ((covera-avgcovera)*(covera-avgcovera));
			sdcoverb = sdcoverb + ((coverb-avgcoverb)*(coverb-avgcoverb));
//			sdsurfarea = sdsurfarea + ((surfarea2-avgsurfarea)*(surfarea2-avgsurfarea));
			sddiam = sddiam + ((diam-avgdiam)*(diam-avgdiam));
			sdlogdiam = sdlogdiam + ((logdiam-avglogdiam)*(logdiam-avglogdiam));
//			sdschmid = sdschmid + ((schmid-avgschmid)*(schmid-avgschmid));
//			sdem = sdem + ((em-avgem)*(em-avgem));
			svn[diamint][2] = svn[diamint][2] + ((nnum-svn[diamint][1])*(nnum-svn[diamint][1]));
			svn[diamint][4] = svn[diamint][4] + ((lognnum-svn[diamint][3])*(lognnum-svn[diamint][3]));
			svbovera[diamint][2] = svbovera[diamint][2] + ((bovera-svbovera[diamint][1])*(bovera-svbovera[diamint][1]));
			svcovera[diamint][2] = svcovera[diamint][2] + ((covera-svcovera[diamint][1])*(covera-svcovera[diamint][1]));
			svcoverb[diamint][2] = svcoverb[diamint][2] + ((coverb-svcoverb[diamint][1])*(coverb-svcoverb[diamint][1]));
//			svsa[diamint][2] = svsa[diamint][2] + ((surfarea2-svsa[diamint][1])*(surfarea2-svsa[diamint][1]));
//			svschmid[diamint][2] = svschmid[diamint][2] + ((schmid-svschmid[diamint][1])*(schmid-svschmid[diamint][1]));
//			svem[diamint][2] = svem[diamint][2] + ((em-svem[diamint][1])*(em-svem[diamint][1]));
			int size = neighborvector[j].size();
			for(int k = 0; k < size; k++)
			{
				int neigh = neighborvector[j][k];
				int neighvol = packedgrain[neigh].vol;
				double neighvoxvol = neighvol*resx*resy*resz;
		//		double neighlogvol = log(neighvoxvol);
				double neighrad3 = 0.75*(1/3.1415926535897)*neighvoxvol;
				double neighdiam = 2*pow(neighrad3,0.333333333);
				int neighdiamint = neighdiam;
				svs[diamint][2] = svs[diamint][2] + ((neighdiam-svs[diamint][1])*(neighdiam-svs[diamint][1]));
				svsbin[diamint][neighdiamint]++;
				sddiam2 = sddiam2 + ((neighdiam-avgdiam2)*(neighdiam-avgdiam2));
			}
			voxvol = voxvol/avgvol;
			int vcur = voxvol/vbw;
			if(vcur == 25) vcur = 24;
			int ncur = nnum/nbw;
			if(ncur == 25) ncur = 24;
			int s1cur = bovera/sbw1;
			if(s1cur == 25) s1cur = 24;
			int s2cur = covera/sbw2;
			if(s2cur == 25) s2cur = 24;
			int s3cur = coverb/sbw3;
			if(s3cur == 25) s3cur = 24;
//			int sacur = surfarea2/sabw;
//			if(sacur == 25) sacur = 24;
//			int schcur = schmid/schw;
//			if(schcur == 25) schcur = 24;
//			int emcur = em/emw;
//			if(emcur == 25) emcur = 24;
			vbin[vcur]++;
			nbin[ncur]++;
			sbin1[s1cur]++;
			sbin2[s2cur]++;
			sbin3[s3cur]++;
//			sabin[sacur]++;
//			schbin[schcur]++;
//			embin[emcur]++;
			svnbin[diamint][ncur]++;
			svboverabin[diamint][s1cur]++;
			svcoverabin[diamint][s2cur]++;
			svcoverbbin[diamint][s3cur]++;
//			svsabin[diamint][sacur]++;
//			svschmidbin[diamint][schcur]++;
//			svembin[diamint][emcur]++;
		}
	}
	for(int temp4 = 0; temp4 < (maxdiamint+1); temp4++)
	{
		if(svn[temp4][0] != 0)
		{
			svn[temp4][2] = svn[temp4][2]/svn[temp4][0];
			svn[temp4][4] = svn[temp4][4]/svn[temp4][0];
			svs[temp4][2] = svs[temp4][2]/svs[temp4][0];
			svbovera[temp4][2] = svbovera[temp4][2]/svbovera[temp4][0];
			svcovera[temp4][2] = svcovera[temp4][2]/svcovera[temp4][0];
			svcoverb[temp4][2] = svcoverb[temp4][2]/svcoverb[temp4][0];
//			svsa[temp4][2] = svsa[temp4][2]/svsa[temp4][0];
//			svschmid[temp4][2] = svschmid[temp4][2]/svschmid[temp4][0];
			svbovera[temp4][3] = svbovera[temp4][1]*(((svbovera[temp4][1]*(1-svbovera[temp4][1]))/svbovera[temp4][2])-1);
			svbovera[temp4][4] = (1-svbovera[temp4][1])*(((svbovera[temp4][1]*(1-svbovera[temp4][1]))/svbovera[temp4][2])-1);
			svcovera[temp4][3] = svcovera[temp4][1]*(((svcovera[temp4][1]*(1-svcovera[temp4][1]))/svcovera[temp4][2])-1);
			svcovera[temp4][4] = (1-svcovera[temp4][1])*(((svcovera[temp4][1]*(1-svcovera[temp4][1]))/svcovera[temp4][2])-1);
			svcoverb[temp4][3] = svcoverb[temp4][1]*(((svcoverb[temp4][1]*(1-svcoverb[temp4][1]))/svcoverb[temp4][2])-1);
			svcoverb[temp4][4] = (1-svcoverb[temp4][1])*(((svcoverb[temp4][1]*(1-svcoverb[temp4][1]))/svcoverb[temp4][2])-1);
			svn[temp4][2] = pow(svn[temp4][2],0.5);
			svn[temp4][4] = pow(svn[temp4][4],0.5);
			svs[temp4][2] = pow(svs[temp4][2],0.5);
			svbovera[temp4][2] = pow(svbovera[temp4][2],0.5);
			svcovera[temp4][2] = pow(svcovera[temp4][2],0.5);
			svcoverb[temp4][2] = pow(svcoverb[temp4][2],0.5);
//			svsa[temp4][2] = pow(svsa[temp4][2],0.5);
//			svschmid[temp4][2] = pow(svschmid[temp4][2],0.5);
//			svem[temp4][2] = pow(svem[temp4][2],0.5);
		}
	}
	sdvol = sdvol/actualgrains;
	sdlnvol = sdlnvol/actualgrains;
	sdneigh = sdneigh/actualgrains;
	sdlnneigh = sdlnneigh/actualgrains;
	sdbovera = sdbovera/actualgrains;
	sdcovera = sdcovera/actualgrains;
	sdcoverb = sdcoverb/actualgrains;
//	sdsurfarea = sdsurfarea/actualgrains;
	sddiam = sddiam/actualgrains;
	sdlogdiam = sdlogdiam/actualgrains;
	sddiam2 = sddiam2/neighcount;
//	sdschmid = sdschmid/actualgrains;
//	sdem = sdem/actualgrains;
//	double volvar = sdvol;
//	double vollnvar = sdlnvol;
	double neighvar = sdneigh;
//	double neighlnvar = sdlnneigh;
	double boveravar = sdbovera;
	double coveravar = sdcovera;
	double coverbvar = sdcoverb;
//	double surfareavar = sdsurfarea;
//	double diamvar = sddiam;
//	double logdiamvar = sdlogdiam;
	double diamvar2 = sddiam2;
//	double schmidvar = sdschmid;
//	double emvar = sdem;
//	double pbovera = avgbovera*(((avgbovera*(1-avgbovera))/boveravar)-1);
//	double qbovera = (1-avgbovera)*(((avgbovera*(1-avgbovera))/boveravar)-1);
//	double pcovera = avgcovera*(((avgcovera*(1-avgcovera))/coveravar)-1);
//	double qcovera = (1-avgcovera)*(((avgcovera*(1-avgcovera))/coveravar)-1);
//	double pcoverb = avgcoverb*(((avgcoverb*(1-avgcoverb))/coverbvar)-1);
//	double qcoverb = (1-avgcoverb)*(((avgcoverb*(1-avgcoverb))/coverbvar)-1);
	sdvol = pow(sdvol,0.5);
	sdlnvol = pow(sdlnvol,0.5);
	sdneigh = pow(sdlnneigh,0.5);
	sdlnneigh = pow(sdneigh,0.5);
	sdbovera = pow(sdbovera,0.5);
	sdcovera = pow(sdcovera,0.5);
	sdcoverb = pow(sdcoverb,0.5);
//	sdsurfarea = pow(sdsurfarea,0.5);
	sddiam = pow(sddiam,0.5);
	sdlogdiam = pow(sdlogdiam,0.5);
	sddiam2 = pow(sddiam2,0.5);
//	sdschmid = pow(sdschmid,0.5);
//	sdem = pow(sdem,0.5);
	double svncr = 0;
	double svscr = 0;
	double svboveracr = 0;
	double svcoveracr = 0;
	double svcoverbcr = 0;
//	double svsacr = 0;
//	double svschmidcr = 0;
//	double svemcr = 0;
	for(int temp5 = 0; temp5 < (maxdiamint+1); temp5++)
	{
		svncr = svncr + (svn[temp5][0]*((svn[temp5][1]-avgneigh)*(svn[temp5][1]-avgneigh)));
		svscr = svscr + (svs[temp5][0]*((svs[temp5][1]-avgdiam)*(svs[temp5][1]-avgdiam)));
		svboveracr = svboveracr + (svbovera[temp5][0]*((svbovera[temp5][1]-avgbovera)*(svbovera[temp5][1]-avgbovera)));
		svcoveracr = svcoveracr + (svcovera[temp5][0]*((svcovera[temp5][1]-avgcovera)*(svcovera[temp5][1]-avgcovera)));
		svcoverbcr = svcoverbcr + (svcoverb[temp5][0]*((svcoverb[temp5][1]-avgcoverb)*(svcoverb[temp5][1]-avgcoverb)));
//		svsacr = svsacr + (svsa[temp5][0]*((svsa[temp5][1]-avgsurfarea)*(svsa[temp5][1]-avgsurfarea)));
//		svschmidcr = svschmidcr + (svschmid[temp5][0]*((svschmid[temp5][1]-avgschmid)*(svschmid[temp5][1]-avgschmid)));
//		svemcr = svemcr + (svem[temp5][0]*((svem[temp5][1]-avgem)*(svem[temp5][1]-avgem)));
	}
	svncr = svncr/(actualgrains*neighvar);
	svscr = svscr/(neighcount*diamvar2);
	svboveracr = svboveracr/(actualgrains*boveravar);
	svcoveracr = svcoveracr/(actualgrains*coveravar);
	svcoverbcr = svcoverbcr/(actualgrains*coverbvar);
//	svsacr = svsacr/(actualgrains*surfareavar);
//	svschmidcr = svschmidcr/(actualgrains*schmidvar);
//	svemcr = svemcr/(actualgrains*emvar);
//	for(int k = 0; k < numgrains; k++)
//	{
//		int onedge = packedgrain[k].getonedge();
//		if(onedge == 0)
//		{
//			double microcount = 0;
//			list<double> misolist = packedgrain[k].getmisolist();
//			int nummisos = misolist.size();
//			while(!misolist.empty())
//			{
//				misocount++;
//				double firstmiso = misolist.front();
//				int misocur = firstmiso/6.5;
//				if(misocur == 10) misocur = 9;
//				misobin[misocur]++;
//				if(firstmiso < 15) microcount++;
//				misolist.pop_front();
//			}
//			microcount = microcount/nummisos;
//			int microcur = microcount/0.25;
//			if(microcur == 4) microcur = 3;
//			microbin[microcur]++;
//		}
//	}
    ofstream outFile;
    outFile.open(outname2.c_str());
	outFile << "INDIVIDUAL DISTRIBUTIONS" << endl;
	outFile << endl;
	outFile << "Parameter" << "	" << "Avg." << "	" << "Std. Dev." << endl;
	outFile << "Volume" << "	" << avgvol << "	" << sdvol << endl;
	outFile << "Volume" << "	" << avglnvol << "	" << sdlnvol << endl;
	outFile << "Neighbors" << "	" << avgneigh << "	" << sdneigh << endl;
	outFile << "Neighbors" << "	" << avglnneigh << "	" << sdlnneigh << endl;
	outFile << "b/a" << "	" << avgbovera << "	" << sdbovera << endl;
	outFile << "c/a" << "	" << avgcovera << "	" << sdcovera << endl;
	outFile << "c/b" << "	" << avgcoverb << "	" << sdcoverb <<  endl;
//	outFile << "Surface Area" << "	" << avgsurfarea << "	" << sdsurfarea << endl;
	outFile << "Diameter" << "	" << avgdiam << "	" << sddiam << endl;
//	outFile << "Schmid Factor" << "	" << avgschmid << "	" << sdschmid << endl;
//	outFile << "Ellipsoidal Misfit" << "	" << avgem << "	" << sdem << endl;
	outFile << endl;
//	outFile << "Volume Bin" << "	" << "Count" << "	" << "Neighbor Bin" << "	" << "Count" << "	" << "b/a Bin" << "	" << "Count" << "	" << "c/a Bin" << "	" << "Count" << "	" << "c/b Bin" << "	" << "Count" << "	" << "Surface Area Bin" << "	" << "Count"  << "	" << "Schmid Factor Bin" << "	" << "Count" << "	" << "Ellipsoidal Misfit Bin" << "	" << "Count" << endl;
//	for(int temp6 = 0; temp6 < 25; temp6++)
//	{
//		outFile << (vbw*temp6) << "	" << vbin[temp6]/actualgrains << "	" << (nbw*temp6) << "	" << nbin[temp6]/actualgrains << "	" << (sbw1*temp6) << "	" << sbin1[temp6]/actualgrains << "	" << (sbw2*temp6) << "	" << sbin2[temp6]/actualgrains << "	" << (sbw3*temp6) << "	" << sbin3[temp6]/actualgrains << "	" << (sabw*temp6) << "	" << sabin[temp6]/actualgrains << "	" << (schw*temp6) << "	" << schbin[temp6]/actualgrains << "	" << (emw*temp6) << "	" << embin[temp6]/actualgrains << endl;
//	}
	outFile << "Volume Bin" << "	" << "Count" << "	" << "Neighbor Bin" << "	" << "Count" << "	" << "b/a Bin" << "	" << "Count" << "	" << "c/a Bin" << "	" << "Count" << "	" << "c/b Bin" << "	" << "Count" << endl;
	for(int temp6 = 0; temp6 < 25; temp6++)
	{
		outFile << (vbw*temp6) << "	" << vbin[temp6]/actualgrains << "	" << (nbw*temp6) << "	" << nbin[temp6]/actualgrains << "	" << (sbw1*temp6) << "	" << sbin1[temp6]/actualgrains << "	" << (sbw2*temp6) << "	" << sbin2[temp6]/actualgrains << "	" << (sbw3*temp6) << "	" << sbin3[temp6]/actualgrains << endl;
	}
	outFile << endl;
	outFile << "CORRELATIONS" << endl;
	outFile << endl;
	outFile << "Size V Neighbors" << "	" << svncr << endl;
	outFile << "Size V Neighbor Size" << "	" << svscr << endl;
	outFile << "Size V b/a" << "	" << svboveracr << endl;
	outFile << "Size V c/a" << "	" << svcoveracr << endl;
	outFile << "Size V c/b" << "	" << svcoverbcr << endl;
//	outFile << "Size V Surface Area" << "	" << svsacr << endl;
//	outFile << "Size V Schmid Factor" << "	" << svschmidcr << endl;
//	outFile << "Size V Ellipsoidal Misfit" << "	" << svemcr << endl;
	outFile << endl;
//	outFile << "Diameter" << "	" << "Count" << "	" << "Avg. Neighbors" << "	" << "Std. Dev. Neighbors" << "	" << "Avg. Ln Neighbors" << "	" << "Std. Dev.  Ln Neighbors" << "	" << "Avg. Neighbor Diameter" << "	" << "Std. Dev. Neighbor Diameter" << "	" <<"Avg. b/a" << "	" << "Std. Dev. b/a" << "	" << "Avg. c/a" << "	" << "Std. Dev. c/a" << "	" << "Avg. c/b" << "	" << "Std. Dev. c/b" << "	" <<"P b/a" << "	" << "Q Dev. b/a" << "	" << "P c/a" << "	" << "Q Dev. c/a" << "	" << "P c/b" << "	" << "Q Dev. c/b" << "	" << "Avg. Surface Area" << "	" << "Std. Dev. Surface Area" << "	" << "Avg. Schmid Factor" << "	" << "Std. Dev. Schmid Factor" << "	" << "Avg. Ellipsoidal misfot" << "	" << "Std. Dev. Ellipsoidal misfot" << endl;
//	for(int temp7 = 0; temp7 < (maxdiamint+1); temp7++)
//	{
//		outFile << temp7 << "	" << svn[temp7][0] << "	" << svn[temp7][1] << "	" << svn[temp7][2] << "	" << svn[temp7][3] << "	" << svn[temp7][4] << "	" << svs[temp7][1] << "	" << svs[temp7][2] << "	" << svbovera[temp7][1] << "	" << svbovera[temp7][2] << "	" << svcovera[temp7][1] << "	" << svcovera[temp7][2] << "	" << svcoverb[temp7][1] << "	" << svcoverb[temp7][2] << "	" << svbovera[temp7][3] << "	" << svbovera[temp7][4] << "	" << svcovera[temp7][3] << "	" << svcovera[temp7][4] << "	" << svcoverb[temp7][3] << "	" << svcoverb[temp7][4] << "	" << svsa[temp7][1] << "	" << svsa[temp7][2] << "	" << svschmid[temp7][1] << "	" << svschmid[temp7][2] << "	" << svem[temp7][1] << "	" << svem[temp7][2] << endl;
//	}
	outFile << "Diameter" << "	" << "Count" << "	" << "Avg. Neighbors" << "	" << "Std. Dev. Neighbors" << "	" << "Avg. Ln Neighbors" << "	" << "Std. Dev.  Ln Neighbors" << "	" << "Avg. Neighbor Diameter" << "	" << "Std. Dev. Neighbor Diameter" << "	" <<"Avg. b/a" << "	" << "Std. Dev. b/a" << "	" << "Avg. c/a" << "	" << "Std. Dev. c/a" << "	" << "Avg. c/b" << "	" << "Std. Dev. c/b" << "	" <<"P b/a" << "	" << "Q Dev. b/a" << "	" << "P c/a" << "	" << "Q Dev. c/a" << "	" << "P c/b" << "	" << "Q Dev. c/b" << endl;
	for(int temp7 = 0; temp7 < (maxdiamint+1); temp7++)
	{
		outFile << temp7 << "	" << svn[temp7][0] << "	" << svn[temp7][1] << "	" << svn[temp7][2] << "	" << svn[temp7][3] << "	" << svn[temp7][4] << "	" << svs[temp7][1] << "	" << svs[temp7][2] << "	" << svbovera[temp7][1] << "	" << svbovera[temp7][2] << "	" << svcovera[temp7][1] << "	" << svcovera[temp7][2] << "	" << svcoverb[temp7][1] << "	" << svcoverb[temp7][2] << "	" << svbovera[temp7][3] << "	" << svbovera[temp7][4] << "	" << svcovera[temp7][3] << "	" << svcovera[temp7][4] << "	" << svcoverb[temp7][3] << "	" << svcoverb[temp7][4] << endl;
	}
	outFile << endl;
	for(int temp8 = 0; temp8 < 25; temp8++)
	{
		for(int temp9 = 0; temp9 < (maxdiamint+1); temp9++)
		{
			outFile << (svnbin[temp9][temp8]/svn[temp9][0]) << "	";
		}
		outFile << endl;
	}
	outFile << endl;
	for(int temp10 = 0; temp10 < (maxdiamint+1); temp10++)
	{
		for(int temp11 = 0; temp11 < (maxdiamint+1); temp11++)
		{
			outFile << (svsbin[temp11][temp10]/svs[temp11][0]) << "	";
		}
		outFile << endl;
	}
	outFile << endl;
	for(int temp12 = 0; temp12 < 25; temp12++)
	{
		for(int temp13 = 0; temp13 < (maxdiamint+1); temp13++)
		{
			outFile << (svboverabin[temp13][temp12]/svn[temp13][0]) << "	";
		}
		outFile << endl;
	}
	outFile << endl;
	for(int temp14 = 0; temp14 < 25; temp14++)
	{
		for(int temp15 = 0; temp15 < (maxdiamint+1); temp15++)
		{
			outFile << (svcoverabin[temp15][temp14]/svn[temp15][0]) << "	";
		}
		outFile << endl;
	}
	outFile << endl;
	for(int temp16 = 0; temp16 < 25; temp16++)
	{
		for(int temp17 = 0; temp17 < (maxdiamint+1); temp17++)
		{
			outFile << (svcoverbbin[temp17][temp16]/svn[temp17][0]) << "	";
		}
		outFile << endl;
	}
	outFile << endl;
//	for(int temp18 = 0; temp18 < 25; temp18++)
//	{
//		for(int temp19 = 0; temp19 < (maxdiamint+1); temp19++)
//		{
//			outFile << (svsabin[temp19][temp18]/svn[temp19][0]) << "	";
//		}
//		outFile << endl;
//	}
//	outFile << endl;
//	for(int temp20 = 0; temp20 < 25; temp20++)
//	{
//		for(int temp21 = 0; temp21 < (maxdiamint+1); temp21++)
//		{
//			outFile << (svschmidbin[temp21][temp20]/svn[temp21][0]) << "	";
//		}
//		outFile << endl;
//	}
//	outFile << endl;
//	for(int temp22 = 0; temp22 < 25; temp22++)
//	{
//		for(int temp23= 0; temp23< (maxdiamint+1); temp23++)
//		{
//			outFile << (svembin[temp23][temp22]/svn[temp23][0]) << "	";
//		}
//		outFile << endl;
//	}
//	outFile << endl;
//	for(int temp24 = 0; temp24 < 10; temp24++)
//	{
//		outFile << (misobin[temp24]/misocount) << endl;
//	}
//	outFile << endl;
//	for(int temp25 = 0; temp25 < 10; temp25++)
//	{
//		outFile << (microbin[temp25]/actualgrains) << endl;
//	}
	outFile.close();
}


void writeCube(string outname1, int numgrains)
{
    ofstream outFile;
    outFile.open(outname1.c_str());
	int xpoints = (sizex/resx);
	int ypoints = (sizey/resy);
	int zpoints = (sizez/resz);
	outFile << "# vtk DataFile Version 2.0" << endl;
	outFile << "data set from FFT2dx_GB" << endl;
	outFile << "ASCII" << endl;
	outFile << "DATASET STRUCTURED_POINTS" << endl;
	outFile << "DIMENSIONS " << xpoints << " " << ypoints << " " << zpoints << endl;
	outFile << "ORIGIN 0.0 0.0 0.0" << endl;
	outFile << "SPACING " << resx << " " << resy << " " << resz << endl;
	outFile << "POINT_DATA " << xpoints*ypoints*zpoints << endl;
	outFile << endl;
	outFile << endl;
	outFile << "SCALARS GrainID int  1" << endl;
	outFile << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < (xpoints*ypoints*zpoints); i++)
	{
		if(i%20 == 0 && i > 0) outFile << endl;
		int name = gridfine[i].grainname;
//		int column = (i%xpoints);
//		int row = ((i/xpoints)%ypoints);
//		int plane = (i/(xpoints*ypoints));
//		double xc = (column*resx)+(resx/2);
//		double yc = (row*resy)+(resy/2);
//		double zc = (plane*resz)+(resz/2);
//		int gnum = gridfine[i].grainname;
//		double r = packedgrain[gnum].red;
//		double g = packedgrain[gnum].green;
//		double b = packedgrain[gnum].blue;
		outFile << "   ";
		if(name < 10000) outFile << " ";
		if(name < 1000) outFile << " ";
		if(name < 100) outFile << " ";
		if(name < 10) outFile << " ";
		outFile << name;
	}
	outFile.close();
}


void write_grains(string outname4, int numgrains)
{
    ofstream outFile;
    outFile.open(outname4.c_str());
	outFile << numgrains << endl;
	for(int i=0;i<numgrains;i++)
	{
		int name = packedgrain[i].grainname;
		double xc = packedgrain[i].xc;
		double yc = packedgrain[i].yc;
		double zc = packedgrain[i].zc;
		double ea1 = packedgrain[i].euler1;
		double ea2 = packedgrain[i].euler2;
		double ea3 = packedgrain[i].euler3;
		int nnum = packedgrain[i].neighnum;
		outFile << name << "	" << xc << "	" << yc << "	" << zc << "	" << ea1 << "	" << ea2 << "	" << ea3 << "	" << nnum;
		int size = neighborvector[i].size();
		for(int j = 0; j < size; j++)
		{
			int neigh = neighborvector[i][j];
			outFile << "	" << neigh;
		}
		outFile << endl;
	}
}
void find_boundarycenters(string outname3, int numgrains)
{
	double xmin,xmax,ymin,ymax,zmin,zmax;
	int count = 0;
	double tempcount = 0;
	int xpoints = (sizex/resx);
	int ypoints = (sizey/resy);
//	int zpoints = (sizez/resz);
	for(int a = 0; a < 100000; a++)
	{
		for(int b = 0; b < 5; b++)
		{
			bcent[a][b] = 0;
		}
	}
	for(int i = 0; i < numgrains; i++)
	{
		int size = neighborvector[i].size();
		for(int j = 0; j < size; j++)
		{
			int firstneigh = neighborvector[i][j];
			tempcount = 0;
			xmin = 1000000;
			xmax = 0;
			ymin = 1000000;
			ymax = 0;
			zmin = 1000000;
			zmax = 0;
			int size2 = voxelvector[i].size();
			for(int k = 0; k < size2; k++)
			{
				int point = voxelvector[i][k];
				int onsurf = gridfine[point].boundarypoint;
				if(onsurf == 1)
				{
					for(int temp = 0; temp < 6; temp++)
					{
						int firstvoxneigh;
						if(temp == 0)
						{
							firstvoxneigh = gridfine[point].neighbor1;
						}
						if(temp == 1)
						{
							firstvoxneigh = gridfine[point].neighbor2;
						}
						if(temp == 2)
						{
							firstvoxneigh = gridfine[point].neighbor3;
						}
						if(temp == 3)
						{
							firstvoxneigh = gridfine[point].neighbor4;
						}
						if(temp == 4)
						{
							firstvoxneigh = gridfine[point].neighbor5;
						}
						if(temp == 5)
						{
							firstvoxneigh = gridfine[point].neighbor6;
						}
						if(firstvoxneigh == firstneigh)
						{
							int column = (point%xpoints);
							int row = ((point/xpoints)%ypoints);
							int plane = (point/(xpoints*ypoints));
							double x = (column*resx)+(resx/2);
							double y = (row*resy)+(resy/2);
							double z = (plane*resz)+(resz/2);
							if(x < xmin) xmin = x;
							if(x > xmax) xmax = x;
							if(y < ymin) ymin = y;
							if(y > ymax) ymax = y;
							if(z < zmin) zmin = z;
							if(z > zmax) zmax = z;
							bcent[count][2] = bcent[count][2] + x;
							bcent[count][3] = bcent[count][3] + y;
							bcent[count][4] = bcent[count][4] + z;
							tempcount++;
							{break;}
						}
					}
				}
			}
			bcent[count][0] = i;
			bcent[count][1] = firstneigh;
			bcent[count][2] = bcent[count][2]/tempcount;
			bcent[count][3] = bcent[count][3]/tempcount;
			bcent[count][4] = bcent[count][4]/tempcount;
			count++;
		}
	}
	ofstream outFile;
	outFile.open(outname3.c_str());
	outFile << count << endl;
	for(int l = 0; l < count; l++)
	{
		int grain1 = bcent[l][0];
		int grain2 = bcent[l][1];
		double xc = bcent[l][2];
		double yc = bcent[l][3];
		double zc = bcent[l][4];
		if(xc != 0 || yc != 0 || zc != 0)
		{
			outFile << grain1 << "	" << grain2 << "	" << xc << "	" << yc << "	" << zc << endl;
		}
	}
	outFile.close();
}




void write_volume(string writename5)
{
    ofstream outFile;
    outFile.open(writename5.c_str());
	int xpoints = (sizex/resx);
	int ypoints = (sizey/resy);
	int zpoints = (sizez/resz);
	for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
	{
		int grainname = gridfine[i].grainname;
		double ea1 = packedgrain[grainname].euler1;
		double ea2 = packedgrain[grainname].euler2;
		double ea3 = packedgrain[grainname].euler3;
		int column = (i%xpoints);
		int row = ((i/xpoints)%ypoints);
		int plane = (i/(xpoints*ypoints));
		double xc = (column*resx)+(resx/2);
		double yc = (row*resy)+(resy/2);
		double zc = (plane*resz)+(resz/2);
		double ci = 1;
		outFile << grainname << "	" << ea1 << "	" << ea2 << "	" << ea3 << "	" << xc << "	" << yc << "	" << zc << "	" << ci << endl;
	}
	outFile.close();
}
