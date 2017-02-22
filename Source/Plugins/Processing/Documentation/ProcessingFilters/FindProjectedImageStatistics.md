Find Projected Image Statistics {#findprojectedimagestatistics}
=============

## Group (Subgroup) ##
Processing (Image)

## Description ##
This **Filter** calculates statistics (minimum, maximum, average, standard deviation and variance) for each *stack* of **Cells** perpendicular to a user selected plane of interest (XY, XZ, YZ).  Each **Cell** in the *stack* is then assigned the statistic(s) for the whole stack. For example, if there was a 100x200x300 **Cell** volume and the plane of interest was selected as *XY*, then a 100x200 temporary *image* would be created.  Each **Cell** in the temporary *image* would represent a 300 **Cell** *stack* in the Z direction.  The statistics listed previously would be calculated for each 300 **Cell** *stack* and stored at the corresponding **Cell** in the 100x200 temporary *image*.  Finally, the values for each of the statistics stored in the temp *image* are assigned to all 300 **Cells** in the *stack* that they describe, which is equivalent to "extruding" the 100x200 temporary *image* 300 *Cells** in the Z direction. 

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Plane of Interest | Enumeration | Which plane (XY, XZ, YZ) to project perpendicular to |

## Required Geometry ##
Image

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | None | Any, except bool | (1) | Any scalar array for which the user would like to calculate projected statistics |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | ProjectedImageMin | float | (1) | The minimum value of the scalar array along the stack of voxels perpendicular to the plane of interest |
| **Cell Attribute Array** | ProjectedImageMax | float | (1) | The maximum value of the scalar array along the stack of voxels perpendicular to the plane of interest |
| **Cell Attribute Array** | ProjectedImageAvg | float | (1) | The average value of the scalar array along the stack of voxels perpendicular to the plane of interest |
| **Cell Attribute Array** | ProjectedImageStd | float | (1) | The standard deviation of the scalar array along the stack of voxels perpendicular to the plane of interest |
| **Cell Attribute Array** | ProjectedImageVar | float | (1) | The variance of the scalar array along the stack of voxels perpendicular to the plane of interest |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


