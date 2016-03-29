/***
 * File:  InvalidParameterException.h
 * Purpose: m3c_basics.exceptions.InvalidParameterException Implementation
 * Notice:  Copyright Stuart Golodetz, 2008. All rights reserved.
* This work is licensed under the Creative Commons Attribution-NonCommercial 3.0
* Unported License. To view a copy of this license, visit
* http://creativecommons.org/licenses/by-nc/3.0/
* or send a letter to Creative Commons,
* 171 Second Street, Suite 300,
* San Francisco, California, 94105, USA.
 ***/

#ifndef _invalidparameterexception_h_
#define _invalidparameterexception_h_

#include "Exception.h"

namespace m3c_basics
{
  namespace exceptions
  {

    class InvalidParameterException : public Exception
    {
        //#################### CONSTRUCTORS ####################
      public:
        explicit InvalidParameterException(const QString& message)
          : Exception("Invalid Parameter: " + message)
        {}
    };

  }
}

#endif
