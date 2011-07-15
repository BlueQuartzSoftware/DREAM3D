#///////////////////////////////////////////////////////////////////////////////
#  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#///////////////////////////////////////////////////////////////////////////////
project(MicrostructureStatisticsPlugin)

set(PLUGIN_NAME MicrostructureStatistics)

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

set(CLASS_NAME "MicrostructureStatistics")
SET(ORIGINAL_FILE ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in)
configure_file( ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in
                ${${CLASS_NAME}Plugin_BINARY_DIR}/Q${CLASS_NAME}.h @ONLY IMMEDIATE)


set (MicrostructureStatisticsPlugin_UI_HDRS
    ${${CLASS_NAME}Plugin_BINARY_DIR}/Q${CLASS_NAME}.h
    ${PROJECT_SOURCE_DIR}/UI/MicrostructureStatisticsPlugin.h
    ${PROJECT_SOURCE_DIR}/UI/MicrostructureStatisticsWidget.h
    ${PROJECT_CODE_DIR}/QtSupport/AIMPluginFrame.h
    )
    
set (MicrostructureStatisticsPlugin_UI_SRCS 
    ${PROJECT_SOURCE_DIR}/UI/MicrostructureStatisticsPlugin.cpp
    ${PROJECT_SOURCE_DIR}/UI/MicrostructureStatisticsWidget.cpp
    ${PROJECT_CODE_DIR}/QtSupport/AIMPluginFrame.cpp
    )

# -- Run MOC and UIC on the necessary files   
QT4_ADD_RESOURCES( MicrostructureStatisticsPlugin_Generated_RC_SRCS ${PROJECT_SOURCE_DIR}/UI/Icons.qrc)    

QT4_WRAP_UI( MicrostructureStatisticsPlugin_Generated_UI_HDRS 
            ${PROJECT_SOURCE_DIR}/UI/MicrostructureStatisticsWidget.ui )

QT4_WRAP_CPP( MicrostructureStatisticsPlugin_Generated_MOC_SRCS ${MicrostructureStatisticsPlugin_UI_HDRS} )

cmp_IDE_SOURCE_PROPERTIES("MicrostructureStatistics/UI/" "${MicrostructureStatisticsPlugin_UI_HDRS}" "${MicrostructureStatisticsPlugin_UI_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES("Generated" "${MicrostructureStatisticsPlugin_Generated_UI_HDRS};${MicrostructureStatisticsPlugin_Generated_RC_SRCS}" "${MicrostructureStatisticsPlugin_Generated_MOC_SRCS}" "0")

SET (MicrostructureStatistics_HDRS
  ${PROJECT_SOURCE_DIR}/MicrostructureStatistics.h
  ${PROJECT_SOURCE_DIR}/MicrostructureStatisticsFunc.h
  ${PROJECT_SOURCE_DIR}/Algo.hpp
)

SET (MicrostructureStatistics_SRCS
  ${PROJECT_SOURCE_DIR}/MicrostructureStatistics.cpp
  ${PROJECT_SOURCE_DIR}/MicrostructureStatisticsFunc.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "MicrostructureStatistics" "${MicrostructureStatistics_HDRS}" "${MicrostructureStatistics_SRCS}" "0")
if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${MicrostructureStatistics_HDRS}
            DESTINATION include/DREAM3D/MicrostructureStatistics
            COMPONENT Headers   )
endif()


set (MicrostructureStatisticsPlugin_PROJECT_SRCS
    ${MicrostructureStatisticsPlugin_UI_SRCS}
    ${MicrostructureStatisticsPlugin_UI_HDRS}
    ${MicrostructureStatisticsPlugin_Generated_MOC_SRCS} 
    ${MicrostructureStatisticsPlugin_Generated_UI_HDRS}
    ${MicrostructureStatisticsPlugin_Generated_RC_SRCS}
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginInterface.h
    ${MicrostructureStatistics_HDRS}
    ${MicrostructureStatistics_SRCS}
)


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

if (NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3D_BINARY_DIR}/Bin  )
endif()


set(${PLUGIN_NAME}_HELP_FILES
    ${PROJECT_SOURCE_DIR}/UI/Help/index.html
    ${PROJECT_SOURCE_DIR}/UI/Help/image001.png
    ${PROJECT_SOURCE_DIR}/UI/Help/image002.png
    ${PROJECT_SOURCE_DIR}/UI/Help/image003.png
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
  
