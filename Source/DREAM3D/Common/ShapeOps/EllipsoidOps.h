/*
 * EllipsoidOps.h
 *
 *  Created on: Jul 18, 2011
 *      Author: mjackson
 */

#ifndef ELLIPSOIDOPS_H_
#define ELLIPSOIDOPS_H_

#include "ShapeOps.h"
#include "MXA/Common/MXASetGetMacros.h"

namespace DREAM3D
{

  /*
   *
   */
  class DREAM3DLib_EXPORT EllipsoidOps : public ShapeOps
  {
    public:
      MXA_SHARED_POINTERS(EllipsoidOps)
      MXA_TYPE_MACRO(EllipsoidOps)
      MXA_STATIC_NEW_MACRO(EllipsoidOps)

      virtual ~EllipsoidOps();

      virtual float radcur1(std::map<ArgName, float> args);
      virtual int inside(float axis1comp, float axis2comp, float axis3comp);
      virtual void init() {  }

    protected:
      EllipsoidOps();
    private:
      EllipsoidOps(const EllipsoidOps&); // Copy Constructor Not Implemented
      void operator=(const EllipsoidOps&); // Operator '=' Not Implemented

  };

} /* namespace AIM */
#endif /* ELLIPSOIDOPS_H_ */
