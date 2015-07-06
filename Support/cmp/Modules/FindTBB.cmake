# Locate Intel Threading Building Blocks include paths and libraries
# TBB can be found at http://www.threadingbuildingblocks.org/
# Written by Hannes Hofmann, hannes.hofmann _at_ informatik.uni-erlangen.de
# Adapted by Gino van den Bergen gino _at_ dtecta.com

# GvdB: This module uses the environment variable TBB_ARCH_PLATFORM which defines architecture and compiler.
#   e.g. "ia32/vc8" or "intel64/cc4.1.0_libc2.4_kernel2.6.16.21"
#   TBB_ARCH_PLATFORM is set by the build script tbbvars[.bat|.sh|.csh], which can be found
#   in the TBB installation directory (TBB_INSTALL_DIR).
#
# For backwards compatibility, you may explicitely set the CMake variables TBB_ARCHITECTURE and TBB_COMPILER.
# TBB_ARCHITECTURE     [ ia32 | intel64 | itanium ]
#   which architecture to use
# TBB_COMPILER         e.g. vc9 or cc3.2.3_libc2.3.2_kernel2.4.21 or cc4.0.1_os10.4.9
#   which compiler to use (detected automatically on Windows)

# This module respects
# TBB_INSTALL_DIR or $ENV{TBB21_INSTALL_DIR} or $ENV{TBB_INSTALL_DIR}

# This module defines
# TBB_INCLUDE_DIRS, where to find task_scheduler_init.h, etc.
# TBB_LIBRARY_DIRS, where to find libtbb, libTBB_MALLOC
# TBB_INSTALL_DIR, the base TBB install directory
# TBB_LIBRARIES, the libraries to link against to use TBB.
# TBB_DEBUG_LIBRARIES, the libraries to link against to use TBB with debug symbols.
# TBB_FOUND, If false, don't try to use TBB.

set(TBB_DEBUG 0)


# $ENV{TBB_ARCH_PLATFORM} is set by the build script tbbvars[.bat|.sh|.csh]
# Look for the environment variable first and TBB_ARCH_PLATFORM CMake variable are NOT set.
# then use the environment varible
if(NOT "$ENV{TBB_ARCH_PLATFORM}" STREQUAL "" AND "${TBB_ARCH_PLATFORM}" STREQUAL "")
	set(TBB_ARCH_PLATFORM $ENV{TBB_ARCH_PLATFORM} )
endif()

if(NOT "$ENV{TBB_ARCH_TYPE}" STREQUAL "" AND "${TBB_ARCH_TYPE}" STREQUAL "")
	set(TBB_ARCH_TYPE $ENV{TBB_ARCH_TYPE} )
endif()

if(TBB_DEBUG)
	message(STATUS "**** TBB Environment Variables")
	message(STATUS "TBB_INSTALL_DIR: ${TBB_INSTALL_DIR}")
	message(STATUS "TBB_ARCH_TYPE: ${TBB_ARCH_TYPE}")
	message(STATUS "TBB_ARCH_PLATFORM: ${TBB_ARCH_PLATFORM}")
endif()

# Figure out the TBB_ARCH_PLATFORM on Windows. Note that we only support
# visual studio 9, 10, 11
if(WIN32 )
	set(TBB_ARCH_TYPE "ia32")
	if( "${CMAKE_SIZEOF_VOID_P}" EQUAL "8" )
		set(TBB_ARCH_TYPE "intel64")
	endif()
    if(MSVC90)
         set(TBB_ARCH_PLATFORM "${TBB_ARCH_TYPE}/vc9")
    endif(MSVC90)
    if(MSVC10)
        set(TBB_ARCH_PLATFORM "${TBB_ARCH_TYPE}/vc10")
    endif(MSVC10)
	if(MSVC11)
		set(TBB_ARCH_PLATFORM "${TBB_ARCH_TYPE}/vc11")
	endif(MSVC11)
	if(MSVC12)
		set(TBB_ARCH_PLATFORM "${TBB_ARCH_TYPE}/vc12")
	endif(MSVC12)
    if(NOT TBB_ARCH_PLATFORM)
        message("ERROR: TBB supports only Visual Studio 9, 10, 11 & 12 compilers on Windows platforms.")
    endif(NOT TBB_ARCH_PLATFORM)
endif()

