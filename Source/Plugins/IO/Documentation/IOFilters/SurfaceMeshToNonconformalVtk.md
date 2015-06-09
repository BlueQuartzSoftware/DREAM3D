Write NonConformal Vtk PolyData from SurfaceMesh {#surfacemeshtononconformalvtk}
======

## Group (Subgroup) ##
I/O Filters (Output)

## Description ##


## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Output Vtk File | Output Path | Creates a .vtk file |
| Write Binary Vtk File | Boolean | binary |

## Required Geometry ##
Triangle

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| SurfaceMeshFaceLabels | None | The list of all triangles (faces) in the mesh | (2)   | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |
| SurfaceMeshNodeType | None |The shared node (vertex) list | (2) | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |

## Created Arrays ##
None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


