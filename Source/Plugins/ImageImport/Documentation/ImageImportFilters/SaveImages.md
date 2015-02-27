Save Slice Images {#saveimages}
=====

## Group (Subgroup) ##

IO (Output)


## Description ##

This filter will save images based on an array that represents RGB, Gray Scale or RGBA color values for each slice in the data set. The options are to produce XY slices along the Z axis, XZ slices along the Y axis or YZ slices along the X axis. The user has the option to save in one of 3 standard image formats (Tif, BMP, PNG). The user should select the output directory and optionally enter a prefix that will be used when creating each output file by checking the File Prefix check box and entering an Image File Prefix. The output files will be numbered sequentially starting at Zero (0) and ending at the total dimensions for the axis. For example, if the Z axis has 117 dimensions, 117 XY image files will be produced and numbered 0 to 116.

An example filter to run would be the [Generate IPF Colors](generateipfcolors.html) filter which will generate RGB values for each voxel in the volume.

Currently the "Generate IPF Colors" is the only filter that can generate the correct array.

## Parameters ##

| Name             | Type |
|------------------|------|
| Colors Array  | String |
| Image Format     | Integer |
| Plane     | Integer |
| File Prefix | Boolean |
| Image File Prefix | String |
| OutputPath   | String |


## Required Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| UInt8  | User Selected    | The user selects a Voxel Cell Array that represents RGB, Gray Scale or RGBA color values for each voxel. | The selected array should be a n component UInt8 array|


## Created Arrays ##

None



## Authors ##

**Copyright** 2013 Michael A. Groeber (AFRL), 2013 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)
