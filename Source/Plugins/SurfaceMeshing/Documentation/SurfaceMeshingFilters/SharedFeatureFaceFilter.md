Shared Feature Face Filter {#sharedfeaturefacefilter}
======

## Group (Subgroup) ##
SurfaceMesh

## Description ##
This filter generates all the unique **Feature** id to **Feature** id values. For example if **Feature** Id = 4 had 3 neighbors with values of 6, 8 and 9 then at least 3 unique values would be generated for the pairs (4,6), (4, 8) and (4,9). This value is stored in the array at the same index as the triangle is stored in the triangle array.


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

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


