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


if(NOT "${DREAM3D_SDK}" STREQUAL "")
  include("${DREAM3D_SDK}/DREAM3D_SDK.cmake")
  message(STATUS "DREAM3D_SDK Location: ${DREAM3D_SDK}")
endif()

if(${CMAKE_MAJOR_VERSION} STREQUAL "3")
  # CMake 3.0 introduces VERSION variables for each Project.
  cmake_policy(SET CMP0048 NEW)
  # CMake 3.0 introduces some OS X RPath policies
  cmake_policy(SET CMP0042 OLD)
  # CMake 3.1 introduces if() policies on dereferencing variables in quotes
  cmake_policy(SET CMP0054 NEW)
endif()

cmake_minimum_required(VERSION 3.1.0)

#------------------------------------------------------------------------------
# Create the Top level Project
#
project(DREAM3DProj VERSION 6.2.0.0)
#------------------------------------------------------------------------------
# There are several different Release Types that we are going to use.
# Official: This is an official release from BlueQuartz Software
# Beta: This is a Beta release that has is getting ready to be officially released.
# Development: This is currently in development and probably has bugs.
set(DREAM3DProj_RELEASE_TYPE "Development")

include(CMakeParseArguments)

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


#Enable use of Solution Folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
#set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
set(PROJECT_CODE_DIR ${PROJECT_SOURCE_DIR}/Source)
set(PROJECT_RESOURCES_DIR ${DREAM3DProj_SOURCE_DIR}/Resources)
set(EXE_DEBUG_EXTENSION "_debug")
set(PROJECT_INSTALL_HEADERS "0")
set(PROJECT_INSTALL_EXECUTABLES "1")
set(DREAM3D_DOCUMENTATION_DIR ${PROJECT_SOURCE_DIR}/Documentation)
set(DREAM3D_SUPPORT_DIR ${PROJECT_SOURCE_DIR}/Support)

set(CMP_SOURCE_DIR ${DREAM3DProj_SOURCE_DIR}/Support/cmp)
set(CMP_CONFIGURED_FILES_SOURCE_DIR ${CMP_SOURCE_DIR}/ConfiguredFiles)
INCLUDE (${CMP_SOURCE_DIR}/cmpCMakeMacros.cmake )
set(CMP_ENABLE_PLUGINS "1")
set(CMP_PLUGIN_LIST_FILE ${PROJECT_BINARY_DIR}/SIMPLib_PluginList.txt)
file(WRITE ${CMP_PLUGIN_LIST_FILE} "")
set(CMP_PLUGIN_SEARCHDIR_FILE ${PROJECT_BINARY_DIR}/libsearchdirs.txt)
file(WRITE ${CMP_PLUGIN_SEARCHDIR_FILE} "${PROJECT_BINARY_DIR}/Bin/plugins;")
file(APPEND ${CMP_PLUGIN_SEARCHDIR_FILE} "${PROJECT_BINARY_DIR}/Bin;")
set(CMP_LIB_SEARCH_DIRS "")
set(CMP_MODULES_SOURCE_DIR ${CMP_SOURCE_DIR}/Modules CACHE INTERNAL "")

if(UNIX AND NOT APPLE)
    set(DREAM3D_LINUX_SYSTEM ${CMAKE_SYSTEM_NAME} CACHE STRING "The Name of the linux distrubution for packaging")
endif()

#------------------------------------------------------------------------------
# Find the Git Package for Versioning. It should be ok if Git is NOT found
Find_package(Git)

cmpRevisionString( GENERATED_HEADER_FILE_PATH "D3DFileNotUsed.h"
                        GENERATED_SOURCE_FILE_PATH "D3DFileNotUsed.cpp"
                        NAMESPACE "DREAM3DProj"
                        PROJECT_NAME "${PROJECT_NAME}"
                        EXPORT_MACRO "DREAM3DProj_EXPORT")

# --------------------------------------------------------------------
#
if("${DREAM3D_DATA_DIR}" STREQUAL "")
  set(DREAM3D_DATA_DIR "${DREAM3DProj_SOURCE_DIR}/../SIMPLib_Data/"  CACHE PATH "The directory where to find test data files")
endif()

if("${DREAM3D_DATA_DIR}" STREQUAL "")
   message(FATAL_ERROR "The DREAM3D_DATA_DIR variable is not set and it needs to be set to a location where the test data is located")
endif()

# --------------------------------------------------------------------
# This is set to "1" so that our test files are deleted at the end of the tests
set(DREAM3D_REMOVE_TEST_FILES "1")

