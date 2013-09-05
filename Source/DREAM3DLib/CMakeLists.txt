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
cmake_minimum_required(VERSION 2.8.9)

set(DREAM3DLib_BUILT_AS_DYNAMIC_LIB)
if(BUILD_SHARED_LIBS)
    set(DREAM3DLib_BUILT_AS_DYNAMIC_LIB 1)
    if(WIN32)
        ADD_DEFINITIONS("-DDREAM3DLib_BUILT_AS_DYNAMIC_LIB")
    endif(WIN32)
endif(BUILD_SHARED_LIBS)

option(DREAM3D_DISABLE_MSVC_WARNINGS "Disable some MSVC Compiler warnings" OFF)
if(DREAM3D_DISABLE_MSVC_WARNINGS)
    add_definitions(-DDREAM3D_DISABLE_MSVC_WARNINGS)
endif()

set(PROJECT_PREFIX "DREAM3D" CACHE STRING "The Prefix to be used for Preprocessor definitions")
set(PROJECT_INSTALL_HEADERS 0)
#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the
# project.
#-------------------------------------------------------------------------------
set(CMP_HEADER_DIR ${DREAM3DLib_BINARY_DIR})
set(CMP_CONFIGURATION_FILE_NAME "DREAM3DConfiguration.h")
set(CMP_TYPES_FILE_NAME "DREAM3DLibTypes.h")
set(CMP_VERSION_HEADER_FILE_NAME "DREAM3DVersion.h")
set(CMP_VERSION_SOURCE_FILE_NAME "DREAM3DVersion.cpp")
set(CMP_TOP_HEADER_FILE "DREAM3DLib.h")
set(CMP_PROJECT_NAMESPACE "${PROJECT_NAME}")
set(CMP_PROJECT_NAME "${PROJECT_NAME}")
set(CMP_ENABLE_PLUGINS "0")
set(CMP_LIB_SEARCH_DIRS "")
set(CMP_GENERATE_VERSION_STRING 1)

set(DREAM3DLib_FILTER_DOC_DIR ${DREAM3D_DOCUMENTATION_DIR}/ReferenceManual/Filters)

INCLUDE (${CMP_SOURCE_DIR}/cmpProject.cmake)

cmpConfigureFileWithMD5Check(CONFIGURED_TEMPLATE_PATH ${DREAM3DLib_SOURCE_DIR}/DREAM3DConfiguration.h.in
                            GENERATED_FILE_PATH ${DREAM3DProj_BINARY_DIR}/DREAM3DLib/${CMP_TOP_HEADER_FILE})

cmp_IDE_GENERATED_PROPERTIES("Generated"
                             "${CMP_HEADER_DIR}/${CMP_TOP_HEADER_FILE};${CMP_HEADER_DIR}/${CMP_TYPES_FILE_NAME};${CMP_HEADER_DIR}/${CMP_CONFIGURATION_FILE_NAME};${CMP_HEADER_DIR}/${CMP_VERSION_HEADER_FILE_NAME}"
                             "${CMP_HEADER_DIR}/${CMP_VERSION_SOURCE_FILE_NAME}")

set(Project_SRCS
    ${CMP_HEADER_DIR}/${CMP_TOP_HEADER_FILE}
    ${CMP_HEADER_DIR}/${CMP_TYPES_FILE_NAME}
    ${CMP_HEADER_DIR}/${CMP_CONFIGURATION_FILE_NAME}
    ${CMP_HEADER_DIR}/${CMP_VERSION_HEADER_FILE_NAME}
    ${CMP_HEADER_DIR}/${CMP_VERSION_SOURCE_FILE_NAME}
)

#----- Bring in our CMake file with the macros
include(${DREAM3DLib_SOURCE_DIR}/DREAM3DLibMacros.cmake)


#-------------------------------------------------------------------------------
Macro(D3DCompileDir DirName)
    include(${DREAM3DLib_SOURCE_DIR}/${DirName}/SourceList.cmake)
    set(Project_SRCS ${Project_SRCS} ${DREAM3DLib_${DirName}_SRCS} ${DREAM3DLib_${DirName}_HDRS})
endMacro()



