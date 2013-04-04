Write NonConformal Vtk PolyData from SurfaceMesh {#surfacemeshtononconformalvtk}
======

## Group (Subgroup) ##
IO Filters

## Description ##


## Parameters ## 

| Name | Type |
|------|------|
| Output Vtk File | Output File |
| Write Binary Vtk File | Boolean (On or Off) |

## Required Arrays ##

| Type | Name | Comment |
|------|------|---------|
| NodeArray Attribute | SurfaceMeshNodeType | The node types values |
| FaceArray Attribute | SurfaceMeshTriangleLabels | The Grain Id Labels for each Face(Triangle) |
| NodeArray | | The shared node (vertex) list |
| FaceArray | | The list of all triangles (faces) in the mesh |

## Created Arrays ##
None



## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



