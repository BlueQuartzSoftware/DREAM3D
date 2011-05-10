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
# set project's name
PROJECT( SurfaceMesh )
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

set (SurfaceMesh_SRCS
    ${SurfaceMesh_SOURCE_DIR}/main.cpp
    ${SurfaceMesh_SOURCE_DIR}/SurfaceMesh.cpp
    ${SurfaceMesh_SOURCE_DIR}/SMVtkFileIO.cpp
)
set (SurfaceMesh_HDRS
    ${SurfaceMesh_SOURCE_DIR}/SurfaceMesh.h
)
cmp_IDE_SOURCE_PROPERTIES( "AIM/SurfaceMesh" "${SurfaceMesh_HDRS}" "${SurfaceMesh_SRCS}" ${PROJECT_INSTALL_HEADERS})
                      
set (${PROJECT_NAME}_PROJECT_SRCS
    ${SurfaceMesh_SRCS}
    ${SurfaceMesh_HDRS}
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
    VERSION_MAJOR ${AIMRepresentation_VER_MAJOR}
    VERSION_MINOR ${AIMRepresentation_VER_MINOR}
    VERSION_PATCH ${AIMRepresentation_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    LINK_LIBRARIES AIMCommon
    LIB_SEARCH_DIRS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMPONENT     Applications
    INSTALL_DEST  "${install_dir}"
)   
