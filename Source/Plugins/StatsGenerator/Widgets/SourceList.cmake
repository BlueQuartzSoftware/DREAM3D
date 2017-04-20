
set(${PLUGIN_NAME}_Widgets_HDRS "")
set(${PLUGIN_NAME}_Widgets_SRCS "")
set(${PLUGIN_NAME}_Widgets_UIS "")


# --------------------------------------------------------------------
# List the Classes here that are QWidget Derived Classes
set(DREAM3D_WIDGETS
  BoundaryPhaseWidget
  EditPhaseDialog
  MatrixPhaseWidget
  PrecipitatePhaseWidget
  PrimaryPhaseWidget
  StatsGenAxisODFWidget
  StatsGenFeatureSizeWidget
  StatsGenMDFWidget
  StatsGenPlotWidget
  StatsGenRDFWidget
  TextureDialog
  TransformationPhaseWidget
  ProgressDialog
)

foreach(FPW ${DREAM3D_WIDGETS})
  set(${PLUGIN_NAME}_Widgets_HDRS ${${PLUGIN_NAME}_Widgets_HDRS}
    ${${PLUGIN_NAME}_SOURCE_DIR}/Widgets/${FPW}.h
    )
  set(${PLUGIN_NAME}_Widgets_SRCS ${${PLUGIN_NAME}_Widgets_SRCS}
    ${${PLUGIN_NAME}_SOURCE_DIR}/Widgets/${FPW}.cpp
    )
  set(${PLUGIN_NAME}_Widgets_UIS ${${PLUGIN_NAME}_Widgets_UIS}
    ${${PLUGIN_NAME}_SOURCE_DIR}/Widgets/UI_Files/${FPW}.ui
    )
endforeach()

# Add in the remaining sources that are actually widgets but are completely Custom and do NOT use private
# inheritance through a .ui file
set(${PLUGIN_NAME}_Widgets_HDRS
  ${${PLUGIN_NAME}_Widgets_HDRS}
  ${${PLUGIN_NAME}_SOURCE_DIR}/Widgets/StatsGenWidget.h
  ${${PLUGIN_NAME}_SOURCE_DIR}/Widgets/StatsGenODFWidget.h

)
set(${PLUGIN_NAME}_Widgets_SRCS
  ${${PLUGIN_NAME}_Widgets_SRCS}
  ${${PLUGIN_NAME}_SOURCE_DIR}/Widgets/StatsGenWidget.cpp
  ${${PLUGIN_NAME}_SOURCE_DIR}/Widgets/StatsGenODFWidget.cpp
  ${${PLUGIN_NAME}_SOURCE_DIR}/Widgets/StatsGenCurveTracker.cpp
)

set(${PLUGIN_NAME}_Widgets_MISC_HDRS
  ${${PLUGIN_NAME}_SOURCE_DIR}/Widgets/StatsGenCurveTracker.h
)

set(StatsGenerator_DELEGATE_SRCS
    ${StatsGenerator_SOURCE_DIR}/Widgets/Delegates/SGBetaItemDelegate.cpp
    ${StatsGenerator_SOURCE_DIR}/Widgets/Delegates/SGLogNormalItemDelegate.cpp
    ${StatsGenerator_SOURCE_DIR}/Widgets/Delegates/SGPowerLawItemDelegate.cpp
    ${StatsGenerator_SOURCE_DIR}/Widgets/Delegates/SGODFItemDelegate.cpp
    ${StatsGenerator_SOURCE_DIR}/Widgets/Delegates/SGMDFItemDelegate.cpp
    ${StatsGenerator_SOURCE_DIR}/Widgets/Delegates/SGRDFItemDelegate.cpp
    )
set(StatsGenerator_DELEGATE_MOC_HDRS
    ${StatsGenerator_SOURCE_DIR}/Widgets/Delegates/SGBetaItemDelegate.h
    ${StatsGenerator_SOURCE_DIR}/Widgets/Delegates/SGLogNormalItemDelegate.h
    ${StatsGenerator_SOURCE_DIR}/Widgets/Delegates/SGPowerLawItemDelegate.h
    ${StatsGenerator_SOURCE_DIR}/Widgets/Delegates/SGODFItemDelegate.h
    ${StatsGenerator_SOURCE_DIR}/Widgets/Delegates/SGMDFItemDelegate.h
    ${StatsGenerator_SOURCE_DIR}/Widgets/Delegates/SGRDFItemDelegate.h
    )
