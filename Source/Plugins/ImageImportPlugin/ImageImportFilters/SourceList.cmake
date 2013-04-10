#--////////////////////////////////////////////////////////////////////////////
#-- Your License or copyright can go here
#--////////////////////////////////////////////////////////////////////////////

set(DREAM3D_FILTER_GROUP_NAME ImageImportFilters)
set(${DREAM3D_FILTER_GROUP_NAME}_FILTERS_HDRS "")

START_FILTER_GROUP(${ImageImportPlugin_BINARY_DIR} "${DREAM3D_FILTER_GROUP_NAME}" "Image Import Filters")


ADD_DREAM3D_FILTER( "ImageImportPlugin" "ImageImportPlugin" ${DREAM3D_FILTER_GROUP_NAME} ImportImageStack TRUE)


END_FILTER_GROUP(${ImageImportPlugin_BINARY_DIR} "${DREAM3D_FILTER_GROUP_NAME}" "Image Import Filters")

