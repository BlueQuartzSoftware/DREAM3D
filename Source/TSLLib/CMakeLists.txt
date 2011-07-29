#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#--  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#--////////////////////////////////////////////////////////////////////////////

#///////////////////////////////////////////////////////////////////////////////
#// This code was partly written under US Air Force Contract FA8650-07-D-5800
#///////////////////////////////////////////////////////////////////////////////

# set project's name
PROJECT( TSLLib )
cmake_minimum_required(VERSION 2.8)


SET (TSLANG_BUILT_AS_DYNAMIC_LIB)
IF (BUILD_SHARED_LIBS)
    SET (TSLANG_BUILT_AS_DYNAMIC_LIB 1)
    IF (WIN32)
        ADD_DEFINITIONS("-DTSLLib_BUILT_AS_DYNAMIC_LIB")
    ENDIF (WIN32)
ENDIF (BUILD_SHARED_LIBS)

SET (PROJECT_CODE_DIR ${PROJECT_SOURCE_DIR}/Source)
if (NOT PROJECT_RESOURCES_DIR)
SET (PROJECT_RESOURCES_DIR ${TSLLib_SOURCE_DIR}/Resources)
endif()
if (NOT DEFINED CMP_SOURCE_DIR)
set(CMP_SOURCE_DIR ${TSLLib_SOURCE_DIR}/Support/cmp)
endif()

SET (PROJECT_PREFIX "TSL" CACHE STRING "The Prefix to be used for Preprocessor definitions")
set (TSLLib_INSTALL_FILES 0)
#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the 
# project.
#------------------------------------------------------------------------------- 
set (CMP_HEADER_DIR ${TSLLib_BINARY_DIR})
set (CMP_CONFIGURATION_FILE_NAME "CMPConfiguration.h")
set (CMP_TYPES_FILE_NAME "TSLLibTypes.h")
set (CMP_VERSION_HEADER_FILE_NAME "TSLLibVersion.h")
set (CMP_EXTRA_CONFIGURATION_FILE "TSLLibConfiguration.h")
set (CMP_PROJECT_NAMESPACE "TSL")
set (CMP_PROJECT_NAME "TSL")
set (CMP_ENABLE_PLUGINS "0")
set (CMP_LIB_SEARCH_DIRS "")
INCLUDE (${CMP_SOURCE_DIR}/cmpProject.cmake)

set (ANG_SRCS
    ${TSLLib_SOURCE_DIR}/AngDirectoryPatterns.cpp
    ${TSLLib_SOURCE_DIR}/AngReader.cpp
    ${TSLLib_SOURCE_DIR}/AngPhase.cpp
 )
 
set (ANG_HDRS
    ${TSLLib_SOURCE_DIR}/AngConstants.h
    ${TSLLib_SOURCE_DIR}/AngDirectoryPatterns.h
    ${TSLLib_SOURCE_DIR}/AngHeaderEntry.h
    ${TSLLib_SOURCE_DIR}/AngReader.h
    ${TSLLib_SOURCE_DIR}/AngPhase.h
    ${TSLLib_SOURCE_DIR}/AngSetGetMacros.h
)

set (TSL_ANG_HDF5_SUPPORT 1)
option(TSLLib_ENABLE_HDF5 "Enable the use of HDF5 functionality" ON)
mark_as_advanced(TSLLib_ENABLE_HDF5)
if (TSLLib_ENABLE_HDF5)
    add_definitions(-DTSLLib_HAVE_HDF5)
    set (ANG_SRCS ${ANG_SRCS}
        ${TSLLib_SOURCE_DIR}/H5AngImporter.cpp
        ${TSLLib_SOURCE_DIR}/H5AngReader.cpp
    )
    set (ANG_HDRS ${ANG_HDRS}
        ${TSLLib_SOURCE_DIR}/H5AngImporter.h
        ${TSLLib_SOURCE_DIR}/H5AngReader.h
    )
    set (TSL_ANG_HDF5_SUPPORT 1)
endif()
cmp_IDE_SOURCE_PROPERTIES( "TSLLib" "${ANG_HDRS}" "${ANG_SRCS}" ${PROJECT_INSTALL_HEADERS})

# -- Include the Utiliy Sources
include ( ${TSLLib_SOURCE_DIR}/Utilities/SourceList.cmake)

# --------------------------------------------------------------------
# Generate a Header file with Compile Version variables
# --------------------------------------------------------------------
set (VERSION_GEN_NAME "TSLLib")
set (VERSION_GEN_NAMESPACE "TSLLib")
set (PROJECT_PREFIX "TSLLib")
set (VERSION_GEN_COMPLETE  "1.0.0")
set (VERSION_GEN_VER_MAJOR "1")
set (VERSION_GEN_VER_MINOR "0")
set (VERSION_GEN_VER_PATCH "0")

configure_file(${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpVersion.h.in   
               ${TSLLib_BINARY_DIR}/TSLLibVersion.h  )


configure_file(${TSLLib_SOURCE_DIR}/TSLLibConfiguration.h.in
               ${TSLLib_BINARY_DIR}/TSLLibConfiguration.h)   

include_directories(${TSLLib_SOURCE_DIR}/..)            
include_directories(${TSLLib_SOURCE_DIR})
include_directories(${TSLLib_BINARY_DIR})

set(PROJECT_SRCS
    ${ANG_SRCS} 
    ${ANG_HDRS}
    ${TSLLib_Utilities_SRCS}
    ${TSLLib_Utilities_HDRS}
)

add_library(${PROJECT_NAME}  ${LIB_TYPE} ${PROJECT_SRCS})
if (TSLLib_ENABLE_HDF5)
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
