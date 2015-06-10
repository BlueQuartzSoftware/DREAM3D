#--////////////////////////////////////////////////////////////////////////////
#-- Your License or copyright can go here
#--////////////////////////////////////////////////////////////////////////////

set(_filterGroupName ReconstructionFilters)
set(${_filterGroupName}_FILTERS_HDRS "")

START_FILTER_GROUP(${Reconstruction_BINARY_DIR} "${_filterGroupName}" "Reconstruction Filters")

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
)

#--------------
# Loop on all the filters adding each one. In this loop we default to making each filter exposed in the user
# interface in DREAM3D. If you want to have the filter compiled but NOT exposed to the user then use the next loop
foreach(f ${_PublicFilters} )
  ADD_DREAM3D_FILTER(  "Reconstruction" "Reconstruction"
                        ${_filterGroupName} ${f}
                        ${Reconstruction_SOURCE_DIR}/Documentation/${_filterGroupName}/${f}.md TRUE)
endforeach()


#---------------
# This is the list of Private Filters. These filters are available from other filters but the user will not
# be able to use them from the DREAM3D user interface.
set(_PrivateFilters
  SegmentFeatures
  GroupFeatures
  AlignSections
  GroupMicroTextureRegions
  IdentifyMicroTextureRegions
  SineParamsSegmentFeatures
  VectorSegmentFeatures
)

#-----------------
# Loop on the Private Filters adding each one to the DREAM3DLib project so that it gets compiled.
foreach(f ${_PrivateFilters} )
  ADD_DREAM3D_FILTER(  "Reconstruction" "Reconstruction"
                        ${_filterGroupName} ${f}
                        ${Reconstruction_SOURCE_DIR}/Documentation/${_filterGroupName}/${f}.md FALSE)
endforeach()

END_FILTER_GROUP(${Reconstruction_BINARY_DIR} "${_filterGroupName}" "Reconstruction Filters")

