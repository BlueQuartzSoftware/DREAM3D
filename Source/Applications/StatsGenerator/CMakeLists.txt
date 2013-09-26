#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2009, 2010 Michael A. Jackson. BlueQuartz Software
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////
project(StatsGenerator)
cmake_minimum_required(VERSION 2.8.9)

# ------------------------------------------------------------------------------
#  Qt 4 Section
# ------------------------------------------------------------------------------
if(QT4_FOUND)
    set(QT_USE_QTSVG "TRUE")
    set(QT_USE_QTSCRIPT "TRUE")
    include( ${QT_USE_FILE} )
    CMP_COPY_QT4_RUNTIME_LIBRARIES( "QtSvg;QtScript")
    CMP_QT_LIBRARIES_INSTALL_RULES( "QtSvg;QtScript" "./")
else()
    message(FATAL_ERROR "Qt 4 is Needed for this project.")
endif(QT4_FOUND)

# ------------------------------------------------------------------------------
#  Qwt Section
# ------------------------------------------------------------------------------
find_package(Qwt)
if(QWT_FOUND)
    include_directories(${QWT_INCLUDE_DIR})
    include_directories(${QWT_INCLUDE_DIR}/qwt)
    CMP_COPY_DEPENDENT_LIBRARIES("qwt")
    CMP_LIBRARIES_INSTALL_RULES("qwt" "./")
else()
    message(FATAL_ERROR "Qwt is required for this project")
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
set(PROJECT_PREFIX "StatsGenerator")
include(${DREAM3DProj_SOURCE_DIR}/License/License.cmake)

set(StatsGenerator_SRCS
    ${StatsGenerator_SOURCE_DIR}/main.cpp
    ${StatsGenerator_SOURCE_DIR}/SGApplication.cpp
    ${StatsGenerator_SOURCE_DIR}/StatsGeneratorUI.cpp
    ${StatsGenerator_SOURCE_DIR}/StatsGenPlotWidget.cpp
    ${StatsGenerator_SOURCE_DIR}/StatsGenODFWidget.cpp
    ${StatsGenerator_SOURCE_DIR}/SGAxisODFWidget.cpp
    ${StatsGenerator_SOURCE_DIR}/StatsGenMDFWidget.cpp
    ${StatsGenerator_SOURCE_DIR}/SGWidget.cpp
    ${StatsGenerator_SOURCE_DIR}/EditPhaseDialog.cpp
    ${StatsGenerator_SOURCE_DIR}/TextureDialog.cpp
    ${StatsGenerator_SOURCE_DIR}/PrimaryPhaseWidget.cpp
    ${StatsGenerator_SOURCE_DIR}/PrecipitatePhaseWidget.cpp
    ${StatsGenerator_SOURCE_DIR}/MatrixPhaseWidget.cpp
    ${StatsGenerator_SOURCE_DIR}/BoundaryPhaseWidget.cpp
    ${StatsGenerator_SOURCE_DIR}/TransformationPhaseWidget.cpp
    )
set(StatsGenerator_HDRS
#    ${StatsGenerator_SOURCE_DIR}/StatsGen.h
)

SET(StatsGenerator_MOC_HDRS
    ${StatsGenerator_SOURCE_DIR}/SGApplication.h
    ${StatsGenerator_SOURCE_DIR}/StatsGeneratorUI.h
    ${StatsGenerator_SOURCE_DIR}/StatsGenPlotWidget.h
    ${StatsGenerator_SOURCE_DIR}/StatsGenODFWidget.h
    ${StatsGenerator_SOURCE_DIR}/SGAxisODFWidget.h
    ${StatsGenerator_SOURCE_DIR}/StatsGenMDFWidget.h
    ${StatsGenerator_SOURCE_DIR}/SGWidget.h
    ${StatsGenerator_SOURCE_DIR}/EditPhaseDialog.h
    ${StatsGenerator_SOURCE_DIR}/TextureDialog.h
    ${StatsGenerator_SOURCE_DIR}/PrimaryPhaseWidget.h
    ${StatsGenerator_SOURCE_DIR}/PrecipitatePhaseWidget.h
    ${StatsGenerator_SOURCE_DIR}/MatrixPhaseWidget.h
    ${StatsGenerator_SOURCE_DIR}/BoundaryPhaseWidget.h
    ${StatsGenerator_SOURCE_DIR}/TransformationPhaseWidget.h
    )
cmp_IDE_SOURCE_PROPERTIES( "StatsGenerator" "${StatsGenerator_HDRS};${StatsGenerator_MOC_HDRS}" "${StatsGenerator_SRCS}" ${PROJECT_INSTALL_HEADERS})

