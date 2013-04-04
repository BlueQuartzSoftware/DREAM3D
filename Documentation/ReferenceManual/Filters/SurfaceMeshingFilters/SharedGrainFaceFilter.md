Shared Grain Face Filter {#}
======
<h1 class="pHeading1">Shared Grain Face Filter Filter</h1>
<p class="pCellBody">
<a href="../SurfaceMeshFilters/SharedGrainFaceFilter.html#wp2">Description</a>
| <a href="../SurfaceMeshFilters/SharedGrainFaceFilter.html#wp3">Options</a>
| <a href="../SurfaceMeshFilters/SharedGrainFaceFilter.html#wp4">Required Arrays</a>
| <a href="../SurfaceMeshFilters/SharedGrainFaceFilter.html#wp5">Created Arrays</a>
| <a href="../SurfaceMeshFilters/SharedGrainFaceFilter.html#wp1">Authors</a> 

SurfaceMesh


 This filter generates all the unique grain id to grain id values. For example if Grain Id = 4 had 3 neighbors with values
 of 6, 8 and 9 then at least 3 unique values would be generated for the pairs (4,6), (4, 8) and (4,9). This value is stored
 in the array at the same index as the triangle is stored in the triangle array. 


## Parameters ##

## Required Arrays ##

| Type | Name | Comment |
|------|------|---------|
| Triangle List | Core Property of the Surface Mesh Data Container | The List of triangles in the Surface Mesh |
| Node List | Core Property of the Surface Mesh Data Container | The shared list of nodes that make up the mesh |

## Created Arrays ##

| Type | Name | Comment |
|------|------|---------|
| int | SurfaceMeshGrainFaceId | The length of the array is the number of triangles. |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



