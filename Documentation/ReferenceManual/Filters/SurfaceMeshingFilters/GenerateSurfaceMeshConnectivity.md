
Generate Surface Mesh Connectivity {#}
======
<h1 class="pHeading1">Generate Surface Mesh Connectivity Filter</h1>
<p class="pCellBody">
<a href="../SurfaceMeshingFilters/GenerateSurfaceMeshConnectivity.html#wp2">Description</a>
| <a href="../SurfaceMeshingFilters/GenerateSurfaceMeshConnectivity.html#wp3">Options</a>
| <a href="../SurfaceMeshingFilters/GenerateSurfaceMeshConnectivity.html#wp4">Required Arrays</a>
| <a href="../SurfaceMeshingFilters/GenerateSurfaceMeshConnectivity.html#wp5">Created Arrays</a>
| <a href="../SurfaceMeshingFilters/GenerateSurfaceMeshConnectivity.html#wp1">Authors</a> 

Surface Meshing


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
None



## Created Arrays ##

| Type | Name | Comment |
|------|------|---------|
## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



