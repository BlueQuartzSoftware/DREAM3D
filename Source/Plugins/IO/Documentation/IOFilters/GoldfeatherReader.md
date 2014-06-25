Goldfeather Reader {#goldfeatherreader}
======
[TOC]

## Group (Subgroup) ##
IO Filters (Input)

## Description ##

This class reads a surface mesh from a .jg file as supplied by Dr. Goldfeather in his paper on curvature estimation. In
addition to the mesh is the data for the Principal Curvatures and Principal Curvature Directions.


## Parameters ##

| Name | Type |
|------|------|
| Input File | File Path |

## Required DataContainers ##
Voxel

## Required Arrays ##
None



## Created Arrays ##

| Type       | Name | Comment |
|------------|------|---------|
| Vert Array | Nodes     | SurfaceMeshDataContainer Property of the Shared Vertex List |
| Face Array | Triangles | SurfaceMeshDataContainer Property of the list of Triangles |
| Double | Node Normals | The Normal at each Vertex |
| Double | Principal Direction 1 | N x 3 Col Double Array |
| Double | Principal Direction 2 | N x 3 Col Double Array |
| Double | Principal Curvature 2 | N x 1 Col Double Array |
| Double | Principal Curvature 2 | N x 1 Col Double Array |
| Int32 | SurfaceMeshTriangleLabels | Triangle Feature Labels |
| Double | SurfaceMeshTriangleNormals | Triangle Normals |



## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

