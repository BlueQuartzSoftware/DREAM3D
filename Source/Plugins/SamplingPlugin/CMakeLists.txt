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

project(SamplingPlugin)

set(PLUGIN_NAME SamplingPlugin)

if(NOT QT4_FOUND)
    message(FATAL_ERROR "Qt 4 is Needed for this project.")
endif()
include( ${QT_USE_FILE} )

include( ${DREAM3DProj_SOURCE_DIR}/Source/FilterWidgets/FilterWidgetMacros.cmake)


include( ${SamplingPlugin_SOURCE_DIR}/Code/Sampling/SourceList.cmake)


include_directories(${DREAM3DProj_SOURCE_DIR}/Source )
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/QtSupport )
include_directories(${DREAM3DProj_BINARY_DIR}/QtSupport)

include_directories(${SamplingPlugin_SOURCE_DIR}/Code)
include_directories(${SamplingPlugin_BINARY_DIR})
include_directories(${SamplingPlugin_BINARY_DIR}/Sampling)
include_directories(${SamplingPlugin_BINARY_DIR}/Code)


set(FILTER_SRCS 
  ${SamplingPlugin_Sampling_SRCS}
)

set(SamplingPlugin_GEN_SRCS "")
set(SamplingPlugin_GEN_HDRS "")

# --------------------------------------------------------------------
# Create an executable to autogenerate the Qt Widget sources for our filters.
GenerateFilterWidgets( PLUGIN_NAME ${PLUGIN_NAME}
                       FILTER_GROUP Sampling
                       INCLUDE_PATH Sampling
                       SOURCE_DIR ${SamplingPlugin_SOURCE_DIR}
                       FILTER_SOURCES ${FILTER_SRCS}
                       FILTER_HEADERS "${SamplingPlugin_Sampling_HDRS}"
                       BINARY_DIR ${SamplingPlugin_BINARY_DIR}
                       WIDGET_GEN_SRCS SamplingPlugin_GEN_SRCS
                       WIDGET_GEN_HDRS SamplingPlugin_GEN_HDRS 
                     )

add_definitions(-DDREAM3D_USE_QT)
if (MSVC)   # Do not generate manifests for the plugins - caused issues loading plugins
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /MANIFEST:NO")
endif()



#-- Setup Source files
SET (SamplingPlugin_HDRS
  ${PROJECT_SOURCE_DIR}/Code/SamplingPlugin.h
)

SET (SamplingPlugin_SRCS
 ${PROJECT_SOURCE_DIR}/Code/SamplingPlugin.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "Sampling/" "${SamplingPlugin_HDRS}" "${SamplingPlugin_SRCS}" "0")



# --------------------------------------------------------------------
# and finally this will run moc:
QT4_WRAP_CPP( SamplingPlugin_Generated_MOC_SRCS ${SamplingPlugin_GEN_HDRS})
QT4_WRAP_CPP( SamplingPlugin_MOC_SRCS ${SamplingPlugin_HDRS} )
# These generated moc files will be #include in the FilterWidget source file that
# are generated so we need to tell the build system to NOT compile these files
set_source_files_properties( ${SamplingPlugin_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)
set_source_files_properties( ${SamplingPlugin_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

# -- Run MOC and UIC on the necessary files
QT4_ADD_RESOURCES( SamplingPlugin_Generated_RC_SRCS "${SamplingPlugin_SOURCE_DIR}/Documentation/${PLUGIN_NAME}Docs.qrc"  )

cmp_IDE_SOURCE_PROPERTIES("Generated" "${SamplingPlugin_Generated_RC_SRCS};${SamplingPlugin_Generated_MOC_SRCS};${SamplingPlugin_MOC_SRCS}" "${SamplingPlugin_GEN_HDRS};${SamplingPlugin_GEN_SRCS}" "0")


if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${SamplingPlugin_HDRS}
            DESTINATION include/${PLUGIN_NAME}/Sampling
            COMPONENT Headers   )
endif()

set (SamplingPlugin_PROJECT_SRCS
    ${PROJECT_CODE_DIR}/QtSupport/DREAM3DPluginInterface.h
    ${SamplingPlugin_Sampling_SRCS}
    ${SamplingPlugin_Sampling_HDRS}
    ${SamplingPlugin_HDRS}
    ${SamplingPlugin_SRCS}
    ${SamplingPlugin_GEN_SRCS}
    ${SamplingPlugin_GEN_HDRS}
    ${SamplingPlugin_Generated_MOC_SRCS}
    ${SamplingPlugin_MOC_SRCS}
    ${SamplingPlugin_Generated_RC_SRCS}
)


# ---------- Setup output Directories -------------------------
if(NOT MSVC)
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3DProj_BINARY_DIR}/Bin/plugins  )
endif()
add_library(${PLUGIN_NAME} MODULE ${SamplingPlugin_PROJECT_SRCS})
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
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${DREAM3DProj_BINARY_DIR}/Bin  )
endif()

