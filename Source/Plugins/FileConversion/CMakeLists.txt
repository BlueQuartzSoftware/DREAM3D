#--////////////////////////////////////////////////////////////////////////////
#  Copyright (c) 2011, Michael A. Jackson. BlueQuartz Software
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////
project(FileConversionPlugin)

set(PLUGIN_NAME FileConversion)

# Get the Directory where the plugin lives
get_filename_component(PluginPath ${PROJECT_SOURCE_DIR} PATH)
include_directories(${DREAM3DProj_BINARY_DIR}
                    ${DREAM3DProj_BINARY_DIR}/QtSupport
                    ${PluginPath} 
                    ${PROJECT_BINARY_DIR} )

#-- Non Qt based source files
SET (FileConversion_HDRS
  ${PROJECT_SOURCE_DIR}/FileConversion.h
#  ${PROJECT_SOURCE_DIR}/FileConversionFunc.h
#  ${PROJECT_SOURCE_DIR}/GrainIdReader.h
#  ${PROJECT_SOURCE_DIR}/GrainIdWriter.h
#  ${PROJECT_SOURCE_DIR}/VtkGrainIdReader.h

)

SET (FileConversion_SRCS
  ${PROJECT_SOURCE_DIR}/FileConversion.cpp
#  ${PROJECT_SOURCE_DIR}/FileConversionFunc.cpp
#  ${PROJECT_SOURCE_DIR}/VtkGrainIdReader.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "FileConversion" "${FileConversion_HDRS}" "${FileConversion_SRCS}" "0")
if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${FileConversion_HDRS}
            DESTINATION include/DREAM3D/FileConversion
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
                    ${DREAM3D_BINARY_DIR}/QtSupport )

set(CLASS_NAME "FileConversion")
SET(ORIGINAL_FILE ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in)
configure_file( ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in
                ${${CLASS_NAME}Plugin_BINARY_DIR}/Q${CLASS_NAME}.h @ONLY IMMEDIATE)

set (FileConversionPlugin_UI_HDRS
    ${${CLASS_NAME}Plugin_BINARY_DIR}/Q${CLASS_NAME}.h
    ${PROJECT_SOURCE_DIR}/UI/FileConversionPlugin.h
    ${PROJECT_SOURCE_DIR}/UI/FileConversionWidget.h
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginFrame.h
    )
    
set (FileConversionPlugin_UI_SRCS 
    ${PROJECT_SOURCE_DIR}/UI/FileConversionPlugin.cpp
    ${PROJECT_SOURCE_DIR}/UI/FileConversionWidget.cpp
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginFrame.cpp
    )

# -- Run MOC and UIC on the necessary files   
QT4_ADD_RESOURCES( FileConversionPlugin_Generated_RC_SRCS ${PROJECT_SOURCE_DIR}/UI/Icons.qrc)    
    
QT4_WRAP_UI( FileConversionPlugin_Generated_UI_HDRS 
            ${PROJECT_SOURCE_DIR}/UI/FileConversionWidget.ui )

QT4_WRAP_CPP( FileConversionPlugin_Generated_MOC_SRCS ${FileConversionPlugin_UI_HDRS} )

cmp_IDE_SOURCE_PROPERTIES("FileConversion/UI/" "${FileConversionPlugin_UI_HDRS}" "${FileConversionPlugin_UI_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES("Generated" "${FileConversionPlugin_Generated_UI_HDRS};${FileConversionPlugin_Generated_RC_SRCS}" "${FileConversionPlugin_Generated_MOC_SRCS}" "0")


set (FileConversionPlugin_PROJECT_SRCS
    ${FileConversionPlugin_UI_SRCS}
    ${FileConversionPlugin_UI_HDRS}
    ${FileConversionPlugin_Generated_MOC_SRCS} 
    ${FileConversionPlugin_Generated_UI_HDRS}
    ${FileConversionPlugin_Generated_RC_SRCS}
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginInterface.h
    ${FileConversion_HDRS}
    ${FileConversion_SRCS}
)


# ---------- Setup output Directories -------------------------
if(NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3DProj_BINARY_DIR}/Bin/plugins  )
endif()
add_library(FileConversionPlugin MODULE ${FileConversionPlugin_PROJECT_SRCS})
target_link_libraries(FileConversionPlugin 
                    MXA
                    DREAM3DLib
                    ${QT_QTCORE_LIBRARY}   
                    ${QT_QTGUI_LIBRARY}
                    QtSupport 
                     )
PluginProperties(FileConversionPlugin "_debug" "${DREAM3D_VERSION}" 
                "${DREAM3D_BINARY_DIR}" "${CMP_PLUGIN_LIST_FILE}")

if (NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3DProj_BINARY_DIR}/Bin  )
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
        DESTINATION Help/FileConversion
        COMPONENT Applications )

endif()
  
# Create a Command line tool for the FileConversion
# --------------------------------------------------------------------
# Setup the install rules for the various platforms
set(install_dir "tools")
if (WIN32)
    set (install_dir ".")
endif()


IF ( DREAM3D_BUILD_CLI_TOOLS )
  #-- Include the CMake commands to build the Comand line tools
  # include(${PROJECT_SOURCE_DIR}/Tools/Tools.cmake)
endif()

IF (DREAM3D_BUILD_TESTING)
  #  add_subdirectory(Test ${FileConversionPlugin_BINARY_DIR}/Test)
endif()





