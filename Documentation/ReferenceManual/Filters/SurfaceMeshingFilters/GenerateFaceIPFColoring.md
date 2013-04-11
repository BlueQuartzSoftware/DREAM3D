Generate Face IPF Coloring {#generatefaceipfcoloring}
======================

## Group (Subgroup) ##
SurfaceMesh

## Description ##

## Parameters ##

## Required DataContainers ##
SurfaceMesh - Valid Surface Mesh containing the shared vertex array and face list


## Required Arrays ##
| Type | Name | Comment |
|------|------|---------|
| Vertex Array | SurfaceMeshNodes | The shared list of nodes that make up the mesh |
| Triangle Array | SurfaceMeshTriangles | The List of triangles in the Surface Mesh |
| Face   | SurfaceMeshFaceLabels | N x 2 Col of signed integer |

## Created Arrays ##
| Type | Name | Comment |
|------|------|---------|
| Triangle Attribute Array | SurfaceMeshTriangleIPFColors | N X 6 Array of Unsigned Chars that are the RGB colors for each **Field** Id Associated with the triangle. |



## Authors ##

**Copyright:** 2013 Michael A. Groeber (AFRL), 2013 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



