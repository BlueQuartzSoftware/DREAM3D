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

project(SurfaceMeshingPlugin)

set(PLUGIN_NAME SurfaceMeshingPlugin)

if(NOT QT4_FOUND)
    message(FATAL_ERROR "Qt 4 is Needed for this project.")
endif()
include( ${QT_USE_FILE} )

include( ${DREAM3DProj_SOURCE_DIR}/Source/FilterWidgets/FilterWidgetMacros.cmake)


include( ${SurfaceMeshingPlugin_SOURCE_DIR}/Code/SurfaceMeshing/SourceList.cmake)
include( ${SurfaceMeshingPlugin_SOURCE_DIR}/Code/SurfaceMeshing/IO/SourceList.cmake)
include( ${SurfaceMeshingPlugin_SOURCE_DIR}/Code/SurfaceMeshing/Meshing/SourceList.cmake)


include_directories(${DREAM3DProj_SOURCE_DIR}/Source )
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/QtSupport )
include_directories(${DREAM3DProj_BINARY_DIR}/QtSupport)
include_directories(${SurfaceMeshingPlugin_SOURCE_DIR}/Code)
include_directories(${SurfaceMeshingPlugin_BINARY_DIR})
include_directories(${SurfaceMeshingPlugin_BINARY_DIR}/SurfaceMeshing)
include_directories(${SurfaceMeshingPlugin_BINARY_DIR}/Code)


set(FILTER_SRCS 
  ${SurfaceMeshingPlugin_SurfaceMeshing_SRCS}
  ${SurfaceMeshingPlugin_IO_SRCS}
  ${SurfaceMeshingPlugin_Meshing_SRCS}
)

set(SurfaceMeshingPlugin_GEN_SRCS "")
set(SurfaceMeshingPlugin_GEN_HDRS "")

# --------------------------------------------------------------------
# Create an executable to autogenerate the Qt Widget sources for our filters.
GenerateFilterWidgets( PLUGIN_NAME ${PLUGIN_NAME}
                       FILTER_GROUP SurfaceMeshing
                       INCLUDE_PATH SurfaceMeshing
                       SOURCE_DIR ${SurfaceMeshingPlugin_SOURCE_DIR}
                       FILTER_SOURCES ${FILTER_SRCS}
                       FILTER_HEADERS "${SurfaceMeshingPlugin_SurfaceMeshing_HDRS}"
                       BINARY_DIR ${SurfaceMeshingPlugin_BINARY_DIR}
                       WIDGET_GEN_SRCS SurfaceMeshingPlugin_GEN_SRCS
                       WIDGET_GEN_HDRS SurfaceMeshingPlugin_GEN_HDRS 
                     )
#message(STATUS "DONE-----------------------")
#message(STATUS "SurfaceMeshingPlugin_GEN_HDRS: ${SurfaceMeshingPlugin_GEN_HDRS}")
#message(STATUS "SurfaceMeshingPlugin_GEN_SRCS: ${SurfaceMeshingPlugin_GEN_SRCS}")

add_definitions(-DDREAM3D_USE_QT)
if (MSVC)   # Do not generate manifests for the plugins - caused issues loading plugins
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /MANIFEST:NO")
endif()



#-- Setup Source files
set(SurfaceMeshingPlugin_HDRS
  ${PROJECT_SOURCE_DIR}/Code/SurfaceMeshingPlugin.h
)

set(SurfaceMeshingPlugin_SRCS
 ${PROJECT_SOURCE_DIR}/Code/SurfaceMeshingPlugin.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "SurfaceMeshing/" "${SurfaceMeshingPlugin_HDRS}" "${SurfaceMeshingPlugin_SRCS}" "0")



# --------------------------------------------------------------------
# and finally this will run moc:
QT4_WRAP_CPP( SurfaceMeshingPlugin_Generated_MOC_SRCS ${SurfaceMeshingPlugin_GEN_HDRS})
QT4_WRAP_CPP( SurfaceMeshingPlugin_MOC_SRCS ${SurfaceMeshingPlugin_HDRS} )
# These generated moc files will be #include in the FilterWidget source file that
# are generated so we need to tell the build system to NOT compile these files
set_source_files_properties( ${SurfaceMeshingPlugin_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties( ${SurfaceMeshingPlugin_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

# -- Run MOC and UIC on the necessary files
QT4_ADD_RESOURCES( SurfaceMeshingPlugin_Generated_RC_SRCS "${SurfaceMeshingPlugin_SOURCE_DIR}/Documentation/${PLUGIN_NAME}Docs.qrc"  )

cmp_IDE_SOURCE_PROPERTIES("Generated" "${SurfaceMeshingPlugin_Generated_RC_SRCS};${SurfaceMeshingPlugin_Generated_MOC_SRCS};${SurfaceMeshingPlugin_MOC_SRCS}" "${SurfaceMeshingPlugin_GEN_HDRS};${SurfaceMeshingPlugin_GEN_SRCS}" "0")


if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${SurfaceMeshingPlugin_HDRS}
            DESTINATION include/${PLUGIN_NAME}/SurfaceMeshing
            COMPONENT Headers   )
endif()

set(SurfaceMeshingPlugin_PROJECT_SRCS
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginInterface.h
    ${SurfaceMeshingPlugin_SurfaceMeshing_SRCS}
    ${SurfaceMeshingPlugin_SurfaceMeshing_HDRS}
	${DREAM3DLib_SurfaceMeshing_HDRS}
    ${SurfaceMeshingPlugin_IO_SRCS}
    ${SurfaceMeshingPlugin_IO_HDRS}
    ${SurfaceMeshingPlugin_Meshing_SRCS}
    ${SurfaceMeshingPlugin_Meshing_HDRS}
    ${SurfaceMeshingPlugin_HDRS}
    ${SurfaceMeshingPlugin_SRCS}
    ${SurfaceMeshingPlugin_GEN_SRCS}
    ${SurfaceMeshingPlugin_GEN_HDRS}
    ${SurfaceMeshingPlugin_Generated_MOC_SRCS}
    ${SurfaceMeshingPlugin_MOC_SRCS}
    ${SurfaceMeshingPlugin_Generated_RC_SRCS}
    
)


# ---------- Setup output Directories -------------------------
if(NOT MSVC)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3DProj_BINARY_DIR}/Bin/plugins  )
endif()
add_library(${PLUGIN_NAME} MODULE ${SurfaceMeshingPlugin_PROJECT_SRCS})
target_link_libraries(${PLUGIN_NAME}
                    MXA
                    DREAM3DLib
                    ${QT_QTCORE_LIBRARY}
                    ${QT_QTGUI_LIBRARY}
                    QtSupport
                    PipelineBuilderLib
                    )
add_dependencies(${PLUGIN_NAME} ${PLUGIN_NAME}_CodeGen)

PluginProperties(${PLUGIN_NAME} "_debug" "${DREAM3D_VERSION}"
                "${DREAM3D_BINARY_DIR}" "${CMP_PLUGIN_LIST_FILE}")

if (NOT MSVC)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3DProj_BINARY_DIR}/Bin  )
endif()

