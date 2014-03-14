Smooth Bad Data (coordination Number) {#erodedilatecoordinationnumber}
======

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This filter smoothes the interface between "good" and "bad" data. The user can specify a *coordination number* that is the number of neighboring **Cells** of opposite type (i.e. "good" or "bad") compared to a given **Cell** that is acceptable.  For example a single "bad" **Cell** surrounded by "good" **Cells** would have a *coordination number* of *6*.  The number entered by the user is actually the maximum tolerated *coordination number*.  If the user entered a value of *4*, then all "good" **Cells** with 5 or more "bad" neighbors and "bad" **Cells** with 5 or more "good" neighbors would be removed.  After **Cells** with unacceptable *coordination number* are removed, then the neighboring **Cells** are *coarsened* to fill the removed **Cells**.  

By default, the filter will only perform a single iteration and will not concern itself with the possibility that after one iteration, **Cells** that were acceptable may become unacceptable by the original *coordination number* criteria due to the small changes to the structure during the *coarsening*.  The user can opt to enable the "Loop Until Gone" parameter, which will continue to run until no **Cells** fail the original criteria.

## Parameters ##

| Name | Type |
|------|------|
| Coordination Number | Integer |
| Loop Until Gone | Boolean (On or Off) |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |


## Created Arrays ##
None



## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

