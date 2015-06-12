Read Ph File (Feature Ids) {#phreader}
======

## Group (Subgroup) ##
I/O Filters (Input)


## Description ##
This Filter reads a custom data file used by Carnegie-Mellon Univ. The file format is a simple ASCII Text file with the following contents.

**Format**

    Line 1:
    {5 Spaces}{x dimension}{5 Spaces}{y dimension}{5 Spaces}{z dimension}
    Line 2:
    'DREAM3'              52.00  1.000  1.0       {Number of **Features**}
    Line 3: (Without the quotes)
    " 0.000 0.000 0.000          0        "
    {Each **Feature** Id on a single line}


**Example File (Partial)**

    11     11     11
    'DREAM3'              52.00  1.000  1.0       6
    0.000 0.000 0.000          0        
    830
    830
    0
    977
    977
    830
    830
    .....
    

## Parameters ##

| Name | Type | Description |
|------|------| ----------- |
| Input File | File Path | .ph file|
| Origin (XYZ) | int | The location in space coordinates of the (0,0,0) voxel |
| Resolution (XYZ) | int | X, Y, Z Resolution of the data |
| Volume Data Container | String | Output data container |
| Cell Attribute Matrix | String | Output attribute matrix |
| FeatureIds | String | Output feature ids |


## Required Geometry ##
Image / Rectilinear Grid


## Required Arrays ##
None

## Created Arrays ##

| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Ids (int) that specify to which **Feature** each **Cell** belongs. | (1) |Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


