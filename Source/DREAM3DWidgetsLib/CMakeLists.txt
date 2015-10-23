#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2012, Michael A. Jackson. BlueQuartz Software
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#--////////////////////////////////////////////////////////////////////////////

#///////////////////////////////////////////////////////////////////////////////
#// This code was partly written under US Air Force Contract FA8650-07-D-5800
#///////////////////////////////////////////////////////////////////////////////

project(DREAM3DWidgetsLib VERSION ${DREAM3DProj_VERSION_MAJOR}.${DREAM3DProj_VERSION_MINOR})


set(DREAM3DWidgetsLib_BUILT_AS_DYNAMIC_LIB)
if(BUILD_SHARED_LIBS)
    set(DREAM3DWidgetsLib_BUILT_AS_DYNAMIC_LIB 1)
    if(WIN32)
        ADD_DEFINITIONS("-DDREAM3DWidgetsLib_BUILT_AS_DYNAMIC_LIB")
    endif(WIN32)
endif(BUILD_SHARED_LIBS)

set(GENERATE_GIT_VERSION 1)

if(NOT PROJECT_RESOURCES_DIR)
set(PROJECT_RESOURCES_DIR ${DREAM3DWidgetsLib_SOURCE_DIR}/Resources)
endif()
if(NOT DEFINED CMP_SOURCE_DIR)
set(CMP_SOURCE_DIR ${DREAM3DWidgetsLib_SOURCE_DIR}/Support/cmp)
endif()

set(PROJECT_PREFIX "DREAM3DWidgetsLib" CACHE STRING "The Prefix to be used for Preprocessor definitions")
set(DREAM3DWidgetsLib_INSTALL_FILES 0)
#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the
# project.
#-------------------------------------------------------------------------------
set(CMP_HEADER_DIR ${DREAM3DWidgetsLib_BINARY_DIR})
set(CMP_CONFIGURATION_FILE_NAME "DREAM3DWidgetsLibConfiguration.h")
set(CMP_TYPES_FILE_NAME "DREAM3DWidgetsLibTypes.h")
set(CMP_VERSION_HEADER_FILE_NAME "DREAM3DWidgetsLibVersion.h")
set(CMP_VERSION_SOURCE_FILE_NAME "DREAM3DWidgetsLibVersion.cpp")
set(CMP_TOP_HEADER_FILE "DREAM3DWidgetsLib.h")
set(CMP_PROJECT_NAMESPACE "${PROJECT_NAME}")
set(CMP_PROJECT_NAME "${PROJECT_NAME}")
set(CMP_ENABLE_PLUGINS "0")
set(CMP_LIB_SEARCH_DIRS "")
set(CMP_GENERATE_VERSION_STRING ${GENERATE_GIT_VERSION})

# Extra Variables that need to be set before all the configured files are generated.
set(DREAM3DWidgetsLib_HDF5_SUPPORT 1)
option(DREAM3DWidgetsLib_ENABLE_HDF5 "Enable the use of HDF5 functionality" ON)
mark_as_advanced(DREAM3DWidgetsLib_ENABLE_HDF5)

include(${CMP_SOURCE_DIR}/cmpProject.cmake)

configure_file(${DREAM3DWidgetsLib_SOURCE_DIR}/DREAM3DWidgetsLibConfiguration.h.in
               ${DREAM3DWidgetsLib_BINARY_DIR}/${CMP_TOP_HEADER_FILE})

set(Project_SRCS "")

set(${PROJECT_NAME}_Generated_HDRS
        ${CMP_HEADER_DIR}/${CMP_CONFIGURATION_FILE_NAME}
        ${CMP_HEADER_DIR}/${CMP_TYPES_FILE_NAME}
        ${CMP_HEADER_DIR}/${CMP_VERSION_HEADER_FILE_NAME}
        ${CMP_HEADER_DIR}/${CMP_TOP_HEADER_FILE}
)

set(${PROJECT_NAME}_Generated_SRCS
        ${CMP_HEADER_DIR}/${CMP_VERSION_SOURCE_FILE_NAME}
)

cmp_IDE_GENERATED_PROPERTIES("Generated" "${${PROJECT_NAME}_Generated_HDRS}" "${${PROJECT_NAME}_Generated_SRCS}")

set(DREAM3DWidgetsLib_HDRS
  ${DREAM3DWidgetsLib_SOURCE_DIR}/FilterWidgetManager.h
  ${DREAM3DWidgetsLib_SOURCE_DIR}/IFilterWidgetFactory.h
  ${DREAM3DWidgetsLib_SOURCE_DIR}/PipelineFilterWidgetFactory.hpp
  ${DREAM3DWidgetsLib_SOURCE_DIR}/DREAM3DWidgetsLibDLLExport.h
  ${DREAM3DWidgetsLib_SOURCE_DIR}/DREAM3DWidgetsLibConstants.h
)

set(DREAM3DWidgetsLib_MOC_HDRS
  ${DREAM3DWidgetsLib_SOURCE_DIR}/UpdateCheck.h
  ${DREAM3DWidgetsLib_SOURCE_DIR}/UpdateCheckData.h
)

set(DREAM3DWidgetsLib_SRCS
  ${DREAM3DWidgetsLib_SOURCE_DIR}/FilterWidgetManager.cpp
  ${DREAM3DWidgetsLib_SOURCE_DIR}/IFilterWidgetFactory.cpp
  ${DREAM3DWidgetsLib_SOURCE_DIR}/UpdateCheck.cpp
  ${DREAM3DWidgetsLib_SOURCE_DIR}/UpdateCheckData.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "DREAM3DWidgetsLib" "${DREAM3DWidgetsLib_HDRS};${DREAM3DWidgetsLib_MOC_HDRS}" "${DREAM3DWidgetsLib_SRCS}" "0")

