#///////////////////////////////////////////////////////////////////////////////
#  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#///////////////////////////////////////////////////////////////////////////////
project(MicrostructureStatisticsPlugin)


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

set(CLASS_NAME "MicrostructureStatistics")
SET(ORIGINAL_FILE ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in)
configure_file( ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in
                ${${CLASS_NAME}Plugin_BINARY_DIR}/Q${CLASS_NAME}.h @ONLY IMMEDIATE)


set (MicrostructureStatisticsPlugin_HDRS
    ${${CLASS_NAME}Plugin_BINARY_DIR}/Q${CLASS_NAME}.h
    ${PROJECT_SOURCE_DIR}/MicrostructureStatisticsPlugin.h
    ${PROJECT_SOURCE_DIR}/MicrostructureStatisticsWidget.h
    ${PROJECT_CODE_DIR}/QtSupport/AIMPluginFrame.h
    )
    
set (MicrostructureStatisticsPlugin_SRCS 
    ${PROJECT_SOURCE_DIR}/MicrostructureStatisticsPlugin.cpp
    ${PROJECT_SOURCE_DIR}/MicrostructureStatisticsWidget.cpp
    ${PROJECT_CODE_DIR}/QtSupport/AIMPluginFrame.cpp
    )

# -- Run MOC and UIC on the necessary files   
QT4_ADD_RESOURCES( MicrostructureStatisticsPlugin_Generated_RC_SRCS ${PROJECT_SOURCE_DIR}/Icons.qrc)    

QT4_WRAP_UI( MicrostructureStatisticsPlugin_Generated_UI_HDRS 
            ${PROJECT_SOURCE_DIR}/MicrostructureStatisticsWidget.ui )

QT4_WRAP_CPP( MicrostructureStatisticsPlugin_Generated_MOC_SRCS ${MicrostructureStatisticsPlugin_HDRS} )

set (MicrostructureStatisticsPlugin_PROJECT_SRCS
    ${MicrostructureStatisticsPlugin_SRCS}
    ${MicrostructureStatisticsPlugin_HDRS}
    ${MicrostructureStatisticsPlugin_Generated_MOC_SRCS} 
    ${MicrostructureStatisticsPlugin_Generated_UI_HDRS}
    ${MicrostructureStatisticsPlugin_Generated_RC_SRCS}
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginInterface.h
)

cmp_IDE_SOURCE_PROPERTIES("Source/" "${MicrostructureStatisticsPlugin_HDRS}" "${MicrostructureStatisticsPlugin_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES("Generated" "${MicrostructureStatisticsPlugin_Generated_UI_HDRS}" "${MicrostructureStatisticsPlugin_Generated_MOC_SRCS}" "0")

# ---------- Setup output Directories -------------------------
if(NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3D_BINARY_DIR}/Bin/plugins  )
endif()
add_library(MicrostructureStatisticsPlugin MODULE ${MicrostructureStatisticsPlugin_PROJECT_SRCS})
target_link_libraries(MicrostructureStatisticsPlugin 
                    MXADataModel
                    DREAM3DLib
                    ${QT_QTCORE_LIBRARY}   
                    ${QT_QTGUI_LIBRARY}
                    QtSupport 
                     )
PluginProperties(MicrostructureStatisticsPlugin "_debug" "${DREAM3D_VERSION}" 
                "${DREAM3D_BINARY_DIR}" "${CMP_PLUGIN_LIST_FILE}")

# Add a Dependency so that the plugins are always build before the main Application
add_dependencies(MicrostructureStatisticsPlugin DREAM3DApp)

if (NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3D_BINARY_DIR}/Bin  )
endif()
