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

set (DREAM3D_FILTER_GROUP_NAME ProcessingFilters)
set (${DREAM3D_FILTER_GROUP_NAME}_FILTERS_HDRS "")

START_FILTER_GROUP(${FilterWidgetsLib_BINARY_DIR} "${DREAM3D_FILTER_GROUP_NAME}")

ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} MinNeighbors MinNeighbors.html "" TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} FlattenImage FlattenImage.html "" TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} MinSize MinSize.html "" TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} ClearData ClearData.html "" TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} PerPhaseMinSize PerPhaseMinSize.html "" TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} FillBadData FillBadData.html "" TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} OpenCloseBadData OpenCloseBadData.html "" TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} OpenCloseCoordinationNumber OpenCloseCoordinationNumber.html "" TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} ConvertEulerAngles ConvertEulerAngles.html "" TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} StandardizeEulerAngles StandardizeEulerAngles.html "" FALSE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} RotateEulerRefFrame RotateEulerRefFrame.html "" TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} IdentifySample IdentifySample.html "" TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} SingleThresholdCells SingleThresholdCells.html "" TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} MultiThresholdCells MultiThresholdCells.html "" TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} SingleThresholdFields SingleThresholdFields.html "" TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} MultiThresholdFields MultiThresholdFields.html "" TRUE)
