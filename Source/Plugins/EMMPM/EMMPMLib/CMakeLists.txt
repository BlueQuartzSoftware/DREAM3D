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
# set project's name
PROJECT( EMMPMLib )
cmake_minimum_required(VERSION 3.1.0)

if(${CMAKE_MAJOR_VERSION} STREQUAL "3")
  # CMake 3.0 introduces VERSION variables for each Project. We do our own version numbering so we need to have
  # cmake ignore that policy
  cmake_policy(SET CMP0048 OLD)
  # CMake 3.0 introduces some OS X RPath policies
  cmake_policy(SET CMP0042 OLD)
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
  if(WIN32)
    ADD_DEFINITIONS("-DEMMPMLib_BUILT_AS_DYNAMIC_LIB")
  endif(WIN32)
endif(BUILD_SHARED_LIBS)


# Look for our Prerequisite Libraries
if(NOT DEFINED DREAM3DProj_BINARY_DIR)

  # ------------------------------------------------------------------------------
  #  Qt 4 Section
  # ------------------------------------------------------------------------------
  # by default only QtCore and QtGui modules are enabled
  # other modules must be enabled like this:
  if(WIN32)
     set(QT_USE_QTMAIN TRUE)
  ENDif(WIN32)

  # this command finds Qt4 libraries and sets all required variables
  # note that it's Qt4, not QT4 or qt4
  FIND_PACKAGE( Qt4 REQUIRED )
  if(QT4_FOUND)
     CMP_COPY_QT4_RUNTIME_LIBRARIES( "QtCore")
     CMP_QT_LIBRARIES_INSTALL_RULES("QtCore" ".")
  else()
    message(FATAL_ERROR "Qt version 4.8 Core Library is REQUIRED to build EMMPMLib")
  endif()

  set(QT_USE_QTCORE "TRUE")

  # ---------- Find Boost Headers/Libraries -----------------------
  #set(Boost_FIND_REQUIRED FALSE)
  set(Boost_DEBUG FALSE)
  set(Boost_USE_MULTITHREADED TRUE)
  set(Boost_USE_STATIC_LIBS TRUE)
  set(Boost_ADDITIONAL_VERSIONS 1.55.0)
  set(Boost_FIND_COMPONENTS "")
  set(Boost_NO_BOOST_CMAKE 1)
  FIND_PACKAGE(Boost)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
  LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

else()
  set(EMMPMLib_HDF5_SUPPORT "1")
  option(EMMPMLib_ENABLE_HDF5 "Enable HDF5 Support in the EMMPMLib" ON)
endif()

mark_as_advanced(EMMPMLib_HDF5_SUPPORT)
mark_as_advanced(EMMPMLib_ENABLE_HDF5)


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
## NOTE that we are using the Plugin's CMake variables because we want to be
## able to use an include path like #include "EMMPMLib/foo.h"
include_directories(${EMMPM_SOURCE_DIR})
include_directories(${EMMPM_BINARY_DIR})

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
target_link_libraries(${PROJECT_NAME} Qt5::Core ${TBB_LIBRARIES} SIMPLib)

LibraryProperties( ${PROJECT_NAME} ${EXE_DEBUG_EXTENSION} )

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


