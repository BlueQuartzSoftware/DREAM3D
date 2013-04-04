Generate Unique Edges Filter {#}
======
<h1 class="pHeading1">Generate Unique Edges Filter</h1>
<p class="pCellBody">
<a href="../SurfaceMeshFilters/GenerateUniqueEdges.html#wp2">Description</a>
| <a href="../SurfaceMeshFilters/GenerateUniqueEdges.html#wp3">Options</a>
| <a href="../SurfaceMeshFilters/GenerateUniqueEdges.html#wp4">Required Arrays</a>
| <a href="../SurfaceMeshFilters/GenerateUniqueEdges.html#wp5">Created Arrays</a>
| <a href="../SurfaceMeshFilters/GenerateUniqueEdges.html#wp1">Authors</a> 

SurfaceMesh


 This will generate a list of unsigned 64 bit integers where the high 32 bits are the first vertex index and the 
 low 32 bits are the second vertex. The lowest vertex id is always listed first and therefor the Edge list becomes a unique list.

## Parameters ## 
## Required Arrays ##

| Type | Name | Comment |
|------|------|---------|
| Triangle List | Core Property of the Surface Mesh Data Container | The List of triangles in the Surface Mesh |
| Node List | Core Property of the Surface Mesh Data Container | The shared list of nodes that make up the mesh |

## Created Arrays ##

| Type | Name | Comment |
|------|------|---------|
| Cell | Unique Edge Ids |  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



