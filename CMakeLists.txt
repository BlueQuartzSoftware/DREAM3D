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
project (DREAM3D)
cmake_minimum_required(VERSION 2.8.3)

# ---------- Setup output Directories -------------------------
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/Bin
  CACHE PATH
  "Single Directory for all Libraries"
  )

# --------- Setup the Executable output Directory -------------
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/Bin
  CACHE PATH
  "Single Directory for all Executables."
  )

# --------- Setup the Executable output Directory -------------
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/Bin
  CACHE PATH
  "Single Directory for all static libraries."
  )

Find_package(Git)

if (GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} describe
        OUTPUT_VARIABLE DVERS
        RESULT_VARIABLE did_run
     )
    string(STRIP ${DVERS} DVERS)
  #  message(STATUS "DVERS: ${DVERS}")
    string(REPLACE  "-" ";" VERSION_LIST ${DVERS})
  #  message(STATUS "VERSION_LIST: ${VERSION_LIST}")
    list(GET VERSION_LIST 0 DREAM3D_VER_MAJOR)
    list(GET VERSION_LIST 1 DREAM3D_VER_MINOR)
    list(GET VERSION_LIST 2 DREAM3D_VER_PATCH)

    set(DREAM3D_VER_MAJOR "${DREAM3D_VER_MAJOR}" CACHE STRING "Major Version Number" FORCE)
    set(DREAM3D_VER_MINOR "${DREAM3D_VER_MINOR}" CACHE STRING "Minor Version Number" FORCE)
    set(DREAM3D_VER_PATCH "${DREAM3D_VER_PATCH}" CACHE STRING "Patch Version Number" FORCE)
else()
    # if we fail to find Git then use some sort of Date Scheme
    set(DREAM3D_VER_MAJOR "2011" CACHE STRING "Major Version Number" FORCE)
    set(DREAM3D_VER_MINOR "07" CACHE STRING "Minor Version Number" FORCE)
    set(DREAM3D_VER_PATCH "06" CACHE STRING "Patch Version Number" FORCE)

endif()

set(DREAM3D_VERSION "${DREAM3D_VER_MAJOR}.${DREAM3D_VER_MINOR}.${DREAM3D_VER_PATCH}" CACHE STRING "Full Version Number" FORCE)

set (CMP_SOURCE_DIR ${DREAM3D_SOURCE_DIR}/Support/cmp)

SET (PROJECT_CODE_DIR ${PROJECT_SOURCE_DIR}/Source)
SET (PROJECT_RESOURCES_DIR ${DREAM3D_SOURCE_DIR}/Resources)
SET (PROJECT_PREFIX "MXA" CACHE STRING "The Prefix to be used for Preprocessor definitions")
SET (EXE_DEBUG_EXTENSION "_debug")
SET (PROJECT_INSTALL_HEADERS "0")
SET (PROJECT_INSTALL_EXECUTABLES "1")

#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the 
# project.
#------------------------------------------------------------------------------- 
set (CMP_HEADER_DIR ${DREAM3D_BINARY_DIR}/MXA)
set (CMP_CONFIGURATION_FILE_NAME "CMPConfiguration.h")
set (CMP_TYPES_FILE_NAME "MXATypes.h")
set (CMP_VERSION_HEADER_FILE_NAME "MXAVersion.h")
set (CMP_EXTRA_CONFIGURATION_FILE "MXAConfiguration.h")
set (CMP_PROJECT_NAMESPACE "MXA")
set (CMP_PROJECT_NAME "MXA")
set (CMP_ENABLE_PLUGINS "1")
set (CMP_PLUGIN_LIST_FILE ${PROJECT_BINARY_DIR}/DREAM3D_PluginList.txt)
set (CMP_LIB_SEARCH_DIRS "")
set(CMP_GENERATE_VERSION_STRING 0)
INCLUDE (${CMP_SOURCE_DIR}/cmpProject.cmake)

# We do NOT want to install any of the headers
SET (DREAM3D_INSTALL_FILES "${PROJECT_INSTALL_HEADERS}")

