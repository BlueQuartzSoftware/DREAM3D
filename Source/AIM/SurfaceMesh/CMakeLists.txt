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


add_executable(SurfaceMesh ${SurfaceMeshMCALayer_SRCS} ${SurfaceMeshMCALayer_HDRS} )
ToolInstallationSupport(SurfaceMesh "${EXE_DEBUG_EXTENSION}" ${AIMRepresentation_BINARY_DIR} )



set (SurfaceMeshSmoothing_SRCS 
 ${SurfaceMesh_SOURCE_DIR}/smooth-grain3d.cpp )
set (SurfaceMeshSmoothing_HDRS 
 ${SurfaceMesh_SOURCE_DIR}/LinearAlgebra.h )
IDE_SOURCE_PROPERTIES( "Source/AIM/SurfaceMesh" "${SurfaceMeshSmoothing_HDRS}" "${SurfaceMeshSmoothing_SRCS}")


add_executable(SurfaceMeshSmoothing ${SurfaceMeshSmoothing_SRCS} ${SurfaceMeshSmoothing_HDRS} )
ToolInstallationSupport(SurfaceMeshSmoothing "${EXE_DEBUG_EXTENSION}" ${AIMRepresentation_BINARY_DIR} )



