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

message(FATAL_ERROR "Volume Mesh is NOT working. All the sources need to be updated")

# set project's name
PROJECT( VolumeMesh )
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


# Add in a target for the command line version that combines all the above code
set (VolumeMesh_SRCS
    ${VolumeMesh_SOURCE_DIR}/main.cpp
#    ${VolumeMesh_SOURCE_DIR}/MeshGenerator.cpp
    ${VolumeMesh_SOURCE_DIR}/VolumeMesh.cpp
)
set (VolumeMesh_HDRS
#    ${VolumeMesh_SOURCE_DIR}/MeshGenerator.h
    ${VolumeMesh_SOURCE_DIR}/VolumeMesh.h
)

cmp_IDE_SOURCE_PROPERTIES( "AIM/VolumeMesh" "${VolumeMesh_HDRS}" "${VolumeMesh_SRCS}" ${PROJECT_INSTALL_HEADERS})

set (${PROJECT_NAME}_PROJECT_SRCS
    ${VolumeMesh_HDRS}
    ${VolumeMesh_SRCS}
)

# --------------------------------------------------------------------
# Setup the install rules for the various platforms
set(install_dir "tools")
if (WIN32)
    set (install_dir ".")
endif()

BuildToolBundle(
    TARGET ${PROJECT_NAME}
    SOURCES ${${PROJECT_NAME}_PROJECT_SRCS}
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    LINK_LIBRARIES AIMCommon
    LIB_SEARCH_DIRS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMPONENT     Applications
    INSTALL_DEST  "${install_dir}"
)   
set_property(TARGET ${PROJECT_NAME} PROPERTY  COMPILE_DEFINITIONS "VOLUME_MESH_LIBRARY" )
