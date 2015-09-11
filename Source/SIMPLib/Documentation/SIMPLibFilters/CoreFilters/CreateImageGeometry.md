Create Geometry (Image)  {#createimagegeometry}
=============

## Group (Subgroup) ##

Core (Generation)


## Description ##

This **Filter** creates an **Image Geometry** specifically for the representation of a 3D rectilinear grid of voxels (3D) or pixels
(2D). Each axis can have its starting point (origin), resolution, and length defined for the **Geometry**. The **Data Container** in which to place the **Image Geometry** must be specified.

### Example Usage ###

If you are reading in raw binary data that represents data on a rectilinear grid, the user will need to run this
filter first to create a description of the **Geometry**.

## Parameters ##
| Name             | Type | Description |
|------------------|------|-------------|
| Dimensions | size_t (3x) | The number of cells in each of the X, Y, Z directions |
| Origin | float (3x) | The origin of each of the axes in X, Y, Z order |
| Resolution | float (3x) | The length scale of each voxel/pixel |

## Required Geometry ##

Not Applicable

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Data Container** | None | N/A | N/A | **Data Container** in which to place the created **Image Geometry** |


## Created Objects ##
Not Applicable

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)

