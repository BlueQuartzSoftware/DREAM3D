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
PROJECT( TSLAngLib )
cmake_minimum_required(VERSION 2.8)


SET (TSLANG_BUILT_AS_DYNAMIC_LIB)
IF (BUILD_SHARED_LIBS)
    SET (TSLANG_BUILT_AS_DYNAMIC_LIB 1)
    IF (WIN32)
        ADD_DEFINITIONS("-DTSLANG_BUILT_AS_DYNAMIC_LIB")
    ENDIF (WIN32)
ENDIF (BUILD_SHARED_LIBS)

SET (PROJECT_CODE_DIR ${PROJECT_SOURCE_DIR}/Source)
if (NOT PROJECT_RESOURCES_DIR)
SET (PROJECT_RESOURCES_DIR ${TSLAngLib_SOURCE_DIR}/Resources)
endif()
if (NOT DEFINED CMP_SOURCE_DIR)
set(CMP_SOURCE_DIR ${TSLAngLib_SOURCE_DIR}/Support/cmp)
endif()

SET (PROJECT_PREFIX "ANG" CACHE STRING "The Prefix to be used for Preprocessor definitions")
set (TSLAngLib_INSTALL_FILES 0)
#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the 
# project.
#------------------------------------------------------------------------------- 
set (CMP_HEADER_DIR ${TSLAngLib_BINARY_DIR})
set (CMP_CONFIGURATION_FILE_NAME "CMPConfiguration.h")
set (CMP_TYPES_FILE_NAME "AngTypes.h")
set (CMP_VERSION_HEADER_FILE_NAME "AngVersion.h")
set (CMP_EXTRA_CONFIGURATION_FILE "TSLAngLibConfiguration.h")
set (CMP_PROJECT_NAMESPACE "ANG")
set (CMP_PROJECT_NAME "ANG")
set (CMP_ENABLE_PLUGINS "0")
set (CMP_LIB_SEARCH_DIRS "")
INCLUDE (${CMP_SOURCE_DIR}/cmpProject.cmake)

set (ANG_SRCS
    ${TSLAngLib_SOURCE_DIR}/AngDirectoryPatterns.cpp
    ${TSLAngLib_SOURCE_DIR}/AngReader.cpp
    ${TSLAngLib_SOURCE_DIR}/AngPhase.cpp
 )
 
set (ANG_HDRS
    ${TSLAngLib_SOURCE_DIR}/AngConstants.h
    ${TSLAngLib_SOURCE_DIR}/AngDirectoryPatterns.h
    ${TSLAngLib_SOURCE_DIR}/AngHeaderEntry.h
    ${TSLAngLib_SOURCE_DIR}/AngReader.h
    ${TSLAngLib_SOURCE_DIR}/AngPhase.h
    ${TSLAngLib_SOURCE_DIR}/AngSetGetMacros.h
)

set (TSL_ANG_HDF5_SUPPORT 0)
option(TSLAngLib_ENABLE_HDF5 "Enable the use of HDF5 functionality" ON)
mark_as_advanced(TSLAngLib_ENABLE_HDF5)
if (TSLAngLib_ENABLE_HDF5)
    add_definitions(-DTSLAngLib_HAVE_HDF5)
    set (ANG_SRCS ${ANG_SRCS}
        ${TSLAngLib_SOURCE_DIR}/H5AngImporter.cpp
        ${TSLAngLib_SOURCE_DIR}/H5AngReader.cpp
    )
    set (ANG_HDRS ${ANG_HDRS}
        ${TSLAngLib_SOURCE_DIR}/H5AngImporter.h
        ${TSLAngLib_SOURCE_DIR}/H5AngReader.h
    )
    include (${TSLAngLib_SOURCE_DIR}/HDF5/SourceList.cmake)
    set (TSL_ANG_HDF5_SUPPORT 1)
endif()
cmp_IDE_SOURCE_PROPERTIES( "TSLAngLib" "${ANG_HDRS}" "${ANG_SRCS}" ${PROJECT_INSTALL_HEADERS})

# -- Include the Utiliy Sources
include ( ${TSLAngLib_SOURCE_DIR}/Utilities/SourceList.cmake)

# --------------------------------------------------------------------
# Generate a Header file with Compile Version variables
# --------------------------------------------------------------------
cmpGenerateVersionString( "${TSLAngLib_BINARY_DIR}/${PROJECT_NAME}Version.h" 
                          "TSLAngLib" 
                          "TSLAngLib")
configure_file(${TSLAngLib_SOURCE_DIR}/TSLAngLibConfiguration.h.in
               ${TSLAngLib_BINARY_DIR}/TSLAngLibConfiguration.h)   

include_directories(${TSLAngLib_SOURCE_DIR}/..)            
include_directories(${TSLAngLib_SOURCE_DIR})
include_directories(${TSLAngLib_BINARY_DIR})

set(PROJECT_SRCS
    ${ANG_SRCS} 
    ${ANG_HDRS}
    ${TSLAngLib_Utilities_SRCS}
    ${TSLAngLib_Utilities_HDRS}
    ${ANG_HDF5_SRCS}
    ${ANG_HDF5_HDRS}
)

add_library(TSLAng  ${LIB_TYPE} ${PROJECT_SRCS})
if (TSLAngLib_ENABLE_HDF5)
    target_link_libraries(TSLAng ${HDF5_LIBRARIES} )
endif()

LibraryProperties( TSLAng ${EXE_DEBUG_EXTENSION} )

set(install_dir "tools")
set(lib_install_dir "lib")
if (WIN32)
    set (install_dir ".")
    set(lib_install_dir "")
endif()


if (BUILD_SHARED_LIBS)
    INSTALL(TARGETS TSLAng 
        COMPONENT Applications
        RUNTIME DESTINATION ${install_dir}
        LIBRARY DESTINATION ${lib_install_dir}
        ARCHIVE DESTINATION ${lib_install_dir}       
        BUNDLE DESTINATION "."
    ) 
endif() 
