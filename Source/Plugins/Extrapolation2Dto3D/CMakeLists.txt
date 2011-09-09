message(STATUS "Extrapolation2Dto3DPlugin Being Configured")
project(Extrapolation2Dto3DPlugin)

set(PLUGIN_NAME Extrapolation2Dto3D)

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
                    ${PROJECT_SOURCE_DIR}/..
                    ${PROJECT_BINARY_DIR}
                    ${PROJECT_CODE_DIR}
                    ${PROJECT_CODE_DIR}/QtSupport
                    ${DREAM3D_BINARY_DIR}/QtSupport
)

set(CLASS_NAME "Extrapolation2Dto3D")
SET(ORIGINAL_FILE ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in)
configure_file( ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in
                ${${CLASS_NAME}Plugin_BINARY_DIR}/Q${CLASS_NAME}.h @ONLY IMMEDIATE)

set (Extrapolation2Dto3DPlugin_UI_HDRS
    ${${CLASS_NAME}Plugin_BINARY_DIR}/Q${CLASS_NAME}.h
    ${PROJECT_SOURCE_DIR}/UI/Extrapolation2Dto3DPlugin.h
    ${PROJECT_SOURCE_DIR}/UI/Extrapolation2Dto3DWidget.h
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginFrame.h
    )
    
set (Extrapolation2Dto3DPlugin_UI_SRCS 
    ${PROJECT_SOURCE_DIR}/UI/Extrapolation2Dto3DPlugin.cpp
    ${PROJECT_SOURCE_DIR}/UI/Extrapolation2Dto3DWidget.cpp
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginFrame.cpp
    )

# -- Run MOC and UIC on the necessary files   
QT4_ADD_RESOURCES( Extrapolation2Dto3DPlugin_Generated_RC_SRCS ${PROJECT_SOURCE_DIR}/UI/Icons.qrc)    
    
QT4_WRAP_UI( Extrapolation2Dto3DPlugin_Generated_UI_HDRS 
            ${PROJECT_SOURCE_DIR}/UI/Extrapolation2Dto3DWidget.ui )

QT4_WRAP_CPP( Extrapolation2Dto3DPlugin_Generated_MOC_SRCS ${Extrapolation2Dto3DPlugin_UI_HDRS} )

cmp_IDE_SOURCE_PROPERTIES("Extrapolation2Dto3D/UI/" "${Extrapolation2Dto3DPlugin_UI_HDRS}" "${Extrapolation2Dto3DPlugin_UI_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES("Generated" "${Extrapolation2Dto3DPlugin_Generated_UI_HDRS};${Extrapolation2Dto3DPlugin_Generated_RC_SRCS}" "${Extrapolation2Dto3DPlugin_Generated_MOC_SRCS}" "0")


SET (Extrapolation2Dto3D_HDRS
  ${PROJECT_SOURCE_DIR}/Extrapolation2Dto3D.h
  ${PROJECT_SOURCE_DIR}/Extrapolation2Dto3DFunc.h
)

SET (Extrapolation2Dto3D_SRCS
  ${PROJECT_SOURCE_DIR}/Extrapolation2Dto3D.cpp
  ${PROJECT_SOURCE_DIR}/Extrapolation2Dto3DFunc.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "Extrapolation2Dto3D" "${Extrapolation2Dto3D_HDRS}" "${Extrapolation2Dto3D_SRCS}" "0")
if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${Extrapolation2Dto3D_HDRS}
            DESTINATION include/DREAM3D/Extrapolation2Dto3D
            COMPONENT Headers   )
endif()



set (Extrapolation2Dto3DPlugin_PROJECT_SRCS
    ${Extrapolation2Dto3DPlugin_UI_SRCS}
    ${Extrapolation2Dto3DPlugin_UI_HDRS}
    ${Extrapolation2Dto3DPlugin_Generated_MOC_SRCS} 
    ${Extrapolation2Dto3DPlugin_Generated_UI_HDRS}
    ${Extrapolation2Dto3DPlugin_Generated_RC_SRCS}
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginInterface.h
    ${Extrapolation2Dto3D_HDRS}
    ${Extrapolation2Dto3D_SRCS}
)

# ---------- Setup output Directories -------------------------
if(NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3D_BINARY_DIR}/Bin/plugins  )
endif()
add_library(Extrapolation2Dto3DPlugin MODULE ${Extrapolation2Dto3DPlugin_PROJECT_SRCS})
target_link_libraries(Extrapolation2Dto3DPlugin 
                    MXADataModel
                    DREAM3DLib
                    ${QT_QTCORE_LIBRARY}   
                    ${QT_QTGUI_LIBRARY}
                    QtSupport 
                     )
PluginProperties(Extrapolation2Dto3DPlugin "_debug" "${DREAM3D_VERSION}" 
                "${DREAM3D_BINARY_DIR}" "${CMP_PLUGIN_LIST_FILE}")

if (NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3D_BINARY_DIR}/Bin  )
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
        DESTINATION Help/Extrapolation2Dto3D
        COMPONENT Applications )
  


