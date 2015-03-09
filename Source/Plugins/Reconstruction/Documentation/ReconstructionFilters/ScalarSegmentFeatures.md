Segment Features (Scalar) {#scalarsegmentfeatures}
======

## Group (Subgroup) ##
Reconstruction Filters (Segmentation)

## Description ##
This filter groups together **Cells** that differ in some user defined scalar value by less than a user defined tolerance.  For example, if the user selected array was an 8bit image array, then the array would consist of integer values between *0* and *255*.  If the user then set a tolerance vlaue of 10, then the filter would identify all sets of contiguous **Cells** that have *grayscale* values with *10* of each other.

## Parameters ##

| Name | Type |
|------|------|
| Input Cell Array Name | Unknown Type |
| Scalar Tolerance | Double |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | *User Chosen* |  | Array will always be present, since the user may only chose from existing arrays | N/A |

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | GrainIds |  |
| Feature | Active |  |


## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.





See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

