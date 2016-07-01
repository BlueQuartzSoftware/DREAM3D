/***
 * File:  Exception.h
 * Purpose: m3c_basics.exceptions.Exception Implementation
 * Notice:  Copyright Stuart Golodetz, 2008. All rights reserved.
* This work is licensed under the Creative Commons Attribution-NonCommercial 3.0
* Unported License. To view a copy of this license, visit
* http://creativecommons.org/licenses/by-nc/3.0/
* or send a letter to Creative Commons,
* 171 Second Street, Suite 300,
* San Francisco, California, 94105, USA.
 ***/

#ifndef _exception_h_
#define _exception_h_

#include <QtCore/QString>

namespace m3c_basics
{
  namespace exceptions
  {

    class Exception
    {
        //#################### PRIVATE VARIABLES ####################
      private:
        QString m_cause;

        //#################### CONSTRUCTORS ####################
      public:
        explicit Exception(const QString& cause)
          : m_cause(cause)
        {}

        //#################### DESTRUCTOR ####################
      public:
        virtual ~Exception() {}

        //#################### PUBLIC METHODS ####################
        virtual const QString& error() const
        {
          return m_cause;
        }
    };

  }
}

#endif
