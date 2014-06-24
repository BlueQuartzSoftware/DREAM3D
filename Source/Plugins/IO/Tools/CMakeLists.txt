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

# Create a Command line tool for the PhToHDF5
COMPILE_TOOL(
    TARGET PhToHDF5
    SOURCES ${DREAM3DTools_SOURCE_DIR}/PhToHDF5.cpp
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    COMPONENT     Applications
    INSTALL_DEST  "${install_dir}"
    LINK_LIBRARIES DREAM3DLib
)

