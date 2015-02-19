Find Bounding Box Features {#findboundingboxfeatures}
=============

## Group (Subgroup) ##
Generic Filters (Spatial)

## Description ##
This filter determines which **Features** are _biased_ by the outer surfaces of the sample. Larger **Features** are more likely to intersect the outer surfaces and thus it is not sufficient to only note which **Features** touch the outer surfaces of the sample.
Denoting which **Features** are biased is important so that they may be excluded from any statistical analyses. 
The algorithm for determining whether a **Feature** is _biased_ is as follows: 

1. The _centroids_ of all **Features** are calculated
2. All **Features** are tested to determine if they touch an outer surface of the sample
3. The largest box is found that does not include any of the _centroids_ of **Features** that intersect an outer surface of the sample
4. Each **Feature**'s _centroid_ is checked to see whether it lies within the box.  

*If a **Feature**'s _centroid_ lies within the box, then the **Feature** is said to be _unbiased_, and if it lies outside the box, then the **Feature** is said to be _biased_.* 

By definition of the box, no **Feature** that intersects an outer surface of the sample can be considered _unbiased_, but it should be noted that **Features** that do not intersect the outer surfaces may still be considered _biased_ 
This algorithm works to determine the biased **Features** because all **Features** have one (and only one) centroid, no matter their size. Generally, this method will deem more small **Features** biased than the set of **Features** that just intersect the outer surfaces - and this corrects for the increased likelihood that larger **Features** will touch an outer surface.

**Filter is modification of algorithm from Dave Rowenhorst (Naval Research Laboratory)

## Parameters ##
None

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##

| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Feature | Centroids | Float | (1,3) | X, Y, Z coordinates of **Feature** center of mass |
| Feature | SurfaceFeatures | Boolean | (1,1) | Flag of 1 if **Feature** touches an outer surface or of 0 if it does not |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Feature | BiasedFeatures | Boolean | (1,1) | Flag of 1 if **Feature** is biased or of 0 if it is not |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)


