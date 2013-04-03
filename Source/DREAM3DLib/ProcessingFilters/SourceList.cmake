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

set(DREAM3D_FILTER_GROUP_NAME ProcessingFilters)
set(${DREAM3D_FILTER_GROUP_NAME}_FILTERS_HDRS "")

START_FILTER_GROUP(${FilterWidgetsLib_BINARY_DIR} "${DREAM3D_FILTER_GROUP_NAME}")

ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} MinNeighbors TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} FlattenImage TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} MinSize TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} ClearData TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} PerPhaseMinSize TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} FillBadData TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} OpenCloseBadData TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} OpenCloseCoordinationNumber TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} ConvertEulerAngles TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} StandardizeEulerAngles FALSE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} RotateEulerRefFrame TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} IdentifySample TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} SingleThresholdCells TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} MultiThresholdCells TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} SingleThresholdFields TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} MultiThresholdFields TRUE)



END_FILTER_GROUP(${FilterWidgetsLib_BINARY_DIR} "${DREAM3D_FILTER_GROUP_NAME}")


