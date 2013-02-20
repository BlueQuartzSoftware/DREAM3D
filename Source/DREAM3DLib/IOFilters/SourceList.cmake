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


set (DREAM3D_FILTER_GROUP_NAME IOFilters)
set (${DREAM3D_FILTER_GROUP_NAME}_FILTERS_HDRS "")
START_FILTER_GROUP("${DREAM3D_FILTER_GROUP_NAME}")

ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} AvizoRectilinearCoordinateWriter AvizoRectilinearCoordinateWriter.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} AvizoUniformCoordinateWriter AvizoUniformCoordinateWriter.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} DataContainerReader DataContainerReader.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} DataContainerWriter DataContainerWriter.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} DxReader DxReader.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} DxWriter DxWriter.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} EbsdToH5Ebsd EbsdToH5Ebsd.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} EnsembleInfoReader EnsembleInfoReader.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} FieldDataCSVWriter FieldDataCSVWriter.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} FieldInfoReader FieldInfoReader.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} H5VoxelFileReader H5VoxelFileReader.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} INLWriter INLWriter.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} NodesTrianglesToStl NodesTrianglesToStl.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} NodesTrianglesToVtk NodesTrianglesToVtk.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} PhReader PhReader.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} PhWriter PhWriter.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} ReadH5Ebsd ReadH5Ebsd.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} SurfaceMeshToNodesTrianglesEdges SurfaceMeshToNodesTrianglesEdges.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} SurfaceMeshToStl SurfaceMeshToStl.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} SurfaceMeshToVtk SurfaceMeshToVtk.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} SurfaceMeshToNonconformalVtk SurfaceMeshToNonconformalVtk.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} VtkRectilinearGridWriter VtkRectilinearGridWriter.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} YSChoiAbaqusReader YSChoiAbaqusReader.html "" TRUE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} RawBinaryReader RawBinaryReader.html "" FALSE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} VtkGrainIdReader VtkGrainIdReader.html "" TRUE)
# This is a reader to read some Data from Jack Goldfeather to validate the Curvature Calculation codes
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} GoldfeatherReader GoldfeatherReader.html "" FALSE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} SolidMeshToVtk SolidMeshToVtk.html "" TRUE)

# These are private, internal use only, filters
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} VoxelDataContainerWriter VoxelDataContainerWriter.html "" FALSE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} VoxelDataContainerReader VoxelDataContainerReader.html "" FALSE)

ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} SurfaceMeshDataContainerWriter SurfaceMeshDataContainerWriter.html "" FALSE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} SurfaceMeshDataContainerReader SurfaceMeshDataContainerReader.html "" FALSE)

ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} SolidMeshDataContainerWriter SolidMeshDataContainerWriter.html "" FALSE)
ADD_DREAM3D_FILTER(${DREAM3D_FILTER_GROUP_NAME} SolidMeshDataContainerReader SolidMeshDataContainerReader.html "" FALSE)
