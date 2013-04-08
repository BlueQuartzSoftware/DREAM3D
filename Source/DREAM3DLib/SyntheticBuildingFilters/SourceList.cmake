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

set(DREAM3D_FILTER_GROUP_NAME SyntheticBuildingFilters)
set(${DREAM3D_FILTER_GROUP_NAME}_FILTERS_HDRS "")

START_FILTER_GROUP(${FilterWidgetsLib_BINARY_DIR} "${DREAM3D_FILTER_GROUP_NAME}" "Synthetic Building Filters")

ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} PackPrimaryPhases TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} MatchCrystallography TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} JumbleOrientations TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} InsertPrecipitatePhases TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} EstablishMatrixPhase TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} AddBadData TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} AddOrientationNoise TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} InitializeSyntheticVolume TRUE)

END_FILTER_GROUP(${FilterWidgetsLib_BINARY_DIR} "${DREAM3D_FILTER_GROUP_NAME}" "Synthetic Building Filters")

