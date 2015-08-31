Find Surface Features (Mask) {#findsurfacefeaturesmask}
=============

## Group (Subgroup) ##
Generic (Spatial)

## Description ##
This **Filter** determines whether a **Feature** touches an outer surface of the sample. Any **Feature** containing an **Element** at x<sub>min</sub>, x<sub>max</sub>, y<sub>min</sub>, y<sub>max</sub>, z<sub>min</sub> or z<sub>max</sub> is considered a surface **Feature**. Any **Feature** containing an **Element** that is false in the *Mask* array or shares a face with an **Element** that is false in the *Mask* array is considered a surface **Feautre**

*Note:* If the structure/data is actually 2D, then the dimension that is planar is not considered and only the **Features** touching the edges are considered surface **Features**.

## Parameters ##
None

## Required Geometry ##
Image 

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies which **Feature** each **Element** belongs |
| **Cell Attribute Array** | Mask | bool | (1) | Specifies if the **Element** is part of the sample|

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Feature Attribute Array** | SurfaceFeatures | bool | (1) | Flag of 1 if **Feature** touches an outer surface or of 0 if it does not |

## License & Copyright ##

Please see the description file distributed with this **Plugin**


