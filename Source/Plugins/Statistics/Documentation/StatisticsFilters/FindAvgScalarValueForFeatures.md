Find Average Value of Scalars For Feature {#findavgscalarvalueforfeatures}
=============

## Group (Subgroup) ##
Statistics Filters (Misc)

## Description ##
This Filter determines the average value of a scalar **Attribute Array** for each **Feature** by the following algorithm:

1. Gather all **Elements** that belong to the **Feature**
2. Average the values of the scalar **Attribute Array** for all **Elements** and store as the average for the **Feature**

## Parameters ##
None

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Element | FeatureIds | Int | (1) | Specifies to which **Feature** each **Element** belongs. |
| Element | None | Variable | (1) | Scalar **Attribute Array** to average for each **Feature** |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | ScalarAverages | Float | (1) | Average value of scalar **Attribute Array** for each **Feature** |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


