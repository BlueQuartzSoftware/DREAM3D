#///////////////////////////////////////////////////////////////////////////////
#  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#///////////////////////////////////////////////////////////////////////////////
project(MXADataModel)

set (MXA_SOURCE_DIR ${PROJECT_CODE_DIR} )
set (Project_SRCS "")
set (Project_HDRS "")

SET (MXA_BUILT_AS_DYNAMIC_LIB)
IF (BUILD_SHARED_LIBS)
    SET (MXA_BUILT_AS_DYNAMIC_LIB 1)
    IF (WIN32)
        ADD_DEFINITIONS("-DMXA_BUILT_AS_DYNAMIC_LIB")
    ENDIF (WIN32)
ENDIF (BUILD_SHARED_LIBS)


Macro(MXACompileDir DirName)
    include(${MXADataModel_SOURCE_DIR}/${DirName}/SourceList.cmake)
    set (Project_SRCS ${Project_SRCS} ${MXA_${DirName}_SRCS} ${MXA_${DirName}_HDRS})
endMacro()

MXACompileDir(Common)
MXACompileDir(Utilities)
set (MXA_HDF5_SUPPORT "1")
MXACompileDir(HDF5)

set (Project_SRCS 
    ${Project_SRCS} 
    )

include_directories(${MXADataModel_SOURCE_DIR}/..)
include_directories(${MXADataModel_BINARY_DIR})
include_directories(${MXADataModel_BINARY_DIR}/..)


set (MXA_TIFF_SUPPORT "0")
set (MXA_XML_SUPPORT "0")
set (HDF5_VTK_BRIDGE "0")

# --------------------------------------------------------------------
# Generate a Header file with Compile Version variables
# --------------------------------------------------------------------
set (VERSION_GEN_NAME "MXA")
set (VERSION_GEN_NAMESPACE "MXA")
set (PROJECT_PREFIX "MXA")
set (VERSION_GEN_COMPLETE  ${DREAM3D_VERSION})
set (VERSION_GEN_VER_MAJOR ${DREAM3D_VER_MAJOR})
set (VERSION_GEN_VER_MINOR ${DREAM3D_VER_MINOR})
set (VERSION_GEN_VER_PATCH ${DREAM3D_VER_PATCH})

configure_file(${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpVersion.h.in   
               ${CMP_HEADER_DIR}/MXAVersion.h  )

configure_file(${PROJECT_RESOURCES_DIR}/MXACMake/MXAConfiguration.h.in
               ${CMP_HEADER_DIR}/${CMP_EXTRA_CONFIGURATION_FILE})
               
# add_definitions(-DQT_CORE_LIB)

add_library(${PROJECT_NAME} ${LIB_TYPE}  ${Project_SRCS})
target_link_libraries(${PROJECT_NAME} ${HDF5_LIBRARIES} )
LibraryProperties( ${PROJECT_NAME} ${EXE_DEBUG_EXTENSION})

set(install_dir "tools")
set(lib_install_dir "lib")
if (WIN32)
    set (install_dir ".")
    set(lib_install_dir "")
endif()

if (BUILD_SHARED_LIBS AND DREAM3D_BUILD_CLI_TOOLS)
    INSTALL(TARGETS ${PROJECT_NAME} 
        COMPONENT Applications
        RUNTIME DESTINATION ${install_dir}
        LIBRARY DESTINATION ${lib_install_dir}
        ARCHIVE DESTINATION ${lib_install_dir}       
        BUNDLE DESTINATION "."
    ) 
endif() 
