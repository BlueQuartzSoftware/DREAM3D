Write Ph File (Feature Ids) {#phwriter}
======

## Group (Subgroup) ##
I/O Filters (Output)


## Description ##
This Filter writes to a data file in a format used by Carnegie Mellon University.  See the [PhReader](PhReader.html "") filter description for example data format. Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. 

### Example Output ###
**Example File (Partial)**

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
| Output File | Select Push Button | Create a file with extension .ph |

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Ids (int32) that specify to which **Feature** each **Cell** belongs. | (1) | Filters Known to Create Data: Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding)

## Created Arrays ##
None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


