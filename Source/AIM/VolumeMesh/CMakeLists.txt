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
    ${VolumeMesh_SOURCE_DIR}/MeshGenerator.cpp
    ${VolumeMesh_SOURCE_DIR}/VolumeMesh.cpp
)
set (VolumeMesh_HDRS
    ${VolumeMesh_SOURCE_DIR}/MeshGenerator.h
    ${VolumeMesh_SOURCE_DIR}/VolumeMesh.h
)

cmp_IDE_SOURCE_PROPERTIES( "AIM/VolumeMesh" "${VolumeMesh_HDRS}" "${VolumeMesh_SRCS}" ${PROJECT_INSTALL_HEADERS})

set (Project_SRCS
    ${VolumeMesh_HDRS}
    ${VolumeMesh_SRCS}
)


add_executable(${PROJECT_NAME} ${Project_SRCS} )
target_link_libraries(${PROJECT_NAME}
                        #MXADataModel
                        AIMCommon )
set_property(TARGET ${PROJECT_NAME} PROPERTY  COMPILE_DEFINITIONS "VOLUME_MESH_LIBRARY" )
# --------------------------------------------------------------------
# Setup the install rules for the various platforms
set(install_dir "tools")
if (WIN32)
    set (install_dir ".")
endif()
cmp_ToolInstallationSupport(${PROJECT_NAME} 
                        "${EXE_DEBUG_EXTENSION}" 
                        ${${PROJECT_NAME}_BINARY_DIR} 
                        "${PROJECT_INSTALL_EXECUTABLES}"
                        "Applications"
                        "${install_dir}"
                        "${CMP_LIB_SEARCH_DIRS}"
                        )
