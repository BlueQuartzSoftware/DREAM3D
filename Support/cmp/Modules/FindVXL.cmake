
# - Find VXL library
# - Derived from the FindVXL.cmake that is included with cmake
# Find the native VXL includes and library
# This module defines
#  VXL_INCLUDE_DIR, where to find VXL.h, etc.
#  VXL_LIBRARIES, libraries to link against to use VXL.
#  VXL_FOUND, If false, do not try to use VXL.
# also defined, but not for general use are
#  VXL_LIBRARY, where to find the VXL library.
#  VXL_LIBRARY_DEBUG - Debug version of VXL library
#  VXL_LIBRARY_RELEASE - Release Version of VXL library

# MESSAGE (STATUS "Finding VXL library and headers..." )

# Only set VXL_INSTALL to the environment variable if it is blank
if ("${VXL_INSTALL}" STREQUAL "")
    set(VXL_INSTALL  $ENV{VXL_INSTALL})
endif()

# Look for the header file.
SET(VXL_INCLUDE_SEARCH_DIRS
  ${VXL_INSTALL}/include
)

set(VXL_LIB_SEARCH_DIRS
  ${VXL_INSTALL}/lib
  )

set(VXL_BIN_SEARCH_DIRS
  ${VXL_INSTALL}/bin
)

FIND_PATH(VXL_VCL_INCLUDE_DIR 
  NAMES vcl_vector.h
  PATHS ${VXL_INCLUDE_SEARCH_DIRS}/vxl/vcl
  NO_DEFAULT_PATH
)

FIND_PATH(VXL_CORE_INCLUDE_DIR 
  NAMES vxl_config.h
  PATHS ${VXL_INCLUDE_SEARCH_DIRS}/vxl/core
  NO_DEFAULT_PATH
)

FIND_PATH(VXL_NETLIB_INCLUDE_DIR 
  NAMES v3p_netlib.h
  PATHS ${VXL_INCLUDE_SEARCH_DIRS}/vxl/v3p/netlib
  NO_DEFAULT_PATH
)

if (VXL_DEBUG)
message(STATUS "VXL_INCLUDE_DIR: ${VXL_INCLUDE_DIR}")
message(STATUS "VXL_VCL_INCLUDE_DIR: ${VXL_VCL_INCLUDE_DIR}")
message(STATUS "VXL_CORE_INCLUDE_DIR: ${VXL_CORE_INCLUDE_DIR}")
message(STATUS "VXL_NETLIB_INCLUDE_DIR: ${VXL_NETLIB_INCLUDE_DIR}")
endif()

macro(findVxlLibrary lib includeDir)
    STRING(TOUPPER ${lib} UPPER)
    # Look for the library.
    set(VXL_SEARCH_DEBUG_NAMES "${lib}_debug")
    set(VXL_SEARCH_RELEASE_NAMES "${lib}")
    set(VXL_${UPPER}_INCLUDE_DIR ${includeDir})
    
    
    FIND_LIBRARY(VXL_${UPPER}_LIBRARY_DEBUG 
      NAMES ${VXL_SEARCH_DEBUG_NAMES}
      PATHS ${VXL_LIB_SEARCH_DIRS} 
      NO_DEFAULT_PATH
      )
      
    FIND_LIBRARY(VXL_${UPPER}_LIBRARY_RELEASE 
      NAMES ${VXL_SEARCH_RELEASE_NAMES}
      PATHS ${VXL_LIB_SEARCH_DIRS} 
      NO_DEFAULT_PATH
      )
    
  #  message(STATUS "VXL_${UPPER}_LIBRARY_DEBUG: ${VXL_${UPPER}_LIBRARY_DEBUG}")
  #  message(STATUS "VXL_${UPPER}_LIBRARY_RELEASE: ${VXL_${UPPER}_LIBRARY_RELEASE}")
    # include the macro to adjust libraries
    INCLUDE (${CMP_MODULES_SOURCE_DIR}/cmpAdjustLibVars.cmake)
    cmp_ADJUST_LIB_VARS( VXL_${UPPER} )
    
    # Group all the libraries together that we have found
    set(VXL_LIBRARIES ${VXL_LIBRARIES} ${VXL_${UPPER}_LIBRARY})
    
