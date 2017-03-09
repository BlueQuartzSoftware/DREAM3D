Compute FeatureRect {#computefeaturerect}
=============

## Group (Subgroup) ##
Reconstruction (Reconstruction)

## Description ##
This **Filter** computes the XYZ minimum and maximum coordinates for each **Feature** in a segmentation. This data can be important for finding the smallest ecompasing volume. This values are given in **Pixel** coordinates.

|       | 0 | 1 | 2 | 3 | 4 |
|-------|---|---|---|---|---|
| **0** | 0 | 0 | 1 | 0 | 0 |
| **1** | 0 | 0 | 1 | 1 | 0 |
| **2** | 0 | 1 | 1 | 1 | 1 |
| **3** | 0 | 0 | 1 | 1 | 0 |
| **4** | 0 | 0 | 0 | 0 | 0 |


If the example matrix above which represents a single feature where the feature ID = 1, the output of the filter would be:

    X Min = 1
    Y Min = 0
    Z Min = 0

    X Max = 4
    Y Max = 3
    Z Max = 0


## Parameters ##

N/A

## Required Geometry ##

ImageGeometry

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **FeatureIds** | FeatureIdsArrayName | int32_t | (1) | |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Feature Attribute Array** | FeatureRect | uint32 | (6) | Xmin, Ymin, Zmin, Xmax, Ymax, Zmax |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users