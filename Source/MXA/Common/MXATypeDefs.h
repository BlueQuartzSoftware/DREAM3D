
//
//  Copyright (c) 2007, 2010 Michael A. Jackson for BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//

#ifndef _MXATYPEDEFS_H__
#define _MXATYPEDEFS_H__


#include <string>

//-- Boost Includes
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_array.hpp>

#include "MXA/MXA.h"


/** @file MXATypeDefs.h
 * @brief This file contains many common typdefs used through out the MXA Source code
 */

#if defined (MXA_USE_SHARED_STRINGS)
typedef boost::shared_ptr<std::string>    MXAString;
#else
typedef std::string MXAString;
#endif

typedef boost::shared_ptr<std::string>   MXASharedString;

//-- Define some Shared Arrays to use
typedef boost::shared_array<uint8_t>                 UCharArray;
typedef boost::shared_array<int8_t>                  CharArray;
typedef boost::shared_array<uint16_t>                UShortArray;
typedef boost::shared_array<int16_t>                 ShortArray;
typedef boost::shared_array<uint32_t>                UIntArray;
typedef boost::shared_array<int32_t>                 IntArray;
typedef boost::shared_array<uint64_t>                ULongLongArray;
typedef boost::shared_array<int64_t>                 LongLongArray;


#define NEW_UCHAR_ARRAY(VarName, Size)\
  UCharArray VarName(new uint8_t[Size]);

/* If we are compiling WITHOUT HDF5 support then we need some enumerated types */
#if  MXA_HDF5_SUPPORT == 0

#define H5T_NATIVE_INT8 1
#define H5T_NATIVE_INT8  1
#define H5T_NATIVE_UINT8  2
#define H5T_NATIVE_INT16  3
#define H5T_NATIVE_UINT16  4
#define H5T_NATIVE_INT32  5
#define H5T_NATIVE_UINT32  6
#define H5T_NATIVE_INT64  7
#define H5T_NATIVE_UINT64  8
#define H5T_NATIVE_FLOAT  9
#define H5T_NATIVE_DOUBLE  10
#define H5T_STRING  11


namespace H5Lite
{



/**
 *
 * @param value
 * @return
 */
template<typename T>
 MXA_EXPORT std::string HDFTypeForPrimitiveAsStr(T value)
{
  if (typeid(value) == typeid(int8_t)) return "H5T_NATIVE_INT8";
  if (typeid(value) == typeid(uint8_t)) return "H5T_NATIVE_UINT8";

  if (typeid(value) == typeid(int16_t)) return "H5T_NATIVE_INT16";
  if (typeid(value) == typeid(uint16_t)) return "H5T_NATIVE_UINT16";

  if (typeid(value) == typeid(int32_t)) return "H5T_NATIVE_INT32";
  if (typeid(value) == typeid(uint32_t)) return "H5T_NATIVE_UINT32";

  if (typeid(value) == typeid(int64_t)) return "H5T_NATIVE_INT64";
  if (typeid(value) == typeid(uint64_t)) return "H5T_NATIVE_UINT64";

  if (typeid(value) == typeid(float32)) return "H5T_NATIVE_FLOAT";
  if (typeid(value) == typeid(float64)) return "H5T_NATIVE_DOUBLE";

  //if (typeid(value) == typeid(bool)) return "H5T_NATIVE_UINT8";

  std::cout << DEBUG_OUT(logTime) << "Error: HDFTypeForPrimitiveAsStr - Unknown Type: " << typeid(value).name() << std::endl;
  return "";
}

/**
 *
 * @param value
 * @return
 */
template<typename T>
 MXA_EXPORT int32_t HDFTypeForPrimitive(T value)
{
  if (typeid(value) == typeid(float32)) return H5T_NATIVE_FLOAT;
  if (typeid(value) == typeid(float64)) return H5T_NATIVE_DOUBLE;

  if (typeid(value) == typeid(int8_t)) return H5T_NATIVE_INT8;
  if (typeid(value) == typeid(uint8_t)) return H5T_NATIVE_UINT8;

  if (typeid(value) == typeid(int16_t)) return H5T_NATIVE_INT16;
  if (typeid(value) == typeid(uint16_t)) return H5T_NATIVE_UINT16;

  if (typeid(value) == typeid(int32_t)) return H5T_NATIVE_INT32;
  if (typeid(value) == typeid(uint32_t)) return H5T_NATIVE_UINT32;

  if (typeid(value) == typeid(int64_t)) return H5T_NATIVE_INT64;
  if (typeid(value) == typeid(uint64_t)) return H5T_NATIVE_UINT64;

  if (typeid(value) == typeid(bool)) return H5T_NATIVE_UINT8;

  return -1;
}

static int32_t HDFTypeFromString(const std::string &value)
{
  if (value.compare("H5T_NATIVE_INT8") == 0) return H5T_NATIVE_INT8;
  if (value.compare("H5T_NATIVE_UINT8") == 0) return H5T_NATIVE_UINT8;

  if (value.compare("H5T_NATIVE_INT16") == 0) return H5T_NATIVE_INT16;
  if (value.compare("H5T_NATIVE_UINT16") == 0) return H5T_NATIVE_UINT16;

  if (value.compare("H5T_NATIVE_INT32") == 0) return H5T_NATIVE_INT32;
  if (value.compare("H5T_NATIVE_UINT32") == 0) return H5T_NATIVE_UINT32;

  if (value.compare("H5T_NATIVE_INT64") == 0) return H5T_NATIVE_INT64;
  if (value.compare("H5T_NATIVE_UINT64") == 0) return H5T_NATIVE_UINT64;

  if (value.compare("H5T_NATIVE_FLOAT") == 0) return H5T_NATIVE_FLOAT;
  if (value.compare("H5T_NATIVE_DOUBLE") == 0) return H5T_NATIVE_DOUBLE;

  if (value.compare("H5T_STRING") == 0) return H5T_STRING;

  std::cout << DEBUG_OUT(logTime) << "Error: HDFTypeFromString - Unknown Type: " << value << std::endl;
  return -1;
}

} /* End H5Lite namespace */

#endif /* MXA_HDF5_SUPPORT == 0 */


#endif /* _MXATYPEDEFS_H__ */
