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
    ${Reconstruction_SOURCE_DIR}/Reconstruction.cpp
)
SET (Reconstruction_HDRS
    ${Reconstruction_SOURCE_DIR}/Reconstruction.h
    )
cmp_IDE_SOURCE_PROPERTIES( "AIM/Reconstruction" "${Reconstruction_HDRS}" "${Reconstruction_SRCS}" ${PROJECT_INSTALL_HEADERS})

 
set (PROJECT_SRCS
    ${Reconstruction_SRCS}
    ${Reconstruction_HDRS}
    ${COMMON_SRCS}
    ${COMMON_HDRS}
    ${ANG_SRCS}
    ${ANG_HDRS}
    )


ADD_EXECUTABLE ( ${PROJECT_NAME}  ${PROJECT_SRCS} )
target_link_libraries(${PROJECT_NAME}
                        # MXADataModel
                        AIMCommon
                        )
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
