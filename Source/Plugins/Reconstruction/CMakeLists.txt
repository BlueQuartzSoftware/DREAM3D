#///////////////////////////////////////////////////////////////////////////////
#  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#///////////////////////////////////////////////////////////////////////////////
project(ReconstructionPlugin)

set(PLUGIN_NAME Reconstruction)

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

include_directories(${PROJECT_SOURCE_DIR} 
                    ${PROJECT_BINARY_DIR}
                    ${PROJECT_SOURCE_DIR}/..
                    ${PROJECT_SOURCE_DIR}/UI
                    ${PROJECT_CODE_DIR}
                    ${PROJECT_CODE_DIR}/QtSupport
                    ${DREAM3D_BINARY_DIR}/QtSupport
)


set(CLASS_NAME "Reconstruction")
SET(ORIGINAL_FILE ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in)
configure_file( ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in
                ${${CLASS_NAME}Plugin_BINARY_DIR}/Q${CLASS_NAME}.h @ONLY IMMEDIATE)


set (ReconstructionPlugin_UI_HDRS
    ${${CLASS_NAME}Plugin_BINARY_DIR}/Q${CLASS_NAME}.h
    ${PROJECT_SOURCE_DIR}/UI/ReconstructionPlugin.h
    ${PROJECT_SOURCE_DIR}/UI/ReconstructionWidget.h
    ${PROJECT_CODE_DIR}/QtSupport/AIMPluginFrame.h
    ${PROJECT_SOURCE_DIR}/UI/QQualityMetricWidget.h
    )
    
set (ReconstructionPlugin_UI_SRCS 
    ${PROJECT_SOURCE_DIR}/UI/ReconstructionPlugin.cpp
    ${PROJECT_SOURCE_DIR}/UI/ReconstructionWidget.cpp
    ${PROJECT_CODE_DIR}/QtSupport/AIMPluginFrame.cpp
    ${PROJECT_SOURCE_DIR}/UI/QQualityMetricWidget.cpp
    )

# -- Run MOC and UIC on the necessary files   
QT4_ADD_RESOURCES( ReconstructionPlugin_Generated_RC_SRCS ${PROJECT_SOURCE_DIR}/UI/Icons.qrc)    
    
QT4_WRAP_UI( ReconstructionPlugin_Generated_UI_HDRS 
            ${PROJECT_SOURCE_DIR}/UI/ReconstructionWidget.ui )

QT4_WRAP_CPP( ReconstructionPlugin_Generated_MOC_SRCS ${ReconstructionPlugin_UI_HDRS} )

cmp_IDE_SOURCE_PROPERTIES("Reconstruction/UI/" "${ReconstructionPlugin_UI_HDRS}" "${ReconstructionPlugin_UI_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES("Generated" "${ReconstructionPlugin_Generated_UI_HDRS};${ReconstructionPlugin_Generated_RC_SRCS}" "${ReconstructionPlugin_Generated_MOC_SRCS}" "0")

#-- Non Qt Sources
SET (Reconstruction_HDRS
  ${PROJECT_SOURCE_DIR}/Reconstruction.h
  ${PROJECT_SOURCE_DIR}/ReconstructionFunc.h
)

SET (Reconstruction_SRCS
  ${PROJECT_SOURCE_DIR}/Reconstruction.cpp
  ${PROJECT_SOURCE_DIR}/ReconstructionFunc.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "Reconstruction" "${Reconstruction_HDRS}" "${Reconstruction_SRCS}" "0")
if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${Reconstruction_HDRS}
            DESTINATION include/DREAM3D/Reconstruction
            COMPONENT Headers   )
endif()

#-- AngSupport Files
include(${PROJECT_SOURCE_DIR}/EbsdSupport/SourceList.cmake)


#-- Gather all the Project Source files in this variable
set (ReconstructionPlugin_PROJECT_SRCS
    ${ReconstructionPlugin_UI_SRCS}
    ${ReconstructionPlugin_UI_HDRS}
    ${ReconstructionPlugin_Generated_MOC_SRCS} 
    ${ReconstructionPlugin_Generated_UI_HDRS}
    ${ReconstructionPlugin_Generated_RC_SRCS}
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginInterface.h
    ${Reconstruction_HDRS}
    ${Reconstruction_SRCS}
    ${EbsdSupport_HDRS}
    ${EbsdSupport_SRCS}
)


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
  

  
# Create a Command line tool for the Reconstruction
# --------------------------------------------------------------------
# Setup the install rules for the various platforms
set(install_dir "tools")
if (WIN32)
    set (install_dir ".")
endif()
COMPILE_TOOL(
    TARGET Reconstruction
    SOURCES ${PROJECT_SOURCE_DIR}/Tools/ReconstructionMain.cpp 
            ${Reconstruction_HDRS} ${Reconstruction_SRCS}
            ${EbsdSupport_HDRS} ${EbsdSupport_SRCS}
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    COMPONENT     Applications
    INSTALL_DEST  "${install_dir}" 
    LINK_LIBRARIES EbsdLib DREAM3DLib
)
  
if (0)
# Create a Command line tool for the H5AngToVtk
COMPILE_TOOL(
    TARGET H5AngToVtk
    SOURCES ${PROJECT_SOURCE_DIR}/Tools/H5AngToVtk.cpp 
            ${Reconstruction_HDRS} ${Reconstruction_SRCS}
            ${EbsdSupport_HDRS} ${EbsdSupport_SRCS}
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    COMPONENT     Applications
    INSTALL_DEST  "${install_dir}" 
    LINK_LIBRARIES DREAM3DLib
)
endif()