cmp_IDE_SOURCE_PROPERTIES( "StatsGenerator/Delegates" "${StatsGenerator_DELEGATE_MOC_HDRS}" "${StatsGenerator_DELEGATE_SRCS}" ${PROJECT_INSTALL_HEADERS})

set(StatsGenerator_TABLEMODEL_SRCS
    ${StatsGenerator_SOURCE_DIR}/Widgets/TableModels/SGAbstractTableModel.cpp
    ${StatsGenerator_SOURCE_DIR}/Widgets/TableModels/SGBetaTableModel.cpp
    ${StatsGenerator_SOURCE_DIR}/Widgets/TableModels/SGPowerLawTableModel.cpp
    ${StatsGenerator_SOURCE_DIR}/Widgets/TableModels/SGLogNormalTableModel.cpp
    ${StatsGenerator_SOURCE_DIR}/Widgets/TableModels/SGODFTableModel.cpp
    ${StatsGenerator_SOURCE_DIR}/Widgets/TableModels/SGMDFTableModel.cpp
    ${StatsGenerator_SOURCE_DIR}/Widgets/TableModels/SGRDFTableModel.cpp
    )

set(StatsGenerator_TABLEMODEL_MOC_HDRS
    ${StatsGenerator_SOURCE_DIR}/Widgets/TableModels/SGAbstractTableModel.h
    ${StatsGenerator_SOURCE_DIR}/Widgets/TableModels/SGBetaTableModel.h
    ${StatsGenerator_SOURCE_DIR}/Widgets/TableModels/SGPowerLawTableModel.h
    ${StatsGenerator_SOURCE_DIR}/Widgets/TableModels/SGLogNormalTableModel.h
    ${StatsGenerator_SOURCE_DIR}/Widgets/TableModels/SGODFTableModel.h
    ${StatsGenerator_SOURCE_DIR}/Widgets/TableModels/SGMDFTableModel.h
    ${StatsGenerator_SOURCE_DIR}/Widgets/TableModels/SGRDFTableModel.h
    )
cmp_IDE_SOURCE_PROPERTIES( "StatsGenerator/TableModels" "${StatsGenerator_TABLEMODEL_MOC_HDRS}" "${StatsGenerator_TABLEMODEL_SRCS}" ${PROJECT_INSTALL_HEADERS})


set(StatsGenerator_PRESETS_SRCS
    ${StatsGenerator_SOURCE_DIR}/Widgets/Presets/MicrostructurePresetManager.cpp
    ${StatsGenerator_SOURCE_DIR}/Widgets/Presets/AbstractMicrostructurePreset.cpp
    ${StatsGenerator_SOURCE_DIR}/Widgets/Presets/PrimaryEquiaxedPreset.cpp
    ${StatsGenerator_SOURCE_DIR}/Widgets/Presets/PrecipitateEquiaxedPreset.cpp
    ${StatsGenerator_SOURCE_DIR}/Widgets/Presets/PrimaryRecrystallizedPreset.cpp
    ${StatsGenerator_SOURCE_DIR}/Widgets/Presets/PrimaryRolledPreset.cpp
    ${StatsGenerator_SOURCE_DIR}/Widgets/Presets/PrecipitateRolledPreset.cpp
)
set(StatsGenerator_PRESETS_MOC_HDRS "")
set(StatsGenerator_PRESETS_HDRS
    ${StatsGenerator_SOURCE_DIR}/Widgets/Presets/AbstractMicrostructurePresetFactory.h
    ${StatsGenerator_SOURCE_DIR}/Widgets/Presets/AbstractMicrostructurePreset.h
    ${StatsGenerator_SOURCE_DIR}/Widgets/Presets/MicrostructurePresetManager.h
    ${StatsGenerator_SOURCE_DIR}/Widgets/Presets/PrimaryEquiaxedPreset.h
    ${StatsGenerator_SOURCE_DIR}/Widgets/Presets/PrecipitateEquiaxedPreset.h
    ${StatsGenerator_SOURCE_DIR}/Widgets/Presets/PrimaryRecrystallizedPreset.h
    ${StatsGenerator_SOURCE_DIR}/Widgets/Presets/PrimaryRolledPreset.h
    ${StatsGenerator_SOURCE_DIR}/Widgets/Presets/PrecipitateRolledPreset.h
)
cmp_IDE_SOURCE_PROPERTIES( "StatsGenerator/Presets" "${StatsGenerator_PRESETS_HDRS};${StatsGenerator_PRESETS_MOC_HDRS}" "${StatsGenerator_PRESETS_SRCS}" ${PROJECT_INSTALL_HEADERS})

