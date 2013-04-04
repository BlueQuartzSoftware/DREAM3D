
Quick Surface Mesh {#}
======
<h1 class="pHeading1">Quick Surface Mesh Filter</h1>
<p class="pCellBody">
<a href="../SurfaceMeshingFilters/QuickSurfaceMesh.html#wp2">Description</a>
| <a href="../SurfaceMeshingFilters/QuickSurfaceMesh.html#wp3">Options</a>
| <a href="../SurfaceMeshingFilters/QuickSurfaceMesh.html#wp4">Required Arrays</a>
| <a href="../SurfaceMeshingFilters/QuickSurfaceMesh.html#wp5">Created Arrays</a>
| <a href="../SurfaceMeshingFilters/QuickSurfaceMesh.html#wp1">Authors</a> 

SurfaceMeshing


 This surface meshing algorithm proceeds by creating a pair of triangles for each face
 of the voxel where the neighboring voxel have a different grain id value. No attempt is
 made to mimic the operations of a MultiMaterial Marching Cubes algorithm. The meshing operation
 is extremely quick but can result in a surface mesh that is very "stair stepped". The user
 is encouraged to use a smoothing operation on the surface mesh.
 

## Parameters ## 
## Required Arrays ##

| Type | Name | Comment |
|------|------|---------|
| Cell | GrainIds |  |

## Created Arrays ##
None



## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL)
**Contact Info** dream3d@bluequartz.net
**Version** 1.0.0
**License**  See the License.txt file that came with DREAM3D.

