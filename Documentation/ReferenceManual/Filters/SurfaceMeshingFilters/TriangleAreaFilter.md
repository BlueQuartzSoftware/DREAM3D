Generate Triangle Areas {#triangleareafilter}
======

## Group (Subgroup) ##
SurfaceMesh

## Description ##
This filter computes the area of each triangle (given by points A,B,C) in the surface mesh by calculating the following: 1/2*|AB||AC|sin(O), where O is the angle between |AB| and |AC|.


## Parameters ##
None

## Required DataContainers ##
SurfaceMesh - Valid Surface Mesh containing the shared vertex array and face list

## Required Arrays ##
None

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Face | SurfaceMeshFaceAreas | |


## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

