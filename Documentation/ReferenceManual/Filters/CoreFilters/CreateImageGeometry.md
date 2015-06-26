Create Image Geometry  {#createimagegeometry}
=============

## Group (Subgroup) ##

Core Filters (Generation)


## Description ##

This filter creates an Image Geometry specifically for the representation of a 3D Rectilinear Grid of Voxels (3D) or Pixels
(2D). Each axis can have its starting point (origin), resolution, and length defined for the **Geometry**. 

### Example Usage ###

If you are reading in raw binary data that represents data on a rectilinear grid the user will need to run this
filter first to create a description of the **Geometry**.

## Parameters ##

| Name             | Type | Description |
|------------------|------|-------------|
| Dimensions | Integer (3) | The number of cells in each of the X, Y, Z directions |
| Origin | Float (3) | The origin of each of the Axis. X, Y, Z Order |
| Resolution | Float (3) | The length scale of each voxel/pixel. |

## Required Geometry ##

Not Applicable

## Required Arrays ##

| Name             | Type | Description |
|------------------|------|-------------|
| Data Container Name | String | Name for Data Container to attach the Geometry. |

## Created Arrays ##

Not Applicable

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users

