Generate Triangle Areas {#triangleareafilter}
======

## Group (Subgroup) ##
SurfaceMesh

## Description ##
This filter computes the area of each triangle (given by points A,B,C) in the surface mesh by calculating the following: 1/2*|AB||AC|sin(O), where O is the angle between |AB| and |AC|.


## Parameters ##
None

## Required DataContainers ##
SurfaceMesh - Valid Surface Mesh containing the shared vertex array and face list

## Required Arrays ##
None

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Face | SurfaceMeshFaceAreas | |



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