# --------------------------------------------------------------------
# Generate a Header file with Compile Version variables
# --------------------------------------------------------------------
set (VERSION_GEN_NAME "DREAM3D")
set (VERSION_GEN_NAMESPACE "DREAM3D")
set (PROJECT_PREFIX "DREAM3D")
set (VERSION_GEN_COMPLETE  ${DREAM3D_VERSION})
set (VERSION_GEN_VER_MAJOR ${DREAM3D_VER_MAJOR})
set (VERSION_GEN_VER_MINOR ${DREAM3D_VER_MINOR})
set (VERSION_GEN_VER_PATCH ${DREAM3D_VER_PATCH})
configure_file(${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpVersion.h.in   
               ${DREAM3D_BINARY_DIR}/DREAM3D/DREAM3DVersion.h  )

configure_file(${DREAM3D_SOURCE_DIR}/Source/DREAM3D/DREAM3DConfiguration.h.in
               ${DREAM3D_BINARY_DIR}/DREAM3D/DREAM3DConfiguration.h)
#-- Create a JavaScript file that can be uploaded to the web server that will automatically
# update the download links to the latest version
configure_file(${PROJECT_RESOURCES_DIR}/downloadcreator.js.in 
                ${DREAM3D_BINARY_DIR}/downloadcreator.js)                          
# --------------------------------------------------------------------
# Generate our ReadMe and License Files
configure_file(${PROJECT_SOURCE_DIR}/License.txt.in
                ${PROJECT_BINARY_DIR}/License.txt )
configure_file(${PROJECT_SOURCE_DIR}/ReadMe.txt.in
                ${PROJECT_BINARY_DIR}/ReadMe.txt )

SET (MXA_INSTALL_FILES "0")
SET (MXA_SOURCE_DIR ${PROJECT_CODE_DIR})

# Include some directories for the compiler
include_directories(${PROJECT_BINARY_DIR})

# Build shared libraries
OPTION (BUILD_SHARED_LIBS "Build Shared Libraries" OFF)
SET (LIB_TYPE STATIC)
IF (BUILD_SHARED_LIBS)
    SET (LIB_TYPE SHARED)
    list(APPEND CMP_LIB_SEARCH_DIRS  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
    if (MSVC)
        list(APPEND CMP_LIB_SEARCH_DIRS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug
                                    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release )
    endif()
ENDIF (BUILD_SHARED_LIBS)

# ---------- Find Boost Headers/Libraries -----------------------
SET (Boost_FIND_REQUIRED TRUE)
SET (Boost_DEBUG FALSE)
set (Boost_USE_MULTITHREADED TRUE)
set (Boost_USE_STATIC_LIBS TRUE)
SET (Boost_ADDITIONAL_VERSIONS "1.44.0" "1.44" "1.36" "1.36.0" "1.41" "1.41.0" "1.39" "1.39.0")
SET (MXA_BOOST_COMPONENTS "")
FIND_PACKAGE(Boost COMPONENTS  ${MXA_BOOST_COMPONENTS} )
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

# -- This needs to be here so that the configured file gets written correctly
find_package(HDF5)
IF(HDF5_FOUND)
    file(APPEND ${CMP_PLUGIN_SEARCHDIR_FILE} "${HDF5_LIBRARY_DIRS};")
    include_directories(${HDF5_INCLUDE_DIRS} )
    # This copies the DLL (if needed) into the various build directories on MSVC systems
    CMP_COPY_DEPENDENT_LIBRARIES("hdf5")
    # Append the locations of the HDF5 libraries to our Library Search Paths
    list(APPEND CMP_LIB_SEARCH_DIRS ${HDF5_BINARY_DIR} ${HDF5_LIBRARY_DIR} )
    # Create an install rule to copy the dependent shared library over to the installed location
    CMP_LIBRARIES_INSTALL_RULES("hdf5" ".")
ELSE(HDF5_FOUND)
    MESSAGE(FATAL_ERROR "Cannot build without HDF5.  Please set HDF5_INSTALL environment variable to point to your HDF5 installation.")
ENDIF(HDF5_FOUND)


# --------------------------------------------------------------------
# Add in some compiler definitions
# --------------------------------------------------------------------
IF ( CMAKE_BUILD_TYPE MATCHES Debug )
  ADD_DEFINITIONS(-DDEBUG)
  ADD_DEFINITIONS(-Wall)
ENDIF ( CMAKE_BUILD_TYPE MATCHES Debug )

# --------------------------------------------------------------------
# Should we use Intel Threading Building Blocks
# --------------------------------------------------------------------
set (DREAM3D_USE_PARALLEL_ALGORITHMS "")
option(DREAM3D_USE_MULTITHREADED_ALGOS "Use MultiThreaded Algorithms where possible" OFF)
if (DREAM3D_USE_MULTITHREADED_ALGOS)
    find_package(TBB)
    if (TBB_FOUND)
        include_directories(${TBB_INCLUDE_DIRS} )
        CMP_COPY_DEPENDENT_LIBRARIES("tbb;tbb_malloc")
        # Append the locations of the TBB libraries to our Library Search Paths
        list(APPEND CMP_LIB_SEARCH_DIRS ${TBB_BINARY_DIR} ${TBBG_LIBRARY_DIR} )
        # Create an install rule to copy the dependent shared library over to the installed location
        CMP_LIBRARIES_INSTALL_RULES("tbb;tbb_malloc" ".")
    else()
        message(FATAL_ERROR "The Intel Threading Building Blocks library is needed to enable the multithreaded algorithms. Please make sure it is installed properly")
    endif()
    set (DREAM3D_USE_PARALLEL_ALGORITHMS "1")
endif()


# --------------------------------------------------------------------
# If was are using GCC, make the compiler messages on a single line
IF(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

if (MSVC)
 add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Figure out here if we are going to build the command line tools
option(DREAM3D_BUILD_CLI_TOOLS "Build the command line versions of the applications" OFF)

# -----------------------------------------------------------------------
# -- Add in the Necessary HDF5 Support Sources into a library
add_subdirectory( ${DREAM3D_SOURCE_DIR}/Source/H5Support ${DREAM3D_BINARY_DIR}/H5Support)
include_directories (${DREAM3D_BINARY_DIR})

# -----------------------------------------------------------------------
# -- Add in the Necessary MXA Sources into a library
add_subdirectory( ${DREAM3D_SOURCE_DIR}/Source/MXA ${DREAM3D_BINARY_DIR}/MXA)
include_directories (${DREAM3D_BINARY_DIR})

# -----------------------------------------------------------------------
# -- Add in the Necessary EBSD Sources into a library
add_subdirectory( ${DREAM3D_SOURCE_DIR}/Source/EbsdLib ${DREAM3D_BINARY_DIR}/EbsdLib)
include_directories (${DREAM3D_BINARY_DIR})

# -----------------------------------------------------------------------
# -- Add in the AIM Common Sources into a static library
include_directories(${DREAM3D_SOURCE_DIR}/Source)
add_subdirectory( ${DREAM3D_SOURCE_DIR}/Source/DREAM3D/ ${DREAM3D_BINARY_DIR}/DREAM3D)

option(DREAM3D_BUILD_QT_APPS "Compile the Qt based GUI applications" ON)
#if we are going to build Qt based applications then we need to find Qt 4 first
if (DREAM3D_BUILD_QT_APPS)
   # ------------------------------------------------------------------------------
   #  Qt 4 Section
   # ------------------------------------------------------------------------------
   # by default only QtCore and QtGui modules are enabled
   # other modules must be enabled like this:  
   IF (WIN32)
       SET (QT_USE_QTMAIN TRUE)
   ENDIF (WIN32)

   # this command finds Qt4 libraries and sets all required variables
   # note that it's Qt4, not QT4 or qt4
   FIND_PACKAGE( Qt4 REQUIRED )
   IF (QT4_FOUND)
       CMP_COPY_QT4_RUNTIME_LIBRARIES( "QtCore;QtGui")
       CMP_QT_LIBRARIES_INSTALL_RULES("QtCore;QtGui" ".")
   endif()
   # add some useful macros and variables
   # (QT_USE_FILE is a variable defined by FIND_PACKAGE( Qt4 ) that 
   # contains a path to CMake script)
   INCLUDE( ${QT_USE_FILE} )
   
   # Append the locations of the Qt libraries to our Library Search Paths
   list(APPEND CMP_LIB_SEARCH_DIRS ${QT_BINARY_DIR} ${QT_LIBRARY_DIR} )
   
   # Build the static QtSupport library - which MUST be a static library or the linking
   # will not work due to signals/slots mechanism.
   add_subdirectory(${DREAM3D_SOURCE_DIR}/Source/QtSupport ${DREAM3D_BINARY_DIR}/QtSupport)
  
   macro(DREAM3D_COMPILE_PLUGIN plugin)
       add_subdirectory(${DREAM3D_SOURCE_DIR}/Source/Plugins/${plugin} ${DREAM3D_BINARY_DIR}/Plugins/${plugin})  
   endmacro()
  
     # Build the DREAM3D Application  
   option(DREAM3D_BUILD_GUI "Compile the DREAM3D GUI Application" ON)
   if (DREAM3D_BUILD_GUI)
   
        #-----------------
        # Add any additional Plugins that you develop here to this list
        set (DREAM3D_PLUGIN_LIST
            OIMImport 
            Reconstruction 
            GrainGenerator 
            SurfaceMesh 
            MicrostructureStatistics 
        )
        
        foreach(p ${DREAM3D_PLUGIN_LIST})
            DREAM3D_COMPILE_PLUGIN(${p})
        endforeach()
  
        #- Add in the Main DREAM.3D Application 
        ADD_SUBDIRECTORY( ${DREAM3D_SOURCE_DIR}/Source/Applications/DREAM3D ${DREAM3D_BINARY_DIR}/Applications/DREAM3D)

 
        #- Now set up the dependency between the main application and each of the plugins so that
        #- things like Visual Studion are forced to rebuild the plugins when launching
        #- the DREAM3D application 
        foreach(p ${DREAM3D_PLUGIN_LIST})
            add_dependencies(${p}Plugin DREAM3D)
        endforeach()
        
        
        # --- Look for user defined external Plugins that the user has entered into CMake-GUI or through CCMake
        set(DREAM3D_EXTRA_PLUGINS "" CACHE LIST "Semicolon delimited list of Extra External Plugins that you want to build into DREAM.3D")
        
        #-- Attempt to look in our local source directory for the plugin. Anywhere else
        # and the user will have to put the entire path into CMake manually.
        foreach(userPlugin ${DREAM3D_EXTRA_PLUGINS})
            message(STATUS "Looking for Plugin: ${userPlugin}")
            set(${userPlugin}_SOURCE_DIR "" CACHE PATH "Path to ${userPlugin} directory")
        
            if(EXISTS ${DREAM3D_SOURCE_DIR}/Plugins/${userPlugin})
                set(${userPlugin}_SOURCE_DIR ${DREAM3D_SOURCE_DIR}/Plugins/${userPlugin})
            endif()
            if(EXISTS ${DREAM3D_SOURCE_DIR}/Plugins/${userPlugin}/CMakeLists.txt)
                set(${userPlugin}_IMPORT_FILE ${DREAM3D_SOURCE_DIR}/Plugins/${userPlugin}/CMakeLists.txt)
            endif()
            if(NOT DEFINED ${userPlugin}_SOURCE_DIR AND NOT DEFINED ${userPlugin}_IMPORT_FILE)
                message(WARNING "${userPlugin}_SOURCE_DIR was NOT defined and it needs to be in order to compile the ${userPlugin} Plugin")
            endif()
        
            if(DEFINED ${userPlugin}_SOURCE_DIR AND DEFINED ${userPlugin}_IMPORT_FILE)
                # include (${${userPlugin}_IMPORT_FILE} )
                add_subdirectory(${${userPlugin}_SOURCE_DIR} ${DREAM3D_BINARY_DIR}/Plugins/${userPlugin}) 
            endif()
        endforeach(userPlugin ${DREAM3D_EXTRA_PLUGINS})


   endif()
   
   # Build the StatsGenerator Application
   OPTION (DREAM3D_BUILD_STATS_GENERATOR "Compile the Statistics Generator GUI Application" ON)
   if (DREAM3D_BUILD_STATS_GENERATOR)
    add_subdirectory(${DREAM3D_SOURCE_DIR}/Source/Applications/StatsGenerator ${DREAM3D_BINARY_DIR}/Applications/StatsGenerator)
   endif()
   
else()
    set (DREAM3D_BUILD_GUI "OFF" CACHE BOOL "" FORCE)
    set (DREAM3D_BUILD_STATS_GENERATOR "OFF" CACHE BOOL "" FORCE)
endif()

# add the Reconstruction Command line Tool
IF ( DREAM3D_BUILD_CLI_TOOLS )
    add_subdirectory( ${DREAM3D_SOURCE_DIR}/Source/Tools ${DREAM3D_BINARY_DIR}/Tools)
endif()

# ------- Enable the CTest testing. Use make test to run all tests ---------------
OPTION(DREAM3D_BUILD_TESTING "Compile the test programs" ON)
if (DREAM3D_BUILD_TESTING)
    ENABLE_TESTING()
    add_subdirectory(${DREAM3D_SOURCE_DIR}/Source/Test ${DREAM3D_BINARY_DIR}/Test)
endif()

# Should we build the Example program that was originally for CMU's use
option(DREAM3D_BUILD_CMU "Compile the CMU Optimization Wrappers" OFF)
if(DREAM3D_BUILD_CMU)
    add_subdirectory(${DREAM3D_SOURCE_DIR}/Source/CMU ${DREAM3D_BINARY_DIR}/AIM/CMU)
endif()

# Should we build the documentation using DOxygen
option(DREAM3D_BUILD_DOCS "Use Doxygen to create the User Manual, Tutorial, Developer and API documentation" OFF)
if(DREAM3D_BUILD_DOCS)
  FIND_PACKAGE(Doxygen)
  if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is needed to build the documentation. Please install it correctly")
  endif()
  # Add a target that builds just the High Level User Manuals/Tutorial type documentation
  # add_subdirectory(${PROJECT_SOURCE_DIR}/Documentation ${PROJECT_BINARY_DIR}/Documentation)
  
  # Generating the API Docs will also include the other "Developer Documentation"
  option(DREAM3D_BUILD_API_DOCS "Use Doxygen to generate the HTML API documentation" ON)
  if (DREAM3D_BUILD_API_DOCS)
      set (DOXY_OUTPUT_DIR ${PROJECT_BINARY_DIR}/API-Documentation)
      configure_file(${PROJECT_RESOURCES_DIR}/AIM.doxyfile.in 
                     ${PROJECT_BINARY_DIR}/Doxyfile  @ONLY IMMEDIATE)
      add_custom_target(apidocs COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile)
  endif()
  
endif()

# Option to have CMake Generate the Skeleton Code for a UI Plugin
option(DREAM3D_CREATE_PLUGIN "Create the Skeleton Code for a new Pipeline and GUI Plugin" OFF)
if (DREAM3D_CREATE_PLUGIN)
    add_subdirectory(${PROJECT_SOURCE_DIR}/Support/PluginMaker ${PROJECT_BINARY_DIR}/PluginMaker)
 #   set (DREAM3D_CREATE_PLUGIN OFF CACHE "" FORCE)
endif()

# This should be the last line in this file:
include(${PROJECT_RESOURCES_DIR}/CPack/PackageProject.cmake)