if(WIN32 AND NOT "${TBB_ARCH_PLATFORM}" STREQUAL "")
	set(TBB_ARCH_TYPE "ia32")
	if( "${CMAKE_SIZEOF_VOID_P}" EQUAL "8" )
		set(TBB_ARCH_TYPE "intel64")
	endif()
    # has intel64/vc8   intel64/vc9
    # has ia32/vc7.1  ia32/vc8   ia32/vc9
    set(_TBB_DEFAULT_INSTALL_DIR "C:/Program Files/Intel/TBB" "C:/Program Files (x86)/Intel/TBB")
    set(_TBB_LIB_NAME "tbb")
    set(_TBB_LIB_MALLOC_NAME "${_TBB_LIB_NAME}malloc")
    set(_TBB_LIB_DEBUG_NAME "${_TBB_LIB_NAME}_debug")
    set(_TBB_LIB_MALLOC_DEBUG_NAME "${_TBB_LIB_MALLOC_NAME}_debug")
endif()

if(UNIX)
    if(APPLE)
        # MAC
        set(_TBB_DEFAULT_INSTALL_DIR "/Library/Frameworks/Intel_TBB.framework/Versions")
        # libs: libtbb.dylib, libTBB_MALLOC.dylib, *_debug
        set(_TBB_LIB_NAME "tbb")
        set(_TBB_LIB_MALLOC_NAME "${_TBB_LIB_NAME}malloc")
        set(_TBB_LIB_DEBUG_NAME "${_TBB_LIB_NAME}_debug")
        set(_TBB_LIB_MALLOC_DEBUG_NAME "${_TBB_LIB_MALLOC_NAME}_debug")
        # has only one flavor: ia32/cc4.0.1_os10.4.9
        set(_TBB_COMPILER "cc4.0.1_os10.4.9")
        set(_TBB_ARCHITECTURE "ia32")
    else (APPLE)
        # LINUX
        set(_TBB_DEFAULT_INSTALL_DIR "/opt/intel/tbb" "/usr/local/include" "/usr/include")
        set(_TBB_LIB_NAME "tbb")
        set(_TBB_LIB_MALLOC_NAME "${_TBB_LIB_NAME}malloc")
        set(_TBB_LIB_DEBUG_NAME "${_TBB_LIB_NAME}_debug")
        set(_TBB_LIB_MALLOC_DEBUG_NAME "${_TBB_LIB_MALLOC_NAME}_debug")
        # has intel64/cc3.2.3_libc2.3.2_kernel2.4.21  intel64/cc3.3.3_libc2.3.3_kernel2.6.5  intel64/cc3.4.3_libc2.3.4_kernel2.6.9  intel64/cc4.1.0_libc2.4_kernel2.6.16.21
        # has ia32/*
        # has itanium/*
        set(_TBB_COMPILER ${TBB_COMPILER})
        set(_TBB_ARCHITECTURE ${TBB_ARCHITECTURE})
    endif(APPLE)
endif(UNIX)

if(CMAKE_SYSTEM MATCHES "SunOS.*")
# SUN
# not yet supported
# has intel64/cc3.4.3_kernel5.10
# has ia32/*
endif(CMAKE_SYSTEM MATCHES "SunOS.*")


#-- Clear the public variables
set(TBB_FOUND "NO")

#-- Find TBB install dir and set ${_TBB_INSTALL_DIR} and cached ${TBB_INSTALL_DIR}
# first: use CMake variable TBB_INSTALL_DIR
if(TBB_INSTALL_DIR)
    set(_TBB_INSTALL_DIR ${TBB_INSTALL_DIR})
endif(TBB_INSTALL_DIR)
# second: use environment variable
if(NOT _TBB_INSTALL_DIR)
    if(NOT "$ENV{TBB_INSTALL_DIR}" STREQUAL "")
        set(_TBB_INSTALL_DIR $ENV{TBB_INSTALL_DIR})
    endif(NOT "$ENV{TBB_INSTALL_DIR}" STREQUAL "")
    # Intel recommends setting TBB21_INSTALL_DIR
    if(NOT "$ENV{TBB21_INSTALL_DIR}" STREQUAL "")
        set(_TBB_INSTALL_DIR $ENV{TBB21_INSTALL_DIR})
    endif(NOT "$ENV{TBB21_INSTALL_DIR}" STREQUAL "")
    if(NOT "$ENV{TBB22_INSTALL_DIR}" STREQUAL "")
        set(_TBB_INSTALL_DIR $ENV{TBB22_INSTALL_DIR})
    endif(NOT "$ENV{TBB22_INSTALL_DIR}" STREQUAL "")
    if(NOT "$ENV{TBB30_INSTALL_DIR}" STREQUAL "")
        set(_TBB_INSTALL_DIR $ENV{TBB30_INSTALL_DIR})
    endif(NOT "$ENV{TBB30_INSTALL_DIR}" STREQUAL "")
    if(NOT "$ENV{TBB40_INSTALL_DIR}" STREQUAL "")
        set(_TBB_INSTALL_DIR $ENV{TBB40_INSTALL_DIR})
    endif(NOT "$ENV{TBB40_INSTALL_DIR}" STREQUAL "")