# Add in our NON-Filter classes
D3DCompileDir(Common)
D3DCompileDir(HDF5)
D3DCompileDir(Math)
D3DCompileDir(OrientationOps)
D3DCompileDir(DistributionAnalysisOps)
D3DCompileDir(ShapeOps)
D3DCompileDir(VTKUtils)
D3DCompileDir(Utilities)



#-------
# Start writing this header file
set(AllFiltersHeaderFile ${DREAM3DLib_BINARY_DIR}/DREAM3DFilters.h_tmp)
file(WRITE ${AllFiltersHeaderFile} "#ifndef _DREAM3D_FILTERS_H_\n#define _DREAM3D_FILTERS_H_\n\n")
file(APPEND ${AllFiltersHeaderFile} "/* The CMake code that generates this file is located in ${CMAKE_CURRENT_LIST_FILE}*/\n")

# --------------------------------------------------------------------
# Create a File that will be used to generate a program that itself generates a
# QFilterWidget derived header for each of the filters based on thier exposed
# filter options
set(CodeGeneratorFile  ${FilterWidgetsLib_BINARY_DIR}/CodeGenerator.h)
file(WRITE ${CodeGeneratorFile} "")
file(APPEND ${CodeGeneratorFile} "/* The CMake code that generates this file is located in ${CMAKE_CURRENT_LIST_FILE}*/\n")

# --------------------------------------------------------------------
# Create a File that has all the headers for the various Filter Widgets
set(AllFilterWidgetsHeaderFile ${FilterWidgetsLib_BINARY_DIR}/FilterWidgetHeaders.h_tmp)
file(WRITE ${AllFilterWidgetsHeaderFile} "#ifndef _ALLFilterWidgetsLib_H_\n#define _ALLFilterWidgetsLib_H_\n")
file(APPEND ${AllFilterWidgetsHeaderFile} "/* The CMake code that generates this file is located in ${CMAKE_CURRENT_LIST_FILE}*/\n")

# --------------------------------------------------------------------
# Create a File that will register all of the DREAM3D Filters
set(RegisterKnownFiltersFile ${DREAM3DLib_BINARY_DIR}/RegisterKnownFilterWidgets.cpp_tmp)
file(WRITE ${RegisterKnownFiltersFile} "/* This file was AUTO GENERATED During the CMake Run. DO NOT EDIT THIS FILE. Edit the CMake files that generate this file */\n")
file(APPEND ${RegisterKnownFiltersFile} "/* The CMake code that generates this file is located in ${CMAKE_CURRENT_LIST_FILE}*/\n")
file(APPEND ${RegisterKnownFiltersFile} "/* This file will be directly included in another source file. */\n")
#file(APPEND ${RegisterKnownFiltersFile} "#include \"DREAM3DLib/Common/FilterManager.h\"\n")
file(APPEND ${RegisterKnownFiltersFile} "#include \"DREAM3DFilters.h\"\n")
file(APPEND ${RegisterKnownFiltersFile} "\nvoid FilterManager::RegisterKnownFilters(FilterManager* fm)\n{\n")

# --------------------------------------------------------------------
# Create a File that will register all of the DREAM3D Filter Qt based Widgets
set(RegisterKnownFilterWidgetsFile ${FilterWidgetsLib_BINARY_DIR}/RegisterKnownFilterWidgets.cpp_tmp)
file(WRITE ${RegisterKnownFilterWidgetsFile} "/* This file was AUTO GENERATED During the CMake Run. DO NOT EDIT THIS FILE. Edit the CMake files that generate this file */\n")
file(APPEND ${RegisterKnownFilterWidgetsFile} "/* The CMake code that generates this file is located in ${CMAKE_CURRENT_LIST_FILE}*/\n")
file(APPEND ${RegisterKnownFilterWidgetsFile} "#include \"PipelineBuilder/FilterWidgetManager.h\"\n")
file(APPEND ${RegisterKnownFilterWidgetsFile} "#include \"QFilterWidgetFactory.hpp\"\n")
file(APPEND ${RegisterKnownFilterWidgetsFile} "#include \"FilterWidgetHeaders.h\"\n")
file(APPEND ${RegisterKnownFilterWidgetsFile} "\nvoid FilterWidgetsLib::RegisterKnownQFilterWidgets()\n{\n")

