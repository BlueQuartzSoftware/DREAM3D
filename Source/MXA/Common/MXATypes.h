/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: MXATypes.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __MXATypes_h
#define __MXATypes_h

#include <MXA/Common/DLLExport.h>

#define MXA_READ_WRITE false
#define MXA_READ_ONLY  true

/*--------------------------------------------------------------------------*/
/* Define a casting macro for use by the constants below.  */
#if defined(__cplusplus)
# define MXA_TYPE_CAST(T, V) static_cast< T >(V)
#else
# define MXA_TYPE_CAST(T, V) ((T)(V))
#endif

/*--------------------------------------------------------------------------*/
/* Define min/max constants for each type.  */
#define MXA_BIT_MIN                 0
#define MXA_BIT_MAX                 1
#if MXA_TYPE_CHAR_IS_SIGNED
# define MXA_CHAR_MIN               MXA_TYPE_CAST(char, 0x80)
# define MXA_CHAR_MAX               MXA_TYPE_CAST(char, 0x7f)
#else
# define MXA_CHAR_MIN               MXA_TYPE_CAST(char, 0u)
# define MXA_CHAR_MAX               MXA_TYPE_CAST(char, 0xffu)
#endif
#define MXA_SIGNED_CHAR_MIN         MXA_TYPE_CAST(signed char, 0x80)
#define MXA_SIGNED_CHAR_MAX         MXA_TYPE_CAST(signed char, 0x7f)
#define MXA_UNSIGNED_CHAR_MIN       MXA_TYPE_CAST(unsigned char, 0u)
#define MXA_UNSIGNED_CHAR_MAX       MXA_TYPE_CAST(unsigned char, 0xffu)
#define MXA_SHORT_MIN               MXA_TYPE_CAST(short, 0x8000)
#define MXA_SHORT_MAX               MXA_TYPE_CAST(short, 0x7fff)
#define MXA_UNSIGNED_SHORT_MIN      MXA_TYPE_CAST(unsigned short, 0u)
#define MXA_UNSIGNED_SHORT_MAX      MXA_TYPE_CAST(unsigned short, 0xffffu)
#define MXA_INT_MIN                 MXA_TYPE_CAST(int, ~(~0u >> 1))
#define MXA_INT_MAX                 MXA_TYPE_CAST(int, ~0u >> 1)
#define MXA_UNSIGNED_INT_MIN        MXA_TYPE_CAST(unsigned int, 0)
#define MXA_UNSIGNED_INT_MAX        MXA_TYPE_CAST(unsigned int, ~0u)
#define MXA_LONG_MIN                MXA_TYPE_CAST(long, ~(~0ul >> 1))
#define MXA_LONG_MAX                MXA_TYPE_CAST(long, ~0ul >> 1)
#define MXA_UNSIGNED_LONG_MIN       MXA_TYPE_CAST(unsigned long, 0ul)
#define MXA_UNSIGNED_LONG_MAX       MXA_TYPE_CAST(unsigned long, ~0ul)
#define MXA_FLOAT_MIN               MXA_TYPE_CAST(float, -1.0e+38f)
#define MXA_FLOAT_MAX               MXA_TYPE_CAST(float,  1.0e+38f)
#define MXA_DOUBLE_MIN              MXA_TYPE_CAST(double, -1.0e+299)
#define MXA_DOUBLE_MAX              MXA_TYPE_CAST(double,  1.0e+299)
#if defined(MXA_SIZEOF_LONG_LONG)
# define MXA_LONG_LONG_MIN          MXA_TYPE_CAST(long long, ~(~0ull >> 1))
# define MXA_LONG_LONG_MAX          MXA_TYPE_CAST(long long, ~0ull >> 1)
# define MXA_UNSIGNED_LONG_LONG_MIN MXA_TYPE_CAST(unsigned long long, 0ull)
# define MXA_UNSIGNED_LONG_LONG_MAX MXA_TYPE_CAST(unsigned long long, ~0ull)
#endif
#if defined(MXA_SIZEOF___INT64)
# define MXA___INT64_MIN            MXA_TYPE_CAST(__int64, ~(~0ui64 >> 1))
# define MXA___INT64_MAX            MXA_TYPE_CAST(__int64, ~0ui64 >> 1)
# define MXA_UNSIGNED___INT64_MIN   MXA_TYPE_CAST(unsigned __int64, 0ui64)
# define MXA_UNSIGNED___INT64_MAX   MXA_TYPE_CAST(unsigned __int64, ~0ui64)
#endif

/* Define compatibility names for these constants.  */
#define MXA_LARGE_INTEGER MXA_INT_MAX
#define MXA_LARGE_FLOAT MXA_FLOAT_MAX

/*--------------------------------------------------------------------------*/
/* Define named types and constants corresponding to specific integer
   and floating-point sizes and signedness.  */

/* Select an 8-bit integer type.  */
#if MXA_SIZEOF_CHAR == 1
typedef unsigned char uint8;
typedef signed char   int8;
# define MXA_TYPE_UINT8 MXA_UNSIGNED_CHAR
# if MXA_TYPE_CHAR_IS_SIGNED
#  define MXA_TYPE_INT8 MXA_CHAR
# else
#  define MXA_TYPE_INT8 MXA_SIGNED_CHAR
# endif
#else
# error "No native data type can represent an 8-bit integer."
#endif

/* Select a 16-bit integer type.  */
#if MXA_SIZEOF_SHORT == 2
typedef unsigned short uint16;
typedef signed short   int16;
# define MXA_TYPE_UINT16 MXA_UNSIGNED_SHORT
# define MXA_TYPE_INT16 MXA_SHORT
#elif MXA_SIZEOF_INT == 2
typedef unsigned int uint16;
typedef signed int   int16;
# define MXA_TYPE_UINT16 MXA_UNSIGNED_INT
# define MXA_TYPE_INT16 MXA_INT
#else
# error "No native data type can represent a 16-bit integer."
#endif

