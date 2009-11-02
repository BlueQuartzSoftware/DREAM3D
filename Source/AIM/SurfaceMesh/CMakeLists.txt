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

set(m_lib "")
if(UNIX)
  set (m_lib "m")
endif(UNIX)

set (SurfaceMeshMCALayer_SRCS 
 ${SurfaceMesh_SOURCE_DIR}/Surface_Mesh_General_MCA_Layer.c )
set (SurfaceMeshMCALayer_HDRS 
 ${SurfaceMesh_SOURCE_DIR}/LinearAlgebra.h 
 ${SurfaceMesh_SOURCE_DIR}/Surface_Mesh_General_MCA_Layer.h)
IDE_SOURCE_PROPERTIES( "Source/AIM/SurfaceMesh" "${SurfaceMeshMCALayer_HDRS}" "${SurfaceMeshMCALayer_SRCS}")

add_executable(SL_SurfaceMesh ${SurfaceMeshMCALayer_SRCS} ${SurfaceMeshMCALayer_HDRS} )
target_link_libraries(SL_SurfaceMesh ${m_lib})
ToolInstallationSupport(SL_SurfaceMesh "${EXE_DEBUG_EXTENSION}" ${AIMRepresentation_BINARY_DIR} )

# Add the command line version of the Smoothing program
set (SurfaceMeshSmoothing_SRCS 
 ${SurfaceMesh_SOURCE_DIR}/smooth-grain3d.cpp )
set (SurfaceMeshSmoothing_HDRS
${SurfaceMesh_SOURCE_DIR}/smooth-grain3d.h 
 ${SurfaceMesh_SOURCE_DIR}/LinearAlgebra.h )
IDE_SOURCE_PROPERTIES( "Source/AIM/SurfaceMesh" "${SurfaceMeshSmoothing_HDRS}" "${SurfaceMeshSmoothing_SRCS}")
add_executable(SL_MeshSmoothing ${SurfaceMeshSmoothing_SRCS} ${SurfaceMeshSmoothing_HDRS} )
target_link_libraries(SL_MeshSmoothing 
                      MXA
                      ${Boost_PROGRAM_OPTIONS_LIBRARY} )
ToolInstallationSupport(SL_MeshSmoothing "${EXE_DEBUG_EXTENSION}" ${AIMRepresentation_BINARY_DIR} )

# Add a command line version of the UpdateEdgeTri program
set (UpdateEdgeTri_SRCS 
 ${SurfaceMesh_SOURCE_DIR}/Update_Edge_Tri.c )
set (UpdateEdgeTri_HDRS 
 ${SurfaceMesh_SOURCE_DIR}/Update_Edge_Tri.h)
IDE_SOURCE_PROPERTIES( "Source/AIM/SurfaceMesh" "${UpdateEdgeTri_HDRS}" "${UpdateEdgeTri_SRCS}")
add_executable(SL_UpdateEdgeTri ${UpdateEdgeTri_SRCS} ${UpdateEdgeTri_HDRS} )
ToolInstallationSupport(SL_UpdateEdgeTri "${EXE_DEBUG_EXTENSION}" ${AIMRepresentation_BINARY_DIR} )


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
    ${SurfaceMesh_SRCS}
    ${SurfaceMesh_HDRS}
    
    ${SurfaceMeshSmoothing_SRCS}
    ${SurfaceMeshSmoothing_HDRS}

    ${SurfaceMeshMCALayer_SRCS}
    ${SurfaceMeshMCALayer_HDRS}
    
    ${UpdateEdgeTri_SRCS}
    ${UpdateEdgeTri_HDRS}
)


add_executable(SurfaceMesh ${Project_SRCS} )
target_link_libraries(SurfaceMesh 
                        MXA
                        AIMCommon
                        ${Boost_PROGRAM_OPTIONS_LIBRARY} 
                        ${m_lib})
set_property(TARGET SurfaceMesh PROPERTY  
                COMPILE_DEFINITIONS "SURFACE_MESH_LIBRARY" 
                COMPILE_DEFINITIONS "UPDATE_EDGE_TRI_LIBRARY" 
                COMPILE_DEFINITIONS "SMOOTH_MESH_LIBRARY")
ToolInstallationSupport(SurfaceMesh "${EXE_DEBUG_EXTENSION}" ${AIMRepresentation_BINARY_DIR} )


