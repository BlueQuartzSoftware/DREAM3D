Generate Misorientation Colors (Face) {#generatefacemisorientationcoloring}
============

## Group (Subgroup) ##
Processing (Crystallography)

## Description ##
This Filter generates a 3 component vector for each **Triangle** in a **Triangle Geometry** that is the axis-angle of the misorientation associated with the **Features** that lie on either side of the **Triangle**.  The axis is normalized, so if the magnitude of the vector is viewed, it will be the *misorientation angle* in degrees.

## Parameters ##
None

## Required Geometry ##
Image + Triangle 

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Face | FaceLabels | Int | (2) | Specifies which **Features** are on either side of each **Face** |
| Feature | AvgQuats | Float | (4) | Specifies the average orientation of each **Feature** in quaternion representation |
| Feature | Phases | Int | (1) | Specifies to which phase each **Feature** belongs |
| Ensemble | CrystalStructures | Int | (1) | Enumeration representing the crystal structure for each phase |


## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Face | MisorientationColors | Float | (3) | A set of RGB color schemes encoded as floats for each **Face** |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