# --------------------------------------------------------------------
# and finally this will run moc:
QT5_WRAP_CPP( DREAM3DWidgetsLib_Generated_MOC_SRCS ${DREAM3DWidgetsLib_MOC_HDRS} )
set_source_files_properties( ${DREAM3DWidgetsLib_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${DREAM3DWidgetsLib_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)


# -- Run MOC and UIC on the necessary files
QT5_ADD_RESOURCES( DREAM3DWidgetsLib_Generated_RC_SRCS "${DREAM3DProj_SOURCE_DIR}/Resources/icons/images/Icons.qrc"  )

# --------------------------------------------------------------------
# Continue on with our Qt4 section
QT5_WRAP_UI( DREAM3DWidgetsLib_Generated_UI_HDRS   )

# --------------------------------------------------------------------
#-- Put the Qt generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Moc" "" "${DREAM3DWidgetsLib_Generated_MOC_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${DREAM3DWidgetsLib_Generated_UI_HDRS}" "" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Qrc" "${DREAM3DWidgetsLib_Generated_RC_SRCS}" "" "0")


# --------------------------------------------------------------------
#-- Include all the generic Widgets that can be used to build up a GUI
include(${DREAM3DWidgetsLib_SOURCE_DIR}/Widgets/SourceList.cmake)

# --------------------------------------------------------------------
# Include all the Filter Parameter Widgets
include(${DREAM3DWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/SourceList.cmake)


set(Project_SRCS
  ${DREAM3DWidgetsLib_HDRS}
  ${DREAM3DWidgetsLib_MOC_HDRS}
  ${DREAM3DWidgetsLib_SRCS}
  ${${PROJECT_NAME}_Generated_HDRS}
  ${${PROJECT_NAME}_Generated_SRCS}
  ${DREAM3DWidgetsLib_Generated_MOC_SRCS}
  ${DREAM3DWidgetsLib_Generated_UI_HDRS}
  ${DREAM3DWidgetsLib_Generated_RC_SRCS}
  #---------------
  ${DREAM3DWidgetsLib_Widgets_HDRS}
  ${DREAM3DWidgetsLib_Widgets_MOC_HDRS}
  ${DREAM3DWidgetsLib_Widgets_SRCS}
  ${DREAM3DWidgetsLib_Widgets_UIS}
  ${DREAM3DWidgetsLib_Widgets_Generated_MOC_SRCS}
  ${DREAM3DWidgetsLib_Widgets_Generated_UI_HDRS}
  #---------------
  ${DREAM3DWidgetsLib_ParameterWidgets_HDRS}
  ${DREAM3DWidgetsLib_ParameterWidgets_MOC_HDRS}
  ${DREAM3DWidgetsLib_ParameterWidgets_SRCS}
  ${DREAM3DWidgetsLib_ParameterWidgets_UIS}
  ${DREAM3DWidgetsLib_ParameterWidgets_Generated_MOC_SRCS}
  ${DREAM3DWidgetsLib_ParameterWidgets_Generated_UI_HDRS}

  # -- This file has all the MOC sources in it
  ${DREAM3DWidgetsLib_Generated_MOC_SRCS}
)



# --------------------------------------------------------------------
# Set some Include directories
include_directories(${PROJECT_CODE_DIR})
include_directories(${PROJECT_CODE_DIR}/QtSupportLib)
include_directories(${DREAM3DProj_BINARY_DIR}/QtSupportLib)
include_directories(${DREAM3DWidgetsLib_SOURCE_DIR}/..)
include_directories(${DREAM3DWidgetsLib_SOURCE_DIR}/Widgets)
include_directories(${DREAM3DWidgetsLib_SOURCE_DIR}/FilterParameterWidgets)
include_directories(${DREAM3DWidgetsLib_BINARY_DIR})
include_directories(${DREAM3DWidgetsLib_BINARY_DIR}/Widgets)
include_directories(${DREAM3DWidgetsLib_BINARY_DIR}/FilterParameterWidgets)

if(UNIX AND NOT APPLE)
    set_source_files_properties(${Project_SRCS} PROPERTIES COMPILE_FLAGS -fPIC)
endif()

add_library(DREAM3DWidgetsLib ${LIB_TYPE} ${Project_SRCS})

set(D3D_LINK_LIBRARIES Qt5::Core
                      Qt5::Gui
                      Qt5::Widgets
                      QtSupportLib
                      SIMPLib)

target_link_libraries(DREAM3DWidgetsLib ${D3D_LINK_LIBRARIES} )

LibraryProperties( DREAM3DWidgetsLib ${EXE_DEBUG_EXTENSION})



set(install_dir "tools")
set(lib_install_dir "lib")
if(WIN32)
    set(install_dir ".")
    set(lib_install_dir "")
endif()


if(BUILD_SHARED_LIBS AND NOT APPLE)
    INSTALL(TARGETS ${PROJECT_NAME}
        COMPONENT Applications
        RUNTIME DESTINATION ${install_dir}
        LIBRARY DESTINATION ${lib_install_dir}
        ARCHIVE DESTINATION ${lib_install_dir}
        BUNDLE DESTINATION "."
    )
endif()

