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

# - Find MXADataModel
# Find the native MXADATAMODEL headers and libraries.
#
#  MXADATAMODEL_INCLUDE_DIRS - where to find MXADataModel.h, etc.
#  MXADATAMODEL_LIBRARIES    - List of libraries when using MXADataModel.
#  MXADATAMODEL_LIBRARY_DEBUG - Debug version of Library
#  MXADATAMODEL_LIBRARY_RELEASE - Release Version of Library
#  MXADATAMODEL_FOUND        - True if MXADataModel found.
#  MXADATAMODEL_RESOURCES_DIR - The directory where supporting cmake files and other files can be found

# Look for the header file.

############################################
#
# Check the existence of the libraries.
#
############################################
# This macro was taken directly from the FindQt4.cmake file that is included
# with the CMake distribution. This is NOT my work. All work was done by the
# original authors of the FindQt4.cmake file. Only minor modifications were
# made to remove references to Qt and make this file more generally applicable
#########################################################################

MACRO (_adjust_lib_var_names basename)
  IF (${basename}_INCLUDE_DIR)

  # if only the release version was found, set the debug variable also to the release version
  IF (${basename}_LIBRARY_RELEASE AND NOT ${basename}_LIBRARY_DEBUG)
    SET(${basename}_LIBRARY_DEBUG ${${basename}_LIBRARY_RELEASE})
    SET(${basename}_LIBRARY       ${${basename}_LIBRARY_RELEASE})
    SET(${basename}_LIBRARIES     ${${basename}_LIBRARY_RELEASE})
  ENDIF (${basename}_LIBRARY_RELEASE AND NOT ${basename}_LIBRARY_DEBUG)

  # if only the debug version was found, set the release variable also to the debug version
  IF (${basename}_LIBRARY_DEBUG AND NOT ${basename}_LIBRARY_RELEASE)
    SET(${basename}_LIBRARY_RELEASE ${${basename}_LIBRARY_DEBUG})
    SET(${basename}_LIBRARY         ${${basename}_LIBRARY_DEBUG})
    SET(${basename}_LIBRARIES       ${${basename}_LIBRARY_DEBUG})
  ENDIF (${basename}_LIBRARY_DEBUG AND NOT ${basename}_LIBRARY_RELEASE)
  IF (${basename}_LIBRARY_DEBUG AND ${basename}_LIBRARY_RELEASE)
    # if the generator supports configuration types then set
    # optimized and debug libraries, or if the CMAKE_BUILD_TYPE has a value
    IF (CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      SET(${basename}_LIBRARY       optimized ${${basename}_LIBRARY_RELEASE} debug ${${basename}_LIBRARY_DEBUG})
    ELSE(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
      # if there are no configuration types and CMAKE_BUILD_TYPE has no value
      # then just use the release libraries
      SET(${basename}_LIBRARY       ${${basename}_LIBRARY_RELEASE} )
    ENDIF(CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE)
    SET(${basename}_LIBRARIES       optimized ${${basename}_LIBRARY_RELEASE} debug ${${basename}_LIBRARY_DEBUG})
  ENDIF (${basename}_LIBRARY_DEBUG AND ${basename}_LIBRARY_RELEASE)

  SET(${basename}_LIBRARY ${${basename}_LIBRARY} CACHE FILEPATH "The ${basename} library")

  IF (${basename}_LIBRARY)
    SET(${basename}_FOUND 1)
  ENDIF (${basename}_LIBRARY)

ENDIF (${basename}_INCLUDE_DIR )

  # Make variables changeble to the advanced user
  MARK_AS_ADVANCED(${basename}_LIBRARY ${basename}_LIBRARY_RELEASE ${basename}_LIBRARY_DEBUG ${basename}_INCLUDE_DIR )
ENDMACRO (_adjust_lib_var_names)

# Only set MXADATAMODEL_INSTALL to the environment variable if it is blank
if ("${MXADATAMODEL_INSTALL}" STREQUAL "")
    set(MXADATAMODEL_INSTALL  $ENV{MXADATAMODEL_INSTALL})
endif()
  
# MESSAGE (STATUS "Finding MXADataModel library and headers..." )
#set(MXADATAMODEL_DEBUG 0)

SET(MXADATAMODEL_INCLUDE_SEARCH_DIRS
  ${MXADATAMODEL_INSTALL}/include
)

set(MXADATAMODEL_LIB_SEARCH_DIRS
  ${MXADATAMODEL_INSTALL}/lib
  )

set(MXADATAMODEL_BIN_SEARCH_DIRS
  ${MXADATAMODEL_INSTALL}/bin
)

FIND_PATH(MXADATAMODEL_INCLUDE_DIR 
  NAMES MXA
  PATHS ${MXADATAMODEL_INCLUDE_SEARCH_DIRS}
  NO_DEFAULT_PATH
)

set(MXADATAMODEL_SEARCH_DEBUG_NAMES "MXADataModel_debug;MXADataModel_D;libMXADataModel_D;libMXADataModel_debug")
set(MXADATAMODEL_SEARCH_RELEASE_NAMES "MXADataModel;libMXADataModel")



IF (MXADATAMODEL_DEBUG)
    message (STATUS "MXADATAMODEL_INCLUDE_SEARCH_DIRS: ${MXADATAMODEL_INCLUDE_SEARCH_DIRS}")
    message (STATUS "MXADATAMODEL_LIB_SEARCH_DIRS: ${MXADATAMODEL_LIB_SEARCH_DIRS}")
    message (STATUS "MXADATAMODEL_BIN_SEARCH_DIRS: ${MXADATAMODEL_BIN_SEARCH_DIRS}")
    message (STATUS "MXADATAMODEL_SEARCH_RELEASE_NAMES: ${MXADATAMODEL_SEARCH_RELEASE_NAMES}")
    message (STATUS "MXADATAMODEL_SEARCH_DEBUG_NAMES: ${MXADATAMODEL_SEARCH_DEBUG_NAMES}")
    message (STATUS "MXADATAMODEL_INCLUDE_DIR: ${MXADATAMODEL_INCLUDE_DIR}")
ENDIF (MXADATAMODEL_DEBUG)

# Look for the library.
FIND_LIBRARY(MXADATAMODEL_LIBRARY_DEBUG 
  NAMES ${MXADATAMODEL_SEARCH_DEBUG_NAMES}
  PATHS ${MXADATAMODEL_LIB_SEARCH_DIRS} 
  NO_DEFAULT_PATH
  )
  
FIND_LIBRARY(MXADATAMODEL_LIBRARY_RELEASE 
  NAMES ${MXADATAMODEL_SEARCH_RELEASE_NAMES}
  PATHS ${MXADATAMODEL_LIB_SEARCH_DIRS} 
  NO_DEFAULT_PATH
  )
  
set(MXADATAMODEL_IMPORT_GENERATOR_PROG_NAME "ImportGenerator")
IF (WIN32)
    set(MXADATAMODEL_IMPORT_GENERATOR_PROG_NAME "ImportGenerator.exe")
ENDIF(WIN32)

FIND_PROGRAM(MXADATAMODEL_IMPORT_GENERATOR_PROG
    NAMES ${MXADATAMODEL_IMPORT_GENERATOR_PROG_NAME}
    PATHS ${MXADATAMODEL_BIN_SEARCH_DIRS} 
    NO_DEFAULT_PATH
)

_adjust_lib_var_names(MXADATAMODEL)

# MESSAGE( STATUS "MXADATAMODEL_LIBRARY: ${MXADATAMODEL_LIBRARY}")

# Copy the results to the output variables.
IF(MXADATAMODEL_INCLUDE_DIR AND MXADATAMODEL_LIBRARY)
  SET(MXADATAMODEL_FOUND 1)
  SET(MXADATAMODEL_LIBRARIES ${MXADATAMODEL_LIBRARY})
  SET(MXADATAMODEL_INCLUDE_DIRS ${MXADATAMODEL_INCLUDE_DIR})
  IF (MXADATAMODEL_LIBRARY_DEBUG)
    GET_FILENAME_COMPONENT(MXADATAMODEL_LIBRARY_PATH ${MXADATAMODEL_LIBRARY_DEBUG} PATH)
    SET(MXADATAMODEL_LIB_DIR ${MXADATAMODEL_LIBRARY_PATH})
    SET(MXADATAMODEL_BIN_DIR ${MXADATAMODEL_LIBRARY_PATH})
  ELSEIF(MXADATAMODEL_LIBRARY_RELEASE)
    GET_FILENAME_COMPONENT(MXADATAMODEL_LIBRARY_PATH ${MXADATAMODEL_LIBRARY_RELEASE} PATH)
    SET(MXADATAMODEL_LIB_DIR ${MXADATAMODEL_LIBRARY_PATH})
    SET(MXADATAMODEL_BIN_DIR ${MXADATAMODEL_LIBRARY_PATH})
  ENDIF(MXADATAMODEL_LIBRARY_DEBUG)
  
  IF (MXADATAMODEL_IMPORT_GENERATOR_PROG)
    GET_FILENAME_COMPONENT(MXADATAMODEL_BIN_PATH ${MXADATAMODEL_IMPORT_GENERATOR_PROG} PATH)
    SET(MXADATAMODEL_BIN_DIR  ${MXADATAMODEL_BIN_PATH})
  ENDIF (MXADATAMODEL_IMPORT_GENERATOR_PROG)
  
 # if (NOT MXADATAMODEL_RESOURCES_DIR)
    GET_FILENAME_COMPONENT(mxa_install ${MXADATAMODEL_LIB_DIR} PATH)
   # message(STATUS "mxa_install: ${mxa_install}")
    set(MXADATAMODEL_RESOURCES_DIR ${mxa_install}/share/MXADataModel/Resources)
    set(MXADATAMODEL_USE_FILE ${mxa_install}/share/MXADataModel/Resources/CMake/UseMXADataModel.cmake)
    
    #message(STATUS "MXADATAMODEL_RESOURCES_DIR: ${MXADATAMODEL_RESOURCES_DIR}")
 # endif (NOT MXADATAMODEL_RESOURCES_DIR)
  
ELSE(MXADATAMODEL_INCLUDE_DIR AND MXADATAMODEL_LIBRARY)
  SET(MXADATAMODEL_FOUND 0)
  SET(MXADATAMODEL_LIBRARIES)
  SET(MXADATAMODEL_INCLUDE_DIRS)
ENDIF(MXADATAMODEL_INCLUDE_DIR AND MXADATAMODEL_LIBRARY)

# Report the results.
IF(NOT MXADATAMODEL_FOUND)
  SET(MXADATAMODEL_DIR_MESSAGE
    "MXADATAMODEL was not found. Make sure MXADATAMODEL_LIBRARY and MXADATAMODEL_INCLUDE_DIR are set or set the MXADATAMODEL_INSTALL environment variable.")
  IF(NOT MXADATAMODEL_FIND_QUIETLY)
    MESSAGE(STATUS "${MXADATAMODEL_DIR_MESSAGE}")
  ELSE(NOT MXADATAMODEL_FIND_QUIETLY)
    IF(MXADATAMODEL_FIND_REQUIRED)
      # MESSAGE(FATAL_ERROR "${MXADATAMODEL_DIR_MESSAGE}")
      MESSAGE(FATAL_ERROR "MXADataModel was NOT found and is Required by this project")
    ENDIF(MXADATAMODEL_FIND_REQUIRED)
  ENDIF(NOT MXADATAMODEL_FIND_QUIETLY)
ENDIF(NOT MXADATAMODEL_FOUND)

IF(MXADATAMODEL_DEBUG)
 MESSAGE(STATUS "MXADATAMODEL_INCLUDE_DIRS: ${MXADATAMODEL_INCLUDE_DIRS}")
 MESSAGE(STATUS "MXADATAMODEL_LIBRARY_DEBUG: ${MXADATAMODEL_LIBRARY_DEBUG}")
 MESSAGE(STATUS "MXADATAMODEL_LIBRARY_RELEASE: ${MXADATAMODEL_LIBRARY_RELEASE}")
 MESSAGE(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
 message(STATUS "MXADATAMODEL_RESOURCES_DIR: ${MXADATAMODEL_RESOURCES_DIR}")
ENDIF(MXADATAMODEL_DEBUG)


IF (MXADATAMODEL_FOUND)
  INCLUDE(CheckSymbolExists)
  #############################################
  # Find out if MXADATAMODEL was build using dll's
  #############################################
  # Save required variable
  SET(CMAKE_REQUIRED_INCLUDES_SAVE ${CMAKE_REQUIRED_INCLUDES})
  SET(CMAKE_REQUIRED_FLAGS_SAVE    ${CMAKE_REQUIRED_FLAGS})
  # Add MXADATAMODEL_INCLUDE_DIR to CMAKE_REQUIRED_INCLUDES
  SET(CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES};${MXADATAMODEL_INCLUDE_DIRS}")

  CHECK_SYMBOL_EXISTS(MXA_BUILT_AS_DYNAMIC_LIB "MXA/MXAConfiguration.h" HAVE_MXADATAMODEL_DLL)

    IF (HAVE_MXADATAMODEL_DLL STREQUAL "TRUE")
        set(HAVE_MXADATAMODEL_DLL "1")
    ENDIF (HAVE_MXADATAMODEL_DLL STREQUAL "TRUE")

  # Restore CMAKE_REQUIRED_INCLUDES and CMAKE_REQUIRED_FLAGS variables
  SET(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES_SAVE})
  SET(CMAKE_REQUIRED_FLAGS    ${CMAKE_REQUIRED_FLAGS_SAVE})
  #
  #############################################

ENDIF (MXADATAMODEL_FOUND)
