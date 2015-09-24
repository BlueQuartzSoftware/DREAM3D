/***
 * Name:  Plane.cpp
 * Purpose: m3c_basics.math.geom.Plane Implementation
 * Notice:  Copyright Stuart Golodetz, 2008. All rights reserved.
* This work is licensed under the Creative Commons Attribution-NonCommercial 3.0
* Unported License. To view a copy of this license, visit
* http://creativecommons.org/licenses/by-nc/3.0/
* or send a letter to Creative Commons,
* 171 Second Street, Suite 300,
* San Francisco, California, 94105, USA.
 ***/

#include "Plane.h"

#include "SIMPLib/Math/SIMPLibMath.h"

#include "SurfaceMeshing/SurfaceMeshingFilters/util/InvalidParameterException.h"

namespace PlaneDetail
{

//double EPSILON = 0.0001;
  double   EPSILON = 0.0001f;
  double PI = 3.141592654;
  double SMALL_EPSILON = 0.0000000001;

}

using namespace m3c_basics::exceptions;
using namespace PlaneDetail;
//using namespace m3c_basics::math_vectors;



//####################RUCTORS ####################
Plane::Plane(VectorType& normal, double d)
  : m_normal(normal), m_d(d)
{
  ensure_invariant();
}

Plane::Plane(VectorType& normal, VectorType& x)
  : m_normal(normal), m_center(x), m_d(normal.dot(x))
{
  ensure_invariant();
}

Plane::Plane(VectorType& normal, float* x) :
  m_normal(normal),
  m_center(x),
  m_d(normal.dot(x))
{
  ensure_invariant();
}

//#################### PUBLIC METHODS ####################
Plane::CP Plane::classify_point(VectorType& p)
{
  double value = m_normal.dot(p) - m_d;

  if (fabs(value) < EPSILON)
  {
    return CP_COPLANAR;
  }
  else if (value > 0)
  {
    return CP_FRONT;
  }

  return CP_BACK;
}

double Plane::distance_to_point(VectorType& p)
{
  // Note that this equation is valid precisely because the stored normal is unit length.
  return fabs(m_normal.dot(p) - m_d);
}

double Plane::distance_value()
{
  return m_d;
}

VectorType& Plane::normal()
{
  return m_normal;
}

//#################### PRIVATE METHODS ####################
void Plane::ensure_invariant()
{
  double length = m_normal.length();
  if(length > SMALL_EPSILON)
  {
    m_normal /= length;
    m_d /= length;
  }
  else { throw InvalidParameterException("The plane's normal must be non-zero"); }
}

//#################### GLOBAL OPERATORS ####################
std::ostream& operator<<(std::ostream& os, Plane::CP rhs)
{
  switch(rhs)
  {
    case Plane::CP_BACK:
      os << "CP_BACK";
      break;
    case Plane::CP_COPLANAR:
      os << "CP_COPLANAR";
      break;
    case Plane::CP_FRONT:
      os << "CP_FRONT";
      break;
    case Plane::CP_STRADDLE:
      os << "CP_STRADDLE";
      break;
  }
  return os;
}

