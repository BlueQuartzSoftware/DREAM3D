Adjust Origin  {#adjustvolumeorigin}
=============

## Group (Subgroup) ##
Core Filters (Spatial)

## Description ##
This filter changes the origin of the volume. For example if the current origin is
at (0,0,0) and the user would like the origin to be (10, 4, 8) then the user should enter the following input values:

+ X Origin: 10
+ Y Origin: 4
+ Z Origin: 8

## Parameters ##

| Name    | Type      |  Description |
|---------|-----------|--------|
|  Origin | 3 x Float | Applied to x, y, and z for Image Geometry and node 1, node 2 and node 3 for Surface Mesh Geometry       |
| Apply to Voxel Volume | Boolean | Should the new origin be applied to the Voxel Volume |
| Apply to SurfaceMesh Volume | Boolean | Should the new origin be applied to the Surface Mesh |

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


