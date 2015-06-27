Convert Orientation Representations {#convertorientations}
=======

## Group (Subgroup) ##
Orientation Analysis (Conversion)

## Description ##

This **Filter** generates a new orientation representation for each **Element**, given the _Input Orientation Representation_ for the **Element**. The following table lists the various orientation representations that are supported. DREAM3D is capable of converting between any representation with some caveats.

The valid range for Euler angles is:
    + phi-1: 0 to 360
    + Phi : 0 to 180
    + phi-2: 0 to 360

If the angles fall outside of this range the **original** Euler Input data **WILL BE CHANGED** to ensure they are within this range.


| Orientation Representation | No. of Components | Orientation Type Index |
|----------------------------|------------------|-------------------------|
| EulerAngles               | 3      | 0 |
| OrientationMatrix | 9 | 1 |
| Quaternions                | 4            | 2 |
| AxisAngle           | 4       | 3 |
| Rodrigues Vectors          | 4 | 4 |
| Homochoric                 | 3 | 5 |


## Parameters ##

| Name             | Type | Description |
|------------------|------|-------------|
| Input Orientation Type | Enumeration | Specifies the incoming orientation representation |
| Output Orientation Type | Enumeration | Specifies to which orientation representation to convert the incoming data  |

## Required Geometry ##

Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| Any **Attribute Array** | None    | float | See above table | Incoming orientation representation |


## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-------------------|
| Any **Attribute Array**     | None            | float | See above table | Converted orientation representation          |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users

