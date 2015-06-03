Scale Volume {#scalevolume}
==============================

## Group (Subgroup) ##
Core Filters (Spatial)

## Description ##
This filter scales the Volume by the factors entered by the user. For example if a volume has a range
of 10 to 30 in the X direction and the user enters a **ScaleFactor** for X of 3, then the new range will be 30 to 90. Note
that the origin will NOT change with this filter. 

## Parameters ##

| Name    | Type      |  Notes |
|---------|-----------|--------|
| Scaling Factor | 3 x Float | Applied to Res x, Res y, and Res z for Image Geometry and node 1, node 2 and node 3 for Vertex Geometry |
| Apply to Voxel Volume | Bool | Should the new scaling be applied to the Voxel Volume |
| Data Container | String | Shown if Apply to Voxel Volume is checked |
| Apply to SurfaceMesh Volume | Bool | Should the new scaling be applied to the Surface Mesh |
| Surface Data Container | String | Shown if Apply to Surface Mesh is checked |


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


