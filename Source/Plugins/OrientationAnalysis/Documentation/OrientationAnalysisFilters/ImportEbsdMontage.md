# Import Ebsd Montage #

## Group (Subgroup) ##

Import/Export (Montage Import)

## Description ##

This **Filter** will import a collection of EBSD data files (Edax .ang or Oxford .ctf) and roughly calculate the proper coordiates for each tile within the global reference frame. Each data file will be placed into it's own DataContainer. This filter uses the _ReadAngData_ or _ReadCtfData_ filters to import the data. The filenames need to be named with the following pattern.

	[File Prefix]r[RowIndex]c[ColIndex][FileSuffix].ang

The user can set the name of the Cell and Ensemble Attribute Matrix that will be created in each DataContainer. The name of each DataContainer is based off the file used to populate the input data for that DataContainer.

## Parameters ##

| Name | Type | Description |
|------|------|------|
| Parameter Name | Parameter Type | Description of parameter... |

## Required Geometry ##

Required Geometry Type -or- Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Data Container** | Data Container Name | N/A | N/A | Description of object... |
| **Attribute Matrix** | Attribute Matrix Name | Element/Feature/Ensemble/etc. | N/A | Description of object... |
| **Element/Feature/Ensemble/etc. Attribute Array** | AttributeArray Name | int32_t/float/etc. | (1)/(3)/etc. | Description of object... |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Data Container** | Data Container Name | N/A | N/A | Description of object... |
| **Attribute Matrix** | Attribute Matrix Name | Element/Feature/Ensemble/etc. | N/A | Description of object... |
| **Element/Feature/Ensemble/etc. Attribute Array** | AttributeArray Name | int32_t/float/etc. | (1)/(3)/etc. | Description of object... |


## Example Pipelines ##

List the names of the example pipelines where this filter is used.

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users