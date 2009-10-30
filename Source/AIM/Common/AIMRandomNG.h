///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////
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
