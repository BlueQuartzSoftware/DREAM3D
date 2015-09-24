#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2015 BlueQuartz Software, LLC
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////
PROJECT( DevHelper VERSION ${DREAM3DProj_VERSION_MAJOR}.${DREAM3DProj_VERSION_MINOR}.${DREAM3DProj_VERSION_PATCH})


# --------------------------------------------------------------------
# Look for Qt 5 as we need it for the plugin GUI to be generated
if(NOT Qt5Core_FOUND)
    message(FATAL_ERROR "Qt 5 is Needed for this project")
endif()


# Set the GUI type for different platforms
SET(GUI_TYPE )

# Set some Win32 Specific Settings
if(WIN32)
 SET(GUI_TYPE WIN32)
ENDif(WIN32)

add_definitions(-DDREAM3D_USE_QT)

# --------------------------------------------------------------------
# Generate the License File Headers
set(PROJECT_PREFIX "PluginMakerProj")
include(${DREAM3DProj_SOURCE_DIR}/License/License.cmake)

set(DevHelper_SRCS
  ${DevHelper_SOURCE_DIR}/main.cpp
  ${DevHelper_SOURCE_DIR}/PluginMaker.cpp
  ${DevHelper_SOURCE_DIR}/FilterMaker.cpp
  ${DevHelper_SOURCE_DIR}/DevHelper.cpp
  ${DevHelper_SOURCE_DIR}/AddFilterParameter.cpp
  ${DevHelper_SOURCE_DIR}/HelpWidget.cpp
  ${DevHelper_SOURCE_DIR}/PMDirGenerator.cpp
  ${DevHelper_SOURCE_DIR}/PMFileGenerator.cpp
  ${DevHelper_SOURCE_DIR}/AddFilterWidget.cpp
  ${DevHelper_SOURCE_DIR}/FilterBundler.cpp
)

SET(DevHelper_HDRS
  ${DevHelper_SOURCE_DIR}/FilterBundler.h
  ${DevHelper_SOURCE_DIR}/PMGeneratorTreeItem.h
)

SET(DevHelper_MOC_HDRS
  ${DevHelper_SOURCE_DIR}/PluginMaker.h
  ${DevHelper_SOURCE_DIR}/FilterMaker.h
  ${DevHelper_SOURCE_DIR}/DevHelper.h
  ${DevHelper_SOURCE_DIR}/AddFilterParameter.h
  ${DevHelper_SOURCE_DIR}/HelpWidget.h
  ${DevHelper_SOURCE_DIR}/PMDirGenerator.h
  ${DevHelper_SOURCE_DIR}/PMFileGenerator.h
  ${DevHelper_SOURCE_DIR}/AddFilterWidget.h
)

cmp_IDE_SOURCE_PROPERTIES( "DevHelper" "${DevHelper_HDRS};${DevHelper_MOC_HDRS}" "${DevHelper_SRCS}" "${PROJECT_INSTALL_HEADERS}")

set(DevHelper_CodeGenerator_HDRS
  ${DevHelper_SOURCE_DIR}/CodeGenerators/CodeGenFactory.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/AttributeMatrixCreationWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/AttributeMatrixSelectionWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/AxisAngleWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/BooleanWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/ChoiceWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/ComparisonSelectionWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/DataArrayCreationWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/DataArraySelectionWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/DataBundleSelectionWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/DataContainerArrayProxyWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/DataContainerReaderWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/DataContainerSelectionWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/DoubleWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/DynamicChoiceWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/DynamicTableWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/FileListInfoWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/FloatVec3WidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/FloatVec4WidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/FourthOrderPolynomialWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/FPCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/InputFileWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/InputPathWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/IntVec3WidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/IntWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/LinkedBooleanWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/MultiDataArraySelectionWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/OutputFileWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/OutputPathWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/PreflightUpdatedValueWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/SecondOrderPolynomialWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/SeparatorWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/StringWidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/Symmetric6x6WidgetCodeGenerator.h
  ${DevHelper_SOURCE_DIR}/CodeGenerators/ThirdOrderPolynomialWidgetCodeGenerator.h
  )

set(DevHelper_CodeGenerator_SRCS
  ${DevHelper_SOURCE_DIR}/CodeGenerators/CodeGenFactory.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/AttributeMatrixCreationWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/AttributeMatrixSelectionWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/AxisAngleWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/BooleanWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/ChoiceWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/ComparisonSelectionWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/DataArrayCreationWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/DataArraySelectionWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/DataBundleSelectionWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/DataContainerArrayProxyWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/DataContainerReaderWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/DataContainerSelectionWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/DoubleWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/DynamicChoiceWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/DynamicTableWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/FileListInfoWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/FloatVec3WidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/FloatVec4WidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/FourthOrderPolynomialWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/FPCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/InputFileWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/InputPathWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/IntVec3WidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/IntWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/LinkedBooleanWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/MultiDataArraySelectionWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/OutputFileWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/OutputPathWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/PreflightUpdatedValueWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/SecondOrderPolynomialWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/SeparatorWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/StringWidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/Symmetric6x6WidgetCodeGenerator.cpp
  ${DevHelper_SOURCE_DIR}/CodeGenerators/ThirdOrderPolynomialWidgetCodeGenerator.cpp
  )

