#--////////////////////////////////////////////////////////////////////////////
#-- Your License or copyright can go here
#--////////////////////////////////////////////////////////////////////////////

set(_filterGroupName StatisticsFilters)
set(${_filterGroupName}_FILTERS_HDRS "")

SIMPL_START_FILTER_GROUP(
  ALL_FILTERS_HEADERFILE ${AllFiltersHeaderFile}
  REGISTER_KNOWN_FILTERS_FILE ${RegisterKnownFiltersFile}
  FILTER_GROUP "${_filterGroupName}"
  BINARY_DIR ${${PLUGIN_NAME}_BINARY_DIR}
  )

#---------
# List your public filters here

set(_PublicFilters
  CalculateArrayHistogram
  ComputeMomentInvariants2D
  FindAvgScalarValueForFeatures
  FindBoundaryElementFractions
  FindDifferenceMap
  FindEuclideanDistMap
  FindFeatureClustering
  FindLargestCrossSections
  FindNeighborhoods
  FindNeighbors
  FindNumFeatures
  FindShapes
  FindSizes
  FindSurfaceAreaToVolume
  FindVolFractions
  FitFeatureData
  GenerateEnsembleStatistics
)


list(LENGTH _PublicFilters PluginNumFilters)
set_property(GLOBAL PROPERTY PluginNumFilters ${PluginNumFilters})

#--------------
# Loop on all the filters adding each one. In this loop we default to making each filter exposed in the user
# interface in DREAM3D. If you want to have the filter compiled but NOT exposed to the user then use the next loop
foreach(f ${_PublicFilters} )
  ADD_SIMPL_FILTER(  "Statistics" "Statistics"
                        ${_filterGroupName} ${f}
                        ${Statistics_SOURCE_DIR}/Documentation/${_filterGroupName}/${f}.md TRUE ${Statistics_BINARY_DIR})
endforeach()


#---------------
# This is the list of Private Filters. These filters are available from other filters but the user will not
# be able to use them from the DREAM3D user interface.
set(_PrivateFilters
  CorrelateValuesWithVectorDirection
  FindEllipsoidError
  FindFeatureHistogram
  FindSaltykovSizes
  FitCorrelatedFeatureData
  QuiltCellData
)

#-----------------
# Loop on the Private Filters adding each one to the DREAM3DLib project so that it gets compiled.
foreach(f ${_PrivateFilters} )
  ADD_SIMPL_FILTER(  "Statistics" "Statistics"
                        ${_filterGroupName} ${f}
                        ${${PLUGIN_NAME}_SOURCE_DIR}/Documentation/${_filterGroupName}/${f}.md FALSE ${${PLUGIN_NAME}_BINARY_DIR})
endforeach()

ADD_SIMPL_SUPPORT_HEADER(${${PLUGIN_NAME}_SOURCE_DIR} ${_filterGroupName} util/MomentInvariants2D.h)
ADD_SIMPL_SUPPORT_SOURCE(${${PLUGIN_NAME}_SOURCE_DIR} ${_filterGroupName} util/MomentInvariants2D.cpp)


SIMPL_END_FILTER_GROUP(${Statistics_BINARY_DIR} "${_filterGroupName}" "Statistics Filters")

