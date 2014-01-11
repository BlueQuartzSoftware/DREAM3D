
set(FilterWidgets_ParameterWidgets_HDRS "")
set(FilterWidgets_ParameterWidgets_SRCS "")
set(FilterWidgets_ParameterWidgets_UIS "")


set(DREAM3D_PARAMETER_WIDGETS
      StringWidget
      IntWidget
      DoubleWidget
      InputFileWidget
      InputPathWidget
      OutputFileWidget
      OutputPathWidget
      BooleanWidget
      ChoiceWidget  
      IntVec3Widget
      FloatVec3Widget
      AxisAngleWidget
      VolumeVertexArrayNameSelectionWidget  
      VolumeEdgeArrayNameSelectionWidget 
      VolumeFaceArrayNameSelectionWidget 
      VolumeCellArrayNameSelectionWidget  
      VolumeFeatureArrayNameSelectionWidget 
      VolumeEnsembleArrayNameSelectionWidget 
      SurfaceVertexArrayNameSelectionWidget
      SurfaceFaceArrayNameSelectionWidget
      SurfaceEdgeArrayNameSelectionWidget
      SurfaceFeatureArrayNameSelectionWidget
      SurfaceEnsembleArrayNameSelectionWidget
      EdgeVertexArrayNameSelectionWidget
      EdgeEdgeArrayNameSelectionWidget
      EdgeFeatureArrayNameSelectionWidget
      EdgeEnsembleArrayNameSelectionWidget
      VertexVertexArrayNameSelectionWidget
      VertexFeatureArrayNameSelectionWidget
      VertexEnsembleArrayNameSelectionWidget
      ArraySelectionWidget 
      CellArrayComparisonSelectionWidget
      FeatureArrayComparisonSelectionWidget
      EnsembleArrayComparisonSelectionWidget
      VertexArrayComparisonSelectionWidget
      FaceArrayComparisonSelectionWidget
      EdgeArrayComparisonSelectionWidget
      CustomWidget
)


foreach(FPW ${DREAM3D_PARAMETER_WIDGETS})
	set(FilterWidgets_ParameterWidgets_HDRS ${FilterWidgets_ParameterWidgets_HDRS}
		${FilterWidgetsLib_SOURCE_DIR}/Widgets/${FPW}.h
		)
	set(FilterWidgets_ParameterWidgets_SRCS ${FilterWidgets_ParameterWidgets_SRCS}
		${FilterWidgetsLib_SOURCE_DIR}/Widgets/${FPW}.cpp
		)
	set(FilterWidgets_ParameterWidgets_UIS ${FilterWidgets_ParameterWidgets_UIS} 
		${FilterWidgetsLib_SOURCE_DIR}/Widgets/UI_Files/${FPW}.ui
		)
endforeach()



cmp_IDE_SOURCE_PROPERTIES( "FilterWidgets/Widgets" "${FilterWidgets_ParameterWidgets_HDRS}" "${FilterWidgets_ParameterWidgets_SRCS}" "0")

cmp_IDE_GENERATED_PROPERTIES("FilterWidgets/Widgets/UI_Files" "${FilterWidgets_ParameterWidgets_UIS}" "")

# --------------------------------------------------------------------
# and finally this will run moc:
QT4_WRAP_CPP( FilterWidgets_ParameterWidgets_Generated_MOC_SRCS ${FilterWidgets_ParameterWidgets_HDRS} )

# These generated moc files will be #include in the FilterWidget source file that
# are generated so we need to tell the build system to NOT compile these files
set_source_files_properties( ${FilterWidgets_ParameterWidgets_Generated_MOC_SRCS} PROPERTIES HEADER_FILE_ONLY TRUE)

# -- Run MOC and UIC on the necessary files
# QT4_ADD_RESOURCES( FilterWidgetsLib_Generated_RC_SRCS "${DREAM3DProj_SOURCE_DIR}/Documentation/Filters/Generated_FilterDocs.qrc"  )

# --------------------------------------------------------------------
# Continue on with our Qt4 section
QT4_WRAP_UI( FilterWidgets_ParameterWidgetes_Generated_UI_HDRS ${FilterWidgets_ParameterWidgets_UIS} )

# --------------------------------------------------------------------
#-- Put the Qt generated files into their own group for IDEs
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Moc" "" "${FilterWidgets_ParameterWidgets_Generated_MOC_SRCS}" "0")
cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${FilterWidgets_ParameterWidgetes_Generated_UI_HDRS}" "" "0")
#cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Qrc" "${FilterWidgetsLib_Generated_RC_SRCS}" "" "0")




