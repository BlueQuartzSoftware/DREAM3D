Generate Triangle Centroids {#trianglecentroidfilter}
======

## Group (Subgroup) ##
SurfaceMesh

## Description ##
This filter computes the centroid of each triangle in the surface mesh by calculating the average position of all 3 nodes that make up the triangle.


## Parameters ##
None

## Required DataContainers ##
SurfaceMesh - Valid Surface Mesh containing the shared vertex array and face list

## Required Arrays ##
None

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Face | SurfaceMeshFaceCentroids | 3x1 double Vector for each face in the Mesh |



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


