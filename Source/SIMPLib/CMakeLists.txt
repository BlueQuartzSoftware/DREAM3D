# ============================================================================
# Copyright (c) 2009-2015 BlueQuartz Software, LLC
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
# contributors may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The code contained herein was partially funded by the followig contracts:
#    United States Air Force Prime Contract FA8650-07-D-5800
#    United States Air Force Prime Contract FA8650-10-D-5210
#    United States Prime Contract Navy N00173-07-C-2068
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

# set project's name
PROJECT( SIMPLib VERSION ${DREAM3DProj_VERSION_MAJOR}.${DREAM3DProj_VERSION_MINOR})


set(SIMPLib_BUILT_AS_DYNAMIC_LIB)
if(BUILD_SHARED_LIBS)
    set(SIMPLib_BUILT_AS_DYNAMIC_LIB 1)
    if(WIN32)
        ADD_DEFINITIONS("-DSIMPLib_BUILT_AS_DYNAMIC_LIB")
    endif(WIN32)
endif(BUILD_SHARED_LIBS)


set(PROJECT_PREFIX "SIMPLib" CACHE STRING "The Prefix to be used for Preprocessor definitions")
set(PROJECT_INSTALL_HEADERS 0)
#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the
# project.
#-------------------------------------------------------------------------------
set(CMP_HEADER_DIR ${SIMPLib_BINARY_DIR})
set(CMP_CONFIGURATION_FILE_NAME "SIMPLibConfiguration.h")
set(CMP_TYPES_FILE_NAME "SIMPLibTypes.h")
set(CMP_VERSION_HEADER_FILE_NAME "SIMPLibVersion.h")
set(CMP_VERSION_SOURCE_FILE_NAME "SIMPLibVersion.cpp")
set(CMP_TOP_HEADER_FILE "SIMPLib.h")
set(CMP_PROJECT_NAMESPACE "${PROJECT_NAME}")
set(CMP_PROJECT_NAME "${PROJECT_NAME}")
set(CMP_ENABLE_PLUGINS "0")
set(CMP_LIB_SEARCH_DIRS "")
set(CMP_GENERATE_VERSION_STRING 1)

set(SIMPLib_FILTER_DOC_DIR ${SIMPLib_SOURCE_DIR}/Documentation/SIMPLibFilters)

include(${CMP_SOURCE_DIR}/cmpProject.cmake)

cmpConfigureFileWithMD5Check(CONFIGURED_TEMPLATE_PATH ${SIMPLib_SOURCE_DIR}/SIMPLibConfiguration.h.in
                            GENERATED_FILE_PATH ${DREAM3DProj_BINARY_DIR}/SIMPLib/${CMP_TOP_HEADER_FILE})

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
include(${SIMPLib_SOURCE_DIR}/SIMPLibMacros.cmake)


#-------------------------------------------------------------------------------
macro(D3DCompileDir DirName)
    set(SIMPLib_${DirName}_Generated_MOC_SRCS "")
    include(${SIMPLib_SOURCE_DIR}/${DirName}/SourceList.cmake)

    foreach(f ${SIMPLib_${DirName}_Generated_MOC_SRCS})
        set_source_files_properties( ${f} PROPERTIES GENERATED TRUE)
        set_source_files_properties( ${f} PROPERTIES HEADER_FILE_ONLY TRUE)
    endforeach()
    set(Project_SRCS ${Project_SRCS} ${SIMPLib_${DirName}_SRCS} ${SIMPLib_${DirName}_HDRS} ${SIMPLib_${SUBDIR_NAME}_Generated_MOC_SRCS})
endMacro()

# Add in our NON-Filter classes
D3DCompileDir(Common)
D3DCompileDir(DataArrays)
D3DCompileDir(DataContainers)
D3DCompileDir(Geometry)
D3DCompileDir(FilterParameters)
D3DCompileDir(HDF5)
D3DCompileDir(Math)
D3DCompileDir(Plugin)
D3DCompileDir(StatsData)
D3DCompileDir(Utilities)
D3DCompileDir(VTKUtils)


