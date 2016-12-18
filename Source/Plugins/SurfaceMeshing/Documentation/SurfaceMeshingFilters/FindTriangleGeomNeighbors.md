Find Feature Neighbors from Triangle Geometry {#findtrianglegeomneighbors}
=============

## Group (Subgroup) ##
Statistics (Morphological)

## Description ##
This **Filter** determines the list of neighboring (surface sharing) **Features** of each **Feature** in a **Triangle Geometry**.  The result is the number of neighbors (and the list of neighbors) of each surface meshed **Feature**.  The neighbors are determined using the following algorithm:

1. Triangulate each face of the polyhedron (in this case, each face is already a triangle within the **Triangle Geometry**)
2. For each triangular face, query the *Face Labels* array to note the **Features** on either side of the triangle face
3. For each **Feature** on either side of the triangle face, add the other **Feature** to the list of neighbors
4. Count the number of unique features in each **Features** neighbor list

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
| **Feature Attribute Array** | NumNeighbors | int32_t | (1) | Number of contiguous neighboring **Features** for a given **Feature** |
| **Feature Attribute Array**  | NeighborList | List of int32_t | (1) | List of the contiguous neighboring **Features** for a given **Feature** |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users