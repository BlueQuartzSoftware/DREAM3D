Neighbor CI Correlation {#neighborcicorrelation}
======

## Group (Subgroup) ##
Processing (Cleanup)

## Description ##
This **Filter** first identifies all **Cells** that have a *confidence index* below the minimum set by the user.  Then, for each of those **Cells**, their neighboring **Cells** are checked to determine the neighbor **Cell** with maximum *confidence index*.  The attributes of the neighbor with the maximum *confidence index* are then reassigned to the reference **Cell**.

*Note:* By default, the **Filter** will run only one iteration of the cleanup.  If the user selects the *Loop Until Gone* option, then the **Filter** will run iteratively until no **Cells** exist with a *confidence index* below the user defined value. So, if a **Cell** is below the tolerance and so are all of its neighbors, then that **Cell** will not be changed during that iteration and will remain unchanged until one of its neighbors gets changed by a **Cell** further away.  

## Parameters ##
| Name | Type | Description |
|------|------|-------------|
| Minimum Confidence Index | float | Sets the minimum value of 'confidence' a **Cell** must have |
| Loop Until Gone | bool | Whether to iterate until all **Cells** are above the minimum tolerance or just run one iteration |

## Required Geometry ##
Image

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Cell Attribute Array** | Confidence Index | float | (1) | Specifies the confidence in the orientation of the **Cell** (TSL data) |

## Created Objects ##
None


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