#-------
# Start writing this header file
set(AllFiltersHeaderFile ${SIMPLib_BINARY_DIR}/SIMPLibFilters.h_tmp)
file(WRITE ${AllFiltersHeaderFile} "#ifndef _SIMPLib_FILTERS_H_\n#define _SIMPLib_FILTERS_H_\n\n")
file(APPEND ${AllFiltersHeaderFile} "/* The CMake code that generates this file is located in ${CMAKE_CURRENT_LIST_FILE}*/\n")


# --------------------------------------------------------------------
# Create a File that will register all of the SIMPLib Filters
set(RegisterKnownFiltersFile ${SIMPLib_BINARY_DIR}/RegisterKnownFilters.cpp_tmp)
file(WRITE ${RegisterKnownFiltersFile} "/* This file was AUTO GENERATED During the CMake Run. DO NOT EDIT THIS FILE. Edit the CMake files that generate this file */\n")
file(APPEND ${RegisterKnownFiltersFile} "/* The CMake code that generates this file is located in ${CMAKE_CURRENT_LIST_FILE}*/\n")
file(APPEND ${RegisterKnownFiltersFile} "/* This file will be directly included in another source file. */\n")
#file(APPEND ${RegisterKnownFiltersFile} "#include \"SIMPLib/Common/FilterManager.h\"\n")
file(APPEND ${RegisterKnownFiltersFile} "#include \"SIMPLibFilters.h\"\n")
file(APPEND ${RegisterKnownFiltersFile} "\nvoid FilterManager::RegisterKnownFilters(FilterManager* fm)\n{\n")


# Add in our Filter classes
# set(PluginAutoMocSourceFile ${${PROJECT_NAME}_BINARY_DIR}/CoreFilters_automoc_cmake.cpp)
# set_source_files_properties( ${PluginAutoMocSourceFile} PROPERTIES GENERATED TRUE)
# Add the generated moc file to the project sources
# set(Project_SRCS ${Project_SRCS} ${PluginAutoMocSourceFile} )
include(${SIMPLib_SOURCE_DIR}/CoreFilters/SourceList.cmake)

#-----------------------------------------------------
# conditionally compile our example codes- These should be turned off for a release
option(SIMPLib_BUILD_TEST_FILTERS "Build the Test filters: These should be OFF when doing a release" ON)
if(SIMPLib_BUILD_TEST_FILTERS)
    # set(PluginAutoMocSourceFile ${${PROJECT_NAME}_BINARY_DIR}/TestFilters_automoc_cmake.cpp)
    # set_source_files_properties( ${PluginAutoMocSourceFile} PROPERTIES GENERATED TRUE)  
    # Add the generated moc file to the project sources
    set(Project_SRCS ${Project_SRCS} ${PluginAutoMocSourceFile} )
    include(${SIMPLib_SOURCE_DIR}/TestFilters/SourceList.cmake)
endif()

#-----------------------------------------------------
# Close up all the files that we just generated
file(APPEND ${AllFiltersHeaderFile} "\n#endif\n")
file(APPEND ${RegisterKnownFiltersFile} "\n }\n")


cmpReplaceFileIfDifferent(OLD_FILE_PATH  ${SIMPLib_BINARY_DIR}/SIMPLibFilters.h
                          NEW_FILE_PATH  ${AllFiltersHeaderFile} )
cmpReplaceFileIfDifferent(OLD_FILE_PATH  ${SIMPLib_BINARY_DIR}/RegisterKnownFilters.cpp
                          NEW_FILE_PATH  ${RegisterKnownFiltersFile} )

#--------------------------
include_directories( ${HDF5_INCLUDE_DIR} )
include_directories( ${PROJECT_CODE_DIR} )
include_directories( ${SIMPLib_BINARY_DIR} )

# include( ${DREAM3D_SUPPORT_DIR}/ITKSupport/IncludeITK.cmake)
# include_directories(${ITK_INCLUDE_DIRS})


#-- Add a library for the SIMPLib Code
add_library(${PROJECT_NAME} ${LIB_TYPE} ${Project_SRCS} )
target_link_libraries(${PROJECT_NAME} Qt5::Core H5Support ${TBB_LIBRARIES})
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
