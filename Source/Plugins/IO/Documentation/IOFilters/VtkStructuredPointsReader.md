VTK STRUCTURED_POINTS Reader  {#vtkstructuredpointsreader}
============

## Group (Subgroup) ##
IO (Input)

## Description ##
This **Filter** reads a _STRUCTURED_POINTS_ type of 3D array from a legacy .vtk file. A _STRUCTURED_POINTS_ file is a more general type of **Image Geometry** where data can be stored at the vertices of each voxel. The currently supported VTK dataset attribute types are SCALARS and VECTORS. Other dataset attributes will not be read correctly and may cause issues when running the **Filter**. The VTK data must be _POINT_DATA_ and/or _CELL_DATA_ and can be either binary or ASCII. The **Filter** will create a new **Data Container** with an **Image** geometry for each of the types of data (i.e., _POINT_DATA_ and/or _CELL_DATA_) selected to be read, along with a **Cell Attribute Matrix** to hold the imported data.

*Note:* In a _STRUCTURED_POINTS_ file, _POINT_DATA_ lies on the vertices of each unit element voxel (i.e., eight values per voxel), while _CELL_DATA_ lies at the voxel center.  This Filter will import *both* types of data as **Image Geometries**, since either form a structured rectilinear grid.  This is to enable easier visualization of the _POINT_DATA_, and to enable greater flexibility when using DREAM.3D analysis tools, many of which rely on an **Image Geometry**.

### Example Input ###

    # vtk DataFile Version 2.0
    GrainIds Stored in Vtk File
    ASCII
    DATASET STRUCTURED_POINTS
    DIMENSIONS 3 4 6
    SPACING 1 1 1
    ORIGIN 0 0 0
    POINT_DATA 72
    SCALARS GrainIds char 1
    LOOKUP_TABLE default
    0 0 0 0 0 0 0 0 0 0 0 0
    0 5 10 15 20 25 25 20 15
    10 5 0 0 10 20 30 40 50
    50 40 30 20 10 0 0 10 20
    30 40 50 50 40 30 20 10 0
    0 5 10 15 20 25 25 20 15
    10 5 0 0 0 0 0 0 0 0 0 0 0 0 0


## Parameters ##
| Name | Type | Description |
|------|------|-------------|
| Input VTK File | File Path | Input .vtk file path |
| Read Point Data | bool | Whether the read point data from the .vtk file |
| Read Cell Data | bool | Whether the read cell data from the .vtk file |

## Required Geometry ##
Not Applicable

## Required Objects ##
None

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Data Container** | ImageDataContainer_PointData | N/A | N/A | Created **Data Container** name with an **Image Geometry** storing POINT_DATA. Only created if _Read Point Data_ is checked |
| **Data Container** | ImageDataContainer_CellData | N/A | N/A | Created **Data Container** name with an **Image Geometry** storing CELL_DATA. Only created if _Read Cell Data_ is checked |
| **Attribute Matrix** | CellData | Cell | N/A | Created **Cell Attribute Matrix** storing POINT_DATA. Only created if _Read Point Data_ is checked |
| **Attribute Matrix** | CellData | Cell | N/A | Created **Cell Attribute Matrix** storing CELL_DATA. Only created if _Read Cell Data_ is checked |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


