#///////////////////////////////////////////////////////////////////////////////
#  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#///////////////////////////////////////////////////////////////////////////////
project(EbsdImportPlugin)

set(PLUGIN_NAME EbsdImport)

# ------------------------------------------------------------------------------
#  Qt 4 Section
# ------------------------------------------------------------------------------
if(QT4_FOUND)
    include( ${QT_USE_FILE} )
else()
    message(FATAL_ERROR "Qt 4 is Needed for this project.")
endif(QT4_FOUND)

add_definitions(-DDREAM3D_USE_QT)
if (MSVC)   # Do not generate manifests for the plugins - caused issues loading plugins
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /MANIFEST:NO")
endif()

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/..)
include_directories(${PROJECT_CODE_DIR} )
include_directories(${PROJECT_CODE_DIR}/QtSupport )
include_directories(${DREAM3D_BINARY_DIR}/QtSupport)

set(CLASS_NAME "EbsdImport")
SET(ORIGINAL_FILE ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in)
configure_file( ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in
                ${${CLASS_NAME}Plugin_BINARY_DIR}/Q${CLASS_NAME}.h @ONLY IMMEDIATE)


set (EbsdImportPlugin_UI_HDRS
    ${${CLASS_NAME}Plugin_BINARY_DIR}/Q${CLASS_NAME}.h
    ${PROJECT_SOURCE_DIR}/UI/EbsdImportPlugin.h
    ${PROJECT_SOURCE_DIR}/UI/EbsdImportWidget.h
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginFrame.h
    )
    
set (EbsdImportPlugin_UI_SRCS 
    ${PROJECT_SOURCE_DIR}/UI/EbsdImportPlugin.cpp
    ${PROJECT_SOURCE_DIR}/UI/EbsdImportWidget.cpp
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginFrame.cpp
    )

# -- Run MOC and UIC on the necessary files   
QT4_ADD_RESOURCES( EbsdImportPlugin_Generated_RC_SRCS 
            ${PROJECT_SOURCE_DIR}/UI/Icons.qrc
)
    
QT4_WRAP_UI( EbsdImportPlugin_Generated_UI_HDRS 
            ${PROJECT_SOURCE_DIR}/UI/EbsdImportWidget.ui )

QT4_WRAP_CPP( EbsdImportPlugin_Generated_MOC_SRCS ${EbsdImportPlugin_UI_HDRS} )

cmp_IDE_SOURCE_PROPERTIES("EbsdImport/UI/" "${EbsdImportPlugin_UI_HDRS}" "${EbsdImportPlugin_UI_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES("Generated" "${EbsdImportPlugin_Generated_UI_HDRS};${EbsdImportPlugin_Generated_RC_SRCS}" "${EbsdImportPlugin_Generated_MOC_SRCS}" "0")

#-- Setup NON Qt Source files
SET (EbsdImport_HDRS
  ${PROJECT_SOURCE_DIR}/EbsdImport.h
)

SET (EbsdImport_SRCS
 ${PROJECT_SOURCE_DIR}/EbsdImport.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "EbsdImport/" "${EbsdImport_HDRS}" "${EbsdImport_SRCS}" "0")
if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${EbsdImport_HDRS}
            DESTINATION include/DREAM3D/EbsdImport
            COMPONENT Headers   )
endif()

set (EbsdImportPlugin_PROJECT_SRCS
    ${EbsdImportPlugin_UI_SRCS}
    ${EbsdImportPlugin_UI_HDRS}
    ${EbsdImportPlugin_Generated_MOC_SRCS} 
    ${EbsdImportPlugin_Generated_UI_HDRS}
    ${EbsdImportPlugin_Generated_RC_SRCS}
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginInterface.h
    ${EbsdImport_HDRS}
    ${EbsdImport_SRCS}
)


# ---------- Setup output Directories -------------------------
if(NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3D_BINARY_DIR}/Bin/plugins  )
endif()
add_library(EbsdImportPlugin MODULE ${EbsdImportPlugin_PROJECT_SRCS})
target_link_libraries(EbsdImportPlugin 
                    MXADataModel
                    DREAM3DLib
                    ${QT_QTCORE_LIBRARY}   
                    ${QT_QTGUI_LIBRARY}
                    QtSupport 
                     )

PluginProperties(EbsdImportPlugin "_debug" "${DREAM3D_VERSION}" 
                "${DREAM3D_BINARY_DIR}" "${CMP_PLUGIN_LIST_FILE}")

if (NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3D_BINARY_DIR}/Bin  )
endif()


set(${PLUGIN_NAME}_HELP_FILES
    ${PROJECT_SOURCE_DIR}/UI/Help/index.html
    ${PROJECT_SOURCE_DIR}/UI/Help/image001.png
    ${PROJECT_SOURCE_DIR}/UI/Help/HKLReferenceFrame.png
    ${PROJECT_SOURCE_DIR}/UI/Help/TSLReferenceFrame.png
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
  
