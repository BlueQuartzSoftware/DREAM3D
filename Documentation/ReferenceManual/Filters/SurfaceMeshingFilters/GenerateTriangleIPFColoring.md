Generate Triangle IPF Coloring {#generatetriangleipfcoloring}
======================

## Group (Subgroup) ##
SurfaceMesh

## Description ##

## Parameters ##

## Required Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Vertex Array | SurfaceMeshNodes | The shared list of nodes that make up the mesh |
| Triangle Array | SurfaceMeshTriangles | The List of triangles in the Surface Mesh |
| Triangle Attribute Array | SurfaceMeshTriangleLabels | Pair of Grain Id Associated with the triangle |
| Triangle Attribute Array | SurfaceMeshTriangleNormals | Normal of each Triangle |

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Triangle Attribute Array | SurfaceMeshTriangleIPFColors | N X 6 Array of Unsigned Chars that are the RGB colors for each Grain Id Associated with the triangle. |


## Authors ##

**Copyright** 2013 Michael A. Groeber (AFRL), 2013 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



