/***
 * Name:  Plane.h
 * Purpose: m3c_basics.math.geom.Plane Interface
 * Notice:  Copyright Stuart Golodetz, 2008. All rights reserved.
* This work is licensed under the Creative Commons Attribution-NonCommercial 3.0
* Unported License. To view a copy of this license, visit
* http://creativecommons.org/licenses/by-nc/3.0/
* or send a letter to Creative Commons,
* 171 Second Street, Suite 300,
* San Francisco, California, 94105, USA.
 ***/

#ifndef _plane_h_
#define _plane_h_

#include <ostream>

#include "SurfaceMeshing/SurfaceMeshingFilters/util/Vector3.h"



/**
Represents planes of the form ax + by + cz - d = 0, i.e. n . x - d = 0.

Datatype invariant: |n| = 1
*/
class Plane
{
    //#################### ENUMERATIONS ####################
  public:
    enum CP       // classification against a plane
    {
      CP_BACK,
      CP_COPLANAR,
      CP_FRONT,
      CP_STRADDLE   // entities with extent (e.g. polygons) only
    };

    //#################### TYPEDEFS ####################
  private:
//  typedef m3c_basics::math_vectors::VectorType VectorType;

    //#################### PRIVATE VARIABLES ####################
  private:
    VectorType m_normal;
    VectorType m_center;
    double m_d;

    //####################RUCTORS ####################
  public:
    Plane(VectorType& normal, double d);
    Plane(VectorType& normal, VectorType& x);
    Plane(VectorType& normal, float* x);

    //#################### PUBLIC METHODS ####################
  public:
    CP classify_point(VectorType& p);
    double distance_to_point(VectorType& p);
    double distance_value();
    VectorType& normal();

    //#################### PRIVATE METHODS ####################
  private:
    void ensure_invariant();
};

//#################### GLOBAL OPERATORS ####################
//std::ostream& operator<<(std::ostream& os, Plane::CP rhs);



#endif
