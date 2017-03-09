Find Feature Average Orientations {#findavgorientations}
======

## Group (Subgroup) ##
Statistics (Crystallographic)

## Description ##
This **Filter** determines the average orientation of each **Feature** by the following algorithm:

1. Gather all **Elements** that belong to the **Feature**
2. Using the symmetry operators of the phase of the **Feature**, rotate the quaternion of the **Feature**'s first **Element** into the *Fundamental Zone* nearest to the origin
3. Rotate each subsequent **Elements**'s quaternion (with same symmetry operators) looking for the quaternion closest to the quaternion selected in Step 2
4. Average the rotated quaternions for all **Elements** and store as the average for the **Feature**

*Note:* The process of finding the nearest quaternion in Step 3 is to account for the periodicity of orientation space, which would cause problems in the averaging if all quaternions were forced to be rotated into the same *Fundamental Zone*

*Note:* The quaternions can be averaged with a simple average because the quaternion space is not distorted like Euler space.

## Parameters ##
None

## Required Geometry ##
Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Element Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Element** belongs |
| **Element Attribute Array** | Phases | int32_t | (1) | Specifies the **Ensemble** of the **Element** |
| **Element Attribute Array** | Quats | float | (4) | Specifies the orientation of the **Element** in quaternion representation |
| **Ensemble Attribute Array** | CrystalStructures | uint32_t | (1) | Enumeration representing the crystal structure for each **Ensemble** |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Feature Attribute Array** | AvgQuats | float | (4) | Specifies the average orientation of the **Feature** in quaternion representation |
| **Feature Attribute Array** | AvgEulerAngles | float | (3) | Specifies the orientation of each **Feature** in Bunge convention (Z-X-Z) |


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


