Write Vtk PolyData File (From SurfaceMesh) {#surfacemeshtovtk}
======

## Group (Subgroup) ##
IO Filters

## Description ##
This filter will create a "legacy" vtk file (.vtk) that contains the POINTS and POLYGONS, where each POLYGON is a triangle.
In addition the NODE_TYPE will be written as POINT_DATA and the GRAIN_ID will be written as CELL_DATA. There are 2 options
for this filter.

__Write Binary Vtk File__ If this option is selected then the data portions of the file will be written in Big Endian
binary format as stipulated by the VTK file format.

- __Write Conformal Mesh__ If this is ___true___ then only a single triangle will be written which will be shared
between 2 **Feature** (region) ids. If this is ___false___ then 2 triangles with opposite winding will be written and each
will have a **Feature** ID assigned to it.



## Parameters ##

| Name | Type |
|------|------|
| Output Vtk File | Output File |
| Write Binary Vtk File | Boolean (On or Off) |
| Write Conformal Mesh | Boolean (On or Off) |

## Required DataContainers ##
SurfaceMesh

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| NodeArray | | The shared node (vertex) list |   | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |
| FaceArray | | The list of all triangles (faces) in the mesh |   | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |

## Created Arrays ##
None

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


