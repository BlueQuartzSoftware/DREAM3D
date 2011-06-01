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
# Set project's name
PROJECT( Reconstruction )
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
  
SET (Reconstruction_SRCS
    ${Reconstruction_SOURCE_DIR}/main.cpp
    ${PROJECT_CODE_DIR}/DREAM3D/Reconstruction/Reconstruction.cpp
)
SET (Reconstruction_HDRS
    ${PROJECT_CODE_DIR}/DREAM3D/Reconstruction/Reconstruction.h
    )
cmp_IDE_SOURCE_PROPERTIES( "AIM/Reconstruction" "${Reconstruction_HDRS}" "${Reconstruction_SRCS}" ${PROJECT_INSTALL_HEADERS})

 
set (${PROJECT_NAME}_PROJECT_SRCS
    ${Reconstruction_SRCS}
    ${Reconstruction_HDRS}
    ${COMMON_SRCS}
    ${COMMON_HDRS}
    ${ANG_SRCS}
    ${ANG_HDRS}
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
    LINK_LIBRARIES DREAM3DLib
    LIB_SEARCH_DIRS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMPONENT     Applications
    INSTALL_DEST  "${install_dir}"
)   
