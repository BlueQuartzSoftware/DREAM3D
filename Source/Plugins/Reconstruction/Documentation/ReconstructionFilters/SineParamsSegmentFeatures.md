Segment Features (Sine Parameters) {#sineparamssegmentfeatures}
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

## Required Objects ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | *User Chosen* |  | Array will always be present, since the user may only chose from existing arrays | N/A |

## Created Objects ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | GrainIds |  |
| Feature | Active |  |




## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


