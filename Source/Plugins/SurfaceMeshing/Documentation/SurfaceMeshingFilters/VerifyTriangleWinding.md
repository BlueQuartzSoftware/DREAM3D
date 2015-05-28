Verify Triangle Winding {#verifytrianglewinding}
======

## Group (Subgroup) ##
SurfaceMesh

## Description ##
This filter analyzes the mesh for consistent triangle winding and fixes any inconsistencies that are found. This algorithm can be memory intensive for larger meshes. The algorithm is an advancing wave front type of algorithm. The algorithm is know to have problems if there are disconnected **Features** with the same ID in the surface mesh.


## Parameters ##
None

## Required DataContainers ##
SurfaceMesh - Valid Surface Mesh containing the shared vertex array and face list

## Required Arrays ##
None

## Created Arrays ##
None




## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


