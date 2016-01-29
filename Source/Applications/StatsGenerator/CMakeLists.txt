#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2015 BlueQuartz Software, LLC
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////
PROJECT( StatsGenerator VERSION ${DREAM3DProj_VERSION_MAJOR}.${DREAM3DProj_VERSION_MINOR}.${DREAM3DProj_VERSION_PATCH})

set(APPLICATION_NAME "StatsGenerator")

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
set(CMP_HEADER_DIR ${StatsGenerator_BINARY_DIR})
set(CMP_CONFIGURATION_FILE_NAME "StatsGeneratorConfiguration.h")
set(CMP_TYPES_FILE_NAME "StatsGeneratorTypes.h")
set(CMP_VERSION_HEADER_FILE_NAME "StatsGeneratorVersion.h")
set(CMP_VERSION_SOURCE_FILE_NAME "StatsGeneratorVersion.cpp")
set(CMP_TOP_HEADER_FILE "StatsGenerator.h")
set(CMP_PROJECT_NAMESPACE "${PROJECT_NAME}")
set(CMP_PROJECT_NAME "${PROJECT_NAME}")
set(CMP_ENABLE_PLUGINS "0")
set(CMP_LIB_SEARCH_DIRS "")
set(CMP_GENERATE_VERSION_STRING 1)
include(${CMP_SOURCE_DIR}/cmpProject.cmake)

cmpConfigureFileWithMD5Check(CONFIGURED_TEMPLATE_PATH ${StatsGenerator_SOURCE_DIR}/StatsGenerator.h.in
                            GENERATED_FILE_PATH ${StatsGenerator_BINARY_DIR}/${CMP_TOP_HEADER_FILE})

set(StatsGenerator_CMP_FILES
        ${CMP_HEADER_DIR}/${CMP_CONFIGURATION_FILE_NAME}
        ${CMP_HEADER_DIR}/${CMP_TYPES_FILE_NAME}
        ${CMP_HEADER_DIR}/${CMP_VERSION_HEADER_FILE_NAME}
        ${CMP_HEADER_DIR}/${CMP_VERSION_SOURCE_FILE_NAME}
        ${CMP_HEADER_DIR}/${CMP_TOP_HEADER_FILE}
)
cmp_IDE_GENERATED_PROPERTIES("StatsGenerator" "${StatsGenerator_CMP_FILES}" ${PROJECT_INSTALL_HEADERS})

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
    ${StatsGenerator_SOURCE_DIR}/StatsGenRDFWidget.cpp
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
  ${BrandedSIMPLView_DIR}/BrandedStrings.h
)