set(StatsGenerator_DELEGATE_SRCS   "" )
set(StatsGenerator_DELEGATE_HDRS
    ${StatsGenerator_SOURCE_DIR}/Delegates/SGBetaItemDelegate.h
    ${StatsGenerator_SOURCE_DIR}/Delegates/SGLogNormalItemDelegate.h
    ${StatsGenerator_SOURCE_DIR}/Delegates/SGPowerLawItemDelegate.h
    ${StatsGenerator_SOURCE_DIR}/Delegates/SGODFItemDelegate.h
    ${StatsGenerator_SOURCE_DIR}/Delegates/SGMDFItemDelegate.h
    )
cmp_IDE_SOURCE_PROPERTIES( "StatsGenerator/Delegates" "${StatsGenerator_DELEGATE_HDRS}" "${StatsGenerator_DELEGATE_SRCS}" ${PROJECT_INSTALL_HEADERS})

set(StatsGenerator_TABLEMODEL_SRCS
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGAbstractTableModel.cpp
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGBetaTableModel.cpp
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGPowerLawTableModel.cpp
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGLogNormalTableModel.cpp
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGODFTableModel.cpp
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGMDFTableModel.cpp
    )

set(StatsGenerator_TABLEMODEL_HDRS
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGAbstractTableModel.h
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGBetaTableModel.h
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGPowerLawTableModel.h
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGLogNormalTableModel.h
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGODFTableModel.h
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGMDFTableModel.h
    )
cmp_IDE_SOURCE_PROPERTIES( "StatsGenerator/TableModels" "${StatsGenerator_TABLEMODEL_HDRS}" "${StatsGenerator_TABLEMODEL_SRCS}" ${PROJECT_INSTALL_HEADERS})


set(StatsGenerator_PRESETS_SRCS
    ${StatsGenerator_SOURCE_DIR}/Presets/MicrostructurePresetManager.cpp
    ${StatsGenerator_SOURCE_DIR}/Presets/AbstractMicrostructurePreset.cpp
    ${StatsGenerator_SOURCE_DIR}/Presets/EquiaxedPreset.cpp
    ${StatsGenerator_SOURCE_DIR}/Presets/DefaultStatsPreset.cpp
    ${StatsGenerator_SOURCE_DIR}/Presets/RecrystallizedPreset.cpp
    ${StatsGenerator_SOURCE_DIR}/Presets/RolledPreset.cpp
)
set(StatsGenerator_PRESETS_HDRS
    ${StatsGenerator_SOURCE_DIR}/Presets/AbstractMicrostructurePresetFactory.h
    ${StatsGenerator_SOURCE_DIR}/Presets/AbstractMicrostructurePreset.h
    ${StatsGenerator_SOURCE_DIR}/Presets/MicrostructurePresetManager.h
    ${StatsGenerator_SOURCE_DIR}/Presets/EquiaxedPreset.h
    ${StatsGenerator_SOURCE_DIR}/Presets/DefaultStatsPreset.h
    ${StatsGenerator_SOURCE_DIR}/Presets/RecrystallizedPreset.h
    ${StatsGenerator_SOURCE_DIR}/Presets/RolledPreset.h
)
cmp_IDE_SOURCE_PROPERTIES( "StatsGenerator/Presets" "${StatsGenerator_PRESETS_HDRS}" "${StatsGenerator_PRESETS_SRCS}" ${PROJECT_INSTALL_HEADERS})

set(StatsGenerator_DIALOGS_SRCS
${StatsGenerator_SOURCE_DIR}/Presets/Dialogs/RolledPresetDialog.cpp
${StatsGenerator_SOURCE_DIR}/Presets/Dialogs/RecrystallizedPresetDialog.cpp
)
set(StatsGenerator_DIALOGS_HDRS
${StatsGenerator_SOURCE_DIR}/Presets/Dialogs/RolledPresetDialog.h
${StatsGenerator_SOURCE_DIR}/Presets/Dialogs/RecrystallizedPresetDialog.h
)
cmp_IDE_SOURCE_PROPERTIES( "StatsGenerator/Presets/Dialogs" "${StatsGenerator_DIALOGS_HDRS}" "${StatsGenerator_DIALOGS_SRCS}" ${PROJECT_INSTALL_HEADERS})

