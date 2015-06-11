Write Images {#writeimages}
=====

## Group (Subgroup) ##

IO (Output)


## Description ##

This filter will save images based on an array that represents RGB, Gray Scale or RGBA color values for each slice in the data set. The options are to produce XY slices along the Z axis, XZ slices along the Y axis or YZ slices along the X axis. The user has the option to save in one of 3 standard image formats (Tif, BMP, PNG). The user should select the output directory and optionally enter a prefix that will be used when creating each output file by checking the File Prefix check box and entering an Image File Prefix. The output files will be numbered sequentially starting at Zero (0) and ending at the total dimensions for the axis. For example, if the Z axis has 117 dimensions, 117 XY image files will be produced and numbered 0 to 116.

An example filter to run would be the [Generate IPF Colors](generateipfcolors.html) filter which will generate RGB values for each voxel in the volume.

**NOTE** The saved image will be in RGBA format regardless of the initial input array. This means that Gray Scale images will be 4x the size of the original input data to the array.

## Parameters ##

| Name             | Type | Description |
|------------------|------|---------|
| Image Format     | Integer | 0=tif, 1=bmp, 2=png |
| Plane     | Integer | 0=XY, 1=XZ, 2=YZ |
| File Prefix | Boolean | True=Add the prefix, False=Do NOT add the prefix |
| Image File Prefix | String | String prefix to add to every image in addition to the slice index |
| Output Path   | File Path | Where to save all the images. If the path does not exist, DREAM3D will try to create the path |

## Required Geometry ##
Not Applicable

## Required Arrays ##

| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| UInt8  | User Selected | UInt8 | Any | Select Color Data. The user selects a Voxel Cell Array that represents RGB, Gray Scale or RGBA color values for each voxel. The selected array should be a 1, 3, or 4 component UInt8 array. |

## Created Arrays ##

None


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users

