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
| Read Field Data | Boolean (On or Off) |
| Read Ensemble Data | Boolean (On or Off) |

## Required DataContainers ##
Voxel

## Required Arrays ##
None

## Created Arrays ##
| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell/Field/Ensemble | *User Chosen* |  | Array(s) will always be present in file, since the user may only chose from arrays found by the reader |


## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