/* Select a 32-bit integer type.  */
#if MXA_SIZEOF_INT == 4
typedef unsigned int uint32;
typedef signed int   int32;
# define MXA_TYPE_UINT32 MXA_UNSIGNED_INT
# define MXA_TYPE_INT32 MXA_INT
#elif MXA_SIZEOF_LONG == 4
typedef unsigned long uint32;
typedef signed long   int32;
# define MXA_TYPE_UINT32 MXA_UNSIGNED_LONG
# define MXA_TYPE_INT32 MXA_LONG
#else
# error "No native data type can represent a 32-bit integer."
#endif

/* Select a 64-bit integer type.  */
#if defined(MXA_TYPE_USE_LONG_LONG) && MXA_SIZEOF_LONG_LONG == 8
typedef unsigned long long uint64;
typedef signed long long   int64;
# define MXA_TYPE_UINT64 MXA_UNSIGNED_LONG_LONG
# define MXA_TYPE_INT64 MXA_LONG_LONG
#elif MXA_SIZEOF_LONG == 8
typedef unsigned long uint64;
typedef signed long   int64;
# define MXA_TYPE_UINT64 MXA_UNSIGNED_LONG
# define MXA_TYPE_INT64 MXA_LONG
#elif defined(MXA_TYPE_USE___INT64) && MXA_SIZEOF___INT64 == 8
typedef unsigned __int64 uint64;
typedef signed __int64   int64;
# define MXA_TYPE_UINT64 MXA_UNSIGNED___INT64
# define MXA_TYPE_INT64 MXA___INT64
#else
# error "No native data type can represent a 64-bit integer."
#endif

/* Select a 32-bit floating point type.  */
#if MXA_SIZEOF_FLOAT == 4
typedef float float32;
# define MXA_TYPE_FLOAT32 MXA_FLOAT
#else
# error "No native data type can represent a 32-bit floating point value."
#endif

/* Select a 64-bit floating point type.  */
#if MXA_SIZEOF_DOUBLE == 8
typedef double float64;
# define MXA_TYPE_FLOAT64 MXA_DOUBLE
#else
# error "No native data type can represent a 64-bit floating point value."
#endif

/*--------------------------------------------------------------------------*/
/* Choose an implementation for mxaIdType.  */
#if 0
#define MXA_HAS_ID_TYPE
#ifdef MXA_USE_64BIT_IDS
# if defined(MXA_SIZEOF_LONG) && MXA_SIZEOF_LONG == 8 && 0
typedef long mxaIdType;
#  define MXA_SIZEOF_ID_TYPE MXA_SIZEOF_LONG
#  define MXA_LARGE_ID MXA_LONG_MAX
# elif defined(MXA_TYPE_USE_LONG_LONG) && MXA_SIZEOF_LONG_LONG == 8
typedef long long mxaIdType;
#  define MXA_SIZEOF_ID_TYPE MXA_SIZEOF_LONG_LONG
#  define MXA_LARGE_ID MXA_LONG_LONG_MAX
# elif defined(MXA_TYPE_USE___INT64) && MXA_SIZEOF___INT64 == 8
typedef __int64 mxaIdType;
#  define MXA_SIZEOF_ID_TYPE MXA_SIZEOF___INT64
#  define MXA_LARGE_ID MXA___INT64_MAX
# else
#  error "MXA_USE_64BIT_IDS is ON but no 64-bit integer type is available."
# endif
#else
typedef int mxaIdType;
# define MXA_SIZEOF_ID_TYPE MXA_SIZEOF_INT
# define MXA_LARGE_ID MXA_INT_MAX
#endif
#endif

/*=========================================================================

  This section just defines some constants that will probably get used. This
  section was NOT originally part of the VTK project.

=========================================================================*/
#include <vector>

#define MXA_UINT8_TYPE 1
#define MXA_INT8_TYPE  2
#define MXA_UINT16_TYPE 4
#define MXA_INT16_TYPE  8
#define MXA_UINT32_TYPE 16
#define MXA_INT32_TYPE  32
#define MXA_UINT64_TYPE 64
#define MXA_INT64_TYPE  128
#define MXA_FLOAT32_TYPE 256
#define MXA_FLOAT64_TYPE  512

namespace MXATypes {

  const int8 Int8Type = 0x0F;
  const uint8 Uint8Type = 0x0F;
  const int16 Int16Type = 0x0F0F;
  const uint16 Uint16Type = 0x0F0F;
  const int32 Int32Type = 0x0F0F0F0F;
  const uint32 Uint32Type = 0x0F0F0F0F;
  const int64 Int64Type = 0x0F0F0F0F0F0F0F0Fll;
  const uint64 Uint64Type = 0x0F0F0F0F0F0F0F0Full;
  const float32 Float32Type = 0.0f;
  const float64 Float64Type = 0.0;

  typedef std::vector<int8> Int8Vector;
  typedef std::vector<uint8> Uint8Vector;
  typedef std::vector<int16> Int16Vector;
  typedef std::vector<uint16> Uint16Vector;
  typedef std::vector<int32> Int32Vector;
  typedef std::vector<uint32> Uint32Vector;
  typedef std::vector<int64> Int64Vector;
  typedef std::vector<uint64> Uint64Vector;
  typedef std::vector<float32> Float32Vector;
  typedef std::vector<float64> Float64Vector;

  typedef std::vector<uint64> H5Dimensions;

  typedef int32 MXAError;

} // namespace Types




#endif
