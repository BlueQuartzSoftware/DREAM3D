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
PROJECT( AIMCommon )
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



SET (AIMCommon_SRCS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/GrainGeneratorFunc.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/SurfaceMeshFunc.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/ReconstructionFunc.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Grain.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Voxel.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Patch.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Face.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Node.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Neighbor.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Segment.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/ISegment.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Bin.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Orient.cpp   
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/AIMRandomNG.cpp
    )

SET (AIMCommon_HDRS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/GrainGeneratorFunc.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/SurfaceMeshFunc.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/ReconstructionFunc.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Grain.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Voxel.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Patch.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Face.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Node.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Neighbor.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Segment.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/ISegment.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Bin.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Orient.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/AIMRandomNG.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/FeiConstants.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Constants.h
)
IDE_SOURCE_PROPERTIES( "Source/AIM/Common" "${AIMCommon_HDRS}" "${AIMCommon_SRCS}")


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
IDE_SOURCE_PROPERTIES( "Source/AIM/ANG" "${ANG_HDRS}" "${ANG_SRCS}")

set (AIMCommon_SRCS
    ${AIMCommon_SRCS}
    ${AIMCommon_HDRS}
    ${ANG_SRCS}
    ${ANG_HDRS}
    )

    
include_directories( ${AIMRepresentation_BINARY_DIR} )
#-- Add a static library for the PCM registration
add_library(AIMCommon STATIC ${AIMCommon_SRCS} )
StaticLibraryProperties( AIMCommon )


    