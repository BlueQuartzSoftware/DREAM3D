Decimate Surface Mesh {#decimatesurfacemesh}
=====

## Group (Subgroup) ##
UCSB (UCSB)


## Description ##
Decimates suface mesh with vertex contraction. Pairs to contract are selected using Quadric Error Metrics. Vertecies will be contracted until the specified percentage of triangles remain. Node and edge arrays will be erased. Face arrays will be kept with remaining traingles retaining their original value.


## Parameters ##
| Name             | Type |
|------------------|------|
| Triangles to Keep | Float|
  

## Required Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Vertex Array | SurfaceMeshNodes | The shared list of nodes that make up the mesh |
| Triangle Array | SurfaceMeshTriangles | The List of triangles in the Surface Mesh |


## Created Arrays ##
None

## Authors ##

**Copyright** 2014 Will Lenthe (UCSB)

**Contact Info** willlenthe@gmail.com

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.

## References ##
[1] Garland, M., Heckbert, P., 1997. Surface simplification using quadric error metrics. Proceedings of the 24th annual conference on Computer graphics and interactive techniques. 209-216.

See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)
