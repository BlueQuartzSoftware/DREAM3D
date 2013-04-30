Write NonConformal Vtk PolyData from SurfaceMesh {#surfacemeshtononconformalvtk}
======

## Group (Subgroup) ##
IO Filters (Output)

## Description ##


## Parameters ##

| Name | Type |
|------|------|
| Output Vtk File | Output File |
| Write Binary Vtk File | Boolean (On or Off) |

## Required DataContainers ##
SurfaceMesh

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data
|------|--------------|-------------|---------|-----|
| NodeArray Attribute | SurfaceMeshNodeType | The node types values |   | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |
| FaceArray Attribute | SurfaceMeshTriangleLabels | The **Field** Id Labels for each Face(Triangle) |   | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |
| NodeArray | | The shared node (vertex) list |   | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |
| FaceArray | | The list of all triangles (faces) in the mesh |   | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |


## Created Arrays ##
None



## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



