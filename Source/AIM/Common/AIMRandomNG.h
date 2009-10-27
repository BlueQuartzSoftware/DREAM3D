/*
 * AIMRandomNG.h
 *
 *  Created on: Oct 21, 2009
 *      Author: mjackson
 */

#ifndef AIMRANDOMNG_H_
#define AIMRANDOMNG_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


#include <MXA/Common/MXATypes.h>
/*
 *
 */
class AIMRandomNG
{
  public:
    AIMRandomNG();
    virtual ~AIMRandomNG();

    void RandomInit(uint32);
    double Random();
    double x[5];

  private:
    AIMRandomNG(const AIMRandomNG&);    // Copy Constructor Not Implemented
      void operator=(const AIMRandomNG&);  // Operator '=' Not Implemented
};

#endif /* AIMRANDOMNG_H_ */
