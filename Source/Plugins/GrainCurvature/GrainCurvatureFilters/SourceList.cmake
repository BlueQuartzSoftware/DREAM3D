#--////////////////////////////////////////////////////////////////////////////
#-- Your License or copyright can go here
#--////////////////////////////////////////////////////////////////////////////

set (DREAM3D_FILTER_GROUP_NAME GrainCurvatureFilters)
set (${DREAM3D_FILTER_GROUP_NAME}_FILTERS_HDRS "")

START_FILTER_GROUP("${DREAM3D_FILTER_GROUP_NAME}")


ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} GrainCurvatureFilter GrainCurvatureFilter.html "" TRUE)

ADD_DREAM3D_SUPPORT_HEADER(${DREAM3D_FILTER_GROUP_NAME} CalculateTriangleGroupCurvatures.h)
ADD_DREAM3D_SUPPORT_SOURCE(${DREAM3D_FILTER_GROUP_NAME} CalculateTriangleGroupCurvatures.cpp)
