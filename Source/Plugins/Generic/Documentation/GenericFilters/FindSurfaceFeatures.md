Find Surface Features {#findsurfacefeatures}
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

## Parameters ##
None

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Feature | BiasedFeatures | Boolean flag of 1 if **Feature** is biased or of 0 if it is not |  | Find Biased Features (Generic) |
| Feature | SurfaceFeatures | Boolean flag of 1 if **Feature** touches an outer surface or of 0 if it does not | Filter will determine if **Features** intersect an outer surface if not already determined | Feature Surface Features (Generic) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature |  |  | |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

