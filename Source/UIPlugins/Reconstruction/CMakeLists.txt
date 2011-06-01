#///////////////////////////////////////////////////////////////////////////////
#  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#///////////////////////////////////////////////////////////////////////////////
project(ReconstructionPlugin)


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


set (ReconstructionPlugin_HDRS
    ${PROJECT_SOURCE_DIR}/ReconstructionPlugin.h
    ${PROJECT_SOURCE_DIR}/ReconstructionWidget.h
    ${PROJECT_CODE_DIR}/QtSupport/AIMPluginFrame.h
    ${PROJECT_CODE_DIR}/DREAM3D/Reconstruction/Reconstruction.h
    )
    
set (ReconstructionPlugin_SRCS 
    ${PROJECT_SOURCE_DIR}/ReconstructionPlugin.cpp
    ${PROJECT_SOURCE_DIR}/ReconstructionWidget.cpp
    ${PROJECT_CODE_DIR}/QtSupport/AIMPluginFrame.cpp
    ${PROJECT_CODE_DIR}/DREAM3D/Reconstruction/Reconstruction.cpp
    )

# -- Run MOC and UIC on the necessary files   
QT4_ADD_RESOURCES( ReconstructionPlugin_Generated_RC_SRCS ${PROJECT_SOURCE_DIR}/Icons.qrc)    
    
QT4_WRAP_UI( ReconstructionPlugin_Generated_UI_HDRS 
            ${PROJECT_SOURCE_DIR}/ReconstructionWidget.ui )

QT4_WRAP_CPP( ReconstructionPlugin_Generated_MOC_SRCS ${ReconstructionPlugin_HDRS} )

set (ReconstructionPlugin_PROJECT_SRCS
    ${ReconstructionPlugin_SRCS}
    ${ReconstructionPlugin_HDRS}
    ${ReconstructionPlugin_Generated_MOC_SRCS} 
    ${ReconstructionPlugin_Generated_UI_HDRS}
    ${ReconstructionPlugin_Generated_RC_SRCS}
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginInterface.h
)

cmp_IDE_SOURCE_PROPERTIES("Source/" "${ReconstructionPlugin_HDRS}" "${ReconstructionPlugin_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES("Generated" "${ReconstructionPlugin_Generated_UI_HDRS}" "${ReconstructionPlugin_Generated_MOC_SRCS}" "0")

# ---------- Setup output Directories -------------------------
if(NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3D_BINARY_DIR}/Bin/plugins  )
endif()
add_library(ReconstructionPlugin MODULE ${ReconstructionPlugin_PROJECT_SRCS})
target_link_libraries(ReconstructionPlugin 
                    MXADataModel
                    DREAM3DLib
                    ${QT_QTCORE_LIBRARY}   
                    ${QT_QTGUI_LIBRARY}
                    QtSupport 
                     )
PluginProperties(ReconstructionPlugin "_debug" "${DREAM3D_VERSION}" 
                "${DREAM3D_BINARY_DIR}" "${CMP_PLUGIN_LIST_FILE}")
if (NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3D_BINARY_DIR}/Bin  )
endif()
