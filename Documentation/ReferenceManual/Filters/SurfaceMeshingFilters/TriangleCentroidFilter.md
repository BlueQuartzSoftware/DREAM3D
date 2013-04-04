Generate Triangle Centroids {#trianglecentroidfilter}
======

## Group/Subgroup ##
SurfaceMesh

## Description ##
This filter computes the centroid of each triangle in the surface mesh by calculating the average position of all 3 nodes that make up the triangle.


## Parameters ##
None

## Required Arrays ##

| Type | Name | Comment |
|------|------|---------|
| Triangle List | Core Property of the Surface Mesh Data Container | The List of triangles in the Surface Mesh |
| Node List | Core Property of the Surface Mesh Data Container | The shared list of nodes that make up the mesh |

## Created Arrays ##

| Type | Name | Comment |
|------|------|---------|
| double | SurfaceMeshTriangleCentroids | 3x1 Vector for each Triangle in the Mesh |


## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



