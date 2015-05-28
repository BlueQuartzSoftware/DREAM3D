Goldfeather Reader {#goldfeatherreader}
======
[TOC]

## Group (Subgroup) ##
IO Filters (Input)

## Description ##

This class reads a surface mesh from a .jg file as supplied by Dr. Goldfeather in his paper on curvature estimation. In
addition to the mesh is the data for the Principal Curvatures and Principal Curvature Directions.


## Parameters ##

| Name | Type |
|------|------|
| Input File | File Path |

## Required DataContainers ##
Voxel

## Required Arrays ##
None



## Created Arrays ##

| Type       | Name | Comment |
|------------|------|---------|
| Vert Array | Nodes     | SurfaceMeshDataContainer Property of the Shared Vertex List |
| Face Array | Triangles | SurfaceMeshDataContainer Property of the list of Triangles |
| Double | Node Normals | The Normal at each Vertex |
| Double | Principal Direction 1 | N x 3 Col Double Array |
| Double | Principal Direction 2 | N x 3 Col Double Array |
| Double | Principal Curvature 2 | N x 1 Col Double Array |
| Double | Principal Curvature 2 | N x 1 Col Double Array |
| Int32 | SurfaceMeshTriangleLabels | Triangle Feature Labels |
| Double | SurfaceMeshTriangleNormals | Triangle Normals |




## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


