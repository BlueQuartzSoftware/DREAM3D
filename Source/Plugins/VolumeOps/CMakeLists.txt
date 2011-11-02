
project(VolumeOpsPlugin)

set(PLUGIN_NAME VolumeOps)

# Get the Directory where the plugin lives
get_filename_component(PluginPath ${PROJECT_SOURCE_DIR} PATH)
include_directories(${DREAM3DProj_BINARY_DIR}
                    ${DREAM3DProj_BINARY_DIR}/QtSupport
                    ${PluginPath} 
                    ${PROJECT_BINARY_DIR} )

#-- Non Qt based source files
SET (VolumeOps_HDRS
  ${PROJECT_SOURCE_DIR}/VolumeOps.h
  ${PROJECT_SOURCE_DIR}/VolumeOpsFunc.h
)

SET (VolumeOps_SRCS
  ${PROJECT_SOURCE_DIR}/VolumeOps.cpp
  ${PROJECT_SOURCE_DIR}/VolumeOpsFunc.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "VolumeOps" "${VolumeOps_HDRS}" "${VolumeOps_SRCS}" "0")
if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${VolumeOps_HDRS}
            DESTINATION include/DREAM3D/VolumeOps
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
include_directories(${PROJECT_CODE_DIR}/QtSupport
                    ${DREAM3DProj_BINARY_DIR}/QtSupport )

set(CLASS_NAME "VolumeOps")
SET(ORIGINAL_FILE ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in)
configure_file( ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in
                ${${CLASS_NAME}Plugin_BINARY_DIR}/Q${CLASS_NAME}.h @ONLY IMMEDIATE)

set (VolumeOpsPlugin_UI_HDRS
    ${${CLASS_NAME}Plugin_BINARY_DIR}/Q${CLASS_NAME}.h
    ${PROJECT_SOURCE_DIR}/UI/VolumeOpsPlugin.h
    ${PROJECT_SOURCE_DIR}/UI/VolumeOpsWidget.h
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginFrame.h
    )
    
set (VolumeOpsPlugin_UI_SRCS 
    ${PROJECT_SOURCE_DIR}/UI/VolumeOpsPlugin.cpp
    ${PROJECT_SOURCE_DIR}/UI/VolumeOpsWidget.cpp
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginFrame.cpp
    )

# -- Run MOC and UIC on the necessary files   
QT4_ADD_RESOURCES( VolumeOpsPlugin_Generated_RC_SRCS ${PROJECT_SOURCE_DIR}/UI/Icons.qrc)    
    
QT4_WRAP_UI( VolumeOpsPlugin_Generated_UI_HDRS 
            ${PROJECT_SOURCE_DIR}/UI/VolumeOpsWidget.ui )

QT4_WRAP_CPP( VolumeOpsPlugin_Generated_MOC_SRCS ${VolumeOpsPlugin_UI_HDRS} )

cmp_IDE_SOURCE_PROPERTIES("VolumeOps/UI/" "${VolumeOpsPlugin_UI_HDRS}" "${VolumeOpsPlugin_UI_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES("Generated" "${VolumeOpsPlugin_Generated_UI_HDRS};${VolumeOpsPlugin_Generated_RC_SRCS}" "${VolumeOpsPlugin_Generated_MOC_SRCS}" "0")


set (VolumeOpsPlugin_PROJECT_SRCS
    ${VolumeOpsPlugin_UI_SRCS}
    ${VolumeOpsPlugin_UI_HDRS}
    ${VolumeOpsPlugin_Generated_MOC_SRCS} 
    ${VolumeOpsPlugin_Generated_UI_HDRS}
    ${VolumeOpsPlugin_Generated_RC_SRCS}
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginInterface.h
    ${VolumeOps_HDRS}
    ${VolumeOps_SRCS}
)


# ---------- Setup output Directories -------------------------
if(NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3DProj_BINARY_DIR}/Bin/plugins  )
endif()
add_library(VolumeOpsPlugin MODULE ${VolumeOpsPlugin_PROJECT_SRCS})
target_link_libraries(VolumeOpsPlugin 
                    MXA
                    DREAM3DLib
                    ${QT_QTCORE_LIBRARY}   
                    ${QT_QTGUI_LIBRARY}
                    QtSupport 
                     )
PluginProperties(VolumeOpsPlugin "_debug" "${DREAM3D_VERSION}" 
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
        DESTINATION Help/VolumeOps
        COMPONENT Applications )

endif()
  
# Create a Command line tool for the VolumeOps
# --------------------------------------------------------------------
# Setup the install rules for the various platforms
set(install_dir "tools")
if (WIN32)
    set (install_dir ".")
endif()

IF ( DREAM3D_BUILD_CLI_TOOLS )

#COMPILE_TOOL(
#    TARGET VolumeOps
#    SOURCES ${PROJECT_SOURCE_DIR}/Tools/VolumeOpsMain.cpp ${PROJECT_SOURCE_DIR}/VolumeOps.cpp  ${VolumeOps_HDRS}
#    ${VolumeOps_SRCS}
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



