/* ============================================================================
 * Copyright (c) 2014 DREAM3D Consortium
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the names of any of the DREAM3D Consortium contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was partially written under United States Air Force Contract number
 *                              FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _TemplateUtilities_H_
#define _TemplateUtilities_H_

#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/DataArrayPath.h"

namespace TemplateConstants
{

  enum Types
  {
    UnknownType = 0,
    Bool,
    Int8,
    UInt8,
    Int16,
    UInt16,
    Int32,
    UInt32,
    Int64,
    UInt64,
    Float,
    Double
  };

}


  class TemplateUtilities
  {
    public:
      TemplateUtilities() {}
      virtual ~TemplateUtilities();

      static QString getTypeNameFromType(int type)
      {
        if(TemplateConstants::Bool == type)
        {
          return DREAM3D::TypeNames::Bool;
        }
        else if(TemplateConstants::Int8 == type)
        {
          return DREAM3D::TypeNames::Int8;
        }
        else if(TemplateConstants::UInt8 == type)
        {
          return DREAM3D::TypeNames::UInt8;
        }
        else if(TemplateConstants::Int16 == type)
        {
          return DREAM3D::TypeNames::Int16;
        }
        else if(TemplateConstants::UInt16 == type)
        {
          return DREAM3D::TypeNames::UInt16;
        }
        else if(TemplateConstants::Int32 == type)
        {
          return DREAM3D::TypeNames::Int32;
        }
        else if(TemplateConstants::UInt32 == type)
        {
          return DREAM3D::TypeNames::UInt32;
        }
        else if(TemplateConstants::Int64 == type)
        {
          return DREAM3D::TypeNames::Int64;
        }
        else if(TemplateConstants::UInt64 == type)
        {
          return DREAM3D::TypeNames::UInt64;
        }
        else if(TemplateConstants::Float == type)
        {
          return DREAM3D::TypeNames::Float;
        }
        else if(TemplateConstants::Double == type)
        {
          return DREAM3D::TypeNames::Double;
        }
        else
        {
          return DREAM3D::TypeNames::UnknownType;
        }
      }

      static int getTypeFromTypeName(QString type)
      {
        if(0 == type.compare(DREAM3D::TypeNames::Bool))
        {
          return TemplateConstants::Bool;
        }
        else if(0 == type.compare(DREAM3D::TypeNames::Int8))
        {
          return TemplateConstants::Int8;
        }
        else if(0 == type.compare(DREAM3D::TypeNames::UInt8))
        {
          return TemplateConstants::UInt8;
        }
        else if(0 == type.compare(DREAM3D::TypeNames::Int16))
        {
          return TemplateConstants::Int16;
        }
        else if(0 == type.compare(DREAM3D::TypeNames::UInt16))
        {
          return TemplateConstants::UInt16;
        }
        else if(0 == type.compare(DREAM3D::TypeNames::Int32))
        {
          return TemplateConstants::Int32;
        }
        else if(0 == type.compare(DREAM3D::TypeNames::UInt32))
        {
          return TemplateConstants::UInt32;
        }
        else if(0 == type.compare(DREAM3D::TypeNames::Int64))
        {
          return TemplateConstants::Int64;
        }
        else if(0 == type.compare(DREAM3D::TypeNames::UInt64))
        {
          return TemplateConstants::UInt64;
        }
        else if(0 == type.compare(DREAM3D::TypeNames::Float))
        {
          return TemplateConstants::Float;
        }
        else if(0 == type.compare(DREAM3D::TypeNames::Double))
        {
          return TemplateConstants::Double;
        }
        else
        {
          return TemplateConstants::UnknownType;
        }
      }

    private:
      TemplateUtilities(const TemplateUtilities&); // Copy Constructor Not Implemented
      void operator=(const TemplateUtilities&); // Operator '=' Not Implemented
  };



#endif /* _TemplateUtilities_H_ */
