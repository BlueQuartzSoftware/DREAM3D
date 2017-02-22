Find Biased Features (Bounding Box) {#findboundingboxfeatures}
=============

## Group (Subgroup) ##
Generic (Spatial)

## Description ##
This **Filter** determines which **Features** are _biased_ by the outer surfaces of the sample. Larger **Features** are more likely to intersect the outer surfaces and thus it is not sufficient to only note which **Features** touch the outer surfaces of the sample. Denoting which **Features** are biased is important so that they may be excluded from any statistical analyses. The algorithm for determining whether a **Feature** is _biased_ is as follows: 

1. The _centroids_ of all **Features** are calculated
2. All **Features** are tested to determine if they touch an outer surface of the sample
3. The largest box is found that does not include any of the _centroids_ of **Features** that intersect an outer surface of the sample
4. Each **Feature**'s _centroid_ is checked to see whether it lies within the box.  

*If a **Feature**'s _centroid_ lies within the box, then the **Feature** is said to be _unbiased_, and if it lies outside the box, then the **Feature** is said to be _biased_.* 

By definition of the box, no **Feature** that intersects an outer surface of the sample can be considered _unbiased_, but it should be noted that **Features** that do not intersect the outer surfaces may still be considered _biased_. This algorithm works to determine the biased **Features** because all **Features** have one (and only one) centroid, no matter their size. Generally, this method will deem more small **Features** biased than the set of **Features** that just intersect the outer surfaces - and this corrects for the increased likelihood that larger **Features** will touch an outer surface.

*Note:* This **Filter** is a modification of an algorithm from Dave Rowenhorst (Naval Research Laboratory).

## Parameters ##
| Name             | Type | Description |
|------------------|------|-------------|
| Apply Phase by Phase | int32_t | Whether to apply the biased **Features** algorithm per **Ensemble** | 

## Required Geometry ##
Image

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Feature Attribute Array** | Centroids | float | (3) | X, Y, Z coordinates of **Feature** center of mass |
| **Feature Attribute Array** | SurfaceFeatures | bool | (1) | Flag of 1 if **Feature** touches an outer surface or of 0 if it does not |
| **Feature Attribute Array** | Phases | int32_t | (1) | Specifies to which **Ensemble** each **Feature** belongs. Only required if _Apply Phase by Phase_ is checked |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Feature Attribute Array** | BiasedFeatures | bool | (1) | Flag of 1 if **Feature** is biased or of 0 if it is not |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)



