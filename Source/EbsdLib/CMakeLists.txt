#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2009, 2010, 2011 Michael A. Jackson. BlueQuartz Software
#--  Copyright (c) 2009, 2010, 2011 Michael Groeber, US Air Force Research Laboratory
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#--////////////////////////////////////////////////////////////////////////////

#///////////////////////////////////////////////////////////////////////////////
#// This code was partly written under US Air Force Contract FA8650-07-D-5800
#///////////////////////////////////////////////////////////////////////////////

if(DEFINED DREAM3DProj_BINARY_DIR)
    # set project's name
    PROJECT( EbsdLib )
    cmake_minimum_required(VERSION 2.8.9)

    set(EbsdLib_BUILT_AS_DYNAMIC_LIB)
    if(BUILD_SHARED_LIBS)
        set(EbsdLib_BUILT_AS_DYNAMIC_LIB 1)
        if(WIN32)
            ADD_DEFINITIONS("-DEbsdLib_BUILT_AS_DYNAMIC_LIB")
        endif(WIN32)
    endif(BUILD_SHARED_LIBS)

    set(GENERATE_GIT_VERSION 1)

elseif(DEFINED PVEbsd_BINARY_DIR)
    message(STATUS "EbsdLib Configuring in PVEbsd Plugin")
    set(EbsdLib_SOURCE_DIR ${PVEbsd_SOURCE_DIR}/Code/EbsdLib)
    set(PROJECT_RESOURCES_DIR ${PVEbsd_SOURCE_DIR}/Resources)
    set(CMP_SOURCE_DIR ${PVEbsd_SOURCE_DIR}/Support/cmp)
    set(EbsdLib_BUILT_AS_DYNAMIC_LIB 0)
    set(PROJECT_NAME "EbsdLib")
    set(PROJECT_CODE_DIR ${PVEbsd_SOURCE_DIR}/Code)
    set(EbsdLib_BINARY_DIR ${PVEbsd_BINARY_DIR}/EbsdLib)
    set(PROJECT_INSTALL_HEADERS 0)
    set(GENERATE_GIT_VERSION 0)
    set(H5Support_HDF5_NAMESPACE "PVEbsd")

endif()



if(NOT PROJECT_RESOURCES_DIR)
set(PROJECT_RESOURCES_DIR ${EbsdLib_SOURCE_DIR}/Resources)
endif()
if(NOT DEFINED CMP_SOURCE_DIR)
set(CMP_SOURCE_DIR ${EbsdLib_SOURCE_DIR}/Support/cmp)
endif()

set(PROJECT_PREFIX "Ebsd" CACHE STRING "The Prefix to be used for Preprocessor definitions")
set(EbsdLib_INSTALL_FILES 0)
#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the
# project.
#-------------------------------------------------------------------------------
set(CMP_HEADER_DIR ${EbsdLib_BINARY_DIR})
set(CMP_CONFIGURATION_FILE_NAME "EbsdLibConfiguration.h")
set(CMP_TYPES_FILE_NAME "EbsdLibTypes.h")
set(CMP_VERSION_HEADER_FILE_NAME "EbsdLibVersion.h")
set(CMP_VERSION_SOURCE_FILE_NAME "EbsdLibVersion.cpp")
set(CMP_TOP_HEADER_FILE "EbsdLib.h")
set(CMP_PROJECT_NAMESPACE "${PROJECT_NAME}")
set(CMP_PROJECT_NAME "${PROJECT_NAME}")
set(CMP_ENABLE_PLUGINS "0")
set(CMP_LIB_SEARCH_DIRS "")
set(CMP_GENERATE_VERSION_STRING ${GENERATE_GIT_VERSION})

# Extra Variables that need to be set before all the configured files are generated.
set(EbsdLib_HDF5_SUPPORT 1)
option(EbsdLib_ENABLE_HDF5 "Enable the use of HDF5 functionality" ON)
mark_as_advanced(EbsdLib_ENABLE_HDF5)

INCLUDE (${CMP_SOURCE_DIR}/cmpProject.cmake)

