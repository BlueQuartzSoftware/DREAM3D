Rotate Sample Reference Frame {#rotatesamplerefframe}
=============

## Group (Subgroup) ##
Sampling (Rotating/Transforming)

## Description ##
This **Filter** will rotate the *spatial reference frame* around a user defined axis, by a user defined angle.  The **Filter** will modify the (X, Y, Z) positions of each **Cell** to correctly represent where the **Cell** sits in the newly defined reference frame. For example, if a user selected a *rotation angle* of 90<sup>o</sup> and a *rotation axis* of (001), then a **Cell** sitting at (10, 0, 0) would be transformed to (0, -10, 0), since the new *reference frame* would have x'=y and y'=-x.   

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Rotation Axis (ijk) | float (3x) | Axis in sample reference frame to rotate about |
| Rotation Angle (Degrees) | float | Magnitude of rotation (in degrees) about the rotation axis |

## Required Geometry ##
Image

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Attribute Matrix** | CellData | Cell | N/A | **Cell Attribute Matrix** that holds the data to rotate |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


