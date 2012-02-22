#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2011, Michael A. Jackson. BlueQuartz Software
#--  Copyright (c) 2011, Michael Groeber, US Air Force Research Laboratory
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////

project(PipelineBuilderPlugin)

set(PLUGIN_NAME PipelineBuilder)

# Get the Directory where the plugin lives
get_filename_component(PluginPath ${PROJECT_SOURCE_DIR} PATH)
include_directories(${PROJECT_CODE_DIR}/QtSupport
                    ${PROJECT_CODE_DIR}/UI
                    ${DREAM3DProj_BINARY_DIR}
                    ${DREAM3DProj_BINARY_DIR}/QtSupport
                    ${PluginPath} 
                    ${PROJECT_BINARY_DIR} 
                    ${PipelineBuilderPlugin_SOURCE_DIR}/UI)

# --------------------------------------------------------------------
# Build the FilterWidgets Static Library which is ONLY needed by this plugin
add_subdirectory( ${PipelineBuilderPlugin_SOURCE_DIR}/FilterWidgets/ ${PROJECT_BINARY_DIR}/FilterWidgets)


#-- Non Qt based source files
SET (PipelineBuilder_HDRS
)

SET (PipelineBuilder_SRCS
)
cmp_IDE_SOURCE_PROPERTIES( "PipelineBuilder" "${PipelineBuilder_HDRS}" "${PipelineBuilder_SRCS}" "0")
if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${PipelineBuilder_HDRS}
            DESTINATION include/DREAM3D/PipelineBuilder
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
add_definitions(-DDREAM3D_USE_QT)
if (MSVC)   # Do not generate manifests for the plugins - caused issues loading plugins
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /MANIFEST:NO")
endif()


set(CLASS_NAME "PipelineBuilder")
#SET(ORIGINAL_FILE ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in)
#configure_file( ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in
#                ${${CLASS_NAME}Plugin_BINARY_DIR}/Q${CLASS_NAME}.h @ONLY IMMEDIATE)

set (PipelineBuilderPlugin_UI_HDRS
    ${PROJECT_SOURCE_DIR}/QFilterPipeline.h
    ${PROJECT_SOURCE_DIR}/UI/PipelineBuilderPlugin.h
    ${PROJECT_SOURCE_DIR}/UI/PipelineBuilderWidget.h  
    ${PROJECT_SOURCE_DIR}/UI/PipelineViewWidget.h   
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginFrame.h
    )
    
set (PipelineBuilderPlugin_UI_SRCS 
    ${PROJECT_SOURCE_DIR}/UI/PipelineBuilderPlugin.cpp
    ${PROJECT_SOURCE_DIR}/UI/PipelineBuilderWidget.cpp
    ${PROJECT_SOURCE_DIR}/UI/PipelineViewWidget.cpp
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginFrame.cpp
    )

# -- Run MOC and UIC on the necessary files   
QT4_ADD_RESOURCES( PipelineBuilderPlugin_Generated_RC_SRCS ${PROJECT_SOURCE_DIR}/UI/Icons.qrc)    
    
QT4_WRAP_UI( PipelineBuilderPlugin_Generated_UI_HDRS 
            ${PROJECT_SOURCE_DIR}/UI/PipelineBuilderWidget.ui)

QT4_WRAP_CPP( PipelineBuilderPlugin_Generated_MOC_SRCS ${PipelineBuilderPlugin_UI_HDRS} )

cmp_IDE_SOURCE_PROPERTIES("PipelineBuilder/UI/" "${PipelineBuilderPlugin_UI_HDRS}" "${PipelineBuilderPlugin_UI_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES("Generated" "${PipelineBuilderPlugin_Generated_UI_HDRS};${PipelineBuilderPlugin_Generated_RC_SRCS}" "${PipelineBuilderPlugin_Generated_MOC_SRCS}" "0")


set (PipelineBuilderPlugin_PROJECT_SRCS
    ${PipelineBuilderPlugin_UI_SRCS}
    ${PipelineBuilderPlugin_UI_HDRS}
    ${PipelineBuilderPlugin_Generated_MOC_SRCS} 
    ${PipelineBuilderPlugin_Generated_UI_HDRS}
    ${PipelineBuilderPlugin_Generated_RC_SRCS}
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginInterface.h
    ${PipelineBuilder_HDRS}
    ${PipelineBuilder_SRCS}
)


# ---------- Setup output Directories -------------------------
if(NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3DProj_BINARY_DIR}/Bin/plugins  )
endif()
add_library(PipelineBuilderPlugin MODULE ${PipelineBuilderPlugin_PROJECT_SRCS})
target_link_libraries(PipelineBuilderPlugin                     
                        MXA
                        DREAM3DLib
                        ${QT_QTCORE_LIBRARY}   
                        ${QT_QTGUI_LIBRARY}
                        QtSupport
                        FilterWidgets
                   )
PluginProperties(PipelineBuilderPlugin "_debug" "${DREAM3D_VERSION}" 
                "${DREAM3D_BINARY_DIR}" "${CMP_PLUGIN_LIST_FILE}")

if (NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3DProj_BINARY_DIR}/Bin  )
endif()


set(${PLUGIN_NAME}_HELP_FILES
    ${PROJECT_SOURCE_DIR}/UI/Help/index.html)

#- This copies all the HTML files into the Build directory so the help
#- works from the Build Tree
file(COPY ${${PLUGIN_NAME}_HELP_FILES} 
     DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Help/${PLUGIN_NAME}/ )

#- This installs all the help files. Note that we assume a flat directory structure
#- in the "Help" folder, ie, all html files, images, etc are all in the "Help"
#- folder and NOT in any subfolders beyond that.
install(FILES ${${PLUGIN_NAME}_HELP_FILES}
        DESTINATION Help/PipelineBuilder
        COMPONENT Applications )

endif()
  
# Create a Command line tool for the PipelineBuilder
# --------------------------------------------------------------------
# Setup the install rules for the various platforms
set(install_dir "tools")
if (WIN32)
    set (install_dir ".")
endif()

IF ( DREAM3D_BUILD_CLI_TOOLS )

#COMPILE_TOOL(
#    TARGET PipelineBuilder
#    SOURCES ${PROJECT_SOURCE_DIR}/Tools/PipelineBuilderMain.cpp ${PROJECT_SOURCE_DIR}/PipelineBuilder.cpp  ${PipelineBuilder_HDRS}
#    ${PipelineBuilder_SRCS}
#    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
#    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
#    VERSION_MINOR ${DREAM3D_VER_MINOR}
#    VERSION_PATCH ${DREAM3D_VER_PATCH}
#    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
#    COMPONENT     Applications
#    INSTALL_DEST  "${install_dir}" 
#    LINK_LIBRARIES DREAM3DLib MXADataModel
#)

endif()


add_subdirectory( ${PipelineBuilderPlugin_SOURCE_DIR}/Test ${PipelineBuilderPlugin_BINARY_DIR}/Test)


