# Set project's name
PROJECT( Reconstruction )
cmake_minimum_required(VERSION 2.6)

# ---------- Setup output Directories -------------------------
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY
  /Bin
  CACHE PATH
  "Single Directory for all Libraries"
  )

# --------- Setup the Executable output Directory -------------
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
  /Bin
  CACHE PATH
  "Single Directory for all Executables."
  )

# --------- Setup the Executable output Directory -------------
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
  /Bin
  CACHE PATH
  "Single Directory for all static libraries."
  )

SET (Reconstruction_SRCS
    ${Reconstruction_SOURCE_DIR}/main.cpp
)
SET (Reconstruction_HDRS
)
IDE_SOURCE_PROPERTIES( "Source/AIM/Reconstruction" "${Reconstruction_HDRS}" "${Reconstruction_SRCS}")

SET (COMMON_SRCS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/MicroGen3D.h
)

SET (COMMON_HDRS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/MicroGen3D.cpp
)
IDE_SOURCE_PROPERTIES( "Source/AIM/Common" "${COMMON_HDRS}" "${COMMON_SRCS}")


ADD_EXECUTABLE ( Reconstruction  ${Reconstruction_SRCS} ${Reconstruction_HDRS} ${COMMON_SRCS} ${COMMON_HDRS})
target_link_libraries(Reconstruction MXA)
ToolInstallationSupport(Reconstruction "${EXE_DEBUG_EXTENSION}" ${AIMRepresentation_BINARY_DIR} )
