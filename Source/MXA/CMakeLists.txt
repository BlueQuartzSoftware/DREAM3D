# set project's name
PROJECT( MXA )
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


#-- Configure the Header file and then add the Binary Directory to the Includes list
CONFIGURE_FILE(${MXA_SOURCE_DIR}/Common/MXAConfiguration.h.in
               ${MXA_BINARY_DIR}/MXAConfiguration.h @ONLY IMMEDIATE)
               

SET (MXA_UTILITIES_SRCS 
    ${MXA_SOURCE_DIR}/Utilities/MXALogger.cpp
    ${MXA_SOURCE_DIR}/Utilities/MXADir.cpp
    ${MXA_SOURCE_DIR}/Utilities/MXAFileInfo.cpp
     )
SET (MXA_UTILITIES_HDRS 
    ${MXA_SOURCE_DIR}/Utilities/MXALogger.h
    ${MXA_SOURCE_DIR}/Utilities/MXADir.h
    ${MXA_SOURCE_DIR}/Utilities/MXAFileInfo.h
    )
    
IF (MSVC)
    SET (MXA_UTILITIES_SRCS ${MXA_UTILITIES_SRCS} ${MXA_SOURCE_DIR}/Utilities/MXADirent.c )
    SET (MXA_UTILITIES_HEADERS ${MXA_UTILITIES_HEADERS} ${MXA_SOURCE_DIR}/Utilities/MXADirent.h )
endif()
IDE_SOURCE_PROPERTIES( "Source/MXA/Utilities" "${MXA_UTILITIES_HDRS}" "${MXA_UTILITIES_SRCS}")

SET (MXA_COMMON_SRCS "")
SET (MXA_COMMON_HDRS 
    ${MXA_SOURCE_DIR}/Common/DLLExport.h
    ${MXA_SOURCE_DIR}/Common/LogTime.h
    ${MXA_SOURCE_DIR}/Common/MXASetGetMacros.h
    ${MXA_SOURCE_DIR}/Common/MXATypes.h
)
IDE_SOURCE_PROPERTIES("Source/MXA/Common" "${MXA_COMMON_HDRS}" "${MXA_COMMON_SRCS}")

set (MXALib_SRCS
    ${MXA_COMMON_SRCS}
    ${MXA_COMMON_HDRS}
    ${MXA_UTILITIES_SRCS}
    ${MXA_UTILITIES_HDRS}
    )
    
include_directories( ${MXA_BINARY_DIR} )
#-- Add a static library for the PCM registration
add_library(MXA STATIC ${MXALib_SRCS} )
StaticLibraryProperties( MXA )


  