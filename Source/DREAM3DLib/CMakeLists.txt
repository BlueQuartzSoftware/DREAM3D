#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
#--  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#--////////////////////////////////////////////////////////////////////////////

#///////////////////////////////////////////////////////////////////////////////
#// This code was partly written under US Air Force Contract FA8650-07-D-5800
#///////////////////////////////////////////////////////////////////////////////

# set project's name
PROJECT( DREAM3DLib )
cmake_minimum_required(VERSION 2.8.4)

SET (DREAM3DLib_BUILT_AS_DYNAMIC_LIB)
IF (BUILD_SHARED_LIBS)
    SET (DREAM3DLib_BUILT_AS_DYNAMIC_LIB 1)
    IF (WIN32)
        ADD_DEFINITIONS("-DDREAM3DLib_BUILT_AS_DYNAMIC_LIB")
    ENDIF (WIN32)
ENDIF (BUILD_SHARED_LIBS)

SET (PROJECT_PREFIX "DREAM3D" CACHE STRING "The Prefix to be used for Preprocessor definitions")
set (PROJECT_INSTALL_HEADERS 0)
#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the 
# project.
#------------------------------------------------------------------------------- 
set (CMP_HEADER_DIR ${DREAM3DLib_BINARY_DIR})
set (CMP_CONFIGURATION_FILE_NAME "DREAM3DConfiguration.h")
set (CMP_TYPES_FILE_NAME "DREAM3DLibTypes.h")
set (CMP_VERSION_HEADER_FILE_NAME "DREAM3DVersion.h")
set (CMP_TOP_HEADER_FILE "DREAM3DLib.h")
set (CMP_PROJECT_NAMESPACE "${PROJECT_NAME}")
set (CMP_PROJECT_NAME "${PROJECT_NAME}")
set (CMP_ENABLE_PLUGINS "0")
set (CMP_LIB_SEARCH_DIRS "")
set (CMP_GENERATE_VERSION_STRING 1)


INCLUDE (${CMP_SOURCE_DIR}/cmpProject.cmake)
configure_file(${DREAM3DLib_SOURCE_DIR}/DREAM3DConfiguration.h.in
               ${DREAM3DProj_BINARY_DIR}/DREAM3DLib/${CMP_TOP_HEADER_FILE})


set(Project_SRCS "")
Macro(D3DCompileDir DirName)
    include(${DREAM3DLib_SOURCE_DIR}/${DirName}/SourceList.cmake)
    set (Project_SRCS ${Project_SRCS} ${DREAM3DLib_${DirName}_SRCS} ${DREAM3DLib_${DirName}_HDRS})
endMacro()


D3DCompileDir(Common)
D3DCompileDir(HDF5)
D3DCompileDir(IO)
D3DCompileDir(OrientationOps)
D3DCompileDir(ShapeOps)
D3DCompileDir(VTKUtils)
D3DCompileDir(GenericFilters)
D3DCompileDir(ManipulationFilters)
D3DCompileDir(StatisticsFilters)
D3DCompileDir(ReconstructionFilters)
D3DCompileDir(SyntheticBuilderFilters)
D3DCompileDir(SurfaceMeshingFilters)

#message(STATUS "------------------------------------")
#foreach(file ${Project_SRCS})
#  message(STATUS "${file}")
#endforeach(file ${Project_SRCS})
#message(STATUS "------------------------------------")



#--------------------------
include_directories( ${HDF5_INCLUDE_DIR} )
include_directories( ${PROJECT_CODE_DIR} )
include_directories( ${DREAM3DLib_BINARY_DIR} )


#-- Add a library for the AIMCommon Code
add_library(${PROJECT_NAME} ${LIB_TYPE} ${Project_SRCS} )
target_link_libraries(${PROJECT_NAME} MXA EbsdLib ${TBB_LIBRARIES} )
LibraryProperties( ${PROJECT_NAME} ${EXE_DEBUG_EXTENSION} )

set(install_dir "tools")
set(lib_install_dir "lib")
if (WIN32)
    set (install_dir ".")
    set(lib_install_dir "")
endif()


if (BUILD_SHARED_LIBS  AND NOT APPLE)
    INSTALL(TARGETS ${PROJECT_NAME} 
        COMPONENT Applications
        RUNTIME DESTINATION ${install_dir}
        LIBRARY DESTINATION ${lib_install_dir}
        ARCHIVE DESTINATION ${lib_install_dir}       
        BUNDLE DESTINATION "."
    ) 
endif() 

  