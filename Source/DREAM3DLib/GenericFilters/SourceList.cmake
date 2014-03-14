#--////////////////////////////////////////////////////////////////////////////
#--
#--  Copyright (c) 2011, Michael A. Jackson. BlueQuartz Software
#--  Copyright (c) 2011, Michael Groeber, US Air Force Research Laboratory
#--  All rights reserved.
#--  BSD License: http://www.opensource.org/licenses/bsd-license.html
#--
#-- This code was partly written under US Air Force Contract FA8650-07-D-5800
#--
#--////////////////////////////////////////////////////////////////////////////

set(_filterGroupName GenericFilters)
set(${_filterGroupName}_FILTERS_HDRS "")

START_FILTER_GROUP(${FilterWidgetsLib_BINARY_DIR} "${_filterGroupName}" "Generic Filters")

#---------
# List your public filters here

set(_PublicFilters
  AdjustVolumeOrigin
  ConvertData
  CopyFeatureArrayToCellArray
  CreateFeatureArrayFromCellArray
  FindBoundingBoxFeatures
  FindCellQuats
  FindFeatureCentroids
  FindFeaturePhases
  FindSurfaceCells
  FindSurfaceFeatures
  GenerateIPFColors
  GenerateMisorientationColors
  GenerateRodriguesColors
  GenerateVectorColors
  LinkFeatureMapToCellArray
  RemoveArrays
  RenameAttributeMatrix
  RenameAttributeArray
  ScaleVolume
  UpdateCellQuats
)


#--------------
# Loop on all the filters adding each one. In this loop we default to making each filter exposed in the user
# interface in DREAM3D. If you want to have the filter compiled but NOT exposed to the user then use the next loop
foreach(f ${_PublicFilters} )
  ADD_DREAM3D_FILTER(  "DREAM3DLib" "FilterWidgetsLib"
                        ${_filterGroupName} ${f}
                        ${DREAM3DLib_FILTER_DOC_DIR}/${_filterGroupName}/${f}.md TRUE)
endforeach()


#---------------
# This is the list of Private Filters. These filters are available from other filters but the user will not
# be able to use them from the DREAM3D user interface.
set(_PrivateFilters
  EmptyFilter
  GenerateEulerColors
)

#-----------------
# Loop on the Private Filters adding each one to the DREAM3DLib project so that it gets compiled.
foreach(f ${_PrivateFilters} )
  ADD_DREAM3D_FILTER(  "DREAM3DLib" "FilterWidgetsLib"
                        ${_filterGroupName} ${f}
                        ${DREAM3DLib_FILTER_DOC_DIR}/${_filterGroupName}/${f}.md FALSE)
endforeach()


#-----------------
# This makes sure some Filter Group meta data is written to build files that is needed later
END_FILTER_GROUP(${FilterWidgetsLib_BINARY_DIR} "${_filterGroupName}" "Generic Filters")
