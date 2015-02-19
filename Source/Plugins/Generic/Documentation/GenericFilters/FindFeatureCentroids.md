Find Feature Centroids {#findfeaturecentroids}
=======

## Group (Subgroup) ##
Generic Filters (Misc)

## Description ##
This filter calculates the _centroid_ of each **Feature** by determining the average x, y, and z position of all the **Cells** belonging to the **Feature**.
Note that **Features** that intersect the outer surfaces of the sample will still have _centroids_ calculated, but they will be _centroids_ of the truncated part of the **Feature** that lies inside the sample.

## Parameters ##
None

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1,1) | Specifies to which **Feature** each **Cell** belongs. |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Feature | Centroids | Float | (1,3) | X, Y, Z coordinates of **Feature** center of mass |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

