

//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//



//
//  Macro for casting. So that c-style casts are easy to find.
//


#ifndef _USUL_CAST_H_
#define _USUL_CAST_H_



//
//  Macro for unsafe casting. Makes it easier to search for these.
//


#define USUL_UNSAFE_CAST(to_type,from_object)\
  ( (to_type) (from_object) )



//
//  Macro for safe casting of FOX pointer.
//


#define SAFE_CAST_FOX(Type,pointer)\
  ( ( (pointer) && (pointer)->isMemberOf ( FXMETACLASS ( Type ) ) ) ? reinterpret_cast < Type * > ( pointer ) : 0x0 )



//
//  Macro for safe casting of const FOX pointer.
//


#define SAFE_CAST_FOX_CONST(Type,pointer)\
  ( ( (pointer) && (pointer)->isMemberOf ( FXMETACLASS ( Type ) ) ) ? reinterpret_cast < const Type * > ( pointer ) : 0x0 )


#endif // _USUL_CAST_H_
