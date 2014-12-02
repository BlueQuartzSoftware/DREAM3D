#--////////////////////////////////////////////////////////////////////////////
#-- Your License or copyright can go here
#--////////////////////////////////////////////////////////////////////////////

set(_filterGroupName IOFilters)
set(${_filterGroupName}_FILTERS_HDRS "")

#--------
# This macro must come first before we start adding any filters
START_FILTER_GROUP(${IO_BINARY_DIR} "${_filterGroupName}" "IO")


#---------
# List your public filters here
set(_PublicFilters
  AbaqusHexahedronWriter
  AbaqusSurfaceMeshWriter
  AvizoRectilinearCoordinateWriter
  AvizoUniformCoordinateWriter
  DxReader
  DxWriter
  EnsembleInfoReader
  FeatureDataCSVWriter
  FeatureInfoReader
  GBCDTriangleDumper
  INLWriter
  LammpsFileWriter
  LoadAdditiveMonitoringData
  LosAlamosFFTWriter
  NodesTrianglesToStl
  NodesTrianglesToVtk
  ParaDisReader
  PhReader
  PhWriter
  SPParksWriter
  SPParksTextReader
  SurfaceMeshToNodesTrianglesEdges
  SurfaceMeshToNonconformalVtk
  SurfaceMeshToStl
  SurfaceMeshToVtk

  VASPReader
  VisualizeGBCDGMT
  VisualizeGBCDPoleFigure
  VtkFeatureIdReader
  VtkRectilinearGridWriter
  YSChoiAbaqusReader
)


#--------------
# Loop on all the filters adding each one. In this loop we default to making each filter exposed in the user
# interface in DREAM3D. If you want to have the filter compiled but NOT exposed to the user then use the next loop
foreach(f ${_PublicFilters} )
  ADD_DREAM3D_FILTER(  "IO" "IO"
                        ${_filterGroupName} ${f}
                        ${IO_SOURCE_DIR}/Documentation/${_filterGroupName}/${f}.md TRUE)
endforeach()


#---------------
# This is the list of Private Filters. These filters are available from other filters but the user will not
# be able to use them from the DREAM3D user interface.
set(_PrivateFilters
    GoldfeatherReader
    V4DataContainerReader
)

#-----------------
# Loop on the Private Filters adding each one to the DREAM3DLib project so that it gets compiled.
foreach(f ${_PrivateFilters} )
  ADD_DREAM3D_FILTER(  "IO" "IO"
                        ${_filterGroupName} ${f}
                        ${DREAM3DLib_FILTER_DOC_DIR}/${_filterGroupName}/${f}.md FALSE)
endforeach()



#---------------------
# This macro must come last after we are done adding all the filters and support files.
END_FILTER_GROUP(${IO_BINARY_DIR} "${_filterGroupName}" "IO")

