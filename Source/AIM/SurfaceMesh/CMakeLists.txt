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


set (SurfaceMeshMCALayer_SRCS 
 ${SurfaceMesh_SOURCE_DIR}/Surface_Mesh_General_MCA_Layer.c )
set (SurfaceMeshMCALayer_HDRS 
 ${SurfaceMesh_SOURCE_DIR}/LinearAlgebra.h )
IDE_SOURCE_PROPERTIES( "Source/AIM/SurfaceMesh" "${SurfaceMeshMCALayer_HDRS}" "${SurfaceMeshMCALayer_SRCS}")

add_executable(SL_SurfaceMesh ${SurfaceMeshMCALayer_SRCS} ${SurfaceMeshMCALayer_HDRS} )
ToolInstallationSupport(SL_SurfaceMesh "${EXE_DEBUG_EXTENSION}" ${AIMRepresentation_BINARY_DIR} )


set (SurfaceMeshSmoothing_SRCS 
 ${SurfaceMesh_SOURCE_DIR}/smooth-grain3d.cpp )
set (SurfaceMeshSmoothing_HDRS 
 ${SurfaceMesh_SOURCE_DIR}/LinearAlgebra.h )
IDE_SOURCE_PROPERTIES( "Source/AIM/SurfaceMesh" "${SurfaceMeshSmoothing_HDRS}" "${SurfaceMeshSmoothing_SRCS}")

add_executable(SL_MeshSmoothing ${SurfaceMeshSmoothing_SRCS} ${SurfaceMeshSmoothing_HDRS} )
ToolInstallationSupport(SL_MeshSmoothing "${EXE_DEBUG_EXTENSION}" ${AIMRepresentation_BINARY_DIR} )

# Add in a target for the command line version that combines all the above code
set (SurfaceMesh_SRCS
    ${SurfaceMesh_SOURCE_DIR}/main.cpp
    ${SurfaceMesh_SOURCE_DIR}/SurfaceMesh.cpp
)
set (SurfaceMesh_HDRS
    ${SurfaceMesh_SOURCE_DIR}/SurfaceMesh.h
)
IDE_SOURCE_PROPERTIES( "Source/AIM/SurfaceMesh" "${SurfaceMesh_HDRS};${SurfaceMeshSmoothing_HDRS};${SurfaceMeshMCALayer_HDRS}" 
  "${SurfaceMesh_SRCS};${SurfaceMeshSmoothing_SRCS};${SurfaceMeshMCALayer_SRCS}")

set (Project_SRCS
    ${SurfaceMesh_HDRS}
    ${SurfaceMeshSmoothing_HDRS}
    ${SurfaceMeshMCALayer_HDRS}
    ${SurfaceMesh_SRCS}
    ${SurfaceMeshSmoothing_SRCS}
    ${SurfaceMeshMCALayer_SRCS}
)


add_executable(SurfaceMesh ${Project_SRCS} )
ToolInstallationSupport(SurfaceMesh "${EXE_DEBUG_EXTENSION}" ${AIMRepresentation_BINARY_DIR} )


