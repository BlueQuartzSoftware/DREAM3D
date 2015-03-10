Read DREAM3D Data File {#datacontainerreader}
====

## Group (Subgroup) ##
I/O Filters (Input)

## Description ##

The user is able to select which arrays from the DREAM3D data file to read. There are 3 types of arrays for each of the 3 types of DataContainer Objects in the file. Only the arrays that are selected by the user are read into memory.

## Parameters ##

| Name | Type |
|------|------|
| Input File | File Path |
| Read Cell Data | Boolean (On or Off) |
| Read Feature Data | Boolean (On or Off) |
| Read Ensemble Data | Boolean (On or Off) |

## Required DataContainers ##
Voxel

## Required Arrays ##
None

## Created Arrays ##
| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell/Feature/Ensemble | *User Chosen* |  | Array(s) will always be present in file, since the user may only chose from arrays found by the reader |


## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

