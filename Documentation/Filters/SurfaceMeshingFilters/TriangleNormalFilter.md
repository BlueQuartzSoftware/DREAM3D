
Generate Triangle Normals Filter {#}
======
<h1 class="pHeading1">Generate Triangle Normals Filter Filter</h1>
<p class="pCellBody">
<a href="../SurfaceMeshFilters/TriangleNormalFilter.html#wp2">Description</a>
| <a href="../SurfaceMeshFilters/TriangleNormalFilter.html#wp3">Options</a>
| <a href="../SurfaceMeshFilters/TriangleNormalFilter.html#wp4">Required Arrays</a>
| <a href="../SurfaceMeshFilters/TriangleNormalFilter.html#wp5">Created Arrays</a>
| <a href="../SurfaceMeshFilters/TriangleNormalFilter.html#wp1">Authors</a> 

SurfaceMesh


This filter calculates a normal vector of length 1 (Normalized) for each triangle.


## Parameters ##
## Required Arrays ##

| Type | Name | Comment |
|------|------|---------|
| Triangle List | Core Property of the Surface Mesh Data Container | The List of triangles in the Surface Mesh |
| Node List | Core Property of the Surface Mesh Data Container | The shared list of nodes that make up the mesh |

## Created Arrays ##

| Type | Name | Comment |
|------|------|---------|
| double | SurfaceMeshTriangleNormals | 3 Component vector for each Triangle in the Mesh |
## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



