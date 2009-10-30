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
project (AIMRepresentation)
cmake_minimum_required(VERSION 2.6)

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


SET (AIMRepresentation_PROJECT_DIR ${AIMRepresentation_SOURCE_DIR})
SET (AIMRepresentation_RESOURCES_DIR ${AIMRepresentation_SOURCE_DIR}/Resources)
set (AIMRepresentation_CMAKE_DIR  ${AIMRepresentation_RESOURCES_DIR}/CMake)
SET (OSX_TOOLS_DIR ${AIMRepresentation_RESOURCES_DIR}/OSX_Tools)
SET (MXADATAMODEL_LIB_NAME MXADataModel)


# Generate the version string for the projects
SET(${PROJECT_NAME}_VERSION "1970.01.01")
SET(${PROJECT_NAME}_VER_MAJOR "1970")
SET(${PROJECT_NAME}_VER_MINOR "01")
SET(${PROJECT_NAME}_VER_PATCH "01")
SET(SSE_COMPILE_FLAGS "")

include (${AIMRepresentation_RESOURCES_DIR}/CMake/AIMTestTypes.cmake )
include (${AIMRepresentation_RESOURCES_DIR}/CMake/ConfigureChecks.cmake)


# ---------- Find Boost Headers/Libraries -----------------------
SET (Boost_FIND_REQUIRED TRUE)
SET (Boost_FIND_QUIETLY TRUE)
SET (Boost_DEBUG FALSE)
set (Boost_USE_MULTITHREADED TRUE)
set (Boost_USE_STATIC_LIBS TRUE)
SET (Boost_ADDITIONAL_VERSIONS "1.39" "1.39.0")

FIND_PACKAGE(Boost COMPONENTS  program_options )
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

# --------------------------------------------------------------------
# Add in some compiler definitions
# --------------------------------------------------------------------
IF ( CMAKE_BUILD_TYPE MATCHES Debug )
  ADD_DEFINITIONS(-DDEBUG)
  ADD_DEFINITIONS(-Wall)
ENDIF ( CMAKE_BUILD_TYPE MATCHES Debug )

# --------------------------------------------------------------------
# If was are using GCC, make the compiler messages on a single line
IF(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCXX) 

# Include some directories for the compiler
include_directories(${PROJECT_BINARY_DIR})
include_directories(${AIMRepresentation_SOURCE_DIR}/Source)

#-- Pull in some macros
include (${AIMRepresentation_RESOURCES_DIR}/CMake/AIMCMakeMacros.cmake)
SET (EXE_DEBUG_EXTENSION "_debug")
include (${AIMRepresentation_RESOURCES_DIR}/CMake/InstallationSupport.cmake)


GenerateVersionString(${PROJECT_NAME})
configure_file(${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/AIMVersion.h.in
               ${AIMRepresentation_BINARY_DIR}/AIM/Common/AIMVersion.h @ONLY)
               
# -----------------------------------------------------------------------
# -- Add in the Necessary MXA Sources into a static library
add_subdirectory( ${AIMRepresentation_SOURCE_DIR}/Source/MXA ${AIMRepresentation_BINARY_DIR}/MXA)
include_directories (${AIMRepresentation_BINARY_DIR})

# -----------------------------------------------------------------------
# -- Add in the AIM Common Sources into a static library
add_subdirectory( ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common ${AIMRepresentation_BINARY_DIR}/AIM)


option(AIM_BUILD_QT_APPS "Compile the Qt based GUI applications" ON)
#if we are going to build Qt based applications then we need to find Qt 4 first
if (AIM_BUILD_QT_APPS)
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
   # add some useful macros and variables
   # (QT_USE_FILE is a variable defined by FIND_PACKAGE( Qt4 ) that 
   # contains a path to CMake script)
   INCLUDE( ${QT_USE_FILE} )
   
   # Add our Qt based GUI project to the list of targets
   ADD_SUBDIRECTORY( ${AIMRepresentation_SOURCE_DIR}/Source/AIM/GUI ${AIMRepresentation_BINARY_DIR}/AIM/GUI)
   
endif()

include_directories(${AIMRepresentation_SOURCE_DIR}/Source)

# add the Reconstruction Command line Tool
OPTION(AIM_BUILD_RECONSTRUCTION_TOOL "Compile Reconstruction Tool" OFF)
IF ( AIM_BUILD_RECONSTRUCTION_TOOL )
    ADD_SUBDIRECTORY( ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Reconstruction ${AIMRepresentation_BINARY_DIR}/AIM/Reconstruction)
ENDIF ()

OPTION (AIM_BUILD_GRAIN_GENERATOR_TOOL "Compile the Grain Generator Tool" OFF)
if (AIM_BUILD_GRAIN_GENERATOR_TOOL)
    add_subdirectory(${AIMRepresentation_SOURCE_DIR}/Source/AIM/GrainGenerator ${AIMRepresentation_BINARY_DIR}/AIM/GrainGenerator)
endif()

OPTION (AIM_BUILD_SURFACE_MESHING_TOOLS "Compile the surface Meshing tools" OFF)
if (AIM_BUILD_SURFACE_MESHING_TOOLS)
    add_subdirectory(${AIMRepresentation_SOURCE_DIR}/Source/AIM/SurfaceMesh ${AIMRepresentation_BINARY_DIR}/AIM/SurfaceMesh)
endif()

OPTION (AIM_BUILD_VOLUME_MESHING_TOOL "Compile the Volume Meshing tools" OFF)
if (AIM_BUILD_VOLUME_MESHING_TOOL)
    add_subdirectory(${AIMRepresentation_SOURCE_DIR}/Source/AIM/VolumeMesh ${AIMRepresentation_BINARY_DIR}/AIM/VolumeMesh)
endif()

#ADD_SUBDIRECTORY( ${AIMRepresentation_SOURCE_DIR}/Source/Test ${AIMRepresentation_BINARY_DIR}/Test)
include(${AIMRepresentation_RESOURCES_DIR}/CMake/AIMInstall.cmake)

