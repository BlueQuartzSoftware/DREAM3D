Save Slice Images {#saveimages}
=====

## Group (Subgroup) ##
IO (Output)


## Description ##

This filter will save images based on an array that represents RGB color values for each XY slice in the data set. The user has the option to save in one of 3 standard image formats (Tif, BMP, PNG). The user should select the output directory and optionally enter a prefix that will be used when creating each output file. The output files will be numbered sequentially starting at Zero (0).

An example filter to run would be the [Generate IPF Colors](generateipfcolors.html) filter which will generate RGB values for each voxel in the volume.

## Parameters ##
| Name             | Type |
|------------------|------|
| RGB Colors Array  | String |
| Image Format     | Integer |
| Image Prefix | String |
| OutputPath   | String |


## Required Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| UInt8  | User Selected    | The user selects a Voxel Cell Array that represents RGB Values for each voxel. ||


## Created Arrays ##

None



## Authors ##

**Copyright** 2013 Michael A. Groeber (AFRL), 2013 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)
