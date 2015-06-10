SPParks Text Reader {#spparkstextreader}
=====

## Group (Subgroup) ##
I/O Filters (Input)


## Description ##
This Filter reads from a data file in a format used by [SPPARKS Kinetic Monte Carlo Simulator](http://spparks.sandia.gov/).

+ The "Values" section is a pair of numbers where the first number is the site ID and the second
 number is the actual value of the **Feature** Id at that site.
+ LINE 4 evidently must be what is shown in the example file.
+ LINE 3 is the total number of **Cells**.
+ LINES 5,6 and 7 are the Dimensions of the volume. 


 More information can be found at the [SPParks web site.](http://spparks.sandia.gov/doc/read_sites.html "SPParks Web Site")

 
**Start Example Partial File**
~~~~~~~~~~~~~~~~~~~~~~~~~
    [LINE 1] -
    [LINE 2] 3 dimension
    [LINE 3] 8000000 sites
    [LINE 4] 26 max neighbors
    [LINE 5] 0 200 xlo xhi
    [LINE 6] 0 200 ylo yhi
    [LINE 7] 0 200 zlo zhi
    [LINE 8]
    [LINE 9] Values
    [LINE 10]
    1 944
    2 944
    3 944
    4 944
    5 509
    6 509
    7 509
    .....
~~~~~~~~~~~~~~~~~~~~~~~~~

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Input File | File Path | .spparks file|
| Origin (XYZ) | int | The location in space coordinates of the (0,0,0) voxel |
| Resolution (XYZ) | int | X, Y, Z Resolution of the data |
| Volume Data Container | String | Output data container, DataContainer |
| Cell Attribute Matrix | String | Output attribute matrix, CellData |
| FeatureIds | String | Output feature ids, Site Type |

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
None


## Created Arrays ##

| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds (Site Type) | Ids (int32) that specify to which **Feature** each **Cell** belongs. | (1) | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users

