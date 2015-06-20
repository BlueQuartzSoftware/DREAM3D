VTK Structured Points Reader  {#vtkstructuredpointsreader}
============

## Group (Subgroup) ##
IO (Input)

## Description ##
This Filter reads an _STRUCTURED POINTS_ type of 3D array from a legacy .vtk file. The currently supported VTK dataset attribute types are SCALARS and VECTORS. Other dataset attributes will not be read correctly and may cause issues when running the Filter. The VTK data must be _POINT DATA_ and/or _CELL DATA_ and can be either binary or ASCII. The Filter will create a new **Data Container** with an **Image** Geometry along with the necessary types of **Attribute Matrix** depending on what type of data is being read.

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
| Input VVTK File | File Path | Input .vtk file path |
| Read Point Data | Boolean | Whether the read point data from the .vtk file |
| Read Cell Data | Boolean | Whether the read point data from the .vtk file |
| Point Data Data Container | String | Created **Data Container** name. Only needed if _Read Point Data_ is checked |
| Vertex Attribute Matrix | String | Created **Vertex Attribute Matrix** name. Only needed if _Read Point Data_ is checked |
| Cell Data Data Container | String | Created **Data Container** name. Only needed if _Read Cell Data_ is checked |
| Cell Attribute Matrix | String | Created **Cell Attribute Matrix** name. Only needed if _Read Cell Data_ is checked |

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


