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
PROJECT( DREAM3DTools )
cmake_minimum_required(VERSION 3.1.0)

# --------------------------------------------------------------------
# Setup the install rules for the various platforms
set(install_dir "bin")
if(WIN32)
    set(install_dir ".")
endif()

# --------------------------------------------------------------------------
macro(ConvertPathToHeaderCompatible INPUT)
    if(WIN32)
      STRING(REPLACE "\\" "\\\\" ${INPUT} ${INPUT} )
      STRING(REPLACE "/" "\\\\" ${INPUT} ${INPUT}  )
    endif()
endmacro()


set(DREAM3DProj_SOURCE_DIR_NORM ${DREAM3DProj_SOURCE_DIR})
ConvertPathToHeaderCompatible(${DREAM3DProj_SOURCE_DIR_NORM})
set(SIMPLib_SOURCE_DIR_NORM ${SIMPLib_SOURCE_DIR})
ConvertPathToHeaderCompatible(${SIMPLib_SOURCE_DIR_NORM})
set(SIMPLib_PLUGIN_DIR_NORM "${DREAM3DProj_SOURCE_DIR}/Source/Plugins")
ConvertPathToHeaderCompatible(${SIMPLib_PLUGIN_DIR_NORM})

get_filename_component(DREAM3DProj_PARENT_DIR_NORM ${DREAM3DProj_SOURCE_DIR} PATH)
ConvertPathToHeaderCompatible(${DREAM3DProj_PARENT_DIR_NORM})


configure_file( ${DREAM3DTools_SOURCE_DIR}/ToolConfiguration.h.in
                ${DREAM3DTools_BINARY_DIR}/ToolConfiguration.h)

#-------------------------------------------------------------------------------
# Function COMPILE_TOOL to help alleviate lots of extra code below for adding
# simple command line tools that just need one or two source files
#
function(COMPILE_TOOL)
    set(options)
    set(oneValueArgs TARGET DEBUG_EXTENSION BINARY_DIR COMPONENT INSTALL_DEST DEFINITION)
    set(multiValueArgs SOURCES LINK_LIBRARIES)
    cmake_parse_arguments(D3DTOOL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    if( ${D3DTOOL_DEFINITION} )
        add_definitions(-D${DEFINITION})
    endif()

    BuildToolBundle(
        TARGET ${D3DTOOL_TARGET}
        SOURCES ${D3DTOOL_SOURCES}
        DEBUG_EXTENSION ${D3DTOOL_DEBUG_EXTENSION}
        VERSION_MAJOR ${D3DTOOL_DREAM3D_VER_MAJOR}
        VERSION_MINOR ${D3DTOOL_DREAM3D_VER_MINOR}
        VERSION_PATCH ${D3DTOOL_DREAM3D_VER_PATCH}
        BINARY_DIR    ${D3DTOOL_BINARY_DIR}
        LINK_LIBRARIES Qt5::Core "${D3DTOOL_LINK_LIBRARIES}"
        LIB_SEARCH_DIRS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMPONENT     ${D3DTOOL_COMPONENT}
        INSTALL_DEST  "${D3DTOOL_INSTALL_DEST}"
        SOLUTION_FOLDER "Applications"
    )

endfunction()

# Create a Command line tool for the PipelineRunner
COMPILE_TOOL(
    TARGET PipelineRunner
    SOURCES ${DREAM3DTools_SOURCE_DIR}/PipelineRunner.cpp
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    COMPONENT     Tools
    INSTALL_DEST  "${install_dir}"
    LINK_LIBRARIES EbsdLib SIMPLib
)


# Create a Command line tool for the FilterParameterTool
COMPILE_TOOL(
    TARGET FilterParameterTool
    SOURCES ${DREAM3DTools_SOURCE_DIR}/FilterParameterTool.cpp
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    COMPONENT     Tools
    LINK_LIBRARIES hdf5 Qt5::Core H5Support SIMPLib
)

# Create a Command line tool for the MATLAB_CodeGen
COMPILE_TOOL(
    TARGET MATLAB_CodeGen
    SOURCES ${DREAM3DTools_SOURCE_DIR}/MATLAB_CodeGen.cpp
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    COMPONENT     Tools
    LINK_LIBRARIES EbsdLib SIMPLib
)

# --------------------------------------------------------------------
# Setup an option to compile the version JSON file. This should be off
# by default as no one needs it except the web admin
OPTION (DREAM3D_BUILD_VERSION_FILE "Generate dream3d_version.json for the webserver" OFF)
if(${DREAM3D_BUILD_VERSION_FILE})
  COMPILE_TOOL(
      TARGET CreateWebServerJson
      SOURCES ${DREAM3DTools_SOURCE_DIR}/CreateWebServerJson.cpp
      DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
      VERSION_MAJOR ${DREAM3D_VER_MAJOR}
      VERSION_MINOR ${DREAM3D_VER_MINOR}
      VERSION_PATCH ${DREAM3D_VER_PATCH}
      BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
      LINK_LIBRARIES hdf5 Qt5::Core H5Support SIMPLib
  )
endif()
