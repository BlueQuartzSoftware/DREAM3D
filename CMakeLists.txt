#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#--  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////


if(NOT ${DREAM3D_SDK} STREQUAL "")
  include("${DREAM3D_SDK}/DREAM3D_SDK.cmake")
endif()

project(DREAM3DProj)
cmake_minimum_required(VERSION 2.8.9)

if(${CMAKE_MAJOR_VERSION} STREQUAL "3")
  # CMake 3.0 introduces VERSION variables for each Project. We do our own version numbering so we need to have
  # cmake ignore that policy
  cmake_policy(SET CMP0048 OLD)
  # CMake 3.0 introduces some OS X RPath policies
  cmake_policy(SET CMP0042 OLD)
endif()


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
INCLUDE (${CMP_SOURCE_DIR}/cmpCMakeMacros.cmake )
set(CMP_ENABLE_PLUGINS "1")
set(CMP_PLUGIN_LIST_FILE ${PROJECT_BINARY_DIR}/DREAM3D_PluginList.txt)
file(WRITE ${CMP_PLUGIN_LIST_FILE} "")
set(CMP_PLUGIN_SEARCHDIR_FILE ${PROJECT_BINARY_DIR}/libsearchdirs.txt)
file(WRITE ${CMP_PLUGIN_SEARCHDIR_FILE} "${PROJECT_BINARY_DIR}/Bin/plugins;")
file(APPEND ${CMP_PLUGIN_SEARCHDIR_FILE} "${PROJECT_BINARY_DIR}/Bin;")
set(CMP_LIB_SEARCH_DIRS "")
set(CMP_MODULES_SOURCE_DIR ${CMP_SOURCE_DIR}/Modules CACHE INTERNAL "")

if(UNIX AND NOT APPLE)
    set(DREAM3D_LINUX_SYSTEM ${CMAKE_SYSTEM_NAME} CACHE STRING "The Name of the linux distrubution for packaging")
endif()

# --------------------------------------------------------------------
#
if("${DREAM3D_DATA_DIR}" STREQUAL "")
  set(DREAM3D_DATA_DIR "${DREAM3DProj_SOURCE_DIR}/../DREAM3D_Data/"  CACHE PATH "The directory where to find test data files")
endif()

if("${DREAM3D_DATA_DIR}" STREQUAL "")
   message(FATAL_ERROR "The DREAM3D_DATA_DIR variable is not set and it needs to be set to a location where the test data is located")
endif()

# --------------------------------------------------------------------
# This is set to "1" so that our test files are deleted at the end of the tests
set(DREAM3D_REMOVE_TEST_FILES "1")

# --------------------------------------------------------------------
# In order to increment the version of DREAM3D simply adjust these variables.
# we are using a 3 part Version scheme commonly found in software development. The
# example would be Version 2.1.6543 which translates to Version [major].[minor].[patch].
# The Major version is set here in the CMake file. We will use Git
# to figure out the "patch" leve of the version by using "git describe" which will
# calculate how many commits since the last "tag" was applied to the repository.
# this basically gives us an always incrementing number for the patch level
# and we can set the Major and Minor versions as we see fit.
set(CMP_VERSION_MAJOR "5.1")

# --------------------------------------------------------------------
# Over ride CMake's built in module directory by prepending cmp's module
# directory first
set(CMAKE_MODULE_PATH ${CMP_MODULES_SOURCE_DIR} ${CMAKE_MODULE_PATH})

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
endif(BUILD_SHARED_LIBS)

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
   CMP_COPY_QT4_RUNTIME_LIBRARIES( "QtCore;QtGui;QtNetwork")
   CMP_QT_LIBRARIES_INSTALL_RULES("QtCore;QtGui;QtNetwork" ".")
else()
  message(FATAL_ERROR "Qt version 4.8 is REQUIRED to build DREAM3D")
endif()

set(QT_USE_QTCORE "TRUE")
set(QT_USE_QTGUI "TRUE")
set(QT_USE_QTNETWORK "TRUE")
set(CMAKE_AUTOMOC TRUE)

# add some useful macros and variables
# (QT_USE_FILE is a variable defined by FIND_PACKAGE( Qt4 ) that
# contains a path to CMake script)
INCLUDE( ${QT_USE_FILE} )


