#///////////////////////////////////////////////////////////////////////////////
#  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#///////////////////////////////////////////////////////////////////////////////
project(OIMImportPlugin)


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


set (OIMImportPlugin_HDRS
    ${PROJECT_SOURCE_DIR}/OIMImportPlugin.h
    ${PROJECT_SOURCE_DIR}/OIMImportWidget.h
    ${PROJECT_CODE_DIR}/QtSupport/AIMPluginFrame.h
    ${PROJECT_CODE_DIR}/DREAM3D/OIMImport/OIMImport.h
    )
    
set (OIMImportPlugin_SRCS 
    ${PROJECT_SOURCE_DIR}/OIMImportPlugin.cpp
    ${PROJECT_SOURCE_DIR}/OIMImportWidget.cpp
    ${PROJECT_CODE_DIR}/QtSupport/AIMPluginFrame.cpp
    ${PROJECT_CODE_DIR}/DREAM3D/OIMImport/OIMImport.cpp
    )

# -- Run MOC and UIC on the necessary files   
QT4_ADD_RESOURCES( OIMImportPlugin_Generated_RC_SRCS ${PROJECT_SOURCE_DIR}/Icons.qrc)    
    
QT4_WRAP_UI( OIMImportPlugin_Generated_UI_HDRS 
            ${PROJECT_SOURCE_DIR}/OIMImportWidget.ui )

QT4_WRAP_CPP( OIMImportPlugin_Generated_MOC_SRCS ${OIMImportPlugin_HDRS} )

set (OIMImportPlugin_PROJECT_SRCS
    ${OIMImportPlugin_SRCS}
    ${OIMImportPlugin_HDRS}
    ${OIMImportPlugin_Generated_MOC_SRCS} 
    ${OIMImportPlugin_Generated_UI_HDRS}
    ${OIMImportPlugin_Generated_RC_SRCS}
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginInterface.h
)

cmp_IDE_SOURCE_PROPERTIES("Source/" "${OIMImportPlugin_HDRS}" "${OIMImportPlugin_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES("Generated" "${OIMImportPlugin_Generated_UI_HDRS}" "${OIMImportPlugin_Generated_MOC_SRCS}" "0")

# ---------- Setup output Directories -------------------------
if(NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3D_BINARY_DIR}/Bin/plugins  )
endif()
add_library(OIMImportPlugin MODULE ${OIMImportPlugin_PROJECT_SRCS})
target_link_libraries(OIMImportPlugin 
                    MXADataModel
                    TSLAng
                    DREAM3DLib
                    ${QT_QTCORE_LIBRARY}   
                    ${QT_QTGUI_LIBRARY}
                    QtSupport 
                     )

PluginProperties(OIMImportPlugin "_debug" "${DREAM3D_VERSION}" 
                "${DREAM3D_BINARY_DIR}" "${CMP_PLUGIN_LIST_FILE}")
if (NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3D_BINARY_DIR}/Bin  )
endif()
