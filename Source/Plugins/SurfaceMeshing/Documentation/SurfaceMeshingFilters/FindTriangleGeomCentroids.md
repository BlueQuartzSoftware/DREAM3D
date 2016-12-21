Find Feature Centroids from Triangle Geometry {#findtrianglegeomcentroids}
=============

## Group (Subgroup) ##
Statistics (Morphological)

## Description ##
This **Filter** determines the centroids of each **Feature** in a **Triangle Geometry**.  The centroids are determined using the following algorithm:

1. Query each triangle within the **Triangle Geometry** to determine its two owners
2. Add the 3 nodes of that triangle to the set of nodes bounding those two owners (*Note that a set will only allow each node to be entered once for a given owner*)
3. For each **Feature**, find the average (x,y,z) coordinate from the set of nodes that bound it

## Parameters ##
None

## Required Geometry ##
Triangle

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Face Attribute Array** | FaceLabels | int32_t | (2) | Specifies which **Features** are on either side of each **Face** |
| **Attribute Matrix** | FaceFeatureData | Face Feature | N/A | **Feature Attribute Matrix** of the selected _Face Labels_ |

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Feature Attribute Array**  | Centroids | float | (3) | Coordinates of the center of mass for a given enclosed **Feature** |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users