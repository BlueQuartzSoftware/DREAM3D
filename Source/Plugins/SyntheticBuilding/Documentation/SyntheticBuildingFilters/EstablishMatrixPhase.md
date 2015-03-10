Establish Matrix Phase {#establishmatrixphase}
======

## Group (Subgroup) ##
Synthetic Builder Filters (Packing)

## Description ##
This Filter inserts matrix phase(s) into a synthetic volume. The algorithm/steps for inserting matrix phase(s) is as follows:

1. Identify all the Matrix phases from the **Ensemble** *PhaseTypes* array
2. Determine the relative volume fractions of the Matrix phases (i.e. if there are two Matrix phases and they have volume fractions of *0.2* and *0.3*, respectively, then they would *0.4* and *0.6* when scaled to total *1*) 
3. Identify all **Cells** that are not currently assigned to a **Feature** (i.e. belong to **Feature** *0*).
4. For each "unassigned" **Cell**, generate a random number to decide which Matrix phase the **Cell** will be assigned to.  

Note: The filter does not actually try to match the volume fractions of the Matrix phases, but rather just uses the relative volume fractions to decide what fraction of the "unassigned"  **Cells** get assigned to each Matrix phase.  If the fraction of "unassigned"  **Cells** is smaller or larger than the total volume fractions of the Matrix phases, then the absolute volume fractions of the Matrix phases will not match the goal (at least at the end of this filter).

## Parameters ##

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Ensemble | PhaseTypes | Enumeration (int) specifying the type of phase of each Ensemble/phase (Primary=0, Precipitate=1, Transformation=2, Matrix=3, Boundary=4) |  | Intialize Synthetic Volume (SyntheticBuilding), Generate Ensemble Statistics (Statistics) |
| Ensemble | Statistics |  |  | Generate Ensemble Statistics (Statistics), StatsGenerator Application |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | Active | Boolean value specifying if the **Feature** is still in the sample (1 if the **Feature** is in the sample and 0 if it is not) | At the end of the filter, all **Features** will be "Active" as the "Inactive" **Features** will have been removed.  |
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature** |  |

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

