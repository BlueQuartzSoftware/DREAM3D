Input Crystal Compliance Values {#inputcrystalcompliances}
=============

## Group (Subgroup) ##
Generic (Crystallography)

## Description ##
This **Filter** stores the _Crystal Compliance Matrix_ for each **Ensemble**, given the single crystal properties for the **Ensemble**. Currently the specified values will be used for every **Ensemble** present.

The symmetry drop down menu is for convenience only (the full 6x6 matrix will be saved for each phase regardless of crystal structure).

## Parameters ##
| Name             | Type | Description |
|------------------|------|---------|
| Compliance Values | float | 21 single crystal compliance values (s<sub>11</sub>, s<sub>12</sub>, s<sub>13</sub>, ... , s<sub>55</sub>, s<sub>56</sub>, s<sub>66</sub>) in units of 10<sup>-11</sup> Pa<sup>-1</sup> |

## Required Geometry ##
Not Applicable

## Required Objects ##
None

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Ensemble Attribute Array** | CrystalCompliances | float | (6,6) | 6x6 matrix that specifies the compliance of the crystal (in the crystal reference frame) |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


