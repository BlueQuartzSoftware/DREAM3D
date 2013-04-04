Find Neighbors {#findneighbors}
==========

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This Filter determines, for each feature/grain, the number of other features/grains that are in contact with it.  The algorithm for determining the number of "contiguous" neighbors of each feature/grain is as follows: 

1) Identify the feature/grain to which a voxel/datapoint belongs
2) Identify the features/grains to which each of the current voxel's six (6) face-face neighboring voxels (front, back, left, right, up, down) belong
3) If a neighboring voxel/datapoint belongs to a different feature/grain than the current voxel/datapoint, then that feature/grain (owner of the neighboring voxel/datapoint) is added to the list of contiguous neighbors of the feature/grain that owns the current voxel. 
4) Repeat 1-3 for all voxels/datapoints

While performing the above steps, the number of neighboring voxel(s)/datapoint(s) with different feature/grain owner(s) than a given voxel/datapoint is stored, which identifies whether a voxel/datapoint lies on the surface of a feature/grain (i.e. the feature/grain boundary).
Additionally, the surface area shared between each set of contiguous features/grains is calculated by tracking the number of times two neighboring voxels/datapoints correspond to a contiguous feature/grain pair.
The filter also notes which features/grains touch the outer surface of the sample (this is obtianed for "free" while performing the above algorithm).

## Parameters ## 

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which feature/grain each voxel/datapoint belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Field | FieldPhases | Phase Id (int) specifying the phase of the feature/grain | Filter will determine feature/grain phases if not previously determined |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | SurfaceVoxels | Value (int) equal to the number of neighboring voxels/datapoints of a given voxel/datapoint that belong to a different feature/grain than itself.  Values will range from *0* to *6* |  |
| Field | NumNeighbors | Value (int) equal to the number of contiguous neighboring features/grains for a given feature/grain |  |
| Field | SurfaceFields | Boolean flag equal to 1 if the feature/grain touches an outer surface of the sample and equal to 0 if it does not. |  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



