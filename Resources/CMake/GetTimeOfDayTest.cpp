///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////
#if defined (TRY_SYS_TIME_H)
#include <sys/time.h>
#include <time.h>
#endif


#if defined (TRY_TIME_H)
#include <time.h>
#endif

int main(int argc, char **argv) {
	  struct timeval t1;
	  gettimeofday(&t1, 0x00);
	return 0;
}
