Fill Bad Data {#fillbaddata}
=============

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This Filter removes small *noise* in the data, but keeps larger regions that are possibly **Features**, e.g., pores or defects.
This Filter collects the *bad* **Cells** (*FeatureId = 0*) and _erodes_ them until none remain. However, contiguous groups of *bad* **Cells** that have at least as many **Cells** as the minimum allowed defect size enter by the user will not be _eroded_.

## Parameters ##
| Name | Type | Decision |
|------|------|------|
| Minimum Allowed Defect Size | Integer | The size at which a group of *bad* **Cells** are left unfilled as a "defect" |
| Replace Bad Data | Boolean | Tells the Filter whether to replace all data or just **FeatureId** |
| Store Defects As New Phase | Boolean | Tells the Filter whether to change the phase of "defects" larger than the minimum allowed size above |

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs. |
| Cell | Phases | Int | (1) | Specifies the phase each **Cell** - is only required if "Store Defects As New Phase" parameter is *true* |

## Created Arrays ##
None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


