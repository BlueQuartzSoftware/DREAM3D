Scale Volume {#scalevolume}
==============================

## Group (Subgroup) ##
Generic Filters (Spatial)

## Description ##
This filter scales the Volume by the factors entered by the user. For example if a volume has a range
of 10 to 30 in the X direction and the user enters a **ScaleFactor** for X of 3, then the new range will be 10 to 90. Note
that the origin will NOT change with this filter.

## Parameters ##

| Name    | Type      |  Notes |
|---------|-----------|--------|
| Scaling | 3 x Float | Scale Factor     |
| Apply to Voxel Volume | Bool | Should the new scaling be applied to the Voxel Volume |
| Apply to SurfaceMesh Volume | Bool | Should the new scaling be applied to the Surface Mesh |

## Required DataContainers ##

Voxel: If being applied to the Voxel Volume
SurfaceMesh: If being applied to the SurfaceMesh

## Required Arrays ##

None

## Created Arrays ##

None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


