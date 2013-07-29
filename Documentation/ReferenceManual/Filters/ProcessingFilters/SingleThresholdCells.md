Single Threshold (Cell Data) {#singlethresholdcells}
======

## Group (Subgroup) ##
Processing Filters (Threshold)

## Description ##
This filter allows the user to select a specific **Cell** array and apply a comparison operation (&lt, &gt, =) of a
 value (set by the user). The result is a new **Cell** array of Boolean values added to the pipeline. The user can set
 the name of the array that is added through the __Output Array Name__ option.
 For example if you wanted to generate a boolean array that indicated which **Cells** had an '_Image Quality_' greater
 than '_120_' then the user would select the '_Image Quality_' array from the __Input Cell Array Name__ combobox,
 set the Comparison Operator to "&gt" and set the __Value__ to 120. The user should also input a name for the created
 boolean array.


## Parameters ##

| Name | Type |
|------|------|
| Input Cell Array Name | String based on an array that is stored in the pipeline at the time the filter is run |
| Comparison Operator | &lt, &gt, = |
| Value | The value to compare each cell against. Internally stored as a double precision floating point. |
| Output Array Name | String |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | *User Chosen* |  | Array will always be present, since the user may only chose from existing arrays | N/A |



## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | User Defined | Boolean Array |


## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.





See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

