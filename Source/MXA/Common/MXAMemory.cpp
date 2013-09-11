
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

#include "MXAMemory.h"

#include <sstream>

#ifdef _MSC_VER
# define NOMINMAX
# include <windows.h> // For GetComputerName()
#endif

#ifdef __APPLE__

#endif

#ifdef __linux
#include <sys/sysinfo.h>
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MXAMemory::MXAMemory()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MXAMemory::~MXAMemory()
{

}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the memory.
//
///////////////////////////////////////////////////////////////////////////////

uint64_t MXAMemory::totalPhysical()
{
#ifdef _MSC_VER

  ::MEMORYSTATUS status;
  ::GlobalMemoryStatus ( &status );
  return status.dwTotalPhys;

#elif __APPLE__
  return MXAMemory::DARWIN_getPhysicalMemory();
#elif __linux

  struct sysinfo info;
  sysinfo ( &info );
  return info.totalram;

#else
  TODO FIX ME

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the memory.
//
///////////////////////////////////////////////////////////////////////////////

uint64_t MXAMemory::totalVirtual()
{
#ifdef _MSC_VER

  ::MEMORYSTATUS status;
  ::GlobalMemoryStatus ( &status );
  return status.dwTotalVirtual;

#elif __APPLE__
#if 0
  //This fails on OS X 10.3.9
  //This isn't really correct.. should not be used.
    struct xsw_usage swapusage;

    int mib[2];
    size_t len;
    mib[0] = CTL_VM;
    mib[1] = VM_SWAPUSAGE;
    len = sizeof(swapusage);
    sysctl(mib, 2, &swapusage, &len, NULL, 0);
    return static_cast< uint64_t > ( swapusage.xsu_total );
#endif
    return 0x0;
#elif __linux

  struct sysinfo info;
  sysinfo ( &info );
  return info.totalswap;

#else
  TODO  FIX ME
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the memory.
//
///////////////////////////////////////////////////////////////////////////////

uint64_t MXAMemory::availablePhysical()
{
#ifdef _MSC_VER

  ::MEMORYSTATUS status;
  ::GlobalMemoryStatus ( &status );
  return status.dwAvailPhys;

#elif __APPLE__
  return  MXAMemory::DARWIN_getAvailableMemory();
#elif __linux

  struct sysinfo info;
  sysinfo ( &info );
  return info.freeram;

#else

  TODO FIX ME

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the memory.
//
///////////////////////////////////////////////////////////////////////////////

uint64_t MXAMemory::availableVirtual()
{
#ifdef _MSC_VER

  ::MEMORYSTATUS status;
  ::GlobalMemoryStatus ( &status );
  return status.dwAvailVirtual;

#elif __APPLE__
#if 0
  This is not correct and should not be used
    struct xsw_usage swapusage;
    int mib[2];
    size_t len;
    mib[0] = CTL_VM;
    mib[1] = VM_SWAPUSAGE;
    len = sizeof(swapusage);
    sysctl(mib, 2, &swapusage, &len, NULL, 0);
    return static_cast< uint64_t > ( swapusage.xsu_avail );
#endif
    return 0x0;
 #elif __linux

  struct sysinfo info;
  sysinfo ( &info );
  return info.freeswap;

#else

 TODO FIX ME

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the memory.
//
///////////////////////////////////////////////////////////////////////////////

uint64_t MXAMemory::usedPhysical()
{
#ifdef __APPLE__

  return MXAMemory::DARWIN_getUsedMemory();

#else

  return ( MXAMemory::totalPhysical() - MXAMemory::availablePhysical() );

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the memory.
//
///////////////////////////////////////////////////////////////////////////////

uint64_t MXAMemory::usedVirtual()
{
  return ( MXAMemory::totalVirtual() - MXAMemory::availableVirtual() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a formatted string.
//
///////////////////////////////////////////////////////////////////////////////

QString MXAMemory::formatPhysical()
{
  const uint64_t toKB ( 1 );
  std::ostringstream out;
  out << "Memory Used: " << MXAMemory::usedPhysical()      / toKB
      << ", Available: " << MXAMemory::availablePhysical() / toKB
      << ", Total: "     << MXAMemory::totalPhysical()     / toKB;
  return out.str();
}


#ifdef __APPLE__
void  MXAMemory::DARWIN_getVMStat (vm_statistics64_t vmstat)
{
	unsigned int count = HOST_VM_INFO_COUNT;
	if ( host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t) vmstat, &count) != KERN_SUCCESS )
		printf("Failed to get VM statistics.");
}

 int MXAMemory::DARWIN_getPageSize ()
{
  int mib[2];
  size_t len;
  size_t pageSize;
  mib[0] = CTL_HW;
  mib[1] = HW_PAGESIZE;
  len = sizeof(pageSize);
  sysctl(mib, 2, &pageSize, &len, NULL, 0);
  return pageSize;
}

 long long int MXAMemory::DARWIN_getPhysicalMemory()
{
  int mib[2];
  size_t len = 0;
  size_t physmem = 0;
  mib[0] = CTL_HW;
  mib[1] = HW_MEMSIZE;
  len = sizeof(physmem);
  sysctl(mib, 2, &physmem, &len, NULL, 0);
  return physmem;
}

 long long int MXAMemory::DARWIN_getUsedMemory()
{
  uint64_t total;
  vm_statistics64_data_t	vmstat;
  MXAMemory::DARWIN_getVMStat (&vmstat);
  total = vmstat.wire_count + vmstat.active_count + vmstat.inactive_count ;
  total = total * MXAMemory::DARWIN_getPageSize();
  return (long long int)total;
}

 long long int MXAMemory::DARWIN_getAvailableMemory()
{
  vm_statistics64_data_t	vmstat;
  MXAMemory::DARWIN_getVMStat (&vmstat);
  return (long long int) (vmstat.free_count * MXAMemory::DARWIN_getPageSize() );
}

#endif