SET(StatsGenerator_MOC_HDRS
    ${StatsGenerator_SOURCE_DIR}/SGApplication.h
    ${StatsGenerator_SOURCE_DIR}/StatsGeneratorUI.h
    ${StatsGenerator_SOURCE_DIR}/StatsGenPlotWidget.h
    ${StatsGenerator_SOURCE_DIR}/StatsGenODFWidget.h
    ${StatsGenerator_SOURCE_DIR}/SGAxisODFWidget.h
    ${StatsGenerator_SOURCE_DIR}/StatsGenMDFWidget.h
    ${StatsGenerator_SOURCE_DIR}/StatsGenRDFWidget.h
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

set(StatsGenerator_DELEGATE_SRCS
    ${StatsGenerator_SOURCE_DIR}/Delegates/SGBetaItemDelegate.cpp
    ${StatsGenerator_SOURCE_DIR}/Delegates/SGLogNormalItemDelegate.cpp
    ${StatsGenerator_SOURCE_DIR}/Delegates/SGPowerLawItemDelegate.cpp
    ${StatsGenerator_SOURCE_DIR}/Delegates/SGODFItemDelegate.cpp
    ${StatsGenerator_SOURCE_DIR}/Delegates/SGMDFItemDelegate.cpp
    ${StatsGenerator_SOURCE_DIR}/Delegates/SGRDFItemDelegate.cpp
    )
set(StatsGenerator_DELEGATE_MOC_HDRS
    ${StatsGenerator_SOURCE_DIR}/Delegates/SGBetaItemDelegate.h
    ${StatsGenerator_SOURCE_DIR}/Delegates/SGLogNormalItemDelegate.h
    ${StatsGenerator_SOURCE_DIR}/Delegates/SGPowerLawItemDelegate.h
    ${StatsGenerator_SOURCE_DIR}/Delegates/SGODFItemDelegate.h
    ${StatsGenerator_SOURCE_DIR}/Delegates/SGMDFItemDelegate.h
    ${StatsGenerator_SOURCE_DIR}/Delegates/SGRDFItemDelegate.h
    )
cmp_IDE_SOURCE_PROPERTIES( "StatsGenerator/Delegates" "${StatsGenerator_DELEGATE_MOC_HDRS}" "${StatsGenerator_DELEGATE_SRCS}" ${PROJECT_INSTALL_HEADERS})

set(StatsGenerator_TABLEMODEL_SRCS
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGAbstractTableModel.cpp
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGBetaTableModel.cpp
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGPowerLawTableModel.cpp
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGLogNormalTableModel.cpp
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGODFTableModel.cpp
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGMDFTableModel.cpp
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGRDFTableModel.cpp
    )

set(StatsGenerator_TABLEMODEL_MOC_HDRS
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGAbstractTableModel.h
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGBetaTableModel.h
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGPowerLawTableModel.h
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGLogNormalTableModel.h
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGODFTableModel.h
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGMDFTableModel.h
    ${StatsGenerator_SOURCE_DIR}/TableModels/SGRDFTableModel.h
    )
cmp_IDE_SOURCE_PROPERTIES( "StatsGenerator/TableModels" "${StatsGenerator_TABLEMODEL_MOC_HDRS}" "${StatsGenerator_TABLEMODEL_SRCS}" ${PROJECT_INSTALL_HEADERS})


set(StatsGenerator_PRESETS_SRCS
    ${StatsGenerator_SOURCE_DIR}/Presets/MicrostructurePresetManager.cpp
    ${StatsGenerator_SOURCE_DIR}/Presets/AbstractMicrostructurePreset.cpp
    ${StatsGenerator_SOURCE_DIR}/Presets/PrimaryEquiaxedPreset.cpp
    ${StatsGenerator_SOURCE_DIR}/Presets/PrecipitateEquiaxedPreset.cpp
    ${StatsGenerator_SOURCE_DIR}/Presets/PrimaryRecrystallizedPreset.cpp
    ${StatsGenerator_SOURCE_DIR}/Presets/PrimaryRolledPreset.cpp
    ${StatsGenerator_SOURCE_DIR}/Presets/PrecipitateRolledPreset.cpp
)
set(StatsGenerator_PRESETS_MOC_HDRS "")
set(StatsGenerator_PRESETS_HDRS
    ${StatsGenerator_SOURCE_DIR}/Presets/AbstractMicrostructurePresetFactory.h
    ${StatsGenerator_SOURCE_DIR}/Presets/AbstractMicrostructurePreset.h
    ${StatsGenerator_SOURCE_DIR}/Presets/MicrostructurePresetManager.h
    ${StatsGenerator_SOURCE_DIR}/Presets/PrimaryEquiaxedPreset.h
    ${StatsGenerator_SOURCE_DIR}/Presets/PrecipitateEquiaxedPreset.h
    ${StatsGenerator_SOURCE_DIR}/Presets/PrimaryRecrystallizedPreset.h
    ${StatsGenerator_SOURCE_DIR}/Presets/PrimaryRolledPreset.h
    ${StatsGenerator_SOURCE_DIR}/Presets/PrecipitateRolledPreset.h
)
cmp_IDE_SOURCE_PROPERTIES( "StatsGenerator/Presets" "${StatsGenerator_PRESETS_HDRS};${StatsGenerator_PRESETS_MOC_HDRS}" "${StatsGenerator_PRESETS_SRCS}" ${PROJECT_INSTALL_HEADERS})

set(StatsGenerator_DIALOGS_SRCS
  ${StatsGenerator_SOURCE_DIR}/Presets/Dialogs/PrimaryRolledPresetDialog.cpp
  ${StatsGenerator_SOURCE_DIR}/Presets/Dialogs/PrecipitateRolledPresetDialog.cpp
  ${StatsGenerator_SOURCE_DIR}/Presets/Dialogs/PrimaryRecrystallizedPresetDialog.cpp
)
set(StatsGenerator_DIALOGS_MOC_HDRS
  ${StatsGenerator_SOURCE_DIR}/Presets/Dialogs/PrimaryRolledPresetDialog.h
  ${StatsGenerator_SOURCE_DIR}/Presets/Dialogs/PrecipitateRolledPresetDialog.h
  ${StatsGenerator_SOURCE_DIR}/Presets/Dialogs/PrimaryRecrystallizedPresetDialog.h
)
cmp_IDE_SOURCE_PROPERTIES( "StatsGenerator/Presets/Dialogs" "${StatsGenerator_DIALOGS_MOC_HDRS}" "${StatsGenerator_DIALOGS_SRCS}" ${PROJECT_INSTALL_HEADERS})

# The UI files As generated by Qt Designer
SET(StatsGenerator_UIS
  ${StatsGenerator_SOURCE_DIR}/UI_Files/StatsGeneratorUI.ui
  ${StatsGenerator_SOURCE_DIR}/UI_Files/StatsGenPlotWidget.ui
  ${StatsGenerator_SOURCE_DIR}/UI_Files/SGAxisODFWidget.ui
  ${StatsGenerator_SOURCE_DIR}/UI_Files/StatsGenMDFWidget.ui
  ${StatsGenerator_SOURCE_DIR}/UI_Files/StatsGenRDFWidget.ui
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

QT5_ADD_RESOURCES( Generated_RC_SRCS ${StatsGenerator_RCS} ${QRC_LICENSE_FILES})

# this will run uic on .ui files:
QT5_WRAP_UI( Generated_UI_HDRS ${StatsGenerator_UIS} )

# --------------------------------------------------------------------
# and finally this will run moc:
QT5_WRAP_CPP( StatsGenerator_Generated_MOC_SRCS
        ${StatsGenerator_MOC_HDRS}
        ${StatsGenerator_TABLEMODEL_MOC_HDRS}
        ${StatsGenerator_PRESETS_MOC_HDRS}
        ${StatsGenerator_DIALOGS_MOC_HDRS}
        ${StatsGenerator_DELEGATE_MOC_HDRS}
)
set_source_files_properties( ${StatsGenerator_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${StatsGenerator_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)


#-- Put the generated files into their own group for IDEs
cmp_IDE_GENERATED_PROPERTIES("Generated/ui" "${Generated_UI_HDRS}" "")
cmp_IDE_GENERATED_PROPERTIES("Generated/moc" "${Generated_MOC_SRCS};${StatsGenerator_Generated_MOC_SRCS}" "")
cmp_IDE_GENERATED_PROPERTIES("Generated/qrc" "${QRC_LICENSE_FILES}" "${Generated_RC_SRCS}")

# -- Finally set the Project Sources to include all the sources
set(StatsGenerator_PROJECT_SRCS
    ${StatsGenerator_SRCS}
    ${StatsGenerator_HDRS}
    ${StatsGenerator_MOC_HDRS}

    ${StatsGenerator_DELEGATE_MOC_HDRS}
    ${StatsGenerator_DELEGATE_SRCS}

    ${StatsGenerator_TABLEMODEL_MOC_HDRS}
    ${StatsGenerator_TABLEMODEL_SRCS}

    ${StatsGenerator_PRESETS_HDRS}
    ${StatsGenerator_PRESETS_MOC_HDRS}
    ${StatsGenerator_PRESETS_SRCS}

    ${StatsGenerator_DIALOGS_MOC_HDRS}
    ${StatsGenerator_DIALOGS_SRCS}

    ${Generated_RC_SRCS}
    ${Generated_UI_HDRS}
    ${Generated_MOC_SRCS}

    ${StatsGenerator_Generated_MOC_SRCS}

    ${StatsGenerator_CMP_FILES}
)

include_directories(${HDF5_INCLUDE_DIR})
include_directories(${PROJECT_CODE_DIR})
include_directories(${PROJECT_CODE_DIR}/Applications)
include_directories(${StatsGenerator_SOURCE_DIR})
include_directories(${StatsGenerator_BINARY_DIR})
include_directories(${DREAM3DProj_BINARY_DIR})
include_directories(${DREAM3DProj_BINARY_DIR}/Applications)
include_directories(${DREAM3DProj_BINARY_DIR}/Applications/${APPLICATION_NAME})
include_directories(${DREAM3DProj_BINARY_DIR}/Applications/${APPLICATION_NAME}/Presets)
include_directories(${DREAM3DProj_BINARY_DIR}/Applications/${APPLICATION_NAME}/Presets/Dialogs)
include_directories(${DREAM3DProj_BINARY_DIR}/Applications/${APPLICATION_NAME}/Delegates)
include_directories(${DREAM3DProj_BINARY_DIR}/Applications/${APPLICATION_NAME}/TableModels)
include_directories(${SIMPLProj_SOURCE_DIR}/Source)
include_directories(${SIMPLProj_BINARY_DIR})
include_directories(${SIMPLViewProj_SOURCE_DIR}/Source)
include_directories(${SIMPLViewProj_SOURCE_DIR}/Source/QtSupportLib)
include_directories(${SIMPLViewProj_BINARY_DIR})
include_directories(${SIMPLViewProj_BINARY_DIR}/QtSupportLib)
include_directories(${BrandedSIMPLView_DIR})


if(APPLE)
    set(APPLICATION_ICON ${PROJECT_RESOURCES_DIR}/icons/icns/${APPLICATION_NAME}.icns)
endif(APPLE)

if(WIN32)
    set(APPLICATION_ICON ${PROJECT_RESOURCES_DIR}/icons/ico/${APPLICATION_NAME}.ico)
ENDif(WIN32)

set(DEST_DIR ".")
if(UNIX AND NOT APPLE)
    set(DEST_DIR "bin")
endif()

BuildQtAppBundle(
    TARGET ${APPLICATION_NAME}
    SOURCES ${${APPLICATION_NAME}_PROJECT_SRCS}
    DEBUG_EXTENSION ${EXE_DEBUG_EXTENSION}
    ICON_FILE  ${APPLICATION_ICON}
    VERSION_MAJOR ${DREAM3D_VER_MAJOR}
    VERSION_MINOR ${DREAM3D_VER_MINOR}
    VERSION_PATCH ${DREAM3D_VER_PATCH}
    BINARY_DIR    ${${APPLICATION_NAME}_BINARY_DIR}
    LINK_LIBRARIES  H5Support EbsdLib SIMPLib OrientationLib QtSupportLib ${QWT_LIBRARY}
    LIB_SEARCH_DIRS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    QT5_MODULES   ${Qt5_COMPONENTS}
    COMPONENT     Applications
    INSTALL_DEST  ${DEST_DIR}
)
