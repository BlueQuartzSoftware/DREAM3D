Vtk Structured Points Reader  {#vtkstructuredpointsreader}
======

## Group (Subgroup) ##
IO Filters

## Description ##

This filter reads an STRUCTURED_POINTS type of 3D array from a legacy .vtk file. The
currently supported vtk dataset attribute types are SCALARS and VECTORS. More will
come eventually. The vtk data must be POINT_DATA and CELL_DATA

The reader should be able to handle both Binary and ASCII data.

The filter will create a new DataContainer with a Structured Points Geometry and will
then create a new AttributeMatrix inside the new Data Container.

### Sample File ###

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

| Name | Type |
|------|------|
| Input Vtk File | Input File |
| DataContainer Name | String |
| Attribute Matrix Name | String |

## Required DataContainers ##
None. This filter will create one

## Required Arrays ##
None. This filter will create data arrays


## Created Arrays ##
One data array for each data set attribute in the vtk file.



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


