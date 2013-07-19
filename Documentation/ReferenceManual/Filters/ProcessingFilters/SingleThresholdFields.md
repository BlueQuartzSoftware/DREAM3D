Single Threshold (Field Data) {#singlethresholdfields}
======

## Group (Subgroup) ##
Processing Filters (Threshold)

## Description ##
This filter allows the user to select a specific **Field** array and apply a comparison operation (&lt, &gt, =) of a
 value (set by the user). The result is a new **Field** array of Boolean values added to the pipeline. The user can set
 the name of the array that is added through the __Output Array Name__ option.
 For example if you wanted to generate a boolean array that indicated which **Fields** had an '_Equivalent Diameter_' greater
 than '_10_' then the user would select the '_Equivalent Diameters_' array from the __Input Field Array Name__ combobox,
 set the Comparison Operator to "&gt" and set the __Value__ to 10. The user should also input a name for the created
 boolean array.


## Parameters ##

| Name | Type |
|------|------|
| Input Field Array Name | Unknown Type |
| Comparison Operator | Choices |
| Value | Double |
| Output Array Name | Unknown Type |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Field | *User Chosen* |  | Array will always be present, since the user may only chose from existing arrays | N/A |

## Created Arrays ##

| Type | Name | Comment |
|------|------|---------|
| Field | User Defined | Boolean Array |



## Authors ##

Copyright [INSERT YOUR NAME HERE]
Contact Info:[INSERT EMAIL ADDRESS HERE]
Version: 1.0.0
License: See the License.txt file that came with DREAM3D.



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

