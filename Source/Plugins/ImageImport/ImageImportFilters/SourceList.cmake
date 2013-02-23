#--////////////////////////////////////////////////////////////////////////////
#-- Your License or copyright can go here
#--////////////////////////////////////////////////////////////////////////////

set (DREAM3D_FILTER_GROUP_NAME ImageImportFilters)
set (${DREAM3D_FILTER_GROUP_NAME}_FILTERS_HDRS "")

START_FILTER_GROUP(${ImageImportPlugin_BINARY_DIR} "${DREAM3D_FILTER_GROUP_NAME}")


ADD_DREAM3D_FILTER( "ImageImport" "ImageImportPlugin" ${DREAM3D_FILTER_GROUP_NAME} ImportImageStack ImportImageStack.html "" TRUE)

