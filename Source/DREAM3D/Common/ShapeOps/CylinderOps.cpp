/*
 * CylinderOps.cpp
 *
 *  Created on: Jul 18, 2011
 *      Author: mjackson
 */

#include "CylinderOps.h"

#include <math.h>

using namespace DREAM3D;

const static float m_pi = M_PI;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CylinderOps::CylinderOps()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CylinderOps::~CylinderOps()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float CylinderOps::radcur1(std::map<ArgName, float> args)
{
  float radcur1 = 0.0f;

  float volcur = args[VolCur];
  float bovera = args[B_OverA];
  float covera = args[C_OverA];

  radcur1 = (volcur*(1.0/m_pi)*(1.0/bovera)*(1.0/covera));
  radcur1 = powf(radcur1, 0.333333333333);
  return radcur1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CylinderOps::inside(float axis1comp, float axis2comp, float axis3comp)
{
  int inside = 1;
  if (fabs(axis1comp) <= 1)
  {
    inside = 1;
    axis2comp = fabs(axis2comp);
    axis3comp = fabs(axis3comp);
    axis2comp = axis2comp*axis2comp;
    axis3comp = axis3comp*axis3comp;
    inside = 1 - axis2comp - axis3comp;
  }
  return inside;
}
