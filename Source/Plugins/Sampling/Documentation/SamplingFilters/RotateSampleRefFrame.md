Rotate Sample Reference Frame {#rotatesamplerefframe}
=============

## Group (Subgroup) ##
Sampling Filters (Rotating/Transforming)

## Description ##
This Filter will rotate the *spatial reference frame* around a user defined axis, by a user defined angle.  The filter will modify the (x,y,z) positions of each **Cell** to correctly represent where the **Cell** sits in the newly defined reference frame.  

*Example:* 
If a user selected a *rotation angle* of *90 degrees* and a *rotation axis* of *(001)*, then a **Cell** sitting at (10,0,0) would be transformed to (0,-10,0), since the new *reference frame* would have x'=y and y'=-x.   

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Rotation Axis | Float (x3) | Vector in sample reference frame to rotate about |
| Rotation Angle | Float | Magnitude of rotation (in degrees) about the rotation axis |

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
None

## Created Arrays ##
None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


