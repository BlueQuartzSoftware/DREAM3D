VTK STRUCTURED_POINTS Reader  {#vtkstructuredpointsreader}
============

## Group (Subgroup) ##
IO (Input)

## Description ##
This Filter reads a _STRUCTURED_POINTS_ type of 3D array from a legacy .vtk file. A _STRUCTURED_POINTS_ file is a more general type of **Image** geometry where data can be stored at the vertices of each voxel. The currently supported VTK dataset attribute types are SCALARS and VECTORS. Other dataset attributes will not be read correctly and may cause issues when running the Filter. The VTK data must be _POINT_DATA_ and/or _CELL_DATA_ and can be either binary or ASCII. The Filter will create a new **Data Container** with an **Image** geometry for each of the types of data (i.e., _POINT_DATA_ and/or _CELL_DATA_) selected to be read, along with a **Cell Attribute Matrix** to hold the imported data.

*Note:* In a _STRUCTURED_POINTS_ file, _POINT_DATA_ lies on the vertices of each unit element voxel (i.e., eight values per voxel), while _CELL_DATA_ lies at the voxel center.  This Filter will import *both* types of data as **Image** geometries, since either form a structured rectilinear grid.  This is to enable easier visualization of the _POINT_DATA_, and to enable greater flexibility when using DREAM.3D analysis tools, many of which rely on an **Image** geometry.

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
| Read Point Data | Boolean | Whether the read point data from the .vtk file |
| Read Cell Data | Boolean | Whether the read cell data from the .vtk file |
| Point Data Data Container | String | Created **Data Container** name. Only needed if _Read Point Data_ is checked |
| Point Data Attribute Matrix | String | Created **Vertex Attribute Matrix** name. Only needed if _Read Point Data_ is checked |
| Cell Data Data Container | String | Created **Data Container** name. Only needed if _Read Cell Data_ is checked |
| Cell Data Attribute Matrix | String | Created **Cell Attribute Matrix** name. Only needed if _Read Cell Data_ is checked |

## Required Geometry ##
Not Applicable

## Required Arrays ##
None

## Created Arrays ##
None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


