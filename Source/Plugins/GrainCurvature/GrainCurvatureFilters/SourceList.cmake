#--////////////////////////////////////////////////////////////////////////////
#-- Your License or copyright can go here
#--////////////////////////////////////////////////////////////////////////////

set (DREAM3D_FILTER_GROUP_NAME GrainCurvatureFilters)
set (${DREAM3D_FILTER_GROUP_NAME}_FILTERS_HDRS "")

START_FILTER_GROUP(${GrainCurvaturePlugin_BINARY_DIR} "${DREAM3D_FILTER_GROUP_NAME}")


ADD_DREAM3D_FILTER( "GrainCurvature" ${GrainCurvaturePlugin_BINARY_DIR} ${DREAM3D_FILTER_GROUP_NAME} ImageImportFilter ImageImportFilter.html "" TRUE)


