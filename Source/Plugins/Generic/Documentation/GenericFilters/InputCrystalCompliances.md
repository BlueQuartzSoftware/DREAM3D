Input Crystal Compliance Values {#inputcrystalcompliances}
=============

## Group (Subgroup) ##
Generic Filters (Crystallography)

## Description ##
This Filter stores the _Crystal Compliance Matrix_ for each **Ensemble**, given the single crystal properties for the **Ensemble**. Currently the specified values will be used for every **Ensemble** present.

The symmetry drop down menu is for convince only (the full 6x6 matrix will be saved for each phase regardless of crystal structure).

## Parameters ##
| Name             | Type | Description |
|------------------|------|---------|
| Compliance Values | Float | 21 single crystal compliance values (s11, s12, s13, ... , s55, s56, s66) in units of 10<sup>-11</sup> Pa<sup>-1</sup> |

## Required Geometry ##
Not Applicable

## Required Arrays ##
None

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Ensemble | CrystalCompliances | Float | (6,6) | 6x6 matrix that specify compliance of the crystal (in the crystal reference frame) |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


