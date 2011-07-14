#///////////////////////////////////////////////////////////////////////////////
#  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#///////////////////////////////////////////////////////////////////////////////
project(SurfaceMeshPlugin)

set(PLUGIN_NAME SurfaceMesh)

# ------------------------------------------------------------------------------
#  Qt 4 Section
# ------------------------------------------------------------------------------
if(QT4_FOUND)
    include( ${QT_USE_FILE} )
else()
    message(FATAL_ERROR "Qt 4 is Needed for this project.")
endif(QT4_FOUND)

add_definitions(-DDREAM3D_USE_QT)
include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/..)
include_directories(${PROJECT_CODE_DIR} )
include_directories(${PROJECT_CODE_DIR}/QtSupport )
include_directories(${DREAM3D_BINARY_DIR}/QtSupport)

set(CLASS_NAME "SurfaceMesh")
SET(ORIGINAL_FILE ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in)
configure_file( ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in
                ${${CLASS_NAME}Plugin_BINARY_DIR}/Q${CLASS_NAME}.h @ONLY IMMEDIATE)


set (SurfaceMeshPlugin_HDRS
    ${${CLASS_NAME}Plugin_BINARY_DIR}/Q${CLASS_NAME}.h
    ${PROJECT_SOURCE_DIR}/UI/SurfaceMeshPlugin.h
    ${PROJECT_SOURCE_DIR}/UI/SurfaceMeshWidget.h
    ${PROJECT_CODE_DIR}/QtSupport/AIMPluginFrame.h
    )
    
set (SurfaceMeshPlugin_SRCS 
    ${PROJECT_SOURCE_DIR}/UI/SurfaceMeshPlugin.cpp
    ${PROJECT_SOURCE_DIR}/UI/SurfaceMeshWidget.cpp
    ${PROJECT_CODE_DIR}/QtSupport/AIMPluginFrame.cpp
    )

# -- Run MOC and UIC on the necessary files   
QT4_ADD_RESOURCES( SurfaceMeshPlugin_Generated_RC_SRCS ${PROJECT_SOURCE_DIR}/UI/Icons.qrc)    
    
QT4_WRAP_UI( SurfaceMeshPlugin_Generated_UI_HDRS 
            ${PROJECT_SOURCE_DIR}/UI/SurfaceMeshWidget.ui )

QT4_WRAP_CPP( SurfaceMeshPlugin_Generated_MOC_SRCS ${SurfaceMeshPlugin_HDRS} )

set (SurfaceMeshPlugin_PROJECT_SRCS
    ${SurfaceMeshPlugin_SRCS}
    ${SurfaceMeshPlugin_HDRS}
    ${SurfaceMeshPlugin_Generated_MOC_SRCS} 
    ${SurfaceMeshPlugin_Generated_UI_HDRS}
    ${SurfaceMeshPlugin_Generated_RC_SRCS}
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginInterface.h
)

cmp_IDE_SOURCE_PROPERTIES("Source/UI/" "${SurfaceMeshPlugin_HDRS}" "${SurfaceMeshPlugin_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES("Generated" "${SurfaceMeshPlugin_Generated_UI_HDRS}" "${SurfaceMeshPlugin_Generated_MOC_SRCS}" "0")

# ---------- Setup output Directories -------------------------
if(NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3D_BINARY_DIR}/Bin/plugins  )
endif()
add_library(SurfaceMeshPlugin MODULE ${SurfaceMeshPlugin_PROJECT_SRCS})
target_link_libraries(SurfaceMeshPlugin 
                    MXADataModel
                    DREAM3DLib
                    ${QT_QTCORE_LIBRARY}   
                    ${QT_QTGUI_LIBRARY}
                    QtSupport 
                     )
PluginProperties(SurfaceMeshPlugin "_debug" "${DREAM3D_VERSION}" 
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
  
