#--////////////////////////////////////////////////////////////////////////////
#-- Your License or copyright can go here
#--////////////////////////////////////////////////////////////////////////////

set(_filterGroupName OrientationAnalysisFilters)
set(${_filterGroupName}_FILTERS_HDRS "")

#--------
# This macro must come first before we start adding any filters
SIMPL_START_FILTER_GROUP(
  ALL_FILTERS_HEADERFILE ${AllFiltersHeaderFile}
  REGISTER_KNOWN_FILTERS_FILE ${RegisterKnownFiltersFile}
  FILTER_GROUP "${_filterGroupName}"
  BINARY_DIR ${${PLUGIN_NAME}_BINARY_DIR}
  )


#---------
# List your public filters here
set(_PublicFilters
  BadDataNeighborOrientationCheck
  ChangeAngleRepresentation
  ConvertHexGridToSquareGrid
  ConvertOrientations
  CreateEnsembleInfo
  CreateLambertSphere
  EbsdToH5Ebsd
  EMsoftSO3Sampler
  EnsembleInfoReader
  FindAvgCAxes
  FindAvgOrientations
  FindBoundaryStrengths
  FindCAxisLocations
  FindFeatureNeighborCAxisMisalignments
  FindFeatureReferenceCAxisMisorientations
  FindFeatureReferenceMisorientations
  FindKernelAvgMisorientations
  FindMisorientations
  FindSchmids
  FindSlipTransmissionMetrics
  FindTwinBoundaries
  FindTwinBoundarySchmidFactors
  INLWriter
  GenerateIPFColors
  NeighborOrientationCorrelation
  ReadAngData
  ReadCtfData
  ReadEdaxH5Data
  ReadH5Ebsd
  ReplaceElementAttributesWithNeighborValues
  RotateEulerRefFrame
  WriteIPFStandardTriangle
  WritePoleFigure
  WriteStatsGenOdfAngleFile
  OrientationUtility
  Stereographic3D
  GenerateFZQuaternions
)



list(LENGTH _PublicFilters PluginNumFilters)
set_property(GLOBAL PROPERTY PluginNumFilters ${PluginNumFilters})

#--------------
# Loop on all the filters adding each one. In this loop we default to making each filter exposed in the user
# interface in DREAM3D. If you want to have the filter compiled but NOT exposed to the user then use the next loop
foreach(f ${_PublicFilters} )
  ADD_SIMPL_FILTER(  "OrientationAnalysis" "OrientationAnalysis"
                        ${_filterGroupName} ${f}
                        ${${PLUGIN_NAME}_SOURCE_DIR}/Documentation/${_filterGroupName}/${f}.md TRUE ${${PLUGIN_NAME}_BINARY_DIR})
endforeach()


#---------------
# This is the list of Private Filters. These filters are available from other filters but the user will not
# be able to use them from the DREAM3D user interface.
set(_PrivateFilters

  FindBasalLoadingFactor

  FindLocalAverageCAxisMisalignments
  FindMicroTextureRegions
  FindOrientationFieldCurl
  GenerateEulerColors
  GenerateRodriguesColors
  YSChoiAbaqusReader

)

#-----------------
# Loop on the Private Filters adding each one to the DREAM3DLib project so that it gets compiled.
foreach(f ${_PrivateFilters} )
  ADD_SIMPL_FILTER(  "OrientationAnalysis" "OrientationAnalysis"
                        ${_filterGroupName} ${f}
                        ${${PLUGIN_NAME}_SOURCE_DIR}/Documentation/${_filterGroupName}/${f}.md FALSE ${${PLUGIN_NAME}_BINARY_DIR})
endforeach()


ADD_SIMPL_SUPPORT_HEADER(${${PLUGIN_NAME}_SOURCE_DIR} ${_filterGroupName} IPFLegendHelpers/IPFLegendPainter.h)
ADD_SIMPL_SUPPORT_SOURCE(${${PLUGIN_NAME}_SOURCE_DIR} ${_filterGroupName} IPFLegendHelpers/IPFLegendPainter.cpp)

macro(addIpfHelper ClassName)
  ADD_SIMPL_SUPPORT_HEADER(${${PLUGIN_NAME}_SOURCE_DIR} ${_filterGroupName} IPFLegendHelpers/${ClassName}IPFLegendPainter.h)
  ADD_SIMPL_SUPPORT_SOURCE(${${PLUGIN_NAME}_SOURCE_DIR} ${_filterGroupName} IPFLegendHelpers/${ClassName}IPFLegendPainter.cpp)
endmacro()

addIpfHelper(Hexagonal)
addIpfHelper(Cubic)
addIpfHelper(HexagonalLow)
addIpfHelper(CubicLow)
addIpfHelper(Triclinic)
addIpfHelper(Monoclinic)
addIpfHelper(Orthorhombic)
addIpfHelper(TetragonalLow)
addIpfHelper(Tetragonal)
addIpfHelper(TrigonalLow)
addIpfHelper(Trigonal)

#---------------------
# This macro must come last after we are done adding all the filters and support files.
SIMPL_END_FILTER_GROUP(${OrientationAnalysis_BINARY_DIR} "${_filterGroupName}" "OrientationAnalysis")

