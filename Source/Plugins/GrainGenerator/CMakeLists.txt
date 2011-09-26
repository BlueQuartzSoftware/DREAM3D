#///////////////////////////////////////////////////////////////////////////////
#  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#///////////////////////////////////////////////////////////////////////////////
project(GrainGeneratorPlugin)

set(PLUGIN_NAME GrainGenerator)

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/..)
include_directories(${PROJECT_CODE_DIR} )


#-- Now set source files that do not need Qt.
SET (GrainGenerator_HDRS
  ${PROJECT_SOURCE_DIR}/GrainGenerator.h
  ${PROJECT_SOURCE_DIR}/GrainGeneratorFunc.h
  ${PROJECT_SOURCE_DIR}/Algorithms/PackGrainsGen2.h
  ${PROJECT_SOURCE_DIR}/Algorithms/PackGrainsGen3.h
)

SET (GrainGenerator_SRCS
  ${PROJECT_SOURCE_DIR}/GrainGenerator.cpp
  ${PROJECT_SOURCE_DIR}/GrainGeneratorFunc.cpp
  ${PROJECT_SOURCE_DIR}/Algorithms/PackGrainsGen2.cpp
  ${PROJECT_SOURCE_DIR}/Algorithms/PackGrainsGen3.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "GrainGenerator/" "${GrainGenerator_HDRS}" "${GrainGenerator_SRCS}" "0")
if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${GrainGenerator_HDRS}
            DESTINATION include/DREAM3D/GrainGenerator
            COMPONENT Headers   )
endif()

#-- Add in the Structure Readers for the Grain Generator
SET (StructureReaders_HDRS
  ${PROJECT_SOURCE_DIR}/StructureReaders/AbstractStructureReader.h
  ${PROJECT_SOURCE_DIR}/StructureReaders/DXStructureReader.h
  ${PROJECT_SOURCE_DIR}/StructureReaders/VTKStructureReader.h
)

SET (StructureReaders_SRCS
  ${PROJECT_SOURCE_DIR}/StructureReaders/DXStructureReader.cpp
  ${PROJECT_SOURCE_DIR}/StructureReaders/VTKStructureReader.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "GrainGenerator/StructureReaders" "${StructureReaders_HDRS}" "${StructureReaders_SRCS}" "0")
if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${StructureReaders_HDRS}
            DESTINATION include/DREAM3D/StructureReaders
            COMPONENT Headers   )
endif()

# ------------------------------------------------------------------------------
#  Qt 4 Section
# ------------------------------------------------------------------------------
if(DREAM3D_BUILD_GUI)
    if(NOT QT4_FOUND)
        message(FATAL_ERROR "Qt 4 is Needed for this project.")
    endif()
include( ${QT_USE_FILE} )
message(STATUS "Plugin: ${PLUGIN_NAME} Enabled")

include_directories(${PROJECT_CODE_DIR}/QtSupport )
include_directories(${DREAM3D_BINARY_DIR}/QtSupport)


add_definitions(-DDREAM3D_USE_QT)
if (MSVC)   # Do not generate manifests for the plugins - caused issues loading plugins
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /MANIFEST:NO")
endif()
set(CLASS_NAME "GrainGenerator")
SET(ORIGINAL_FILE ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in)
configure_file( ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in
                ${${CLASS_NAME}Plugin_BINARY_DIR}/Q${CLASS_NAME}.h @ONLY IMMEDIATE)

set (GrainGeneratorPlugin_UI_HDRS
    ${${CLASS_NAME}Plugin_BINARY_DIR}/Q${CLASS_NAME}.h
    ${PROJECT_SOURCE_DIR}/UI/GrainGeneratorPlugin.h
    ${PROJECT_SOURCE_DIR}/UI/GrainGeneratorWidget.h
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginFrame.h
    )
    
set (GrainGeneratorPlugin_UI_SRCS 
    ${PROJECT_SOURCE_DIR}/UI/GrainGeneratorPlugin.cpp
    ${PROJECT_SOURCE_DIR}/UI/GrainGeneratorWidget.cpp
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginFrame.cpp
    )

