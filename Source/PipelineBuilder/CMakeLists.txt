#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2011, Michael A. Jackson. BlueQuartz Software
#--  Copyright (c) 2011, Michael Groeber, US Air Force Research Laboratory
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////

project(PipelineBuilderLib)

SET (PipelineBuilderLib_BUILT_AS_DYNAMIC_LIB)
IF (BUILD_SHARED_LIBS)
    SET (PipelineBuilderLib_BUILT_AS_DYNAMIC_LIB 1)
    IF (WIN32)
        ADD_DEFINITIONS("-DPipelineBuilderLib_BUILT_AS_DYNAMIC_LIB")
    ENDIF (WIN32)
ENDIF (BUILD_SHARED_LIBS)

# Get the Directory where the plugin lives
#get_filename_component(PluginPath ${PROJECT_SOURCE_DIR} PATH)
include_directories(${PROJECT_CODE_DIR}/QtSupport
                    ${DREAM3DProj_BINARY_DIR}
                    ${DREAM3DProj_BINARY_DIR}/QtSupport
                    ${PROJECT_BINARY_DIR} 
                    ${PipelineBuilderLib_SOURCE_DIR}
)


#-- Non Qt based source files
SET (PipelineBuilder_HDRS
    ${PROJECT_SOURCE_DIR}/IFilterWidgetFactory.h
    ${PROJECT_SOURCE_DIR}/FilterWidgetManager.h
)

SET (PipelineBuilder_SRCS
    ${PROJECT_SOURCE_DIR}/FilterWidgetManager.cpp
)
cmp_IDE_SOURCE_PROPERTIES( "PipelineBuilder" "${PipelineBuilder_HDRS}" "${PipelineBuilder_SRCS}" "0")


if ( ${PROJECT_INSTALL_HEADERS} EQUAL 1 )
    INSTALL (FILES ${PipelineBuilder_HDRS}
            DESTINATION include/DREAM3D/PipelineBuilderLib
            COMPONENT Headers   )
endif()

set(CLASS_NAME "PipelineBuilder")
#SET(ORIGINAL_FILE ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in)
#configure_file( ${PROJECT_CODE_DIR}/QtSupport/QAbstractPipelineWrapper.h.in
#                ${${CLASS_NAME}Plugin_BINARY_DIR}/Q${CLASS_NAME}.h @ONLY IMMEDIATE)

set (PipelineBuilderLib_HDRS
    ${PROJECT_SOURCE_DIR}/QFilterWidget.h
    ${PROJECT_SOURCE_DIR}/PipelineBuilderWidget.h
    ${PROJECT_SOURCE_DIR}/PipelineViewWidget.h  
    ${PROJECT_SOURCE_DIR}/QFilterPipeline.h
    )
    
set (PipelineBuilderLib_SRCS 
    ${PROJECT_SOURCE_DIR}/QFilterWidget.cpp
    ${PROJECT_SOURCE_DIR}/PipelineBuilderWidget.cpp
    ${PROJECT_SOURCE_DIR}/PipelineViewWidget.cpp
    ${PROJECT_SOURCE_DIR}/QFilterPipeline.cpp
    )

cmp_IDE_SOURCE_PROPERTIES("PipelineBuilderLib/" "${PipelineBuilderLib_HDRS}" "${PipelineBuilderLib_SRCS}" "0")


# -- Run MOC and UIC on the necessary files   
QT4_ADD_RESOURCES( PipelineBuilderLib_Generated_RC_SRCS ""  )
    
QT4_WRAP_UI( PipelineBuilderLib_Generated_UI_HDRS 
            ${PROJECT_SOURCE_DIR}/UI_Files/PipelineBuilderWidget.ui)

QT4_WRAP_CPP( PipelineBuilderLib_Generated_MOC_SRCS ${PipelineBuilderLib_HDRS} )
#set_source_files_properties( ${PipelineBuilderLib_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

cmp_IDE_SOURCE_PROPERTIES("Generated" "${PipelineBuilderLib_Generated_UI_HDRS};${PipelineBuilderLib_Generated_RC_SRCS}" "${PipelineBuilderLib_Generated_MOC_SRCS}" "0")


set (PipelineBuilderLib_PROJECT_SRCS
    ${PipelineBuilderLib_SRCS}
    ${PipelineBuilderLib_HDRS}
    ${PipelineBuilderLib_Generated_MOC_SRCS} 
    ${PipelineBuilderLib_Generated_UI_HDRS}
    ${PipelineBuilderLib_Generated_RC_SRCS}
    ${PipelineBuilder_HDRS}
    ${PipelineBuilder_SRCS}
)


# ---------- Setup output Directories -------------------------
# This library MUST be shared because there is a singleton instance
# variable that ALL plugins should use.
add_library(${PROJECT_NAME} SHARED ${PipelineBuilderLib_PROJECT_SRCS})
target_link_libraries(${PROJECT_NAME}                     
                        MXA
                        DREAM3DLib
                        ${QT_QTCORE_LIBRARY}   
                        ${QT_QTGUI_LIBRARY}
                        QtSupport
                   )
LibraryProperties( ${PROJECT_NAME}  ${EXE_DEBUG_EXTENSION} )

set(install_dir "tools")
set(lib_install_dir "lib")
if (WIN32)
    set (install_dir ".")
    set(lib_install_dir "")
endif()


if (BUILD_SHARED_LIBS AND NOT APPLE)
    INSTALL(TARGETS ${PROJECT_NAME}  
        COMPONENT Applications
        RUNTIME DESTINATION ${install_dir}
        LIBRARY DESTINATION ${lib_install_dir}
        ARCHIVE DESTINATION ${lib_install_dir}       
        BUNDLE DESTINATION "."
    ) 
endif() 



