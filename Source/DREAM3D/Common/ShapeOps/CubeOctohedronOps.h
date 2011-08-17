/*
 * CubOctohedronOps.h
 *
 *  Created on: Jul 18, 2011
 *      Author: mjackson
 */

#ifndef CUBOCTOHEDRONOPS_H_
#define CUBOCTOHEDRONOPS_H_

#include "ShapeOps.h"
#include "MXA/Common/MXASetGetMacros.h"

namespace DREAM3D
{

  /*
   *
   */
  class DREAM3DLib_EXPORT CubeOctohedronOps : public ShapeOps
  {
    public:
      MXA_SHARED_POINTERS(CubeOctohedronOps)
      MXA_TYPE_MACRO(CubeOctohedronOps)
      MXA_STATIC_NEW_MACRO(CubeOctohedronOps)

      virtual ~CubeOctohedronOps();

      virtual float radcur1(std::map<ArgName, float> args);

      virtual int inside(float axis1comp, float axis2comp, float axis3comp);
      virtual void init() { Gvalue = 0.0f; }

    protected:
      CubeOctohedronOps();
    private:
      float Gvalue;

      CubeOctohedronOps(const CubeOctohedronOps&); // Copy Constructor Not Implemented
      void operator=(const CubeOctohedronOps&); // Operator '=' Not Implemented
  };

} /* namespace AIM */
#endif /* CUBOCTOHEDRONOPS_H_ */
