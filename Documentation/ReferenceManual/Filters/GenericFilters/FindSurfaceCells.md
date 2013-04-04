Find Surface Cells {#findsurfacecells}
=============
## Group (Subgroup) ##
Generic Filters

## Description ##
This Filter determines, for each voxel/datapoint, the number of neighboring voxels/datapoints that are owned by a different feature/grain.  The algorithm for determining this is as follows: 

1) Identify the feature/grain to which a voxel/datapoint belongs
2) Identify the features/grains to which each of the current voxel's six (6) face-face neighboring voxels (front, back, left, right, up, down) belong
3) Determine the number of those neighboring voxels/datapoints belonging to a different feature/grain than the current voxel/datapoint. 
4) Repeat 1-3 for all voxels/datapoints

## Parameters ## 

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which feature/grain each voxel/datapoint belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | SurfaceVoxels | Value (int) equal to the number of neighboring voxels/datapoints of a given voxel/datapoint that belong to a different feature/grain than itself |  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