# --------------------------------------------------------------------
# We need the Qt Image Plugins (Most likley) for either the DREAM3D application
# or the pipeline runner which might load images through one of the plugins
# so we are just going to look for the plugins here and include them.
FindQt4Plugins("qgif;qjpeg;qtiff" "${DREAM3DProj_BINARY_DIR}/Qt_Plugins.txt"
                "${DREAM3DProj_BINARY_DIR}/LibSearchDirs.txt"
                "imageformats" )

# Append the locations of the Qt libraries to our Library Search Paths
list(APPEND CMP_LIB_SEARCH_DIRS ${QT_BINARY_DIR} ${QT_LIBRARY_DIR} )

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

# ---------- Find HDF5 Headers/Libraries -----------------------
# HDF5 now comes with everything that is needed for CMake to load
# up the targets (Exported) that it needs. We just need to find where
# HDF5 is installed.
message(STATUS "HDF5_INSTALL: ${HDF5_INSTALL}")
if("${HDF5_INSTALL}" STREQUAL "")
    set(HDF5_INSTALL  $ENV{HDF5_INSTALL})
endif()

if(WIN32)
    set(HDF5_ROOT_DIR_HINT "${HDF5_INSTALL}/cmake/hdf5")
    set(CMAKE_MODULE_PATH ${HDF5_INSTALL}/cmake/hdf5 ${CMAKE_MODULE_PATH})
else()
    set(HDF5_ROOT_DIR_HINT "${HDF5_INSTALL}/share/cmake")
    set(CMAKE_MODULE_PATH ${HDF5_INSTALL}/share/cmake ${CMAKE_MODULE_PATH})
endif()
message (STATUS "HDF5_ROOT_DIR_HINT: ${HDF5_ROOT_DIR_HINT}")

find_package(HDF5)
if(HDF5_FOUND)
  file(APPEND ${CMP_PLUGIN_SEARCHDIR_FILE} "${HDF5_LIBRARY_DIRS};")
  include_directories(${HDF5_INCLUDE_DIRS} )
  list(APPEND CMP_LIB_SEARCH_DIRS ${HDF5_BINARY_DIR} ${HDF5_LIBRARY_DIR} )
  
  if(MSVC_IDE)
    set(BUILD_TYPES Debug Release)
  else()
    set(BUILD_TYPES "${CMAKE_BUILD_TYPE}")
    if("${BUILD_TYPES}" STREQUAL "")
        set(BUILD_TYPES "Debug")
    endif()
  endif()
  AddHDF5CopyInstallRules(LIBVAR HDF5_LIB
                      LIBNAME hdf5
                      TYPES ${BUILD_TYPES})
  AddHDF5CopyInstallRules(LIBVAR HDF5_CPP_LIB
                      LIBNAME hdf5_cpp
                      TYPES ${BUILD_TYPES})

ELSE(HDF5_FOUND)
    MESSAGE(FATAL_ERROR "Cannot build without HDF5.  Please set HDF5_INSTALL environment variable to point to your HDF5 installation.")
ENDif(HDF5_FOUND)

# --------------------------------------------------------------------
# Should we use Intel Threading Building Blocks
# --------------------------------------------------------------------
set(DREAM3D_USE_PARALLEL_ALGORITHMS "")
option(DREAM3D_USE_MULTITHREADED_ALGOS "Use MultiThreaded Algorithms where possible" ON)
if(DREAM3D_USE_MULTITHREADED_ALGOS)
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
    set(DREAM3D_USE_PARALLEL_ALGORITHMS "1")
endif()

# --------------------------------------------------------------------
# DREAM3D needs the Eigen library for Least Squares fit and Eigen
# value/vector calculations.
Find_Package(Eigen REQUIRED)
if(EIGEN_FOUND)
  message(STATUS "Found Eigen Library")
  include_directories( ${EIGEN_INCLUDE_DIRS} )
else()
  message(WARNING "The Eigen Library is required for parallel algorithms. The Eigen Library could not be located so these algorithms will be compiled in serial mode.")
endif()

# --------------------------------------------------------------------
# Find the Doxygen package
FIND_PACKAGE(Doxygen)
if(DOXYGEN_FOUND)
  message(STATUS "Doxygen Found at ${DOXYGEN_EXECUTABLE}")
