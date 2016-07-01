Append Image Geometry Z-Slice  {#appendimagegeometryzslice}
=====

## Group (Subgroup) ##
Sampling (Memory/Management)


## Description ##

This filter allows the user to append an Image Geometry onto the "end" of another Image Geometry. The input and 
destination **ImageGeometry** objects must have the same X&Y dimensions. Optional Checks for equal **Resolution** values 
can also be performed.

For example, if the user has an already existing **Image Geometry** that is 100 voxels in the *X* direction by 200 pixels in the 
*Y* direction and composed of 5 *Z* slices then appending another data set that is the same dimensions in X & Y but contains
10 *Z* slices then the resulting **Image Geometry** will have a total of 15 *Z* slices.


## Parameters ##

| Name             | Type |
|------------------|------|
| Check Resolution | Boolean |
| DestinationAttributeMatrix | DataArrayPath |

## Required Arrays ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Attribute Matrix** | Input Cell Data | Cell | N/A | The incoming cell data that is to be appended. |
| **Attribute Matrix** | Destination Cell Data | Cell | N/A | The destination cell data that is the final location for the appended data. |



## License & Copyright ##

Please see the description file distributed with this **Plugin**


## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)







