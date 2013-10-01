#///////////////////////////////////////////////////////////////////////////////
#  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#///////////////////////////////////////////////////////////////////////////////
project(MXA)
set(MXA_BUILT_AS_DYNAMIC_LIB)
if(BUILD_SHARED_LIBS)
    set(MXA_BUILT_AS_DYNAMIC_LIB 1)
    if(WIN32)
        ADD_DEFINITIONS("-DMXA_BUILT_AS_DYNAMIC_LIB")
    endif(WIN32)
endif(BUILD_SHARED_LIBS)

set(PROJECT_CODE_DIR ${PROJECT_SOURCE_DIR}/Source)
if(NOT PROJECT_RESOURCES_DIR)
set(PROJECT_RESOURCES_DIR ${MXA_SOURCE_DIR}/Resources)
endif()
if(NOT DEFINED CMP_SOURCE_DIR)
set(CMP_SOURCE_DIR ${MXA_SOURCE_DIR}/Support/cmp)
endif()

set(PROJECT_PREFIX "MXA" CACHE STRING "The Prefix to be used for Preprocessor definitions")
set(MXA_INSTALL_FILES 0)
#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the 
# project.
#------------------------------------------------------------------------------- 
set(CMP_HEADER_DIR ${MXA_BINARY_DIR})
set(CMP_CONFIGURATION_FILE_NAME "MXAConfiguration.h")
set(CMP_TYPES_FILE_NAME "MXATypes.h")
set(CMP_VERSION_HEADER_FILE_NAME "MXAVersion.h")
set(CMP_VERSION_SOURCE_FILE_NAME "MXAVersion.cpp")
set(CMP_TOP_HEADER_FILE "MXA.h")
set(CMP_PROJECT_NAMESPACE "MXA")
set(CMP_PROJECT_NAME "MXA")
set(CMP_ENABLE_PLUGINS "0")
set(CMP_LIB_SEARCH_DIRS "")
INCLUDE (${CMP_SOURCE_DIR}/cmpProject.cmake)


Macro(MXACompileDir DirName)
    include(${MXA_SOURCE_DIR}/${DirName}/SourceList.cmake)
    set(Project_SRCS ${Project_SRCS} ${MXA_${DirName}_SRCS} ${MXA_${DirName}_HDRS})
endMacro()

MXACompileDir(Common)
MXACompileDir(Utilities)
set(MXA_HDF5_SUPPORT "1")
#MXACompileDir(HDF5)


include_directories(${MXA_SOURCE_DIR}/..)
include_directories(${MXA_BINARY_DIR})
include_directories(${MXA_BINARY_DIR}/..)


set(MXA_TIFF_SUPPORT "0")
set(MXA_XML_SUPPORT "0")
set(HDF5_VTK_BRIDGE "0")

# --------------------------------------------------------------------
# Generate a Header file with Compile Version variables
# --------------------------------------------------------------------
cmpVersionStringsFromGit(GENERATED_HEADER_FILE_PATH ${CMP_HEADER_DIR}/MXAVersion.h 
                         GENERATED_SOURCE_FILE_PATH ${CMP_HEADER_DIR}/MXAVersion.cpp
                         NAMESPACE "MXA"
                         cmpProjectName "MXA" )
                         

configure_file(${MXA_SOURCE_DIR}/MXAConfiguration.h.in
               ${CMP_HEADER_DIR}/${CMP_TOP_HEADER_FILE})

cmp_IDE_GENERATED_PROPERTIES("Generated" 
                             "${CMP_HEADER_DIR}/${CMP_TOP_HEADER_FILE};${CMP_HEADER_DIR}/${CMP_TYPES_FILE_NAME};${CMP_HEADER_DIR}/${CMP_CONFIGURATION_FILE_NAME};${CMP_HEADER_DIR}/MXAVersion.h"
                             "${CMP_HEADER_DIR}/MXAVersion.cpp")


            
set(Project_SRCS 
    ${Project_SRCS} 
    ${CMP_HEADER_DIR}/${CMP_TOP_HEADER_FILE}
    ${CMP_HEADER_DIR}/${CMP_TYPES_FILE_NAME}
    ${CMP_HEADER_DIR}/${CMP_CONFIGURATION_FILE_NAME}
    ${CMP_HEADER_DIR}/MXAVersion.h
    ${CMP_HEADER_DIR}/MXAVersion.cpp
)



add_library(${PROJECT_NAME} ${LIB_TYPE}  ${Project_SRCS})
target_link_libraries(${PROJECT_NAME} ${HDF5_LIBRARIES} MXA)
LibraryProperties( ${PROJECT_NAME} ${EXE_DEBUG_EXTENSION})

set(install_dir "tools")
set(lib_install_dir "lib")
if(WIN32)
    set(install_dir ".")
    set(lib_install_dir "")
endif()

if(BUILD_SHARED_LIBS AND NOT APPLE)
    INSTALL(TARGETS ${PROJECT_NAME} 
        COMPONENT Applications
        RUNTIME DESTINATION ${install_dir}
        LIBRARY DESTINATION ${lib_install_dir}
        ARCHIVE DESTINATION ${lib_install_dir}       
        BUNDLE DESTINATION "."
    ) 
endif() 