else()
  message(FATAL_ERROR "Doxygen is needed to build the documentation. Please install it correctly")
endif()


# --------------------------------------------------------------------
# Now that we have found all of our required libraries and packages we can start the all
# the tests

# We do NOT want to install any of the headers
set(DREAM3DProj_INSTALL_FILES "${PROJECT_INSTALL_HEADERS}")

# --------------------------------------------------------------------
# Generate our ReadMe and License Files
configure_file(${PROJECT_SOURCE_DIR}/License.txt
                ${PROJECT_BINARY_DIR}/License.txt )
configure_file(${PROJECT_SOURCE_DIR}/ReadMe.md
                ${PROJECT_BINARY_DIR}/ReadMe.md )
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
  ADD_DEFINITIONS(-Wall)
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
 option(DREAM3D_DISABLE_MSVC_WARNINGS "Disable some MSVC Compiler warnings" OFF)
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
#set(FilterWidgetsLib_SOURCE_DIR ${DREAM3DProj_SOURCE_DIR}/Source/FilterWidgetsLib)
set(FilterWidgetsLib_BINARY_DIR ${DREAM3DProj_BINARY_DIR}/FilterWidgetsLib)

# -----------------------------------------------------------------------
# -- Add in the DREAM3D Sources into a static library
add_subdirectory( ${DREAM3DProj_SOURCE_DIR}/Source/DREAM3DLib/ ${PROJECT_BINARY_DIR}/DREAM3DLib)

# Build the static QtSupport library - which MUST be a static library or the linking
# will not work due to signals/slots mechanism.
add_subdirectory(${DREAM3DProj_SOURCE_DIR}/Source/QtSupport ${PROJECT_BINARY_DIR}/QtSupport)

# --------------------------------------------------------------------
# Build the DREAM3DWidgetsLib Library which holds all the DREAM3D Specific Widgets such
# as the Filter Parameter Widgets and various building block widgets for the DREAM3D app
# The plugins are also going to depend on these being built
add_subdirectory( ${DREAM3DProj_SOURCE_DIR}/Source/DREAM3DWidgetsLib ${PROJECT_BINARY_DIR}/DREAM3DWidgetsLib)

#-------------------------------------------------------------------------------
# Build the DREAM3D Application here so we can add dependencies between the plugin and
# the main DREAM3D application target. This should ensure that everything is built and
# up-to-date properly.
option(DREAM3D_BUILD_DREAM3D "Compile the DREAM3D GUI Application" ON)
if(DREAM3D_BUILD_DREAM3D)

  #- Add in the Main DREAM.3D Application
  add_subdirectory( ${DREAM3DProj_SOURCE_DIR}/Source/Applications/DREAM3D ${PROJECT_BINARY_DIR}/Applications/DREAM3D)

  set(DREAM3D_DOCS_ROOT_DIR "${DREAM3DProj_BINARY_DIR}/Bin/Help/DREAM3D")
endif()


