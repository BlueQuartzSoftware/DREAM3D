#/* --------------------------------------------------------------------------*
#* This source code has been cleared for public release by the                *
#* US Air Force 88th Air Base Wing Public Affairs Office under                *
#* case number 88ABW-2010-4857 on Sept. 7, 2010.                              *
#* -------------------------------------------------------------------------- */
#--////////////////////////////////////////////////////////////////////////////
#--  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--////////////////////////////////////////////////////////////////////////////

# - Find EM/MPM
# Find the native EMMPM headers and libraries.
#
#  EMMPM_INCLUDE_DIRS - where to find EM/MPM.h, etc.
#  EMMPM_LIBRARIES    - List of libraries when using EM/MPM.
#  EMMPM_LIBRARY_DEBUG - Debug version of Library
#  EMMPM_LIBRARY_RELEASE - Release Version of Library
#  EMMPM_FOUND        - True if EM/MPM found.
#  EMMPM_RESOURCES_DIR - The directory where supporting cmake files and other files can be found

# Look for the header file.
# Only set HDF5_INSTALL to the environment variable if it is blank
if ("${EMMPM_INSTALL}" STREQUAL "")
set(EMMPM_INSTALL  $ENV{EMMPM_INSTALL})
endif()

# MESSAGE (STATUS "Finding EM/MPM library and headers..." )
# set(EMMPM_DEBUG 1)

SET(EMMPM_INCLUDE_SEARCH_DIRS
  ${EMMPM_INSTALL}/include
)

set(EMMPM_LIB_SEARCH_DIRS
  ${EMMPM_INSTALL}/lib
  )

set(EMMPM_BIN_SEARCH_DIRS
  ${EMMPM_INSTALL}/bin
)

FIND_PATH(EMMPM_INCLUDE_DIR 
  NAMES emmpm/public/EMMPM.h
  PATHS ${EMMPM_INCLUDE_SEARCH_DIRS}
  NO_DEFAULT_PATH
)

set(EMMPM_SEARCH_DEBUG_NAMES "EMMPMLib_debug;EMMPMLib_D;libEMMPMLib_D;libEMMPMLib_debug")
set(EMMPM_SEARCH_RELEASE_NAMES "EMMPMLib;libEMMPMLib")



IF (EMMPM_DEBUG)
    message (STATUS "EMMPM_INCLUDE_SEARCH_DIRS: ${EMMPM_INCLUDE_SEARCH_DIRS}")
    message (STATUS "EMMPM_LIB_SEARCH_DIRS: ${EMMPM_LIB_SEARCH_DIRS}")
    message (STATUS "EMMPM_BIN_SEARCH_DIRS: ${EMMPM_BIN_SEARCH_DIRS}")
    message (STATUS "EMMPM_SEARCH_RELEASE_NAMES: ${EMMPM_SEARCH_RELEASE_NAMES}")
    message (STATUS "EMMPM_SEARCH_DEBUG_NAMES: ${EMMPM_SEARCH_DEBUG_NAMES}")
    message (STATUS "EMMPM_INCLUDE_DIR: ${EMMPM_INCLUDE_DIR}")
ENDIF (EMMPM_DEBUG)

# Look for the library.
FIND_LIBRARY(EMMPM_LIBRARY_DEBUG 
  NAMES ${EMMPM_SEARCH_DEBUG_NAMES}
  PATHS ${EMMPM_LIB_SEARCH_DIRS} 
  NO_DEFAULT_PATH
  )
  
FIND_LIBRARY(EMMPM_LIBRARY_RELEASE 
  NAMES ${EMMPM_SEARCH_RELEASE_NAMES}
  PATHS ${EMMPM_LIB_SEARCH_DIRS} 
  NO_DEFAULT_PATH
  )
 


set(EMMPM_PROG_NAME "emmpm")
IF (WIN32)
    set(EMMPM_PROG_NAME "emmpm.exe")
ENDIF(WIN32)

FIND_PROGRAM(EMMPM_PROG
    NAMES ${EMMPM_PROG_NAME}
    PATHS ${EMMPM_BIN_SEARCH_DIRS} 
    NO_DEFAULT_PATH
)
MARK_AS_ADVANCED(EMMPM_PROG)

# include the macro to adjust libraries
INCLUDE (${CMP_MODULES_SOURCE_DIR}/cmpAdjustLibVars.cmake)
cmp_ADJUST_LIB_VARS(EMMPM)

# MESSAGE( STATUS "EMMPM_LIBRARY: ${EMMPM_LIBRARY}")

