/* ============================================================================
 * Copyright (c) 2015 BlueQuartz Softwae, LLC
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
 * Neither the names of any of the BlueQuartz Software contributors
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/*
 * Example usage:
 * int32_t value;
 *  ReadFromSomeFile(...., value);
 *  SIMPLib::Endian::FromLittleToSystem::convert(value);
 */

#ifndef _SIMPLibENDIAN_H_
#define _SIMPLibENDIAN_H_


//-- Needed for the memcpy function
#include <string.h>

#include <iosfwd>
#include <string>



#include "SIMPLib/SIMPLib.h"

//
//  For 1025, which, in binary is:
//  00000000 00000000 00000100 00000001
//  Address  Big-Endian  Little-Endian
//  00       00000000    00000001
//  01       00000000    00000100
//  02       00000100    00000000
//  03       00000001    00000000
//



//
//  Compile-time detection of byte order. See boost/detail/endian.hpp and
//  http://www.unixpapa.com/incnote/byteorder.html
//


// GNU libc offers the helpful header <endian.h> which defines
// __BYTE_ORDER

#if defined (__GLIBC__)
#include <endian.h>
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
#define SIMPLib_LITTLE_ENDIAN
#elif (__BYTE_ORDER == __BIG_ENDIAN)
#define SIMPLib_BIG_ENDIAN
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
#define SIMPLib_BIG_ENDIAN
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
#define SIMPLib_LITTLE_ENDIAN
#else
#error Unknown endian type in SIMPLib/Endian.h
#endif


namespace SIMPLib
{
  namespace Endian
  {


    const char BIGE[4] = { 'B', 'I', 'G', 'E' };
    const char LITE[4] = { 'L', 'I', 'T', 'E'};
    const std::string BIGE_STR ("BIGE");
    const std::string LITE_STR ("LITE");

//
//  Run-time checking for big endian.
//


    inline bool isBig()
    {
      unsigned long u = 1;
      return ( 0 == ( *( (char*) &u ) ) );
    }



//
//  Run-time checking for little endian.
//


    inline bool isLittle()
    {
      return ( false == SIMPLib::Endian::isBig() );
    }



//
//  Details.
//


    namespace Detail
    {


//
//  Reverse the bytes of the integer.
//


      inline void _reverseBytes ( uint16_t& n )
      {
        n = ( ((((uint16_t)n) >> 8) & 0x00FF) |
              ((((uint16_t)n) << 8) & 0xFF00) );
      }



//
//  Reverse the bytes of the float.
//


      inline void _reverseBytes ( float32& n )
      {
        n = static_cast < float32 >
            ( ((((uint32_t)n) << 24) & 0xFF000000) |
              ((((uint32_t)n) << 8) & 0x00FF0000) |
              ((((uint32_t)n) >> 8) & 0x0000FF00) |
              ((((uint32_t)n) >> 24) & 0x000000FF) );
      }


//
//  Reverse the bytes of the integer.
//
//////////////////////////////////////////////////////////////////////////

      inline void _reverseBytes ( uint32_t& n )
      {
        n = ( ((((uint32_t)n) << 24) & 0xFF000000) |
              ((((uint32_t)n) << 8) & 0x00FF0000) |
              ((((uint32_t)n) >> 8) & 0x0000FF00) |
              ((((uint32_t)n) >> 24) & 0x000000FF) );
      }



//
//  Reverse the bytes of the integer.
//


      inline void _reverseBytes ( uint64_t& n )
      {
        typedef uint64_t UInt64;

        n = ( ((((UInt64)n) << 56) & 0xFF00000000000000ULL) |
              ((((UInt64)n) << 40) & 0x00FF000000000000ULL) |
              ((((UInt64)n) << 24) & 0x0000FF0000000000ULL) |
              ((((UInt64)n) << 8) & 0x000000FF00000000ULL) |
              ((((UInt64)n) >> 8) & 0x00000000FF000000ULL) |
              ((((UInt64)n) >> 24) & 0x0000000000FF0000ULL) |
              ((((UInt64)n) >> 40) & 0x000000000000FF00ULL) |
              ((((UInt64)n) >> 56) & 0x00000000000000FFULL) );
      }



//
//  Template definition and specialization for reversing the bytes.
//


