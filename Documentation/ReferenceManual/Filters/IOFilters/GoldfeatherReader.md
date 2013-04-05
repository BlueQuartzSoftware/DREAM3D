Goldfeather Reader {#goldfeatherreader}
======

## Group (Subgroup) ##
IO Filters (Input)

## Description ##

This class reads a surface mesh from a .jg file as supplied by Dr. Goldfeather in his paper on curvature estimation. In
addition to the mesh is the data for the Principal Curvatures and Principal Curvature Directions.


## Parameters ##

| Name | Type |
|------|------|
| Input File | File Path |

## Required Arrays ##
None



## Created Arrays ##

| Type | Name | Comment |
|------|------|---------|
|  | Nodes | SurfaceMeshDataContainer Property |
|  | Triangles | SurfaceMeshDataContainer Property |
| char | Node Type | The Type of Nodes. One value for each Node created. |
| Float | Principal Direction 1 |  |
| Float | Principal Direction 2 |  |
| Float | Principal Curvature 2 |  |
| Float | Principal Curvature 2 |  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



