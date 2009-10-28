# Set project's name
PROJECT( Reconstruction )
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
  
SET (Reconstruction_SRCS
    ${Reconstruction_SOURCE_DIR}/main.cpp
    ${Reconstruction_SOURCE_DIR}/Reconstruction.cpp
)
SET (Reconstruction_HDRS
    ${Reconstruction_SOURCE_DIR}/Reconstruction.h
    )
IDE_SOURCE_PROPERTIES( "Source/AIM/Reconstruction" "${Reconstruction_HDRS}" "${Reconstruction_SRCS}")

 
set (PROJECT_SRCS
    ${Reconstruction_SRCS}
    ${Reconstruction_HDRS}
    ${COMMON_SRCS}
    ${COMMON_HDRS}
    ${ANG_SRCS}
    ${ANG_HDRS}
    )

ADD_EXECUTABLE ( Reconstruction  ${PROJECT_SRCS} )
target_link_libraries(Reconstruction 
                        MXA
                        AIMCommon
                        ${Boost_PROGRAM_OPTIONS_LIBRARY} )
ToolInstallationSupport(Reconstruction "${EXE_DEBUG_EXTENSION}" ${AIMRepresentation_BINARY_DIR} )
