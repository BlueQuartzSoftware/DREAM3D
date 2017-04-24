Segment Features (Sine Parameters) {#sineparamssegmentfeatures}
======

## Group (Subgroup) ##
Reconstruction Filters (Segmentation)

## Description ##
This filter groups together **Cells** that differ in some user defined scalar value by less than a user defined tolerance.  For example, if the user selected array was an 8bit image array, then the array would consist of integer values between *0* and *255*.  If the user then set a tolerance value of 10, then the filter would identify all sets of contiguous **Cells** that have *grayscale* values with *10* of each other.

## Parameters ##

| Name | Type |
|------|------|
| Use Good Voxels Array | Bool |

## Required DataContainers ##
Voxel

## Required Objects ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | *User Chosen* |  | Array will always be present, since the user may only chose from existing arrays | N/A |
| Cell | GoodVoxels Array Path | | | |

## Created Objects ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Feature | FeatureIds | |
| Cell | Cell Feature Attribute Matrix Name | |
| Feature | Active | |




## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


