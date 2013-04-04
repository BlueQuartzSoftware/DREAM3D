Erode/Dilate Bad Data {#openclosebaddata}
======

## Group ##
Processing Filters

## Description ##
Bad data means a _voxel_ has a __GrainID__ of 0, which means the _voxel_ has failed some sort of test and been marked 
as a _Bad Voxel_. To remedy this situation, this Filter _Dilates_ or _Erodes_ the bad data. 
If the bad data is _dilated_, the Filter grows the bad data by one _voxel_ in an iteration sequence. The number of
 iterations to grow the _voxel_ is limited to the situation in which only one grain remains or there is no place to grow.

The __GrainID__ of any neighboring _voxel_ will be changed to 0. 
For the _erode_ option, the __GrainID__ of the bad data _voxel_ is changed to the __GrainID__ of the majority
 of its neighbors, with the requirement that at least one of the neighbors have a non-zero __GrainID__.
 If there is a tie between two __GrainID__s, then one of the __GrainID__s, chosen randomly, will be assigned to the bad _voxel_.

Goals a user might be trying to accomplish with this Filter are

- to remove small or thin regions of bad data by running a single
 (or two) iteration _erode_ operation. 
- to increase the size of a "bad" data region by running an _open operation_. This might be useful if the experimental technique 
 always underestimates the size of certain objects. 
For example, when running EBSD, the pores (which show up as
 bad data) are always smaller in the scans than in the specimen, because the
 beam, when it is just inside the pore, still can pick up signal from the
 material just beneath the pore.  

Running the _open-close_ operations in pairs can
 often change the size of some objects without affecting others.  For
 example, if there were a number of big pores and a number of single _voxel_ bad
 data regions, running a single _close_ operation would remove
 the single _voxel_ regions and reduce the pores by one _voxel_. If this is followed immediately by  an _open_
 operation, then the pores would grow by one _voxel_ and return to their original size. Because the single
_voxel_s would be gone, they would not be able to grow back.

## Parameters ## 

| Name | Type |
|------|------|
| Direction of Operation | Dilate or Erode |
| Number of Iterations | The number of iterations to use for the filter |

## Required Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|
| Cell | CellPhases | Phase Id (int) specifying the phase of the voxel/datapoint | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Cell | GrainIds | Ids (ints) that specify to which feature/grain each voxel/datapoint belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Field | FieldPhases | Phase Id (int) specifying the phase of the feature/grain | Filter will determine feature/grain phases if not previously determined |

## Created Arrays ##
None

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



