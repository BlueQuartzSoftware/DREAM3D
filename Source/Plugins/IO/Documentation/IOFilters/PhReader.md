Read Ph File (Feature Ids) {#phreader}
=============

## Group (Subgroup) ##
IO (Input)

## Description ##
This Filter reads a custom data file used by the Materials Science Department at Carnegie Mellon University. The information in the file defines an **Image** geometry with a set of **Feature** Ids.  The file format is a simple ASCII text file with the following contents:

**Format**

    Line 1:
    {5 Spaces}{x dimension}{5 Spaces}{y dimension}{5 Spaces}{z dimension}
    Line 2:
    'DREAM3'              52.00  1.000  1.0       {Number of Features}
    Line 3: (Without the quotes)
    " 0.000 0.000 0.000          0        "
    {Each Feature Id on a single line}

## Example Input ##

    11     11     11
    'DREAM3'              52.00  1.000  1.0       6
    0.000 0.000 0.000          0        
    830
    830
    0
    977
    977
    830
    830
    …..

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Input File | File Path | Input .ph file path |
| Origin | Float (x3) | The location in space of the (0, 0, 0) voxel |
| Resolution | Float (x3) | Vector of resolution values (dx, dy, dz) |
| Data Container | String | Created **Data Container** name |
| Cell Attribute Matrix | String | Created **Cell Attribute Matrix** name |

## Required Geometry ##
Not Applicable


## Required Arrays ##
None

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


