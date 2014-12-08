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

## Authors ##


**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

