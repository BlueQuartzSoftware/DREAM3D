Generate Surface Mesh Connectivity {#generatesurfacemeshconnectivity}
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


## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

