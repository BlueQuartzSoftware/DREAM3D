#///////////////////////////////////////////////////////////////////////////////
#  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
#  All rights reserved.
#  BSD License: http://www.opensource.org/licenses/bsd-license.html
#///////////////////////////////////////////////////////////////////////////////
project(QtSupportLib VERSION ${DREAM3DProj_VERSION_MAJOR}.${DREAM3DProj_VERSION_MINOR})


if(NOT PROJECT_RESOURCES_DIR)
  set(PROJECT_RESOURCES_DIR ${QtSupportLib_SOURCE_DIR}/Resources)
endif()
if(NOT DEFINED CMP_SOURCE_DIR)
  set(CMP_SOURCE_DIR ${QtSupportLib_SOURCE_DIR}/Support/cmp)
endif()

# Bring in our Macros from the CMP project
INCLUDE (${CMP_SOURCE_DIR}/cmpCMakeMacros.cmake )


set(QtSupportLib_BUILT_AS_DYNAMIC_LIB 0)
if(BUILD_SHARED_LIBS)
  set(QtSupportLib_BUILT_AS_DYNAMIC_LIB 1)
  if(WIN32)
    ADD_DEFINITIONS("-DQtSupportLib_BUILT_AS_DYNAMIC_LIB")
  endif(WIN32)
endif(BUILD_SHARED_LIBS)


if(NOT Qt5Core_FOUND)
    message(FATAL_ERROR "Qt 5 is Needed for this project.")
endif()

set(QtSupportLib_HDF5_SUPPORT "1")
option(QtSupportLib_ENABLE_HDF5 "Enable HDF5 Support in the QtSupportLib" ON)
mark_as_advanced(QtSupportLib_HDF5_SUPPORT)
mark_as_advanced(QtSupportLib_ENABLE_HDF5)


set(PROJECT_PREFIX "QtSupportLib" CACHE STRING "The Prefix to be used for Preprocessor definitions")
set(QtSupportLib_INSTALL_FILES 0)
#-------------------------------------------------------------------------------
# Include the cmp project to perform all the basic configuration tests for the
# project.
#-------------------------------------------------------------------------------
if(NOT DEFINED DREAM3DProj_BINARY_DIR)
  set(CMP_HEADER_DIR ${QtSupportLib_BINARY_DIR}/QtSupportLib)
else()
  set(CMP_HEADER_DIR ${QtSupportLib_BINARY_DIR}/QtSupportLib)
endif()
set(CMP_CONFIGURATION_FILE_NAME "QtSupportLibConfiguration.h")
set(CMP_TYPES_FILE_NAME "QtSupportLibTypes.h")
set(CMP_VERSION_HEADER_FILE_NAME "QtSupportLibVersion.h")
set(CMP_VERSION_SOURCE_FILE_NAME "QtSupportLibVersion.cpp")
set(CMP_TOP_HEADER_FILE "QtSupportLib.h")
set(CMP_PROJECT_NAMESPACE "${PROJECT_NAME}")
set(CMP_PROJECT_NAME "${PROJECT_NAME}")
set(CMP_ENABLE_PLUGINS "0")
set(CMP_LIB_SEARCH_DIRS "")
set(GENERATE_GIT_VERSION 1)
set(CMP_GENERATE_VERSION_STRING ${GENERATE_GIT_VERSION})

INCLUDE (${CMP_SOURCE_DIR}/cmpProject.cmake)

if(NOT DEFINED DREAM3DProj_BINARY_DIR)
  cmpConfigureFileWithMD5Check(CONFIGURED_TEMPLATE_PATH ${QtSupportLib_SOURCE_DIR}/QtSupportLibConfiguration.h.in
                            GENERATED_FILE_PATH ${CMP_HEADER_DIR}/${CMP_TOP_HEADER_FILE})
else()
  cmpConfigureFileWithMD5Check(CONFIGURED_TEMPLATE_PATH ${QtSupportLib_SOURCE_DIR}/QtSupportLibConfiguration.h.in
                            GENERATED_FILE_PATH ${QtSupportLib_BINARY_DIR}/${CMP_TOP_HEADER_FILE})
endif()

cmp_IDE_GENERATED_PROPERTIES("Generated"
                             "${QtSupportLib_BINARY_DIR}/${CMP_TOP_HEADER_FILE};${QtSupportLib_BINARY_DIR}/${PROJECT_NAME}/${CMP_CONFIGURATION_FILE_NAME};${QtSupportLib_BINARY_DIR}/${PROJECT_NAME}/${CMP_TYPES_FILE_NAME};${QtSupportLib_BINARY_DIR}/${PROJECT_NAME}/${CMP_VERSION_HEADER_FILE_NAME}"
                             "${QtSupportLib_BINARY_DIR}/${PROJECT_NAME}/${CMP_VERSION_SOURCE_FILE_NAME}")

