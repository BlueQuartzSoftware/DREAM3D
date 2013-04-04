Find Grain Phases {#findgrainphases}
==========   
## Group/Subgroup ##
Generic Filters

## Description ##
This filter determines the phase of each feature/grain by querying the phase of the voxels/datapoints that belong to the feature/grain. 
Note that it is assumed that all voxels/datapoints belonging to a feature/grain are of the same phase, and thus any voxel/datapoint can be used to determine the phase of the feature/grain that owns that voxel/datapoint.

## Parameters ## 

## Required Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|
| Cell | CellPhases | Phase Id (int) specifying the phase of the voxel/datapoint | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Cell | GrainIds | Ids (ints) that specify to which feature/grain each voxel/datapoint belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |


## Created Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|
| Field | FieldPhases | Phase Id (int) specifying the phase of the feature/grain |  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



