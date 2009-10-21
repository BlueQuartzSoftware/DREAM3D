/*
 * AIMRandomNG.h
 *
 *  Created on: Oct 21, 2009
 *      Author: mjackson
 */

#ifndef AIMRANDOMNG_H_
#define AIMRANDOMNG_H_

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