configure_file(${EbsdLib_SOURCE_DIR}/EbsdLibConfiguration.h.in
               ${EbsdLib_BINARY_DIR}/${CMP_TOP_HEADER_FILE})

# Setup the Include Directories
include_directories(${PROJECT_CODE_DIR})
include_directories(${EbsdLib_BINARY_DIR})

#-- Include all the Source Files
include( ${EbsdLib_SOURCE_DIR}/TSL/SourceList.cmake)
include( ${EbsdLib_SOURCE_DIR}/HKL/SourceList.cmake)
include( ${EbsdLib_SOURCE_DIR}/HEDM/SourceList.cmake)



set(EbsdLib_SRCS
    ${EbsdLib_SOURCE_DIR}/H5EbsdVolumeInfo.cpp
    ${EbsdLib_SOURCE_DIR}/AbstractEbsdFields.cpp
    ${EbsdLib_SOURCE_DIR}/EbsdReader.cpp
    ${EbsdLib_SOURCE_DIR}/H5EbsdVolumeReader.cpp
    )
set(EbsdLib_HDRS
    ${EbsdLib_SOURCE_DIR}/AbstractEbsdFields.h
    ${EbsdLib_SOURCE_DIR}/EbsdReader.h
    ${EbsdLib_SOURCE_DIR}/EbsdConstants.h
    ${EbsdLib_SOURCE_DIR}/EbsdHeaderEntry.h
    ${EbsdLib_SOURCE_DIR}/EbsdImporter.h
    ${EbsdLib_SOURCE_DIR}/EbsdLibDLLExport.h
    ${EbsdLib_SOURCE_DIR}/EbsdMacros.h
    ${EbsdLib_SOURCE_DIR}/EbsdSetGetMacros.h
    ${EbsdLib_SOURCE_DIR}/H5EbsdVolumeInfo.h
    ${EbsdLib_SOURCE_DIR}/H5EbsdVolumeReader.h
)
cmp_IDE_SOURCE_PROPERTIES( "EbsdLib/" "${EbsdLib_HDRS}" "${EbsdLib_SRCS}" ${PROJECT_INSTALL_HEADERS})

set(EbsdLib_PROJECT_SRCS
    ${EbsdLib_SRCS}
    ${EbsdLib_HDRS}
    ${TSL_SRCS}
    ${TSL_HDRS}
    ${HKL_SRCS}
    ${HKL_HDRS}
    ${HEDM_SRCS}
    ${HEDM_HDRS}
    ${EbsdLib_Utilities_SRCS}
    ${EbsdLib_Utilities_HDRS}
    ${EbsdLib_BINARY_DIR}/${CMP_VERSION_SOURCE_FILE_NAME}
)

#-- We ONLY want to add the actual library for the DREAM3D project. With the
#-- PVEbsd ParaView Plugin we are directly compiling everything into the plugin
#-- in order to reduce the deployment issues.
if(DEFINED DREAM3DProj_BINARY_DIR)

    add_library(${PROJECT_NAME} ${LIB_TYPE} ${EbsdLib_PROJECT_SRCS})

    target_link_libraries(${PROJECT_NAME} ${HDF5_LIBRARIES} H5Support ${QT_QTCORE_LIBRARY})

    LibraryProperties( ${PROJECT_NAME}  ${EXE_DEBUG_EXTENSION} )

    set(install_dir "tools")
    set(lib_install_dir "lib")
    if(WIN32)
        set(install_dir ".")
        set(lib_install_dir "")
    endif()


    if(BUILD_SHARED_LIBS AND NOT APPLE)
        INSTALL(TARGETS ${PROJECT_NAME}
            COMPONENT Applications
            RUNTIME DESTINATION ${install_dir}
            LIBRARY DESTINATION ${lib_install_dir}
            ARCHIVE DESTINATION ${lib_install_dir}
            BUNDLE DESTINATION "."
        )
    endif()

    add_subdirectory(${EbsdLib_SOURCE_DIR}/Test ${EbsdLib_BINARY_DIR}/Test)
endif(DEFINED DREAM3DProj_BINARY_DIR)
