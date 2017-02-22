Find Feature Reference C-Axis Misalignments {#findfeaturereferencecaxismisorientations}
======

## Group (Subgroup) ##
Statistics (Crystallographic)

## Description ##
This **Filter** calculates the misorientation angle between the C-axis of each **Cell** within a **Feature** and the average C-axis for that **Feature** and stores that value for each **Cell**.  The average and standard deviation of those values for all **Cells** belonging to the same **Feature** is also stored for each **Feature**.

## Parameters ##
None

## Required Geometry ##
Image

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs |
| **Cell Attribute Array** | Quats | float | (4) | Specifies the orientation of the **Cell** in quaternion representation |
| **Cell Attribute Array**     | Phases            | int32_t | (1) | Specifies to which **Ensemble** each **Cell** belongs |
| **Feature Attribute Array** | AvgCAxes | float | (3) | The direction <u,v,w> of the **Feature's** C-axis in the sample reference frame |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | FeatureReferenceCAxisMisorientations | float | (1) | Misorientation angle (in degrees) between **Cell's** C-axis and the C-axis of the **Feature** that owns that **Cell** |
| **Feature Attribute Array** | FeatureAvgCAxisMisorientations | float | (1) | Average of the *FeatureReferenceCAxisMisorientation* values for all of the **Cells** that belong to the **Feature** |
| **Feature Attribute Array** | FeatureStdevCAxisMisorientations | float | (1) | Standard deviation of the *FeatureReferenceCAxisMisorientation* values for all of the **Cells** that belong to the **Feature** |


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


