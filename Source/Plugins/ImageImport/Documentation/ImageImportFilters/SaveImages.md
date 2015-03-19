Save Slice Images {#saveimages}
=====

## Group (Subgroup) ##

IO (Output)


## Description ##

This filter will save images based on an array that represents RGB, Gray Scale or RGBA color values for each slice in the data set. The options are to produce XY slices along the Z axis, XZ slices along the Y axis or YZ slices along the X axis. The user has the option to save in one of 3 standard image formats (Tif, BMP, PNG). The user should select the output directory and optionally enter a prefix that will be used when creating each output file by checking the File Prefix check box and entering an Image File Prefix. The output files will be numbered sequentially starting at Zero (0) and ending at the total dimensions for the axis. For example, if the Z axis has 117 dimensions, 117 XY image files will be produced and numbered 0 to 116.

An example filter to run would be the [Generate IPF Colors](generateipfcolors.html) filter which will generate RGB values for each voxel in the volume.


## Parameters ##

| Name             | Type | Comment |
|------------------|------|---------|
| Colors Array  | String | UInt8 array representing Grayscale, RGB  or RGBA Arrays |
| Image Format     | Integer | 0=Tif, 1=bmp, 2=png |
| Plane     | Integer | 0=XY, 1=XZ, 2=YZ |
| File Prefix | Boolean | True=Add the prefix, False=Do NOT add the prefix |
| Image File Prefix | String | String prefix to add to every image in addition to the slice index |
| OutputPath   | String | Where to save all the images. If the path does not exist, DREAM3D will try to create the path |


## Required Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| UInt8  | User Selected    | The user selects a Voxel Cell Array that represents RGB, Gray Scale or RGBA color values for each voxel. | The selected array should be a 1, 3, or 4 component UInt8 array |


## Created Arrays ##

None



## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)
