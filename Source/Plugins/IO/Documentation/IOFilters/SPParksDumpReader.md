Read SPParks Dump File {#spparksdumpreader}
=============

## Group (Subgroup) ##
IO (Input)

## Description ##
This **Filter** reads from a data file in a format used by [SPPARKS Kinetic Monte Carlo Simulator](http://spparks.sandia.gov/). The information in the file defines an **Image Geometry** with a set of **Feature** Ids. More information can be found at the [SPParks Dump file web site.](http://spparks.sandia.gov/doc/dump.html)

** This filter will read from a _DUMP_ file from a SPParks simulation.**
## Example Input ##

    [LINE 1] ITEM: TIMESTEP
    [LINE 2] 210    21000.6
    [LINE 3] ITEM: NUMBER OF ATOMS
    [LINE 4] 106480
    [LINE 5] ITEM: BOX BOUNDS
    [LINE 6] 0.5 44.5
    [LINE 7] 0.5 44.5
    [LINE 8] 0.5 55.5
    [LINE 9] ITEM: ATOMS id type x y z
    1 944 54 23 1
    2 944 54 24 2
    3 944 54 25 3
    4 944 54 26 4
    5 944 54 27 5
    6 944 54 28 6
    7 944 54 29 7
    …..


## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Input File | File Path | The input .dump file path |
| Origin | float (3x) | The location in space of the (0, 0, 0) coordinate |
| Resolution | float (3x) | The resolution values (dx, dy, dz) |
| One Based Arrays | bool | Whether the origin starts at (1, 1, 1) |

## Required Geometry ##
Not Applicable

## Required Objects ##
None

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Data Container** | ImageDataContainer | N/A | N/A | Created **Data Container** name with an **Image Geometry** |
| **Attribute Matrix** | CellData | Cell | N/A | Created **Cell Attribute Matrix** name |
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)

