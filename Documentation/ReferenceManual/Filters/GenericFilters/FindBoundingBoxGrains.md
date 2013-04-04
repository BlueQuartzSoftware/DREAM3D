Find Bounding Box Grains {#findboundingboxgrains}
=============

## Group (Subgroup) ##
Generic Filters (Spatial)

## Description ##
This filter determines which features/grains are _biased_ by the outer surfaces of the sample. Larger features/grains are more likely to intersect the outer surfaces and thus it is not sufficient to only note which features/grains touch the outer surfaces of the sample.
Denoting which features/grains are biased is important so that they may be excluded from any statistical analyses. 
The algorithm for determining whether a **Field** is _biased_ is as follows: 

1) The _centroids_ of all features/grains are calculated
2) All features/grains are tested to determine if they touch an outer surface of the sample
3) The largest box is found that does not include any of the _centroids_ of features/grains that intersect an outer surface of the sample
4) Each **Field**'s _centroid_ is checked to see whether it lies within the box.  

*If a **Field**'s _centroid_ lies within the box, then the **Field** is said to be _unbiased_, and if it lies outside the box, then the **Field** is said to be _biased_.* 

By definition of the box, no **Field** that intersects an outer surface of the sample can be considered _unbiased_, but it should be noted that features/grains that do not intersect the outer surfaces may still be considered _biased_ 
This algorithm works to determine the biased features/grains because all features/grains have one (and only one) centroid, no matter their size. Generally, this method will deem more small features/grains biased than the set of features/grains that just intersect the outer surfaces - and this corrects for the increased likelihood that larger features/grains will touch an outer surface.

## Parameters ##
None

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | Centroids | X, Y, Z coordinates (floats) of **Field** center of mass | Filter will calculate **Field** centroids if not previously calculated |
| Field | SurfaceFields | Boolean flag of 1 if **Field** touches an outer surface or of 0 if it does not | Filter will determine if features/grains intersect an outer surface if not already determined |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | BiasedFields | Boolean flag of 1 if **Field** is biased or of 0 if it is not 

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



