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

add_definitions(-DUSE_QT)
include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/..)
include_directories(${PROJECT_CODE_DIR} )
include_directories(${PROJECT_CODE_DIR}/QtSupport )
include_directories(${DREAM3D_BINARY_DIR}/QtSupport)

set(CLASS_NAME "SurfaceMesh")
SET(ORIGINAL_FILE ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in)
configure_file( ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in
                ${${CLASS_NAME}Plugin_BINARY_DIR}/Q${CLASS_NAME}.h @ONLY IMMEDIATE)


set (SurfaceMeshPlugin_UI_HDRS
    ${${CLASS_NAME}Plugin_BINARY_DIR}/Q${CLASS_NAME}.h
    ${PROJECT_SOURCE_DIR}/UI/SurfaceMeshPlugin.h
    ${PROJECT_SOURCE_DIR}/UI/SurfaceMeshWidget.h
    ${PROJECT_CODE_DIR}/QtSupport/AIMPluginFrame.h
    )
    
set (SurfaceMeshPlugin_UI_SRCS 
    ${PROJECT_SOURCE_DIR}/UI/SurfaceMeshPlugin.cpp
    ${PROJECT_SOURCE_DIR}/UI/SurfaceMeshWidget.cpp
    ${PROJECT_CODE_DIR}/QtSupport/AIMPluginFrame.cpp
    )

# -- Run MOC and UIC on the necessary files   
QT4_ADD_RESOURCES( SurfaceMeshPlugin_Generated_RC_SRCS ${PROJECT_SOURCE_DIR}/UI/Icons.qrc)    
    
QT4_WRAP_UI( SurfaceMeshPlugin_Generated_UI_HDRS 
            ${PROJECT_SOURCE_DIR}/UI/SurfaceMeshWidget.ui )

QT4_WRAP_CPP( SurfaceMeshPlugin_Generated_MOC_SRCS ${SurfaceMeshPlugin_UI_HDRS} )


cmp_IDE_SOURCE_PROPERTIES("SurfaceMesh/UI/" "${SurfaceMeshPlugin_UI_HDRS}" "${SurfaceMeshPlugin_UI_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES("Generated" "${SurfaceMeshPlugin_Generated_UI_HDRS};${SurfaceMeshPlugin_Generated_RC_SRCS}" "${SurfaceMeshPlugin_Generated_MOC_SRCS}" "0")




SET (SurfaceMesh_HDRS
  ${PROJECT_SOURCE_DIR}/SurfaceMesh.h
  ${PROJECT_SOURCE_DIR}/SurfaceMeshFunc.h

  )

SET (SurfaceMesh_SRCS
  ${PROJECT_SOURCE_DIR}/SurfaceMesh.cpp
  ${PROJECT_SOURCE_DIR}/SurfaceMeshFunc.cpp

)
cmp_IDE_SOURCE_PROPERTIES( "SurfaceMesh" "${SurfaceMesh_HDRS}" "${SurfaceMesh_SRCS}" "0")


SET (MESHING_SRCS
  ${PROJECT_SOURCE_DIR}/Meshing/Face.h
  ${PROJECT_SOURCE_DIR}/Meshing/Neighbor.h
  ${PROJECT_SOURCE_DIR}/Meshing/Node.h
  ${PROJECT_SOURCE_DIR}/Meshing/Patch.h
  ${PROJECT_SOURCE_DIR}/Meshing/Segment.h
  ${PROJECT_SOURCE_DIR}/Meshing/STLWriter.h
  ${PROJECT_SOURCE_DIR}/Meshing/SMVtkFileIO.h
)
SET (MESHING_HDRS
  ${PROJECT_SOURCE_DIR}/Meshing/Face.cpp
  ${PROJECT_SOURCE_DIR}/Meshing/Neighbor.cpp
  ${PROJECT_SOURCE_DIR}/Meshing/Node.cpp
  ${PROJECT_SOURCE_DIR}/Meshing/Patch.cpp
  ${PROJECT_SOURCE_DIR}/Meshing/Segment.cpp
  ${PROJECT_SOURCE_DIR}/Meshing/STLWriter.cpp
  ${PROJECT_SOURCE_DIR}/Meshing/SMVtkFileIO.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "SurfaceMesh/Meshing" "${MESHING_HDRS}" "${MESHING_SRCS}" "0")

SET (SMOOTHING_SRCS
    ${PROJECT_SOURCE_DIR}/Smoothing/Smoothing.cpp
)
SET (SMOOTHING_HDRS
    ${PROJECT_SOURCE_DIR}/Smoothing/Smoothing.h
    ${PROJECT_SOURCE_DIR}/Smoothing/LinearAlgebra.h
    ${PROJECT_SOURCE_DIR}/Smoothing/Triangle.hpp
    ${PROJECT_SOURCE_DIR}/Smoothing/Node.hpp
)
cmp_IDE_SOURCE_PROPERTIES( "SurfaceMesh/Smoothing" "${SMOOTHING_HDRS}" "${SMOOTHING_SRCS}" "0")


if ( ${DREAM3D_INSTALL_FILES} EQUAL 1 )
    INSTALL (FILES ${SurfaceMesh_HDRS} ${MESHING_HDRS} ${SMOOTHING_HDRS}
            DESTINATION include/DREAM3D/SurfaceMesh
            COMPONENT Headers   )
endif()


#-- Gather all the project source files in this variable
set (SurfaceMeshPlugin_PROJECT_SRCS
    ${SurfaceMeshPlugin_UI_SRCS}
    ${SurfaceMeshPlugin_UI_HDRS}
    ${SurfaceMeshPlugin_Generated_MOC_SRCS} 
    ${SurfaceMeshPlugin_Generated_UI_HDRS}
    ${SurfaceMeshPlugin_Generated_RC_SRCS}
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginInterface.h
    ${SurfaceMesh_HDRS}
    ${SurfaceMesh_SRCS}
    ${MESHING_SRCS}
    ${MESHING_HDRS}
    ${SMOOTHING_SRCS}
    ${SMOOTHING_HDRS}
)

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
PluginProperties(SurfaceMeshPlugin "_debug" "${VERSION}" 
                "${BINARY_DIR}" "${CMP_PLUGIN_LIST_FILE}")

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
  
# Create a Command line tool for the SurfaceMesh
# --------------------------------------------------------------------
# Setup the install rules for the various platforms
set(install_dir "tools")
if (WIN32)
    set (install_dir ".")
endif()
COMPILE_TOOL(
    TARGET SurfaceMesh
    SOURCES ${PROJECT_SOURCE_DIR}/Tools/SurfaceMeshMain.cpp     ${SurfaceMesh_HDRS}
                                                                ${SurfaceMesh_SRCS}
                                                                ${MESHING_SRCS}
                                                                ${MESHING_HDRS}
                                                                ${SMOOTHING_SRCS}
                                                                ${SMOOTHING_HDRS}
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    COMPONENT     Applications
    INSTALL_DEST  "${install_dir}" 
    LINK_LIBRARIES DREAM3DLib
)

