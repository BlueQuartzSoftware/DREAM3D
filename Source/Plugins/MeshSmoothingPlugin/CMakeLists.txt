#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2011, Michael A. Jackson. BlueQuartz Software
#--  Copyright (c) 2011, Michael A. Groeber, US Air Force Research Laboratory
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////

project(MeshSmoothingPlugin)

set(PLUGIN_NAME MeshSmoothingPlugin)

if(NOT QT4_FOUND)
    message(FATAL_ERROR "Qt 4 is Needed for this project.")
endif()
include( ${QT_USE_FILE} )
message(STATUS "Plugin: ${PLUGIN_NAME} Enabled")

include( ${DREAM3DProj_SOURCE_DIR}/Source/FilterWidgets/FilterWidgetMacros.cmake)

include( ${MeshSmoothingPlugin_SOURCE_DIR}/Code/SurfaceMeshing/SourceList.cmake)
include( ${MeshSmoothingPlugin_SOURCE_DIR}/Code/IO/SourceList.cmake)
include( ${MeshSmoothingPlugin_SOURCE_DIR}/Code/Meshing/SourceList.cmake)
include( ${MeshSmoothingPlugin_SOURCE_DIR}/Code/Smoothing/SourceList.cmake)

include_directories(${DREAM3DProj_SOURCE_DIR}/Source )
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/QtSupport )
include_directories(${DREAM3DProj_BINARY_DIR}/QtSupport)
include_directories(${MeshSmoothingPlugin_SOURCE_DIR}/Code)
include_directories(${MeshSmoothingPlugin_BINARY_DIR})
# --------------------------------------------------------------------
# Create an executable to autogenerate the sources for our filters.
GenerateFilterWidgets( PLUGIN_NAME ${PLUGIN_NAME}
                       FILTER_GROUP MeshSmoothing
                       SOURCE_DIR ${MeshSmoothingPlugin_SOURCE_DIR}
                       FILTER_SOURCES "${MeshSmoothingPlugin_Filters_SRCS};${MeshSmoothingPlugin_IO_SRCS};${MeshSmoothingPlugin_Meshing_SRCS};${MeshSmoothingPlugin_Smoothing_SRCS}"
                       FILTER_HEADERS "${MeshSmoothingPlugin_Filters_HDRS}"
                       BINARY_DIR ${MeshSmoothingPlugin_BINARY_DIR}
                     )



add_definitions(-DDREAM3D_USE_QT)
if (MSVC)   # Do not generate manifests for the plugins - caused issues loading plugins
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /MANIFEST:NO")
endif()



#-- Setup NON Qt Source files
SET (SurfaceMeshing_HDRS
  ${PROJECT_SOURCE_DIR}/MeshSmoothingPlugin.h
)

SET (SurfaceMeshing_SRCS
 ${PROJECT_SOURCE_DIR}/MeshSmoothingPlugin.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "SurfaceMeshing/" "${SurfaceMeshing_HDRS}" "${SurfaceMeshing_SRCS}" "0")
if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${SurfaceMeshing_HDRS}
            DESTINATION include/DREAM3D/SurfaceMeshing
            COMPONENT Headers   )
endif()

set (MeshSmoothingPlugin_PROJECT_SRCS
    ${MeshSmoothingPlugin_Filters_HDRS}
    ${MeshSmoothingPlugin_Filters_SRCS}
    ${MeshSmoothingPlugin_IO_HDRS}
    ${MeshSmoothingPlugin_IO_SRCS}
    ${MeshSmoothingPlugin_Meshing_HDRS}
    ${MeshSmoothingPlugin_Meshing_SRCS}
    ${MeshSmoothingPlugin_Smoothing_HDRS}
    ${MeshSmoothingPlugin_Smoothing_SRCS}
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginInterface.h
    ${SurfaceMeshing_HDRS}
    ${SurfaceMeshing_SRCS}
)


# ---------- Setup output Directories -------------------------
if(NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3DProj_BINARY_DIR}/Bin/plugins  )
endif()
add_library(MeshSmoothingPlugin MODULE ${MeshSmoothingPlugin_PROJECT_SRCS})
target_link_libraries(MeshSmoothingPlugin
                    MXA
                    DREAM3DLib
                    ${QT_QTCORE_LIBRARY}
                    ${QT_QTGUI_LIBRARY}
                    QtSupport
                    PipelineBuilderLib
                    )
add_dependencies(MeshSmoothingPlugin SurfaceMeshing_CodeGen)

PluginProperties(MeshSmoothingPlugin "_debug" "${DREAM3D_VERSION}"
                "${DREAM3D_BINARY_DIR}" "${CMP_PLUGIN_LIST_FILE}")

if (NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3DProj_BINARY_DIR}/Bin  )
endif()

