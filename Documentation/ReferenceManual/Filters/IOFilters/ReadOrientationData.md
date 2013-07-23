Read Orientation Data {#readorientationdata}
=====

## Group (Subgroup) ##
IOFilters (Input)


## Description ##
This filter will read a single .ang or .ctf file into the VoxelDataContainer allowing the immediate use of filters
on the data instead of having to generate the intermediate .h5ebsd file.

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

**Copyright** 2012 Michael A. Groeber (AFRL), 2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

