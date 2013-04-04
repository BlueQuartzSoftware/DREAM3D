Minimum Size Filter (All Phases) {#minsize}
======

## Group (Subgroup) ##
Processing Filters

## Description ##
This filter removes features/grains that have a total number of voxels/datapoints below the minimum threshold defined by the user. If the user chooses a
value larger than the largest feature/grain, then all features/grains will be eliminated and the filter will become stuck in an inifinite loop trying to 
fill the eliminated features/grains. 
*The number of voxels/datapoints in the largest grain is determined during the running of this filter. Hence, a choice of threshold should be carefully be choosen
if it is not known how many voxels/datapoints are in the largest features/grains.
After removing all the small features/grains, the remaining features/grains are isotropically coarsened to fill the gaps left by the small features/grains.

## Parameters ## 

| Name | Type | Description |
|------|------|-------------|
| Minimum Allowed Grain Size | Integer | Number of voxels/datapoints that must be present in the feature/grain for it to remain in the sample |

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | CellPhases | Phase Id (int) specifying the phase of the voxel/datapoint | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Cell | GrainIds | Ids (ints) that specify to which feature/grain each voxel/datapoint belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Field | FieldPhases | Phase Id (int) specifying the phase of the feature/grain | Filter will determine feature/grain phases if not previously determined |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | Active | Boolean value specifying if the feature/grain is still in the sample (1 if the feature/grain is in the sample and 0 if it is not) | At the end of the filter, all features/grains will be "Active" as the "Inactive" features/grains will have been removed.  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



