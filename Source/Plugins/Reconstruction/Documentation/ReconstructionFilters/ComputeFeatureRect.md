ComputeFeatureRect {#computefeaturerect}
=============

## Group (Subgroup) ##
Reconstruction (Reconstruction)

## Description ##
This **Filter** computes the XYZ minimum and maximum coordinates for each **Feature** in a segmentation. This data can be important for finding the smallest ecompasing volume. This values are given in **Pixel** coordinates.

## Parameters ##
| Name | Type | Description |
|------|------|------|


## Required Geometry ##

ImageGeometry

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **FeatureIds** | FeatureIdsArrayName | int32_t | (1) | |

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Data Container** | Data Container Name | N/A | N/A | Description of object... |
| **Attribute Matrix** | Attribute Matrix Name | Element/Feature/Ensemble/etc. | N/A | Description of object... |
| **Feature Attribute Array** | FeatureRect Array Name | uint32 | (6) | Xmin, Ymin, Zmin, Xmax, Ymax, Zmax |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users