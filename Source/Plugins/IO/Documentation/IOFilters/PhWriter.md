Write Ph File (Feature Ids) {#phwriter}
=============

## Group (Subgroup) ##
IO (Output)

## Description ##
This Filter writes a custom data file used by the Materials Science Department at Carnegie Mellon University.  See the [PhReader](PhReader.html "") Filter description for example data format.

### Example Output ###

         189     201     117
    'DREAM3'              52.00  1.000  1.0       2353
    0.000 0.000 0.000          0        
    0
    0
    0
    0
    0
    0
    0
    0
    0
    0
    0
    0
    0
    0
    0
    0
    0
    1742
    0
    …..

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Output File | File Path | Output .ph file path |

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs |

## Created Arrays ##
None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


