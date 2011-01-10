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
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/AIMRandomNG.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Quaternions.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/VTKFileUtils.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/MisorientationCalculations.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/HDF5/AIM_H5VtkDataWriter.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Reconstruction/H5ReconStatsWriter.cpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Reconstruction/H5ReconStatsReader.cpp
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
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/AIMRandomNG.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/FeiConstants.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Constants.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/VTKFileUtils.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/Quaternions.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/MisorientationCalculations.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/OIMColoring.hpp
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/HDF5/AIM_H5VtkDataWriter.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/HDF5/VTKH5Constants.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Reconstruction/H5ReconStatsWriter.h
    ${AIMRepresentation_SOURCE_DIR}/Source/AIM/Reconstruction/H5ReconStatsReader.h
)

cmp_IDE_SOURCE_PROPERTIES( "AIM/Common" "${AIMCommon_HDRS}" "${AIMCommon_SRCS}" ${PROJECT_INSTALL_HEADERS})

# --------------------------------------------------------------------
include_directories(${HDF5_INCLUDE_DIR})

# --------------------------------------------------------------------
include (${AIMRepresentation_SOURCE_DIR}/Source/AIM/ANG/Sources.cmake)


set (AIMCommon_SRCS
    ${AIMCommon_SRCS}
    ${AIMCommon_HDRS}
    ${ANG_SRCS}
    ${ANG_HDRS}
    )

configure_file(${AIMRepresentation_SOURCE_DIR}/Source/AIM/Common/AIMCommonConfiguration.h.in
               ${AIMRepresentation_BINARY_DIR}/AIM/Common/AIMCommonConfiguration.h)
               
               
include_directories( ${AIMRepresentation_BINARY_DIR} )
include_directories( ${PROJECT_CODE_DIR} )

#-- Add a library for the AIMCommon Code
add_library(${PROJECT_NAME} ${LIB_TYPE} ${AIMCommon_SRCS} )
target_link_libraries(${PROJECT_NAME} MXADataModel ${HDF5_LIBRARIES} )
LibraryProperties( ${PROJECT_NAME} ${EXE_DEBUG_EXTENSION} )

set(install_dir "tools")
set(lib_install_dir "lib")
if (WIN32)
    set (install_dir ".")
    set(lib_install_dir "")
endif()

if (BUILD_SHARED_LIBS)
    INSTALL(TARGETS ${PROJECT_NAME} 
        COMPONENT Applications
        RUNTIME DESTINATION ${install_dir}
        LIBRARY DESTINATION ${lib_install_dir}
        ARCHIVE DESTINATION ${lib_install_dir}       
        BUNDLE DESTINATION "."
    ) 
endif() 
    