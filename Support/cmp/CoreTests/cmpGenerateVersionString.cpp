
//
//  Copyright (c) Mike Jackson, BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//

#ifndef _GENERATE_VERSION_STRING_H_
#define _GENERATE_VERSION_STRING_H_


#if defined (HAVE_SYS_TIME_GETTIMEOFDAY)
#include <sys/time.h>
#define CMP_GET_TIME_OF_DAY gettimeofday
#endif

#if defined (HAVE_TIME_GETTIMEOFDAY) || defined (_MSC_VER)
#include <time.h>
#define CMP_GET_TIME_OF_DAY gettimeofday
#endif

#ifndef HAVE_TIME_GETTIMEOFDAY
#ifndef HAVE_SYS_TIME_GETTIMEOFDAY
#ifdef __MINGW32__
#ifndef GET_TIME_OF_DAY_DEFINITION
#define GET_TIME_OF_DAY_DEFINITION
//#warning no gettimeofday() function native to system. Using own implementation
//#endif

#include <windows.h>
#include <sys/time.h>
void __stdcall GetSystemTimeAsFileTime(FILETIME*);

inline void CMP_gettimeofday(struct timeval* p, void* tz /* IGNORED */)
 {
  union {
     long long ns100; /*time since 1 Jan 1601 in 100ns units */
   FILETIME ft;
  } now;

    GetSystemTimeAsFileTime( &(now.ft) );
    p->tv_usec=(long)((now.ns100 / 10LL) % 1000000LL );
    p->tv_sec= (long)((now.ns100-(116444736000000000LL))/10000000LL);
 }

#define CMP_GET_TIME_OF_DAY CMP_gettimeofday
#endif /*  GET_TIME_OF_DAY_WARNING */
#else
#error Your system does not have a gettimeofday() function. Please contact the author of this library for advice.
#endif /* MinGW case */
#endif /* HAVE_SYS_TIME_GETTIMEOFDAY */
#endif /* HAVE_TIME_GETTIMEOFDAY */

#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>

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

#define DEBUG_OUT(function) \
  function() << "File: " << __FILE__ << "(" << __LINE__ << "): "
/**
* @brief Returns a Formatted String of the current Date/Time for logging
* purpose.
* @return A std:string of the current date/time
*/
inline std::string logTime() {
  TimeType long_time = 0;
  TimeFunc(&long_time);
  tm *t = 0x0;

#ifdef _MSC_VER
#if _MSC_VER < 1400
  t = _localtime64(&long_time);
#else
  tm time;
  t = &time;
  errno_t tError = _localtime64_s(&time, &long_time);
#endif
#else  // Non windows platforms
  t = localtime(&long_time);
#endif

  std::stringstream ss;
  ss.setf(std::ios::fixed);
  ss.fill('0');
  ss  << "[" << std::setw(4) << t->tm_year + 1900 << ":"
    << std::setw(2) << t->tm_mon + 1 << ":"
    << std::setw(2) << t->tm_mday << " "
    << std::setw(2) << t->tm_hour << ":"
    << std::setw(2) << t->tm_min << ":"
    << std::setw(2) << t->tm_sec << "] ";
  return ss.str();
}

/**
 * Returns a date/time string suitable for tiff tags.
 * @return
 */
inline std::string tifDateTime() {
  TimeType long_time = 0;
  TimeFunc(&long_time);
  tm *t = 0x0;

#ifdef _MSC_VER
#if _MSC_VER < 1400
  t = _localtime64(&long_time);
#else
  tm time;
  t = &time;
  errno_t tError = _localtime64_s(&time, &long_time);
#endif
#else  // Non windows platforms
  t = localtime(&long_time);
#endif

  std::stringstream ss;
  ss.setf(std::ios::fixed);
  ss.fill('0');
  ss   << std::setw(4) << t->tm_year + 1900 << ":"
    << std::setw(2) << t->tm_mon + 1 << ":"
    << std::setw(2) << t->tm_mday << " "
    << std::setw(2) << t->tm_hour << ":"
    << std::setw(2) << t->tm_min << ":"
    << std::setw(2) << t->tm_sec;
  return ss.str();
}

namespace MXA {

/**
 * @brief Generates a String suitable for using as a version string for
 * applications.
 * @return
 */
inline std::string MXAVersionString()
{
  TimeType long_time = 0;
  TimeFunc(&long_time);
  tm *t = 0x0;

#ifdef _MSC_VER
#if _MSC_VER < 1400
  t = _localtime64(&long_time);
#else
  tm time;
  t = &time;
  errno_t tError = _localtime64_s(&time, &long_time);
#endif
#else  // Non windows platforms
  t = localtime(&long_time);
#endif

  std::stringstream ss;
  ss.setf(std::ios::fixed);
  ss.fill('0');
  ss << std::setw(4) << t->tm_year + 1900 << "."
    << std::setw(2) << t->tm_mon + 1 << "."
    << std::setw(2) << t->tm_mday;
  return ss.str();
}



/**
 * @brief returns the number of milliseconds from a platform specified time.
 */
inline unsigned long long int getMilliSeconds()
{
#ifdef _MSC_VER
  return (unsigned long long int)(::clock());
#else
  struct timeval t1;
  CMP_GET_TIME_OF_DAY(&t1, NULL);
  unsigned long long int seconds ( t1.tv_sec );
  unsigned long long int microSec ( t1.tv_usec );
  seconds *= 1000;
  microSec /= 1000;
  return seconds + microSec; // Both in milliseconds at this point.
#endif
}


} // end Namespace MXA


int main(int argc, char **argv) {
  std::cout << MXA::MXAVersionString();
  return 0;
}


#endif //_GENERATE_VERSION_STRING_H_
