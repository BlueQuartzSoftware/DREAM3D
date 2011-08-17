/*
 * CylinderOps.h
 *
 *  Created on: Jul 18, 2011
 *      Author: mjackson
 */

#ifndef CYLINDEROPS_H_
#define CYLINDEROPS_H_


#include "ShapeOps.h"
#include "MXA/Common/MXASetGetMacros.h"

namespace DREAM3D
{

  /*
   *
   */
  class DREAM3DLib_EXPORT CylinderOps : public ShapeOps
  {
    public:
      MXA_SHARED_POINTERS(CylinderOps)
      MXA_TYPE_MACRO(CylinderOps)
      MXA_STATIC_NEW_MACRO(CylinderOps)


      virtual ~CylinderOps();

      virtual float radcur1(std::map<ArgName, float> args);
      virtual float inside(float axis1comp, float axis2comp, float axis3comp);
      virtual void init() {  }

    protected:
      CylinderOps();
    private:
      CylinderOps(const CylinderOps&); // Copy Constructor Not Implemented
      void operator=(const CylinderOps&); // Operator '=' Not Implemented

  };

} /* namespace AIM */
#endif /* CYLINDEROPS_H_ */
