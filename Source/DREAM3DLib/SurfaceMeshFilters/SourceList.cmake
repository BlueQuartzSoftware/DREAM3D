#--////////////////////////////////////////////////////////////////////////////
#-- Your License or copyright can go here
#--////////////////////////////////////////////////////////////////////////////

set(DREAM3D_FILTER_GROUP_NAME SurfaceMeshFilters)
set(${DREAM3D_FILTER_GROUP_NAME}_FILTERS_HDRS "")

START_FILTER_GROUP("${DREAM3D_FILTER_GROUP_NAME}")

ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} QuickSurfaceMesh QuickSurfaceMesh.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} M3CEntireVolume M3CEntireVolume.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} M3CSliceBySlice M3CSliceBySlice.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} MovingFiniteElementSmoothing MovingFiniteElementSmoothing.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} LaplacianSmoothing LaplacianSmoothing.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} GenerateNodeTriangleConectivity GenerateNodeTriangleConectivity.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} GenerateUniqueEdges GenerateUniqueEdges.html "" FALSE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} TriangleCentroidFilter TriangleCentroidFilter.html "" FALSE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} TriangleNormalFilter TriangleNormalFilter.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} SharedGrainFaceFilter SharedGrainFaceFilter.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} ReverseTriangleWinding ReverseTriangleWinding.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} VerifyTriangleWinding VerifyTriangleWinding.html "" TRUE)


ADD_DREAM3D_SUPPORT_HEADER(${DREAM3D_FILTER_GROUP_NAME} MeshFunctions.h)
ADD_DREAM3D_SUPPORT_HEADER(${DREAM3D_FILTER_GROUP_NAME} MeshLinearAlgebra.h)

ADD_DREAM3D_SUPPORT_HEADER(${DREAM3D_FILTER_GROUP_NAME} FindNRingNeighbors.h)
ADD_DREAM3D_SUPPORT_SOURCE(${DREAM3D_FILTER_GROUP_NAME} FindNRingNeighbors.cpp)

if(EIGEN_FOUND)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} GrainFaceCurvatureFilter GrainFaceCurvatureFilter.html "" TRUE)

ADD_DREAM3D_SUPPORT_HEADER(${DREAM3D_FILTER_GROUP_NAME} CalculateTriangleGroupCurvatures.h)
ADD_DREAM3D_SUPPORT_SOURCE(${DREAM3D_FILTER_GROUP_NAME} CalculateTriangleGroupCurvatures.cpp)
endif()
