Find Volume Fractions of Phases {#findvolfractions}
=============

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This Filter determines the volume fraction of each **Ensemble**.  Technically, the Filter counts the number of **Elements** belonging to each **Ensemble** and stores the number fraction.  In the case of an *Image Geometry* (or any geometry with **Elements** of a single size), this will also be the volume fraction

## Parameters ##
None 

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Element | Phases | Int | (1) | Id specifying the **Ensemble** of the **Element** |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Ensemble | VolFractions | Float | (1) | Fraction of volume that belongs to each **Ensemble** |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


