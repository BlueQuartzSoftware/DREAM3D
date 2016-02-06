#--////////////////////////////////////////////////////////////////////////////
#-- Your License or copyright can go here
#--////////////////////////////////////////////////////////////////////////////

set(_filterGroupName ProcessingFilters)
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
  
  ErodeDilateBadData
  ErodeDilateCoordinationNumber
  ErodeDilateMask
  FillBadData
  FindProjectedImageStatistics
  FindRelativeMotionBetweenSlices
  FlattenImage
  IdentifySample
  InitializeData
  MinNeighbors
  MinSize
  MultiThresholdObjects
  RemoveFlaggedFeatures
)

#--------------
# Loop on all the filters adding each one. In this loop we default to making each filter exposed in the user
# interface in DREAM3D. If you want to have the filter compiled but NOT exposed to the user then use the next loop
foreach(f ${_PublicFilters} )
  ADD_SIMPL_FILTER(  "Processing" "Processing"
                        ${_filterGroupName} ${f}
                        ${Processing_SOURCE_DIR}/Documentation/${_filterGroupName}/${f}.md TRUE ${Processing_BINARY_DIR})
endforeach()


#---------------
# This is the list of Private Filters. These filters are available from other filters but the user will not
# be able to use them from the DREAM3D user interface.
set(_PrivateFilters
)

#-----------------
# Loop on the Private Filters adding each one to the DREAM3DLib project so that it gets compiled.
foreach(f ${_PrivateFilters} )
  ADD_SIMPL_FILTER(  "Processing" "Processing"
                        ${_filterGroupName} ${f}
                        ${${PLUGIN_NAME}_SOURCE_DIR}/Documentation/${_filterGroupName}/${f}.md FALSE ${${PLUGIN_NAME}_BINARY_DIR})
endforeach()




SIMPL_END_FILTER_GROUP(${Processing_BINARY_DIR} "${_filterGroupName}" "Processing Filters")

