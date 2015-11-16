#--////////////////////////////////////////////////////////////////////////////
#-- Your License or copyright can go here
#--////////////////////////////////////////////////////////////////////////////

set(_filterGroupName SurfaceMeshingFilters)
set(${_filterGroupName}_FILTERS_HDRS "")

START_FILTER_GROUP(${SurfaceMeshing_BINARY_DIR} "${_filterGroupName}" "Surface Meshing Filters")

#---------
# List your public filters here

set(_PublicFilters
  FindGBCD
  FindGBCDMetricBased
  GenerateFaceIPFColoring
  GenerateFaceMisorientationColoring
  GenerateGeometryConnectivity
  LaplacianSmoothing
  QuickSurfaceMesh
  ReverseTriangleWinding
  SharedFeatureFaceFilter
  TriangleAreaFilter
  TriangleCentroidFilter
  TriangleDihedralAngleFilter
  TriangleNormalFilter
)

if(EIGEN_FOUND)
  set(_PublicFilters ${_PublicFilters} FeatureFaceCurvatureFilter)

  ADD_DREAM3D_SUPPORT_HEADER(${SurfaceMeshing_SOURCE_DIR} ${_filterGroupName} CalculateTriangleGroupCurvatures.h)
  ADD_DREAM3D_SUPPORT_SOURCE(${SurfaceMeshing_SOURCE_DIR} ${_filterGroupName} CalculateTriangleGroupCurvatures.cpp)
endif()

#--------------
# Loop on all the filters adding each one. In this loop we default to making each filter exposed in the user
# interface in DREAM3D. If you want to have the filter compiled but NOT exposed to the user then use the next loop
foreach(f ${_PublicFilters} )
  ADD_DREAM3D_FILTER(  "SurfaceMeshing" "SurfaceMeshing"
                        ${_filterGroupName} ${f}
                        ${SurfaceMeshing_SOURCE_DIR}/Documentation/${_filterGroupName}/${f}.md TRUE)
endforeach()


#---------------
# This is the list of Private Filters. These filters are available from other filters but the user will not
# be able to use them from the DREAM3D user interface.
set(_PrivateFilters
  FindBoundaryAreas
  GenerateFaceSchuhMisorientationColoring
  # These filters require extensive updates to comply with the IGeometry design
  #M3CSliceBySlice
  #MovingFiniteElementSmoothing
  #VerifyTriangleWinding
)

#-----------------
# Loop on the Private Filters adding each one to the DREAM3DLib project so that it gets compiled.
foreach(f ${_PrivateFilters} )
  ADD_DREAM3D_FILTER(  "SurfaceMeshing" "SurfaceMeshing"
                        ${_filterGroupName} ${f}
                        ${DREAM3DLib_FILTER_DOC_DIR}/${_filterGroupName}/${f}.md FALSE)
endforeach()


#-------------
# These are files that need to be compiled into DREAM3DLib but are NOT filters
ADD_DREAM3D_SUPPORT_MOC_HEADER(${SurfaceMeshing_SOURCE_DIR} ${_filterGroupName} SurfaceMeshFilter.h)
ADD_DREAM3D_SUPPORT_SOURCE(${SurfaceMeshing_SOURCE_DIR} ${_filterGroupName} SurfaceMeshFilter.cpp)

ADD_DREAM3D_SUPPORT_MOC_HEADER(${SurfaceMeshing_SOURCE_DIR} ${_filterGroupName} BinaryNodesTrianglesReader.h)
ADD_DREAM3D_SUPPORT_SOURCE(${SurfaceMeshing_SOURCE_DIR} ${_filterGroupName} BinaryNodesTrianglesReader.cpp)

ADD_DREAM3D_SUPPORT_HEADER(${SurfaceMeshing_SOURCE_DIR} ${_filterGroupName} MeshFunctions.h)
ADD_DREAM3D_SUPPORT_HEADER(${SurfaceMeshing_SOURCE_DIR} ${_filterGroupName} MeshLinearAlgebra.h)

ADD_DREAM3D_SUPPORT_HEADER(${SurfaceMeshing_SOURCE_DIR} ${_filterGroupName} FindNRingNeighbors.h)
ADD_DREAM3D_SUPPORT_SOURCE(${SurfaceMeshing_SOURCE_DIR} ${_filterGroupName} FindNRingNeighbors.cpp)

ADD_DREAM3D_SUPPORT_HEADER(${SurfaceMeshing_SOURCE_DIR} ${_filterGroupName} util/Vector3.h)
ADD_DREAM3D_SUPPORT_SOURCE(${SurfaceMeshing_SOURCE_DIR} ${_filterGroupName} util/Vector3.cpp)

ADD_DREAM3D_SUPPORT_HEADER(${SurfaceMeshing_SOURCE_DIR} ${_filterGroupName} util/Plane.h)
ADD_DREAM3D_SUPPORT_SOURCE(${SurfaceMeshing_SOURCE_DIR} ${_filterGroupName} util/Plane.cpp)

ADD_DREAM3D_SUPPORT_HEADER(${SurfaceMeshing_SOURCE_DIR} ${_filterGroupName} util/TriangleOps.h)
ADD_DREAM3D_SUPPORT_SOURCE(${SurfaceMeshing_SOURCE_DIR} ${_filterGroupName} util/TriangleOps.cpp)

#ADD_DREAM3D_SUPPORT_HEADER(${SurfaceMeshing_SOURCE_DIR} ${_filterGroupName} util/Exception.h)
#ADD_DREAM3D_SUPPORT_HEADER(${SurfaceMeshing_SOURCE_DIR} ${_filterGroupName} util/InvalidParameterException.h)



END_FILTER_GROUP(${SurfaceMeshing_BINARY_DIR} "${_filterGroupName}" "Surface Meshing Filters")