# Copy the results to the output variables.
IF(EMMPM_INCLUDE_DIR AND EMMPM_LIBRARY)
  SET(EMMPM_FOUND 1)
  SET(EMMPM_LIBRARIES ${EMMPM_LIBRARY})
  SET(EMMPM_INCLUDE_DIRS ${EMMPM_INCLUDE_DIR})
  IF (EMMPM_LIBRARY_DEBUG)
    GET_FILENAME_COMPONENT(EMMPM_LIBRARY_PATH ${EMMPM_LIBRARY_DEBUG} PATH)
    SET(EMMPM_LIB_DIR ${EMMPM_LIBRARY_PATH})
    SET(EMMPM_BIN_DIR ${EMMPM_LIBRARY_PATH})
  ELSEIF(EMMPM_LIBRARY_RELEASE)
    GET_FILENAME_COMPONENT(EMMPM_LIBRARY_PATH ${EMMPM_LIBRARY_RELEASE} PATH)
    SET(EMMPM_LIB_DIR ${EMMPM_LIBRARY_PATH})
    SET(EMMPM_BIN_DIR ${EMMPM_LIBRARY_PATH})
  ENDIF(EMMPM_LIBRARY_DEBUG)
  IF (EMMPM_PROG)
    GET_FILENAME_COMPONENT(EMMPM_BIN_PATH ${EMMPM_PROG} PATH)
    SET(EMMPM_BIN_PATH  ${EMMPM_BIN_PATH})
  ENDIF (EMMPM_PROG)
  
ELSE(EMMPM_INCLUDE_DIR AND EMMPM_LIBRARY)
  SET(EMMPM_FOUND 0)
  SET(EMMPM_LIBRARIES)
  SET(EMMPM_INCLUDE_DIRS)
ENDIF(EMMPM_INCLUDE_DIR AND EMMPM_LIBRARY)

# Report the results.
IF(NOT EMMPM_FOUND)
  SET(EMMPM_DIR_MESSAGE
    "EMMPM was not found. Make sure EMMPM_LIBRARY and EMMPM_INCLUDE_DIR are set or set the EMMPM_INSTALL environment variable.")
  IF(NOT EMMPM_FIND_QUIETLY)
    MESSAGE(STATUS "${EMMPM_DIR_MESSAGE}")
  ELSE(NOT EMMPM_FIND_QUIETLY)
    IF(EMMPM_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "EM/MPM was NOT found and is Required by this project")
    ENDIF(EMMPM_FIND_REQUIRED)
  ENDIF(NOT EMMPM_FIND_QUIETLY)
ENDIF(NOT EMMPM_FOUND)

IF(EMMPM_DEBUG)
 MESSAGE(STATUS "EMMPM_INCLUDE_DIRS: ${EMMPM_INCLUDE_DIRS}")
 MESSAGE(STATUS "EMMPM_LIBRARY_DEBUG: ${EMMPM_LIBRARY_DEBUG}")
 MESSAGE(STATUS "EMMPM_LIBRARY_RELEASE: ${EMMPM_LIBRARY_RELEASE}")
 MESSAGE(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
ENDIF(EMMPM_DEBUG)


IF (EMMPM_FOUND)
  INCLUDE(CheckSymbolExists)
  #############################################
  # Find out if EMMPM was built using dll's
  #############################################
  # Save required variable
  SET(CMAKE_REQUIRED_INCLUDES_SAVE ${CMAKE_REQUIRED_INCLUDES})
  SET(CMAKE_REQUIRED_FLAGS_SAVE    ${CMAKE_REQUIRED_FLAGS})
  # Add EMMPM_INCLUDE_DIR to CMAKE_REQUIRED_INCLUDES
  SET(CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES};${EMMPM_INCLUDE_DIRS}")

  CHECK_SYMBOL_EXISTS(EMMPM_BUILT_AS_DYNAMIC_LIB "emmpm/common/EMMPMConfiguration.h" HAVE_EMMPM_DLL)

  IF (HAVE_EMMPM_DLL STREQUAL "1")
    set(EMMPM_IS_SHARED 1 CACHE INTERNAL "EMMPM Built as DLL or Shared Library")
  ENDIF (HAVE_EMMPM_DLL STREQUAL "1")

  # Restore CMAKE_REQUIRED_INCLUDES and CMAKE_REQUIRED_FLAGS variables
  SET(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES_SAVE})
  SET(CMAKE_REQUIRED_FLAGS    ${CMAKE_REQUIRED_FLAGS_SAVE})
  #
  #############################################

ENDIF ()
