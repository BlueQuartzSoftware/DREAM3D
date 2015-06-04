Read DREAM3D Data File {#datacontainerreader}
====

## Group (Subgroup) ##
I/O Filters (Input)

## Description ##

The user selects the .dream3d file to be read from clicking the Select File button, which then displays the Select Input File window. After selecting the file, the selected file including the path is shown to the right of the Select File button. The data container, attribute matrices and the attribute arrays are populated in the box. The attribute matrices and the attribute arrays can be selected/deselected by the check box corresponding to that item. Only the arrays that are selected by the user are read into memory. The Overwrite Existing Data Container check box is useful if the filter is placed in the middle of a pipeline.


## Parameters ##

| Name | Type |
|------|------|
| Select File | File Path |
| Overwrite Existing DataContainers | Boolean (On or Off) |

## Required DataContainers ##
Voxel

## Required Arrays ##
None

## Created Arrays ##
| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Various | *User Chosen* |  | Array(s) will always be present in file, since the user may only chose from arrays found by the reader |



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


