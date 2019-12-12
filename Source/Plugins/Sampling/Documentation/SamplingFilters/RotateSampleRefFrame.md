# Rotate Sample Reference Frame  #

## Group (Subgroup) ##

Sampling (Rotating/Transforming)

## Description ##

This **Filter** will rotate the *spatial reference frame* around a user defined axis, by a user defined angle.  The **Filter** will modify the (X, Y, Z) positions of each **Cell** to correctly represent where the **Cell** sits in the newly defined reference frame. For example, if a user selected a *rotation angle* of 90<sup>o</sup> and a *rotation axis* of (001), then a **Cell** sitting at (10, 0, 0) would be transformed to (0, -10, 0), since the new *reference frame* would have x'=y and y'=-x.

The equivalent rotation matrix for the above rotation would be the following:

|   |   |   |
| - | - | - |
| 0 | -1 | 0 |
| 1 | 0 | 0 |
| 0 | 0 | 1 |

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Rotation Representation | Enumeration | Which form used to represent rotation (**axis angle** or **rotation matrix)** |
| Rotation Axis (ijk) | float (3x) | Axis in sample reference frame to rotate about (if **axis angle**) |
| Rotation Angle (Degrees) | float | Magnitude of rotation (in degrees) about the rotation axis (if **axis angle**) |
| Rotation Matrix | float (3x3) | Axis in sample reference frame to rotate about (if **rotation matrix**) |

## Required Geometry ##

Image

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Attribute Matrix** | CellData | Cell | N/A | **Cell Attribute Matrix** that holds the data to rotate |

## Created Objects ##

None

## Example Pipelines ##

+ INL Export
+ Export Small IN100 ODF Data (StatsGenerator)
+ TxCopper_Exposed
+ TxCopper_Unexposed
+ Edax IPF Colors
+ Confidence Index Histogram

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)
