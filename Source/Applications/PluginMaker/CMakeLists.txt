project(PluginMaker)
cmake_minimum_required(VERSION 3.1.0)


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

set(PluginMaker_SRCS
  ${PluginMaker_SOURCE_DIR}/main.cpp
  ${PluginMaker_SOURCE_DIR}/PluginMaker.cpp
  ${PluginMaker_SOURCE_DIR}/FilterMaker.cpp
  ${PluginMaker_SOURCE_DIR}/DevHelper.cpp
  ${PluginMaker_SOURCE_DIR}/AddFilterParameter.cpp
  ${PluginMaker_SOURCE_DIR}/HelpWidget.cpp
  ${PluginMaker_SOURCE_DIR}/PMDirGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/PMFileGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/AddFilterWidget.cpp
  ${PluginMaker_SOURCE_DIR}/FilterBundler.cpp
  )

SET(PluginMaker_HDRS
    ${PluginMaker_SOURCE_DIR}/FilterBundler.h
    ${PluginMaker_SOURCE_DIR}/PMGeneratorTreeItem.h
  )

SET(PluginMaker_MOC_HDRS
    ${PluginMaker_SOURCE_DIR}/PluginMaker.h
    ${PluginMaker_SOURCE_DIR}/FilterMaker.h
    ${PluginMaker_SOURCE_DIR}/DevHelper.h
    ${PluginMaker_SOURCE_DIR}/AddFilterParameter.h
    ${PluginMaker_SOURCE_DIR}/HelpWidget.h
    ${PluginMaker_SOURCE_DIR}/PMDirGenerator.h
    ${PluginMaker_SOURCE_DIR}/PMFileGenerator.h
  ${PluginMaker_SOURCE_DIR}/AddFilterWidget.h
  )

set(PluginMaker_CodeGenerator_HDRS
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/CodeGenFactory.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/AttributeMatrixSelectionWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/AxisAngleWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/BooleanWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/ChoiceWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/ComparisonSelectionWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/DataArrayCreationWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/DataArraySelectionWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/DataBundleSelectionWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/DataContainerArrayProxyWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/DataContainerReaderWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/DataContainerSelectionWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/DoubleWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/DynamicChoiceWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/DynamicTableWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/FileListInfoWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/FilterParameterWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/FloatVec3WidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/FloatVec4WidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/FourthOrderPolynomialWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/FPCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/InputFileWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/InputPathWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/IntVec3WidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/IntWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/LinkedBooleanWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/MultiDataArraySelectionWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/OutputFileWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/OutputPathWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/PhaseTypeSelectionWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/PreflightUpdatedValueWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/SecondOrderPolynomialWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/SeparatorWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/ShapeTypeSelectionWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/StringWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/Symmetric6x6WidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/ThirdOrderPolynomialWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/UnknownWidgetCodeGenerator.h
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/VolumeDataContainerInfoWidgetCodeGenerator.h
  )

set(PluginMaker_CodeGenerator_SRCS
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/CodeGenFactory.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/AttributeMatrixSelectionWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/AxisAngleWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/BooleanWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/ChoiceWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/ComparisonSelectionWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/DataArrayCreationWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/DataArraySelectionWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/DataBundleSelectionWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/DataContainerArrayProxyWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/DataContainerReaderWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/DataContainerSelectionWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/DoubleWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/DynamicChoiceWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/DynamicTableWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/FileListInfoWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/FilterParameterWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/FloatVec3WidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/FloatVec4WidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/FourthOrderPolynomialWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/FPCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/InputFileWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/InputPathWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/IntVec3WidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/IntWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/LinkedBooleanWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/MultiDataArraySelectionWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/OutputFileWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/OutputPathWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/PhaseTypeSelectionWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/PreflightUpdatedValueWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/SecondOrderPolynomialWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/SeparatorWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/ShapeTypeSelectionWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/StringWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/Symmetric6x6WidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/ThirdOrderPolynomialWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/UnknownWidgetCodeGenerator.cpp
  ${PluginMaker_SOURCE_DIR}/CodeGenerators/VolumeDataContainerInfoWidgetCodeGenerator.cpp
  )

cmp_IDE_SOURCE_PROPERTIES( "PluginMaker" "${PluginMaker_HDRS};${PluginMaker_MOC_HDRS};${PluginMaker_CodeGenerator_HDRS}" "${PluginMaker_SRCS};${PluginMaker_CodeGenerator_SRCS}" "${PROJECT_INSTALL_HEADERS}")

