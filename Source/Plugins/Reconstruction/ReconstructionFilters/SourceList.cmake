#--////////////////////////////////////////////////////////////////////////////
#-- Your License or copyright can go here
#--////////////////////////////////////////////////////////////////////////////

set(_filterGroupName ReconstructionFilters)
set(${_filterGroupName}_FILTERS_HDRS "")

SIMPL_START_FILTER_GROUP(
  ALL_FILTERS_HEADERFILE ${AllFiltersHeaderFile}
  REGISTER_KNOWN_FILTERS_FILE ${RegisterKnownFiltersFile}
  FILTER_GROUP "${_filterGroupName}"
  BINARY_DIR ${Reconstruction_BINARY_DIR} )

#---------------
# This is the list of Filters Superclasses. Other filters use these filters as a
# superclass. If the filters are NOT meant to be ever invoked from the user interface
# then they go here. This is also so that the python wrapping will work correctly.
set(_SuperclassFilters
  SegmentFeatures
  GroupFeatures
  AlignSections
)

#-----------------
# Loop on the Filter Superclasses adding each one to the DREAM3DLib project so that it gets compiled.
foreach(f ${_SuperclassFilters} )
  ADD_SIMPL_SUPERCLASS_FILTER("${PLUGIN_NAME}" "${PLUGIN_NAME}" ${_filterGroupName} ${f})
endforeach()

#---------
# List your public filters here

set(_PublicFilters
  AlignSectionsFeature
  AlignSectionsFeatureCentroid
  AlignSectionsList
  AlignSectionsMisorientation
  AlignSectionsMutualInformation
  CAxisSegmentFeatures
  EBSDSegmentFeatures
  MergeColonies
  MergeTwins
  ScalarSegmentFeatures
  ComputeFeatureRect
  PartitionGeometry
)


list(LENGTH _PublicFilters PluginNumFilters)
set_property(GLOBAL PROPERTY PluginNumFilters ${PluginNumFilters})

#--------------
# Loop on all the filters adding each one. In this loop we default to making each filter exposed in the user
# interface in DREAM3D. If you want to have the filter compiled but NOT exposed to the user then use the next loop
foreach(f ${_PublicFilters} )
  ADD_SIMPL_FILTER(  "Reconstruction" "Reconstruction"
                        ${_filterGroupName} ${f}
                        ${Reconstruction_SOURCE_DIR}/Documentation/${_filterGroupName}/${f}.md TRUE)
endforeach()


#---------------
# This is the list of Private Filters. These filters are available from other filters but the user will not
# be able to use them from the DREAM3D user interface.
set(_PrivateFilters
  GroupMicroTextureRegions
  IdentifyMicroTextureRegions
  SineParamsSegmentFeatures
  VectorSegmentFeatures
)

#-----------------
# Loop on the Private Filters adding each one to the DREAM3DLib project so that it gets compiled.
foreach(f ${_PrivateFilters} )
  ADD_SIMPL_FILTER(  "Reconstruction" "Reconstruction"
                        ${_filterGroupName} ${f}
                        ${${PLUGIN_NAME}_SOURCE_DIR}/Documentation/${_filterGroupName}/${f}.md FALSE)
endforeach()

SIMPL_END_FILTER_GROUP(${Reconstruction_BINARY_DIR} "${_filterGroupName}" "Reconstruction Filters")

