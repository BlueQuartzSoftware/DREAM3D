Write Vtk PolyData File (From SurfaceMesh) {#surfacemeshtovtk}
======

## Group (Subgroup) ##
I/O Filters (Output)

## Description ##
This filter will create a "legacy" vtk file (.vtk) that contains the POINTS and POLYGONS, where each POLYGON is a triangle.
In addition the NODE_TYPE will be written as POINT_DATA and the GRAIN_ID will be written as CELL_DATA. There are 2 options
for this filter.

__Write Binary Vtk File__ If this option is selected then the data portions of the file will be written in Big Endian
binary format as stipulated by the VTK file format.

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Output Vtk File | Output Path | Creates a .vtk file |
| Write Binary Vtk File | Boolean | Big Endian binary format |

## Required Geometry ##
Triangle

## Required Arrays ##

| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Triangle | SurfaceMeshFaceLabels | int32  | (2)   | The list of all triangles (faces) in the mesh. Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |
| Vertex | SurfaceMeshNodeType | int8 | (2) | The shared node (vertex) list. Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |

## Created Arrays ##
None


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