# The UI files As generated by Qt Designer
SET(PluginMaker_UIS
  ${PluginMaker_SOURCE_DIR}/UI_Files/PluginMaker.ui
  ${PluginMaker_SOURCE_DIR}/UI_Files/FilterMaker.ui
  ${PluginMaker_SOURCE_DIR}/UI_Files/DevHelper.ui
  ${PluginMaker_SOURCE_DIR}/UI_Files/AddFilterParameter.ui
  ${PluginMaker_SOURCE_DIR}/UI_Files/HelpWidget.ui
  ${PluginMaker_SOURCE_DIR}/UI_Files/AddFilterWidget.ui
#    ${DREAM3DProj_SOURCE_DIR}/Source/QtSupport/UI_Files/ApplicationAboutBoxDialog.ui
  )

cmp_IDE_GENERATED_PROPERTIES("PluginMaker/UI_Files" "${PluginMaker_UIS}" "")

# and finally any resource files
# SET(PluginMaker_RCS ${PluginMaker_SOURCE_DIR}/Templates.qrc)
SET(PluginMaker_RCS "")

# -- Run MOC and UIC on the necessary files
QT5_ADD_RESOURCES( PluginMaker_Generated_RC_SRCS ${PluginMaker_RCS} ${QRC_LICENSE_FILES})

# this will run uic on .ui files:
QT5_WRAP_UI( PluginMaker_Generated_UI_HDRS ${PluginMaker_UIS} )

# and finally this will run moc:
#QT5_WRAP_CPP( PluginMaker_Generated_MOC_SRCS ${PluginMaker_MOC_HDRS} )

#-- Put the generated files into their own group for IDEs
cmp_IDE_GENERATED_PROPERTIES("Generated/ui" "${PluginMaker_Generated_UI_HDRS}" "")
cmp_IDE_GENERATED_PROPERTIES("Generated/moc" "${PluginMaker_Generated_MOC_SRCS}" "")
cmp_IDE_GENERATED_PROPERTIES("Generated/qrc" "${QRC_LICENSE_FILES}" "${PluginMaker_Generated_RC_SRCS}")

# -- Finally set the Project Sources to include all the sources
set(${PROJECT_NAME}_PROJECT_SRCS
  ${PluginMaker_SRCS}
    ${PluginMaker_HDRS}
  ${PluginMaker_MOC_HDRS}
    ${PluginMaker_Generated_RC_SRCS}
    ${PluginMaker_Generated_UI_HDRS}
    ${PluginMaker_CodeGenerator_SRCS}
    ${PluginMaker_CodeGenerator_HDRS}
#    ${PluginMaker_Generated_MOC_SRCS}
  )
#message(STATUS "${PROJECT_NAME}_PROJECT_SRCS: ${${PROJECT_NAME}_PROJECT_SRCS}")
include_directories(${PluginMaker_SOURCE_DIR} )
include_directories(${PluginMaker_SOURCE_DIR}/.. )
include_directories(${PluginMaker_BINARY_DIR} )
include_directories(${DREAM3DProj_BINARY_DIR}/QtSupport)
include_directories(${DREAM3DProj_SOURCE_DIR}/Source/QtSupport)


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
    SOURCES ${PluginMaker_PROJECT_SRCS}
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    ICON_FILE  ${APPLICATION_ICON}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    LINK_LIBRARIES QtSupport
    LIB_SEARCH_DIRS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    QT5_MODULES   ${Qt5_COMPONENTS}
    COMPONENT     Applications
    INSTALL_DEST  ${DEST_DIR}
)


#-------------------------------------------------------------------------------
#- This copies all the Prebuilt Pipeline files into the Build directory so the help
#- works from the Build Tree
add_custom_target(TemplateFileCopy ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${PluginMaker_SOURCE_DIR}/Template/
            ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Template/
            COMMENT "Copying PluginMakeTemplate into Binary Directory")
set_target_properties(TemplateFileCopy PROPERTIES FOLDER ZZ_COPY_FILES)

# Make Sure this target is built so that the DREAM3D Application has access to
# the HTML help files
#add_dependencies(${PROJECT_NAME} TemplateFileCopy)

#- This installs all the help files. Note that we assume a flat directory structure
#- in the "Help" folder, ie, all html files, images, etc are all in the "Help"
#- folder and NOT in any subfolders beyond that.
install(DIRECTORY ${PluginMaker_SOURCE_DIR}/Template
        DESTINATION .
        COMPONENT Applications )
