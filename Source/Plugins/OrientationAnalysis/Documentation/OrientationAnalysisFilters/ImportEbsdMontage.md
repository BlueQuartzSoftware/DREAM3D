# Import Ebsd Montage #

## Group (Subgroup) ##

Import/Export (Montage Import)

## Description ##

This **Filter** will import a collection of EBSD data files (Edax .ang or Oxford .ctf) and roughly calculate the proper coordinates for each tile within the global reference frame. Each data file will be placed into it's own DataContainer. This filter uses the _ReadAngData_ or _ReadCtfData_ filters to import the data. The filenames need to be named with the following pattern.

	[File Prefix]r[RowIndex]c[ColIndex][FileSuffix].ang

The user can set the name of the Cell and Ensemble Attribute Matrix that will be created in each DataContainer. The name of each DataContainer is based off the file used to populate the input data for that DataContainer.

Currently **only** EDAX .ang and Oxford Instruments .ctf files are supported.

## Parameters ##

| Name | Type | Description |
|------|------|------|
| File List Info | EbsdMontageListInfo_t | List of values that are used to generate all the input EBSD files. |
| Generate IPF Colors | Boolean | Automatically generate 001 IPF Colors for each _DataContainer_ |


## Required Geometry ##

Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|


## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Data Container** | Data Container Name based off the file name | N/A | N/A | Description of object... |
| **Cell Attribute Matrix** | Scan Data | Depends on size of 2D EBSD scan | N/A | Description of object... |
| **Ensemble Attribute Matrix** | Phase Data | 1 Tuple for each Phase | N/A | Description of object... |
| **Element Attribute Array** | Depends on OEM EBSD file being read. | int32_t/float | (1)/(3)/etc. | Description of object... |
| **Ensemble Attribute Array** |Crystal Structure, Phase Name, Lattice Constants | int32_t/float/etc. | (1)/(3)/etc. | Description of object... |


## Example Pipelines ##



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users