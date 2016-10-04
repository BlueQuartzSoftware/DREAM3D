///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

#include <string>

#include "EMMPMLib/Common/EMTime.h"
#include "EMMPMLib/Common/MSVCDefines.h"

static char em_log_time[128];
static char em_tif_time[128];

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char* formattedTime(const char* formatting, char* output)
{
  struct tm* t;
#ifdef _MSC_VER
#if _MSC_VER >= 1400
  struct tm time;
  errno_t tError;
#endif
#endif
  TimeType long_time;
  TimeFunc(&long_time);
  t = nullptr;
#ifdef _MSC_VER
#if _MSC_VER < 1400
  t = _localtime64(&long_time);
#else
  t = &time;
  tError = _localtime64_s(&time, &long_time);
#endif
#else // Non windows platforms
  t = localtime(&long_time);
#endif
  memset(output, 0, 128);
  snprintf(output, 128, formatting, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
  return output;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char* EMMPM_LogTime()
{
  std::string formatString("[%04d:%02d:%02d:%02d:%02d:%02d]");
  return formattedTime(formatString.c_str(), em_log_time);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
char* EMMPM_TiffDateTime()
{
  std::string formatString("%04d:%02d:%02d %02d:%02d:%02d");

  return formattedTime(formatString.c_str(), em_tif_time);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned long long int EMMPM_getMilliSeconds()
{
#ifdef _MSC_VER
  return (unsigned long long int)(clock());
#else
  struct timeval t1;
  CMP_GET_TIME_OF_DAY(&t1, nullptr);
  unsigned long long int seconds = t1.tv_sec;
  unsigned long long int microSec = t1.tv_usec;
  seconds *= 1000;
  microSec /= 1000;
  return seconds + microSec; // Both in milliseconds at this point.
#endif
}
