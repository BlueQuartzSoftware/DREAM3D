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


set (DREAM3D_FILTER_GROUP_NAME ReconstructionFilters)
set (${DREAM3D_FILTER_GROUP_NAME}_FILTERS_HDRS "")

START_FILTER_GROUP("${DREAM3D_FILTER_GROUP_NAME}")

ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} AlignSectionsMisorientation AlignSectionsMisorientation.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} AlignSectionsMutualInformation AlignSectionsMutualInformation.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} AlignSectionsFeature AlignSectionsFeature.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} AlignSectionsFeatureCentroid AlignSectionsFeatureCentroid.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} AlignSectionsList AlignSectionsList.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} EBSDSegmentGrains EBSDSegmentGrains.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} CAxisSegmentGrains CAxisSegmentGrains.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} MergeTwins MergeTwins.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} MergeColonies MergeColonies.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} GroupMicroTextureRegions GroupMicroTextureRegions.html "" TRUE)


ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} SegmentGrains SegmentGrains.html "" FALSE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} AlignSections AlignSections.html "" FALSE)
