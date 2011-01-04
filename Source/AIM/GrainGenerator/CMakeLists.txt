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
PROJECT( GrainGenerator )
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

add_definitions(-DSVG_3D_LIBRARY)

set (GrainGenerator_SRCS
    ${GrainGenerator_SOURCE_DIR}/GrainGenerator.cpp
    ${GrainGenerator_SOURCE_DIR}/main.cpp
)

set (GrainGenerator_HDRS 
    ${GrainGenerator_SOURCE_DIR}/GrainGenerator.h
)
cmp_IDE_SOURCE_PROPERTIES( "Source/AIM/GrainGenerator" "${GrainGenerator_HDRS}" "${GrainGenerator_SRCS}" ${PROJECT_INSTALL_HEADERS} )

set (PROJECT_SRCS
    ${GrainGenerator_SRCS}
    ${GrainGenerator_HDRS}
)
    

ADD_EXECUTABLE ( ${PROJECT_NAME}  ${PROJECT_SRCS} )
target_link_libraries(${PROJECT_NAME} 
                        MXADataModel
                        AIMCommon 
                        ${Boost_PROGRAM_OPTIONS_LIBRARY} )
cmp_ToolInstallationSupport(${PROJECT_NAME} 
                        "${EXE_DEBUG_EXTENSION}" 
                        ${${PROJECT_NAME}_BINARY_DIR} 
                        "${PROJECT_INSTALL_EXECUTABLES}"
                        "Applications"
                        "Utilities"
                        )
    
    