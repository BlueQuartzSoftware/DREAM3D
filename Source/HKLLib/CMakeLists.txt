#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2011, Michael A. Jackson. BlueQuartz Software
#--  Copyright (c) 2011, Michael Groeber, US Air Force Research Laboratory
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#--////////////////////////////////////////////////////////////////////////////

#///////////////////////////////////////////////////////////////////////////////
#// This code was partly written under US Air Force Contract FA8650-07-D-5800
#///////////////////////////////////////////////////////////////////////////////

# set project's name
PROJECT( HKLLib )
cmake_minimum_required(VERSION 2.8)


SET (HKLLib_BUILT_AS_DYNAMIC_LIB)
IF (BUILD_SHARED_LIBS)
    SET (HKLLib_BUILT_AS_DYNAMIC_LIB 1)
    IF (WIN32)
        ADD_DEFINITIONS("-DHKLLib_BUILT_AS_DYNAMIC_LIB")
    ENDIF (WIN32)
ENDIF (BUILD_SHARED_LIBS)

SET (PROJECT_CODE_DIR ${PROJECT_SOURCE_DIR}/Source)
if (NOT PROJECT_RESOURCES_DIR)
    SET (PROJECT_RESOURCES_DIR ${HKLLib_SOURCE_DIR}/Resources)
endif()
if (NOT DEFINED CMP_SOURCE_DIR)
    set(CMP_SOURCE_DIR ${HKLLib_SOURCE_DIR}/Support/cmp)
endif()

SET (PROJECT_PREFIX "HKL" CACHE STRING "The Prefix to be used for Preprocessor definitions")
set (HKLLib_INSTALL_FILES 0)
#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the 
# project.
#------------------------------------------------------------------------------- 
set (CMP_HEADER_DIR ${HKLLib_BINARY_DIR})
set (CMP_CONFIGURATION_FILE_NAME "CMPConfiguration.h")
set (CMP_TYPES_FILE_NAME "HKLLibTypes.h")
set (CMP_VERSION_HEADER_FILE_NAME "HKLLibVersion.h")
set (CMP_EXTRA_CONFIGURATION_FILE "HKLLibConfiguration.h")
set (CMP_PROJECT_NAMESPACE "HKL")
set (CMP_PROJECT_NAME "HKL")
set (CMP_ENABLE_PLUGINS "0")
set (CMP_LIB_SEARCH_DIRS "")
INCLUDE (${CMP_SOURCE_DIR}/cmpProject.cmake)

set (CTF_SRCS
  #  ${HKLLib_SOURCE_DIR}/CtfDirectoryPatterns.cpp
    ${HKLLib_SOURCE_DIR}/CtfReader.cpp
  #  ${HKLLib_SOURCE_DIR}/CtfPhase.cpp
 )
 
set (CTF_HDRS
    ${HKLLib_SOURCE_DIR}/CtfConstants.h
     ${HKLLib_SOURCE_DIR}/CtfMacros.h
  #  ${HKLLib_SOURCE_DIR}/CtfDirectoryPatterns.h
  #  ${HKLLib_SOURCE_DIR}/CtfHeaderEntry.h
    ${HKLLib_SOURCE_DIR}/CtfReader.h
  #  ${HKLLib_SOURCE_DIR}/CtfPhase.h
  #  ${HKLLib_SOURCE_DIR}/CtfSetGetMacros.h
)

set (TSL_CTF_HDF5_SUPPORT 1)
option(HKLLib_ENABLE_HDF5 "Enable the use of HDF5 functionality" ON)
mark_as_advanced(HKLLib_ENABLE_HDF5)
if (HKLLib_ENABLE_HDF5)
    add_definitions(-DHKLLib_HAVE_HDF5)
    set (CTF_SRCS ${CTF_SRCS}
        ${HKLLib_SOURCE_DIR}/H5CtfImporter.cpp
        ${HKLLib_SOURCE_DIR}/H5CtfReader.cpp
    )
    set (CTF_HDRS ${CTF_HDRS}
        ${HKLLib_SOURCE_DIR}/H5CtfImporter.h
        ${HKLLib_SOURCE_DIR}/H5CtfReader.h
    )
    set (TSL_CTF_HDF5_SUPPORT 1)
endif()
cmp_IDE_SOURCE_PROPERTIES( "HKLLib" "${CTF_HDRS}" "${CTF_SRCS}" ${PROJECT_INSTALL_HEADERS})

# -- Include the Utiliy Sources
#include ( ${HKLLib_SOURCE_DIR}/Utilities/SourceList.cmake)

# --------------------------------------------------------------------
# Generate a Header file with Compile Version variables
# --------------------------------------------------------------------
set (VERSION_GEN_NAME "HKLLib")
set (VERSION_GEN_NAMESPACE "HKLLib")
set (PROJECT_PREFIX "HKLLib")
set (VERSION_GEN_COMPLETE  "1.0.0")
set (VERSION_GEN_VER_MAJOR "1")
set (VERSION_GEN_VER_MINOR "0")
set (VERSION_GEN_VER_PATCH "0")

configure_file(${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpVersion.h.in   
               ${HKLLib_BINARY_DIR}/HKLLibVersion.h  )


configure_file(${HKLLib_SOURCE_DIR}/HKLLibConfiguration.h.in
               ${HKLLib_BINARY_DIR}/HKLLibConfiguration.h)   

include_directories(${HKLLib_SOURCE_DIR}/..)            
include_directories(${HKLLib_SOURCE_DIR})
include_directories(${HKLLib_BINARY_DIR})

set(PROJECT_SRCS
    ${CTF_SRCS} 
    ${CTF_HDRS}
    ${HKLLib_Utilities_SRCS}
    ${HKLLib_Utilities_HDRS}
)

add_library(${PROJECT_NAME}  ${LIB_TYPE} ${PROJECT_SRCS})
if (HKLLib_ENABLE_HDF5)
    target_link_libraries(${PROJECT_NAME} ${HDF5_LIBRARIES} H5Support)
endif()

LibraryProperties( ${PROJECT_NAME}  ${EXE_DEBUG_EXTENSION} )

set(install_dir "tools")
set(lib_install_dir "lib")
if (WIN32)
    set (install_dir ".")
    set(lib_install_dir "")
endif()


if (BUILD_SHARED_LIBS AND NOT APPLE)
    INSTALL(TARGETS ${PROJECT_NAME}  
        COMPONENT Applications
        RUNTIME DESTINATION ${install_dir}
        LIBRARY DESTINATION ${lib_install_dir}
        ARCHIVE DESTINATION ${lib_install_dir}       
        BUNDLE DESTINATION "."
    ) 
endif() 
