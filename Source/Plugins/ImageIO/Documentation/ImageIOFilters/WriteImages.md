Write Images {#writeimages}
=============

## Group (Subgroup) ##
IO (Output)

## Description ##
This **Filter** will save images based on an array that represents grayscale, RGB or ARGB color values. If the input array represents a 3D volume, the **Filter** can output a series of slices along one of the orthogonal axes.  The options are to produce XY slices along the Z axis, XZ slices along the Y axis or YZ slices along the X axis. The user has the option to save in one of 3 standard image formats: TIF, BMP, or PNG. The user must define the folder to which the images are written and may define a file prefix for the saved images. The output files will be numbered sequentially starting at zero (0) and ending at the total dimensions for the chosen axis. For example, if the Z axis has 117 dimensions, 117 XY image files will be produced and numbered 0 to 116.

An example of a **Filter** that produces color data that can be used as input to this **Filter** is the [Generate IPF Colors](generateipfcolors.html) **Filter**, which will generate RGB values for each voxel in the volume.

*Note:* The saved images will be in ARGB format regardless of the initial input array. This means that grayscale images will be four times the size of the original input data to the array.

## Parameters ##
| Name             | Type | Description |
|------------------|------|---------|
| Image Format | Enumeration | Selection for tif, bmp, or png image formats |
| Plane | Enumeration | Selection for plane normal for writing the images (XY, XZ, or YZ) |
| File Prefix | bool | Whether to add a prefix to the saved images |
| Image File Prefix | String | String prefix to add to every image in addition to the slice index |
| Output Directory Path | File Path | Output directory path for the saved images |

## Required Geometry ##
Image 

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | None| uint8_t | (n) | Selected color data for output image. The data should represent grayscale, RGB or ARGB color values. The dimensionality of the array depends on the kind of image read: (1) for grayscale, (3) for RGB, and (4) for ARGB |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)

