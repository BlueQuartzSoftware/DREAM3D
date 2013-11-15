Find Feature Phases {#findfeaturephases}
==========
## Group (Subgroup) ##
Generic Filters (Misc)

## Description ##
This filter determines the phase of each **Feature** by querying the phase of the **Cells** that belong to the **Feature**.
Note that it is assumed that all **Cells** belonging to a **Feature** are of the same phase, and thus any **Cell** can be used to determine the phase of the **Feature** that owns that **Cell**.

## Parameters ##
None


## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |


## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature** | Values will begin at 1 as there is no phase 0, which is used temporarily in some filters for bad data|

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

