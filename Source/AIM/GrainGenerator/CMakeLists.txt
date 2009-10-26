# set project's name
PROJECT( GrainGenerator )
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

set (GrainGenerator_SRCS 
    ${GrainGenerator_SOURCE_DIR}/GrainGenerator.cpp
    ${GrainGenerator_SOURCE_DIR}/main.cpp
)

set (GrainGenerator_HDRS 
    ${GrainGenerator_SOURCE_DIR}/GrainGenerator.h
)
IDE_SOURCE_PROPERTIES( "Source/AIM/GrainGenerator" "${GrainGenerator_HDRS}" "${GrainGenerator_SRCS}")

SET (COMMON_SRCS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/MicroGen3D.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Grain.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Voxel.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Bin.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Orient.cpp   
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/AIMRandomNG.cpp
    )

SET (COMMON_HDRS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/MicroGen3D.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Grain.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Voxel.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Bin.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Orient.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/AIMRandomNG.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/FeiConstants.h
)
IDE_SOURCE_PROPERTIES( "Source/AIM/Common" "${COMMON_HDRS}" "${COMMON_SRCS}")

set (PROJECT_SRCS
    ${GrainGenerator_SRCS}
    ${GrainGenerator_HDRS}
    ${COMMON_SRCS}
    ${COMMON_HDRS}
)
    
    
    
    