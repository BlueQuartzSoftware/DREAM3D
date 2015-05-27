Read Feature Info File {#featureinforeader}
======
## Group (Subgroup) ##
IO Filters (Input)

## Description ##
This filter reads information about the **Features** that are in the sample. The
 basic format of the file is the number of **Features** on the first line of the file followed by
 columnar formatted data listing: **Feature** Id, PhaseID, Euler Angle 1, Euler Angle 2 and Euler Angle 3.
 *If you do not have Euler Angle data for each **Feature** then substituting Zero (0.0) is fine.<br/>

__**Feature** Ids start from One (1) and Phase Ids start from One (1).
DREAM3D uses **Feature** Id Zero(0) internally which has several ramifications which the user needs to understand. Statistics
will _NOT_ be calculated for any **Cell** or feature data with a value of Zero (0). If your segmented data numbering
starts with Zero (0) then the user needs to find a way to renumber **Feature**=0 to another value. Max + 1 typically.__
For example if you have a file with 123 **Features** and 2 phases then the file would be similar to the following: 

    123
    1   2  1.2  0.4  0.8
    2   1  0.8  0.99  1.0
    .....
    121 1 0.3  0.5  0.9
    123 2 1.0  1.1  0.03

__Note the arrays that are being created__. <br>
If the option to _Create Cell Level Arrays_ is selected then any existing
 arrays will be OVER WRITTEN with new data derived from the feature data being written. In plain if the user uses this filter
 to read in Phase and Euler Angle data for their data set and the _Create Cell Level Arrays_ is __ON/True__ then
 any existing EulerAngles and Phase Arrays at the Cell level will be over written with data derived from these new Feature
 level arrays. If you have the _Create Cell Level Arrays_ set to __OFF/False__ then those cell level arrays will
 not be created.


## Parameters ##

| Name | Type |
|------|------|
| Input Feature Info File | Input File |
| Create Cell Level Arrays | Boolean (On or Off) |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding)

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | CellEulerAngles | Three (3) angles (floats) defining the orientation of the **Cell** in Bunge convention (Z-X-Z) | Only created if boolean is checked |
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** | Only created if boolean is checked |
| Feature | FeatureEulerAngles | Three (3) angles (floats) defining the orientation of each **Feature** in Bunge convention (Z-X-Z) |  |
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature** |  |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


