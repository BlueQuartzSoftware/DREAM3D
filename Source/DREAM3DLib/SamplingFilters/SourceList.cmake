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

set (DREAM3D_FILTER_GROUP_NAME SamplingFilters)
set (${DREAM3D_FILTER_GROUP_NAME}_FILTERS_HDRS "")

START_FILTER_GROUP("${DREAM3D_FILTER_GROUP_NAME}")

ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} ChangeResolution ChangeResolution.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} RegularizeZSpacing RegularizeZSpacing.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} RotateSampleRefFrame RotateSampleRefFrame.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} CropVolume CropVolume.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} Hex2SqrConverter Hex2SqrConverter.html "" TRUE)

