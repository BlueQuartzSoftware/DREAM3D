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

SET (AIMCOMMON_BUILT_AS_DYNAMIC_LIB)
IF (BUILD_SHARED_LIBS)
    SET (AIMCOMMON_BUILT_AS_DYNAMIC_LIB 1)
    IF (WIN32)
        ADD_DEFINITIONS("-DAIMCOMMON_BUILT_AS_DYNAMIC_LIB")
    ENDIF (WIN32)
ENDIF (BUILD_SHARED_LIBS)


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
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Bin.cpp   
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/AIMRandomNG.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/VTKFileUtils.cpp
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
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Bin.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/AIMRandomNG.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/FeiConstants.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Constants.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/VTKFileUtils.h
)
cmp_IDE_SOURCE_PROPERTIES( "Source/AIM/Common" "${AIMCommon_HDRS}" "${AIMCommon_SRCS}" ${CMP_INSTALL_FILES})


set (ANG_SRCS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngDirectoryPatterns.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngFileHelper.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngReader.cpp
 )
 
set (ANG_HDRS
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngDirectoryPatterns.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngFileHelper.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngReader.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngConstants.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/AngHeaderEntry.h
)
cmp_IDE_SOURCE_PROPERTIES( "Source/AIM/ANG" "${ANG_HDRS}" "${ANG_SRCS}" ${CMP_INSTALL_FILES})

set(HDF5_SRCS "")
set(HDF5_HDRS "")
set(AIM_HDF5_SUPPORT "0")
if (AIM_USE_HDF5)
    set(AIM_HDF5_SUPPORT "1")
    SET (HDF5_SRCS
        ${AIMRepresentation_SOURCE_DIR}/Source/AIM/HDF5/AIMH5DataWriter.cpp)
    SET (HDF5_HDRS
        ${AIMRepresentation_SOURCE_DIR}/Source/AIM/HDF5/AIMH5DataWriter.h
        ${AIMRepresentation_SOURCE_DIR}/Source/AIM/HDF5/VTKH5Constants.h)
    include_directories(${HDF5_INCLUDE_DIR})
    set (AIMCOMMON_DEP_LIBS ${HDF5_LIBRARIES} MXADataModel)
    cmp_IDE_SOURCE_PROPERTIES( "Source/AIM/HDF5" "${HDF5_HDRS}" "${HDF5_SRCS}" ${CMP_INSTALL_FILES})
endif()

set (AIMCommon_SRCS
    ${AIMCommon_SRCS}
    ${AIMCommon_HDRS}
    ${ANG_SRCS}
    ${ANG_HDRS}
    ${HDF5_SRCS}
    ${HDF5_HDRS}
    )

configure_file(${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/AIMCommonConfiguration.h.in
               ${AIMRepresentation_BINARY_DIR}/AIM/Common/AIMCommonConfiguration.h)
               
               
include_directories( ${AIMRepresentation_BINARY_DIR} )
include_directories( ${PROJECT_CODE_DIR} )

#-- Add a library for the AIMCommon Code
add_library(AIMCommon ${LIB_TYPE} ${AIMCommon_SRCS} )
target_link_libraries(AIMCommon ${AIMCOMMON_DEP_LIBS} )
if (AIM_USE_HDF5)
    CMP_COPY_DEPENDENT_LIBRARIES("hdf5")
endif()
LibraryProperties( AIMCommon ${EXE_DEBUG_EXTENSION} )


    