#ifndef RTTIMACROS_H_
#define RTTIMACROS_H_



//
//  Copyright (c) 2002, Perry L. Miller IV
//  Small Changes by Michael A. Jackson for the MXA Project
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//



//
//  Base class with virtual type-id member.
//




#include "MXA/MXADLLExport.h"

#include <typeinfo>


namespace MXA {
namespace RTTI {


struct MXA_EXPORT Typed
{
  static const std::type_info &     classTypeId();
  virtual const std::type_info &    typeId() const = 0;
};


}; // namespace RTTI
}; // namespace MXA



//
//  Convenience macros so that classes can easily implement typeId().
//


#define MXA_DECLARE_TYPE_ID(class_name) \
  static const std::type_info &classTypeId(); \
  virtual const std::type_info &typeId() const

#define MXA_IMPLEMENT_TYPE_ID(class_name) \
  const std::type_info &class_name::classTypeId() \
  { \
    return typeid ( class_name ); \
  } \
  const std::type_info &class_name::typeId() const \
  { \
    return class_name::classTypeId(); \
  }

#define MXA_IMPLEMENT_INLINE_TYPE_ID(class_name) \
  static const std::type_info &classTypeId() \
  { \
    return typeid ( class_name ); \
  } \
  virtual const std::type_info &typeId() const \
  { \
    return class_name::classTypeId(); \
  }




#endif /*RTTIMACROS_H_*/
