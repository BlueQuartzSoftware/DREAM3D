SurfaceMeshToStl Filter {#surfacemeshtostl}
======

## Group (Subgroup) ##
I/O Filters (Output)

## Description ##
This filter will write a binary STL File for each unique **Feature** or Region ID in the surface Mesh. The STL files
will be named with the [Grain_id].stl. The user can designate an optional prefix for the files.


## Parameters ##

| Name | Type | Description |
|------|------|------|
| Output STL Directory | File Path | The path to the output directory where all of the individual STL files will be saved |
| Stl File Prefix | String | Optional Prefix to use when creating the stl file name. |
| Group Files By Phase | Boolean | Include SurfaceMeshFacePhases |

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


