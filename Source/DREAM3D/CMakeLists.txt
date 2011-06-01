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
PROJECT( DREAM3DLib )
cmake_minimum_required(VERSION 2.8.3)

SET (DREAM3DLib_BUILT_AS_DYNAMIC_LIB)
IF (BUILD_SHARED_LIBS)
    SET (DREAM3DLib_BUILT_AS_DYNAMIC_LIB 1)
    IF (WIN32)
        ADD_DEFINITIONS("-DDREAM3DLib_BUILT_AS_DYNAMIC_LIB")
    ENDIF (WIN32)
ENDIF (BUILD_SHARED_LIBS)

set(Project_SRCS "")
Macro(D3DCompileDir DirName)
    include(${DREAM3DLib_SOURCE_DIR}/${DirName}/SourceList.cmake)
    set (Project_SRCS ${Project_SRCS} ${DREAM3D_${DirName}_SRCS} ${DREAM3D_${DirName}_HDRS})
endMacro()

D3DCompileDir(ANGSupport)
D3DCompileDir(CalculateStats)
D3DCompileDir(Common)
D3DCompileDir(GrainGenerator)
D3DCompileDir(HDF5)
D3DCompileDir(MicrostructureStatistics)
D3DCompileDir(OIMImport)
D3DCompileDir(Parallel)
D3DCompileDir(Reconstruction)
D3DCompileDir(StructureReaders)
D3DCompileDir(SurfaceMesh)
#D3DCompileDir(VolumeMesh)

# --------------------------------------------------------------------
include_directories(${HDF5_INCLUDE_DIR})
include_directories( ${PROJECT_CODE_DIR} )
include_directories( ${PROJECT_CODE_DIR}/ANG ) 
include_directories( ${DREAM3DLib_BINARY_DIR} )


#-- Add a library for the AIMCommon Code
add_library(${PROJECT_NAME} ${LIB_TYPE} ${Project_SRCS} )
target_link_libraries(${PROJECT_NAME} MXADataModel TSLAng ${TBB_LIBRARIES} )
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

  