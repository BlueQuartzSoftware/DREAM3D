# - Find MINGW library
# - Derived from the FindMINGW.cmake that is included with cmake
# Find the native MINGW includes and library
# This module defines
#  MINGW_INCLUDE_DIR, where to find MINGW.h, etc.
#  MINGW_LIBRARIES, libraries to link against to use MINGW.
#  MINGW_FOUND, If false, do not try to use MINGW.
# also defined, but not for general use are
#  MINGW_LIBRARY, where to find the MINGW library.
#  MINGW_LIBRARY_DEBUG - Debug version of MINGW library
#  MINGW_LIBRARY_RELEASE - Release Version of MINGW library

# MESSAGE (STATUS "Finding MINGW library and headers..." )

# Only set MINGW_INSTALL to the environment variable if it is blank
if ("${MINGW_INSTALL}" STREQUAL "")
    set(MINGW_INSTALL  $ENV{MINGW_INSTALL})
endif()

# Look for the header file.
SET(MINGW_INCLUDE_SEARCH_DIRS
  ${MINGW_INSTALL}/include
)

set(MINGW_LIB_SEARCH_DIRS
  ${MINGW_INSTALL}/bin
  )

FIND_PATH(MINGW_INCLUDE_DIR 
  NAMES _mingw.h
  PATHS ${MINGW_INCLUDE_SEARCH_DIRS} 
  NO_DEFAULT_PATH
)

set(MINGW_SEARCH_NAMES "mingwm10")

# Look for the library.
FIND_LIBRARY(MINGW_LIBRARY
  NAMES ${MINGW_SEARCH_NAMES}
  PATHS ${MINGW_LIB_SEARCH_DIRS} 
  NO_DEFAULT_PATH
  )

# MESSAGE(STATUS "MINGW_INCLUDE_DIR: ${MINGW_INCLUDE_DIR}")
# MESSAGE(STATUS "MINGW_LIBRARY: ${MINGW_LIBRARY}")
# MESSAGE(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

IF(MINGW_INCLUDE_DIR AND MINGW_LIBRARY)
  SET(MINGW_FOUND 1)
  SET(MINGW_LIBRARIES ${MINGW_LIBRARY})
  SET(MINGW_INCLUDE_DIRS ${MINGW_INCLUDE_DIR})
ELSE(MINGW_INCLUDE_DIR AND MINGW_LIBRARY)
  SET(MINGW_FOUND 0)
  SET(MINGW_LIBRARIES)
  SET(MINGW_INCLUDE_DIRS)
ENDIF(MINGW_INCLUDE_DIR AND MINGW_LIBRARY)

# Report the results.
IF(NOT MINGW_FOUND)
  SET(MINGW_DIR_MESSAGE
    "MINGW was not found. Set MINGW_INSTALL environment variable to help this. Make sure MINGW_LIBRARY and MINGW_INCLUDE_DIR are set.")
  IF(NOT MINGW_FIND_QUIETLY)
    MESSAGE(STATUS "${MINGW_DIR_MESSAGE}")
  ELSE(NOT MINGW_FIND_QUIETLY)
    IF(MINGW_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "MINGW was NOT found and is Required by this project")
    ENDIF(MINGW_FIND_REQUIRED)
  ENDIF(NOT MINGW_FIND_QUIETLY)
ENDIF(NOT MINGW_FOUND)

