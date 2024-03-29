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
# The code contained herein was partially funded by the following contracts:
#    United States Air Force Prime Contract FA8650-07-D-5800
#    United States Air Force Prime Contract FA8650-10-D-5210
#    United States Prime Contract Navy N00173-07-C-2068
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
# set project's name
PROJECT( EMMPMLib VERSION 1.0)
cmake_minimum_required(VERSION 3.14.0)

if(${CMAKE_MAJOR_VERSION} STREQUAL "3")

endif()

if(NOT DEFINED DREAM3DProj_BINARY_DIR)

  # ---------- Setup output Directories -------------------------
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/Bin
    CACHE PATH
    "Single Directory for all Libraries"
    )

  # --------- Setup the Executable output Directory -------------
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/Bin
    CACHE PATH
    "Single Directory for all Executables."
    )

  # --------- Setup the Executable output Directory -------------
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/Bin
    CACHE PATH
    "Single Directory for all static libraries."
    )

  set(EXE_DEBUG_EXTENSION "_debug")
  set(PROJECT_INSTALL_HEADERS "0")
  set(PROJECT_INSTALL_EXECUTABLES "1")

endif()

if(NOT PROJECT_RESOURCES_DIR)
  set(PROJECT_RESOURCES_DIR ${EMMPMLib_SOURCE_DIR}/Resources)
endif()
if(NOT DEFINED CMP_SOURCE_DIR)
  set(CMP_SOURCE_DIR ${EMMPMLib_SOURCE_DIR}/Support/cmp)
endif()

# Bring in our Macros from the CMP project
INCLUDE (${CMP_SOURCE_DIR}/cmpCMakeMacros.cmake )


set(EMMPMLib_BUILT_AS_DYNAMIC_LIB 0)
if(BUILD_SHARED_LIBS)
  set(EMMPMLib_BUILT_AS_DYNAMIC_LIB 1)
endif(BUILD_SHARED_LIBS)


set(EMMPMLib_HDF5_SUPPORT "1")
#option(EMMPMLib_ENABLE_HDF5 "Enable HDF5 Support in the EMMPMLib" ON)
if(SIMPL_USE_MULTITHREADED_ALGOS)
  set(EMMPM_USE_PARALLEL_ALGORITHMS "1")
else()
  set(EMMPM_USE_PARALLEL_ALGORITHMS "0")
endif()

mark_as_advanced(EMMPMLib_HDF5_SUPPORT)
mark_as_advanced(EMMPMLib_ENABLE_HDF5)
mark_as_advanced(EMMPM_USE_PARALLEL_ALGORITHMS)

set(PROJECT_PREFIX "EMMPMLib" CACHE STRING "The Prefix to be used for Preprocessor definitions")
set(EMMPMLib_INSTALL_FILES 0)
#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the
# project.
#-------------------------------------------------------------------------------
if(NOT DEFINED DREAM3DProj_BINARY_DIR)
  set(CMP_HEADER_DIR ${EMMPMLib_BINARY_DIR})
else()
  set(CMP_HEADER_DIR ${EMMPMLib_BINARY_DIR})
endif()
set(CMP_CONFIGURATION_FILE_NAME "EMMPMLibConfiguration.h")
set(CMP_TYPES_FILE_NAME "EMMPMLibTypes.h")
set(CMP_VERSION_HEADER_FILE_NAME "EMMPMLibVersion.h")
set(CMP_VERSION_SOURCE_FILE_NAME "EMMPMLibVersion.cpp")
set(CMP_TOP_HEADER_FILE "EMMPMLib.h")
set(CMP_PROJECT_NAMESPACE "${PROJECT_NAME}")
set(CMP_PROJECT_NAME "${PROJECT_NAME}")
set(CMP_ENABLE_PLUGINS "0")
set(CMP_LIB_SEARCH_DIRS "")
set(GENERATE_GIT_VERSION 1)
set(CMP_GENERATE_VERSION_STRING ${GENERATE_GIT_VERSION})

INCLUDE (${CMP_SOURCE_DIR}/cmpProject.cmake)

if(NOT DEFINED DREAM3DProj_BINARY_DIR)
  cmpConfigureFileWithMD5Check(CONFIGURED_TEMPLATE_PATH ${EMMPMLib_SOURCE_DIR}/EMMPMLibConfiguration.h.in
                            GENERATED_FILE_PATH ${CMP_HEADER_DIR}/${CMP_TOP_HEADER_FILE})
else()
  cmpConfigureFileWithMD5Check(CONFIGURED_TEMPLATE_PATH ${EMMPMLib_SOURCE_DIR}/EMMPMLibConfiguration.h.in
                            GENERATED_FILE_PATH ${EMMPMLib_BINARY_DIR}/${CMP_TOP_HEADER_FILE})
endif()


# Setup the Include Directories
# NOTE that we are using the Plugin's CMake variables because we want to be
# able to use an include path like #include "EMMPMLib/foo.h"

