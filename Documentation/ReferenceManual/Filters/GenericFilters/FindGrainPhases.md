Find Grain Phases {#findgrainphases}
==========   
## Group (Subgroup) ##
Generic Filters

## Description ##
This filter determines the phase of each **Field** by querying the phase of the voxels/datapoints that belong to the **Field**. 
Note that it is assumed that all voxels/datapoints belonging to a **Field** are of the same phase, and thus any **Cell** can be used to determine the phase of the **Field** that owns that **Cell**.

## Parameters ##

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |


## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | FieldPhases | Phase Id (int) specifying the phase of the **Field** |  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