endif(NOT _TBB_INSTALL_DIR)
# third: try to find path automatically
if(NOT _TBB_INSTALL_DIR)
    if(_TBB_DEFAULT_INSTALL_DIR)
        set(_TBB_INSTALL_DIR $ENV{_TBB_DEFAULT_INSTALL_DIR})
    endif(_TBB_DEFAULT_INSTALL_DIR)
endif(NOT _TBB_INSTALL_DIR)
# sanity check
if(NOT _TBB_INSTALL_DIR)
    message ("ERROR: TBB_INSTALL_DIR not found. ${_TBB_INSTALL_DIR}")
else (NOT _TBB_INSTALL_DIR)
# finally: set the cached CMake variable TBB_INSTALL_DIR
if(NOT TBB_INSTALL_DIR)
    set(TBB_INSTALL_DIR ${_TBB_INSTALL_DIR} CACHE PATH "Intel TBB install directory")
    mark_as_advanced(TBB_INSTALL_DIR)
endif(NOT TBB_INSTALL_DIR)

if(TBB_DEBUG)
	message(STATUS "*** TBB Variables Calculated")
	message(STATUS "TBB_INSTALL_DIR: ${TBB_INSTALL_DIR}")
	message(STATUS "TBB_ARCH_PLATFORM: ${TBB_ARCH_PLATFORM}")
	message(STATUS "TBB_ARCH_TYPE: ${TBB_ARCH_TYPE}")
	message(STATUS "TBB_BIN_DIR: ${TBB_BIN_DIR}")
	message(STATUS "TBB_INCLUDE_DIR: ${TBB_INCLUDE_DIR}")
endif()

#-- A macro to rewrite the paths of the library. This is necessary, because
#   find_library() always found the intel64/vc9 version of the TBB libs
macro(TBB_CORRECT_LIB_DIR var_name)
#    if(NOT "${_TBB_ARCHITECTURE}" STREQUAL "intel64")
        string(REPLACE intel64 "${_TBB_ARCHITECTURE}" ${var_name} ${${var_name}})
#    endif(NOT "${_TBB_ARCHITECTURE}" STREQUAL "intel64")
    string(REPLACE ia32 "${_TBB_ARCHITECTURE}" ${var_name} ${${var_name}})
    string(REPLACE vc7.1 "${_TBB_COMPILER}" ${var_name} ${${var_name}})
    string(REPLACE vc8 "${_TBB_COMPILER}" ${var_name} ${${var_name}})
    string(REPLACE vc9 "${_TBB_COMPILER}" ${var_name} ${${var_name}})
endmacro(TBB_CORRECT_LIB_DIR var_content)


#-- Look for include directory and set ${TBB_INCLUDE_DIR}
set(TBB_INC_SEARCH_DIR ${_TBB_INSTALL_DIR}/include)
find_path(TBB_INCLUDE_DIR
    tbb/task_scheduler_init.h
    PATHS ${TBB_INC_SEARCH_DIR}
    NO_DEFAULT_PATH
)
mark_as_advanced(TBB_INCLUDE_DIR)



#-- Look for libraries

set(_TBB_LIBRARY_DIR
	 ${_TBB_INSTALL_DIR}/lib/${TBB_ARCH_PLATFORM}
	 ${_TBB_INSTALL_DIR}/${TBB_ARCH_PLATFORM}/lib
	)
	
find_library(TBB_LIBRARY_RELEASE
		${_TBB_LIB_NAME}
		${_TBB_LIBRARY_DIR}
	 	NO_DEFAULT_PATH)
find_library(TBB_MALLOC_LIBRARY_RELEASE 
		${_TBB_LIB_MALLOC_NAME} 
		${_TBB_LIBRARY_DIR} 
		NO_DEFAULT_PATH)

#-- Look for debug libraries
find_library(TBB_LIBRARY_DEBUG        
		${_TBB_LIB_DEBUG_NAME}        
		${_TBB_LIBRARY_DIR} 
		NO_DEFAULT_PATH)
find_library(TBB_MALLOC_LIBRARY_DEBUG 
		${_TBB_LIB_MALLOC_DEBUG_NAME} 
		${_TBB_LIBRARY_DIR} 
		NO_DEFAULT_PATH)

# Set the binary directory where the Windows DLL files are located.
set(TBB_BIN_DIR ${TBB_INSTALL_DIR}/bin/${TBB_ARCH_PLATFORM} CACHE PATH "" FORCE )
set(TBB_MALLOC_BIN_DIR ${TBB_INSTALL_DIR}/bin/${TBB_ARCH_PLATFORM}  CACHE PATH "" FORCE)
set(TBB_COMPONENTS tbb tbbmalloc)

