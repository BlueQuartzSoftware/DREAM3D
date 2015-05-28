Find Surface Area To Volume {#findsurfaceareatovolume}
=============

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This Filter calculates the ratio of surface area to volume for each **Feature** for an *Image Geometry* . First all the boundary **Cells** are found for each **Feature**. Next the surface area for each face that is in contact with a different **Feature** is totaled. This is divided by the volume of each **Feature**, calculated by taking the number of **Cells** of each **Feature** and multiplying by the volume of a **Cell**. 

*Note:* The surface area will be the surface area of the **Cells** in contact with the neighboring **Feature** and will be influenced by the aliasing of the structure.  As a result, the surface area-to-volume will likely be over-estimated with respect to the *real* **Feature**.

## Parameters ##
None

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs. |
| Feature | NumCells |  Int | (1) | Number of **Cells** that are owned by the **Feature**. This value does not place any distinction between **Cells** that may be of a different size. |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | SurfaceAReaVolumeRatio | Float | (1) | Ratio of surface area to volume for each **Feature**. The units are in inverse length units |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


