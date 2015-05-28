Find Feature Centroids {#findfeaturecentroids}
=============

## Group (Subgroup) ##
Generic Filters (Misc)

## Description ##
This Filter calculates the _centroid_ of each **Feature** by determining the average x, y, and z position of all the **Cells** belonging to the **Feature**.
Note that **Features** that intersect the outer surfaces of the sample will still have _centroids_ calculated, but they will be _centroids_ of the truncated part of the **Feature** that lies inside the sample.

## Parameters ##
None

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs. |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | Centroids | Float | (3) | X, Y, Z coordinates of **Feature** center of mass |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


