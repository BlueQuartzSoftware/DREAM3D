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
)
set (SurfaceMesh_HDRS
    ${SurfaceMesh_SOURCE_DIR}/SurfaceMesh.h
)
cmp_IDE_SOURCE_PROPERTIES( "AIM/SurfaceMesh" "${SurfaceMesh_HDRS}" "${SurfaceMesh_SRCS}" ${CMP_INSTALL_FILES})
                      
set (Project_SRCS
    ${SurfaceMesh_SRCS}
    ${SurfaceMesh_HDRS}
    )


add_executable(SurfaceMesh ${Project_SRCS} )
target_link_libraries(SurfaceMesh 
                        MXADataModel
                        AIMCommon
                        ${Boost_PROGRAM_OPTIONS_LIBRARY} )
cmp_ToolInstallationSupport(SurfaceMesh "${EXE_DEBUG_EXTENSION}" ${SurfaceMesh_BINARY_DIR} )


