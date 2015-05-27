Find Feature Phases {#findfeaturephases}
=============

## Group (Subgroup) ##
Generic Filters (Misc)

## Description ##
This Filter determines the phase of each **Feature** by querying the phase of the **Elements** that belong to the **Feature**.
Note that it is assumed that all **Elements** belonging to a **Feature** are of the same phase, and thus any **Element** can be used to determine the phase of the **Feature** that owns that **Element**.

## Parameters ##
None

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Element | Phases | Int | (1) | Specifies the phase of the **Element** |
| Element | FeatureIds | Int | (1) | Specifies to which **Feature** each **Element** belongs. |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | Phases | Int | (1) | Specifies the phase of the **Feature** - Values will begin at 1 as there is no phase 0, which is used temporarily in some filters for *bad* data|


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


