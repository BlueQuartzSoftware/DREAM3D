Find Feature Average C-Axes {#findavgcaxes}
======

## Group (Subgroup) ##
Orientation Analysis Filters (Statistics)

## Description ##
This Filter determines the average c-axis location of each **Feature** by the following algorithm:

1. Gather all **Elements** that belong to the **Feature**
2. Determine the location of the c-axis in the sample *reference frame* for the rotated quaternions for all **Elements**.

3.  Average the locations and store as the average for the **Feature**.

Note: This filter will only work properly for *Hexagonal* materials.  The filter does not apply any symmetry operators because there is only one c-axis (<001>) in *Hexagonal* materials and thus all symmetry operators will leave the c-axis in the same position in the sample *reference frame*.  However, in *Cubic* materials, for example, the {100} family of directions are all equivalent and the <001> direction will change location in the sample *reference frame* when symmetry operators are applied. 

## Parameters ##
None

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Element | FeatureIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs. |
| Element | Quats | Float | (4) | Specifies the orientation of the **Element** in quaternion representation |

## Created Arrays ##

| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | AvgCAxes | Float | (3) | The direction <u,v,w> of the **Feature's** c-axis in the sample reference frame |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


