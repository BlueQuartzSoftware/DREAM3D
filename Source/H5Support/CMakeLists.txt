# ============================================================================
# Copyright (c) 2009-2015 BlueQuartz Software, LLC
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
# contributors may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The code contained herein was partially funded by the followig contracts:
#    United States Air Force Prime Contract FA8650-07-D-5800
#    United States Air Force Prime Contract FA8650-10-D-5210
#    United States Prime Contract Navy N00173-07-C-2068
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

# set project's name
PROJECT( H5Support VERSION ${DREAM3DProj_VERSION_MAJOR}.${DREAM3DProj_VERSION_MINOR})

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
set(VERSION_GEN_HEADER_FILE_NAME "H5SupportVersion.h")

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
target_link_libraries(${PROJECT_NAME} hdf5 Qt5::Core )


LibraryProperties( ${PROJECT_NAME}  ${EXE_DEBUG_EXTENSION} )

set(install_dir "bin")
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




# ------- Enable the CTest testing. Use make test to run all tests ---------------
if(DREAM3D_BUILD_TESTING)
    add_subdirectory(${H5Support_SOURCE_DIR}/Test ${H5Support_BINARY_DIR}/Test)
endif()




