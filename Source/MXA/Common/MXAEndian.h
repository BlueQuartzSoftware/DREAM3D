///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  For reversing bytes and bits.
//
///////////////////////////////////////////////////////////////////////////////

/*
 * Example usage:
 * int32 value;
 *  ReadFromSomeFile(...., value);
 *  MXA::Endian::FromLittleToSystem::convert(value);
 */

#ifndef MXAENDIAN_H_
#define MXAENDIAN_H_


#include "MXATypes.h"

#include <iosfwd>
#include <string>

//-- Needed for the memcpy function
#include <string.h>


///////////////////////////////////////////////////////////////////////////////
//
//  For 1025, which, in binary is:
//  00000000 00000000 00000100 00000001
//  Address  Big-Endian  Little-Endian
//  00       00000000    00000001
//  01       00000000    00000100
//  02       00000100    00000000
//  03       00000001    00000000
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Compile-time detection of byte order. See boost/detail/endian.hpp and
//  http://www.unixpapa.com/incnote/byteorder.html
//
///////////////////////////////////////////////////////////////////////////////

// GNU libc offers the helpful header <endian.h> which defines
// __BYTE_ORDER

#if defined (__GLIBC__)
 #include <endian.h>
 #if (__BYTE_ORDER == __LITTLE_ENDIAN)
    #define MXA_LITTLE_ENDIAN
 #elif (__BYTE_ORDER == __BIG_ENDIAN)
    #define MXA_BIG_ENDIAN
 #else
  #error Unknown machine endianness detected.
 #endif /* Not GCC */
#elif defined ( __sparc     ) || \
      defined ( __sparc__   ) || \
      defined ( __powerpc__ ) || \
      defined ( __ppc__     ) || \
      defined ( __hppa      ) || \
      defined ( _MIPSEB     ) || \
      defined ( _POWER      ) || \
      defined ( __BIG_ENDIAN__)
 #define MXA_BIG_ENDIAN
#elif defined ( __i386__    ) || \
      defined ( __alpha__   ) || \
      defined ( __ia64      ) || \
      defined ( __ia64__    ) || \
      defined ( i386        ) || \
      defined ( __i386__    ) || \
      defined ( _M_IX86     ) || \
      defined ( vax         ) || \
      defined ( __alpha     ) || \
      defined ( __LITTLE_ENDIAN__ ) || \
	  defined (_M_X64       ) || \
	  defined (_M_IA64      ) || \
	  defined (__x86_64     )
 #define MXA_LITTLE_ENDIAN
#else
 #error Unknown endian type in MXA/Endian.h
#endif


