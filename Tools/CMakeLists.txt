PROJECT( DREAM3DTools )
cmake_minimum_required(VERSION 3.8.0)

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

if(APPLE)
  set(HTML_OUTPUT_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${DREAM3D_PACKAGE_DEST_PREFIX}Resources/Help/DREAM3D")
else()
  set(HTML_OUTPUT_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Help/DREAM3D")
endif()


configure_file( ${DREAM3DTools_SOURCE_DIR}/ToolConfiguration.h.in
                ${DREAM3DTools_BINARY_DIR}/${PROJECT_NAME}Configuration.h)

# ---------- Find HDF5 Headers/Libraries -----------------------
# HDF5 now comes with everything that is needed for CMake to load
# up the targets (Exported) that it needs. We just need to find where
# HDF5 is installed.
include(${CMP_SOURCE_DIR}/ExtLib/HDF5Support.cmake)

#-------------------------------------------------------------------------------
# Function COMPILE_TOOL to help alleviate lots of extra code below for adding
# simple command line tools that just need one or two source files
#
function(COMPILE_TOOL)
    set(options)
    set(oneValueArgs TARGET DEBUG_EXTENSION BINARY_DIR COMPONENT INSTALL_DEST DEFINITION)
    set(multiValueArgs SOURCES LINK_LIBRARIES)
    cmake_parse_arguments(D3DTOOL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

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
        SOLUTION_FOLDER "Tools"
    )
    target_include_directories(${D3DTOOL_TARGET}
                                PUBLIC
                                  ${SIMPLProj_SOURCE_DIR}/Source
                                  ${SIMPLProj_BINARY_DIR}
                                  # ${SIMPLViewProj_BINARY_DIR}
                                  ${SIMPLViewProj_BINARY_DIR}
                                  ${D3DTOOL_BINARY_DIR}
                                  ${HDF5_INCLUDE_DIR}
                                  )

    if( ${D3DTOOL_DEFINITION} )
        target_compile_definitions(${D3DTOOL_TARGET} PRIVATE -D${D3DTOOL_DEFINITION})
    endif()

endfunction()


# Create a Command line tool for the SandboxTool
COMPILE_TOOL(
    TARGET SandboxTool
    SOURCES ${DREAM3DTools_SOURCE_DIR}/SandboxTool.cpp ${DREAM3DTools_BINARY_DIR}/${PROJECT_NAME}Configuration.h
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    COMPONENT     Tools
    LINK_LIBRARIES Qt5::Core
)

COMPILE_TOOL(
    TARGET CreateWebServerJson
    SOURCES ${DREAM3DTools_SOURCE_DIR}/CreateWebServerJson.cpp ${SIMPLView_VERSION_SRC_FILE} ${DREAM3DTools_BINARY_DIR}/${PROJECT_NAME}Configuration.h
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    LINK_LIBRARIES SIMPLib
)

COMPILE_TOOL(
    TARGET RESTServerInfo
    SOURCES ${DREAM3DTools_SOURCE_DIR}/RESTServerInfo/RESTServerInfo.cpp 
            ${SIMPLView_VERSION_SRC_FILE}
            ${DREAM3DTools_BINARY_DIR}/${PROJECT_NAME}Configuration.h
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    COMPONENT     Tools
    LINK_LIBRARIES Qt5::Core SIMPLib
)


if(0)

# Create a Command line tool for the SandboxTool
COMPILE_TOOL(
    TARGET ExamplePipelineCoverage
    SOURCES ${DREAM3DTools_SOURCE_DIR}/ExamplePipelineCoverage.cpp ${DREAM3DTools_BINARY_DIR}/${PROJECT_NAME}Configuration.h
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    COMPONENT     Tools
    LINK_LIBRARIES Qt5::Core SIMPLib
)

# Create a Command line tool for the MATLAB_CodeGen
COMPILE_TOOL(
    TARGET MATLAB_CodeGen
    SOURCES ${DREAM3DTools_SOURCE_DIR}/MATLAB_CodeGen.cpp ${DREAM3DTools_BINARY_DIR}/${PROJECT_NAME}Configuration.h
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    COMPONENT     Tools
    LINK_LIBRARIES EbsdLib SIMPLib
)

endif()