# --------------------------------------------------------------------
# Over ride CMake's built in module directory by prepending cmp's module
# directory first
set(CMAKE_MODULE_PATH ${CMP_MODULES_SOURCE_DIR} ${CMAKE_MODULE_PATH} ${Qt5_DIR})

# Build shared libraries
OPTION (BUILD_SHARED_LIBS "Build Shared Libraries" ON)
set(LIB_TYPE STATIC)
if(BUILD_SHARED_LIBS)
  #message(FATAL_ERROR "Due to issues exporting symbols with MSVC DREAM3D does NOT currently support intermediate shared libraries. Please turn OFF BUILD_SHARED_LIBS")
    set(LIB_TYPE SHARED)
    list(APPEND CMP_LIB_SEARCH_DIRS  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
    if(MSVC)
        list(APPEND CMP_LIB_SEARCH_DIRS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug
                                        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release )
    endif()

else()
  message(FATAL_ERROR "BUILD_SHARED_LIBS Must be set to ON in order to build properly.")
endif(BUILD_SHARED_LIBS)


# ---------- Find and Use the Qt5 Libraries -----------------------
include(${CMP_SOURCE_DIR}/ExtLib/Qt5Support.cmake)


# ---------- Find HDF5 Headers/Libraries -----------------------
# HDF5 now comes with everything that is needed for CMake to load
# up the targets (Exported) that it needs. We just need to find where
# HDF5 is installed.
include(${CMP_SOURCE_DIR}/ExtLib/HDF5Support.cmake)

# ---------- Find Boost Headers/Libraries -----------------------
#set(Boost_FIND_REQUIRED FALSE)
set(Boost_DEBUG FALSE)
set(Boost_USE_MULTITHREADED TRUE)
set(Boost_USE_STATIC_LIBS TRUE)
set(Boost_ADDITIONAL_VERSIONS 1.57)
set(Boost_FIND_COMPONENTS "")
set(Boost_NO_BOOST_CMAKE 1)
FIND_PACKAGE(Boost)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

# --------------------------------------------------------------------
# Should we use Intel Threading Building Blocks
# --------------------------------------------------------------------
set(SIMPLib_USE_PARALLEL_ALGORITHMS "")
option(SIMPLib_USE_MULTITHREADED_ALGOS "Use MultiThreaded Algorithms" ON)
if(SIMPLib_USE_MULTITHREADED_ALGOS)
    find_package(TBB)
    if(TBB_FOUND)
        include_directories(${TBB_INCLUDE_DIRS} )
        CMP_COPY_DEPENDENT_LIBRARIES("tbb;tbb_malloc")
        # Append the locations of the TBB libraries to our Library Search Paths
        list(APPEND CMP_LIB_SEARCH_DIRS ${TBB_BINARY_DIR} ${TBB_LIBRARY_DIR} )
        # Create an install rule to copy the dependent shared library over to the installed location
        CMP_LIBRARIES_INSTALL_RULES("tbb;tbb_malloc" ".")
    else()
        message(FATAL_ERROR "The Intel Threading Building Blocks library is needed to enable the multithreaded algorithms. Please make sure it is installed properly")
    endif()
    set(SIMPLib_USE_PARALLEL_ALGORITHMS "1")
endif()

# --------------------------------------------------------------------
# DREAM3D needs the Eigen library for Least Squares fit and Eigen
# value/vector calculations.
set(SIMPLib_USE_EIGEN "")
Find_Package(Eigen REQUIRED)
if(EIGEN_FOUND)
  message(STATUS "Found Eigen Library")
  include_directories( ${EIGEN_INCLUDE_DIRS} )
  set(SIMPLib_USE_EIGEN "1")
else()
  message(WARNING "The Eigen Library is required for some algorithms to execute. These algorithms will be disabled.")
endif()

# --------------------------------------------------------------------
# Find the Doxygen package
FIND_PACKAGE(Doxygen)
if(DOXYGEN_FOUND)
  message(STATUS "Doxygen Found at ${DOXYGEN_EXECUTABLE}")
else()
  message(FATAL_ERROR "Doxygen is needed to build the documentation. Please install it correctly")
endif()


# ------------------------------------------------------------------------------
#  Qwt Section. Find Qwt and setup copy and install rules
# ------------------------------------------------------------------------------
include(${CMP_SOURCE_DIR}/ExtLib/QwtSupport.cmake)

# --------------------------------------------------------------------
# Now that we have found all of our required libraries and packages we can start the all
# the tests

# We do NOT want to install any of the headers
set(DREAM3DProj_INSTALL_FILES "${PROJECT_INSTALL_HEADERS}")

# --------------------------------------------------------------------
# Generate our ReadMe and License Files
configure_file(${PROJECT_RESOURCES_DIR}/DREAM3D/DREAM3DLicense.txt
                ${PROJECT_BINARY_DIR}/DREAM3D/DREAM3DLicense.txt )
configure_file(${PROJECT_SOURCE_DIR}/ReadMe.md
                ${PROJECT_BINARY_DIR}/ReadMe.md )

# --------------------------------------------------------------------
# Generate install rules for the text files
install(FILES ${PROJECT_BINARY_DIR}/ReadMe.md
        DESTINATION .
        COMPONENT Applications)

# Include some directories for the compiler
include_directories(${PROJECT_BINARY_DIR})

# --------------------------------------------------------------------
# Add in some compiler definitions
# --------------------------------------------------------------------
if( CMAKE_BUILD_TYPE MATCHES Debug )
  ADD_DEFINITIONS(-DDEBUG)
ENDif( CMAKE_BUILD_TYPE MATCHES Debug )

if(NOT MSVC)
 # ADD_DEFINITIONS(-Wall)
endif()

# On linux we need to set this because some of the libraries are Static
# and some are shared.
if( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" AND NOT MSVC )
  ADD_DEFINITIONS(-fPIC)
ENDif()

# --------------------------------------------------------------------
# If was are using GCC, make the compiler messages on a single line
if(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
ENDif(CMAKE_COMPILER_IS_GNUCC)
if(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
ENDif(CMAKE_COMPILER_IS_GNUCXX)

if(MSVC)
 add_definitions(-D_CRT_SECURE_NO_WARNINGS)
 add_definitions(-D_SCL_SECURE_NO_WARNINGS)
 option(SIMPLib_DISABLE_MSVC_WARNINGS "Disable some MSVC Compiler warnings" OFF)
endif()



include_directories(${DREAM3DProj_SOURCE_DIR}/Source)
include_directories(${PROJECT_BINARY_DIR})

# -----------------------------------------------------------------------
# Setup some Global Lists that are used to gather Documentation Information
file(WRITE ${DREAM3DProj_BINARY_DIR}/DREAM3DDocGroupList "")



# ------- Enable the CTest testing. Use make test to run all tests ---------------
OPTION(DREAM3D_BUILD_TESTING "Compile the test programs" ON)
if(DREAM3D_BUILD_TESTING)
    #ENABLE_TESTING()
    include( CTest )
endif()



# -----------------------------------------------------------------------
# Add in the Necessary HDF5 Support Sources into a library
# -----------------------------------------------------------------------
add_subdirectory( ${DREAM3DProj_SOURCE_DIR}/Source/H5Support ${PROJECT_BINARY_DIR}/H5Support)

# -----------------------------------------------------------------------
# Add in the Necessary EBSD Sources into a library
# -----------------------------------------------------------------------
add_subdirectory( ${DREAM3DProj_SOURCE_DIR}/Source/EbsdLib ${PROJECT_BINARY_DIR}/EbsdLib)

# -----------------------------------------------------------------------
# This needs to be set here as we are going to look for files in this directory
# -----------------------------------------------------------------------
set(FilterWidgetsLib_BINARY_DIR ${DREAM3DProj_BINARY_DIR}/FilterWidgetsLib)

# -----------------------------------------------------------------------
# -- Add in the DREAM3D Sources into a library
add_subdirectory( ${DREAM3DProj_SOURCE_DIR}/Source/SIMPLib/ ${PROJECT_BINARY_DIR}/SIMPLib)

# -----------------------------------------------------------------------
# -- Add in the DREAM3D Sources into a library
add_subdirectory( ${DREAM3DProj_SOURCE_DIR}/Source/OrientationLib/ ${PROJECT_BINARY_DIR}/OrientationLib)

# Build the static QtSupportLib library - which MUST be a static library or the linking
# will not work due to signals/slots mechanism.
add_subdirectory(${DREAM3DProj_SOURCE_DIR}/Source/QtSupportLib ${PROJECT_BINARY_DIR}/QtSupportLib)

# --------------------------------------------------------------------
# Build the DREAM3DWidgetsLib Library which holds all the DREAM3D Specific Widgets such
# as the Filter Parameter Widgets and various building block widgets for the DREAM3D app
# The plugins are also going to depend on these being built
add_subdirectory( ${DREAM3DProj_SOURCE_DIR}/Source/DREAM3DWidgetsLib ${PROJECT_BINARY_DIR}/DREAM3DWidgetsLib)

#-------------------------------------------------------------------------------
# Build the DREAM3D Application here so we can add dependencies between the plugin and
# the main DREAM3D application target. This should ensure that everything is built and
# up-to-date properly.
option(SIMPLib_BUILD_DREAM3D "Compile the DREAM3D GUI Application" ON)
if(SIMPLib_BUILD_DREAM3D)

  #- Add in the Main DREAM.3D Application
  add_subdirectory( ${DREAM3DProj_SOURCE_DIR}/Source/Applications/DREAM3D ${PROJECT_BINARY_DIR}/Applications/DREAM3D)

  set(SIMPLib_DOCS_ROOT_DIR "${DREAM3DProj_BINARY_DIR}/Bin/Help/DREAM3D")
endif()


#-------------------------------------------------------------------------------
# Compile the Core Plugins that come with DREAM3D and any other Plugins that the
# developer has added.
include(${PROJECT_CODE_DIR}/Plugins/Plugins.cmake)


# Build the StatsGenerator Application
OPTION(SIMPLib_BUILD_STATS_GENERATOR "Compile the Statistics Generator GUI Application" ON)
if(SIMPLib_BUILD_STATS_GENERATOR)
    add_subdirectory(${DREAM3DProj_SOURCE_DIR}/Source/Applications/StatsGenerator ${PROJECT_BINARY_DIR}/Applications/StatsGenerator)
endif()


#- Add in the DevHelper application
option(SIMPLib_BUILD_DEVHELPER "Compile the DevHelper Application" ON)
if(SIMPLib_BUILD_DEVHELPER)
    add_subdirectory( ${DREAM3DProj_SOURCE_DIR}/Source/Applications/DevHelper  ${PROJECT_BINARY_DIR}/Applications/DevHelper)
endif()


# add the Command line utilities
# Figure out here if we are going to build the command line tools
option(SIMPLib_BUILD_CLI_TOOLS "Build the command line versions of the applications" ON)
if( SIMPLib_BUILD_CLI_TOOLS )
    add_subdirectory(${DREAM3DProj_SOURCE_DIR}/Tools ${PROJECT_BINARY_DIR}/Tools)
    add_subdirectory(${DREAM3DProj_SOURCE_DIR}/Tools/astyle ${PROJECT_BINARY_DIR}/Tools/astyle)
endif()


#-------------------------------------------------------------------------------
# Should we build the documentation using DOxygen
option(SIMPLib_BUILD_DOCUMENTATION "Build the DREAM3D documentation" ON)
if(SIMPLib_BUILD_DOCUMENTATION)
  set(DOCUMENTATION_RESOURCES_DIR ${PROJECT_RESOURCES_DIR}/DocConfigFiles)

  set(DOX_GENERATE_RTF "NO")
  option(SIMPLib_GENERATE_RTF "Generate RTF Documentation" OFF)
  if(SIMPLib_GENERATE_RTF STREQUAL "ON")
    set(DOX_GENERATE_RTF "YES")
  endif()

  # By default we generate the Documentation in HTML format
  set(DOX_GENERATE_HTML "NO")
  option(SIMPLib_GENERATE_HTML "Generate HTML Documentation" ON)
  if(SIMPLib_GENERATE_HTML STREQUAL "ON")
    set(DOX_GENERATE_HTML "YES")
  endif()

  #most windows machines do NOT have LATEX built in so default this to OFF
  set(DOX_GENERATE_LATEX "NO")
  option(SIMPLib_GENERATE_LATEX "Generate PDF Documentation" OFF)
  if(SIMPLib_GENERATE_LATEX STREQUAL "ON")
    set(DOX_GENERATE_LATEX "YES")
    # Check for existence of LaTex executable files
    INCLUDE( ${DOCUMENTATION_RESOURCES_DIR}/CMake/UseLATEX.cmake)
    if(APPLE)
      option(AUTO_OPEN_PDF "Automatically open generated PDF files with default PDF reading application" ON)
    endif()
  endif()


  # Now add each subdirectory that holds our documentation files
  add_subdirectory(${PROJECT_SOURCE_DIR}/Documentation/ReferenceManual ${PROJECT_BINARY_DIR}/Documentation/ReferenceManual)
  add_subdirectory(${PROJECT_SOURCE_DIR}/Documentation/APIDocs ${PROJECT_BINARY_DIR}/Documentation/APIDocs)
endif()


# This should be the last line in this file:
include(${PROJECT_RESOURCES_DIR}/CPack/PackageProject.cmake)

# ------- Enable the CTest testing. Use make test to run all tests ---------------
if(DREAM3D_BUILD_TESTING)
    include(${DREAM3DProj_SOURCE_DIR}/Test/CMakeLists.txt)
endif()

