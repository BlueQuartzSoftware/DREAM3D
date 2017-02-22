Write SPParks File (Feature Ids) {#spparkswriter}
============

## Group (Subgroup) ##
IO (Output)

## Description ##
This **Filter** writes to a data file in a format used by [SPPARKS Kinetic Monte Carlo Simulator](http://spparks.sandia.gov/).

+ The "Values" section is a pair of numbers where the first number is the site ID and the second
 number is the actual value of the **Feature** Id at that site.
+ LINE 4 evidently must be what is shown in the example file.
+ LINE 3 is the total number of **Cells**.
+ LINES 5, 6 and 7 are the dimensions of the volume. 

More information can be found at the [SPParks web site.](http://spparks.sandia.gov/doc/read_sites.html "SPParks Web Site")

## Example Output ##

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
    …..

## Parameters ##

| Name | Type | Description |
|------|------|------|
| Output File | File Path | Output .spparks file path |

## Required Geometry ##
Image

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


