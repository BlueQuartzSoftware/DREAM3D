#--////////////////////////////////////////////////////////////////////////////
#-- Your License or copyright can go here
#--////////////////////////////////////////////////////////////////////////////

set(_filterGroupName HEDMAnalysisFilters)
set(${_filterGroupName}_FILTERS_HDRS "")

#--------
# This macro must come first before we start adding any filters
START_FILTER_GROUP(${HEDMAnalysis_BINARY_DIR} "${_filterGroupName}" "HEDMAnalysis")


#---------
# List your public filters here
set(_PublicFilters
  ReadMicData
  TesselateFarFieldGrains
)


#--------------
# Loop on all the filters adding each one. In this loop we default to making each filter exposed in the user
# interface in DREAM3D. If you want to have the filter compiled but NOT exposed to the user then use the next loop
foreach(f ${_PublicFilters} )
  ADD_DREAM3D_FILTER(  "HEDMAnalysis" "HEDMAnalysis"
                        ${_filterGroupName} ${f}
                        ${HEDMAnalysis_SOURCE_DIR}/Documentation/${_filterGroupName}/${f}.md TRUE)
endforeach()


#---------------
# This is the list of Private Filters. These filters are available from other filters but the user will not
# be able to use them from the DREAM3D user interface.
set(_PrivateFilters

)

#-----------------
# Loop on the Private Filters adding each one to the DREAM3DLib project so that it gets compiled.
foreach(f ${_PrivateFilters} )
  ADD_DREAM3D_FILTER(  "HEDMAnalysis" "HEDMAnalysis"
                        ${_filterGroupName} ${f}
                        ${DREAM3DLib_FILTER_DOC_DIR}/${_filterGroupName}/${f}.md FALSE)
endforeach()


ADD_DREAM3D_SUPPORT_HEADER(${HEDMAnalysis_SOURCE_DIR} ${_filterGroupName} HEDM/H5MicImporter.h)
ADD_DREAM3D_SUPPORT_SOURCE(${HEDMAnalysis_SOURCE_DIR} ${_filterGroupName} HEDM/H5MicImporter.cpp)
ADD_DREAM3D_SUPPORT_HEADER(${HEDMAnalysis_SOURCE_DIR} ${_filterGroupName} HEDM/H5MicReader.h)
ADD_DREAM3D_SUPPORT_SOURCE(${HEDMAnalysis_SOURCE_DIR} ${_filterGroupName} HEDM/H5MicReader.cpp)
ADD_DREAM3D_SUPPORT_HEADER(${HEDMAnalysis_SOURCE_DIR} ${_filterGroupName} HEDM/H5MicVolumeReader.h)
ADD_DREAM3D_SUPPORT_SOURCE(${HEDMAnalysis_SOURCE_DIR} ${_filterGroupName} HEDM/H5MicVolumeReader.cpp)
ADD_DREAM3D_SUPPORT_HEADER(${HEDMAnalysis_SOURCE_DIR} ${_filterGroupName} HEDM/MicFields.h)
ADD_DREAM3D_SUPPORT_SOURCE(${HEDMAnalysis_SOURCE_DIR} ${_filterGroupName} HEDM/MicFields.cpp)
ADD_DREAM3D_SUPPORT_HEADER(${HEDMAnalysis_SOURCE_DIR} ${_filterGroupName} HEDM/MicPhase.h)
ADD_DREAM3D_SUPPORT_SOURCE(${HEDMAnalysis_SOURCE_DIR} ${_filterGroupName} HEDM/MicPhase.cpp)
ADD_DREAM3D_SUPPORT_HEADER(${HEDMAnalysis_SOURCE_DIR} ${_filterGroupName} HEDM/MicReader.h)
ADD_DREAM3D_SUPPORT_SOURCE(${HEDMAnalysis_SOURCE_DIR} ${_filterGroupName} HEDM/MicReader.cpp)
ADD_DREAM3D_SUPPORT_HEADER(${HEDMAnalysis_SOURCE_DIR} ${_filterGroupName} HEDM/MicHeaderEntry.h)
ADD_DREAM3D_SUPPORT_HEADER(${HEDMAnalysis_SOURCE_DIR} ${_filterGroupName} HEDM/MicConstants.h)



#---------------------
# This macro must come last after we are done adding all the filters and support files.
END_FILTER_GROUP(${HEDMAnalysis_BINARY_DIR} "${_filterGroupName}" "HEDMAnalysis")