# Setup the Include Directories
include_directories(${PROJECT_CODE_DIR})
include_directories(${QtSupportLib_BINARY_DIR})
include_directories(${QtSupportLib_SOURCE_DIR}/..)

set(Project_SRCS "")
set(Project_HDRS "")

set(QtSupportLib_SRCS
    ${QtSupportLib_SOURCE_DIR}/AIMImageGraphicsDelegate.cpp
    ${QtSupportLib_SOURCE_DIR}/SIMPLPluginFrame.cpp
    ${QtSupportLib_SOURCE_DIR}/ApplicationAboutBoxDialog.cpp
    ${QtSupportLib_SOURCE_DIR}/ColorComboPicker.cpp
    ${QtSupportLib_SOURCE_DIR}/QFileCompleter.cpp
    ${QtSupportLib_SOURCE_DIR}/QRecentFileList.cpp
    ${QtSupportLib_SOURCE_DIR}/QFSDropLineEdit.cpp
    ${QtSupportLib_SOURCE_DIR}/QDroppableScrollArea.cpp
    ${QtSupportLib_SOURCE_DIR}/QCheckboxDialog.cpp
    ${QtSupportLib_SOURCE_DIR}/HelpDialog.cpp
    ${QtSupportLib_SOURCE_DIR}/HoverButton.cpp
    ${QtSupportLib_SOURCE_DIR}/DistributionTypeWidget.cpp
    ${QtSupportLib_SOURCE_DIR}/DREAM3DHelpUrlGenerator.cpp
    ${QtSupportLib_SOURCE_DIR}/PoleFigureImageUtilities.cpp
    ${QtSupportLib_SOURCE_DIR}/PoleFigureData.cpp
    ${QtSupportLib_SOURCE_DIR}/DREAM3DGraphicsView.cpp
    ${QtSupportLib_SOURCE_DIR}/FaderWidget.cpp
    ${QtSupportLib_SOURCE_DIR}/SearchLineEdit.cpp
    ${QtSupportLib_SOURCE_DIR}/QFSDropLabel.cpp
    ${QtSupportLib_SOURCE_DIR}/FileDragMessageBox.cpp
    ${QtSupportLib_SOURCE_DIR}/DREAM3DStyles.cpp
    ${QtSupportLib_SOURCE_DIR}/ApplicationFileInfo.cpp
    ${QtSupportLib_SOURCE_DIR}/DREAM3DSplitter.cpp
    ${QtSupportLib_SOURCE_DIR}/DREAM3DSplitterHandle.cpp
    ${QtSupportLib_SOURCE_DIR}/FavoritesChangedDialog.cpp
    ${QtSupportLib_SOURCE_DIR}/DREAM3DSettings.cpp
    ${QtSupportLib_SOURCE_DIR}/BookmarkMissingDialog.cpp
)

set(QtSupportLib_HDRS
    ${QtSupportLib_SOURCE_DIR}/QtSupportLibDLLExport.h
    ${QtSupportLib_SOURCE_DIR}/AIMImageGraphicsDelegate.h
    ${QtSupportLib_SOURCE_DIR}/SIMPLPluginFrame.h
    ${QtSupportLib_SOURCE_DIR}/ApplicationAboutBoxDialog.h
    ${QtSupportLib_SOURCE_DIR}/ColorComboPicker.h
    ${QtSupportLib_SOURCE_DIR}/QFileCompleter.h
    ${QtSupportLib_SOURCE_DIR}/QRecentFileList.h
    ${QtSupportLib_SOURCE_DIR}/QFSDropLineEdit.h
    ${QtSupportLib_SOURCE_DIR}/QDroppableScrollArea.h
    ${QtSupportLib_SOURCE_DIR}/QCheckboxDialog.h
    ${QtSupportLib_SOURCE_DIR}/HelpDialog.h
    ${QtSupportLib_SOURCE_DIR}/HoverButton.h
    ${QtSupportLib_SOURCE_DIR}/PoleFigureImageUtilities.h
    ${QtSupportLib_SOURCE_DIR}/PoleFigureData.h
    ${QtSupportLib_SOURCE_DIR}/DREAM3DGraphicsView.h
    ${QtSupportLib_SOURCE_DIR}/FaderWidget.h
    ${QtSupportLib_SOURCE_DIR}/SearchLineEdit.h
    ${QtSupportLib_SOURCE_DIR}/QFSDropLabel.h
    ${QtSupportLib_SOURCE_DIR}/FileDragMessageBox.h
    ${QtSupportLib_SOURCE_DIR}/DREAM3DStyles.h
    ${QtSupportLib_SOURCE_DIR}/ApplicationFileInfo.h
    ${QtSupportLib_SOURCE_DIR}/DREAM3DSplitter.h
    ${QtSupportLib_SOURCE_DIR}/DREAM3DSplitterHandle.h
    ${QtSupportLib_SOURCE_DIR}/FavoritesChangedDialog.h
    ${QtSupportLib_SOURCE_DIR}/DREAM3DSettings.h
    ${QtSupportLib_SOURCE_DIR}/BookmarkMissingDialog.h
)