#-- Include all the Source Files
include( ${EMMPMLib_SOURCE_DIR}/Common/SourceList.cmake)
include( ${EMMPMLib_SOURCE_DIR}/Core/SourceList.cmake)

set(EMMPMLib_SRCS

)
set(EMMPMLib_HDRS
    ${EMMPMLib_SOURCE_DIR}/EMMPMLibDLLExport.h
)
cmp_IDE_SOURCE_PROPERTIES( "EMMPMLib/" "${EMMPMLib_HDRS}" "${EMMPMLib_SRCS}" ${PROJECT_INSTALL_HEADERS})

cmp_IDE_GENERATED_PROPERTIES("Generated"
                             "${EMMPMLib_BINARY_DIR}/${CMP_TOP_HEADER_FILE};${EMMPMLib_BINARY_DIR}/${PROJECT_NAME}/${CMP_CONFIGURATION_FILE_NAME};${EMMPMLib_BINARY_DIR}/${PROJECT_NAME}/${CMP_TYPES_FILE_NAME};${EMMPMLib_BINARY_DIR}/${PROJECT_NAME}/${CMP_VERSION_HEADER_FILE_NAME}"
                             "${EMMPMLib_Core_Generated_MOC_SRCS};${EMMPMLib_BINARY_DIR}/${PROJECT_NAME}/${CMP_VERSION_SOURCE_FILE_NAME}")


set(EMMPMLib_PROJECT_SRCS
    ${EMMPMLib_Common_SRCS}
    ${EMMPMLib_Common_HDRS}
    ${EMMPMLib_Core_HDRS}
    ${EMMPMLib_Core_SRCS}
    ${EMMPMLib_Core_Moc_HDRS}

    #-- Our Generated Header Files
    ${EMMPMLib_BINARY_DIR}/${CMP_TOP_HEADER_FILE}
    ${EMMPMLib_BINARY_DIR}/${CMP_CONFIGURATION_FILE_NAME}
    ${EMMPMLib_BINARY_DIR}/${CMP_TYPES_FILE_NAME}
    ${EMMPMLib_BINARY_DIR}/${CMP_VERSION_HEADER_FILE_NAME}
    ${EMMPMLib_BINARY_DIR}/${CMP_VERSION_SOURCE_FILE_NAME}
    ${EMMPMLib_Core_Generated_MOC_SRCS}
)

add_library(${PROJECT_NAME} ${LIB_TYPE} ${EMMPMLib_PROJECT_SRCS})
CMP_AddDefinitions(TARGET ${PROJECT_NAME})
LibraryProperties( ${PROJECT_NAME} ${EXE_DEBUG_EXTENSION} )
set_target_properties( ${PROJECT_NAME} PROPERTIES FOLDER ${PLUGIN_NAME}Plugin)

if(WIN32 AND BUILD_SHARED_LIBS)
  target_compile_definitions(${PROJECT_NAME} PUBLIC "-DEMMPMLib_BUILT_AS_DYNAMIC_LIB")
endif()
#-- Configure Target Specific Include Directories
get_filename_component(TARGET_BINARY_DIR_PARENT ${${PROJECT_NAME}_BINARY_DIR} PATH)
get_filename_component(TARGET_SOURCE_DIR_PARENT ${${PROJECT_NAME}_SOURCE_DIR} PATH)

target_include_directories(${PROJECT_NAME} PUBLIC ${TARGET_BINARY_DIR_PARENT})
target_include_directories(${PROJECT_NAME} PUBLIC ${TARGET_SOURCE_DIR_PARENT})
target_include_directories(${PROJECT_NAME} PUBLIC ${TBB_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${EIGEN_INCLUDE_DIRS} )
CMP_MODULE_INCLUDE_DIRS (TARGET ${PROJECT_NAME} LIBVARS HDF5 Qt5Core Qt5Network)

target_link_libraries(${PROJECT_NAME} Qt5::Core ${TBB_LIBRARIES} SIMPLib)


set(install_dir "bin")
set(lib_install_dir "lib")
if(WIN32)
    set(install_dir ".")
    set(lib_install_dir ".")
endif()

if(NOT DREAM3D_ANACONDA)
  if(BUILD_SHARED_LIBS AND NOT APPLE)
    install(TARGETS ${PROJECT_NAME}
      COMPONENT Applications
      RUNTIME DESTINATION ${install_dir}
      LIBRARY DESTINATION ${lib_install_dir}
      ARCHIVE DESTINATION ${lib_install_dir}
      BUNDLE DESTINATION "."
    )
  endif()
else()
  if(WIN32)
    install(TARGETS ${PROJECT_NAME}
      RUNTIME DESTINATION bin
    )
  else()
    install(TARGETS ${PROJECT_NAME}
      LIBRARY DESTINATION lib
    )
  endif()
endif()



