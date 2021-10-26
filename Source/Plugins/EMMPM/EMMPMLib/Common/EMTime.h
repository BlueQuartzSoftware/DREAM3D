///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EMMPMLib/EMMPMLib.h"

#include <time.h>


#ifdef _MSC_VER
#if _MSC_VER < 1400
#define TimeType __time64_t
#define TimeFunc _time64
#else
#define TimeType __time64_t
#define TimeFunc _time64
#endif
#else
#define TimeType time_t
#define TimeFunc time
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief Returns a Formatted String of the current Date/Time for logging
* purpose.
* @return A string of the current date/time
*/
EMMPMLib_EXPORT char* EMMPM_LogTime();

/**
 * Returns a date/time string suitable for tiff tags.
 * @return
 */
EMMPMLib_EXPORT char* EMMPM_TiffDateTime() ;

/**
 * @brief returns the number of milliseconds from a platform specified time.
 */
EMMPMLib_EXPORT unsigned long long int EMMPM_getMilliSeconds();


#ifdef __cplusplus
}
#endif