# --------------------------------------------------------------------
# These are headers that do NOT inherit from QObject and do NOT need MOC run on them
# --------------------------------------------------------------------
set(QtSupportLib_NO_MOC_HDRS
    ${QtSupportLib_SOURCE_DIR}/DREAM3DHelpUrlGenerator.h
)

cmp_IDE_SOURCE_PROPERTIES("QtSupportLib" "${QtSupportLib_HDRS};${QtSupportLib_NO_MOC_HDRS}" "${QtSupportLib_SRCS}" "0")

# --------------------------------------------------------------------
# The UI files As generated by Qt Designer
# --------------------------------------------------------------------
SET(DREAM3D_QT_UI_FILES
  ${QtSupportLib_SOURCE_DIR}/UI_Files/ApplicationAboutBoxDialog.ui
  ${QtSupportLib_SOURCE_DIR}/UI_Files/HelpDialog.ui
  ${QtSupportLib_SOURCE_DIR}/UI_Files/DistributionTypeWidget.ui
  ${QtSupportLib_SOURCE_DIR}/UI_Files/FileDragMessageBox.ui
  ${QtSupportLib_SOURCE_DIR}/UI_Files/FavoritesChangedDialog.ui
  ${QtSupportLib_SOURCE_DIR}/UI_Files/BookmarkMissingDialog.ui
)
cmp_IDE_SOURCE_PROPERTIES( "QtSupportLib/UI_Files" "${DREAM3D_QT_UI_FILES}" "" "0")

# and finally any resource files
#SET(QtSupportLib_RCS ${PROJECT_RESOURCES_DIR}/icons/images/Icons.qrc)

# --------------------------------------------------------------------
# Run on the files
#QT5_ADD_RESOURCES( QtSupportLib_Generated_RC_SRCS ${QCrossCorrelation_RCS} )

# --------------------------------------------------------------------
# this will run uic on .ui files:
QT5_WRAP_UI( QtSupportLib_Generated_UI_HDRS ${DREAM3D_QT_UI_FILES} )

# --------------------------------------------------------------------
# and finally this will run moc:
#QT5_WRAP_CPP( QtSupportLib_Generated_MOC_SRCS ${QtSupportLib_HDRS} )

# --------------------------------------------------------------------
#-- Put the generated files into their own group for IDEs
#cmp_IDE_SOURCE_PROPERTIES( "Generated" "${QtSupportLib_Generated_UI_HDRS}" "${QtSupportLib_Generated_MOC_SRCS};${QtSupportLib_Generated_RC_SRCS}" "0")

set(Project_SRCS ${Project_SRCS}
  ${QtSupportLib_SRCS}
  ${QtSupportLib_HDRS}
  ${QtSupportLib_NO_MOC_HDRS}
  ${QtSupportLib_Generated_UI_HDRS}
  ${QtSupportLib_Generated_MOC_SRCS}
  # -- Add in our Generated Version Files
  ${QtSupportLib_BINARY_DIR}/${CMP_VERSION_HEADER_FILE_NAME}
  ${QtSupportLib_BINARY_DIR}/${CMP_VERSION_SOURCE_FILE_NAME}
)

include_directories(${QtSupportLib_SOURCE_DIR}/..)
include_directories(${QtSupportLib_BINARY_DIR})
include_directories(${QtSupportLib_BINARY_DIR}/..)
add_definitions(-DQT_CORE_LIB)

if(UNIX AND NOT APPLE)
    set_source_files_properties(${Project_SRCS} PROPERTIES COMPILE_FLAGS -fPIC)
endif()

add_library(${PROJECT_NAME} ${LIB_TYPE} ${Project_SRCS})


set(QtSupportLib_LINK_LIBRARIES SIMPLib)
#if we are compiling HDF5 support, include those libraries
if(${QtSupportLib_ENABLE_HDF5})
  set(QtSupportLib_LINK_LIBRARIES
    ${QtSupportLib_LINK_LIBRARIES}
    hdf5 H5Support
    )
endif()

# Add in the Qt5 Libraries to the Link Library List
set(QtSupportLib_LINK_LIBRARIES
  ${QtSupportLib_LINK_LIBRARIES}
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  Qt5::Network
  )


target_link_libraries(${PROJECT_NAME} ${QtSupportLib_LINK_LIBRARIES})

LibraryProperties( ${PROJECT_NAME} ${EXE_DEBUG_EXTENSION} )

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



