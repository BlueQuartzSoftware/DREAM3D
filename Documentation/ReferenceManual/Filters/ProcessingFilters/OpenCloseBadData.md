Erode/Dilate Bad Data {#openclosebaddata}
======

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
Bad data means a **Cell** has a _GrainId_ of *0*, which means the **Cell** has failed some sort of test and been marked 
as a _Bad **Cell**_. To remedy this situation, this filter _Dilates_ or _Erodes_ the bad data. 
If the bad data is _dilated_, the filter grows the bad data by one **Cell** in an iteration sequence. The number of
 iterations to grow the **Cell** is limited to the situation in which only one **Field** remains or there is no place to grow.

The _GrainId_ of any neighboring **Cell** will be changed to *0*. 
For the _erode_ option, the _GrainId_ of the bad data **Cell** is changed to the _GrainId_ of the majority
 of its neighbors, with the requirement that at least one of the neighbors have a non-zero _GrainId_.
 If there is a tie between two _GrainIds_, then one of the *GrainIds*, chosen randomly, will be assigned to the bad **Cell**.

Goals a user might be trying to accomplish with this Filter are

- to remove small or thin regions of bad data by running a single
 (or two) iteration _erode_ operation. 
- to increase the size of a "bad" data region by running an _open operation_. This might be useful if the experimental technique 
 always underestimates the size of certain objects. 
For example, when running EBSD, the pores (which show up as
 bad data) are generally smaller in the scans than in the specimen, because the
 beam, when it is just inside the pore, still can pick up signal from the
 material just beneath the pore.  

Running the _erode-dilate_ operations in pairs can
 often change the size of some objects without affecting others.  For
 example, if there were a number of big pores and a number of single **Cell** bad
 data regions, running a single _close_ operation would remove
 the single **Cell** regions and reduce the pores by one **Cell**. If this is followed immediately by  an _open_
 operation, then the pores would grow by one **Cell** and return to near their original size. Because the single
**Cells** would be gone, they would not be able to grow back.

## Parameters ##

| Name | Type |
|------|------|
| Direction of Operation | Dilate or Erode |
| Number of Iterations | The number of iterations to use for the filter |

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Field | FieldPhases | Phase Id (int) specifying the phase of the **Field** | Filter will determine **Field** phases if not previously determined |

## Created Arrays ##
None

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



