
set(${PLUGIN_NAME}_Widgets_HDRS "")
set(${PLUGIN_NAME}_Widgets_SRCS "")
set(${PLUGIN_NAME}_Widgets_UIS "")


# --------------------------------------------------------------------
# List the Classes here that are QWidget Derived Classes
set(DREAM3D_WIDGETS
    QEbsdReferenceFrameDialog
    EulerWidget
    QuatWidget
    OmWidget
    AxisAngleWidget
    RodriguesWidget
    HomochoricWidget
    CubochoricWidget
    StereographicWidget
)

set(DREAM3D_WIDGETS_NO_UI
    OrientationUtilityCalculator
    OrientationWidget
)

foreach(FPW ${DREAM3D_WIDGETS})
  set(${PLUGIN_NAME}_Widgets_MOC_HDRS ${${PLUGIN_NAME}_Widgets_MOC_HDRS}
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/${FPW}.h
    )
  set(${PLUGIN_NAME}_Widgets_SRCS ${${PLUGIN_NAME}_Widgets_SRCS}
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/${FPW}.cpp
    )
  set(${PLUGIN_NAME}_Widgets_UIS ${${PLUGIN_NAME}_Widgets_UIS}
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/UI_Files/${FPW}.ui
    )
endforeach()

foreach(FPW ${DREAM3D_WIDGETS_NO_UI})
  set(${PLUGIN_NAME}_Widgets_MOC_HDRS ${${PLUGIN_NAME}_Widgets_MOC_HDRS}
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/${FPW}.h
    )
  set(${PLUGIN_NAME}_Widgets_SRCS ${${PLUGIN_NAME}_Widgets_SRCS}
    ${${PLUGIN_NAME}_SOURCE_DIR}/Gui/Widgets/${FPW}.cpp
    )
endforeach()

# Add in the remaining sources that are actually widgets but are completely Custom and do NOT use private
# inheritance through a .ui file
set(${PLUGIN_NAME}_Widgets_HDRS
  ${${PLUGIN_NAME}_Widgets_MOC_HDRS}

  )
set(${PLUGIN_NAME}_Widgets_SRCS
  ${${PLUGIN_NAME}_Widgets_SRCS}

  )

cmp_IDE_SOURCE_PROPERTIES( "Widgets" "${${PLUGIN_NAME}_Widgets_MOC_HDRS};${${PLUGIN_NAME}_Widgets_HDRS}" "${${PLUGIN_NAME}_Widgets_SRCS}" "0")

cmp_IDE_GENERATED_PROPERTIES("Widgets/UI_Files" "${${PLUGIN_NAME}_Widgets_UIS}" "")

# --------------------------------------------------------------------
# and finally this will run moc:
# QT5_WRAP_CPP( ${PLUGIN_NAME}_Widgets_Generated_MOC_SRCS ${${PLUGIN_NAME}_Widgets_MOC_HDRS} )

# These generated moc files will be #include in the FilterWidget source file that
# are generated so we need to tell the build system to NOT compile these files
# set_source_files_properties( ${${PLUGIN_NAME}_Widgets_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

# -- Run MOC and UIC on the necessary files
# QT5_ADD_RESOURCES( ${PLUGIN_NAME}_Generated_RC_SRCS "${DREAM3DProj_SOURCE_DIR}/Documentation/Filters/Generated_FilterDocs.qrc"  )

# --------------------------------------------------------------------
# Wrap UI files so they are AUTO UIC'ed
QT5_WRAP_UI( ${PLUGIN_NAME}_Widgets_Generated_UI_HDRS ${${PLUGIN_NAME}_Widgets_UIS} )
foreach(h ${${PLUGIN_NAME}_Widgets_Generated_UI_HDRS})
  set_property(SOURCE ${h} PROPERTY SKIP_AUTOMOC ON)
endforeach()


# --------------------------------------------------------------------
#-- Put the Qt generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Moc" "" "${${PLUGIN_NAME}_Widgets_Generated_MOC_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${${PLUGIN_NAME}_Widgets_Generated_UI_HDRS}" "" "0")
#cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Qrc" "${${PLUGIN_NAME}_Generated_RC_SRCS}" "" "0")




