Remove Arrays {#removearrays}
=============

## Group (Subgroup) ##
Generic Filters

## Description ##
This filter allows the user to remove specified arrays from the pipeline by deleting them from memory. This can be helpful
if the user has operations that need as much memory as possible and there are extra arrays that are not needed residing
in memory. For example when creating a surface mesh one typically only needs an array that defines the structure of the
volume and then afterwards can be safely deleted.


## Parameters ## 

| Name | Type |
|------|------|
| Arrays to Delete | Unknown Type |

## Required Arrays ##
None



## Created Arrays ##
None



## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.


