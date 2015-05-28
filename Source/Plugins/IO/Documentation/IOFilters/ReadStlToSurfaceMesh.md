ReadStlToSurfaceMesh Filter {#readstltosurfacemesh}
======

## Group (Subgroup) ##
IO Filters

## Description ##
This filter will read a binary STL File.


## Parameters ##

| Name | Type | Comment |
|------|------|------|
| Output STL Directory | Path to Directory | The path to the output directory where all of the individual STL files will be saved |
| Stl File Prefix | String | Optional Prefix to use when creating the stl file name. |

## Required DataContainers ##
SurfaceMesh

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| NodeArray | | The shared node (vertex) list |   | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |
| FaceArray | | The list of all triangles (faces) in the mesh |   | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |

## Created Arrays ##
None



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


