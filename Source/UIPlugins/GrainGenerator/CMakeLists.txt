#///////////////////////////////////////////////////////////////////////////////
#  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#///////////////////////////////////////////////////////////////////////////////
project(GrainGeneratorPlugin)

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


set (GrainGeneratorPlugin_HDRS
    ${PROJECT_SOURCE_DIR}/GrainGeneratorPlugin.h
    ${PROJECT_SOURCE_DIR}/GrainGeneratorWidget.h
    ${PROJECT_CODE_DIR}/QtSupport/AIMPluginFrame.h
    ${PROJECT_CODE_DIR}/DREAM3D/GrainGenerator/GrainGenerator.h
    )
    
set (GrainGeneratorPlugin_SRCS 
    ${PROJECT_SOURCE_DIR}/GrainGeneratorPlugin.cpp
    ${PROJECT_SOURCE_DIR}/GrainGeneratorWidget.cpp
    ${PROJECT_CODE_DIR}/QtSupport/AIMPluginFrame.cpp
    ${PROJECT_CODE_DIR}/DREAM3D/GrainGenerator/GrainGenerator.cpp
    )

# -- Run MOC and UIC on the necessary files   
QT4_ADD_RESOURCES( GrainGeneratorPlugin_Generated_RC_SRCS ${PROJECT_SOURCE_DIR}/Icons.qrc)    
    
QT4_WRAP_UI( GrainGeneratorPlugin_Generated_UI_HDRS 
            ${PROJECT_SOURCE_DIR}/GrainGeneratorWidget.ui )

QT4_WRAP_CPP( GrainGeneratorPlugin_Generated_MOC_SRCS ${GrainGeneratorPlugin_HDRS} )

set (GrainGeneratorPlugin_PROJECT_SRCS
    ${GrainGeneratorPlugin_SRCS}
    ${GrainGeneratorPlugin_HDRS}
    ${GrainGeneratorPlugin_Generated_MOC_SRCS} 
    ${GrainGeneratorPlugin_Generated_UI_HDRS}
    ${GrainGeneratorPlugin_Generated_RC_SRCS}
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginInterface.h
)

cmp_IDE_SOURCE_PROPERTIES("Source/" "${GrainGeneratorPlugin_HDRS}" "${GrainGeneratorPlugin_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES("Generated" "${GrainGeneratorPlugin_Generated_UI_HDRS}" "${GrainGeneratorPlugin_Generated_MOC_SRCS}" "0")

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
                "${DREAM3D_BINARY_DIR}" "DREAM3D_Plugins.txt")
if (NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3D_BINARY_DIR}/Bin  )
endif()