set(StatsGenerator_DIALOGS_SRCS
  ${StatsGenerator_SOURCE_DIR}/Widgets/Presets/Dialogs/PrimaryRolledPresetDialog.cpp
  ${StatsGenerator_SOURCE_DIR}/Widgets/Presets/Dialogs/PrecipitateRolledPresetDialog.cpp
  ${StatsGenerator_SOURCE_DIR}/Widgets/Presets/Dialogs/PrimaryRecrystallizedPresetDialog.cpp
)
set(StatsGenerator_DIALOGS_MOC_HDRS
  ${StatsGenerator_SOURCE_DIR}/Widgets/Presets/Dialogs/PrimaryRolledPresetDialog.h
  ${StatsGenerator_SOURCE_DIR}/Widgets/Presets/Dialogs/PrecipitateRolledPresetDialog.h
  ${StatsGenerator_SOURCE_DIR}/Widgets/Presets/Dialogs/PrimaryRecrystallizedPresetDialog.h
)
cmp_IDE_SOURCE_PROPERTIES( "StatsGenerator/Presets/Dialogs" "${StatsGenerator_DIALOGS_MOC_HDRS}" "${StatsGenerator_DIALOGS_SRCS}" ${PROJECT_INSTALL_HEADERS})


# Organize the Source files for things like Visual Studio and Xcode
cmp_IDE_SOURCE_PROPERTIES( "${PLUGIN_NAME}/Widgets" "${${PLUGIN_NAME}_Widgets_HDRS}" "${${PLUGIN_NAME}_Widgets_SRCS}" "0")

# Organize the Source files for things like Visual Studio and Xcode
cmp_IDE_GENERATED_PROPERTIES("${PLUGIN_NAME}/Widgets/UI_Files" "${${PLUGIN_NAME}_Widgets_UIS}" "")

# --------------------------------------------------------------------
# and finally this will run moc:
QT5_WRAP_CPP( StatsGenerator_Generated_MOC_SRCS
        ${${PLUGIN_NAME}_Widgets_HDRS}
        ${StatsGenerator_TABLEMODEL_MOC_HDRS}
        ${StatsGenerator_PRESETS_MOC_HDRS}
        ${StatsGenerator_DIALOGS_MOC_HDRS}
        ${StatsGenerator_DELEGATE_MOC_HDRS}
)
set_source_files_properties( ${StatsGenerator_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${StatsGenerator_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

# These generated moc files will be #include in the FilterWidget source file that
# are generated so we need to tell the build system to NOT compile these files
set_source_files_properties( ${${PLUGIN_NAME}_Widgets_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

# --------------------------------------------------------------------
# -- Run UIC on the necessary files
QT5_WRAP_UI( ${PLUGIN_NAME}_Widgets_Generated_UI_HDRS ${${PLUGIN_NAME}_Widgets_UIS} )

# --------------------------------------------------------------------
#-- Put the Qt generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Moc" "" "${${PLUGIN_NAME}_Widgets_Generated_MOC_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${${PLUGIN_NAME}_Widgets_Generated_UI_HDRS}" "" "0")

# --------------------------------------------------------------------
# If you are doing more advanced Qt programming where you are including resources you will have to enable this section
# with your own cmake codes to include your resource file (.qrc) and any other needed files
# QT5_ADD_RESOURCES( ${PLUGIN_NAME}_Generated_RC_SRCS ""  )
# cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Qrc" "${${PLUGIN_NAME}_Generated_RC_SRCS}" "" "0")

# include_directories(${${PLUGIN_NAME}_BINARY_DIR}/Widgets/Presets)
# include_directories(${${PLUGIN_NAME}_BINARY_DIR}/Widgets/Presets/Dialogs)
# include_directories(${${PLUGIN_NAME}_BINARY_DIR}/Widgets/Delegates)
# include_directories(${${PLUGIN_NAME}_BINARY_DIR}/Widgets/TableModels)




