
set(FilterWidgetsLib_Widgets_HDRS "")
set(FilterWidgetsLib_Widgets_SRCS "")
set(FilterWidgetsLib_Widgets_UIS "")


# --------------------------------------------------------------------
# List the Classes here that are QWidget Derived Classes
set(DREAM3D_WIDGETS
  QFilterWidget
)

foreach(FPW ${DREAM3D_WIDGETS})
  set(FilterWidgetsLib_Widgets_HDRS ${FilterWidgetsLib_Widgets_HDRS}
    ${FilterWidgetsLib_SOURCE_DIR}/Widgets/${FPW}.h
    )
  set(FilterWidgetsLib_Widgets_SRCS ${FilterWidgetsLib_Widgets_SRCS}
    ${FilterWidgetsLib_SOURCE_DIR}/Widgets/${FPW}.cpp
    )
  set(FilterWidgetsLib_Widgets_UIS ${FilterWidgetsLib_Widgets_UIS}
    ${FilterWidgetsLib_SOURCE_DIR}/Widgets/UI_Files/${FPW}.ui
    )
endforeach()



cmp_IDE_SOURCE_PROPERTIES( "FilterWidgetsLib/Widgets" "${FilterWidgetsLib_Widgets_HDRS}" "${FilterWidgetsLib_Widgets_SRCS}" "0")

cmp_IDE_GENERATED_PROPERTIES("FilterWidgetsLib/Widgets/UI_Files" "${FilterWidgetsLib_Widgets_UIS}" "")

# --------------------------------------------------------------------
# and finally this will run moc:
QT4_WRAP_CPP( FilterWidgetsLib_Widgets_Generated_MOC_SRCS ${FilterWidgetsLib_Widgets_HDRS} )

# These generated moc files will be #include in the FilterWidget source file that
# are generated so we need to tell the build system to NOT compile these files
set_source_files_properties( ${FilterWidgetsLib_Widgets_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

# -- Run MOC and UIC on the necessary files
# QT4_ADD_RESOURCES( FilterWidgetsLib_Generated_RC_SRCS "${DREAM3DProj_SOURCE_DIR}/Documentation/Filters/Generated_FilterDocs.qrc"  )

# --------------------------------------------------------------------
# Continue on with our Qt4 section
QT4_WRAP_UI( FilterWidgetsLib_Widgets_Generated_UI_HDRS ${FilterWidgetsLib_Widgets_UIS} )

# --------------------------------------------------------------------
#-- Put the Qt generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Moc" "" "${FilterWidgetsLib_Widgets_Generated_MOC_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${FilterWidgetsLib_ParameterWidgetes_Generated_UI_HDRS}" "" "0")
#cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Qrc" "${FilterWidgetsLib_Generated_RC_SRCS}" "" "0")