# include the macro to adjust libraries
INCLUDE (${CMP_MODULES_SOURCE_DIR}/cmpAdjustLibVars.cmake)
cmp_ADJUST_LIB_VARS(TBB)
# This line is needed to trigger the correct behavior in the next line
set(TBB_MALLOC_INCLUDE_DIR ${TBB_INCLUDE_DIR})
cmp_ADJUST_LIB_VARS(TBB_MALLOC)

if(TBB_DEBUG)
	message(STATUS "*** TBB Internal Variables ")
    message(STATUS "_TBB_LIB_NAME: ${_TBB_LIB_NAME}")
    message(STATUS "_TBB_LIB_DEBUG_NAME: ${_TBB_LIB_DEBUG_NAME}")
	message(STATUS "_TBB_LIBRARY_DIR: ${_TBB_LIBRARY_DIR}")
	message(STATUS "TBB_LIBRARY: ${TBB_LIBRARY}")
	message(STATUS "TBB_LIBRARY_DEBUG: ${TBB_LIBRARY_DEBUG}")
	message(STATUS "TBB_LIBRARY_RELEASE: ${TBB_LIBRARY_RELEASE}")
	message(STATUS "TBB_MALLOC_LIBRARY: ${TBB_MALLOC_LIBRARY}")
	message(STATUS "TBB_MALLOC_LIBRARY_DEBUG: ${TBB_MALLOC_LIBRARY_DEBUG}")
	message(STATUS "TBB_MALLOC_LIBRARY_RELEASE: ${TBB_MALLOC_LIBRARY_RELEASE}")
	message(STATUS "TBB_BIN_DIR: ${TBB_BIN_DIR}")
	message(STATUS "TBB_MALLOC_BIN_DIR: ${TBB_MALLOC_BIN_DIR}")
endif()


if(TBB_INCLUDE_DIR)
    if(TBB_LIBRARY)
        set(TBB_FOUND 1)
        set(TBB_LIBRARIES ${TBB_LIBRARY} ${TBB_MALLOC_LIBRARY} )

        set(TBB_MALLOC_LIBRARIES ${TBB_MALLOC_LIBRARY})
        set(TBB_INCLUDE_DIRS ${TBB_INCLUDE_DIR} CACHE PATH "TBB include directory")
        mark_as_advanced(TBB_INCLUDE_DIRS)
        set(TBB_BIN_DIR ${_TBB_INSTALL_DIR}/bin/$ENV{TBB_ARCH_PLATFORM} CACHE PATH "TBB Binary Directory")
        mark_as_advanced(TBB_BIN_DIR)
        set(TBB_LIB_DIR ${_TBB_LIBRARY_DIR} CACHE PATH "TBB Library Directory")
        set(TBB_MALLOC_BIN_DIR ${_TBB_INSTALL_DIR}/bin/$ENV{TBB_ARCH_PLATFORM} CACHE PATH "TBB Binary Directory")
        set(TBB_MALLOC_LIB_DIR ${_TBB_LIBRARY_DIR} CACHE PATH "TBB Library Directory")
        if(TBB_LIBRARY_DEBUG)
            GET_FILENAME_COMPONENT(TBB_LIBRARY_PATH ${TBB_LIBRARY_DEBUG} PATH)
            SET(TBB_LIBRARY_DIR  ${TBB_LIBRARY_PATH} CACHE FILEPATH "Path to tbb Library Directory")
        ELSEif(TBB_LIBRARY_RELEASE)
            GET_FILENAME_COMPONENT(TBB_LIBRARY_PATH ${TBB_LIBRARY_RELEASE} PATH)
            SET(TBB_LIBRARY_DIR  ${TBB_LIBRARY_PATH} CACHE FILEPATH "Path to tbb Library Directory")
        ENDif(TBB_LIBRARY_DEBUG)
        set(TBB_IS_SHARED "TRUE")
        set(TBB_MALLOC_IS_SHARED "TRUE")
        message(STATUS "Intel TBB Location: ${_TBB_INSTALL_DIR}")
    endif(TBB_LIBRARY)
endif(TBB_INCLUDE_DIR)

if(NOT TBB_FOUND)
    message("ERROR: Intel TBB NOT found!")
    message(STATUS "Looked for Threading Building Blocks in ${_TBB_INSTALL_DIR}")
    # do only throw fatal, if this pkg is REQUIRED
    if(TBB_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find TBB library.")
    endif(TBB_FIND_REQUIRED)
endif(NOT TBB_FOUND)

endif(NOT _TBB_INSTALL_DIR)