endmacro(findVxlLibrary lib)


findVxlLibrary(vcl ${VXL_VCL_INCLUDE_DIR})
# Find the Core Libraries
findVxlLibrary(vnl_algo ${VXL_CORE_INCLUDE_DIR})
findVxlLibrary(vnl ${VXL_CORE_INCLUDE_DIR})
findVxlLibrary(vpl ${VXL_CORE_INCLUDE_DIR})
findVxlLibrary(vul ${VXL_CORE_INCLUDE_DIR})
findVxlLibrary(vbl ${VXL_CORE_INCLUDE_DIR})
# Find the 3rd Party Libraries
findVxlLibrary(netlib ${VXL_NETLIB_INCLUDE_DIR})
findVxlLibrary(v3p_netlib ${VXL_NETLIB_INCLUDE_DIR})

set(VXL_INCLUDE_DIRS
    ${VXL_VCL_INCLUDE_DIR}
    ${VXL_CORE_INCLUDE_DIR}
    ${VXL_NETLIB_INCLUDE_DIR}
)

IF(VXL_INCLUDE_DIRS AND VXL_LIBRARIES)
  SET(VXL_FOUND 1)
  IF (VXL_VCL_LIBRARY_DEBUG)
    GET_FILENAME_COMPONENT(VXL_VCL_LIBRARY_PATH ${VXL_VCL_LIBRARY_DEBUG} PATH)
    SET(VXL_LIB_DIR  ${VXL_VCL_LIBRARY_PATH})
  ELSEIF(VXL_VCL_LIBRARY_RELEASE)
    GET_FILENAME_COMPONENT(VXL_VCL_LIBRARY_PATH ${VXL_VCL_LIBRARY_RELEASE} PATH)
    SET(VXL_LIB_DIR  ${VXL_VCL_LIBRARY_PATH})
  ENDIF(VXL_VCL_LIBRARY_DEBUG)
ELSE()
  SET(VXL_FOUND 0)
  SET(VXL_LIBRARIES)
  SET(VXL_INCLUDE_DIRS)
ENDIF()

# Report the results.
IF(NOT VXL_FOUND)
  SET(VXL_DIR_MESSAGE
    "VXL was not found. Make sure VXL_LIBRARY and VXL_INCLUDE_DIR are set or set the TIIF_INSTALL environment variable.")
  IF(NOT VXL_FIND_QUIETLY)
    MESSAGE(STATUS "${VXL_DIR_MESSAGE}")
  ELSE(NOT VXL_FIND_QUIETLY)
    IF(VXL_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "VXL was NOT found and is Required by this project")
    ENDIF(VXL_FIND_REQUIRED)
  ENDIF(NOT VXL_FIND_QUIETLY)
ENDIF(NOT VXL_FOUND)

IF (VXL_FOUND)
  INCLUDE(CheckSymbolExists)
  #############################################
  # Find out if VXL was build using dll's
  #############################################
  # Save required variable
  SET(CMAKE_REQUIRED_INCLUDES_SAVE ${CMAKE_REQUIRED_INCLUDES})
  SET(CMAKE_REQUIRED_FLAGS_SAVE    ${CMAKE_REQUIRED_FLAGS})
  # Add VXL_INCLUDE_DIR to CMAKE_REQUIRED_INCLUDES
  SET(CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES};${VXL_INCLUDE_DIRS}")

  CHECK_SYMBOL_EXISTS(VXL_BUILD_SHARED_LIBS "vxl_config.h" HAVE_VXL_DLL)
  if (HAVE_VXL_DLL)
    set(VXL_IS_SHARED 1 CACHE INTERNAL "VXL Built as DLL or Shared Library")
  endif()

  # Restore CMAKE_REQUIRED_INCLUDES and CMAKE_REQUIRED_FLAGS variables
  SET(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES_SAVE})
  SET(CMAKE_REQUIRED_FLAGS    ${CMAKE_REQUIRED_FLAGS_SAVE})
  #
  #############################################
ENDIF (VXL_FOUND)
