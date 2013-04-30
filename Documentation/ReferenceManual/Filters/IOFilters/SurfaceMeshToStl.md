SurfaceMeshToStl Filter {#surfacemeshtostl}
======

## Group (Subgroup) ##
IO Filters

## Description ##
This filter will write a binary STL File for each unique **Field** or Region ID in the surface Mesh. The STL files
will be named with the [Grain_id].stl. The user can designate an optional prefix for the files.


## Parameters ##

| Name | Type | Comment |
|------|------|------|
| Output STL Directory | Path to Directory | The path to the output directory where all of the individual STL files will be saved |
| Stl File Prefix | String | Optional Prefix to use when creating the stl file name. |

## Required DataContainers ##
SurfaceMesh

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data
|------|--------------|-------------|---------|-----|
| NodeArray | | The shared node (vertex) list |   | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |
| FaceArray | | The list of all triangles (faces) in the mesh |   | Quick Surface Mesh (SurfaceMeshing), M3C Surface Meshing (Slice at a Time) |

## Created Arrays ##
None

## Authors ##


**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



