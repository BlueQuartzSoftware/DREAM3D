Shared Field Face Filter {#sharedgrainfacefilter}
======

## Group (Subgroup) ##
SurfaceMesh

## Description ##
This filter generates all the unique **Field** id to **Field** id values. For example if **Field** Id = 4 had 3 neighbors with values of 6, 8 and 9 then at least 3 unique values would be generated for the pairs (4,6), (4, 8) and (4,9). This value is stored in the array at the same index as the triangle is stored in the triangle array.


## Parameters ##


## Required DataContainers ##

SurfaceMesh - Valid Surface Mesh containing the shared vertex array and face list

## Required Arrays ##
None

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Face | SurfaceMeshGrainFaceId | The length of the array is the number of triangles for that unique face id value |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

