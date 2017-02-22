Find Feature Centroids {#findfeaturecentroids}
=============

## Group (Subgroup) ##
Generic (Misc)

## Description ##
This **Filter** calculates the _centroid_ of each **Feature** by determining the average X, Y, and Z position of all the **Cells** belonging to the **Feature**. Note that **Features** that intersect the outer surfaces of the sample will still have _centroids_ calculated, but they will be _centroids_ of the truncated part of the **Feature** that lies inside the sample.

## Parameters ##
None

## Required Geometry ##
Image

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Feature Attribute Array** | Centroids | float | (3) | X, Y, Z coordinates of **Feature** center of mass |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