# -- Run MOC and UIC on the necessary files   
QT4_ADD_RESOURCES( GrainGeneratorPlugin_Generated_RC_SRCS ${PROJECT_SOURCE_DIR}/UI/Icons.qrc)    
    
QT4_WRAP_UI( GrainGeneratorPlugin_Generated_UI_HDRS 
            ${PROJECT_SOURCE_DIR}/UI/GrainGeneratorWidget.ui )

QT4_WRAP_CPP( GrainGeneratorPlugin_Generated_MOC_SRCS ${GrainGeneratorPlugin_UI_HDRS} )

cmp_IDE_SOURCE_PROPERTIES("GrainGenerator/UI/" "${GrainGeneratorPlugin_UI_HDRS}" "${GrainGeneratorPlugin_UI_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES("Generated" "${GrainGeneratorPlugin_Generated_UI_HDRS};${GrainGeneratorPlugin_Generated_RC_SRCS}" "${GrainGeneratorPlugin_Generated_MOC_SRCS}" "0")


set (GrainGeneratorPlugin_PROJECT_SRCS
    ${GrainGeneratorPlugin_UI_SRCS}
    ${GrainGeneratorPlugin_UI_HDRS}
    ${GrainGeneratorPlugin_Generated_MOC_SRCS} 
    ${GrainGeneratorPlugin_Generated_UI_HDRS}
    ${GrainGeneratorPlugin_Generated_RC_SRCS}
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginInterface.h
    ${GrainGenerator_HDRS}
    ${GrainGenerator_SRCS}
    ${StructureReaders_HDRS}
    ${StructureReaders_SRCS}
)


# ---------- Setup output Directories -------------------------
if(NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3D_BINARY_DIR}/Bin/plugins  )
endif()
add_library(GrainGeneratorPlugin MODULE ${GrainGeneratorPlugin_PROJECT_SRCS})
target_link_libraries(GrainGeneratorPlugin 
                    MXADataModel
                    DREAM3DLib
                    ${QT_QTCORE_LIBRARY}   
                    ${QT_QTGUI_LIBRARY}
                    QtSupport 
                     )
PluginProperties(GrainGeneratorPlugin "_debug" "${DREAM3D_VERSION}" 
                "${DREAM3D_BINARY_DIR}" "${CMP_PLUGIN_LIST_FILE}")              


if (NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3D_BINARY_DIR}/Bin  )
endif()

set(${PLUGIN_NAME}_HELP_FILES
    ${PROJECT_SOURCE_DIR}/UI/Help/index.html
    ${PROJECT_SOURCE_DIR}/UI/Help/image001.png
    )

#- This copies all the HTML files into the Build directory so the help
#- works from the Build Tree
file(COPY ${${PLUGIN_NAME}_HELP_FILES} 
    DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Help/${PLUGIN_NAME}/ )

#- This installs all the help files. Note that we assume a flat directory structure
#- in the "Help" folder, ie, all html files, images, etc are all in the "Help"
#- folder and NOT in any subfolders beyond that.
install(FILES ${${PLUGIN_NAME}_HELP_FILES}
        DESTINATION Help/${PLUGIN_NAME}
        COMPONENT Applications )
endif()
  
# Create a Command line tool for the GrainGenerator
# --------------------------------------------------------------------
# Setup the install rules for the various platforms
set(install_dir "tools")
if (WIN32)
    set (install_dir ".")
endif()

IF ( DREAM3D_BUILD_CLI_TOOLS )
COMPILE_TOOL(
    TARGET GrainGenerator
    SOURCES ${PROJECT_SOURCE_DIR}/Tools/GrainGeneratorMain.cpp ${PROJECT_SOURCE_DIR}/GrainGenerator.cpp  ${GrainGenerator_HDRS}
    ${GrainGenerator_SRCS}
    ${StructureReaders_HDRS}
    ${StructureReaders_SRCS}
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    COMPONENT     Applications
    INSTALL_DEST  "${install_dir}" 
    DEFINITION   "SVG_3D_LIBRARY"
    LINK_LIBRARIES DREAM3DLib MXADataModel
)
endif()

