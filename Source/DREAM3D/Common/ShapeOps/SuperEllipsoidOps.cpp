/*
 * SuperEllipsoidOps.cpp
 *
 *  Created on: Jul 18, 2011
 *      Author: mjackson
 */

#include "SuperEllipsoidOps.h"

#include "DREAM3D/Common/AIMMath.h"

const static float m_pi = M_PI;

using namespace DREAM3D;

float ShapeClass2Omega3[41][2] = {{0.0f, 0.0f},
                    {0.0f, 0.25f},
                    {0.0f, 0.5f},
                    {0.0f, 0.75f},
                    {0.0f, 1.0f},
                    {0.0f, 1.25f},
                    {0.0f, 1.5f},
                    {0.0f, 1.75f},
                    {0.0f, 2.0f},
                    {0.0f, 2.25f},
                    {0.0f, 2.5f},
                    {0.0f, 2.75f},
                    {0.0f, 3.0f},
                    {0.0f, 3.25f},
                    {0.0f, 3.5f},
                    {0.0f, 3.75f},
                    {0.0f, 4.0f},
                    {0.0f, 4.25f},
                    {0.0f, 4.5f},
                    {0.0f, 4.75f},
                    {0.0f, 5.0f},
                    {0.0f, 5.25f},
                    {0.0f, 5.5f},
                    {0.0f, 5.75f},
                    {0.0f, 6.0f},
                    {0.0f, 6.25f},
                    {0.0f, 6.5f},
                    {0.0f, 6.75f},
                    {0.0f, 7.0f},
                    {0.0f, 7.25f},
                    {0.0f, 7.5f},
                    {0.0f, 7.75f},
                    {0.0f, 8.0f},
                    {0.0f, 8.25f},
                    {0.0f, 8.5f},
                    {0.0f, 8.75f},
                    {0.0f, 9.0f},
                    {0.0f, 9.25f},
                    {0.0f, 9.5f},
                    {0.0f, 9.75f},
                    {0.0f, 10.0f}};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SuperEllipsoidOps::SuperEllipsoidOps() :
Nvalue(0.0f)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SuperEllipsoidOps::~SuperEllipsoidOps()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float SuperEllipsoidOps::radcur1(std::map<ArgName, float> args)
{
  float radcur1 = 0.0f;
  float Nvaluedist = 0.0f;
  float bestNvaluedist = 1000000.0f;

  float omega3 = args[Omega3];
  float volcur = args[VolCur];
  float bovera = args[B_OverA];
  float covera = args[C_OverA];

  for (int i = 0; i < 41; i++)
  {
    float a = gamma(1.0 + 1.0 / ShapeClass2Omega3[i][1]);
    float b = gamma(5.0 / ShapeClass2Omega3[i][1]);
    float c = gamma(3.0 / ShapeClass2Omega3[i][1]);
    float d = gamma(1.0 + 3.0 / ShapeClass2Omega3[i][1]);
    ShapeClass2Omega3[i][0] = powf(20.0 * ((a*a*a) * b) / (c * powf(d, 5.0 / 3.0)), 3) / (2000.0 * M_PI * M_PI / 9.0);
    Nvaluedist = fabsf(omega3 - ShapeClass2Omega3[i][0]);
    if (Nvaluedist < bestNvaluedist)
    {
      bestNvaluedist = Nvaluedist;
      Nvalue = ShapeClass2Omega3[i][1];
    }
  }
  float beta1 = (gamma((1.0 / Nvalue))*gamma((1.0/Nvalue)))/gamma((2.0/Nvalue));
  float beta2 = (gamma((2.0 / Nvalue))*gamma((1.0/Nvalue)))/gamma((3.0/Nvalue));
  radcur1 = (volcur * (3.0 / 2.0) * (1.0 / bovera) * (1.0 / covera) * ((Nvalue * Nvalue) / 4.0) * (1.0 / beta1) * (1.0 / beta2));
  radcur1 = powf(radcur1, 0.333333333333);
  return radcur1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SuperEllipsoidOps::inside(float axis1comp, float axis2comp, float axis3comp)
{
  int inside = 1;
  axis1comp = fabs(axis1comp);
  axis2comp = fabs(axis2comp);
  axis3comp = fabs(axis3comp);
  axis1comp = powf(axis1comp, Nvalue);
  axis2comp = powf(axis2comp, Nvalue);
  axis3comp = powf(axis3comp, Nvalue);
  inside = 1 - axis1comp - axis2comp - axis3comp;
  return inside;
}
