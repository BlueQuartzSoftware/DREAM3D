
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that queries machine memory.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MXAMEMORY_H_
#define _MXAMEMORY_H_



#ifdef __APPLE__
#include <sys/sysctl.h>
#include <stdio.h>
#include "mach/mach_host.h"
#include <mach/mach.h>
#include <mach/mach_types.h>
#endif

#include <string>

#include "MXA/MXA.h"



class MXA_EXPORT MXAMemory
{
  public:
    virtual ~MXAMemory();

    // Get the memory properties.
    static uint64_t       availablePhysical();
    static uint64_t       availableVirtual();
    static uint64_t       totalPhysical();
    static uint64_t       totalVirtual();
    static uint64_t       usedPhysical();
    static uint64_t       usedVirtual();

  #ifdef __APPLE__
    static void                     DARWIN_getVMStat (vm_statistics64_t vmstat);
    static int                      DARWIN_getPageSize();
    static long long int            DARWIN_getPhysicalMemory();
    static long long int            DARWIN_getUsedMemory();
    static long long int            DARWIN_getAvailableMemory();
  #endif
    // Return a formatted string.
    static std::string               formatPhysical();

  protected:
    MXAMemory();

  private:
    MXAMemory(const MXAMemory&); // Copy Constructor Not Implemented
    void operator=(const MXAMemory&); // Operator '=' Not Implemented
};



#endif // _MXAMEMORY_H_
