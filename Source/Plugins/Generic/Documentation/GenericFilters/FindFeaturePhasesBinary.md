Find Feature Phases Binary {#findfeaturephasesbinary}
=============

## Group (Subgroup) ##
Generic Filters (Misc)

## Description ##
This Filter assigns a phase number to binary data. The *true* **Cells** will be phase 1, and *false* **Cells** will be phase 0. It is generally for use when the **Cell** phases were not known ahead of time. For example, if an image is segmented into precipitates and non-precipitates, this Filter will assign the precipitates to phase 1, and the non-precipitates to phase 0.

Additionally, this Filter creates an **Ensemble Attribute Matrix** to hold ensemble data. 

## Parameters ##
None

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | GoodVoxels | Boolean | (1) | Specifies if the **Cell** is to be counted as a phase or not |
| Cell | FeatureIds | Int | (1) | Ids that specify to which **Feature** each **Cell** belongs. |


## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | Phases | Int | (1) | Id specifying the phase of the **Feature** |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


