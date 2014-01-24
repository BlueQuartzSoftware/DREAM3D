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

project(FilterWidgetsLib)
cmake_minimum_required(VERSION 2.8.9)
set(FilterWidgetsLib_BUILT_AS_DYNAMIC_LIB)
if(BUILD_SHARED_LIBS)
    set(FilterWidgetsLib_BUILT_AS_DYNAMIC_LIB 1)
    if(WIN32)
        ADD_DEFINITIONS("-DFilterWidgetsLib_BUILT_AS_DYNAMIC_LIB")
    endif(WIN32)
endif(BUILD_SHARED_LIBS)

set(GENERATE_GIT_VERSION 1)




if(NOT PROJECT_RESOURCES_DIR)
set(PROJECT_RESOURCES_DIR ${FilterWidgetsLib_SOURCE_DIR}/Resources)
endif()
if(NOT DEFINED CMP_SOURCE_DIR)
set(CMP_SOURCE_DIR ${FilterWidgetsLib_SOURCE_DIR}/Support/cmp)
endif()

set(PROJECT_PREFIX "Ebsd" CACHE STRING "The Prefix to be used for Preprocessor definitions")
set(FilterWidgetsLib_INSTALL_FILES 0)
#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the
# project.
#-------------------------------------------------------------------------------
set(CMP_HEADER_DIR ${FilterWidgetsLib_BINARY_DIR})
set(CMP_CONFIGURATION_FILE_NAME "FilterWidgetsLibConfiguration.h")
set(CMP_TYPES_FILE_NAME "FilterWidgetsLibTypes.h")
set(CMP_VERSION_HEADER_FILE_NAME "FilterWidgetsLibVersion.h")
set(CMP_VERSION_SOURCE_FILE_NAME "FilterWidgetsLibVersion.cpp")
set(CMP_TOP_HEADER_FILE "FilterWidgetsLib.h")
set(CMP_PROJECT_NAMESPACE "${PROJECT_NAME}")
set(CMP_PROJECT_NAME "${PROJECT_NAME}")
set(CMP_ENABLE_PLUGINS "0")
set(CMP_LIB_SEARCH_DIRS "")
set(CMP_GENERATE_VERSION_STRING ${GENERATE_GIT_VERSION})

# Extra Variables that need to be set before all the configured files are generated.
set(FilterWidgetsLib_HDF5_SUPPORT 1)
option(FilterWidgetsLib_ENABLE_HDF5 "Enable the use of HDF5 functionality" ON)
mark_as_advanced(FilterWidgetsLib_ENABLE_HDF5)

include(${CMP_SOURCE_DIR}/cmpProject.cmake)

configure_file(${FilterWidgetsLib_SOURCE_DIR}/FilterWidgetsLibConfiguration.h.in
               ${FilterWidgetsLib_BINARY_DIR}/${CMP_TOP_HEADER_FILE})

# Setup the Include Directories
include_directories(${PROJECT_CODE_DIR})
include_directories(${FilterWidgetsLib_BINARY_DIR})



set(Project_SRCS "")

set(FilterWidgetsLib_Generated_HDRS
        ${FilterWidgetsLib_BINARY_DIR}/FilterWidgetsLibConfiguration.h
        ${FilterWidgetsLib_BINARY_DIR}/FilterWidgetsLibTypes.h
        ${FilterWidgetsLib_BINARY_DIR}/FilterWidgetsLibVersion.h
        ${FilterWidgetsLib_BINARY_DIR}/FilterWidgetsLib.h
        ${FilterWidgetsLib_BINARY_DIR}/${CMP_TOP_HEADER_FILE}
)

set(FilterWidgetsLib_Generated_SRCS
        ${FilterWidgetsLib_BINARY_DIR}/FilterWidgetsLibVersion.cpp
)

cmp_IDE_GENERATED_PROPERTIES("Generated" "${FilterWidgetsLib_Generated_HDRS}" "${FilterWidgetsLib_Generated_SRCS}")

set(FilterWidgetsLib_HDRS
  ${FilterWidgetsLib_SOURCE_DIR}/FilterWidgetManager.h
  ${FilterWidgetsLib_SOURCE_DIR}/IFilterWidgetFactory.h
  ${FilterWidgetsLib_SOURCE_DIR}/QFilterWidgetFactory.hpp
  ${FilterWidgetsLib_SOURCE_DIR}/FilterWidgetsLibDLLExport.h
)

