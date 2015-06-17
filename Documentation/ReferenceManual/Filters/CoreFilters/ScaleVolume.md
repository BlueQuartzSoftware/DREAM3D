Scale Volume {#scalevolume}
==============================

## Group (Subgroup) ##
Core Filters (Spatial)

## Description ##
This filter scales the Volume by the factors entered by the user. For example if a volume has a range
of 10 to 30 in the X direction and the user enters a **ScaleFactor** for X of 3, then the new range will be 30 to 90. Note
that the origin will NOT change with this filter. 

## Parameters ##

| Name    | Type      |  Description |
|---------|-----------|--------|
| Scaling Factor | 3 x Float | Applied to Res x, Res y, and Res z for Image Geometry and node 1, node 2 and node 3 for Surface Mesh Geometry |
| Apply to Voxel Volume | Boolean | Should the new scaling be applied to the Voxel Volume |
| Apply to SurfaceMesh Volume | Boolean | Should the new scaling be applied to the Surface Mesh |

## Required Geometry ##
Image / Rectilinear Grid and/or 2D Surface Mesh

## Required Arrays ##
None

## Created Arrays ##
None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