      template < uint32_t Size > struct ReverseBytes;
      template <> struct ReverseBytes < sizeof ( uint16_t ) >
      {
        void operator () ( uint16_t& n ) const
      {
        SIMPLib::Endian::Detail::_reverseBytes ( n );
      }
      void operator () ( int16_t& n ) const
      {
        SIMPLib::Endian::Detail::_reverseBytes ( (uint16_t&)n );
      }
      };
      template <> struct ReverseBytes < sizeof ( uint32_t ) >
      {
        void operator () ( uint32_t& n ) const
      {
        SIMPLib::Endian::Detail::_reverseBytes ( n );
      }
      void operator () ( int32_t& n ) const
      {
        SIMPLib::Endian::Detail::_reverseBytes ( (uint32_t&)n  );
      }
      void operator () ( float32& n ) const
      {
        SIMPLib::Endian::Detail::_reverseBytes ( ( uint32_t&)n );
      }
      };
      template <> struct ReverseBytes < sizeof ( uint64_t ) >
      {
        void operator () ( uint64_t& n ) const
      {
        SIMPLib::Endian::Detail::_reverseBytes ( n );
      }
      void operator () ( int64_t& n ) const
      {
        SIMPLib::Endian::Detail::_reverseBytes ( (uint64_t&)n );
      }
      void operator () ( float64& n ) const
      {
        SIMPLib::Endian::Detail::_reverseBytes ( (uint64_t&)n );
      }
      };



//
//  End of details.
//


    }



//
//  Reverse the bytes of the data.
//


    template < class Type >
    inline void reverseBytes ( Type& n )
    {
      typedef SIMPLib::Endian::Detail::ReverseBytes < sizeof ( Type ) > ReverseBytes;
      ReverseBytes () ( n );
    }



//
//  Convert the bytes.
//


    struct ByteSwapper
    {
      template < class T > static void convert ( T& t )
      {
        SIMPLib::Endian::reverseBytes ( t );
      }
      template < class T > void operator () ( T& t )
      {
        this->convert ( t );
      }

    };


//
//  Convert the bytes.
//


    struct FromSystemToBig
    {
      template < class T > static void convert ( T& t )
      {
#ifdef SIMPLib_LITTLE_ENDIAN
        SIMPLib::Endian::reverseBytes ( t );
#endif
      }
      template < class T > void operator () ( T& t )
      {
        this->convert ( t );
      }
      void getEndianType(char* type)
      {
        ::memcpy(type, SIMPLib::Endian::BIGE, 4);
      }
    };



//
//  Convert the bytes.
//


    struct FromSystemToLittle
    {
      template < class T > static void convert ( T& t )
      {
#ifdef SIMPLib_BIG_ENDIAN
        SIMPLib::Endian::reverseBytes ( t );
#endif
      }
      template < class T > void operator () ( T& t )
      {
        this->convert ( t );
      }
      void getEndianType(char* type)
      {
        ::memcpy(type, SIMPLib::Endian::LITE, 4);
      }
    };



//
//  Convert the bytes.
//


    struct FromBigToSystem
    {
      template < class T > static void convert ( T& t )
      {
#ifdef SIMPLib_LITTLE_ENDIAN
        SIMPLib::Endian::reverseBytes ( t );
#endif
      }
      template < class T > void operator () ( T& t )
      {
        this->convert ( t );
      }
      void getEndianType(char* type)
      {
        ::memcpy(type, SIMPLib::Endian::BIGE, 4);
      }
    };



//
//  Convert the bytes.
//


    struct FromLittleToSystem
    {
      template < class T > static void convert ( T& t )
      {
#ifdef SIMPLib_BIG_ENDIAN
        SIMPLib::Endian::reverseBytes ( t );
#endif
      }
      template < class T > void operator () ( T& t )
      {
        this->convert ( t );
      }
      void getEndianType(char* type)
      {
        ::memcpy(type, SIMPLib::Endian::LITE, 4);
      }
    };




  } // namespace Endian
} // namespace SIMPLib




#endif /*_SIMPLibENDIAN_H_*/
