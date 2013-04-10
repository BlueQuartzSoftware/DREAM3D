Generate Triangle Centroids {#trianglecentroidfilter}
======

## Group (Subgroup) ##
SurfaceMesh

## Description ##
This filter computes the centroid of each triangle in the surface mesh by calculating the average position of all 3 nodes that make up the triangle.


## Parameters ##
None

## Required DataContainers ##
SurfaceMesh - Valid Surface Mesh containing the shared vertex array and face list

## Required Arrays ##
None

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Face | SurfaceMeshFaceCentroids | 3x1 double Vector for each face in the Mesh |


## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



