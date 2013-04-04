Verify Triangle Winding {#}
======
<h1 class="pHeading1">Verify Triangle Winding Filter</h1>
<p class="pCellBody">
<a href="../SurfaceMeshFilters/VerifyTriangleWinding.html#wp2">Description</a>
| <a href="../SurfaceMeshFilters/VerifyTriangleWinding.html#wp3">Options</a>
| <a href="../SurfaceMeshFilters/VerifyTriangleWinding.html#wp4">Required Arrays</a>
| <a href="../SurfaceMeshFilters/VerifyTriangleWinding.html#wp5">Created Arrays</a>
| <a href="../SurfaceMeshFilters/VerifyTriangleWinding.html#wp1">Authors</a> 

SurfaceMesh


 This filter analyzes the mesh for consistent triangle winding and fixes any inconsistencies that are found. This algorithm
 can be memory intensive for larger meshes. The algorithm is an advancing wave front type of algorithm. The algorithm is know to have
 problems if there are disconnected grains with the same ID in the surface mesh.


## Parameters ##

## Required Arrays ##

| Type | Name | Comment |
|------|------|---------|
| Triangle List | Core Property of the Surface Mesh Data Container | The List of triangles in the Surface Mesh |
| Node List | Core Property of the Surface Mesh Data Container | The shared list of nodes that make up the mesh |

## Created Arrays ##
None



## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



