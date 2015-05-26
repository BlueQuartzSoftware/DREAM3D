Rotate Euler Reference Frame {#rotateeulerrefframe}
=============

## Group (Subgroup) ##
OrientationAnalysis Filters (Conversion)

## Description ##
This Filter will rotate the *Euler Angles* that exist at each **Element**.  In actuality, the *reference frame* is being rotated and thus the *Euler Angles* necessary to represent the same orientation must change to account for the new *reference frame*.  The user can set an *angle* and an *axis* to define the rotation of the *reference frame*.

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Rotation Axis | Float (x 3) | Axis that the Euler reference frame will be rotated about |
| Rotation Angle | Float | Angle (in degrees) that the Euler reference frame will be rotated around the rotation axis |

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Element | EulerAngles | Float | (3) | Euler angles defining the orientation of the **Element** in Bunge convention (Z-X-Z) |

## Created Arrays ##
None

## Authors ##
**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


