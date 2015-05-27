Generate Geometry Connectivity {#generategeometryconnectivity}
======

## Group (Subgroup) ##
SurfaceMesh

## Description ##
This filter will generate the following connectivity data structures:

__Per Vertex Triangle List__ For each vertex in the mesh a list of triangles is generated that the vertex is a part of.

__Per Triangle Neighbor List__ For each triangle in the mesh a list of triangles that share a common edge is generated. 
Because the mesh is non-manifold each triangle can have more than 3 neighbors.

__List of Unique Edges__ This will generate a list of unsigned 64 bit integers where the high 32 bits are the first vertex index and the 
 low 32 bits are the second vertex. The lowest vertex id is always listed first and therefor the Edge list becomes a unique list.

## Parameters ##

| Name | Type |
|------|------|
| Generate Per Vertex Triangle List | Boolean (On or Off) |
| Generate Triangle Neighbors List | Boolean (On or Off) |
| Generate Unique Edge Ids | Boolean (On or Off) |

## Required DataContainers ##
SurfaceMesh - Valid Surface Mesh containing the shared vertex array and face list

## Required Arrays ##
None

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| MeshVertLinks | Internal Data Structure | For each vertex, the array of triangles that contain it |
| TriangleNeighbor List | Internal Data Structure | For each triangle, the array of triangles that share its edges |
| Unique Edges Array | Internal Data Structure | The three unique edge ids for the triangle |



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


