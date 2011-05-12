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
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/GrainGeneratorFunc.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/GrainGeneratorVTKWriter.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/SurfaceMeshFunc.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/ReconstructionFunc.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/ReconstructionVTKWriter.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/Grain.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/Voxel.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/Patch.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/Face.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/Node.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/Neighbor.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/Segment.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/AIMRandomNG.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/OrientationMath.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/Texture.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/TexturePreset.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/ANG/AngDataLoader.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/ANG/H5AngDataLoader.cpp
    )

SET (AIMCommon_HDRS
    ${DREAM3D_SOURCE_DIR}/Source/AIM/ANG/AbstractAngDataLoader.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/ANG/AngDataLoader.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/ANG/H5AngDataLoader.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/GrainGeneratorFunc.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/GrainGeneratorVTKWriter.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/VTKWriterMacros.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/CrystalStructure.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/SurfaceMeshFunc.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/ReconstructionFunc.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/ReconstructionVTKWriter.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/Grain.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/Voxel.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/Patch.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/Face.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/Node.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/Neighbor.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/Segment.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/AIMRandomNG.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/Constants.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/OrientationMath.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/OIMColoring.hpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/Texture.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/TexturePreset.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/AIMMath.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/AIMArray.hpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/AIMDLLExport.h
    )
cmp_IDE_SOURCE_PROPERTIES( "AIM/Common" "${AIMCommon_HDRS}" "${AIMCommon_SRCS}" ${PROJECT_INSTALL_HEADERS})

    
set (AIMCommon_HDF5_SRCS
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/HDF5/AIM_H5VtkDataWriter.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/HDF5/H5ReconStatsWriter.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/HDF5/H5ReconStatsReader.cpp
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/HDF5/H5GrainWriter.cpp
    )
set (AIMCommon_HDF5_HDRS
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/HDF5/AIM_H5VtkDataWriter.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/HDF5/VTKH5Constants.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/HDF5/H5ReconStatsWriter.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/HDF5/H5ReconStatsReader.h
    ${DREAM3D_SOURCE_DIR}/Source/AIM/Common/HDF5/H5GrainWriter.h
)
cmp_IDE_SOURCE_PROPERTIES( "AIM/Common/HDF5" "${AIMCommon_HDF5_HDRS}" "${AIMCommon_HDF5_SRCS}" ${PROJECT_INSTALL_HEADERS})

set (AIMCommon_OrientatioOps_SRCS
${DREAM3D_SOURCE_DIR}/Source/AIM/Common/OrientationOps/CubicOps.cpp
${DREAM3D_SOURCE_DIR}/Source/AIM/Common/OrientationOps/HexagonalOps.cpp
${DREAM3D_SOURCE_DIR}/Source/AIM/Common/OrientationOps/OrthoRhombicOps.cpp
)
set (AIMCommon_OrientatioOps_HDRS
${DREAM3D_SOURCE_DIR}/Source/AIM/Common/OrientationOps/CubicOps.h
${DREAM3D_SOURCE_DIR}/Source/AIM/Common/OrientationOps/HexagonalOps.h
${DREAM3D_SOURCE_DIR}/Source/AIM/Common/OrientationOps/OrthoRhombicOps.h
)
cmp_IDE_SOURCE_PROPERTIES( "AIM/Common/OrientationOps" "${AIMCommon_OrientatioOps_HDRS}" "${AIMCommon_OrientatioOps_SRCS}" ${PROJECT_INSTALL_HEADERS})

# --------------------------------------------------------------------
include_directories(${HDF5_INCLUDE_DIR})

set (AIMCommon_SRCS
    ${AIMCommon_SRCS}
    ${AIMCommon_HDRS}
    ${AIMCommon_HDF5_SRCS}
    ${AIMCommon_HDF5_HDRS}
    ${AIMCommon_OrientatioOps_SRCS}
    ${AIMCommon_OrientatioOps_HDRS}
    )

configure_file(${DREAM3D_SOURCE_DIR}/Source/AIM/Common/AIMCommonConfiguration.h.in
               ${DREAM3D_BINARY_DIR}/AIM/Common/AIMCommonConfiguration.h)
               
               
include_directories( ${DREAM3D_BINARY_DIR} )
include_directories( ${PROJECT_CODE_DIR} )
include_directories( ${PROJECT_CODE_DIR}/ANG ) 

#-- Add a library for the AIMCommon Code
add_library(${PROJECT_NAME} ${LIB_TYPE} ${AIMCommon_SRCS} )
target_link_libraries(${PROJECT_NAME} MXADataModel ${HDF5_LIBRARIES} TSLAng ${TBB_LIBRARIES} )
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

  