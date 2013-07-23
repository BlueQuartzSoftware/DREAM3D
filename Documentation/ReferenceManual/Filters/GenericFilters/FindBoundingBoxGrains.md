Find Bounding Box Fields {#findboundingboxgrains}
=============

## Group (Subgroup) ##
Generic Filters (Spatial)

## Description ##
This filter determines which **Fields** are _biased_ by the outer surfaces of the sample. Larger **Fields** are more likely to intersect the outer surfaces and thus it is not sufficient to only note which **Fields** touch the outer surfaces of the sample.
Denoting which **Fields** are biased is important so that they may be excluded from any statistical analyses. 
The algorithm for determining whether a **Field** is _biased_ is as follows: 

1. The _centroids_ of all **Fields** are calculated
2. All **Fields** are tested to determine if they touch an outer surface of the sample
3. The largest box is found that does not include any of the _centroids_ of **Fields** that intersect an outer surface of the sample
4. Each **Field**'s _centroid_ is checked to see whether it lies within the box.  

*If a **Field**'s _centroid_ lies within the box, then the **Field** is said to be _unbiased_, and if it lies outside the box, then the **Field** is said to be _biased_.* 

By definition of the box, no **Field** that intersects an outer surface of the sample can be considered _unbiased_, but it should be noted that **Fields** that do not intersect the outer surfaces may still be considered _biased_ 
This algorithm works to determine the biased **Fields** because all **Fields** have one (and only one) centroid, no matter their size. Generally, this method will deem more small **Fields** biased than the set of **Fields** that just intersect the outer surfaces - and this corrects for the increased likelihood that larger **Fields** will touch an outer surface.

## Parameters ##
None

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Field | Centroids | X, Y, Z coordinates (floats) of **Field** center of mass | Filter will calculate **Field** centroids if not previously calculated | Find Field Centroids (Generic) |
| Field | SurfaceFields | Boolean flag of 1 if **Field** touches an outer surface or of 0 if it does not | Filter will determine if **Fields** intersect an outer surface if not already determined | Field Surface Fields (Generic) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | BiasedFields | Boolean flag of 1 if **Field** is biased or of 0 if it is not | |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