# The UI files As generated by Qt Designer
SET(StatsGenerator_UIS
  ${StatsGenerator_SOURCE_DIR}/UI_Files/StatsGeneratorUI.ui
  ${StatsGenerator_SOURCE_DIR}/UI_Files/StatsGenPlotWidget.ui
  ${StatsGenerator_SOURCE_DIR}/UI_Files/SGAxisODFWidget.ui
  ${StatsGenerator_SOURCE_DIR}/UI_Files/StatsGenMDFWidget.ui
  ${StatsGenerator_SOURCE_DIR}/UI_Files/EditPhaseDialog.ui
  ${StatsGenerator_SOURCE_DIR}/UI_Files/TextureDialog.ui
  ${StatsGenerator_SOURCE_DIR}/UI_Files/PrimaryPhaseWidget.ui
  ${StatsGenerator_SOURCE_DIR}/UI_Files/PrecipitatePhaseWidget.ui
  ${StatsGenerator_SOURCE_DIR}/UI_Files/MatrixPhaseWidget.ui
  ${StatsGenerator_SOURCE_DIR}/UI_Files/BoundaryPhaseWidget.ui
  ${StatsGenerator_SOURCE_DIR}/UI_Files/TransformationPhaseWidget.ui
)
cmp_IDE_GENERATED_PROPERTIES("StatsGenerator/UI_Files" "${StatsGenerator_UIS}" "")

# and finally any resource files
SET(StatsGenerator_RCS ${PROJECT_RESOURCES_DIR}/icons/images/Icons.qrc)

# -- Run MOC and UIC on the necessary files
#message(STATUS "QRC_LICENSE_FILES:")
#foreach(lic ${QRC_LICENSE_FILES})
#  message(STATUS "  ${lic}")
#endforeach(lic ${QRC_LICENSE_FILES})
QT4_ADD_RESOURCES( Generated_RC_SRCS ${StatsGenerator_RCS} ${QRC_LICENSE_FILES})

# this will run uic on .ui files:
QT4_WRAP_UI( Generated_UI_HDRS ${StatsGenerator_UIS} )

# and finally this will run moc:
QT4_WRAP_CPP( Generated_MOC_SRCS
            ${StatsGenerator_MOC_HDRS}
            ${StatsGenerator_DELEGATE_HDRS}
            ${StatsGenerator_TABLEMODEL_HDRS}
            ${StatsGenerator_DIALOGS_HDRS} )

#-- Put the generated files into their own group for IDEs
cmp_IDE_GENERATED_PROPERTIES("Generated/ui" "${Generated_UI_HDRS}" "")
cmp_IDE_GENERATED_PROPERTIES("Generated/moc" "${Generated_MOC_SRCS}" "")
cmp_IDE_GENERATED_PROPERTIES("Generated/qrc" "${QRC_LICENSE_FILES}" "${Generated_RC_SRCS}")

# -- Finally set the Project Sources to include all the sources
set(StatsGenerator_PROJECT_SRCS
    ${StatsGenerator_SRCS}
    ${StatsGenerator_HDRS}
    ${StatsGenerator_MOC_HDRS}
    ${StatsGenerator_DELEGATE_HDRS}
    ${StatsGenerator_DELEGATE_SRCS}
    ${StatsGenerator_TABLEMODEL_HDRS}
    ${StatsGenerator_TABLEMODEL_SRCS}
    ${StatsGenerator_PRESETS_HDRS}
    ${StatsGenerator_PRESETS_SRCS}
    ${StatsGenerator_DIALOGS_SRCS}
    ${StatsGenerator_DIALOGS_HDRS}
    ${Generated_RC_SRCS}
    ${Generated_UI_HDRS}
    ${Generated_MOC_SRCS}
)


include_directories(${PROJECT_CODE_DIR})
include_directories(${HDF5_INCLUDE_DIR})
include_directories(${StatsGenerator_SOURCE_DIR} )
include_directories(${StatsGenerator_SOURCE_DIR}/.. )
include_directories(${StatsGenerator_BINARY_DIR} )
include_directories(${DREAM3DProj_BINARY_DIR}/QtSupport)


if(APPLE)
    set(APPLICATION_ICON ${PROJECT_RESOURCES_DIR}/icons/icns/${PROJECT_NAME}.icns)
endif(APPLE)

if(Q_WS_WIN)
    set(APPLICATION_ICON ${PROJECT_RESOURCES_DIR}/icons/ico/${PROJECT_NAME}.ico)
ENDif(Q_WS_WIN)

set(DEST_DIR ".")
if(UNIX AND NOT APPLE)
    set(DEST_DIR "bin")
endif()

BuildQtAppBundle(
    TARGET ${PROJECT_NAME}
    SOURCES ${${PROJECT_NAME}_PROJECT_SRCS}
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    ICON_FILE  ${APPLICATION_ICON}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${PROJECT_NAME}_BINARY_DIR}
    LINK_LIBRARIES  H5Support EbsdLib DREAM3DLib ${QT_LIBRARIES} QtSupport ${QWT_LIBRARY}
    LIB_SEARCH_DIRS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
#    QT_PLUGINS    ${QT_PLUGINS}
#    OTHER_PLUGINS ${OTHER_PLUGINS}
    COMPONENT     Applications
    INSTALL_DEST  ${DEST_DIR}
)
