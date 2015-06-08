Read DREAM3D Data File {#datacontainerreader}
====

## Group (Subgroup) ##
I/O Filters (Input)

## Description ##

The user selects the .dream3d file to be read from Select File. Only the arrays that are selected by the user are read into memory. The Overwrite Existing Data Container check box is useful if the filter is placed in the middle of a pipeline.


## Parameters ##

| Name | Type | Description |
|------|------|--------------|
| Select File | Push Button | Select .dream3d file to be read |
| Overwrite Existing DataContainers | Boolean | |

## Required Geometry ##
Not Applicable

## Required Arrays ##
None

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Any | User set | Any | Any | Array(s) will always be present in file, since the user may only chose from arrays found by the reader. |



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


