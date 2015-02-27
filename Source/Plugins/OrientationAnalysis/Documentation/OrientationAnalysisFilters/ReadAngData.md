Read EDAX/TSL Ebsd Data {#readangdata}
=====

## Group (Subgroup) ##
IOFilters (Input)


## Description ##
This filter will read a single .ang file into the VoxelDataContainer allowing the immediate use of filters on the data instead of having to generate the intermediate .h5ebsd file. The user should be aware that simply reading the file then performing operations that are dependent on the proper crystallographic and sample reference frame will be undefined or simply **wrong**. In order to bring the crystal reference frame and sample reference frame into coincidence the proper filters need to be run. The user should read the documentation for the "[Rotate Sample Reference Frame](rotatesamplerefframe.html)" and "[Rotate Euler Reference Frame](rotateeulerrefframe.html)".

### Default TSL Transformations ###

If the data has come from a TSL based acquisition system and the settings of the acquisition software were in the default modes then the following reference frame transformations need to be performed:

+ Sample Reference Frame: 180 Degrees about the <010> Axis
+ Crystal Reference Frame: 90 Degrees about the <001> Axis

The user also may want to assign unindexed pixels to be ignored and be assigned an RGB Color of Black. In this case the user can insert the [MultiThreshold (Cell Data) Filter](multithresholdcells.html) to define the "Good Voxels" cell data.

 
The user also may want to assign unindexed pixels to be ignored and be assigned an RGB Color of Black. In this case the user can insert the [Single Threshold (Cell Data) Filter](singlethresholdcells.html) to define the "Good Voxels" cell data. For HKL data the "Error" column defines each point as being properly indexed (Value = 0) or an error occurred and the point was not indexed (Value > 0). 

## Parameters ##

| Name             | Type |
|------------------|------|
| Input File | The Path to the .ang or .ctf file |

## Required Arrays ##

None

## Required DataContainers ##

Voxel


## Created Arrays ##

Arrays are created based on the data read from the EBSD file.


| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| Int  | SomeName           | ....        | other   |



## Authors ##

**Copyright** 2015 BlueQuartz Software

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

