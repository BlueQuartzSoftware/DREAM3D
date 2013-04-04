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
| Genereate Per Vertex Triangle List | Boolean (On or Off) |
| Genereate Triangle Neighbors List | Boolean (On or Off) |
| Genereate Unique Edge Ids | Boolean (On or Off) |

## Required Arrays ##

| Type | Name | Comment |
|------|------|---------|
| Triangle List | Core Property of the Surface Mesh Data Container | The List of triangles in the Surface Mesh |
| Node List | Core Property of the Surface Mesh Data Container | The shared list of nodes that make up the mesh |


## Created Arrays ##

| Type | Name | Comment |
|------|------|---------|
| MeshVertLinks | Internal Data Structure |  |
| TriangleNeighbor List | Internal Data Structure |  |
| Unique Edges Array | Internal Data Structure |  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



