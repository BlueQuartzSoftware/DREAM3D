#--////////////////////////////////////////////////////////////////////////////
#-- Your License or copyright can go here
#--////////////////////////////////////////////////////////////////////////////

set (DREAM3D_FILTER_GROUP_NAME SurfaceMeshFilters)
set (${DREAM3D_FILTER_GROUP_NAME}_FILTERS_HDRS "")

START_FILTER_GROUP("${DREAM3D_FILTER_GROUP_NAME}")

ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} M3CEntireVolume M3CEntireVolume.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} M3CSliceBySlice M3CSliceBySlice.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} MovingFiniteElementSmoothing MovingFiniteElementSmoothing.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} LaplacianSmoothing LaplacianSmoothing.html "" TRUE)
#ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} QSAMCurvature QSAMCurvature.html "" TRUE)

ADD_DREAM3D_SUPPORT_HEADER(${DREAM3D_FILTER_GROUP_NAME} MeshFunctions.h)
ADD_DREAM3D_SUPPORT_HEADER(${DREAM3D_FILTER_GROUP_NAME} MeshLinearAlgebra.h)
#ADD_DREAM3D_SUPPORT_CLASS(${DREAM3D_FILTER_GROUP_NAME} AbstractSurfaceMeshAlgorithm)

