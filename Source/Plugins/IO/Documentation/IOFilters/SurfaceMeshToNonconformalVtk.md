Write NonConformal Vtk PolyData from SurfaceMesh {#surfacemeshtononconformalvtk}
======

## Group (Subgroup) ##
I/O Filters (Output)

## Description ##
This filter will create a "legacy" vtk file (.vtk) that contains the POINTS and POLYGONS, where each POLYGON is a triangle.
In addition the NODE_TYPE will be written as POINT_DATA and the GRAIN_ID will be written as CELL_DATA. There are 2 options for this filter.

__Write Binary Vtk File__ If this option is selected then the data portions of the file will be written in Big Endian
binary format as stipulated by the VTK file format.


## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Output Vtk File | Output Path | Creates a .vtk file |
| Write Binary Vtk File | Boolean | binary |

## Required Geometry ##
Triangle

## Required Objects ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Triangle | SurfaceMeshFaceLabels | int  | (2)   | The list of all triangles (faces) in the mesh. Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |
| Vertex | SurfaceMeshNodeType | int | (1) | The shared node (vertex) list. Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |


## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


