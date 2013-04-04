Generate Triangle IPF Coloring {#generatetriangleipfcoloring}
======================

## Group (Subgroup) ##
SurfaceMesh

## Description ##

## Parameters ##

## Required Arrays ##

| Type | Name | Comment |
|------|------|---------|
| Triangle List | Core Property of the Surface Mesh Data Container | The List of triangles in the Surface Mesh |
| Node List | Core Property of the Surface Mesh Data Container | The shared list of nodes that make up the mesh |
| Triangle Attribute Array | SurfaceMeshTriangleLabels | Grain Id Associated with the triangle |
| Triangle Attribute Array | SurfaceMeshTriangleNormals | Normal of each Triangle |

## Created Arrays ##

| Type | Name | Comment |
|------|------|---------|
| Triangle Attribute Array | SurfaceMeshTriangleIPFColors | N X 6 Array of Unsigned Chars that are the RGB colors for each Grain Id Associated with the triangle. |


## Authors ##

**Copyright** 2013 Michael A. Groeber (AFRL), 2013 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



