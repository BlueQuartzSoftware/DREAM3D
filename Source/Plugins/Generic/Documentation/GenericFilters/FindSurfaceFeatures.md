Find Surface Features {#findsurfacefeatures}
=============

## Group (Subgroup) ##
Generic Filters (Spatial)

## Description ##
This filter determines whether a **Feature** touches an outer surface of the sample. This is accomplished by simply querying the **Feature** owners of the **Cells** that sit at either xMin, xMax, yMin, yMax, zMin or zMax.
Any **Feature** that owns one of those **Cells** is said to touch an outer surface and all other **Features** are said to not touch an outer surface of the sample.

NOTE: If the structure/data is actually 2D, then the dimension that is planar is not considered and only the **Features** touching the edges are considered surface **Features**.

## Parameters ##
None

## Required Geometry ##
Image/Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1,1) | Specifies to which **Feature** each **Cell** belongs. |

## Created Arrays ##

| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Feature | SurfaceFeatures | Boolean | (1,1) | Flag equal to 1 if the **Feature** touches an outer surface of the sample and equal to 0 if it does not. |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

