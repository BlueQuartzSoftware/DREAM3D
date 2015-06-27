Rotate Euler Reference Frame {#rotateeulerrefframe}
=============

## Group (Subgroup) ##
Orientation Analysis (Conversion)

## Description ##
This **Filter** will rotate the *Euler angles* that exist at each **Element**.  In actuality, the *reference frame* is being rotated and thus the *Euler Angles* necessary to represent the same orientation must change to account for the new *reference frame*.  The user can set an *angle* and an *axis* to define the rotation of the *reference frame*.

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Rotation Axis | float (3x) | Axis that the Euler reference frame will be rotated about |
| Rotation Angle | float | Angle (in degrees) that the Euler reference frame will be rotated around the rotation axis |

## Required Geometry ##
Not Applicable

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Element Attribute Array** | EulerAngles | float | (3) | Three angles defining the orientation of the **Cell** in Bunge convention (Z-X-Z) |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