namespace MXA {
namespace Endian {


const char BIGE[4] = { 'B', 'I', 'G', 'E' };
const char LITE[4] = { 'L', 'I', 'T', 'E'};
const std::string BIGE_STR ("BIGE");
const std::string LITE_STR ("LITE");
///////////////////////////////////////////////////////////////////////////////
//
//  Run-time checking for big endian.
//
///////////////////////////////////////////////////////////////////////////////

inline bool isBig()
{
  unsigned long u = 1;
  return ( 0 == ( *( (char *) &u ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run-time checking for little endian.
//
///////////////////////////////////////////////////////////////////////////////

inline bool isLittle()
{
  return ( false == MXA::Endian::isBig() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Details.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail {

///////////////////////////////////////////////////////////////////////////////
//
//  Reverse the bytes of the integer.
//
///////////////////////////////////////////////////////////////////////////////

inline void _reverseBytes ( uint16 &n )
{
  n = ( ((((uint16)n)>>8) & 0x00FF) |
        ((((uint16)n)<<8) & 0xFF00) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reverse the bytes of the float.
//
///////////////////////////////////////////////////////////////////////////////

inline void _reverseBytes ( float32 &n )
{
  n = static_cast < float32 >
      ( ((((uint32)n)<<24) & 0xFF000000) |
        ((((uint32)n)<< 8) & 0x00FF0000) |
        ((((uint32)n)>> 8) & 0x0000FF00) |
        ((((uint32)n)>>24) & 0x000000FF) );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Reverse the bytes of the integer.
//
//////////////////////////////////////////////////////////////////////////

inline void _reverseBytes ( uint32 &n )
{
  n = ( ((((uint32)n)<<24) & 0xFF000000) |
        ((((uint32)n)<< 8) & 0x00FF0000) |
        ((((uint32)n)>> 8) & 0x0000FF00) |
        ((((uint32)n)>>24) & 0x000000FF) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reverse the bytes of the integer.
//
///////////////////////////////////////////////////////////////////////////////

inline void _reverseBytes ( uint64 &n )
{
  typedef uint64 UInt64;

 n = ( ((((UInt64)n)<<56) & 0xFF00000000000000ULL) |
       ((((UInt64)n)<<40) & 0x00FF000000000000ULL) |
       ((((UInt64)n)<<24) & 0x0000FF0000000000ULL) |
       ((((UInt64)n)<< 8) & 0x000000FF00000000ULL) |
       ((((UInt64)n)>> 8) & 0x00000000FF000000ULL) |
       ((((UInt64)n)>>24) & 0x0000000000FF0000ULL) |
       ((((UInt64)n)>>40) & 0x000000000000FF00ULL) |
       ((((UInt64)n)>>56) & 0x00000000000000FFULL) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Template definition and specialization for reversing the bytes.
//
///////////////////////////////////////////////////////////////////////////////

template < uint32 Size > struct ReverseBytes;
template <> struct ReverseBytes < sizeof ( uint16 ) >
{
  void operator () ( uint16 &n ) const
  {
    MXA::Endian::Detail::_reverseBytes ( n );
  }
  void operator () ( int16 &n ) const
  {
    MXA::Endian::Detail::_reverseBytes ( (uint16&)n );
  }
};
template <> struct ReverseBytes < sizeof ( uint32 ) >
{
  void operator () ( uint32 &n ) const
  {
    MXA::Endian::Detail::_reverseBytes ( n );
  }
  void operator () ( int32 &n ) const
  {
    MXA::Endian::Detail::_reverseBytes ( (uint32&)n  );
  }
  void operator () ( float32 &n ) const
  {
    MXA::Endian::Detail::_reverseBytes ( ( uint32&)n );
  }
};
template <> struct ReverseBytes < sizeof ( uint64 ) >
{
  void operator () ( uint64 &n ) const
  {
    MXA::Endian::Detail::_reverseBytes ( n );
  }
  void operator () ( int64 &n ) const
  {
    MXA::Endian::Detail::_reverseBytes ( (uint64&)n );
  }
  void operator () ( float64 &n ) const
  {
    MXA::Endian::Detail::_reverseBytes ( (uint64&)n );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  End of details.
//
///////////////////////////////////////////////////////////////////////////////

};


///////////////////////////////////////////////////////////////////////////////
//
//  Reverse the bytes of the data.
//
///////////////////////////////////////////////////////////////////////////////

template < class Type >
inline void reverseBytes ( Type &n )
{
  typedef MXA::Endian::Detail::ReverseBytes < sizeof ( Type ) > ReverseBytes;
  ReverseBytes () ( n );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the bytes.
//
///////////////////////////////////////////////////////////////////////////////

struct ByteSwapper
{
 template < class T > static void convert ( T &t )
  {
    MXA::Endian::reverseBytes ( t );
  }
  template < class T > void operator () ( T &t )
  {
    this->convert ( t );
  }

};

///////////////////////////////////////////////////////////////////////////////
//
//  Convert the bytes.
//
///////////////////////////////////////////////////////////////////////////////

struct FromSystemToBig
{
  template < class T > static void convert ( T &t )
  {
    #ifdef MXA_LITTLE_ENDIAN
    MXA::Endian::reverseBytes ( t );
    #endif
  }
  template < class T > void operator () ( T &t )
  {
    this->convert ( t );
  }
  void getEndianType(char* type) {
    ::memcpy(type, MXA::Endian::BIGE, 4);
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the bytes.
//
///////////////////////////////////////////////////////////////////////////////

struct FromSystemToLittle
{
  template < class T > static void convert ( T &t )
  {
    #ifdef MXA_BIG_ENDIAN
    MXA::Endian::reverseBytes ( t );
    #endif
  }
  template < class T > void operator () ( T &t )
  {
    this->convert ( t );
  }
  void getEndianType(char* type) {
    ::memcpy(type, MXA::Endian::LITE, 4);
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the bytes.
//
///////////////////////////////////////////////////////////////////////////////

struct FromBigToSystem
{
  template < class T > static void convert ( T &t )
  {
    #ifdef MXA_LITTLE_ENDIAN
    MXA::Endian::reverseBytes ( t );
    #endif
  }
  template < class T > void operator () ( T &t )
  {
    this->convert ( t );
  }
  void getEndianType(char* type) {
    ::memcpy(type, MXA::Endian::BIGE, 4);
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the bytes.
//
///////////////////////////////////////////////////////////////////////////////

struct FromLittleToSystem
{
  template < class T > static void convert ( T &t )
  {
    #ifdef MXA_BIG_ENDIAN
    MXA::Endian::reverseBytes ( t );
    #endif
  }
  template < class T > void operator () ( T &t )
  {
    this->convert ( t );
  }
  void getEndianType(char* type) {
    ::memcpy(type, MXA::Endian::LITE, 4);
  }
};




} // namespace Endian
} // namespace MXA




#endif /*MXAENDIAN_H_*/
