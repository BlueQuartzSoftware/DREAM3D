Reverse Triangle Winding Filter {#reversetrianglewinding}
======

## Group/Subgroup ##
SurfaceMesh

## Description ##
This filter reverses the winding on each triangle. If the next filter is to compute the surface normals the user will notice that the normals now point in the opposite direction. Some analysis filters require the normals to be pointing "away" from the center of a grain. The current surface meshing algorithms tend to orient the normals pointing "into" the grain.


## Parameters ##

## Required Arrays ##

| Type | Name | Comment |
|------|------|---------|
| Triangle List | Core Property of the Surface Mesh Data Container | The List of triangles in the Surface Mesh |

## Created Arrays ##
None

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



