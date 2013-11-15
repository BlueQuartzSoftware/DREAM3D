Delete Arrays {#removearrays}
=============

## Group (Subgroup) ##
Generic Filters (Memory/Management)

## Description ##
This filter allows the user to remove specified arrays from the pipeline by deleting them from memory. This can be helpful if the user has operations that need as much memory as possible and there are extra arrays that are not needed residing in memory. For example when creating a surface mesh one typically only needs an array that defines the structure of the volume and then afterwards can be safely deleted.


## Parameters ##

| Name | Type |
|------|------|
| Arrays to Delete | String(s) |


## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell/Feature/Ensemble | *User Chosen* | - | Array(s) will always be present, since the user may only chose from existing arrays | N/A |



## Created Arrays ##
None



## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

