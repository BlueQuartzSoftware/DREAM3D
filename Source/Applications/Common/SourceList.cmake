
set(AppsCommon_Widgets_HDRS "")
set(AppsCommon_Widgets_SRCS "")
set(AppsCommon_Widgets_UIS "")


# --------------------------------------------------------------------
# List the Classes here that are QWidget Derived Classes
set(APPS_WIDGETS

)

#-- Only include this widget if we are compiling against QtWebEngine
if(DREAM3D_USE_QtWebEngine)
  set(APPS_WIDGETS ${APPS_WIDGETS} DREAM3DUserManualDialog)
endif()

foreach(FPW ${APPS_WIDGETS})
  set(AppsCommon_Widgets_HDRS ${AppsCommon_Widgets_HDRS}
    ${DREAM3DProj_SOURCE_DIR}/Source/Applications/Common/${FPW}.h
    )
  set(AppsCommon_Widgets_SRCS ${AppsCommon_Widgets_SRCS}
    ${DREAM3DProj_SOURCE_DIR}/Source/Applications/Common/${FPW}.cpp
    )
  set(AppsCommon_Widgets_UIS ${AppsCommon_Widgets_UIS}
    ${DREAM3DProj_SOURCE_DIR}/Source/Applications/Common/UI_Files/${FPW}.ui
    )
endforeach()

# Add in the remaining sources that are actually widgets but are completely Custom and do NOT use private
# inheritance through a .ui file
set(AppsCommon_Widgets_HDRS
  ${AppsCommon_Widgets_HDRS}
  )
set(AppsCommon_Widgets_SRCS
  ${AppsCommon_Widgets_SRCS}
  )



cmp_IDE_SOURCE_PROPERTIES( "Applications/Common" "${AppsCommon_Widgets_HDRS}" "${AppsCommon_Widgets_SRCS}" "0")

cmp_IDE_GENERATED_PROPERTIES("Applications/Common/UI_Files" "${AppsCommon_Widgets_UIS}" "")

# --------------------------------------------------------------------
# and finally this will run moc:
#QT5_WRAP_CPP( AppsCommon_Widgets_Generated_MOC_SRCS ${AppsCommon_Widgets_HDRS} )

# These generated moc files will be #include in the FilterWidget source file that
# are generated so we need to tell the build system to NOT compile these files
set_source_files_properties( ${AppsCommon_Widgets_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

# -- Run MOC and UIC on the necessary files
# QT5_ADD_RESOURCES( AppsCommon_Generated_RC_SRCS "${DREAM3DProj_SOURCE_DIR}/Documentation/Filters/Generated_FilterDocs.qrc"  )

# --------------------------------------------------------------------
# Continue on with our Qt4 section
QT5_WRAP_UI( AppsCommon_Widgets_Generated_UI_HDRS ${AppsCommon_Widgets_UIS} )

# --------------------------------------------------------------------
#-- Put the Qt generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Moc" "" "${AppsCommon_Widgets_Generated_MOC_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${AppsCommon_Widgets_Generated_UI_HDRS}" "" "0")
#cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Qrc" "${AppsCommon_Generated_RC_SRCS}" "" "0")




