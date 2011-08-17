/*
 * SuperEllipsoidOps.h
 *
 *  Created on: Jul 18, 2011
 *      Author: mjackson
 */

#ifndef SUPERELLIPSOIDOPS_H_
#define SUPERELLIPSOIDOPS_H_

#include "ShapeOps.h"
#include "MXA/Common/MXASetGetMacros.h"

namespace DREAM3D
{

  /*
   *
   */
  class DREAM3DLib_EXPORT SuperEllipsoidOps : public ShapeOps
  {
    public:
      MXA_SHARED_POINTERS(SuperEllipsoidOps)
      MXA_TYPE_MACRO(SuperEllipsoidOps)
      MXA_STATIC_NEW_MACRO(SuperEllipsoidOps)

      virtual ~SuperEllipsoidOps();

      virtual float radcur1(std::map<ArgName, float> args);

      virtual int inside(float axis1comp, float axis2comp, float axis3comp);
      virtual void init() { Nvalue = 0.0f; }

    protected:
      SuperEllipsoidOps();
    private:
      float Nvalue;

      SuperEllipsoidOps(const SuperEllipsoidOps&); // Copy Constructor Not Implemented
      void operator=(const SuperEllipsoidOps&); // Operator '=' Not Implemented
  };

} /* namespace AIM */
#endif /* SUPERELLIPSOIDOPS_H_ */
