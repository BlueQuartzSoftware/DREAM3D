Generate Misorientation Colors (Face) {#generatefacemisorientationcoloring}
============

## Group (Subgroup) ##
Processing (Crystallography)

## Description ##
This **Filter** generates a 3 component vector for each **Triangle** in a **Triangle Geometry** that is the axis-angle of the misorientation associated with the **Features** that lie on either side of the **Triangle**.  The axis is normalized, so if the magnitude of the vector is viewed, it will be the *misorientation angle* in degrees.

## Parameters ##
None

## Required Geometry ##
Image + Triangle 

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Face Attribute Array** | FaceLabels | int32_t | (2) | Specifies which **Features** are on either side of each **Face** |
| **Feature Attribute Array** | AvgQuats | float | (4) | Specifies the average orientation of each **Feature** in quaternion representation |
| **Feature Attribute Array** | Phases | int32_t | (1) | Specifies to which phase each **Feature** belongs |
| **Ensemble Attribute Array** | CrystalStructures | uint32_t | (1) | Enumeration representing the crystal structure for each **Ensemble** |


## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Face Attribute Array** | MisorientationColors | float | (3) | A set of RGB color schemes encoded as floats for each **Face** |


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


