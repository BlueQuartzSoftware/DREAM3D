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

configure_file(${PROJECT_RESOURCES_DIR}/MXACMake/MXAConfiguration.h.in
               ${CMP_HEADER_DIR}/${CMP_EXTRA_CONFIGURATION_FILE})
               
# add_definitions(-DQT_CORE_LIB)

# - Check to see if any other project is redefining the library names
IF ( NOT DEFINED  MXADATAMODEL_LIB_NAME)
  SET (MXADATAMODEL_LIB_NAME ${PROJECT_NAME})
ENDIF ( NOT DEFINED MXADATAMODEL_LIB_NAME )

add_library(${PROJECT_NAME} ${LIB_TYPE}  ${Project_SRCS})
target_link_libraries(${PROJECT_NAME} ${HDF5_LIBRARIES} )
LibraryProperties( ${PROJECT_NAME} ${EXE_DEBUG_EXTENSION})
if (BUILD_SHARED_LIBS)
    INSTALL(TARGETS ${PROJECT_NAME} 
        COMPONENT Applications
        RUNTIME DESTINATION ./
        LIBRARY DESTINATION ./ 
        ARCHIVE DESTINATION ./        
        BUNDLE DESTINATION ./
    ) 
endif() 