cmp_IDE_SOURCE_PROPERTIES( "DevHelper/CodeGenerators" "${DevHelper_CodeGenerator_HDRS}" "${DevHelper_CodeGenerator_SRCS}" "${PROJECT_INSTALL_HEADERS}")

# The UI files As generated by Qt Designer
SET(DevHelper_UIS
  ${DevHelper_SOURCE_DIR}/UI_Files/PluginMaker.ui
  ${DevHelper_SOURCE_DIR}/UI_Files/FilterMaker.ui
  ${DevHelper_SOURCE_DIR}/UI_Files/DevHelper.ui
  ${DevHelper_SOURCE_DIR}/UI_Files/AddFilterParameter.ui
  ${DevHelper_SOURCE_DIR}/UI_Files/HelpWidget.ui
  ${DevHelper_SOURCE_DIR}/UI_Files/AddFilterWidget.ui
#    ${DREAM3DProj_SOURCE_DIR}/Source/QtSupportLib/UI_Files/ApplicationAboutBoxDialog.ui
  )

cmp_IDE_GENERATED_PROPERTIES("DevHelper/UI_Files" "${DevHelper_UIS}" "")

# and finally any resource files
# SET(DevHelper_RCS ${DevHelper_SOURCE_DIR}/Templates.qrc)
SET(DevHelper_RCS ${PROJECT_RESOURCES_DIR}/icons/images/Icons.qrc)

# -- Run MOC and UIC on the necessary files
QT5_ADD_RESOURCES( DevHelper_Generated_RC_SRCS ${DevHelper_RCS} ${QRC_LICENSE_FILES})

# this will run uic on .ui files:
QT5_WRAP_UI( DevHelper_Generated_UI_HDRS ${DevHelper_UIS} )

# and finally this will run moc:
QT5_WRAP_CPP( DevHelper_Generated_MOC_SRCS ${DevHelper_MOC_HDRS} )
set_source_files_properties( ${DevHelper_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${DevHelper_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)


#-- Put the generated files into their own group for IDEs
cmp_IDE_GENERATED_PROPERTIES("Generated/ui" "${DevHelper_Generated_UI_HDRS}" "")
cmp_IDE_GENERATED_PROPERTIES("Generated/moc" "${DevHelper_Generated_MOC_SRCS}" "")
cmp_IDE_GENERATED_PROPERTIES("Generated/qrc" "${QRC_LICENSE_FILES}" "${DevHelper_Generated_RC_SRCS}")

# -- Finally set the Project Sources to include all the sources
set(${PROJECT_NAME}_PROJECT_SRCS
  ${DevHelper_SRCS}
  ${DevHelper_HDRS}
  ${DevHelper_MOC_HDRS}
  ${DevHelper_Generated_RC_SRCS}
  ${DevHelper_Generated_UI_HDRS}
  ${DevHelper_CodeGenerator_SRCS}
  ${DevHelper_CodeGenerator_HDRS}

  ${DevHelper_Generated_MOC_SRCS}
)

include_directories(${DevHelper_SOURCE_DIR} )
include_directories(${DevHelper_SOURCE_DIR}/.. )
include_directories(${DevHelper_BINARY_DIR} )
include_directories(${DREAM3DProj_BINARY_DIR}/QtSupportLib)
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/QtSupportLib)


if(APPLE)
    set(APPLICATION_ICON ${PROJECT_RESOURCES_DIR}/icons/icns/${PROJECT_NAME}.icns)
endif(APPLE)

if(WIN32)
    set(APPLICATION_ICON ${PROJECT_RESOURCES_DIR}/icons/ico/${PROJECT_NAME}.ico)
ENDif(WIN32)

set(DEST_DIR ".")
if(UNIX AND NOT APPLE)
    set(DEST_DIR "bin")
endif()

BuildQtAppBundle(
    TARGET ${PROJECT_NAME}
    SOURCES ${DevHelper_PROJECT_SRCS}
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    ICON_FILE  ${APPLICATION_ICON}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    LINK_LIBRARIES QtSupportLib
    LIB_SEARCH_DIRS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    QT5_MODULES   ${Qt5_COMPONENTS}
    COMPONENT     Applications
    INSTALL_DEST  ${DEST_DIR}
)


#-------------------------------------------------------------------------------
#- This copies all the Prebuilt Pipeline files into the Build directory so the help
#- works from the Build Tree
add_custom_target(TemplateFileCopy ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${DevHelper_SOURCE_DIR}/Template/
            ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Template/
            COMMENT "Copying PluginMakeTemplate into Binary Directory")
set_target_properties(TemplateFileCopy PROPERTIES FOLDER ZZ_COPY_FILES)

# Make Sure this target is built so that the DREAM3D Application has access to
# the HTML help files
#add_dependencies(${PROJECT_NAME} TemplateFileCopy)

#- This installs all the help files. Note that we assume a flat directory structure
#- in the "Help" folder, ie, all html files, images, etc are all in the "Help"
#- folder and NOT in any subfolders beyond that.
install(DIRECTORY ${DevHelper_SOURCE_DIR}/Template
        DESTINATION .
        COMPONENT Applications )
