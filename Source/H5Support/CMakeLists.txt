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
PROJECT( H5Support )
cmake_minimum_required(VERSION 2.8.9)


set(H5Support_BUILT_AS_DYNAMIC_LIB)
if(BUILD_SHARED_LIBS)
    set(H5Support_BUILT_AS_DYNAMIC_LIB 1)
    if(WIN32)
        ADD_DEFINITIONS("-DH5Support_BUILT_AS_DYNAMIC_LIB")
    endif(WIN32)
endif(BUILD_SHARED_LIBS)

set(PROJECT_CODE_DIR ${PROJECT_SOURCE_DIR}/Source)
if(NOT PROJECT_RESOURCES_DIR)
set(PROJECT_RESOURCES_DIR ${H5Support_SOURCE_DIR}/Resources)
endif()
if(NOT DEFINED CMP_SOURCE_DIR)
set(CMP_SOURCE_DIR ${H5Support_SOURCE_DIR}/Support/cmp)
endif()

set(PROJECT_PREFIX "H5Support" CACHE STRING "The Prefix to be used for Preprocessor definitions")
set(H5Support_INSTALL_FILES 0)
#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the
# project.
#-------------------------------------------------------------------------------
set(CMP_HEADER_DIR ${H5Support_BINARY_DIR})
set(CMP_CONFIGURATION_FILE_NAME "H5SupportConfiguration.h")
set(CMP_TYPES_FILE_NAME "H5SupportTypes.h")
set(CMP_VERSION_HEADER_FILE_NAME "H5SupportVersion.h")
set(CMP_VERSION_SOURCE_FILE_NAME "H5SupportVersion.cpp")
set(CMP_TOP_HEADER_FILE "H5Support.h")
set(CMP_PROJECT_NAMESPACE "H5Support")
set(CMP_PROJECT_NAME "H5Support")
set(CMP_ENABLE_PLUGINS "0")
set(CMP_LIB_SEARCH_DIRS "")
INCLUDE (${CMP_SOURCE_DIR}/cmpProject.cmake)

#-------------------------------------------------------------------------------
# Set our H5Lite codes to also compile the Qt API version of H5Lite
#-------------------------------------------------------------------------------
set(H5Support_USE_QT 0)
option(H5Support_INCLUDE_QT_API "Include support for using Qt classes with H5Lite" ON)
if(H5Support_INCLUDE_QT_API)
  set(H5Support_USE_QT 1)
endif()
MARK_AS_ADVANCED(H5Support_INCLUDE_QT_API)
MARK_AS_ADVANCED(H5Support_USE_QT)

set(H5Support_SRCS
    ${H5Support_SOURCE_DIR}/H5Lite.cpp
    ${H5Support_SOURCE_DIR}/H5Utilities.cpp
    ${H5Support_SOURCE_DIR}/HDF5ScopedFileSentinel.cpp
 )

set(H5Support_HDRS
    ${H5Support_SOURCE_DIR}/H5Lite.h
    ${H5Support_SOURCE_DIR}/H5Utilities.h
    ${H5Support_SOURCE_DIR}/HDF5ScopedFileSentinel.h
    ${H5Support_SOURCE_DIR}/H5Macros.h
)

if(H5Support_USE_QT)
  set(H5Support_SRCS
    ${H5Support_SRCS}
    ${H5Support_SOURCE_DIR}/QH5Lite.cpp
    ${H5Support_SOURCE_DIR}/QH5Utilities.cpp
  )
  set(H5Support_HDRS
    ${H5Support_HDRS}
    ${H5Support_SOURCE_DIR}/QH5Lite.h
    ${H5Support_SOURCE_DIR}/QH5Utilities.h
  )
endif()

cmp_IDE_SOURCE_PROPERTIES( "H5Support" "${H5Support_HDRS}" "${H5Support_SRCS}" ${PROJECT_INSTALL_HEADERS})

# --------------------------------------------------------------------
# Generate a Header file with Compile Version variables
# --------------------------------------------------------------------
set(VERSION_GEN_NAME "H5Support")
set(VERSION_GEN_NAMESPACE "H5Support")
set(PROJECT_PREFIX "H5Support")
set(VERSION_GEN_COMPLETE  "1.0.0")
set(VERSION_GEN_VER_MAJOR "1")
set(VERSION_GEN_VER_MINOR "0")
set(VERSION_GEN_VER_PATCH "0")

configure_file(${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpVersion.h.in
               ${H5Support_BINARY_DIR}/H5SupportVersion.h  )
configure_file(${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpVersion.cpp.in
               ${H5Support_BINARY_DIR}/H5SupportVersion.cpp  )

configure_file(${H5Support_SOURCE_DIR}/H5SupportConfiguration.h.in
               ${H5Support_BINARY_DIR}/${CMP_TOP_HEADER_FILE})

include_directories(${H5Support_SOURCE_DIR}/..)
include_directories(${H5Support_SOURCE_DIR})
include_directories(${H5Support_BINARY_DIR})

set(PROJECT_SRCS
    ${H5Support_SRCS}
    ${H5Support_HDRS}
    ${H5Support_BINARY_DIR}/${CMP_VERSION_SOURCE_FILE_NAME}
)

add_library(${PROJECT_NAME} ${LIB_TYPE} ${PROJECT_SRCS})
target_link_libraries(${PROJECT_NAME} ${HDF5_LIBRARIES} ${QT_QTCORE_LIBRARY} )


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
