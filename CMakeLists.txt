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
cmake_minimum_required(VERSION 2.8)

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
set (CMP_LIB_SEARCH_DIRS "")
INCLUDE (${CMP_SOURCE_DIR}/cmpProject.cmake)

# --------------------------------------------------------------------
# Generate a Header file with Compile Version variables
# --------------------------------------------------------------------
cmpGenerateVersionString( "${DREAM3D_BINARY_DIR}/AIM/Common/${PROJECT_NAME}Version.h" 
                          "DREAM3D" 
                          "DREAM3D")
                          
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
# Should we write Binary or ASCII VTK output files
# --------------------------------------------------------------------
set(AIM_WRITE_BINARY_VTK_FILE)
option(AIM_WRITE_BINARY_VTK_FILES "Write Binary VTK Files instead of ASCII files" ON)
if (AIM_WRITE_BINARY_VTK_FILES)
 set(AIM_WRITE_BINARY_VTK_FILE 1)
endif()

# --------------------------------------------------------------------
# Should we use Intel Threading Building Blocks
# --------------------------------------------------------------------
set (AIM_USE_PARALLEL_ALGORITHMS "")
option(AIM_USE_MULTITHREADED_ALGOS "Use MultiThreaded Algorithms where possible" OFF)
if (AIM_USE_MULTITHREADED_ALGOS)
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
    set (AIM_USE_PARALLEL_ALGORITHMS "1")
endif()


# --------------------------------------------------------------------
# If was are using GCC, make the compiler messages on a single line
IF(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCXX) 

               
# -----------------------------------------------------------------------
# -- Add in the Necessary MXA Sources into a static library
add_subdirectory( ${DREAM3D_SOURCE_DIR}/Source/MXA ${DREAM3D_BINARY_DIR}/MXA)
include_directories (${DREAM3D_BINARY_DIR})

# -----------------------------------------------------------------------
# -- Add in the Necessary MXA Sources into a static library
add_subdirectory( ${DREAM3D_SOURCE_DIR}/Source/ANG ${DREAM3D_BINARY_DIR}/ANG)
include_directories (${DREAM3D_BINARY_DIR})

# -----------------------------------------------------------------------
# -- Add in the AIM Common Sources into a static library
add_subdirectory( ${DREAM3D_SOURCE_DIR}/Source/AIM/Common ${DREAM3D_BINARY_DIR}/AIM)


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
   
   add_subdirectory(${DREAM3D_SOURCE_DIR}/Source/QtSupport ${DREAM3D_BINARY_DIR}/QtSupport)
   # Add our Qt based GUI project to the list of targets

   OPTION (AIM_BUILD_GUI_STATS_GENERATOR "Compile the Statistics Generator GUI Application" ON)
   if (AIM_BUILD_GUI_STATS_GENERATOR)
    add_subdirectory(${DREAM3D_SOURCE_DIR}/Source/StatsGenerator ${DREAM3D_BINARY_DIR}/Apps/StatsGenerator)
   endif()
      
   option(AIM_BUILD_GUI_REPRESENTATION "Compile the DREAM3D GUI Application" ON)
   if (AIM_BUILD_GUI_REPRESENTATION)
       ADD_SUBDIRECTORY( ${DREAM3D_SOURCE_DIR}/Source/AIM/GUI ${DREAM3D_BINARY_DIR}/Apps/Representation)
   endif()
else()
    set (AIM_BUILD_GUI_REPRESENTATION "OFF" CACHE BOOL "" FORCE)
    set (AIM_BUILD_GUI_STATS_GENERATOR "OFF" CACHE BOOL "" FORCE)
endif()

include_directories(${DREAM3D_SOURCE_DIR}/Source)

# add the Reconstruction Command line Tool
OPTION(AIM_BUILD_TOOL_RECONSTRUCTION "Compile Reconstruction Tool" OFF)
IF ( AIM_BUILD_TOOL_RECONSTRUCTION )
    ADD_SUBDIRECTORY( ${DREAM3D_SOURCE_DIR}/Source/AIM/Reconstruction ${DREAM3D_BINARY_DIR}/AIM/Reconstruction)
ENDIF ()

OPTION (AIM_BUILD_TOOL_GRAIN_GENERATOR "Compile the Grain Generator Tool" OFF)
if (AIM_BUILD_TOOL_GRAIN_GENERATOR)
    add_subdirectory(${DREAM3D_SOURCE_DIR}/Source/AIM/GrainGenerator ${DREAM3D_BINARY_DIR}/AIM/GrainGenerator)
endif()

OPTION (AIM_BUILD_TOOL_SURFACE_MESHING "Compile the surface Meshing tool" OFF)
if (AIM_BUILD_TOOL_SURFACE_MESHING)
    add_subdirectory(${DREAM3D_SOURCE_DIR}/Source/AIM/SurfaceMesh ${DREAM3D_BINARY_DIR}/AIM/SurfaceMesh)
endif()

OPTION (AIM_BUILD_TOOL_VOLUME_MESHING "Compile the Volume Meshing tools" OFF)
if (AIM_BUILD_TOOL_VOLUME_MESHING)
    add_subdirectory(${DREAM3D_SOURCE_DIR}/Source/AIM/VolumeMesh ${DREAM3D_BINARY_DIR}/AIM/VolumeMesh)
endif()

OPTION(AIM_BUILD_TESTING "Compile the test programs" ON)
if (AIM_BUILD_TESTING)
    add_subdirectory(${DREAM3D_SOURCE_DIR}/Source/Test ${DREAM3D_BINARY_DIR}/AIM/Test)
endif()


option(AIM_BUILD_CMU "Compile the CMU Optimization Wrappers" OFF)
if(AIM_BUILD_CMU)
    add_subdirectory(${DREAM3D_SOURCE_DIR}/Source/CMU ${DREAM3D_BINARY_DIR}/AIM/CMU)
endif()

# Should we build the HTML documentation
option(AIM_BUILD_DOCS "Use Doxygen to create the HTML based API documentation" OFF)
if(AIM_BUILD_DOCS)
  FIND_PACKAGE(Doxygen)
  if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is needed to build the documentation. Please install it correctly")
  endif()
  # Add a target that builds just the High Level User Manuals/Tutorial type documentation
  add_subdirectory(${PROJECT_SOURCE_DIR}/Documentation ${PROJECT_BINARY_DIR}/Documentation)
  
  option(AIM_BUILD_API_DOCS "Use Doxygen to generate the HTML API documentation" ON)
  if (AIM_BUILD_API_DOCS)
      configure_file(${PROJECT_RESOURCES_DIR}/AIM.doxyfile.in 
                 ${PROJECT_BINARY_DIR}/Doxyfile  @ONLY IMMEDIATE)
    add_custom_target(apidocs COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile)
  endif()
  
endif()

include(${PROJECT_RESOURCES_DIR}/CPack/PackageProject.cmake)

