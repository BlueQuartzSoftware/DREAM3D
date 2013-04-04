Find Grain Centroids {#findgraincentroids}
=======

## Group/Subgroup ##
Generic Filters

## Description ##
This filter calculates the _centroid_ of each feature/grain by determining the average x, y, and z position of all the voxels/datapoints belonging to the feature/grain. 
Note that features/grains that intersect the outer surfaces of the sample will still have _centroids_ calculated, but they will be _centroids_ of the truncated part of the feature/grain that lies inside the sample.


## Parameters ## 

## Required Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which feature/grain each voxel/datapoint belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |

## Created Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|
| Field | Centroids | X, Y, Z coordinates (floats) of feature/grain center of mass |  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



