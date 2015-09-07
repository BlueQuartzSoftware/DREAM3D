
set(DREAM3DWidgetsLib_Widgets_HDRS "")
set(DREAM3DWidgetsLib_Widgets_SRCS "")
set(DREAM3DWidgetsLib_Widgets_UIS "")


# --------------------------------------------------------------------
# List the Classes here that are QWidget Derived Classes
set(DREAM3D_WIDGETS
    BookmarksDockWidget
    DREAM3DUpdateCheckDialog
    FilterInputWidget
    FilterLibraryDockWidget
    FilterListDockWidget
    IssuesDockWidget
    PipelineFilterWidget
    PrebuiltPipelinesDockWidget
    DataContainerArrayWidget
    DropBoxWidget
)

foreach(FPW ${DREAM3D_WIDGETS})
  set(DREAM3DWidgetsLib_Widgets_HDRS ${DREAM3DWidgetsLib_Widgets_HDRS}
    ${DREAM3DWidgetsLib_SOURCE_DIR}/Widgets/${FPW}.h
    )
  set(DREAM3DWidgetsLib_Widgets_SRCS ${DREAM3DWidgetsLib_Widgets_SRCS}
    ${DREAM3DWidgetsLib_SOURCE_DIR}/Widgets/${FPW}.cpp
    )
  set(DREAM3DWidgetsLib_Widgets_UIS ${DREAM3DWidgetsLib_Widgets_UIS}
    ${DREAM3DWidgetsLib_SOURCE_DIR}/Widgets/UI_Files/${FPW}.ui
    )
endforeach()

# Add in the remaining sources that are actually widgets but are completely Custom and do NOT use private
# inheritance through a .ui file
set(DREAM3DWidgetsLib_Widgets_HDRS
  ${DREAM3DWidgetsLib_Widgets_HDRS}
  ${DREAM3DWidgetsLib_SOURCE_DIR}/Widgets/PipelineViewWidget.h
  ${DREAM3DWidgetsLib_SOURCE_DIR}/Widgets/FilterListWidget.h
  ${DREAM3DWidgetsLib_SOURCE_DIR}/Widgets/FilterTreeWidget.h
  ${DREAM3DWidgetsLib_SOURCE_DIR}/Widgets/FilterLibraryTreeWidget.h
  ${DREAM3DWidgetsLib_SOURCE_DIR}/Widgets/BookmarksItem.h
  ${DREAM3DWidgetsLib_SOURCE_DIR}/Widgets/BookmarksModel.h
  ${DREAM3DWidgetsLib_SOURCE_DIR}/Widgets/BookmarksTreeView.h
  ${DREAM3DWidgetsLib_SOURCE_DIR}/Widgets/BookmarksItemDelegate.h
  )
set(DREAM3DWidgetsLib_Widgets_SRCS
  ${DREAM3DWidgetsLib_Widgets_SRCS}
  ${DREAM3DWidgetsLib_SOURCE_DIR}/Widgets/PipelineViewWidget.cpp
  ${DREAM3DWidgetsLib_SOURCE_DIR}/Widgets/FilterListWidget.cpp
  ${DREAM3DWidgetsLib_SOURCE_DIR}/Widgets/FilterTreeWidget.cpp
  ${DREAM3DWidgetsLib_SOURCE_DIR}/Widgets/FilterLibraryTreeWidget.cpp
  ${DREAM3DWidgetsLib_SOURCE_DIR}/Widgets/BookmarksItem.cpp
  ${DREAM3DWidgetsLib_SOURCE_DIR}/Widgets/BookmarksModel.cpp
  ${DREAM3DWidgetsLib_SOURCE_DIR}/Widgets/BookmarksTreeView.cpp
  ${DREAM3DWidgetsLib_SOURCE_DIR}/Widgets/BookmarksItemDelegate.cpp
  )



cmp_IDE_SOURCE_PROPERTIES( "DREAM3DWidgetsLib/Widgets" "${DREAM3DWidgetsLib_Widgets_HDRS}" "${DREAM3DWidgetsLib_Widgets_SRCS}" "0")

cmp_IDE_GENERATED_PROPERTIES("DREAM3DWidgetsLib/Widgets/UI_Files" "${DREAM3DWidgetsLib_Widgets_UIS}" "")

# --------------------------------------------------------------------
# and finally this will run moc:
#QT5_WRAP_CPP( DREAM3DWidgetsLib_Widgets_Generated_MOC_SRCS ${DREAM3DWidgetsLib_Widgets_HDRS} )

# These generated moc files will be #include in the FilterWidget source file that
# are generated so we need to tell the build system to NOT compile these files
set_source_files_properties( ${DREAM3DWidgetsLib_Widgets_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

# -- Run MOC and UIC on the necessary files
# QT5_ADD_RESOURCES( DREAM3DWidgetsLib_Generated_RC_SRCS "${DREAM3DProj_SOURCE_DIR}/Documentation/Filters/Generated_FilterDocs.qrc"  )

# --------------------------------------------------------------------
# Continue on with our Qt4 section
QT5_WRAP_UI( DREAM3DWidgetsLib_Widgets_Generated_UI_HDRS ${DREAM3DWidgetsLib_Widgets_UIS} )

# --------------------------------------------------------------------
#-- Put the Qt generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Moc" "" "${DREAM3DWidgetsLib_Widgets_Generated_MOC_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${DREAM3DWidgetsLib_Widgets_Generated_UI_HDRS}" "" "0")
#cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Qrc" "${DREAM3DWidgetsLib_Generated_RC_SRCS}" "" "0")




