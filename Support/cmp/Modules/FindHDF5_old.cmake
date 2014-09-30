# This will find the HDF5 libraries and define the following variables:
#  HDF5_FOUND - Is the base HDF5 library and include file found
#  HDF5_INCLUDE_DIR - The Include directory containing all the HDF5 include files
#  HDF5_LIBRARY - The Actual HDF5 library
#  HDF5_HL_FOUND - Is the High Level HDF5 API Found
#  HDF5_HL_INCLUDE_DIR - The High Level Include Directory
#  HDF5_HL_LIBRARY - The Actual High Level Library
#  HDF5_USE_HIGH_LEVEL - Set this to TRUE if you need to link against the HDF5 High level APIs.
#  HDF5_INSTALL - This is an Environment variable that can be used to help find the HDF5 libraries and Include Directories
#  HDF5_LIBRARIES - The List of HDF5 libraries that were found. This variable can be used in a LINK_LIBRARIES(...) command
#  HDF5_LIBRARY_DEBUG - Debug Version of HDF5 library
#  HDF5_LIBRARY_RELEASE - Release Version of HDF5 library
# User Settable options that control which libraries will be found.
#  HDF5_USE_STATIC_LIBS - Set this to TRUE if you want this to prefer to find static libraries
#
#  To make finding HDF5 easier set the "HDF5_INSTALL" environment variable to the location
#  where HDF5 is installed.

# ------------------ START FIND HDF5 LIBS --------------------------------------
set(HDF5_FOUND "NO")
set(HDF5_HL_FOUND "NO")
set(HDF5_CPP_FOUND "NO")

# Only set HDF5_INSTALL to the environment variable if it is blank
if("${HDF5_INSTALL}" STREQUAL "")
    set(HDF5_INSTALL  $ENV{HDF5_INSTALL})
endif()

SET(HDF5_INCLUDE_SEARCH_DIRS
  ${HDF5_INSTALL}/include
)

set(HDF5_LIB_SEARCH_DIRS
  ${HDF5_INSTALL}/lib
)

set(HDF5_BIN_SEARCH_DIRS
  ${HDF5_INSTALL}/bin
)

# -- Find the Include directory for HDF5
FIND_PATH(HDF5_INCLUDE_DIR
  NAMES hdf5.h
  PATHS ${HDF5_INCLUDE_SEARCH_DIRS}
  NO_DEFAULT_PATH
)

if(WIN32 AND NOT MINGW)
    set(HDF5_SEARCH_DEBUG_NAMES "hdf5_D;libhdf5_D;hdf5dll_D;hdf5ddll")
    set(HDF5_SEARCH_RELEASE_NAMES "hdf5;libhdf5;hdf5dll")
ELSE (WIN32 AND NOT MINGW)
    set(HDF5_SEARCH_DEBUG_NAMES "hdf5_debug")
    set(HDF5_SEARCH_RELEASE_NAMES "hdf5")
ENDif(WIN32 AND NOT MINGW)

