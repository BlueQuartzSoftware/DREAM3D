
Multi-Material Marching Cubes (Volume at a Time) {#}
======
<h1 class="pHeading1">Multi-Material Marching Cubes (Volume at a Time)</h1>
<p class="pCellBody">
<a href="../Surface_MeshingFilters/MMCSurfaceMeshingFilter.html#wp2">Description</a>
| <a href="../Surface_MeshingFilters/MMCSurfaceMeshingFilter.html#wp3">Options</a>
| <a href="../Surface_MeshingFilters/MMCSurfaceMeshingFilter.html#wp4">Required Arrays</a>
| <a href="../Surface_MeshingFilters/MMCSurfaceMeshingFilter.html#wp5">Created Arrays</a>
| <a href="../Surface_MeshingFilters/MMCSurfaceMeshingFilter.html#wp1">Authors</a> 

Surface Meshing

This filter creates a surface mesh using a MultiMaterial Marching Cubes (M3C) algorithm originally
 developed at Carnegie-Mellon University by Dr. Sukbin Lee in the Marterials Engineering
 department based on the Wu/Sullivan algorithm. Heavy modifications were performed by
 M. Groeber and M. Jackson for the DREAM3D project.
This version of the code meshes by looking at the entire volume of voxels at one time. The ramifications
 of this means that this version may be more memory intensive than other versions of the M3C algorithm, specifically
 the "Slice at a Time" variant. The option to _Add Surface Layer_ is needed
 in most cases as the algorithm needs a ghost layer of voxels around the entire volume
 to properly work. Sometimes when importing a volume through some of the IO filters (Dx and Ph Readers)
 the volume that is being imported already has this surface layer added. If this
 is true then the parameter is not needed.
The values for the __Node Type__ array can take one of the following values.
<pre>
namespace SurfaceMesh {<br/>
  namespace NodeType {<br/>
    const int8_t Unused = -1;<br/>
    const int8_t Default = 2;<br/>
    const int8_t TriplePoint = 3;<br/>
    const int8_t QuadPoint = 4;<br/>
    const int8_t SurfaceDefault = 12;<br/>
    const int8_t SurfaceTriplePoint = 13;<br/>
    const int8_t SurfaceQuadPoint = 14;<br/>
  }<br/>
}<br/>
</pre>

## Parameters ## 

## Required Arrays ##

| Type | Name | Comment |
|------|------|---------|
| Cell | GrainIds |  |

## Created Arrays ##

| Type | Name | Comment |
|------|------|---------|
|  | Nodes | SurfaceMeshDataContainer Property |
|  | Triangles | SurfaceMeshDataContainer Property |
| Cell/Triangle | Node Type | The type of nodes |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



