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
    ${Reconstruction_SOURCE_DIR}/Reconstruction.cpp
)
SET (Reconstruction_HDRS
    ${Reconstruction_SOURCE_DIR}/Reconstruction.h
    )
IDE_SOURCE_PROPERTIES( "Source/AIM/Reconstruction" "${Reconstruction_HDRS}" "${Reconstruction_SRCS}")

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


 set (ANG_SRCS
  ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngDirectoryPatterns.cpp
 ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngFileReader.cpp
 ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngFileHelper.cpp
 )
 
 set (ANG_HDRS
 ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngDirectoryPatterns.h
 ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngFileReader.h
 ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngFileHelper.h
  )
 IDE_SOURCE_PROPERTIES( "Source/AIM/ANG" "${COMMON_HDRS}" "${COMMON_SRCS}")
 
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
                        ${Boost_PROGRAM_OPTIONS_LIBRARY} )
ToolInstallationSupport(Reconstruction "${EXE_DEBUG_EXTENSION}" ${AIMRepresentation_BINARY_DIR} )
