
//
//  Copyright (c) 2006, Michael A. Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//

#ifndef _WIN32DEFINES_H_
#define _WIN32DEFINES_H_

#ifdef _MSC_VER
  #include <cstdlib>
  #include <string>
  #include <stdio.h>
  #include "MXA/MXA.h"
/*
"It's a known, long-standing bug in the compiler system's headers.  For
some reason the manufacturer, in its infinite wisdom, chose to #define
macros min() and max() in violation of the upper-case convention and so
break any legitimate functions with those names, including those in the
standard C++ library."
*/

  #ifndef NOMINMAX
    #define NOMINMAX
  #endif

  #define WINDOWS_LARGE_FILE_SUPPORT
	#if _MSC_VER < 1400
		#define snprintf _snprintf
	#else
		#define snprintf sprintf_s
	#endif
#endif



#endif /* _WIN32DEFINES_H_ */
