Generate Triangle Areas {#triangledihedralanglefilter}
======

## Group (Subgroup) ##
SurfaceMesh

## Description ##
This filter computes the minimum dihedral angle of each triangle (given by points A,B,C) in the surface mesh by calculating the angles between the three sides of the triangle and storing the minimum value for the triangle.


## Parameters ##
None

## Required DataContainers ##
SurfaceMesh - Valid Surface Mesh containing the shared vertex array and face list

## Required Arrays ##
None

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Face | SurfaceMeshFaceDihedralAngles | |


## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



