Read Oxford Instr. Ebsd Data {#readctfdata}
=====

## Group (Subgroup) ##
IOFilters (Input)


## Description ##
This filter will read a single .ctf file into the VoxelDataContainer allowing the immediate use of filters on the data instead of having to generate the intermediate .h5ebsd file. The user should be aware that simply reading the file then performing operations that are dependent on the proper crystallographic and sample reference frame will be undefined or simply **wrong**. In order to bring the crystal reference frame and sample reference frame into coincidence the proper filters need to be run. The user should read the documentation for the "[Rotate Sample Reference Frame](rotatesamplerefframe.html)" and "[Rotate Euler Reference Frame](rotateeulerrefframe.html)".


The user also may want to assign unindexed pixels to be ignored and be assigned an RGB Color of Black. In this case the user can insert the [MultiThreshold (Cell Data) Filter](multithresholdcells.html) to define the "Good Voxels" cell data.

### Default HKL Transformations ###

If the data has come from a HKL based acquisition system and the settings of the acquisition software were in the default modes then the following reference frame transformations need to be performed:

+ Sample Reference Frame: 180 Degrees about the <010> Axis
+ Crystal Reference Frame: None
 
The user also may want to assign unindexed pixels to be ignored and be assigned an RGB Color of Black. In this case the user can insert the [Single Threshold (Cell Data) Filter](singlethresholdcells.html) to define the "Good Voxels" cell data. For HKL data the "Error" column defines each point as being properly indexed (Value = 0) or an error occurred and the point was not indexed (Value > 0).

### Radians and Degrees ###

2D CTF Files have their angles in **degrees**. Please be sure to insert a filter to convert the Euler Angles from Degrees to Radians before running any other filter.

## Parameters ##

| Name             | Type |
|------------------|------|
| Input File | The Path to the .ctf file |

## Required Arrays ##

None

## Required DataContainers ##

The filter will create a new DataContainer with an ImageGeometry object.

## Created Arrays ##

Arrays are created based on the data read from the EBSD file.


| Type | Array Name | Component Dimensions | Comment |
|------|--------------------|-------------|---------|
| Float  | BC | [1]     |  |
| Float  | Euler Angle      | [3]     |    |
| Float  | BS              | [1]     |    |
| Float  | Bands    | [1] |    |
| Int    | Phases             | [1] | Numbered starting from 1 for a valid phase   |
| Float  | MAD       | [1] | Typical threshold value is < 1.8  |
| Float  | X Position       | [1] |    |
| Float  | Y Position       | [1] |    |
| int  | Error       | [1] | Value=0 is a well indexed scan point   |



## Authors ##

**Copyright** 2015 BlueQuartz Software

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users

