
//
//  Copyright (c) 2007, 2010 Michael A. Jackson for BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number 
//                           FA8650-04-C-5229
//

#ifndef MXAERRORDEFINITIONS_H_
#define MXAERRORDEFINITIONS_H_


/**
 * @file MXAErrorDefinitions.h
 * @brief This file contains the error codes for errors thrown only from XFORMAT
*  If the error came from HDF5, you have to use their error routines
*/
// -----------------------------------------------------------------------------
//  File errors -50xx
// -----------------------------------------------------------------------------

 /**
  * @def MXA_ERROR_FILE_NOT_MXA 
  * @brief Definition for MXA_ERROR_FILE_NOT_XMDA 
*/
#define MXA_ERROR_FILE_NOT_MXA -5002 

 /**
  * @def MXA_ERROR_FILE_NOT_FOUND 
  * @brief Definition for MXA_ERROR_FILE_NOT_FOUND 
*/
#define MXA_ERROR_FILE_NOT_FOUND -5003

 /**
  * @def MXA_ERROR_FILE_NOT_DEFINED 
  * @brief Definition for MXA_ERROR_FILE_NOT_DEFINED 
*/
#define MXA_ERROR_FILE_NOT_DEFINED -5004 

 /**
  * @def MXA_ERROR_FILE_ZERO_SIZE 
  * @brief Definition for MXA_ERROR_FILE_ZERO_SIZE 
*/
#define MXA_ERROR_FILE_ZERO_SIZE -5005

/**
 * @def MXA_ERROR_FILE_NOT_OPENED
 * @brief Error when the HDF file identifier is not valid
 */
#define MXA_ERROR_FILE_NOT_OPENED -5006

/**
 * @def MXA_ERROR_IMAGE_FORMAT_NOT_SUPPORTED
 * @brief Error when importing an image format that is not supported
 */
#define MXA_ERROR_IMAGE_FORMAT_NOT_SUPPORTED -5007

/**
 * @def MXA_ERROR_FILE_FORMAT_NOT_SUPPORTED
 * @brief General error when parsers or importers are trying to open a file that
 * the parser does not understand
 */
#define MXA_ERROR_FILE_FORMAT_NOT_SUPPORTED -5008

/**
  * @def MXA_UNKNOWN_DATA_RECORD_TYPE 
  * @brief Definition for MXA_UNKNOWN_DATA_RECORD_TYPE 
*/
#define MXA_UNKNOWN_DATA_RECORD_TYPE -7000 

/* C++ style constants */
namespace MXA {
  const int32_t ErrorFileNotMXA = MXA_ERROR_FILE_NOT_MXA;
  const int32_t ErrorFileNotFound = MXA_ERROR_FILE_NOT_FOUND;
  const int32_t ErrorFileNotDefined = MXA_ERROR_FILE_NOT_DEFINED;
  const int32_t ErrorFileZeroSize = MXA_ERROR_FILE_ZERO_SIZE;
  const int32_t ErrorFileNotOpened = MXA_ERROR_FILE_NOT_OPENED;
  const int32_t ErrorImageFormatNotSupported = MXA_ERROR_IMAGE_FORMAT_NOT_SUPPORTED;
  const int32_t ErrorUnknownDataRecordType = MXA_UNKNOWN_DATA_RECORD_TYPE;
  const int32_t ErrorFileFormatNotSupported = MXA_ERROR_FILE_FORMAT_NOT_SUPPORTED;
};



#endif /*MXAERRORDEFINITIONS_H_*/
