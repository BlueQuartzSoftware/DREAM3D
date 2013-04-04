Fill Bad Data {#fillbaddata}
======

## Group (Subgroup) ##
Processing Filters

## Description ##
This Filter removes small noise in data, but keeps larger regions that are possibly features, e.g., pores or defects.
This Filter collects the _bad voxels_ (__GrainId__ = 0) and sets the _close_ operation, as in the _open/close_ filter [_see below_], to be run on those _bad voxels_ until none remain. 
However, those _bad voxels_ that satisfy the following are not changed:
The _voxels_ that are contiguous to any group of _bad voxels_ are tested to determine if they have as many as or more _voxels_ than the minimum defect size (_voxels_). If true, then the _bad voxels_ remain unchanged when run in the _close_ operations.
-----------------
 _In the Open/Close Filter, for the _close_ option, the __GrainID__ of the bad _voxel_ is changed to the __GrainID__ of the majority of its neighbors, with the requirement that at least one of the neighbors have a non-zero __GrainID___.

## Parameters ## 

| Name | Type |
|------|------|
| Minimum Allowed Defect Size | Integer |

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which feature/grain each voxel/datapoint belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Cell | CellPhases | Phase Id (int) specifying the phase of the voxel/datapoint | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Field | FieldPhases | Phase Id (int) specifying the phase of the feature/grain | Filter will determine feature/grain phases if not previously determined |

## Created Arrays ##
None

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