if( HDF5_USE_STATIC_LIBS )
 SET(_hdf5_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
 if(WIN32)
   SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
 ELSE(WIN32)
   SET(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
 ENDif(WIN32)
ENDif( HDF5_USE_STATIC_LIBS )

# Look for the library.
FIND_LIBRARY(HDF5_LIBRARY_DEBUG
  NAMES ${HDF5_SEARCH_DEBUG_NAMES}
  PATHS ${HDF5_LIB_SEARCH_DIRS}
  NO_DEFAULT_PATH
  )

FIND_LIBRARY(HDF5_LIBRARY_RELEASE
  NAMES ${HDF5_SEARCH_RELEASE_NAMES}
  PATHS ${HDF5_LIB_SEARCH_DIRS}
  NO_DEFAULT_PATH
  )

if(IS_SYMLINK ${HDF5_LIBRARY_DEBUG})
 # message(STATUS "SYMLINK: ${HDF5_LIBRARY_DEBUG}")
  get_filename_component(hdf5_debug_resolved_path ${HDF5_LIBRARY_DEBUG} REALPATH)
 # message(STATUS "Real Path: ${hdf5_debug_resolved_path}")
  set(HDF5_LIBRARY_DEBUG ${hdf5_debug_resolved_path})
endif()
if(IS_SYMLINK ${HDF5_LIBRARY_RELEASE})
 # message(STATUS "SYMLINK: ${HDF5_LIBRARY_RELEASE}")
  get_filename_component(hdf5_release_resolved_path ${HDF5_LIBRARY_RELEASE} REALPATH)
 # message(STATUS "Real Path: ${hdf5_release_resolved_path}")
  set(HDF5_LIBRARY_RELEASE ${hdf5_release_resolved_path})
endif()

if( HDF5_USE_STATIC_LIBS )
  SET(CMAKE_FIND_LIBRARY_SUFFIXES ${_hdf5_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES})
ENDif( HDF5_USE_STATIC_LIBS )

set(HDF5_DUMP_PROG_NAME "h5dump")
if(WIN32)
    set(HDF5_DUMP_PROG_NAME "h5dump.exe")
ENDif(WIN32)

FIND_PROGRAM(HDF5_DUMP_PROG
    NAMES ${HDF5_DUMP_PROG_NAME}
    PATHS ${HDF5_BIN_SEARCH_DIRS}
    NO_DEFAULT_PATH
)
MARK_AS_ADVANCED(HDF5_DUMP_PROG)


# include the macro to adjust libraries
INCLUDE (${CMP_MODULES_SOURCE_DIR}/cmpAdjustLibVars.cmake)
cmp_ADJUST_LIB_VARS(HDF5)

if(NOT ${HDF5_FIND_QUIETLY})
 MESSAGE(STATUS "HDF5_INCLUDE_SEARCH_DIRS: ${HDF5_INCLUDE_SEARCH_DIRS}")
 MESSAGE(STATUS "HDF5_LIB_SEARCH_DIRS: ${HDF5_LIB_SEARCH_DIRS}")
 MESSAGE(STATUS "HDF5_INCLUDE_DIR: ${HDF5_INCLUDE_DIR}")
 MESSAGE(STATUS "HDF5_LIBRARY_DEBUG: ${HDF5_LIBRARY_DEBUG}")
 MESSAGE(STATUS "HDF5_LIBRARY: ${HDF5_LIBRARY}")
 MESSAGE(STATUS "HDF5_LIBRARY_RELEASE: ${HDF5_LIBRARY_RELEASE}")
ENDif(NOT ${HDF5_FIND_QUIETLY})

if(HDF5_INCLUDE_DIR AND HDF5_LIBRARY)
  SET(HDF5_FOUND 1)
  SET(HDF5_LIBRARIES ${HDF5_LIBRARY})
  SET(HDF5_INCLUDE_DIRS ${HDF5_INCLUDE_DIR})
  if(HDF5_LIBRARY_DEBUG)
    GET_FILENAME_COMPONENT(HDF5_LIBRARY_PATH ${HDF5_LIBRARY_DEBUG} PATH)
    SET(HDF5_LIB_DIR  ${HDF5_LIBRARY_PATH})
  ELSEif(HDF5_LIBRARY_RELEASE)
    GET_FILENAME_COMPONENT(HDF5_LIBRARY_PATH ${HDF5_LIBRARY_RELEASE} PATH)
    SET(HDF5_LIB_DIR  ${HDF5_LIBRARY_PATH})
  ENDif(HDF5_LIBRARY_DEBUG)

  if(HDF5_DUMP_PROG)
    GET_FILENAME_COMPONENT(HDF5_BIN_PATH ${HDF5_DUMP_PROG} PATH)
    SET(HDF5_BIN_DIR  ${HDF5_BIN_PATH})
  endif(HDF5_DUMP_PROG)

  message(STATUS "Found HDF5 Library ${HDF5_LIBRARY_RELEASE}")

ELSE(HDF5_INCLUDE_DIR AND HDF5_LIBRARY)
  SET(HDF5_FOUND 0)
  SET(HDF5_LIBRARIES)
  SET(HDF5_INCLUDE_DIRS)
ENDif(HDF5_INCLUDE_DIR AND HDF5_LIBRARY)

# Report the results.
if(NOT HDF5_FOUND)
  SET(HDF5_DIR_MESSAGE
    "HDF5 was not found. Make sure HDF5_LIBRARY and HDF5_INCLUDE_DIR are set or set the HDF5_INSTALL environment variable.")
  if(NOT HDF5_FIND_QUIETLY)
    MESSAGE(STATUS "${HDF5_DIR_MESSAGE}")
  ELSE(NOT HDF5_FIND_QUIETLY)
    if(HDF5_FIND_REQUIRED)
      # MESSAGE(FATAL_ERROR "${HDF5_DIR_MESSAGE}")
      MESSAGE(FATAL_ERROR "HDF5 was NOT found and is Required by this project")
    ENDif(HDF5_FIND_REQUIRED)
  ENDif(NOT HDF5_FIND_QUIETLY)
ENDif(NOT HDF5_FOUND)


# ------------------ START FIND HDF5 HL LIBS -----------------------------------
if( HDF5_USE_HIGH_LEVEL )

  FIND_PATH(HDF5_HL_INCLUDE_DIR
    NAMES H5LT.h
    PATHS ${HDF5_INCLUDE_SEARCH_DIRS}
    #  NO_DEFAULT_PATH
  )

  if( HDF5_USE_STATIC_LIBS )
   SET(_hdf5_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
   if(WIN32)
     SET(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
   ELSE(WIN32)
     SET(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
   ENDif(WIN32)
  ENDif( HDF5_USE_STATIC_LIBS )

  FIND_LIBRARY(HDF5_HL_LIBRARY
    NAMES hdf5_hl
    PATHS ${HDF5_LIB_SEARCH_DIRS}
    #  NO_DEFAULT_PATH
  )

  if( HDF5_USE_STATIC_LIBS )
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ${_hdf5_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES})
  ENDif( HDF5_USE_STATIC_LIBS )

  if(HDF5_HL_INCLUDE_DIR )
    if(HDF5_HL_LIBRARY)
      set(HDF5_HL_FOUND "YES")
      set(HDF5_LIBRARIES ${HDF5_HL_LIBRARY} ${HDF5_LIBRARIES} )
      INCLUDE_DIRECTORIES( ${HDF5_HL_INCLUDE_DIR} )
    ELSE (HDF5_HL_LIBRARY)
      set(HDF5_HL_FOUND "NO")
    endif(HDF5_HL_LIBRARY)
  endif(HDF5_HL_INCLUDE_DIR)
endif( HDF5_USE_HIGH_LEVEL )


if(HDF5_FOUND)
    INCLUDE(CheckSymbolExists)
    #############################################
    # Find out if HDF5 was build using dll's
    #############################################
    # Save required variable
    SET(CMAKE_REQUIRED_INCLUDES_SAVE ${CMAKE_REQUIRED_INCLUDES})
    SET(CMAKE_REQUIRED_FLAGS_SAVE${CMAKE_REQUIRED_FLAGS})
    # Add HDF5_INCLUDE_DIR to CMAKE_REQUIRED_INCLUDES
    SET(CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES};${HDF5_INCLUDE_DIRS}")

    CHECK_SYMBOL_EXISTS(H5_BUILT_AS_DYNAMIC_LIB "H5pubconf.h" HAVE_HDF5_DLL)
    if(HAVE_HDF5_DLL)
     set(HDF5_IS_SHARED 1 CACHE INTERNAL "HDF5 Built as DLL or Shared Library")
    endif()

    # Restore CMAKE_REQUIRED_INCLUDES and CMAKE_REQUIRED_FLAGS variables
    SET(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES_SAVE})
    SET(CMAKE_REQUIRED_FLAGS${CMAKE_REQUIRED_FLAGS_SAVE})
    #
    #############################################

    if(NOT HDF5_VERSION)
        # We parse the version information from the boost/version.hpp header.
        file(STRINGS ${HDF5_INCLUDE_DIRS}/H5pubconf.h HDF5_VERSIONSTR
          REGEX "#define[ ]+H5_PACKAGE_VERSION[ ]+\"[0-9]+\\.[0-9]+\\.[0-9]+\"")
        if(HDF5_VERSIONSTR)
          string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" HDF5_VERSIONSTR ${HDF5_VERSIONSTR})
          string(REGEX MATCHALL "[0-9]+" VERSION_LIST ${HDF5_VERSIONSTR})
          list(GET VERSION_LIST 0 HDF5_VERSION_MAJOR)
          list(GET VERSION_LIST 1 HDF5_VERSION_MINOR)
          list(GET VERSION_LIST 2 HDF5_VERSION_SUBMINOR)
          set(HDF5_VERSION "${HDF5_VERSION_MAJOR}.${HDF5_VERSION_MINOR}.${HDF5_VERSION_SUBMINOR}" CACHE STRING "Version of HDF5 found")
          mark_as_advanced(HDF5_VERSION)
          message(STATUS "Found HDF5 Version ${HDF5_VERSION_MAJOR}.${HDF5_VERSION_MINOR}.${HDF5_VERSION_SUBMINOR}")
        else()
         message(WARNING
            "Unable to parse HDF5 version from ${HDF5_INCLUDE_DIRS}/H5pubconf.h. You may not be using the latest version of HDF5. This should NOT impact any compiles")
        endif()
    endif()



endif(HDF5_FOUND)
