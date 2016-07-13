Stereographic3D {#stereographic3d}
=============

## Group (Subgroup) ##

OrientationAnalysis (Conversion Filters)

## Description ##

This **Filter** will generate a set of 3D stereographic coordinates where the generated coordinates are all located within the 3D ball of unit radius.

![images/Stereographic3D_Equations.png](images/Stereographic3D_Equations.png)

**Note that if the value of &omega equals -1.0 then the filter will exit early and present an error. This is because with a value of -1 the output value would divide by zero.**


### Data Layout Notes ###

DREAM.3D orders a quaternion as ( < x, y, z >, w)> If you are importing quaternions from another source please ensure that the quaternions are in the correct order.

## Parameters ##

None

## Required Geometry ##

Not Applicable

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Element Attribute Array** | Quats | float | (4) | Specifies the orientation of the **Element** in quaternion representation |

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Attribute Array** | Coordinates | float | 3 | Converted quaternion |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users