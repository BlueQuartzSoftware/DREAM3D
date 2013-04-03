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


set(DREAM3D_FILTER_GROUP_NAME IOFilters)
set(${DREAM3D_FILTER_GROUP_NAME}_FILTERS_HDRS "")
START_FILTER_GROUP(${FilterWidgetsLib_BINARY_DIR} "${DREAM3D_FILTER_GROUP_NAME}")

ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} AvizoRectilinearCoordinateWriter TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} AvizoUniformCoordinateWriter TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} DataContainerReader TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} DataContainerWriter TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} DxReader TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} DxWriter TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} EbsdToH5Ebsd TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} EnsembleInfoReader TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} FieldDataCSVWriter TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} FieldInfoReader TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} H5VoxelFileReader TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} INLWriter TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} NodesTrianglesToStl TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} NodesTrianglesToVtk TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} PhReader TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} PhWriter TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} ReadH5Ebsd TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} SurfaceMeshToNodesTrianglesEdges TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} SurfaceMeshToStl TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} SurfaceMeshToVtk TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} SurfaceMeshToNonconformalVtk TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} VtkRectilinearGridWriter TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} YSChoiAbaqusReader TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} RawBinaryReader TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} VtkGrainIdReader TRUE)
# This is a reader to read some Data from Jack Goldfeather to validate the Curvature Calculation codes
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} GoldfeatherReader FALSE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} SolidMeshToVtk TRUE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} SPParksWriter TRUE)

# These are private, internal use only, filters
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} VoxelDataContainerWriter FALSE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} VoxelDataContainerReader FALSE)

ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} SurfaceMeshDataContainerWriter FALSE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} SurfaceMeshDataContainerReader FALSE)

ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} SolidMeshDataContainerWriter FALSE)
ADD_DREAM3D_FILTER( "DREAM3DLib" "FilterWidgetsLib" ${DREAM3D_FILTER_GROUP_NAME} SolidMeshDataContainerReader FALSE)

END_FILTER_GROUP(${FilterWidgetsLib_BINARY_DIR} "${DREAM3D_FILTER_GROUP_NAME}")
