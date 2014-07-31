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
PROJECT( DREAM3DTools )
cmake_minimum_required(VERSION 2.8.9)

# --------------------------------------------------------------------
# Setup the install rules for the various platforms
set(install_dir "tools")
if(WIN32)
    set(install_dir ".")
endif()

# --------------------------------------------------------------------------
macro(ConvertPathToHeaderCompatible INPUT)
    if(WIN32)
      STRING(REPLACE "\\" "\\\\" ${INPUT} ${${INPUT}} )
      STRING(REPLACE "/" "\\\\" ${INPUT} ${${INPUT}}  )
    endif()
endmacro()


set(DREAM3DProj_SOURCE_DIR_NORM ${DREAM3DProj_SOURCE_DIR})
ConvertPathToHeaderCompatible(${DREAM3DProj_SOURCE_DIR_NORM})
set(DREAM3DLib_SOURCE_DIR_NORM ${DREAM3DLib_SOURCE_DIR})
ConvertPathToHeaderCompatible(${DREAM3DLib_SOURCE_DIR_NORM})
set(DREAM3DLib_PLUGIN_DIR_NORM "${DREAM3DProj_SOURCE_DIR}/Source/Plugins")
ConvertPathToHeaderCompatible(${DREAM3DLib_PLUGIN_DIR_NORM})

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
        LINK_LIBRARIES ${QT_QTCORE_LIBRARY} EbsdLib DREAM3DLib "${D3DTOOL_LINK_LIBRARIES}"
        LIB_SEARCH_DIRS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMPONENT     Applications
        INSTALL_DEST  "${D3DTOOL_INSTALL_DEST}"
        SOLUTION_FOLDER "Tools"
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
    COMPONENT     Applications
    INSTALL_DEST  "${install_dir}"
    LINK_LIBRARIES DREAM3DLib
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
    COMPONENT     Applications
    LINK_LIBRARIES DREAM3DLib
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
    COMPONENT     Applications
    LINK_LIBRARIES DREAM3DLib
)

#COMPILE_TOOL(
#    TARGET ModifiedLambertTest
#    SOURCES ${DREAM3DTools_SOURCE_DIR}/ModifiedLambertTest.cpp
#    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
#    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
#    VERSION_MINOR ${DREAM3D_VER_MINOR}
#    VERSION_PATCH ${DREAM3D_VER_PATCH}
#    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
#    COMPONENT     Applications
#    INSTALL_DEST  "${install_dir}"
#    LINK_LIBRARIES ${QT_QTGUI_LIBRARY}
#)


# --------------------------------------------------------------------
# Run Qts automoc program to generate some source files that get compiled
#QT4_WRAP_CPP( DREAM3DFileStructure_Generated_MOC_SRCS ${DREAM3DTools_SOURCE_DIR}/DREAM3DFileStructure.h)
COMPILE_TOOL(
    TARGET DREAM3DFileStructure
    SOURCES ${DREAM3DTools_SOURCE_DIR}/DREAM3DFileStructure.cpp
            ${DREAM3DTools_SOURCE_DIR}/DREAM3DFileStructure_Main.cpp
            ${DREAM3DFileStructure_Generated_MOC_SRCS}
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    COMPONENT     Applications
    LINK_LIBRARIES ${QT_QTGUI_LIBRARY}
)

# Create a Command line tool for the DocsToPdf
if(0)
set(DREAM3D_DOCS_PDF_FILENAME ${DREAM3DProj_BINARY_DIR}/DREAM3D_Filter_Docs.pdf)
#  message(STATUS "DREAM3D_DOCS_ROOT_DIR: ${DREAM3D_DOCS_ROOT_DIR}")
#  message(STATUS "DREAM3D_DOCS_PDF_FILENAME: ${DREAM3D_DOCS_PDF_FILENAME}" )

configure_file(${DREAM3DTools_SOURCE_DIR}/DocsToPdf.h.in ${DREAM3DTools_BINARY_DIR}/DocsToPdf.h)
include_directories(${DREAM3DTools_BINARY_DIR})
COMPILE_TOOL(
    TARGET DocsToPdf
    SOURCES ${DREAM3DTools_SOURCE_DIR}/DocsToPdf.cpp ${DREAM3DTools_BINARY_DIR}/DocsToPdf.h
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    COMPONENT     Applications
    LINK_LIBRARIES ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
)
endif()

option(COMPILE_EBSD_TO_TIFF "Compile the EbsdToTiff program" OFF)
if(COMPILE_EBSD_TO_TIFF)
     # Prepend our own "FindTiff" code in front of what CMake itself provides
        set(CMAKE_MODULE_PATH ${CMP_MODULES_SOURCE_DIR} ${CMAKE_MODULE_PATH} )
        FIND_PACKAGE(Tiff)
        if(NOT TIFF_FOUND)
              MESSAGE (FATAL_ERROR " Tiff Library was NOT Found and is needed.")
        endif(NOT TIFF_FOUND)
        INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIRS})
      # Copy DLL libraries into the correct build location on MSVC platforms
      if(HAVE_TIFF_DLL)
        CMP_COPY_DEPENDENT_LIBRARIES ("tiff")
        CMP_LIBRARIES_INSTALL_RULES ("tiff" "." )
      endif()

    COMPILE_TOOL(
        TARGET EbsdToTiff
        SOURCES ${DREAM3DTools_SOURCE_DIR}/EbsdToTiff.cpp
        DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
        VERSION_MAJOR ${DREAM3D_VER_MAJOR}
        VERSION_MINOR ${DREAM3D_VER_MINOR}
        VERSION_PATCH ${DREAM3D_VER_PATCH}
        BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
        COMPONENT     Applications
        LINK_LIBRARIES  ${TIFF_LIBRARY} EbsdLib  DREAM3DLib
    )


    # Create a Command line tool for the AngReaderTest
    COMPILE_TOOL(
        TARGET BcfDumper
        SOURCES ${DREAM3DTools_SOURCE_DIR}/BcfDumper.cpp ${DREAM3DTools_SOURCE_DIR}/TiffUtilities.cpp ${DREAM3DTools_SOURCE_DIR}/TiffUtilities.h
        DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
        VERSION_MAJOR ${DREAM3D_VER_MAJOR}
        VERSION_MINOR ${DREAM3D_VER_MINOR}
        VERSION_PATCH ${DREAM3D_VER_PATCH}
        BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
        COMPONENT     Applications
        LINK_LIBRARIES  ${TIFF_LIBRARY}
    )

    # create a tool to generate the IPF Triangle
  COMPILE_TOOL(
       TARGET TSLTriangle
       SOURCES ${DREAM3DTools_SOURCE_DIR}/TSLTriangle.cpp
       DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
       VERSION_MAJOR ${DREAM3D_VER_MAJOR}
       VERSION_MINOR ${DREAM3D_VER_MINOR}
       VERSION_PATCH ${DREAM3D_VER_PATCH}
       BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
       COMPONENT     Applications
       LINK_LIBRARIES ${TIFF_LIBRARY}
   )
endif()
