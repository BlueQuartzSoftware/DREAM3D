
Generate Node Triangle Connectivity Filter {#}
======
<h1 class="pHeading1">Generate Node Triangle Connectivity Filter</h1>
<p class="pCellBody">
<a href="../SurfaceMeshFilters/GenerateMeshConnectivity.html#wp2">Description</a>
| <a href="../SurfaceMeshFilters/GenerateMeshConnectivity.html#wp3">Options</a>
| <a href="../SurfaceMeshFilters/GenerateMeshConnectivity.html#wp4">Required Arrays</a>
| <a href="../SurfaceMeshFilters/GenerateMeshConnectivity.html#wp5">Created Arrays</a>
| <a href="../SurfaceMeshFilters/GenerateMeshConnectivity.html#wp1">Authors</a> 

SurfaceMesh


For each vertex in the mesh a list of triangles is generated that the vertex is a part of. The data structure is stored as a
 property of the SurfaceMeshDataContainer.


## Parameters ## 
## Required Arrays ##

| Type | Name | Comment |
|------|------|---------|
| Triangle List | Core Property of the Surface Mesh Data Container | The List of triangles in the Surface Mesh |
| Node List | Core Property of the Surface Mesh Data Container | The shared list of nodes that make up the mesh |

## Created Arrays ##

| Type | Name | Comment |
|------|------|---------|
| Cell | NodeTriangle List |  |
## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