# --------------------------------------------------------------------
# This function optionally compiles a named plugin when compiling DREAM3D
# This function will add in an Option "DREAM3D_BUILD_PLUGIN_${NAME} which
# the programmer can use to enable/disable the compiling of specific plugins
# Arguments:
# PLUGIN_NAME The name of the Plugin
# PLUGIN_SOURCE_DIR The source directory for the plugin
function(DREAM3D_COMPILE_PLUGIN)
    set(options)
    set(oneValueArgs PLUGIN_NAME PLUGIN_SOURCE_DIR)
    cmake_parse_arguments(PLUG  "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    option(DREAM3D_BUILD_PLUGIN_${PLUG_PLUGIN_NAME} "Build the ${PLUG_PLUGIN_NAME}" ON)
    if(DREAM3D_BUILD_PLUGIN_${PLUG_PLUGIN_NAME})
        # message(STATUS "${PLUG_PLUGIN_NAME}: Configuring Plugin ....")
        add_subdirectory(${PLUG_PLUGIN_SOURCE_DIR} ${PROJECT_BINARY_DIR}/Plugins/${PLUG_PLUGIN_NAME})
        #- Now set up the dependency between the main application and each of the plugins so that
        #- things like Visual Studio are forced to rebuild the plugins when launching
        #- the DREAM3D application
    if(DREAM3D_BUILD_PLUGIN_${p})
          add_dependencies(DREAM3D ${p})
      endif()
    else()
        message(STATUS "Plugin: ${PLUG_PLUGIN_NAME} Disabled")
    endif()
endfunction()

#-----------------
# These are the core plugins that need to be built
set(DREAM3D_PLUGIN_LIST
  Generic
  IO
  DDDAnalysisToolbox
  ImageImport
  OrientationAnalysis
  Processing
  Reconstruction
  Sampling
  Statistics
  SurfaceMeshing
  SyntheticBuilding
)

#-------------------------------------------------------------------------------
# Build the "Built in" Plugins
#message(STATUS "*** Configuring Standard Plugins ***")
#foreach(p ${DREAM3D_PLUGIN_LIST})
#    DREAM3D_COMPILE_PLUGIN(PLUGIN_NAME ${p}
#                           PLUGIN_SOURCE_DIR ${DREAM3DProj_SOURCE_DIR}/Source/Plugins/${p})
#endforeach()


# --- Look for user defined external Plugins that the user has entered
# into CMake-GUI or through CCMake by the user.
if("${DREAM3D_EXTRA_PLUGINS}" STREQUAL "")
  set(DREAM3D_EXTRA_PLUGINS "" CACHE LIST "Semicolon delimited list of Extra External Plugins that you want to build into DREAM.3D")
endif()

set(DREAM3D_PLUGIN_LIST ${DREAM3D_PLUGIN_LIST} ${DREAM3D_EXTRA_PLUGINS} )

#-- Attempt to look in our local source directory for the plugin. Anywhere else
# and the user will have to put the entire path into CMake manually.
foreach(userPlugin ${DREAM3D_PLUGIN_LIST})

  message(STATUS "Adding Plugin: ${userPlugin}...")

  if(DEFINED ${userPlugin}_SOURCE_DIR AND "${${userPlugin}_SOURCE_DIR}" STREQUAL "")
    set(pluginSearchDir ${PROJECT_CODE_DIR}/Plugins/${userPlugin})
    set(${userPlugin}_SOURCE_DIR ${pluginSearchDir} CACHE PATH "")
  endif()


  if(NOT DEFINED ${userPlugin}_SOURCE_DIR)
      set(pluginSearchDir ${PROJECT_CODE_DIR}/Plugins/${userPlugin})
      if(EXISTS ${pluginSearchDir})
        set(${userPlugin}_SOURCE_DIR ${pluginSearchDir} CACHE PATH "")
        message(STATUS "Plugin: Defining ${userPlugin}_SOURCE_DIR to ${${userPlugin}_SOURCE_DIR}")
      else()
        set(pluginSearchDir ${DREAM3DProj_SOURCE_DIR}/../${userPlugin})
        if(EXISTS ${pluginSearchDir})
            set(${userPlugin}_SOURCE_DIR ${pluginSearchDir} CACHE PATH "")
            message(STATUS "Plugin: Defining ${userPlugin}_SOURCE_DIR to ${${userPlugin}_SOURCE_DIR}")
        endif()
      endif()
  endif()


# Now that we have defined where the user's plugin directory is at we
# need to make sure it have a CMakeLists.txt file in it
  if(EXISTS ${${userPlugin}_SOURCE_DIR}/CMakeLists.txt)
      set(${userPlugin}_IMPORT_FILE ${userPlugin}_SOURCE_DIR/CMakeLists.txt)
  endif()

# By this point we should have everything defined and ready to go...
  if(DEFINED ${userPlugin}_SOURCE_DIR AND DEFINED ${userPlugin}_IMPORT_FILE)
      #message(STATUS "Plugin: Adding Plugin ${${userPlugin}_SOURCE_DIR}")
      DREAM3D_COMPILE_PLUGIN(PLUGIN_NAME ${userPlugin}
                             PLUGIN_SOURCE_DIR ${${userPlugin}_SOURCE_DIR})
  else()
      set(${userPlugin}_SOURCE_DIR ${pluginSearchDir} CACHE PATH "" FORCE)
      message(FATAL_ERROR "Plugin: The directory for plugin ${userPlugin} was not found. Please set this directory manually")
  endif()

endforeach(userPlugin ${DREAM3D_EXTRA_PLUGINS})


# Build the StatsGenerator Application
OPTION(DREAM3D_BUILD_STATS_GENERATOR "Compile the Statistics Generator GUI Application" ON)
if(DREAM3D_BUILD_STATS_GENERATOR)
    add_subdirectory(${DREAM3DProj_SOURCE_DIR}/Source/Applications/StatsGenerator ${PROJECT_BINARY_DIR}/Applications/StatsGenerator)
endif()


#- Add in the PluginMaker application
option(DREAM3D_BUILD_PLUGINMAKER "Compile the PluginMaker Application" ON)
if(DREAM3D_BUILD_PLUGINMAKER)
    add_subdirectory( ${DREAM3DProj_SOURCE_DIR}/Source/Applications/PluginMaker  ${PROJECT_BINARY_DIR}/Applications/PluginMaker)
endif()


# add the Command line utilities
# Figure out here if we are going to build the command line tools
option(DREAM3D_BUILD_CLI_TOOLS "Build the command line versions of the applications" ON)
if( DREAM3D_BUILD_CLI_TOOLS )
    add_subdirectory(${DREAM3DProj_SOURCE_DIR}/Tools ${PROJECT_BINARY_DIR}/Tools)
    add_subdirectory(${DREAM3DProj_SOURCE_DIR}/Tools/astyle ${PROJECT_BINARY_DIR}/Tools/astyle)
endif()


#-------------------------------------------------------------------------------
# Should we build the documentation using DOxygen
option(DREAM3D_BUILD_DOCUMENTATION "Build the DREAM3D documentation" ON)
if(DREAM3D_BUILD_DOCUMENTATION)
  set(DOCUMENTATION_RESOURCES_DIR ${PROJECT_RESOURCES_DIR}/DocConfigFiles)

  set(DOX_GENERATE_RTF "NO")
  option(DREAM3D_GENERATE_RTF "Generate RTF Documentation" OFF)
  if(DREAM3D_GENERATE_RTF STREQUAL "ON")
    set(DOX_GENERATE_RTF "YES")
  endif()

  # By default we generate the Documentation in HTML format
  set(DOX_GENERATE_HTML "NO")
  option(DREAM3D_GENERATE_HTML "Generate HTML Documentation" ON)
  if(DREAM3D_GENERATE_HTML STREQUAL "ON")
    set(DOX_GENERATE_HTML "YES")
  endif()

  #most windows machines do NOT have LATEX built in so default this to OFF
  set(DOX_GENERATE_LATEX "NO")
  option(DREAM3D_GENERATE_LATEX "Generate PDF Documentation" OFF)
  if(DREAM3D_GENERATE_LATEX STREQUAL "ON")
    set(DOX_GENERATE_LATEX "YES")
    # Check for existence of LaTex executable files
    INCLUDE( ${DOCUMENTATION_RESOURCES_DIR}/CMake/UseLATEX.cmake)
    if(APPLE)
      option(AUTO_OPEN_PDF "Automatically open generated PDF files with default PDF reading application" ON)
    endif()
  endif()


  # Now add each subdirectory that holds our documentation files
  add_subdirectory(${PROJECT_SOURCE_DIR}/Documentation/Developer ${PROJECT_BINARY_DIR}/Documentation/Developer)
  add_subdirectory(${PROJECT_SOURCE_DIR}/Documentation/ReferenceManual ${PROJECT_BINARY_DIR}/Documentation/ReferenceManual)
  add_subdirectory(${PROJECT_SOURCE_DIR}/Documentation/APIDocs ${PROJECT_BINARY_DIR}/Documentation/APIDocs)
endif()


# This should be the last line in this file:
include(${PROJECT_RESOURCES_DIR}/CPack/PackageProject.cmake)

# ------- Enable the CTest testing. Use make test to run all tests ---------------
if(DREAM3D_BUILD_TESTING)
    add_subdirectory(${DREAM3DProj_SOURCE_DIR}/Test ${PROJECT_BINARY_DIR}/Test)
endif()

