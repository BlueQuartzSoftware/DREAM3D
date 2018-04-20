
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
  StatsProgressWidget
)

foreach(FPW ${DREAM3D_WIDGETS})
  set(${PLUGIN_NAME}_Widgets_HDRS ${${PLUGIN_NAME}_Widgets_HDRS}
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/${FPW}.h
    )
  set(${PLUGIN_NAME}_Widgets_SRCS ${${PLUGIN_NAME}_Widgets_SRCS}
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/${FPW}.cpp
    )
  set(${PLUGIN_NAME}_Widgets_UIS ${${PLUGIN_NAME}_Widgets_UIS}
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/UI_Files/${FPW}.ui
    )
endforeach()

# Add in the remaining sources that are actually widgets but are completely Custom and do NOT use private
# inheritance through a .ui file
set(${PLUGIN_NAME}_Widgets_HDRS
  ${${PLUGIN_NAME}_Widgets_HDRS}
  ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/StatsGenWidget.h
  ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/StatsGenODFWidget.h

)
set(${PLUGIN_NAME}_Widgets_SRCS
  ${${PLUGIN_NAME}_Widgets_SRCS}
  ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/StatsGenWidget.cpp
  ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/StatsGenODFWidget.cpp
  ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/StatsGenCurveTracker.cpp
)

set(${PLUGIN_NAME}_Widgets_MISC_HDRS
  ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/StatsGenCurveTracker.h
)

set(${PLUGIN_NAME}_DELEGATE_SRCS
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/Delegates/SGBetaItemDelegate.cpp
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/Delegates/SGLogNormalItemDelegate.cpp
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/Delegates/SGPowerLawItemDelegate.cpp
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/Delegates/SGODFItemDelegate.cpp
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/Delegates/SGMDFItemDelegate.cpp
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/Delegates/SGRDFItemDelegate.cpp
    )
set(${PLUGIN_NAME}_DELEGATE_MOC_HDRS
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/Delegates/SGBetaItemDelegate.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/Delegates/SGLogNormalItemDelegate.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/Delegates/SGPowerLawItemDelegate.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/Delegates/SGODFItemDelegate.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/Delegates/SGMDFItemDelegate.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/Delegates/SGRDFItemDelegate.h
    )
cmp_IDE_SOURCE_PROPERTIES( "SyntheticBuilding/Delegates" "${${PLUGIN_NAME}_DELEGATE_MOC_HDRS}" "${${PLUGIN_NAME}_DELEGATE_SRCS}" ${PROJECT_INSTALL_HEADERS})

set(${PLUGIN_NAME}_TABLEMODEL_SRCS
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/TableModels/SGAbstractTableModel.cpp
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/TableModels/SGBetaTableModel.cpp
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/TableModels/SGPowerLawTableModel.cpp
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/TableModels/SGLogNormalTableModel.cpp
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/TableModels/SGODFTableModel.cpp
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/TableModels/SGMDFTableModel.cpp
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/TableModels/SGRDFTableModel.cpp
    )

set(${PLUGIN_NAME}_TABLEMODEL_MOC_HDRS
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/TableModels/SGAbstractTableModel.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/TableModels/SGBetaTableModel.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/TableModels/SGPowerLawTableModel.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/TableModels/SGLogNormalTableModel.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/TableModels/SGODFTableModel.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/TableModels/SGMDFTableModel.h
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/TableModels/SGRDFTableModel.h
    )
cmp_IDE_SOURCE_PROPERTIES( "${PLUGIN_NAME}/TableModels" "${${PLUGIN_NAME}_TABLEMODEL_MOC_HDRS}" "${${PLUGIN_NAME}_TABLEMODEL_SRCS}" ${PROJECT_INSTALL_HEADERS})

set(${PLUGIN_NAME}_DIALOGS_SRCS
  ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/Presets/Dialogs/RolledPresetDialog.cpp
  ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/Presets/Dialogs/PrimaryRecrystallizedPresetDialog.cpp
)
set(${PLUGIN_NAME}_DIALOGS_MOC_HDRS
  ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/Presets/Dialogs/RolledPresetDialog.h
  ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/Presets/Dialogs/PrimaryRecrystallizedPresetDialog.h
)
cmp_IDE_SOURCE_PROPERTIES( "${PLUGIN_NAME}/Presets/Dialogs" "${${PLUGIN_NAME}_DIALOGS_MOC_HDRS}" "${${PLUGIN_NAME}_DIALOGS_SRCS}" ${PROJECT_INSTALL_HEADERS})


# Organize the Source files for things like Visual Studio and Xcode
cmp_IDE_SOURCE_PROPERTIES( "${PLUGIN_NAME}/Widgets" "${${PLUGIN_NAME}_Widgets_HDRS}" "${${PLUGIN_NAME}_Widgets_SRCS}" "0")

# Organize the Source files for things like Visual Studio and Xcode
cmp_IDE_GENERATED_PROPERTIES("${PLUGIN_NAME}/Widgets/UI_Files" "${${PLUGIN_NAME}_Widgets_UIS}" "")

# --------------------------------------------------------------------
# and finally this will run moc:
# QT5_WRAP_CPP( ${PLUGIN_NAME}_Generated_MOC_SRCS
#         ${${PLUGIN_NAME}_Widgets_HDRS}
#         ${${PLUGIN_NAME}_TABLEMODEL_MOC_HDRS}
#         ${${PLUGIN_NAME}_PRESETS_MOC_HDRS}
#         ${${PLUGIN_NAME}_DIALOGS_MOC_HDRS}
#         ${${PLUGIN_NAME}_DELEGATE_MOC_HDRS}
# )
set_source_files_properties( ${${PLUGIN_NAME}_Generated_MOC_SRCS} PROPERTIES GENERATED TRUE)
set_source_files_properties( ${${PLUGIN_NAME}_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

# These generated moc files will be #include in the FilterWidget source file that
# are generated so we need to tell the build system to NOT compile these files
set_source_files_properties( ${${PLUGIN_NAME}_Widgets_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

# --------------------------------------------------------------------
# -- Run UIC on the necessary files
QT5_WRAP_UI( ${PLUGIN_NAME}_Widgets_Generated_UI_HDRS ${${PLUGIN_NAME}_Widgets_UIS} )
foreach(h ${${PLUGIN_NAME}_Widgets_Generated_UI_HDRS})
  set_property(SOURCE ${h} PROPERTY SKIP_AUTOMOC ON)
endforeach()


# --------------------------------------------------------------------
#-- Put the Qt generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Moc" "" "${${PLUGIN_NAME}_Widgets_Generated_MOC_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${${PLUGIN_NAME}_Widgets_Generated_UI_HDRS}" "" "0")

# --------------------------------------------------------------------
# If you are doing more advanced Qt programming where you are including resources you will have to enable this section
# with your own cmake codes to include your resource file (.qrc) and any other needed files
# QT5_ADD_RESOURCES( ${PLUGIN_NAME}_Generated_RC_SRCS ""  )
# cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Qrc" "${${PLUGIN_NAME}_Generated_RC_SRCS}" "" "0")