set(FilterWidgetsLib_MOC_HDRS
)

set(FilterWidgetsLib_SRCS
  ${FilterWidgetsLib_SOURCE_DIR}/FilterWidgetManager.cpp
  ${FilterWidgetsLib_SOURCE_DIR}/IFilterWidgetFactory.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "FilterWidgets" "${FilterWidgetsLib_HDRS};${FilterWidgetsLib_MOC_HDRS}" "${FilterWidgetsLib_SRCS}" "0")

# --------------------------------------------------------------------
# and finally this will run moc:
QT4_WRAP_CPP( FilterWidgetsLib_Generated_MOC_SRCS ${FilterWidgetsLib_MOC_HDRS} )

# These generated moc files will be #include in the FilterWidget source file that
# are generated so we need to tell the build system to NOT compile these files
set_source_files_properties( ${FilterWidgetsLib_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)


# -- Run MOC and UIC on the necessary files
# QT4_ADD_RESOURCES( FilterWidgetsLib_Generated_RC_SRCS "${DREAM3DProj_SOURCE_DIR}/Documentation/Filters/Generated_FilterDocs.qrc"  )

# --------------------------------------------------------------------
# Continue on with our Qt4 section
QT4_WRAP_UI( FilterWidgetsLib_Generated_UI_HDRS   )

# --------------------------------------------------------------------
#-- Put the Qt generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Moc" "" "${FilterWidgetsLib_Generated_MOC_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${FilterWidgetsLib_Generated_UI_HDRS}" "" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Qrc" "${FilterWidgetsLib_Generated_RC_SRCS}" "" "0")


# --------------------------------------------------------------------
#-- Include all the generic Widgets that can be used to build up a GUI
include(${FilterWidgetsLib_SOURCE_DIR}/Widgets/SourceList.cmake)

# --------------------------------------------------------------------
# Include all the Filter Parameter Widgets
include(${FilterWidgetsLib_SOURCE_DIR}/FilterParameterWidgets/SourceList.cmake)


set(Project_SRCS
        ${FilterWidgetsLib_HDRS}
        ${FilterWidgetsLib_MOC_HDRS}
        ${FilterWidgetsLib_SRCS}
        ${FilterWidgetsLib_Generated_HDRS}
        ${FilterWidgetsLib_Generated_SRCS}
        ${FilterWidgetsLib_Generated_MOC_SRCS}
        ${FilterWidgetsLib_Generated_UI_HDRS}
        ${FilterWidgetsLib_Generated_RC_SRCS}
#---------------
        ${FilterWidgetsLib_Widgets_HDRS}
        ${FilterWidgetsLib_Widgets_SRCS}
        ${FilterWidgetsLib_Widgets_UIS}
        ${FilterWidgetsLib_Widgets_Generated_MOC_SRCS}
        ${FilterWidgetsLib_Widgets_Generated_UI_HDRS}
#---------------
        ${FilterWidgetsLib_ParameterWidgets_HDRS}
        ${FilterWidgetsLib_ParameterWidgets_SRCS}
        ${FilterWidgetsLib_ParameterWidgets_UIS}
        ${FilterWidgetsLib_ParameterWidgets_Generated_MOC_SRCS}
        ${FilterWidgetsLib_ParameterWidgets_Generated_UI_HDRS}
)


# --------------------------------------------------------------------
# Set some Include directories
include_directories(${FilterWidgetsLib_SOURCE_DIR}/..)
include_directories(${FilterWidgetsLib_BINARY_DIR})


if(UNIX AND NOT APPLE)
    set_source_files_properties(${Project_SRCS} PROPERTIES COMPILE_FLAGS -fPIC)
endif()

add_library(FilterWidgetsLib ${LIB_TYPE} ${Project_SRCS})
target_link_libraries(FilterWidgetsLib
                    ${QT_QTCORE_LIBRARY}
                    ${QT_QTGUI_LIBRARY}
                    QtSupport
                    DREAM3DLib
)

LibraryProperties( FilterWidgetsLib ${EXE_DEBUG_EXTENSION})



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

