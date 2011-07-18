/*
 * EllipsoidOps.cpp
 *
 *  Created on: Jul 18, 2011
 *      Author: mjackson
 */

#include "EllipsoidOps.h"

#include <math.h>


using namespace DREAM3D;

const static float m_pi = M_PI;
const static float m_one_over_pi = 1.0/m_pi;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EllipsoidOps::EllipsoidOps()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EllipsoidOps::~EllipsoidOps()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float EllipsoidOps::radcur1(std::map<ArgName, float> args)
{
  float radcur1 = 0.0f;

  float volcur = args[VolCur];
  float bovera = args[B_OverA];
  float covera = args[C_OverA];

  radcur1 = (volcur * 0.75f * (m_one_over_pi) * (1.0 / bovera) * (1.0 / covera));
  radcur1 = powf(radcur1, 0.333333333333);
  return radcur1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EllipsoidOps::inside(float axis1comp, float axis2comp, float axis3comp)
{
  int inside = 1;
  axis1comp = fabs(axis1comp);
  axis2comp = fabs(axis2comp);
  axis3comp = fabs(axis3comp);
  axis1comp = axis1comp*axis1comp;
  axis2comp = axis2comp*axis2comp;
  axis3comp = axis3comp*axis3comp;
  inside = 1 - axis1comp - axis2comp - axis3comp;
  return inside;
}