# --------------------------------------------------------------------
# Generate a file with all of our MOC'ed sources and  headers
set(FilterWidget_GEN_HDRS_File ${FilterWidgetsLib_BINARY_DIR}/FilterWidget_GEN_HDRS_File.cmake)
set(FilterWidget_GEN_SRCS_File ${FilterWidgetsLib_BINARY_DIR}/FilterWidget_GEN_SRCS.cmake)
file(WRITE ${FilterWidget_GEN_HDRS_File})
file(WRITE ${FilterWidget_GEN_SRCS_File})



# Add in our Filter classes
#include(${DREAM3DLib_SOURCE_DIR}/CustomFilters/SourceList.cmake)
include(${DREAM3DLib_SOURCE_DIR}/GenericFilters/SourceList.cmake)
include(${DREAM3DLib_SOURCE_DIR}/IOFilters/SourceList.cmake)
include(${DREAM3DLib_SOURCE_DIR}/ProcessingFilters/SourceList.cmake)
include(${DREAM3DLib_SOURCE_DIR}/ReconstructionFilters/SourceList.cmake)
include(${DREAM3DLib_SOURCE_DIR}/SamplingFilters/SourceList.cmake)
include(${DREAM3DLib_SOURCE_DIR}/StatisticsFilters/SourceList.cmake)
include(${DREAM3DLib_SOURCE_DIR}/SurfaceMeshingFilters/SourceList.cmake)
include(${DREAM3DLib_SOURCE_DIR}/SyntheticBuildingFilters/SourceList.cmake)

#-----------------------------------------------------
# Close up all the files that we just generated
file(APPEND ${AllFiltersHeaderFile} "\n#endif\n")
file(APPEND ${AllFilterWidgetsHeaderFile} "\n#endif\n")
file(APPEND ${RegisterKnownFilterWidgetsFile} "\n }\n")
file(APPEND ${RegisterKnownFiltersFile} "\n }\n")


cmpReplaceFileIfDifferent(OLD_FILE_PATH  ${DREAM3DLib_BINARY_DIR}/DREAM3DFilters.h
                          NEW_FILE_PATH  ${AllFiltersHeaderFile} )
cmpReplaceFileIfDifferent(OLD_FILE_PATH  ${FilterWidgetsLib_BINARY_DIR}/RegisterKnownFilterWidgets.cpp
                          NEW_FILE_PATH  ${RegisterKnownFilterWidgetsFile} )
cmpReplaceFileIfDifferent(OLD_FILE_PATH  ${DREAM3DLib_BINARY_DIR}/RegisterKnownFilters.cpp
                          NEW_FILE_PATH  ${RegisterKnownFiltersFile} )
cmpReplaceFileIfDifferent(OLD_FILE_PATH  ${FilterWidgetsLib_BINARY_DIR}/FilterWidgetHeaders.h
                          NEW_FILE_PATH  ${AllFilterWidgetsHeaderFile} )

#--------------------------
include_directories( ${HDF5_INCLUDE_DIR} )
include_directories( ${PROJECT_CODE_DIR} )
include_directories( ${DREAM3DLib_BINARY_DIR} )

#-- Add a library for the DREAM3DLib Code
add_library(${PROJECT_NAME} ${LIB_TYPE} ${Project_SRCS} )
target_link_libraries(${PROJECT_NAME} ${QT_QTCORE_LIBRARY} EbsdLib ${TBB_LIBRARIES})
LibraryProperties( ${PROJECT_NAME} ${EXE_DEBUG_EXTENSION} )

set(install_dir "tools")
set(lib_install_dir "lib")
if(WIN32)
    set(install_dir ".")
    set(lib_install_dir "")
endif()


if(BUILD_SHARED_LIBS  AND NOT APPLE)
    INSTALL(TARGETS ${PROJECT_NAME}
        COMPONENT Applications
        RUNTIME DESTINATION ${install_dir}
        LIBRARY DESTINATION ${lib_install_dir}
        ARCHIVE DESTINATION ${lib_install_dir}
        BUNDLE DESTINATION "."
    )
endif()
